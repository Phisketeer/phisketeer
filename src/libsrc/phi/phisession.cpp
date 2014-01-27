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
#include <QSqlError>
#include <QDateTime>
#include <QUuid>
#include <QDir>
#include <QSqlQuery>
#include <QThread>
#include "phisession.h"
#include "phiapplication.h"
#include "phirequest.h"
#include "phiresponserec.h"

#define PHICREATETABLE "CREATE TABLE sess ( sid TEXT NOT NULL, "\
    "dtime TEXT NOT NULL, tout INTEGER NOT NULL, PRIMARY KEY( sid ) )"

PHISession* PHISession::_instance=0;

PHIRC PHISession::init( QString &error, QObject *parent )
{
    if ( parent ) setParent( parent );
    else setParent( phiApp );
    _name=L1( "phisession.db" );
    QString path=phiApp->tmpPath()+L1( "/db/" );
    QDir dir( path );
    if ( !dir.exists() ) dir.mkpath( path );
    path=path+_name;
    qDebug() << "PHISession DB name:" << path;
    _db=QSqlDatabase::addDatabase( SL( "QSQLITE" ), _name );
    // because of multi threading we can not use storage type memory
    // @todo: implement shared memory to speed up db access
    _db.setDatabaseName( path );
    if ( !_db.open() ) {
        error=tr( "Could not create session DB '%1': '%2'" )
            .arg( path ).arg( _db.lastError().text() );
        return PHIRC_DB_ERROR;
    }
    QStringList tables=_db.tables();
    if ( tables.isEmpty() ) {
        QSqlQuery query( _db );
        if ( !query.exec( QString::fromLatin1( PHICREATETABLE ) ) ) {
            error=tr( "Could not create session table: %1" )
                .arg( query.lastError().text() );
            return PHIRC_QUERY_ERROR;
        }
    }
    return PHIRC_OK;
}

PHISession::~PHISession()
{
    if ( _db.isValid() && _db.isOpen() ) _db.close();
    _db=QSqlDatabase();
    QSqlDatabase::removeDatabase( _name );
    qDebug( "PHISession::~PHISession(): %s", qPrintable( _name ) );
}

QString PHISession::createSession( const PHIRequest *req, qint32 timeout, const QString &sid ) const
{
    Q_ASSERT( parent() );
    if ( Q_UNLIKELY( !_db.isOpen() ) ) {
        req->responseRec()->log( PHILOGERR, PHIRC_DB_ERROR, tr( "Session DB is not open." ) );
        return QString();
    }
    QString uid=sid;
    if ( uid.isEmpty() ) {
        uid=QUuid::createUuid().toString();
        uid.chop( 1 );
        uid.remove( 0, 1 );
    }
    qDebug( "SESSION create %s (%d)", qPrintable( uid ), timeout );
    QSqlQuery query( _db );
    query.exec( SL( "DELETE FROM sess WHERE sid='" )+uid+SL( "'" ) );
    QDateTime cdt=QDateTime::currentDateTime();
    QString sql=SL( "INSERT INTO sess (sid,dtime,tout) VALUES('" );
    sql+=uid+L1( "','" )+cdt.toString( PHI::dtFormatString() )
        +L1( "'," )+QString::number( timeout )+QLatin1Char( ')' );
    if ( Q_UNLIKELY( !query.exec( sql ) ) ) {
        req->responseRec()->log( PHILOGERR, PHIRC_QUERY_ERROR,
            tr( "Could not insert session key into DB: %1" ).arg( query.lastError().text() ) );
        return QString();
    }
    sql=SL( "SELECT sid,dtime,tout FROM sess" );
    if ( Q_UNLIKELY( !query.exec( sql ) ) ) {
        req->responseRec()->log( PHILOGERR, PHIRC_QUERY_ERROR,
            tr( "Could not cleanup session DB for page '%1': %2" )
            .arg( req->canonicalFilename() ).arg( query.lastError().text() ) );
        return uid;
    }
    QDateTime dt;
    QStringList timeoutIds;
    while ( query.next() ) {
        dt=QDateTime::fromString( query.value( 1 ).toString(), PHI::dtFormatString() );
        if ( dt.addSecs( query.value( 2 ).toInt()*60 ) < cdt )
            timeoutIds.append( query.value( 0 ).toString() );
    }
    foreach ( sql, timeoutIds ) {
        query.exec( SL( "DELETE FROM sess WHERE sid='" )+sql+QLatin1Char( '\'' ) );
        qDebug( "Deleting %s", qPrintable( sql ) );
    }
    return uid;
}

bool PHISession::validateSession( const PHIRequest *req, qint32 timeout, const QString &uid ) const
{
    if ( Q_UNLIKELY( !_db.isOpen() ) ) {
        req->responseRec()->log( PHILOGERR, PHIRC_DB_ERROR,
            tr( "Session DB is not open: %1" ).arg( _db.lastError().text() ) );
        return false;
    }
    QSqlQuery query( _db );
    QString sql=SL( "SELECT dtime,tout FROM sess WHERE sid='" )+uid+QLatin1Char( '\'' );
    if ( Q_UNLIKELY( !query.exec( sql ) ) ) {
        req->responseRec()->log( PHILOGERR, PHIRC_QUERY_ERROR,
            tr( "Could not get session key from DB for page '%1': %2" )
            .arg( req->canonicalFilename() ).arg( query.lastError().text() ) );
        return false;
    }
    if ( Q_UNLIKELY( !query.next() ) ) {
        qDebug( "uid=%s (invalid)", qPrintable( uid ) );
        return false;
    }
    QDateTime dt=QDateTime::fromString( query.value( 0 ).toString(), PHI::dtFormatString() );
    QDateTime cdt=QDateTime::currentDateTime();
    qDebug( "SESSION ADD secs %d (%d)", query.value( 1 ).toInt()*60, timeout );
    if ( Q_UNLIKELY( dt.addSecs( query.value( 1 ).toInt()*60 ) < cdt ) ) {
        qDebug( "SESSION timedout %s %s",
            qPrintable( dt.toString( QStringLiteral( "hh:mm:ss:zzz" ) ) ),
            qPrintable( cdt.toString( QStringLiteral( "hh:mm:ss:zzz" ) ) ) );
        return false;
    }
    sql=SL( "UPDATE sess SET dtime='" )+cdt.toString( PHI::dtFormatString() )
        +L1( "' WHERE sid='" )+uid+QLatin1Char( '\'' );
    if ( Q_UNLIKELY( !query.exec( sql ) ) ) {
        req->responseRec()->log( PHILOGERR, PHIRC_QUERY_ERROR,
            tr( "Could not update session key in DB for page '%1': %2" )
            .arg( req->canonicalFilename() ).arg( query.lastError().text() ) );
        return false;
    }
    return true;
}

#undef PHICREATETABLE
