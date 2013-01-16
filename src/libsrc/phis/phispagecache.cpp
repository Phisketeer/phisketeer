/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2013  Phisys AG, Switzerland
#    Copyright (C) 2012-2013  Phisketeer.org team
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
#include "phisitemcache.h"

QHash <QString, PageHash> PHISPageCache::_pages;
QHash <QString, PageModified> PHISPageCache::_modified;
QSet <int> PHISPageCache::_dbIds;
QMutex PHISPageCache::_dbLock;
QReadWriteLock PHISPageCache::_lock;

// must be read locked outside
QDateTime PHISPageCache::modified( const PHISRequest *req, const QByteArray &pageId )
{
    QDateTime dt;
    dt=_modified.value( req->hostname() ).value( pageId, QDateTime() );
    return dt;
}

// must be read locked outside
PHISPage* PHISPageCache::page( const PHISRequest *req, const QString &id, const QDateTime &dt )
{
    PHISPage *p=_pages.value( req->hostname() ).value( id, 0 );
    if ( !p ) return 0;
    QDateTime stamp=_modified.value( req->hostname() ).value( p->internalId(), QDateTime() );
    qDebug( "STAMP %s / %s", qPrintable( stamp.toString() ), qPrintable( dt.toString() ) );
    if ( stamp != dt ) {        
        _lock.unlock();
        _lock.lockForWrite();
        PHISItemCache::instance()->invalidate( req, p->internalId() );
        qDebug( "%s has been updated", p->internalId().data() );

        PageModified pm=_modified.take( req->hostname() );
        pm.remove( p->internalId() );
        _modified.insert( req->hostname(), pm );

        PageHash ph=_pages.take( req->hostname() );
        Q_ASSERT( p==ph.value( id, 0 ) );
        ph.remove( id );
        delete p;
        _pages.insert( req->hostname(), ph );
        return 0;
    }
    qDebug( "found cached page" );
    return p;
}

//must be read locked outside
PHISPage* PHISPageCache::insert( const PHISRequest *req, PHISPage *p )
{
    return insert( req, req->canonicalFilename(), req->lastModified(), p );
}

//must be read locked outside
PHISPage* PHISPageCache::insert( const PHISRequest *req, const QString &id, const QDateTime &dt, PHISPage *p )
{
    PageModified pm;
    PageHash ph;
    _lock.unlock();
    _lock.lockForWrite();
    if ( _pages.value( req->hostname() ).contains( id ) ) {
        // race condition: another thread inserted page before
        delete p;
        return _pages.value( req->hostname() ).value( id, 0 );
    }

    pm=_modified.take( req->hostname() );
    pm.insert( p->internalId(), dt );
    _modified.insert( req->hostname(), pm );

    ph=_pages.take( req->hostname() );
    Q_ASSERT( !ph.contains( id ) );
    ph.insert( id, p );
    _pages.insert( req->hostname(), ph );
    return p;
}

void PHISPageCache::invalidate()
{
    qDebug( "PHISPageCache::invalidate()" );
    _lock.lockForWrite();
    PHISItemCache::instance()->invalidate();
    PageHash ph;
    PHISPage *page;
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

int PHISPageCache::getDbId()
{
    _dbLock.lock();
    int i=-1;
    while( 1 ) {
        if ( _dbIds.contains( ++i ) ) continue;
        break;
    }
    _dbIds.insert( i );
    _dbLock.unlock();
    return i;
}

void PHISPageCache::removeDbId( int id )
{
    _dbLock.lock();
    _dbIds.remove( id );
    _dbLock.unlock();
}
