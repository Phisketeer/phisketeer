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
#include "phisession.h"
#include "phierror.h"
#include "phispagecache.h"

#define PHICREATETABLE "CREATE TABLE sess ( sid TEXT NOT NULL, "\
    "dtime TEXT NOT NULL, tout INTEGER NOT NULL, PRIMARY KEY( sid ) )"

PHISession::PHISession( PHIResponseRec *resp, const QString &path, QObject *parent )
    : QObject( parent ), _resp( resp )
{
    qDebug( "PHISession::PHISession()" );
    Q_ASSERT( resp );
    _id=PHISPageCache::getDbId();
    QSqlDatabase db=QSqlDatabase::addDatabase( "QSQLITE", QString::number( _id ) );
    db.setDatabaseName( path+QDir::separator()+"phisid.db" );
    QDir dbdir( path );
    if ( !dbdir.exists() ) dbdir.mkpath( path );
    if ( !db.open() ) {
        _resp->log( PHILOGERR, PHIRC_DB_ERROR,
            QObject::tr( "Could not open session DB %1. Sessions will not work." )
            .arg( db.databaseName() ) );
        return;
    }
    QStringList tables=db.tables();
    if ( tables.isEmpty() ) {
        QSqlQuery query( db );
        if ( !query.exec( PHICREATETABLE ) ) {
            _resp->log( PHILOGERR, PHIRC_QUERY_ERROR,
                QObject::tr( "Could not create session table." )
                + PHI::errorText().arg( query.lastError().text() ) );
        }
    }
}

PHISession::~PHISession()
{
    if ( QSqlDatabase::contains( QString::number( _id ) ) ) {
        { // needed to delete db instance before removing
            QSqlDatabase db=QSqlDatabase::database( QString::number( _id ) );
            if ( db.isValid() && db.isOpen() ) {
                qDebug( "Removing session DB connection Id %d", _id );
                db.close();
            }
        } // remove db instance
        QSqlDatabase::removeDatabase( QString::number( _id ) );
    }
    PHISPageCache::removeDbId( _id );
    qDebug( "PHISession::~PHISession()" );
}

QString PHISession::createSession( qint32 timeout, const QString &sid )
{
    qDebug( "PHISession::createSession()" );
    QSqlDatabase db=QSqlDatabase::database( QString::number( _id ) );
    if ( !db.isOpen() ) {
        _resp->log( PHILOGERR, PHIRC_DB_ERROR, QObject::tr( "Session DB is not open." ) );
        return QString();
    }
    QString uid=sid;
    if ( uid.isEmpty() ) {
        uid=QUuid::createUuid().toString();
        uid.chop( 1 );
        uid.remove( 0, 1 );
    }
    qDebug( "SESSION create %s (%d)", qPrintable( uid ), timeout );
    QSqlQuery query( db );
    QDateTime cdt=QDateTime::currentDateTime();
    QString sql=QString( "INSERT INTO sess ( sid, dtime, tout ) VALUES( '%1', '%2', %3 )" )
        .arg( uid ).arg( cdt.toString( PHI::dtFormat() ) ).arg( timeout );
    if ( !query.exec( sql ) ) {
        _resp->log( PHILOGERR, PHIRC_QUERY_ERROR,
            QObject::tr( "Could not insert session key into DB." )
            + PHI::errorText().arg( query.lastError().text() ) );
        return QString();
    }
    sql=QString( "SELECT sid, dtime, tout FROM sess" );
    if ( !query.exec( sql ) ) {
        _resp->log( PHILOGERR, PHIRC_QUERY_ERROR,
            QObject::tr( "Could not cleanup session DB." )
            + PHI::errorText().arg( query.lastError().text() ) );
        return uid;
    }
    QDateTime dt;
    QStringList timeoutIds;
    while ( query.next() ) {
        dt=QDateTime::fromString( query.value( 1 ).toString(), PHI::dtFormat() );
        if ( dt.addSecs( query.value( 2 ).toInt()*60 ) < cdt )
            timeoutIds.append( query.value( 0 ).toString() );
    }
    foreach ( sql, timeoutIds ) {
        query.exec( "DELETE FROM sess WHERE sid='"+sql+"'" );
        qDebug( "Deleting %s", qPrintable( sql ) );
    }
    return uid;
}

bool PHISession::validateSession( const QString &uid, qint32 timeout )
{
    qDebug( "PHISession::validateSession" );
    QSqlDatabase db=QSqlDatabase::database( QString::number( _id ) );
    if ( !db.isOpen() ) {
        _resp->log( PHILOGERR, PHIRC_DB_ERROR, QObject::tr( "Session DB is not open." ) );
        return false;
    }
    QSqlQuery query( db );
    QString sql=QString( "SELECT dtime, tout FROM sess WHERE sid='%1'" ).arg( uid );
    if ( !query.exec( sql ) ) {
        _resp->log( PHILOGERR, PHIRC_QUERY_ERROR,
            QObject::tr( "Could not get session key from DB." )
            + PHI::errorText().arg( query.lastError().text() ) );
        return false;
    }
    if ( !query.next() ) {
        qDebug( "uid=%s (invalid)", qPrintable( uid ) );
        return false;
    }
    QDateTime dt=QDateTime::fromString( query.value( 0 ).toString(), PHI::dtFormat() );
    QDateTime cdt=QDateTime::currentDateTime();
    qDebug( "SESSION ADD secs %d (%d)", query.value( 1 ).toInt()*60, timeout );
    if ( dt.addSecs( query.value( 1 ).toInt()*60 ) < cdt ) {
        qDebug( "SESSION timedout %s %s",
                qPrintable( dt.toString("hh:mm:ss:zzz")), qPrintable( cdt.toString("hh:mm:ss:zzz") ) );
        // session timedout
        return false;
    }
    qDebug( "SESSION valid %s %s", qPrintable( dt.toString("hh:mm:ss:zzz")), qPrintable( cdt.toString("hh:mm:ss:zzz") ) );
    sql=QString( "UPDATE sess SET dtime='%1' WHERE sid='%2'" )
        .arg( cdt.toString( PHI::dtFormat() ) ).arg( uid );
    if ( !query.exec( sql ) ) {
        _resp->log( PHILOGERR, PHIRC_QUERY_ERROR,
            QObject::tr( "Could not update session key in DB." )
            + PHI::errorText().arg( query.lastError().text() ) );
        return false;
    }
    return true;
}

#undef PHICREATETABLE
