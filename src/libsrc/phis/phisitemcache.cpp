/*
#    Copyright (C) 2010-2012  Marius B. Schumacher
#    Copyright (C) 2011-2012  Phisys AG, Switzerland
#    Copyright (C) 2012  Phisketeer.org team
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
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "phisitemcache.h"
#include "phispagecache.h"
#include "phierror.h"

#define PHICREATETABLE "CREATE TABLE delimg ( pageid TEXT NOT NULL, "\
    "dtime TEXT NOT NULL, imageid TEXT NOT NULL, PRIMARY KEY( pageid ) )"

PHISItemCache* PHISItemCache::_instance=0;

PHISItemCache::PHISItemCache( QObject *parent )
    : QObject( parent )
{
    qDebug( "PHISItemCache::PHISItemCache()" );
    //_id=PHISPageCache::getDbId();
    //QSqlDatabase::addDatabase( "QSQLITE", QString::number( _id ) );
}

/*
void PHISItemCache::openDB( const PHISRequest *req )
{
    QString path=req->tmpDir()+QDir::separator()+"db";
    _imageIdLock.lockForWrite();
    QSqlDatabase db=QSqlDatabase::database( QString::number( _id ), false );
    db.setDatabaseName( path+QDir::separator()+"phiitemcache.db" );
    QDir dbdir( path );
    if ( !dbdir.exists() ) dbdir.mkpath( path );
    if ( !db.open() ) {
        _imageIdLock.unlock();
        req->responseRec()->log( PHILOGERR, PHIRC_DB_ERROR,
        QObject::tr( "Could not open item cache DB '%1'.\nTemporarily created images will not be deleted automatically."
            "\nCheck the directory '%2' manually." ).arg( db.databaseName() ).arg( req->imgDir() ) );
        return;
    }
    QStringList tables=db.tables();
    if ( tables.isEmpty() ) {
        QSqlQuery query( db );
        if ( !query.exec( PHICREATETABLE ) ) {
            req->responseRec()->log( PHILOGERR, PHIRC_QUERY_ERROR,
            QObject::tr( "Could not create item cache table." )
                + PHI::errorText().arg( query.lastError().text() ) );
        }
    }
    _imageIdLock.unlock();
}
*/

PHISItemCache::~PHISItemCache()
{
    invalidate();
    _instance=0;
    /*
    if ( QSqlDatabase::contains( QString::number( _id ) ) ) {
        { // needed to delete db instance before removing
            QSqlDatabase db=QSqlDatabase::database( QString::number( _id ) );
            if ( db.isValid() && db.isOpen() ) {
                qDebug( "Removing item cache DB connection Id %d", _id );
                db.close();
            }
        } // remove db instance
        QSqlDatabase::removeDatabase( QString::number( _id ) );
    }
    PHISPageCache::removeDbId( _id );
    */
    qDebug( "PHISItemCache::~PHISItemCache()" );
}

PHISItemCache* PHISItemCache::instance( QObject *parent )
{
    if ( _instance ) return _instance;
    _instance=new PHISItemCache( parent );
    return _instance;
}

void PHISItemCache::insertItem( const QString &domain, const QByteArray &page,
    const QByteArray &id, PHISItem *it )
{
    Q_ASSERT( !containsItem( domain, page, id ) );
    PhisItemPageHash ph=_items.take( domain );
    PhisItemHash ih=ph.take( page );
    ih.insert( id, it );
    ph.insert( page, ih );
    _items.insert( domain, ph );
}

void PHISItemCache::insertImageId( const QString &domain, const QByteArray &page, const QByteArray &key,
    const QString &id, const QByteArray &lang, int num )
{
    QByteArray newkey=key+lang+ (num>-1 ? QByteArray::number( num ) : "");
    _imageIdLock.lockForWrite();
    PhisImagePageHash ph=_imageIds.take( domain );
    PhisImageHash ih=ph.take( page );
    ih.insert( newkey, id );
    ph.insert( page, ih );
    _imageIds.insert( domain, ph );
    _imageIdLock.unlock();
}

QString PHISItemCache::imageId( const QString &domain, const QByteArray &page, const QByteArray &key,
    const QByteArray &lang, int num ) const
{
    QByteArray newkey=key+lang+ (num>-1 ? QByteArray::number( num ) : "");
    QReadLocker l( &_imageIdLock );
    return _imageIds.value( domain ).value( page ).value( newkey, QString() );
}

void PHISItemCache::insertPosition( const QString &domain, const QByteArray &page,
    const QByteArray &id, const QByteArray &pos )
{
    _posLock.lockForWrite();
    PhisPositionPageHash pp=_positions.take( domain );
    PhisPositionHash ph=pp.take( page );
    ph.insert( id, pos );
    pp.insert( page, ph );
    _positions.insert( domain, pp );
    _posLock.unlock();
}

void PHISItemCache::invalidate( const PHISRequest *req, const QByteArray &page )
{
    _textLock.lockForWrite();
    _textHash.clear();
    _textLock.unlock();

    {
    _imageIdLock.lockForWrite();
    PhisImagePageHash ph=_imageIds.take( req->hostname() );
    ph.remove( page );
    _imageIds.insert( req->hostname(), ph );
    _imageIdLock.unlock();

    _posLock.lockForWrite();
    PhisPositionPageHash pp=_positions.take( req->hostname() );
    pp.remove( page );
    _positions.insert( req->hostname(), pp ); 
    _posLock.unlock();
    }

    _lock.lockForWrite();
    PhisItemPageHash ph=_items.take( req->hostname() );
    PhisItemHash ih=ph.take( page );
    PHISItem *it;
    foreach ( it, ih.values() ) delete it;
    ih.clear();
    ph.insert( page, ih );
    _items.insert( req->hostname(), ph );
    _lock.unlock();
}

void PHISItemCache::invalidate()
{
    _lock.lockForWrite();
    qDebug( "PHISItemCache::invalidate()" );
    PHISItem *it;
    PhisItemPageHash ph;
    PhisItemHash ih;
    foreach ( ph, _items ) {
        foreach ( ih, ph.values() ) {
            foreach ( it, ih.values() ) delete it;
            ih.clear();
        }
        ph.clear();
    }
    _items.clear();
    _lock.unlock();

    _textLock.lockForWrite();
    _textHash.clear();
    _textLock.unlock();

    _imageIdLock.lockForWrite();
    _positions.clear();
    _imageIds.clear();

    _imageIdLock.unlock();
}

#undef PHICREATETABLE
