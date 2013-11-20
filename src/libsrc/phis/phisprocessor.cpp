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
#include <QDir>
#include <QScriptEngine>
#include <QScriptValue>
#include "phisprocessor.h"
#include "phirequest.h"
#include "phibasepage.h"
#include "phispagecache.h"
#include "phiresponserec.h"
#include "phiitemfactory.h"
#include "phisscriptobjects.h"
#include "phibaseitem.h"
#include "phiabstractitems.h"
#include "phidataparser.h"
#include "phisession.h"
#include "phi.h"

static void _findMatchingLang( const PHIBasePage *p, const PHIRequest *req )
{
    if ( p->languages().contains( req->currentLang() ) ) return;
    // document does not provide the requested language, look if we find a matching one
    QByteArray lang;
    foreach ( lang, req->acceptedLanguages() ) { // provided by accept languages of the browser
        if ( p->languages().contains( QString::fromLatin1( lang ), Qt::CaseSensitive ) ) { // available langs in page
            req->setCurrentLang( lang ); //
            qDebug( "setCurrentLang %s", lang.constData() );
            return;
        }
    }
    req->setCurrentLang( p->defaultLanguage() );
}

void PHISProcessor::run()
{
    _req->responseRec()->clear();
    _req->responseRec()->setMinorHttpVer( _req->httpServerMinorVersion() );
    if ( Q_UNLIKELY( _req->keyword( PHIRequest::KMethod )=="HEAD" ) ) _req->responseRec()->setHeadRequestOnly();
    if ( _req->canonicalFilename().endsWith( SL( "phi.phis" ) ) ) return genSysItem();
    QFile f( _req->canonicalFilename() );
    if ( Q_UNLIKELY( !f.exists() ) ) return _req->responseRec()->error( PHILOGERR,
           PHIRC_HTTP_NOT_FOUND, tr( "File not found on server." ) );
    if ( Q_UNLIKELY( !_req->canonicalFilename().endsWith( SL( "phis" ) ) ) ) {
        // not used in Apache (other file types are handled by Apache itself):
        _req->responseRec()->setFileName( _req->canonicalFilename() );
        _req->responseRec()->setContentLength( f.size() );
        _req->responseRec()->setHeader( BL( "Last-Modified" ),
            _req->responseRec()->timeEncoded( QFileInfo( f ).lastModified() ) );
        return;
    }

    // normal processing
    PHIBasePage *page=PHISPageCache::page( _req, _req->canonicalFilename() ); // implicitly copies items
    if ( Q_UNLIKELY( !page ) ) page=PHISPageCache::insert( _req, loadPage( f ), _req->canonicalFilename() );
    else initDb( page );
    if ( Q_UNLIKELY( !page ) ) return _req->responseRec()->error( PHILOGCRIT,
        PHIRC_HTTP_INTERNAL_SERVER_ERROR, tr( "Resource error." ) );
    _findMatchingLang( page, _req );
    _req->setDefaultLang( page->defaultLanguage() );
    page->setLang( _req->currentLang() );
    PHIDataParser parser( _req, page->id(), _db );
    page->phisParseData( parser );

    PHIBaseItem *it;
    foreach( it, page->items() ) {
        it->phisPrivateParseData( parser );
        if ( it->hasExtension() ) {
            QByteArray ext;
            PHIWID wid=it->htmlScriptExtension( ext );
            if ( wid ) page->insertHtmlJQueryExtension( wid, ext );
        }
    }

    // master template
    PHIBasePage *master( 0 );
    if ( page->flags() & PHIBasePage::FHasMasterTemplate ) {
        f.setFileName( resolveRelativeFile( page->templatePage() ) );
        qDebug() << "filename master" << f.fileName();
        if ( Q_LIKELY( f.exists() ) ) {
            master=PHISPageCache::page( _req, f.fileName() );
            if ( Q_UNLIKELY( !master ) ) master=PHISPageCache::insert( _req, loadPage( f ), f.fileName() );
            if ( master ) parseMaster( master, page );
        } else {
            _req->responseRec()->log( PHILOGERR, PHIRC_OBJ_NOT_FOUND_ERROR,
                tr( "The page '%1' is marked for using a master template '%2'.\n"
                    "However the file could not be resolved." )
                .arg( _req->canonicalFilename() ).arg( f.fileName() ) );
        }
    }

    // run server script
    bool ownContent=false;
    if ( master && !master->serverScript().isNull() ) {
        ownContent=runScript( master, page );
    } else if ( !page->serverScript().isNull() ) {
        ownContent=runScript( 0, page );
    }
    delete master; // not needed anymore
    if ( Q_UNLIKELY( ownContent ) ) {
        // true: content changed by user in a server scripting class
        Q_ASSERT( !_req->responseRec()->contentType().isEmpty() );
        delete page;
        return;
    }

    // session management
    static const QString phisid( L1( "phisid" ) );
    if ( Q_UNLIKELY( page->sessionOptions() & PHIBasePage::SCreateSession ) ) {
        page->setSession( PHISession::instance()->createSession( _req, page->sessionTimeout(), page->session() ) );
        if ( page->sessionOptions() & PHIBasePage::SSessionCookie ) {
            _req->responseRec()->setCookie( phisid, page->session(), page->sessionTimeout() );
        }
    }
    if ( page->sessionOptions() & PHIBasePage::SRequiresSession ) {
        QString sid=_req->requestValue( phisid );
        if ( Q_LIKELY( PHISession::instance()->validateSession( _req, page->sessionTimeout(), sid ) ) ) {
            page->setSession( sid );
            if ( page->sessionOptions() & PHIBasePage::SSessionCookie ) {
                _req->responseRec()->setCookie( phisid, sid, page->sessionTimeout() );
            }
        } else {
            _req->responseRec()->redirect( resolveRelativeFile( page->sessionRedirect() ) );
            delete page;
            return;
        }
    }
    bool genPhi=false;
    if ( Q_UNLIKELY( _req->requestKeys().contains( SL( "phis" ) ) ) ) {
        if ( _req->requestValue( SL( "phis" ) ).toInt()==1 ) genPhi=true;
    } else if ( Q_UNLIKELY( _req->serverValue( SL( "accept" ) ).contains( QString::fromLatin1( PHI::phiMimeType() ), Qt::CaseSensitive ) ) ) {
        genPhi=true;
    }
    QByteArray out;
    out.reserve( 50*1024 );
    if ( Q_UNLIKELY( genPhi ) ) {
        //PHIGenerator phiGenerator( _req->responseRec() );
        //arr=phiGenerator.genPhi( page );
    } else {
        QFileInfo cssinfo( _req->tmpDir()+SL( "/css/" )+page->id()+SL( ".css" ) );
        if ( Q_LIKELY( cssinfo.exists() ) ) {
            if ( Q_UNLIKELY( _req->lastModified() > cssinfo.lastModified() ) ) page->createCSSFile( _req );
        } else page->createCSSFile( _req );
        page->generateHtml( _req, out );
    }
    _req->responseRec()->setHeader( BL( "Cache-Control" ), BL( "no-cache" ) );
    _req->responseRec()->setBody( out );
    _req->responseRec()->setContentLength( out.size() );
    delete page;
}

bool PHISProcessor::runScript( const PHIBasePage *master, PHIBasePage *page ) const
{
    Q_ASSERT( _req->responseRec()->body().isEmpty() );
    Q_ASSERT( _req->responseRec()->contentLength()==0 );
    QScriptEngine *engine=new QScriptEngine( page );
    qScriptRegisterMetaType( engine, baseItemToScriptValue, baseItemFromScriptValue );
    PHISGlobalScriptObj gso( page, _req, _db, engine );
    Q_UNUSED( gso )
    QScriptValue res=engine->newQObject( page, QScriptEngine::QtOwnership, QScriptEngine::PreferExistingWrapperObject |
        QScriptEngine::ExcludeSuperClassContents | QScriptEngine::ExcludeDeleteLater );
    engine->globalObject().setProperty( SL( "document" ), res );
    if ( Q_UNLIKELY( page->flags() & PHIBasePage::FServerModulesCombat ) ) {
        // version 1.x style
        _req->responseRec()->log( PHILOGWARN, PHIRC_MODULE_LOG,
            tr( "Page '%1' (or its master page)\nis using the old server module interface."
                " Please use 'loadModule(<module>)'." ).arg( _req->canonicalFilename() ) );
        enum ScriptModule { SNone=0x0, SDatabase=0x1, SFile=0x2, SSystem=0x4, SProcess=0x8,
            SServer=0x10, SRequest=0x20, SEmail=0x40, SReply=0x80, SAll=0xFFFF }; // qint32
        qint32 modules=page->serverModules();
        const QString lm=SL( "loadModule('" );
        if ( modules & SDatabase ) engine->evaluate( lm+L1( "sql')" ) );
        if ( modules & SFile ) engine->evaluate( lm+L1( "file')" ) );
        if ( modules & SSystem ) engine->evaluate( lm+L1( "system')" ) );
        if ( modules & SProcess ) engine->evaluate( lm+L1( "process')" ) );
        if ( modules & SServer ) engine->evaluate( lm+L1( "server')" ) );
        if ( modules & SRequest ) engine->evaluate( lm+L1( "request')" ) );
        if ( modules & SEmail ) engine->evaluate( lm+L1( "email')" ) );
        if ( modules & SReply ) engine->evaluate( lm+L1( "reply')" ) );
    }
    if ( master ) {
        res=engine->evaluate( master->serverScript() );
        if ( Q_UNLIKELY( res.isError() ) ) {
            QStringList list=engine->uncaughtExceptionBacktrace();
            QString tmp=tr( "Parse error in master page with ID '%1', line: %2" )
                .arg( master->id() ).arg( engine->uncaughtExceptionLineNumber() )+
            QString::fromLatin1( PHI::nl() )+res.toString();
            tmp+=QString::fromLatin1( PHI::nl()+PHI::nl() )+list.join( QString::fromLatin1( PHI::nl() ) );
            _req->responseRec()->log( PHILOGERR, PHIRC_SCRIPT_PARSE_ERROR, tmp );
        }
    }
    res=engine->evaluate( page->serverScript() );
    if ( Q_UNLIKELY( res.isError() ) ) {
        QStringList list=engine->uncaughtExceptionBacktrace();
        QString tmp=tr( "Parse error in page '%1', line: %2" )
            .arg( _req->canonicalFilename() ).arg( engine->uncaughtExceptionLineNumber() )+
        QString::fromLatin1( PHI::nl() )+res.toString();
        tmp+=QString::fromLatin1( PHI::nl()+PHI::nl() )+list.join( QString::fromLatin1( PHI::nl() ) );
        _req->responseRec()->log( PHILOGERR, PHIRC_SCRIPT_PARSE_ERROR, tmp );
    }
    if ( Q_UNLIKELY( _req->responseRec()->contentLength() ) ) {
        if ( Q_UNLIKELY( _req->responseRec()->contentType().isEmpty() ) ) {
            QString tmp=tr( "Reply content type not set in server script for page '%1'." )
                .arg( _req->canonicalFilename() );
            _req->responseRec()->log( PHILOGWARN, PHIRC_OBJ_TYPE_ERROR, tmp );
            tmp=tr( "Script parsing error." );
            _req->responseRec()->error( PHILOGERR, PHIRC_HTTP_INTERNAL_SERVER_ERROR, tmp );
        }
        return true; // user defined own content to send back to the client
    }
    return false; // normal processing of the page
}

void PHISProcessor::parseMaster( PHIBasePage *master, PHIBasePage *page ) const
{
    if ( Q_UNLIKELY( master->flags() & PHIBasePage::FUseDB ) ) {
        _req->responseRec()->log( PHILOGWARN, PHIRC_DB_ERROR,
            tr( "Using a defined database in master template pages is not supported.\n"
            "The database settings of page '%1' will be used instead (if any)\nfor '%2'." )
            .arg( _req->canonicalFilename() ).arg( master->id() ) );
    }
    PHIDataParser parser( _req, master->id(), _db );
    master->phisParseData( parser );
    page->copyMasterData( master );
    PHIBaseItem *it;
    PHIByteArrayList itemIds=page->itemIdsByteArray();
    foreach( it, master->items() ) {
        if ( Q_UNLIKELY( itemIds.contains( it->id() ) ) ) {
            _req->responseRec()->log( PHILOGERR, PHIRC_OBJ_IN_USE_ERROR,
                tr( "The page '%1' contains already an item with id '%2'.\n"
                "The item from the master page '%3' will be discarded!" )
                .arg( _req->canonicalFilename() ).arg( it->name() ).arg( master->id() ) );
            continue;
        }
        it->phisPrivateParseData( parser );
        it->setParent( page );
        it->setZIndex( -10000+it->realZIndex() );
        if ( it->hasExtension() ) {
            QByteArray ext;
            PHIWID wid=it->htmlScriptExtension( ext );
            if ( wid ) page->insertHtmlJQueryExtension( wid, ext );
        }
    }
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
        page->phisCreateData( parser );
    } catch ( std::bad_alloc& ) {
        delete page;
        return 0;
    }
    quint8 wid;
    quint16 wid16;
    QByteArray id, arr;
    PHIBaseItem *it;
    PHIAbstractLayoutItem *l;
    QList <PHIAbstractLayoutItem*> layouts;
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
            it->phisPrivateCreateData( parser );
            if ( it->hasExtension() ) {
                QByteArray ext;
                PHIWID extWid=it->htmlHeaderExtension( ext );
                if ( extWid ) page->insertHtmlHeaderExtension( extWid, ext );
            }
            l=qobject_cast<PHIAbstractLayoutItem*>(it);
            if ( l ) layouts.append( l );
        } catch ( std::bad_alloc& ) {
            delete page;
            return 0;
        }
    }
    file.close();
    qDebug() << "layout child count" << layouts.count();
    foreach ( l, layouts ) l->activateLayout();
    page->genJQueryThemeFile( _req );
    static bool created=false;
    if ( !created ) genScripts(); // @todo: create once at server start
    created=true;
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
    QStringList list=_req->getKeys();
    if ( list.contains( SL( "i" ) ) ) return genImage();
    else if ( list.contains( SL( "j" ) ) ) return genJS();
    else if ( list.contains( SL( "s" ) ) ) return genCSS();
    return _req->responseRec()->error( PHILOGERR, PHIRC_HTTP_NOT_FOUND,
        tr( "Could not access requested phi* system item." ) );
}

void PHISProcessor::genImage() const
{
    bool useTmp=_req->getKeys().contains( SL( "t" ) );
    QString imgPath=_req->getValue( SL( "i" ) );
    if ( Q_UNLIKELY( !useTmp ) ) {
        _req->dump();
        imgPath=resolveRelativeFile( imgPath );
    } else {
        // @todo: remove tmp images from PHIImageCache
        if ( imgPath.endsWith( SL( ".ico" ) ) )
            imgPath=_req->imgDir()+QDir::separator()+imgPath;
        else imgPath=_req->imgDir()+QDir::separator()+imgPath+SL( ".png" );
    }
    QFileInfo fi( imgPath );
    if ( !fi.isReadable() ) {
        _req->responseRec()->log( PHILOGERR, PHIRC_IO_FILE_ACCESS_ERROR,
            tr( "Image file '%1' is not accessible. Referer: '%2'.").arg( imgPath ).arg( _req->referer() ) );
        return _req->responseRec()->error( PHILOGERR, PHIRC_HTTP_NOT_FOUND,
            tr( "Could not find image with id '%1'." ).arg( _req->getValue( SL( "i" ) ) ) );
    }
    if ( fi.lastModified().toUTC() <= _req->ifModifiedSince() ) return _req->responseRec()->notModified();
    _req->responseRec()->setFileName( imgPath );
    //_req->responseRec()->setContentType(); // automatically resolved mime type
    _req->responseRec()->setContentLength( fi.size() );
    _req->responseRec()->setHeader( BL( "Last-Modified" ), _req->responseRec()->timeEncoded( fi.lastModified() ) );
    _req->responseRec()->setHeader( BL( "Cache-Control" ), BL( "public" ) );
}

void PHISProcessor::genCSS() const
{
    QString path=_req->getValue( SL( "s" ) );
    path=_req->tmpDir()+L1( "/css/" )+path+L1( ".css" );
    if ( Q_UNLIKELY( !QFile::exists( path ) ) ) {
        _req->responseRec()->log( PHILOGERR, PHIRC_IO_FILE_ACCESS_ERROR,
            tr( "Could not access CSS file '%1'." ).arg( path ) );
        return _req->responseRec()->error( PHILOGERR, PHIRC_HTTP_NOT_FOUND, tr( "System CSS not found." ) );
    }
    QFileInfo fi( path );
    if ( Q_UNLIKELY( !fi.isReadable() ) ) {
        _req->responseRec()->log( PHILOGERR, PHIRC_IO_FILE_ACCESS_ERROR,
            tr( "Could not access CSS file '%1'." ).arg( path ) );
        return _req->responseRec()->error( PHILOGERR, PHIRC_HTTP_NOT_FOUND, tr( "Could not access CSS file." ) );
    }
    _req->responseRec()->setContentType( BL( "text/css" ) );
    if ( fi.lastModified().toUTC() <= _req->ifModifiedSince() ) return _req->responseRec()->notModified();
    _req->responseRec()->setFileName( path );
    _req->responseRec()->setContentLength( fi.size() );
    _req->responseRec()->setHeader( BL( "Last-Modified" ), _req->responseRec()->timeEncoded( fi.lastModified() ) );
    _req->responseRec()->setHeader( BL( "Cache-Control" ), BL( "public" ) );
}

void PHISProcessor::genJS() const
{
    QString path=_req->getValue( SL( "j" ) );
    path=_req->tmpDir()+SL( "/js/" )+path+SL( ".js" );
    if ( Q_UNLIKELY( !QFile::exists( path ) ) ) {
        _req->responseRec()->log( PHILOGERR, PHIRC_IO_FILE_ACCESS_ERROR,
            tr( "Could not access JavaScript file '%1'." ).arg( path ) );
        return _req->responseRec()->error( PHILOGERR, PHIRC_HTTP_NOT_FOUND, tr( "JavaScript file not found." ) );
    }
    QFileInfo fi( path );
    if ( Q_UNLIKELY( !fi.isReadable() ) ) {
        _req->responseRec()->log( PHILOGERR, PHIRC_IO_FILE_ACCESS_ERROR,
            tr( "Could not access JavaScript file '%1'." ).arg( path ) );
        return _req->responseRec()->error( PHILOGERR, PHIRC_HTTP_NOT_FOUND, tr( "Could not access JavaScript file." ) );
    }
    _req->responseRec()->setContentType( BL( "text/javascript" ) );
    if ( fi.lastModified().toUTC() <= _req->ifModifiedSince() ) return _req->responseRec()->notModified();
    _req->responseRec()->setFileName( path );
    _req->responseRec()->setContentLength( fi.size() );
    _req->responseRec()->setHeader( BL( "Last-Modified" ), _req->responseRec()->timeEncoded( fi.lastModified() ) );
    _req->responseRec()->setHeader( BL( "Cache-Control" ), "public" );
}

void PHISProcessor::genScripts() const
{
    createJS( L1( "phibase.js" ) );
    createJS( L1( "jquery.js" ) );
    createJS( L1( "jqueryiefix.js" ) );
    createJS( L1( "excanvas.js" ) );
    createJS( L1( "ui-core.js" ) );
    createJS( L1( "ui-datepicker.js" ) );
    createJS( L1( "ui-progressbar.js" ) );
    createJS( L1( "ui-effects.js" ) );
    createJS( L1( "ui-button.js" ) );
    //createJS( L1( "ui-mouse.js" ) ); // included in ui-core
    //createJS( L1( "ui-draggable.js" ) ); // included in ui-core
    //createJS( L1( "ui-droppable.js" ) ); // included in ui-core
    //createJS( L1( "ui-widget.js" ) ); // included in ui-core
    createUiCSS();
}

void PHISProcessor::createJS( const QString &name ) const
{
    QFile src( L1( ":/" )+name );
    if ( !src.open( QIODevice::ReadOnly ) ) {
        _req->responseRec()->log( PHILOGERR, PHIRC_IO_FILE_CREATION_ERROR,
            tr( "Could not read JavaScript data '%1'." ).arg( name ) );
        return;
    }
    QFile f( _req->tmpDir()+L1( "/js/" )+name );
    if ( !f.open( QIODevice::WriteOnly ) ) {
        _req->responseRec()->log( PHILOGERR, PHIRC_IO_FILE_CREATION_ERROR,
            tr( "Could not write JavaScript file '%1'." ).arg( f.fileName() ) );
        return;
    }
    if ( name==L1( "phibase.js" ) ) {
        QString str=QString::fromUtf8( src.readAll() );
        // minimize:
        str.replace( QRegExp( L1( " {2,}" ) ), QString() );
        str.replace( L1( " (" ), L1( "(" ) );
        str.replace( L1( "( " ), L1( "(" ) );
        str.replace( L1( " )" ), L1( ")" ) );
        str.replace( L1( ") " ), L1( ")" ) );
        str.replace( L1( ", " ), L1( "," ) );
        str.replace( L1( " {" ), L1( "{" ) );
        str.replace( L1( "{ " ), L1( "{" ) );
        str.replace( L1( " }" ), L1( "}" ) );
        str.replace( L1( "} " ), L1( "}" ) );
#ifndef PHIDEBUG
        str.replace( QRegExp( L1( "\n{1,1}" ) ), QString() );
#endif
        f.write( str.toLatin1() );
    } else f.write( src.readAll() );
}

void PHISProcessor::createUiCSS() const
{
    QFile dest( _req->tmpDir()+L1( "/css/ui-core.css" ) );
    if ( !dest.open( QIODevice::WriteOnly ) ) {
        _req->responseRec()->log( PHILOGERR, PHIRC_IO_FILE_CREATION_ERROR,
            tr( "Could not write to UI CSS file '%1'." ).arg( dest.fileName() ) );
        return;
    }
    QStringList list;
    list << L1( "ui-core.css" ) << L1( "ui-datepicker.css" ) << L1( "ui-button.css" );
    foreach( const QString css, list ) {
        QFile src( L1( ":/" )+css );
        if ( !src.open( QIODevice::ReadOnly ) ) {
            _req->responseRec()->log( PHILOGERR, PHIRC_IO_FILE_CREATION_ERROR,
                tr( "Could not read internal CSS data '%1'." ).arg( src.fileName() ) );
            return;
        }
        dest.write( src.readAll() );
    }
    QByteArray out=BL( "\n.phi .ui-datepicker{width:100%;height:100%;padding:.2em "
        ".2em 0;display:none;font-size:80%;min-width:240px;}\n"
        "div.ui-datepicker{width:auto;padding:.2em .2em 0;display:none;font-size:90%}\n" );
    dest.write( out );
}
