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
#include <QDateTime>
#include <QUuid>
#include <QDir>
#include "phierror.h"
#include "phisimagecache.h"
#include "phispagecache.h"

#define PHICREATETABLE "CREATE TABLE imgcache ( cid TEXT NOT NULL, "\
    "dtime TEXT NOT NULL, tout INTEGER NOT NULL, PRIMARY KEY( cid ) )"

PHISImageCache::PHISImageCache( PHIResponseRec *resp, const QString &path, QObject *parent )
    : QObject( parent ), _resp( resp ), _path( path )
{
    qDebug( "PHISImageCache::PHISImageCache()" );
    Q_ASSERT( resp );
    _id=PHISPageCache::getDbId();
    QSqlDatabase db=QSqlDatabase::addDatabase( "QSQLITE", QString::number( _id ) );
    db.setDatabaseName( path+QDir::separator()+"db"+QDir::separator()+"phiimgcache.db" );
    QDir dbdir( path );
    if ( !dbdir.exists() ) dbdir.mkpath( path );
    if ( !db.open() ) {
        _resp->log( PHILOGERR, PHIRC_DB_ERROR,
            QObject::tr( "Could not open image cache DB %1. Images will not be deleted automatically." )
            .arg( db.databaseName() ) );
        return;
    }
    QStringList tables=db.tables();
    if ( tables.isEmpty() ) {
        QSqlQuery query( db );
        if ( !query.exec( PHICREATETABLE ) ) {
            _resp->log( PHILOGERR, PHIRC_QUERY_ERROR,
                QObject::tr( "Could not create image cache table." )
                + PHI::errorText().arg( query.lastError().text() ) );
        }
    }
}

PHISImageCache::~PHISImageCache()
{
    if ( QSqlDatabase::contains( QString::number( _id ) ) ) {
        { // needed to delete db instance before removing
            QSqlDatabase db=QSqlDatabase::database( QString::number( _id ) );
            if ( db.isValid() && db.isOpen() ) {
                qDebug( "Removing image cache DB connection Id %d", _id );
                db.close();
            }
        } // remove db instance
        QSqlDatabase::removeDatabase( QString::number( _id ) );
    }
    PHISPageCache::removeDbId( _id );
    qDebug( "PHISImageCache::~PHISImageCache()" );
}

QString PHISImageCache::createId()
{
    qDebug( "PHISImageCache::createId()" );
    QSqlDatabase db=QSqlDatabase::database( QString::number( _id ) );
    if ( !db.isOpen() ) {
        _resp->log( PHILOGERR, PHIRC_DB_ERROR, QObject::tr( "Image cache DB is not open." ) );
        return PHI::createPngUuid();
    }
    QString uid=PHI::createPngUuid();
    QSqlQuery query( db );
    int timeout=60;
    QDateTime cdt=QDateTime::currentDateTime();
    QString sql=QString( "INSERT INTO imgcache ( cid, dtime, tout ) VALUES( '%1', '%2', %3 )" )
        .arg( uid ).arg( cdt.toString( PHI::dtFormat() ) ).arg( timeout );
    if ( !query.exec( sql ) ) {
        _resp->log( PHILOGERR, PHIRC_QUERY_ERROR,
            QObject::tr( "Could not insert image cache key into DB. Image will not be removed automatically." )
            + PHI::errorText().arg( query.lastError().text() ) );
        return uid;
    }
    sql=QString( "SELECT cid, dtime, tout FROM imgcache" );
    if ( !query.exec( sql ) ) {
        _resp->log( PHILOGERR, PHIRC_QUERY_ERROR,
            QObject::tr( "Could not cleanup image cache DB." )
            + PHI::errorText().arg( query.lastError().text() ) );
        return uid;
    }
    QDateTime dt;
    QStringList timeoutIds;
    while ( query.next() ) {
        dt=QDateTime::fromString( query.value( 1 ).toString(), PHI::dtFormat() );
        if ( dt.addSecs( query.value( 2 ).toInt() ) < cdt )
            timeoutIds.append( query.value( 0 ).toString() );
    }
    foreach ( sql, timeoutIds ) {
        query.exec( "DELETE FROM imgcache WHERE cid='"+sql+"'" );
        qDebug( "Deleting %s", qPrintable( sql ) );
        QFile::remove( _path+QDir::separator()+"img"+QDir::separator()+sql );
    }
    return uid;
}

#undef PHICREATETABLE
