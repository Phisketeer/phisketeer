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
#include <QDataStream>
#include <QFileInfo>
#include <QSqlError>
#include "phisprocessor.h"
#include "phirequest.h"
#include "phibasepage.h"
#include "phispagecache.h"
#include "phiresponserec.h"
#include "phiitemfactory.h"
#include "phibaseitem.h"
#include "phidataparser.h"
#include "phi.h"

static void _findMatchingLang( const PHIBasePage *p, const PHIRequest *req )
{
    if ( p->languages().contains( req->currentLang() ) ) return;
    // document does not provide the requested language, look if we find a matching one
    QString lang;
    foreach ( lang, req->acceptedLanguages() ) { // provided by accept languages of the browser
        if ( p->languages().contains( lang, Qt::CaseSensitive ) ) { // available langs in page
            req->setCurrentLang( lang ); //
            qDebug( "setCurrentLang %s", qPrintable( lang ) );
            return;
        }
    }
    foreach ( lang, req->acceptedLanguages() ) {
        QString l;
        foreach ( l, p->languages() ) {
            if ( lang.startsWith( l, Qt::CaseInsensitive ) ) {
                req->setCurrentLang( l );
                qDebug( "setCurrentLang %s", qPrintable( l ) );
                return;
            }
        }
    }
    req->setCurrentLang( p->defaultLanguage() );
}

void PHISProcessor::run()
{
    _req->responseRec()->clear();
    _req->responseRec()->setMinorHttpVer( _req->httpServerMinorVersion() );
    if ( Q_UNLIKELY( _req->keyword( PHIRequest::KMethod )=="HEAD" ) ) _req->responseRec()->setHeadRequestOnly();
    if ( _req->canonicalFilename().endsWith( L1( "/phi.phis" ) ) ) return genSysItem();
    QFile f( _req->canonicalFilename() );
    if ( Q_UNLIKELY( !f.exists() ) ) return _req->responseRec()->error( PHILOGERR,
           PHIRC_HTTP_NOT_FOUND, tr( "File not found on server." ) );
    if ( Q_UNLIKELY( !_req->canonicalFilename().endsWith( L1( "phis" ) ) ) ) {
        // not used in Apache (other file types are handled by Apache itself)
        _req->responseRec()->setFileName( _req->canonicalFilename() );
        _req->responseRec()->setContentLength( f.size() );
        _req->responseRec()->setHeader( BL( "Last-Modified" ),
            _req->responseRec()->timeEncoded( QFileInfo( f ).lastModified() ) );
        return;
    }
    PHIBasePage *page=PHISPageCache::page( _req );
    if ( Q_UNLIKELY( !page ) ) page=PHISPageCache::insert( _req, loadPage( f ) );
    else initDb( page );
    if ( Q_UNLIKELY( !page ) ) return _req->responseRec()->error( PHILOGCRIT,
        PHIRC_HTTP_INTERNAL_SERVER_ERROR, tr( "Resource error." ) );
    _findMatchingLang( page, _req );
    _req->setDefaultLang( page->defaultLanguage() );
    _req->dump();
    PHIDataParser parser( _req, page->id(), _db );
    page->parseData( parser );
    PHIBaseItem *it;
    foreach( it, page->items() ) {
        qDebug() << it->id();
        it->privateParseData( parser );
    }
    _req->responseRec()->setBody( QByteArray( "<html><h1>Hello world</h1></html>" ) );
    _req->responseRec()->setContentLength( _req->responseRec()->body().size() );
    delete page;
}

PHIBasePage* PHISProcessor::loadPage( QFile &file )
{
    if ( Q_UNLIKELY( !file.open( QIODevice::ReadOnly ) ) ) {
        _req->responseRec()->log( PHILOGERR, PHIRC_IO_FORMAT_ERROR,
            tr( "Could not access Phis file '%1'." ).arg( _req->canonicalFilename() ) );
        return 0;
    }
    QDataStream in( &file );
    in.setVersion( PHI_DSV );
    quint32 magic;
    quint8 version;
    in >> magic >> version;
    if ( Q_UNLIKELY( magic!=static_cast<quint32>(PHI_MAGIC) ) ) {
        _req->responseRec()->log( PHILOGERR, PHIRC_IO_FORMAT_ERROR,
            tr( "Could not read Phis file header for '%1'." ).arg( _req->url().path() ) );
        return 0;
    }
    if ( Q_UNLIKELY( version > static_cast<quint8>(PHI_SFV) ) ) {
        _req->responseRec()->log( PHILOGERR, PHIRC_IO_FORMAT_ERROR,
            tr( "Phis file version for '%1' is newer than the server can handle." )
            .arg( _req->url().path() ) );
        return 0;
    }
    quint16 itemCount;
    PHIBasePage *page;
    try {
        page=new PHIBasePage( 0 );
        itemCount=page->load( in, static_cast<int>(version) );
        initDb( page );
    } catch ( std::bad_alloc& ) {
        return 0;
    }
    PHIDataParser parser( _req, page->id(), _db );
    try {
        page->createTmpData( parser );
    } catch ( std::bad_alloc& ) {
        delete page;
        return 0;
    }
    quint8 wid;
    quint16 wid16;
    QByteArray id, arr;
    PHIBaseItem *it;
    for ( quint16 i=0; i<itemCount; i++ ) {
        in >> id >> wid;
        if ( Q_UNLIKELY( wid==0 ) ) in >> wid16;
        else wid16=static_cast<quint16>(wid);
        try {
            it=PHIItemFactory::instance()->item( static_cast<PHIWID>(wid16),
                PHIBaseItemPrivate( PHIBaseItemPrivate::TServerItem, page, 0 ) );
            if ( Q_UNLIKELY( !it ) ) {
                _req->responseRec()->log( PHILOGERR, PHIRC_OBJ_ACCESS_ERROR,
                    tr( "Could not create item for WID '%1'" ).arg( wid16 ) );
                continue;
            }
            it->setId( id );
            arr.clear();
            in >> arr;
            it->load( arr, static_cast<int>(version) );
            it->privateCreateTmpData( parser );
        } catch ( std::bad_alloc& ) {
            delete page;
            return 0;
        }
    }
    return page;
}

void PHISProcessor::initDb( const PHIBasePage *page )
{
    Q_ASSERT( page );
    if ( page->flags() & PHIBasePage::FUseDB ) {
        if ( Q_UNLIKELY( QSqlDatabase::contains( QString::number( _dbConnId ) ) ) ) return;
        else if ( Q_UNLIKELY( page->flags() & PHIBasePage::FDBFile ) ) {
            QFile dbfile( page->dbFileName() );
            if ( dbfile.open( QIODevice::ReadOnly ) ) {
                QString entry=QString::fromUtf8( dbfile.readAll() );
                QStringList list=entry.split( QLatin1Char( ':' ) );
                if ( list.count()<7 ) {
                    _req->responseRec()->log( PHILOGWARN, PHIRC_ARGUMENT_ERROR, tr(
                        "There are not enough fields to extract the database settings from the file '%1'. "
                        "A valid string is: 'QDRIVER:host:dbname:username:passwd:connectoptions:"
                        "portnumber'" ).arg( dbfile.fileName() ) );
                    return;
                } else {
                    _db=QSqlDatabase::addDatabase( list.at( 0 ), QString::number( _dbConnId ) );
                    _db.setHostName( list.at( 1 ) );
                    _db.setDatabaseName( list.at( 2 ) );
                    _db.setUserName( list.at( 3 ) );
                    _db.setPassword( list.at( 4 ) );
                    _db.setConnectOptions( list.at( 5 ) );
                    _db.setPort( list.at( 6 ).toInt() );
                }
                dbfile.close();
            } else {
                _req->responseRec()->log( PHILOGWARN, PHIRC_IO_FILE_ACCESS_ERROR,
                    tr( "Could not open file '%1' for reading database settings." )
                    .arg( dbfile.fileName() ) );
                return;
            }
        } else {
            _db=QSqlDatabase::addDatabase( page->dbDriver(), QString::number( _dbConnId ) );
            _db.setDatabaseName( page->dbName() );
            _db.setHostName( page->dbHost() );
            _db.setUserName( page->dbUser() );
            _db.setPassword( page->dbPasswd() );
            _db.setConnectOptions( page->dbConnectOptions() );
            _db.setPort( page->dbPort() );
        }
        if ( Q_LIKELY( _db.isValid() ) ) {
            if ( !_db.isOpen() ) {
                if ( Q_UNLIKELY( !_db.open() ) ) {
                    _req->responseRec()->log( PHILOGWARN, PHIRC_DB_ERROR, tr(
                        "Page '%1' with ID '%2' is marked for database access. But the DB could not be "
                        "opened using DB name '%3', user '%4', host '%5', port '%6'. DB error: [%7] %8" )
                        .arg( _req->canonicalFilename() ).arg( page->id() ).arg( _db.databaseName() )
                        .arg( _db.userName() ).arg( _db.hostName() ).arg( _db.port() )
                        .arg( _db.lastError().number() ).arg( _db.lastError().text() ) );
                } else qDebug( "Opening DB with connection ID %d", _dbConnId );
            }
        } else {
            _req->responseRec()->log( PHILOGWARN, PHIRC_DB_ERROR, tr(
                "Page '%1' with ID '%2' is marked for database access. But the DB is invalid "
                "using DB name '%3', user '%4', host '%5', port '%6'. DB error: [%7] %8" )
                .arg( _req->canonicalFilename() ).arg( page->id() ).arg( _db.databaseName() )
                .arg( _db.userName() ).arg( _db.hostName() ).arg( _db.port() )
                .arg( _db.lastError().number() ).arg( _db.lastError().text() ) );
        }
    }
}

void PHISProcessor::genSysItem() const
{

}
