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
#include <QFileInfo>
#include <QTemporaryFile>
#include <QSqlDatabase>
#include <QSqlError>
#include <QPair>
#include <QList>
#include <QtScript>
#include <QReadWriteLock>
#include <QUuid>
#include "phiprocessor.h"
#include "phis.h"
#include "phi.h"
#include "phidataparser.h"
#include "phierror.h"
#include "phispagecache.h"
#include "phisitemcache.h"
#include "phisscriptobjects.h"
#include "phihtmlgenerator.h"
#include "phigenerator.h"
#include "phisession.h"
#include "phiparent.h"
#include "philicense.h"
#include "phismodulefactory.h"
#include "phismodule.h"

#ifdef PHIAPPSTORE
#include "macfilebookmark.h"
#endif

static QByteArray _allocError=QT_TRANSLATE_NOOP( "PHIProcessor",
    "Could not parse page. Memory allocation error." );
static QByteArray _resourceError=QT_TRANSLATE_NOOP( "PHIProcessor",
    "Resource error. Try again later." );

PHIProcessor::PHIProcessor( const PHISRequest *req, int connId )
    : _req( req ), _connectionId( connId )
{
    //qDebug( "PHIProcessor::PHIProcessor()" );
    Q_ASSERT( req );
    _resp=req->responseRec();
    Q_ASSERT( _resp );
}

PHIProcessor::~PHIProcessor()
{
    //qDebug( "PHIProcessor::~PHIProcessor()" );
}

void PHIProcessor::run()
{
    _resp->clear();
    _resp->_minorHttpVer=_req->httpServerMinorVersion();
    if ( _req->keyword( PHISRequest::KMethod )=="HEAD" ) _resp->_options|=PHIResponseRec::HeaderOnly;
    if ( _req->canonicalFilename().endsWith( "/phi.phis" ) ) return genSysItem();
#ifdef PHIAPPSTORE
    PHISecFile f( _req->canonicalFilename() );
#else
    QFile f( _req->canonicalFilename() );
#endif
    QFileInfo fi( f );
    if ( fi.suffix()!=QString::fromLatin1( "phis" ) ) {
        // not used in Apache (other file types are handled by Apache itself)
        if ( !fi.isReadable() )
            return _resp->error( PHILOGERR, PHIRC_HTTP_NOT_FOUND, QObject::tr( "File not found on server." ) );
        /* Moved to phihttp.cpp - apache handels it internally
        QByteArray type=PHI::mimeType( fi.suffix().toUtf8() );
        if ( type.isEmpty() ) {
            return _resp->error( PHILOGERR, PHIRC_HTTP_UNSUPPORTED_MEDIA_TYPE,
                QObject::tr( "Could not identify mime type of requested resource (%1)." )
                    .arg( fi.suffix() ) );
        }*/
        _resp->_options|=PHIResponseRec::SendFile;
        _resp->_fileToSend=_req->canonicalFilename();
        _resp->_contentLength=f.size();
        //_resp->_contentType=type;
        _resp->_headersOut.insert( "Last-Modified", _resp->timeEncoded( fi.lastModified() ) );
        return;
    }
    QReadLocker l( PHISPageCache::readWriteLock() );
    PHISPage *page=PHISPageCache::page( _req );
    if ( !page ) {
        if ( !f.open( QIODevice::ReadOnly ) ) {
            _resp->log( PHILOGWARN, PHIRC_IO_FILE_ACCESS_ERROR,
                QObject::tr( "Could not open '%1' for reading." )
                .arg( _req->canonicalFilename() )+PHI::errorText().arg( f.errorString() ) );
            return _resp->error( PHILOGERR, PHIRC_HTTP_NOT_FOUND,
                QObject::tr( "Could not open '%1' page." ).arg( _req->url().toString() ) );
        }
        loadPage( page, &f, false );
        f.close();
        if ( _resp->rc()!=PHIRC_OK ) return;
        page=PHISPageCache::insert( _req, page );
    }
    Q_ASSERT( page );
    if ( page->attributes() & PHIPage::ADatabase ) {
        if ( QSqlDatabase::contains( QString::number( _connectionId ) ) ) {
            _db=QSqlDatabase::database( QString::number( _connectionId ) );
        } else if ( page->attributes() & PHIPage::ADbSettingsFromFile ) {
            QFile dbfile( page->dbFileName() );
            if ( dbfile.open( QIODevice::ReadOnly ) ) {
                QString entry=QString::fromUtf8( dbfile.readAll() );
                QStringList list=entry.split( ':' );
                if ( list.count()<7 ) {
                    _resp->log( PHILOGWARN, PHIRC_ARGUMENT_ERROR, QObject::tr(
                        "There are not enough fields to extract the database settings from the file '%1'. "
                        "A valid string is: 'QDRIVER:host:dbname:username:passwd:connectoptions:"
                        "portnumber'" ).arg( dbfile.fileName() ) );
                    _db=QSqlDatabase();
                } else {
                    _db=QSqlDatabase::addDatabase( list.at( 0 ), QString::number( _connectionId ) );
                    _db.setHostName( list.at( 1 ) );
                    _db.setDatabaseName( list.at( 2 ) );
                    _db.setUserName( list.at( 3 ) );
                    _db.setPassword( list.at( 4 ) );
                    _db.setConnectOptions( list.at( 5 ) );
                    _db.setPort( list.at( 6 ).toInt() );
                }
                dbfile.close();
            } else {
                _resp->log( PHILOGWARN, PHIRC_IO_FILE_ACCESS_ERROR, QObject::tr(
                    "Could not open file '%1' for database settings." )
                    .arg( dbfile.fileName() ) );
                _db=QSqlDatabase();
            }
        } else {
            _db=QSqlDatabase::addDatabase( page->dbDriver(), QString::number( _connectionId ) );
            _db.setDatabaseName( page->dbName() );
            _db.setHostName( page->dbHost() );
            _db.setUserName( page->dbUser() );
            _db.setPassword( page->dbPasswd() );
            _db.setConnectOptions( page->dbConnectOptions() );
            _db.setPort( page->dbPort() );
        }
        if ( _db.isValid() ) {
            if ( !_db.isOpen() ) {
                if ( !_db.open() ) {
                    _resp->log( PHILOGWARN, PHIRC_DB_ERROR, QObject::tr(
                        "Page '%1' with ID '%2' is marked for database access. But the DB could not be "
                        "opened using DB name '%3', user '%4', host '%5', port '%6'. DB error: [%7] %8" )
                        .arg( _req->canonicalFilename() ).arg( page->id() ).arg( _db.databaseName() )
                        .arg( _db.userName() ).arg( _db.hostName() ).arg( _db.port() )
                        .arg( _db.lastError().number() ).arg( _db.lastError().text() ) );
                } else qDebug( "Opening DB with connection ID %d", _connectionId );
            }
        } else {
            _resp->log( PHILOGWARN, PHIRC_DB_ERROR, QObject::tr(
                "Page '%1' with ID '%2' is marked for database access. But the DB is marked invalid "
                "using DB name '%3', user '%4', host '%5', port '%6'. DB error: [%7] %8" )
                .arg( _req->canonicalFilename() ).arg( page->id() ).arg( _db.databaseName() )
                .arg( _db.userName() ).arg( _db.hostName() ).arg( _db.port() )
                .arg( _db.lastError().number() ).arg( _db.lastError().text() ) );
        }
    }
    /* PHISPageCache must be locked at this point! */
    _resp->_body=parse( page );

    if ( _resp->rc()!=PHIRC_OK ) return;    
    if ( _resp->_options & PHIResponseRec::Redirect ) return;

    //_resp->_contentType=PHI::mimeType();
    _resp->_contentLength=_resp->_body.size();
    _resp->_headersOut.insert( "Last-Modified", _resp->timeEncoded( fi.lastModified() ) );
    if ( _resp->_options & PHIResponseRec::SendFile ) {
        // refers to another file
        fi.setFile( _resp->_fileToSend );
        _resp->_contentLength=fi.size();
        _resp->_headersOut.insert( "Last-Modified", _resp->timeEncoded( fi.lastModified() ) );
        return;
    }
}

void PHIProcessor::genSysItem() const
{
    QStringList list=_req->getKeys();
    if ( list.contains( "phiimg" ) ) return genImage();
    else if ( list.contains( "phicss" ) ) return genCSS();
    else if ( list.contains( "phijs" ) ) return genJS();
    else if ( list.contains( "philicense" ) ) return genLicense();
    return _resp->error( PHILOGERR, PHIRC_HTTP_NOT_FOUND,
        QObject::tr( "Could not access requested system item.") );
}

void PHIProcessor::genCSS() const
{
    QString file=_req->getValue( "phicss" );
    QString path=_req->tmpDir()+QDir::separator()+"css"+QDir::separator()+file+".css";
    if ( !QFile::exists( path ) ) {
        _resp->log( PHILOGERR, PHIRC_IO_FILE_ACCESS_ERROR,
            QObject::tr( "Could not access CSS file '%1'." ).arg( path ) );
        return _resp->error( PHILOGERR, PHIRC_HTTP_NOT_FOUND, QObject::tr( "System CSS not found." ) );
    }
    QFileInfo fi( path );
    if ( !fi.isReadable() ) {
        _resp->log( PHILOGERR, PHIRC_IO_FILE_ACCESS_ERROR,
            QObject::tr( "Could not access CSS file '%1'." ).arg( path ) );
        return _resp->error( PHILOGERR, PHIRC_HTTP_NOT_FOUND, QObject::tr( "Could not access CSS file." ) );
    }
    _resp->_contentType="text/css";
    if ( fi.lastModified().toUTC() <= _req->ifModifiedSince() ) return _resp->notModified();

    _resp->_options|=PHIResponseRec::SendFile;
    _resp->_fileToSend=path;
    _resp->_contentLength=fi.size();
    _resp->_headersOut.insert( "Last-Modified", _resp->timeEncoded( fi.lastModified() ) );
    _resp->_headersOut.insert( "Cache-Control", "public" );
}

void PHIProcessor::genJS() const
{
    QString file=_req->getValue( "phijs" );
    QString path=_req->tmpDir()+QDir::separator()+"js"+QDir::separator()+file+".js";
    if ( !QFile::exists( path ) ) {
        _resp->log( PHILOGERR, PHIRC_IO_FILE_ACCESS_ERROR,
            QObject::tr( "Could not access JavaScript file '%1'." ).arg( path ) );
        return _resp->error( PHILOGERR, PHIRC_HTTP_NOT_FOUND, QObject::tr( "JavaScript file not found." ) );
    }
    QFileInfo fi( path );
    if ( !fi.isReadable() ) {
        _resp->log( PHILOGERR, PHIRC_IO_FILE_ACCESS_ERROR,
            QObject::tr( "Could not access JavaScript file '%1'." ).arg( path ) );
        return _resp->error( PHILOGERR, PHIRC_HTTP_NOT_FOUND, QObject::tr( "Could not access JavaScript file." ) );
    }
    _resp->_contentType="text/javascript";
    if ( fi.lastModified().toUTC() <= _req->ifModifiedSince() ) return _resp->notModified();

    _resp->_options|=PHIResponseRec::SendFile;
    _resp->_fileToSend=path;
    _resp->_contentLength=fi.size();
    _resp->_headersOut.insert( "Last-Modified", _resp->timeEncoded( fi.lastModified() ) );
    _resp->_headersOut.insert( "Cache-Control", "public" );
}

void PHIProcessor::genImage() const
{
    try {
        QString path=_req->getValue( "phitmp" );
        bool useTmp=path.isEmpty() ? false : true;
        QString imgId=_req->getValue( "phiimg" );
        if ( !useTmp ) {
            if ( imgId.startsWith( '/' || imgId.startsWith( '\\' ) ) ) {
                path=_req->documentRoot()+imgId;
            } else {
                path=_req->documentRoot()+QDir::separator()+imgId;
            }
        } else {
            path=_req->imgDir()+QDir::separator()+imgId;
        }
        QFileInfo fi( path );
        if ( !fi.isReadable() ) {
            _resp->log( PHILOGERR, PHIRC_IO_FILE_ACCESS_ERROR,
                QObject::tr( "Image file '%1' is not accessible.").arg( path ) );
            return _resp->error( PHILOGERR, PHIRC_HTTP_NOT_FOUND,
                QObject::tr( "Could not find image with id '%1'." ).arg( imgId ) );
        }
        if ( fi.lastModified().toUTC() <= _req->ifModifiedSince() ) return _resp->notModified();

        _resp->_options|=PHIResponseRec::SendFile;
        _resp->_fileToSend=path;
        //_resp->_contentType=type; //automatically done by file ending
        _resp->_contentLength=fi.size();
        _resp->_headersOut.insert( "Last-Modified", _resp->timeEncoded( fi.lastModified() ) );
        _resp->_headersOut.insert( "Cache-Control", "public" );
    } catch ( std::bad_alloc& ) {
        _resp->_options &= ~PHIResponseRec::SendFile;
        _resp->_body.clear();
        _resp->_contentLength=0;
        _resp->log( PHILOGCRIT, PHIRC_OUT_OF_MEMORY, QObject::tr( _allocError ) );
        return _resp->error( PHILOGWARN, PHIRC_HTTP_SERVICE_UNAVAILABLE,
            QObject::tr( _resourceError ) );
    }
}

void PHIProcessor::genLicense() const
{
    //disabled for security issues:
    //const PHILicense *l=PHIParent::instance()->license( _req->hostname() );
    const PHILicense *l(0);
    if ( !l ) return _resp->error( PHILOGERR, PHIRC_HTTP_FORBIDDEN, QObject::tr( "Access denied" ) );
    /*
    _resp->_contentType="text/plain";
    _resp->_body="ID: "+l->id()+"\r\nProcessors: "+QByteArray::number( l->processors() )
        +"\r\nDomain: "+l->domain()+"\r\nRegistrar: "+l->registrar()
        +"\r\nValidity: "+l->validDate().toString( "yyyy-MM-dd" ).toLatin1()
        +"\r\nCreation: "+l->created().toString( "yyyy-MM-dd hh:mm:ss" ).toLatin1();
    _resp->_contentLength=_resp->_body.size();
    */
}

QByteArray PHIProcessor::parse( const PHISPage *const p )
{
    //qDebug( "PHIProcessor::parse()" );
    QByteArray arr;
    QString script;
    if ( !p->languages().contains( _req->currentLang() ) ) {
        // document does not provide the requested language, look if we find a matching one
        //if ( !_req->isPhiLangRequest() ) {
            bool found=false;
            // script variable used for lang
            foreach ( script, _req->acceptedLanguages() ) { // provided by accept languages of the browser
                if ( p->languages().contains( script, Qt::CaseSensitive ) ) { // available langs in page
                    _req->setCurrentLang( script );
                    qDebug( "setCurrentLang %s", qPrintable( script ) );
                    found=true;
                    break;
                }
            }
            if ( !found ) {
                foreach ( script, _req->acceptedLanguages() ) {
                    QString l;
                    foreach ( l, p->languages() ) {
                        if ( script.startsWith( l, Qt::CaseInsensitive ) ) {
                            _req->setCurrentLang( l );
                            qDebug( "setCurrentLang %s", qPrintable( l ) );
                            found=true;
                            break;
                        }
                    }
                }
            }
            if ( !found ) _req->setCurrentLang( "C" );
            script.clear();
        //}
    }
    qDebug( "matching lang %s", qPrintable( _req->currentLang() ) );
    PHIDataParser parser( p->internalId(), _req, _db );

    PHIBasePage page( *p );
    QString pId=p->id();

    try {
        PHISPage *master( 0 );
        page.setTitle( parser.text( pId, p->titleData() ) );
        page.setCopyright( parser.text( pId, p->copyrightData() ) );
        page.setVersion( parser.text( pId, p->versionData() ) );
        page.setAuthor( parser.text( pId, p->authorData() ) );
        page.setCompany( parser.text( pId, p->companyData() ) );
        page.setDescription( parser.text( pId, p->descriptionData() ) );
        if ( page.attributes() & PHIPage::AUseOpenGraph ) page.setOpenGraph( parser.text( pId, p->opengraphData() ) );
        if ( page.attributes() & PHIPage::AServerscript ) script=parser.text( pId, p->serverscriptData() );
        if ( page.attributes() & PHIPage::AJavascript ) page.setJavascript( parser.text( pId, p->javascriptData() ) );
        if ( page.attributes() & PHIPage::AStyleSheet ) page.setStyleSheet( parser.text( pId, p->styleSheetData() ) );
        if ( page.attributes() & PHIPage::AFormAction ) page.setAction( parser.text( pId, p->actionData() ) );
        if ( page.attributes() & PHIPage::AKeywords ) page.setKeywords( parser.text( pId, p->keysData() ) );
        if ( page.attributes() & PHIPage::ATemplate ) { // check if we have a master template page
            /* We locked for write if page is loaded newly otherwise master is loaded already as well */
            page.setTemplatePage( parser.text( pId, p->templatePageData() ) );
            qDebug( "template: %s", qPrintable( page.templatePage() ) );
            master=loadMasterPage( page.templatePage() );
            if ( master ) {
                if ( page.title().isEmpty() ) page.setTitle( parser.text( pId, master->titleData() ) );
                if ( page.copyright().isEmpty() ) page.setCopyright( parser.text( pId, master->copyrightData() ) );
                if ( page.version().isEmpty() ) page.setVersion( parser.text( pId, master->versionData() ) );
                if ( page.author().isEmpty() ) page.setAuthor( parser.text( pId, master->authorData() ) );
                if ( page.company().isEmpty() ) page.setCompany( parser.text( pId, master->companyData() ) );
                if ( master->attributes() & PHIPage::AKeywords ) {
                    if ( page.keywords().isEmpty() ) page.setKeywords( parser.text( pId, master->keysData() ) );
                }
                if ( master->attributes() & PHIPage::AJavascript )
                    page.setJavascript( parser.text( pId, master->javascriptData() )+'\n'+page.javascript() );
                if ( master->attributes() & PHIPage::AStyleSheet )
                    page.setStyleSheet( parser.text( pId, master->styleSheetData() )+'\n'+page.styleSheet() );
                if ( master->attributes() & PHIPage::AFormAction && page.action().isEmpty() )
                    page.setAction( parser.text( pId, master->actionData() ) );
                if ( master->attributes() & PHIPage::AServerscript ) {
                    script=parser.text( pId, master->serverscriptData() )+'\n'+script;
                    page.joinScriptModules( master->scriptModules() );
                }
                if ( !(page.attributes() & PHIPage::AUseOpenGraph) && (master->attributes() & PHIPage::AUseOpenGraph) ) {
                    page.setOpenGraph( parser.text( pId, master->opengraphData() ) );
                }
                page.setWidth( master->width() );
                page.setHeight( master->height() );
                if ( page.attributes() & PHIPage::AUseTemplatePalette ) {
                    if ( master->attributes() & PHIPage::APalette ) {
                        QPalette pal=master->palette();
                        if ( master->attributes() & PHIPage::ABgColor  ) {
                            pal.setColor( QPalette::Window, master->bgColor() );
                            page.setBgColor( master->bgColor().name() );
                        }
                        page.setTemplatePageOverwritesPalette( pal );
                    } else {
                        QPalette pal=page.palette();
                        if ( master->attributes() & PHIPage::ABgColor  ) {
                            pal.setColor( QPalette::Window, master->bgColor() );
                            page.setBgColor( master->bgColor().name() );
                        }
                        page.setPalette( pal );
                    }
                }
                if ( master->attributes() & PHIPage::AHasCalendar ) page.setPageUsesCalendar();
                page.addExtensions( master->extensions() );
                /*
                if ( page.attributes() & PHIPage::AUseTemplateCalendarNames ) {
                    page.setShortDayNames( parser.text( pId, master->shortDayNamesData() )
                        .split( ':', QString::SkipEmptyParts ) );
                    page.setMonthNames( parser.text( pId, master->monthNamesData() )
                        .split( ':', QString::SkipEmptyParts ) );
                }
                */
            }
        }
        QString sessPath=_req->tmpDir()+QDir::separator()+"db";
        if ( page.attributes() & PHIPage::ARequiresSession ) {
            qDebug( "requires SESSION" );
            PHISession ses( _resp, sessPath );
            QString sid=_req->requestValue( "phisid" );
            if ( ses.validateSession( sid, p->sessionTimeout() ) ) {
                page.setSession( sid );
                if ( page.attributes() & PHIPage::ASetCookie )
                    _resp->setCookie( "phisid", page.session(), p->sessionTimeout() );
            } else {
                page.setSessionRedirect( parser.text( pId, p->sessionRedirectData() ) );
                if ( master && page.sessionRedirect().isEmpty() )
                    page.setSessionRedirect( parser.text( pId, master->sessionRedirectData() ) );
                qDebug( "Session not valid - redirecting to %s", qPrintable( page.sessionRedirect() ) );
                //QUrl url( "host/index.html?url={}" );
                //qDebug( "session %s", QUrl::toPercentEncoding( url.toString() ).data() );
            }
        }
        page.setLanguages( p->languages() );
        page.setLang( _req->currentLang() );
        /*
        if ( _vars.contains( "philang", PHIVars::Request ) )
            page.setLang( _vars.value( "philang", PHIVars::Request ));
        else page.setLang( _vars.value( "lang", PHIVars::Server ) );
        */
        QByteArray id;
        PHIBaseItem *it;
        QReadLocker readLocker( PHISItemCache::instance()->readWriteLock() );
        if ( master ) {
            parser.setPageId( master->internalId() );
            foreach( id, master->ids() ) {
                it=parser.parseItem( id );
                qDebug( "inserting master item id %s", it->id().data() );
                if ( it ) page.insertElementById( it->id(), it );
            }
            parser.setPageId( p->internalId() );
        }
        foreach ( id, p->ids() ) {
            it=parser.parseItem( id );
            if ( it ) {
                if ( page.contains( it->id() ) ) {
                    _resp->log( PHILOGWARN, PHIRC_OBJ_EXISTS_ERROR,
                        QObject::tr( "Element with ID '%1' in page '%2' already exists."
                        " Item will be overwritten. Check the master template page." )
                        .arg( QString::fromUtf8( it->id() ) ).arg( page.id() ) );
                    page.removeElementById( it->id() );
                }
                page.insertElementById( it->id(), it );
            }
        }
        if ( page.attributes() & PHIPage::AApplication ) {
            QList <PHISPageMenuEntry> sourceEntries;
            if ( master && (master->attributes() & PHIPage::AApplication) )
                sourceEntries=master->menuEntries();
            else sourceEntries=p->menuEntries();

            PHISPageMenuEntry sEntry;
            foreach ( sEntry, sourceEntries ) {
                PHIPageMenuEntry dEntry=sEntry.parse( &parser );
                qDebug( "MENU %s %s", qPrintable( dEntry.text() ), qPrintable( dEntry.id() ) );
                page.addMenuEntry( dEntry );
            }
        }
        if ( page.attributes() & PHIPage::ACreateSession ) {
            PHISession sess( _resp, sessPath );
            page.setSession( sess.createSession( p->sessionTimeout(), page.session() ) );
            if ( page.attributes() & PHIPage::ASetCookie )
                _resp->setCookie( "phisid", page.session(), p->sessionTimeout() );
        }
    } catch( std::bad_alloc& ) {
        _resp->error( PHILOGCRIT, PHIRC_HTTP_SERVICE_UNAVAILABLE, QObject::tr( _resourceError ) );
        return QByteArray();
    }
    // Now we are safe to unlock - Do NOT use 'master' and 'p' after this point!!
    //qDebug( "before unlock" );
    //PHISPageCache::readWriteLock()->unlock();
    //qDebug( "after unlock" );

    if ( !script.isEmpty() ) {
        if ( runScriptEngine( &page, script ) ) {
            // true==content changed by user in reply class module via server scripting
            return _resp->body();
        }
    }
    if ( !page.sessionRedirect().isEmpty() ) {
        _resp->redirect( page.sessionRedirect() );
        return QByteArray();
    }

    bool genPhi=false;
    //qDebug( "-------%s", qPrintable( _vars.value( "accept", PHIVars::Server ) ) );
    if ( _req->requestKeys().contains( "phis" ) ) {
        if ( _req->requestValue( "phis" ).toInt()==1 ) genPhi=true;
    } else if ( _req->serverValue( "accept" ).contains( "application/x-phi", Qt::CaseSensitive ) ) {
        genPhi=true;
        if ( page.attributes() & PHIPage::AForceHtmlOutput ) genPhi=false;
    }
    if ( genPhi ) {
        PHIGenerator phiGenerator( _resp );
        arr=phiGenerator.genPhi( &page );
    } else {
        //if ( !(page.attributes() & PHIPage::ANoSystemCSS) ) {
            QFileInfo cssinfo( _req->tmpDir()+QDir::separator()+"css"+QDir::separator()+page.id()+".css" );
            if ( cssinfo.exists() ) {
                if ( _req->lastModified() > cssinfo.lastModified() ) createSystemCSS( &page );
            } else createSystemCSS( &page );
        //}
        PHIHtmlGenerator generator( _req, &page );
        arr=generator.genHtml();
    }
    _resp->_headersOut.insert( "Cache-Control", "no-cache" );
    return arr;
}

void PHIProcessor::createJS( const QString &name ) const
{
    QString file=_req->tmpDir()+QDir::separator()+"js";
    QDir dir( file );
    if ( !dir.exists( file ) ) {
        if ( !dir.mkpath( file ) ) {
            _resp->log( PHILOGERR, PHIRC_IO_FILE_CREATION_ERROR,
                QObject::tr( "Could not create temp direcory '%1'.").arg( file ) );
            return;
        }
    }
    QFile src( ":/"+name );
    if ( !src.open( QIODevice::ReadOnly ) ) {
        _resp->log( PHILOGERR, PHIRC_IO_FILE_CREATION_ERROR,
            QObject::tr( "Could not read JavaScript file '%1'.").arg( name ) );
        return;
    }
    QFile f( file+QDir::separator()+name );
    if ( !f.open( QIODevice::WriteOnly ) ) {
        _resp->log( PHILOGERR, PHIRC_IO_FILE_CREATION_ERROR,
            QObject::tr( "Could not write JavaScript file '%1'.").arg( file+QDir::separator()+name ) );
        return;
    }
    if ( name=="phibase.js" ) {
        QString str=QString::fromLatin1( src.readAll() );

        str.replace( QRegExp( " {2,}"), "" );
        str.replace( " (", "(" );
        str.replace( "( ", "(" );
        str.replace( " )", ")" );
        str.replace( ") ", ")" );
        str.replace( ", ", "," );
        str.replace( " {", "{" );
        str.replace( "{ ", "{" );
        str.replace( " }", "}" );
        str.replace( "} ", "}" );
#ifndef PHIDEBUG
        str.replace( QRegExp( "\n{1,1}" ), "" );
#endif
        f.write( str.toLatin1() );
    } else f.write( src.readAll() );
    f.close();
    src.close();
}

void PHIProcessor::createSystemCSS( const PHIBasePage* const p ) const
{
    QString file=_req->tmpDir()+QDir::separator()+"css";
    QDir dir( file );
    if ( !dir.exists( file ) ) {
        if ( !dir.mkpath( file ) ) {
            _resp->log( PHILOGERR, PHIRC_IO_FILE_CREATION_ERROR,
                QObject::tr( "Could not create temp direcory '%1'.").arg( file ) );
            return;
        }
    }
    file+=QDir::separator()+p->id()+".css";
    QFile css( file );
    if ( !css.open( QIODevice::WriteOnly ) ) {
        _resp->log( PHILOGERR, PHIRC_IO_FILE_CREATION_ERROR,
            QObject::tr( "Could not write system css file '%1'.").arg( file ) );
        return;
    }
    QByteArray out;
    out.reserve( 2000 );
    out+="@charset \"UTF-8\";\n";
    if ( !(p->attributes() & PHIPage::ANoSystemCSS) ) {
        QFont f=p->font();
        QByteArray font="font-family:'"+f.family().toUtf8();
        if ( !f.lastResortFamily().isEmpty() ) {
            font+="','"+f.lastResortFamily().toUtf8();
        }
        font+="';";
        if ( f.pointSize() > -1 ) {
            font+="font-size:"+QByteArray::number( f.pointSize() )+"pt;";
        }
        out+="body {\n";
        //out+="\twidth:"+QByteArray::number( p->width() )+"px;"
        //    "height:"+QByteArray::number( p->height() )+"px;\n"
        //    "\tpadding-left:100px;overflow:visible;\n";
        //if ( p->attributes() & PHIPage::ADocumentLeft ) {
            out+="\tmargin:0;padding:0;\n";
            //out+="\tmargin-left:auto;margin-right:auto;margin-top:0px;\n"
            //    "\tposition:relative;top:0px;left:0px;\n";
        //} else {
        //    out+="\tmargin:0;top:0px;left:0px";
        //}
        out+="\tbackground-color:"+p->palette().window().color().name().toLatin1()+";\n";
            //p->palette().window() is set to p->bgColor()
        out+="\t"+font+'\n';
        out+="\tcolor:"+p->palette().windowText().color().name().toLatin1()+";\n"
            "}\na:link {text-decoration:none;color:"
            +p->palette().link().color().name().toLatin1()+"; }\n"
            "a:visited {text-decoration:none;color:"
            +p->palette().linkVisited().color().name().toLatin1()+";}\n";
        if ( p->attributes() & PHIPage::ANoUnderlineLinks ) out+="a:hover {text-decoration:none;}\n";
        else out+="a:hover {text-decoration:underline;}\n";
        out+="input {"+font+"} select {"+font+"}\n";
        out+="button {"+font+"} textarea {"+font+"}\n";
        out+="img {position:absolute;border:none;}\n";
        out+="table {border:none;border-spacing:0;margin:0;padding:0;}\n";
        out+=".phibuttontext {color:"+p->palette().buttonText().color().name().toLatin1()+";}\n";
        out+=".phibutton {background-color:"+p->palette().button().color().name().toLatin1()+";}\n";
        out+=".phihighlight {background-color:"+p->palette().highlight().color().name().toLatin1()+";}\n";
        out+=".phihighlightedtext {color:"+p->palette().highlightedText().color().name().toLatin1()+";}\n";
        out+=".phibase {background-color:"+p->palette().base().color().name().toLatin1()+"; }\n";
        out+=".phitext {color:"+p->palette().text().color().name().toLatin1()+";}\n";
        out+=".phiwindowtext {color:"+p->palette().windowText().color().name().toLatin1()+";}\n";
        out+=".phiwindow {background-color:transparent;}\n";
        out+=".phichklistitem {margin-top:auto;margin-bottom:auto;width:20px;}\n";
        out+=".phiradioitem {width:20px;margin-top:auto;margin-bottom:auto;overflow:visible;}\n";
        out+=".phi_"+QByteArray::number( PHI::RICH_TEXT )+" {margin:0;padding:3px;overflow:auto;}\n";
        out+=".phi_"+QByteArray::number( PHI::TEXT )+" {margin:0;padding:3px;overflow:visible;}\n";
        out+=".phi_"+QByteArray::number( PHI::PROGRESSBAR )+" {overflow:hidden;}\n";

        if ( p->attributes() & PHIPage::ADocumentLeft ) {
            out+=".phicontent {z-index:0;display:block;position:relative;left:0;top:0;width:"
                +QByteArray::number( p->width() )+"px;margin:0;padding:0;}\n";
        } else {
            out+=".phicontent {z-index:0;display:block;position:relative;width:"
                +QByteArray::number( p->width() )+"px;margin:0 auto;padding:0;}\n";
        }

        out+=".phitableth {padding:4px;text-align:left;white-space:nowrap;}\n";
        out+=".phitabletd {padding-left:4px;white-space:nowrap;padding-top:2px;padding-bottom:2px;color:"
            +p->palette().text().color().name().toLatin1()+";}\n";
    }
    // User defined global page CSS
    if ( p->attributes() & PHIPage::AStyleSheet ) {
        out+=p->styleSheet().toUtf8();
    }

    if ( !(p->attributes() & PHIPage::ANoUiCSS) ) {
        // jQuery
        QFile f( ":/ui-core.css" ); //jquery ui core CSS
        if ( f.open( QIODevice::ReadOnly ) ) { out+='\n'+f.readAll(); f.close(); }
        out+=createjQueryTheme( p );
    }
    out+='\n';

    // User defined CSS for items
    PHIBaseItem *it;
    foreach ( it, p->items() ) {
        if ( !(it->itemProperties() & PHIItem::PStyleSheet) ) continue;
        QString el=it->styleSheet().simplified();
        QStringList list=el.split( '}', QString::SkipEmptyParts );
        if ( list.count()==1 ) {
            if ( el.contains( '{' ) ) out+='#'+it->id()+' '+el.toUtf8()+'\n';
            else out+='#'+it->id()+" { "+el.toUtf8()+" }\n";
        } else {
            foreach( el, list ) {
                out+='#'+it->id()+' '+el.simplified().toUtf8()+" }\n";
            }
        }
    }
    css.write( out );
    css.close();
}

static QString _phireg( const QByteArray &s ) { return QString( " [^ ]*/\\*\\{"+s+"\\}\\*/" ); }
static QString _phiurl( const QByteArray &s ) { return QString( " url(phi.phis?phiimg="+s+"&phitmp=1)" ); }

QByteArray PHIProcessor::createjQueryTheme( const PHIBasePage* const p ) const
{
    QByteArray out;
    QFont font=p->font();
    QByteArray fontarr="'"+font.family().toUtf8();
    if ( !font.lastResortFamily().isEmpty() ) fontarr+="','"+font.lastResortFamily().toUtf8();
    fontarr+="'";

    if ( !QFile::exists( _req->imgDir()+QDir::separator()+"uiiconscontent.png" ) ) {
        QImage icons( ":/iconsContent" ); //Indexed8
        icons.save( _req->imgDir()+QDir::separator()+"uiiconscontent.png", "PNG" );
        icons.save( _req->imgDir()+QDir::separator()+"uiiconsheader.png", "PNG" );
        icons.load( ":/iconsActive");
        icons.save( _req->imgDir()+QDir::separator()+"uiiconsactive.png", "PNG" );
        icons.load( ":/iconsDefault");
        icons.save( _req->imgDir()+QDir::separator()+"uiiconsdefault.png", "PNG" );
        icons.load( ":/iconsError");
        icons.save( _req->imgDir()+QDir::separator()+"uiiconserror.png", "PNG" );
        icons.load( ":/iconsHighlight");
        icons.save( _req->imgDir()+QDir::separator()+"uiiconshighlight.png", "PNG" );
        icons.load( ":/bgImgUrlHover");
        /** @todo colorize background images to respect palette. */
        icons.save( _req->imgDir()+QDir::separator()+"uibgimgurlhover.png", "PNG" );
        icons.load( ":/bgImgUrlActive");
        icons.save( _req->imgDir()+QDir::separator()+"uibgimgurlactive.png", "PNG" );
        icons.load( ":/bgImgUrlHeader");
        icons.save( _req->imgDir()+QDir::separator()+"uibgimgurlheader.png", "PNG" );
        icons.load( ":/bgImgUrlError");
        icons.save( _req->imgDir()+QDir::separator()+"uibgimgurlerror.png", "PNG" );
        icons.load( ":/bgImgUrlContent");
        icons.save( _req->imgDir()+QDir::separator()+"uibgimgurlcontent.png", "PNG" );
        icons.load( ":/bgImgUrlDefault");
        icons.save( _req->imgDir()+QDir::separator()+"uibgimgurldefault.png", "PNG" );
        icons.load( ":/bgImgUrlOverlay");
        icons.save( _req->imgDir()+QDir::separator()+"uibgimgurloverlay.png", "PNG" );
        icons.load( ":/bgImgUrlHighlight");
        icons.save( _req->imgDir()+QDir::separator()+"uibgimgurlhighlight.png", "PNG" );
    }
    QString theme;
    QFile f( ":/ui-theme.css" ); //jquery ui theme CSS
    if ( f.open( QIODevice::ReadOnly ) ) {
        theme=QString::fromLatin1( f.readAll() );
        f.close();
        /*
        theme.replace( QRegExp( _phireg( "ffDefault" ) ), ": "+fontarr );
        if ( font.pointSize()>-1 ) theme.replace( QRegExp( _phireg( "fsDefault" ) ),
            ": "+QByteArray::number( font.pointSize() )+"pt" );
        */
        theme.replace( QRegExp( _phireg( "iconsError" ) ), _phiurl( "uiiconserror.png" ) );
        theme.replace( QRegExp( _phireg( "iconsContent" ) ), _phiurl( "uiiconscontent.png" ) );
        theme.replace( QRegExp( _phireg( "iconsHeader" ) ), _phiurl( "uiiconsheader.png" ) );
        theme.replace( QRegExp( _phireg( "iconsDefault" ) ), _phiurl( "uiiconsdefault.png" ) );
        theme.replace( QRegExp( _phireg( "iconsHighlight" ) ), _phiurl( "uiiconshighlight.png" ) );
        theme.replace( QRegExp( _phireg( "iconsActive" ) ), _phiurl( "uiiconsactive.png" ) );
        theme.replace( QRegExp( _phireg( "iconsHover" ) ), _phiurl( "uiiconsactive.png" ) );

        theme.replace( QRegExp( _phireg( "bgImgUrlContent" ) ), _phiurl( "uibgimgurlcontent.png" ) );

        theme.replace( QRegExp( _phireg( "bgImgUrlHover" ) ), _phiurl( "uibgimgurlhover.png" ) );
        theme.replace( QRegExp( _phireg( "bgImgUrlActive" ) ), _phiurl( "uibgimgurlactive.png" ) );
        theme.replace( QRegExp( _phireg( "bgImgUrlHeader" ) ), _phiurl( "uibgimgurlheader.png" ) );
        theme.replace( QRegExp( _phireg( "bgImgUrlError" ) ), _phiurl( "uibgimgurlerror.png" ) );
        theme.replace( QRegExp( _phireg( "bgImgUrlDefault" ) ), _phiurl( "uibgimgurldefault.png" ) );
        theme.replace( QRegExp( _phireg( "bgImgUrlOverlay" ) ), _phiurl( "uibgimgurloverlay.png" ) );
        theme.replace( QRegExp( _phireg( "bgImgUrlHighlight" ) ), _phiurl( "uibgimgurlhighlight.png" ) );

        theme.replace( QRegExp( _phireg( "fcDefault" ) ),
            " "+p->palette().windowText().color().name() );
        theme.replace( QRegExp( _phireg( "fcActive" ) ),
            " "+p->palette().link().color().name() );
        theme.replace( QRegExp( _phireg( "fcHover" ) ),
            " "+p->additionalColor( PHIPage::DColorHover ).name() );
        theme.replace( QRegExp( _phireg( "fcHighlight" ) ),
            " "+p->palette().highlightedText().color().name() );
        theme.replace( QRegExp( _phireg( "bgColorHighlight" ) ),
            " "+p->palette().highlight().color().name() );
        theme.replace( QRegExp( _phireg( "fcHeader" ) ),
            " "+p->palette().buttonText().color().name() );
        theme.replace( QRegExp( _phireg( "bgColorHeader" ) ),
            " "+p->palette().button().color().name() );
        theme.replace( QRegExp( _phireg( "fcError" ) ),
            " "+p->additionalColor( PHIPage::DColorError ).name() );
        theme.replace( QRegExp( _phireg( "bgColorError" ) ),
            " "+p->additionalColor( PHIPage::DColorBgError ).name() );

        out+=theme.toLatin1();
    }
    if ( p->attributes() & PHIPage::AHasCalendar ) {
        f.setFileName( ":/ui-datepicker.css" ); //jquery ui datepicker CSS
        if ( f.open( QIODevice::ReadOnly ) ) { out+='\n'+f.readAll(); f.close(); }
        out+=( "\n.phical .ui-datepicker {left:0px;width:100%;height:100%;padding:.2em "
            ".2em 0;display:none;}\n.ui-datepicker {font-size:90%;min-width:240px;}\n" );
    }
    if ( p->extensions() & PHIPage::EProgressBar ) {
        f.setFileName( ":/ui-progressbar.css" ); //jquery ui progressbar CSS
        if ( f.open( QIODevice::ReadOnly ) ) { out+='\n'+f.readAll(); f.close(); }
    }
    return out;
}

bool PHIProcessor::runScriptEngine( PHIBasePage *p, const QString &script )
{
    Q_ASSERT( _resp->body().isEmpty() );
    Q_ASSERT( _resp->contentLength()==0 );
    QScriptEngine engine( p );
    qScriptRegisterMetaType( &engine, baseItemToScriptValue, baseItemFromScriptValue );
    new PHISGlobalScriptObj( _req, &engine, _db );
    QScriptValue doc=engine.newQObject( p, QScriptEngine::QtOwnership,
        QScriptEngine::PreferExistingWrapperObject |
        QScriptEngine::ExcludeSuperClassContents | QScriptEngine::ExcludeDeleteLater );
    engine.globalObject().setProperty( "document", doc );
    //PHISReplyObj *replyObj(0);
    if ( p->scriptModules()!=PHIPage::SNone ) { // keep old module style working
        //PHISServerObj *serverObj(0);
        //PHISSystemObj *systemObj(0);
        //PHISFileObj *fileObj(0);
        //PHISRequestObj *requestObj(0);
        //PHISSqlObj *sqlObj(0);
        //PHISEmailObj *emailObj(0);
        //if ( p->scriptModules() & PHIPage::SServer ) serverObj=new PHISServerObj( _req, &engine );
        //if ( p->scriptModules() & PHIPage::SSystem ) systemObj=new PHISSystemObj( &engine, _resp );
        //if ( p->scriptModules() & PHIPage::SFile ) fileObj=new PHISFileObj( &engine, _resp );
        //if ( p->scriptModules() & PHIPage::SRequest ) requestObj=new PHISRequestObj( _req, &engine, _resp );
        //if ( p->scriptModules() & PHIPage::SDatabase ) sqlObj=new PHISSqlObj( _db, &engine, _resp, p->attributes() & PHIPage::ADatabase );
        //if ( p->scriptModules() & PHIPage::SReply ) replyObj=new PHISReplyObj( &engine, _resp );
        //if ( p->scriptModules() & PHIPage::SEmail ) emailObj=new PHISEmailObj( _req, &engine, _resp );
        PHISModuleFactory *factory=PHISModuleFactory::instance();
        QMap <PHIPage::ScriptModule, QString> smmap;
        smmap.insert( PHIPage::SEmail, QStringLiteral( "email" ) );
        smmap.insert( PHIPage::SSystem, QStringLiteral( "system" ) );
        smmap.insert( PHIPage::SServer, QStringLiteral( "server" ) );
        smmap.insert( PHIPage::SFile, QStringLiteral( "file" ) );
        smmap.insert( PHIPage::SRequest, QStringLiteral( "request" ) );
        smmap.insert( PHIPage::SReply, QStringLiteral( "reply" ) );
        smmap.insert( PHIPage::SDatabase, QStringLiteral( "sql" ) );
        QString key;
        factory->lock(); //locking for read
        foreach( PHIPage::ScriptModule sm, smmap.keys() ) {
            PHISModule *mod(0);
            key=smmap.value( sm );
            if ( p->scriptModules() & sm ) mod=factory->module( key );
            if ( !mod ) continue;
            PHISScriptObj *obj=mod->create( key, new PHISInterface( _req, &engine, _db ) );
            if ( obj ) {
                obj->initObject( key );
                _resp->log( PHILOGWARN, PHIRC_MODULE_DEPRECATED,
                    QObject::tr( "The page '%1' is using the old module interface.\n"
                    "Please switch to the dynamic module loading by using\n\"loadModule('%2');\""
                    " in your server script." ).arg( p->id() ).arg( key ) );
            } else {
                QString tmp=QObject::tr( "Could not load module '%1'." ).arg( smmap.value( sm ) );
                _resp->log( PHILOGCRIT, PHIRC_MODULE_LOAD_ERROR, tmp );
            }
        }
        factory->unlock();
    }
    //if ( engine.canEvaluate( script ) ) {
        doc=engine.evaluate( script );
        if ( doc.isError() ) {
            QStringList list=engine.uncaughtExceptionBacktrace();
            QString tmp=QObject::tr( "Line: %1" ).arg( engine.uncaughtExceptionLineNumber() )+PHI::nl()+doc.toString();
            tmp+=PHI::nl()+PHI::nl()+list.join( PHI::nl() );
            _resp->log( PHILOGERR, PHIRC_SCRIPT_PARSE_ERROR, tmp );
        }
    //} else {
    //    _resp->log( PHILOGWARN, PHIRC_SCRIPT_PARSE_ERROR, QObject::tr( "Could not evaluate serverscript." ) );
    //}
    if ( _resp->contentLength() ) {
        //if ( replyObj->hasOwnContent() ) {
            qDebug( "%s %s %lld", _resp->body().data(), _resp->contentType().data(), _resp->contentLength() );
            if ( _resp->contentType().isEmpty() ) {
                QString tmp=QObject::tr( "Reply content type not set." );
                _resp->log( PHILOGWARN, PHIRC_OBJ_TYPE_ERROR, tmp );
                _resp->error( PHILOGERR, PHIRC_HTTP_INTERNAL_SERVER_ERROR, tmp );
            }
            return true; // user sets own content to send back to the client
        //}
    }
    return false; // normal processing of the page
}

PHISPage* PHIProcessor::loadMasterPage( const QString &relfilename )
{
    if ( relfilename.isEmpty() ) {
        _resp->log( PHILOGWARN, PHIRC_ARGUMENT_ERROR,
            QObject::tr( "Page '%1' is marked for using a master template page, but filename is empty." )
                .arg( _req->canonicalFilename() ) );
        return 0;
    }
    QFileInfo basedir( _req->canonicalFilename() );
    QString filename=basedir.absolutePath();
    if ( relfilename.startsWith( '/' ) || relfilename.startsWith( '\\' ) ) {
        filename=_req->documentRoot()+relfilename;
    } else {
        filename+=QDir::separator()+relfilename;
    }
    basedir.setFile( filename );
    filename=basedir.canonicalFilePath();
    qDebug( "Looking up master page %s", qPrintable( filename ) );
    PHISPage *page=PHISPageCache::page( _req, filename, basedir.lastModified() );
    if ( page ) return page;
    else {
        qDebug( "Master page not in cache, loading" );
#ifdef PHIAPPSTORE
        PHISecFile f( filename );
#else
        QFile f( filename );
#endif
        if ( !f.open( QIODevice::ReadOnly ) ) {
            _resp->log( PHILOGERR, PHIRC_IO_FILE_ACCESS_ERROR,
                QObject::tr( "Could not open master template page '%1' for reading." )
                    .arg( filename )+PHI::errorText().arg( f.errorString() ) );
            return 0;
        }
        loadPage( page, &f, true );
        f.close();
        if ( page ) page=PHISPageCache::insert( _req, filename, basedir.lastModified(), page );
        if ( page->font()==PHI::invalidFont() ) page->setFont( PHI::defaultFont() );
    }
    return page;
}

void PHIProcessor::loadPage( PHISPage *&page, QFile *f, bool isMasterPage )
{
    qDebug( "Loading page %s", qPrintable( f->fileName() ) );
    try {
        page=new PHISPage();
        QDataStream in( f );
        in.setVersion( PHI_DSV );
        quint32 magic;
        quint8 version;
        in >> magic >> version;
        if ( magic!=static_cast<quint32>(PHI_MAGIC) ) {
            _resp->log( PHILOGWARN, PHIRC_IO_FORMAT_ERROR,
                QObject::tr( "'%1' has unknown file format (magic number mismatch)." )
                    .arg( f->fileName() ) );
            return _resp->error( PHILOGERR, PHIRC_HTTP_INTERNAL_SERVER_ERROR,
                QObject::tr( "Unknown file format for '%1'." ).arg( _req->url().toString() ) );
        }
        if ( version > static_cast<quint8>(PHI_SFV) ) {
            _resp->log( PHILOGWARN, PHIRC_IO_FORMAT_ERROR,
                QObject::tr( "'%1' has a newer file format that this server can handle." )
                    .arg( f->fileName() ) );
            return _resp->error( PHILOGERR, PHIRC_HTTP_INTERNAL_SERVER_ERROR,
                QObject::tr( "'%1' has a wrong file format." )
                    .arg( _req->url().toString() ) );
        }

        page->load( in, false );
        if ( in.status()!=QDataStream::Ok ) {
            delete page;
            page=0;
            _resp->log( PHILOGWARN, PHIRC_IO_STREAM_ERROR,
                QObject::tr( "'%1' produced stream input errors." ).arg( f->fileName() ) );
            return _resp->error( PHILOGERR, PHIRC_HTTP_INTERNAL_SERVER_ERROR,
                QObject::tr( "Could not read '%1' page." ).arg( _req->url().toString() ) );
        }
        if ( page->font()==PHI::invalidFont() ) page->setFont( PHI::defaultFont() );
        if ( !(page->attributes() & PHIPage::APalette) ) {
            QPalette pal;
            pal.setColor( QPalette::Window, page->bgColor() );
            pal.setColor( QPalette::Base, QColor( Qt::white ) );
            pal.setColor( QPalette::WindowText, QColor( Qt::black ) );
            pal.setColor( QPalette::Text, QColor( Qt::black ) );
            pal.setColor( QPalette::Highlight, QColor( 98, 140, 178 ) );
            pal.setColor( QPalette::HighlightedText, QColor( Qt::white ) );
            pal.setColor( QPalette::Button, QColor( 239, 235, 231 ) );
            pal.setColor( QPalette::ButtonText, QColor( Qt::black ) );
            pal.setColor( QPalette::Link, QColor( 0, 0, 255 ) );
            pal.setColor( QPalette::LinkVisited, QColor( 255, 0, 255 ) );
            page->setPalette( pal );
        } else {
            QPalette pal=page->palette();
            pal.setColor( QPalette::Window, page->bgColor() );
            page->setPalette( pal );
        }

        if ( page->attributes() & PHIPage::AIcon ) {
            QImage icon=page->image();
            QString path=_req->imgDir()+QDir::separator()+page->id()+".ico";
            icon.save( path, "ICO" );
            QFile iconFile( path );
            iconFile.setPermissions( iconFile.permissions()
                | QFile::ReadGroup | QFile::ReadOther );
        }

        QByteArray id, arr;
        quint8 wid;
        QReadWriteLock *lock=PHISItemCache::instance()->readWriteLock();
        lock->lockForWrite();
        PHISItem *item( 0 );
        for ( quint16 c=0; c < page->storedItemCount(); c++ ) {
            in >> id >> wid >> arr;
            //qDebug( "Adding id %s", id.data() );
            if ( PHISItemCache::instance()->containsItem( _req->hostname(), page->internalId(), id ) ) {
                _resp->log( PHILOGERR, PHIRC_OBJ_EXISTS_ERROR,
                    QObject::tr( "Item IDs must be unique within the same page! "
                    "Item '%1' already exists. The second item will not be displayed." )
                    .arg( QString::fromUtf8( id ) )+PHI::nl()+QObject::tr(
                    "Check also if two pages have the same page ID." ) );
            } else {
                try {
                    item=new PHISItem();
                    item->loadFromByteArray( wid, id, arr );
                    page->addId( id ); // we are caching the ids in a list
                    if ( isMasterPage ) {
                        item->setTemplateItem( true );
                        item->setTemplatePageId( page->internalId() );
                    }
                    //qDebug( "inserting %s %s", page->internalId().data(), id.data() );
                    PHISItemCache::instance()->insertItem( _req->hostname(), page->internalId(), id, item );
                    if ( wid==PHI::CALENDAR || wid==PHI::DATEEDIT ) page->setAttribute( PHIPage::AHasCalendar, true );
                    else if ( wid==PHI::PHISYS_LINK ) item->setZValue( PHI::maxZValue()+1 );
                    else if ( item->isDragDrop() ) page->setAttribute( PHIPage::AHasDragDrop, true );
                    else if ( wid==PHI::PROGRESSBAR ) page->setExtension( PHIPage::EProgressBar, true );
                    else if ( wid==PHI::FACEBOOK_LIKE ) page->setExtension( PHIPage::EHasFacebookLike, true );
                    else if ( wid==PHI::TWITTER ) page->setExtension( PHIPage::EHasTwitter, true );
                    else if ( wid==PHI::GOOGLE_PLUS ) page->setExtension( PHIPage::EHasGooglePlus, true );
                } catch ( std::bad_alloc& ) {
                    delete page;
                    page=0;
                    lock->unlock();
                    _resp->log( PHILOGCRIT, PHIRC_OUT_OF_MEMORY, _allocError );
                    return _resp->error( PHILOGWARN, PHIRC_HTTP_SERVICE_UNAVAILABLE, _resourceError );
                }
            }
        }
        // Check if JavaScript files are already created
        QString file=_req->tmpDir()+QDir::separator()+"js"+QDir::separator()+"phibase.js";
        if ( !QFile::exists( file ) ) {
            createJS( "phibase.js" );
            createJS( "jquery.js" );
            createJS( "ui-core.js" );
            createJS( "ui-widget.js" );
            createJS( "ui-datepicker.js" );
            createJS( "ui-progressbar.js" );
            createJS( "ui-effects.js" );
            createJS( "ui-mouse.js" );
            createJS( "ui-draggable.js" );
            createJS( "ui-droppable.js" );
        }

        lock->unlock();
        //if ( !isMasterPage ) {
            // room for additional page settings
        //}
    } catch ( std::bad_alloc& ) {
        page=0;
        _resp->log( PHILOGCRIT, PHIRC_OUT_OF_MEMORY, _allocError );
        return _resp->error( PHILOGWARN, PHIRC_HTTP_SERVICE_UNAVAILABLE, _resourceError );
    }
}
