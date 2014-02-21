/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2014  Phisys AG, Switzerland
#    Copyright (C) 2012-2014  Phisketeer.org team
#
#    This C++ library is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "phispagecache.h"
#include "phibasepage.h"
#include "phirequest.h"
#include "phiresponserec.h"
#include "phiitemfactory.h"
#include "phiabstractitems.h"

PHISPageCache* PHISPageCache::_instance=0;

static void _copyItems( PHIBasePage *dest, const PHIBasePage *source )
{
    Q_ASSERT( dest && source );
    PHIBaseItem *it;
    PHIBaseItem *copy;
    PHIAbstractLayoutItem *lit;
    QMultiHash <QByteArray, PHIBaseItem*> children;
    QList <PHIAbstractLayoutItem*> layouts;
    foreach( it, source->items() ) {
        copy=PHIItemFactory::instance()->copy( it );
        if ( !copy ) continue;
        copy->setParent( dest );
        if ( !copy->parentId().isEmpty() ) children.insert( copy->parentId(), copy );
        lit=qobject_cast<PHIAbstractLayoutItem*>(copy);
        if ( lit ) layouts.append( lit );
    }
    foreach ( lit, layouts ) lit->setChildItems( children.values( lit->id() ) );
}

QDateTime PHISPageCache::modified( const PHIRequest *req, const QString &pageId )
{
    QDateTime dt;
    _lock.lockForRead();
    dt=_modified.value( req->hostname() ).value( pageId, QDateTime() );
    _lock.unlock();
    return dt;
}

PHIBasePage* PHISPageCache::page( const PHIRequest *req, const QString &filename )
{
    _lock.lockForRead();
    const PHIBasePage *p=_pages.value( req->hostname() ).value( filename, 0 );
    if ( Q_LIKELY( p ) ) {
        QDateTime stamp=_modified.value( req->hostname() ).value( p->id(), QDateTime() );
        if ( Q_UNLIKELY( stamp < QFileInfo( filename ).lastModified() ) ) { // remove cached page
            _lock.unlock();
            _lock.lockForWrite();
            PageModified pm=_modified.take( req->hostname() );
            pm.remove( p->id() );
            _modified.insert( req->hostname(), pm );
            PageHash ph=_pages.take( req->hostname() );
            Q_ASSERT( p==ph.value( filename, 0 ) );
            ph.remove( filename );
            _pages.insert( req->hostname(), ph );
            delete p;
            _lock.unlock();
            return 0;
        }
        try {
            PHIBasePage *copy=new PHIBasePage( *p );
            _copyItems( copy, p );
            Q_CHECK_PTR( copy );
            _lock.unlock();
            return copy;
        } catch ( std::bad_alloc& ) {
            req->responseRec()->log( PHILOGCRIT, PHIRC_OUT_OF_MEMORY, QObject::tr( "Could not copy page '%1'." ).arg( p->id() ) );
        }
    }
    _lock.unlock();
    return 0;
}

PHIBasePage* PHISPageCache::insert( const PHIRequest *req, const PHIBasePage *p, const QString &filename )
{
    if ( Q_UNLIKELY( !p ) ) return 0;
    PageModified pm;
    PageHash ph;
    _lock.lockForWrite();
    if ( Q_UNLIKELY( _pages.value( req->hostname() ).contains( filename ) ) ) {
        // race condition: another thread inserted page before
        const PHIBasePage *p2=_pages.value( req->hostname() ).value( filename, 0 );
        Q_ASSERT( p2 );
        delete p;
        try {
            PHIBasePage *copy=new PHIBasePage( *p2 );
            _copyItems( copy, p2 );
            _lock.unlock();
            return copy;
        } catch ( std::bad_alloc& ) {
            req->responseRec()->log( PHILOGCRIT, PHIRC_OUT_OF_MEMORY, QObject::tr( "Could not copy page '%1'." ).arg( p->id() ) );
        }
        _lock.unlock();
        return 0;
    }
    pm=_modified.take( req->hostname() );
    pm.insert( p->id(), QFileInfo( filename ).lastModified() );
    _modified.insert( req->hostname(), pm );

    ph=_pages.take( req->hostname() );
    Q_ASSERT( !ph.contains( filename ) );
    ph.insert( filename, p );
    _pages.insert( req->hostname(), ph );
    try {
        PHIBasePage *copy=new PHIBasePage( *p );
        _copyItems( copy, p );
        _lock.unlock();
        return copy;
    } catch ( std::bad_alloc& ) {
        req->responseRec()->log( PHILOGCRIT, PHIRC_OUT_OF_MEMORY, QObject::tr( "Could not copy page '%1'." ).arg( p->id() ) );
    }
    _lock.unlock();
    return 0;
}

void PHISPageCache::invalidate()
{
    qDebug( "PHISPageCache::invalidate()" );
    _lock.lockForWrite();
    PageHash ph;
    const PHIBasePage *page;
    foreach ( ph, _pages.values() ) {
        foreach ( page, ph.values() ) delete page;
        ph.clear();
    }
    _pages.clear();

    PageModified pm;
    foreach ( pm, _modified ) pm.clear();
    _modified.clear();
    _lock.unlock();
}
