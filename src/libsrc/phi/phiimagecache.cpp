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
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QUuid>
#include <QDir>
#include <QThread>
#include "phiimagecache.h"
#include "phiapplication.h"
#include "phirequest.h"
#include "phiresponserec.h"

#define PHICREATETABLE "CREATE TABLE imgcache ( cid TEXT NOT NULL, "\
    "dtime TEXT NOT NULL, tout INTEGER NOT NULL, path TEXT NOT NULL, PRIMARY KEY( cid ) )"

PHIImageCache* PHIImageCache::_instance=0;

PHIRC PHIImageCache::init( QString &error, QObject *parent )
{
    if ( parent ) setParent( parent );
    else setParent( phiApp );
    _name.sprintf( "%s.db", "phiimagecache" );
    QString path=phiApp->tmpPath()+L1( "/db/" );
    QDir dir( path );
    if ( !dir.exists() && !dir.mkpath( path ) ) {
        error=tr( "Could not access path '%1'." ).arg( path );
        return PHIRC_IO_FILE_ACCESS_ERROR;
    }
    path=path+_name;
    qDebug() << "PHIImageCache DB name:" << path;
    _db=QSqlDatabase::addDatabase( SL( "QSQLITE" ), _name );
    // because of multi threading we can not use storage type memory
    // @todo: implement shared memory for speed up
    _db.setDatabaseName( path );
    if ( !_db.open() ) {
        error=tr( "Could not create image cache DB '%1'." ).arg( path );
        return PHIRC_DB_ERROR;
    }
    QStringList tables=_db.tables();
    if ( tables.isEmpty() ) {
        QSqlQuery query( _db );
        if ( !query.exec( QString::fromLatin1( PHICREATETABLE ) ) ) {
            error=tr( "Could not create image cache table: %1" )
                .arg( query.lastError().text() );
            return PHIRC_QUERY_ERROR;
        }
    }
    return PHIRC_OK;
}

PHIImageCache::~PHIImageCache()
{
    if ( _db.isValid() && _db.isOpen() ) {
        QSqlQuery query( _db );
        query.setForwardOnly( true );
        if ( query.exec( L1( "SELECT path FROM imgcache" ) ) ) {
            while ( query.next() ) {
                QFile::remove( query.value( 0 ).toString() );
            }
        }
        _db.close();
    }
    _db=QSqlDatabase();
    QSqlDatabase::removeDatabase( _name );
    _instance=0;
    qDebug( "PHIImageCache::~PHIImageCache(): %s", qPrintable( _name ) );
}

QByteArray PHIImageCache::createUid( const PHIRequest *req )
{
    qDebug( "PHIImageCache::createId()" );
    const QString uid=PHI::createPngUuid();
    if ( Q_UNLIKELY( !_db.isOpen() ) ) {
        req->responseRec()->log( PHILOGERR, PHIRC_DB_ERROR,
            tr( "Image cache DB is not open. Cached image '%1' will not be automatically removed!" ).arg( uid ) );
        return '$'+uid.toLatin1();
    }
    const QString path=req->imgDir()+QDir::separator()+QLatin1Char( '$' )+uid+SL( ".png" );
    const QDateTime cdt=QDateTime::currentDateTime();
    QSqlQuery query( _db );
    QString sql=SL( "INSERT INTO imgcache (cid,dtime,tout,path) VALUES( '" );
    sql.append( uid ).append( SL( "','" ) ).append( cdt.toString( PHI::dtFormatString() ) )
        .append( SL( "',60,'" ) ).append( path ).append( SL( "')" ) );
    if ( Q_UNLIKELY( !query.exec( sql ) ) ) {
        req->responseRec()->log( PHILOGERR, PHIRC_QUERY_ERROR,
            tr( "Could not insert image cache key into DB. Image '%1' will not be removed automatically: '%2'." )
            .arg( path ).arg( query.lastError().text() ) );
    }
    return '$'+uid.toLatin1();
}

void PHIImageCache::cleanCache( const PHIRequest *req ) const
{
    qDebug( "PHIImageCache::cleanCache()" );
    if ( Q_UNLIKELY( !_db.isOpen() ) ) {
        req->responseRec()->log( PHILOGERR, PHIRC_DB_ERROR,
            tr( "Image cache DB '%1' is not open. Could not clean the cache!" )
            .arg( _db.databaseName() ) );
        return;
    }
    QSqlQuery query( _db );
    query.setForwardOnly( true );
    if ( Q_UNLIKELY( !query.exec( SL( "SELECT cid,dtime,tout,path FROM imgcache" ) ) ) ) {
        req->responseRec()->log( PHILOGERR, PHIRC_QUERY_ERROR, tr( "Could not cleanup image cache DB: '%1'" )
            .arg( query.lastError().text() ) );
        return;
    }
    const QDateTime cdt=QDateTime::currentDateTime();
    QDateTime dt;
    QStringList timeoutIds, pathes;
    while ( query.next() ) {
        dt=QDateTime::fromString( query.value( 1 ).toString(), PHI::dtFormatString() );
        if ( dt.addSecs( query.value( 2 ).toInt() ) < cdt ) {
            timeoutIds.append( query.value( 0 ).toString() );
            pathes.append( query.value( 3 ).toString() );
        }
    }
    for ( int i=0; i<timeoutIds.count(); i++ ) {
        query.exec( SL( "DELETE FROM imgcache WHERE cid='" )+timeoutIds.at( i )+QLatin1Char( '\'' ) );
        qDebug( "Deleting img %s", qPrintable( pathes.at( i ) ) );
        QFile::remove( pathes.at( i ) );
    }
}

#undef PHICREATETABLE
