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
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QStandardPaths>
#include <QFileInfo>
#include <QDir>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QProcess>
#include <QLocale>
#include <QTranslator>
#include <QEvent>
#include <QFileOpenEvent>
#include "phiapplication.h"
#include "phisysinfo.h"
#include "phi.h"
#include "phiitemfactory.h"
#include "phidatasources.h"

#ifdef Q_OS_WIN
extern QString qAppFileName();
#endif

PHIPrivateApplication::PHIPrivateApplication( int &argc, char **argv )
    : QApplication( argc, argv )
{
    qDebug( "PHIPrivateApplication::PHIPrivateApplication()" );
}

bool PHIPrivateApplication::event( QEvent *e )
{
#ifdef Q_OS_MAC
    if ( e->type()==QEvent::FileOpen ) {
        qDebug( "FileOpenEvent" );
        emit openFileRequest( dynamic_cast<QFileOpenEvent*>(e)->file() );
    }
#endif
    return QApplication::event( e );
}

PHIApplication* PHIApplication::_instance=0;

PHIApplication::PHIApplication( int &argc, char **argv, const char *name , const char *version, Type type )
    : QObject( 0 ), _app( 0 ), _settings( 0 ), _serverSettings( 0 )
{
    qDebug( "PHIApplication::PHIApplication()" );
    Q_ASSERT( argc>0 );
    _instance=this;
    setObjectName( QString::fromLatin1( name ) );
    const QString phis=QLatin1String( "phis" );
    QDir rootDir;

#ifdef PHIAPPSTORE
    // needed for Apple AppStore:
    const QString domain=QString::fromLatin1( "phisys.com" );
    const QString org=QString::fromLatin1( "Phisys AG" );
#else
    const QString domain=QString::fromLatin1( PHIDOM );
    const QString org=QString::fromLatin1( PHIORG );
#endif

#ifdef Q_OS_WIN
    _serverSettings=new QSettings( QSettings::NativeFormat, QSettings::SystemScope, org, phis, this );
    rootDir=QFileInfo( qAppFileName() ).dir();
    rootDir.cdUp();
    _rootPath=rootDir.canonicalPath();
    _binPath=_rootPath+QLatin1String( "/bin" );
    _libPath=_binPath;
    _pluginsPath=_binPath+QLatin1String( "/plugins" );
    _modulesPath=_pluginsPath+QLatin1String( "/modules" );
    _itemsPath=_pluginsPath+QLatin1String( "/items" );
    _tsPath=_rootPath+QLatin1String( "/ts" );
    _serverBin=_binPath+QLatin1String( "/Phis.exe" );
    _appBin=_binPath+QLatin1String( "/PhiApp.exe" );
#elif defined Q_OS_MAC
    _serverSettings=new QSettings( domain, phis, this );
    rootDir=QFileInfo( QString::fromLocal8Bit( argv[0] ) ).dir(); // bundleID/Contents/MacOS
    _binPath=rootDir.canonicalPath();
    rootDir.cdUp(); // bundleID/Contents
    _libPath=rootDir.canonicalPath()+QLatin1String( "/Frameworks" );
    rootDir.cdUp(); // bundleID
    _rootPath=rootDir.canonicalPath();
    _pluginsPath=_rootPath+QLatin1String( "/Contents/PlugIns" );
    _modulesPath=_pluginsPath+QLatin1String( "/modules" );
    _itemsPath=_pluginsPath+QLatin1String( "/items" );
    _tsPath=_rootPath+QLatin1String( "/Contents/Resources/ts" );
    _serverBin=_binPath+QLatin1String( "/Phis" );
    _appBin=_binPath+QLatin1String( "/PhiApp" );
#elif defined Q_OS_LINUX
    QString conf;
    if ( PHISysInfo::effUserId()==0 ) conf=QLatin1String( "/etc/phi/phis.conf" );
    else conf=QStandardPaths::writableLocation( QStandardPaths::HomeLocation )
        +QLatin1String( "/.phi/phis.conf" );
    _serverSettings=new QSettings( conf, QSettings::IniFormat, this );
    if ( type!=ApacheModule ) { // normal application
        qFatal( "not implemented" );
        QFileInfo fi( QString::fromLatin1( "/proc/%1/exe" ).arg( PHISysInfo::processId() ) );
        if ( fi.exists() && fi.isSymLink() ) rootDir=fi.dir();
        else rootDir=QFileInfo( QString::fromLocal8Bit( argv[0] ) ).dir();
    } else { // Apache module
        conf="/opt/phi/bin"; // fallback
        _binPath=_serverSettings->value( QLatin1String( "BinDir" ), conf ).toString();
        conf="/opt/phi/plugins"; // fallback
        _pluginsPath=_serverSettings->value( QLatin1String( "PluginsPath" ), conf ).toString();
        rootDir.setPath( _binPath );
        rootDir.cdUp();
    }
    _rootPath=rootDir.canonicalPath();
    if ( _pluginsPath.isEmpty() ) _pluginsPath=_rootPath+QLatin1String( "/plugins" );
    _modulesPath=_pluginsPath+QLatin1String( "/modules" );
    _itemsPath=_pluginsPath+QLatin1String( "/items" );
    _libPath=_rootPath+QLatin1String( "/lib" );
    _tsPath=_rootPath+QLatin1String( "/ts" );
    _serverBin=_binPath+QLatin1String( "/phis" );
    _appBin=_binPath+QLatin1String( "/phiapp" );
#else
#error Unsupported system
#endif

    Q_ASSERT( _serverSettings );
    // Fix QPA (Qt5 needs to find platform plugins - better way could be qt.conf)
    _pluginsPath=_serverSettings->value( QLatin1String( "PluginsPath" ), _pluginsPath ).toString();
    qputenv( "QT_PLUGIN_PATH", _pluginsPath.toLatin1() );

    if ( type==ApacheModule ) {
        QStringList argList;
        argList << _libPath+QLatin1String( "/libmod_phi.so" );
        argList << QStringLiteral( "-platform" ) << QStringLiteral( "offscreen" );
        int p_argc=argList.size();
        QVector<char *> p_argv( p_argc );
        QList<QByteArray> argvData;
        for ( int i=0; i<p_argc; ++i ) argvData.append( argList.at( i ).toLatin1() );
        for ( int i=0; i<p_argc; ++i ) p_argv[i]=argvData[i].data();
        _app=new QGuiApplication( p_argc, p_argv.data() );
        delete _settings;
        _settings=_serverSettings;
    } else if ( type==Service ) {
#ifdef Q_OS_MAC
        qputenv( "QT_MAC_DISABLE_FOREGROUND_APPLICATION_TRANSFORM", "1" );
#endif
        _app=new QGuiApplication( argc, argv );
        delete _settings;
        _settings=_serverSettings;
    } else {
        PHIPrivateApplication *pApp=new PHIPrivateApplication( argc, argv );
        connect( pApp, &PHIPrivateApplication::openFileRequest,
            this, &PHIApplication::openFileRequest, Qt::QueuedConnection );
        _app=pApp;
#ifdef Q_OS_MAC
        _settings=new QSettings( QSettings::UserScope, domain, objectName(), this );
#elif defined Q_OS_WIN
        _settings=new QSettings( QSettings::UserScope, org, objectName(), this );
#elif defined Q_OS_LINUX
        QString s=QStandardPaths::writableLocation( QStandardPaths::HomeLocation )
            +"/.phi/"+objectName()+".conf";
        _settings=new QSettings( s, QSettings::IniFormat, this );
#else
#error Unsupported system
#endif
    }

    Q_ASSERT( _settings );
    Q_ASSERT( _app );
    _app->setApplicationName( objectName() );
    _app->setApplicationVersion( QString::fromLatin1( version ) );
    _app->setOrganizationDomain( domain );
    _app->setOrganizationName( org );
    _app->addLibraryPath( _pluginsPath );

    _cachePath=QStandardPaths::writableLocation( QStandardPaths::CacheLocation );
    _tmpPath=QStandardPaths::writableLocation( QStandardPaths::TempLocation )
        +QLatin1Char( '/' )+objectName();
    if ( !QFileInfo( _cachePath ).exists() ) QDir( _cachePath ).mkpath( _cachePath );
    if ( !QFileInfo( _tmpPath ).exists() ) QDir( _tmpPath ).mkpath( _tmpPath );

    qRegisterMetaTypeStreamOperators<PHIRectHash>("PHIRectHash");
    qRegisterMetaTypeStreamOperators<PHIByteArrayList>("PHIByteArrayList");
    qRegisterMetaTypeStreamOperators<PHIImageHash>("PHIImageHash");
    qRegisterMetaTypeStreamOperators<QGradientStops>("QGradientStops");

#ifdef PHIDEBUG
    _pluginsPath=QLatin1String( PHIDIR )+QLatin1String( "/plugins" );
    _itemsPath=_pluginsPath+QLatin1String( "/items" );
    _serverBin=QLatin1String( PHIDIR )+QLatin1String( "/bin/Phisd" );
    _appBin=QLatin1String( PHIDIR )+QLatin1String( "/bin/PhiAppd" );
    _tsPath=QLatin1String( PHIDIR )+QLatin1String( "/src/ts" );
    _libPath=QLatin1String( PHIDIR )+QLatin1String( "/lib" );
#ifdef Q_OS_WIN
    _libPath=QLatin1String( PHIDIR )+QLatin1String( "/bin" );
    _serverBin=QLatin1String( PHIDIR )+QLatin1String( "/bin/Phisd.exe" );
    _appBin=QLatin1String( PHIDIR )+QLatin1String( "/bin/PhiAppd.exe" );
#elif defined Q_OS_LINUX
    _serverBin=QLatin1String( PHIDIR )+QLatin1String( "/bin/phis_debug" );
    _appBin=QLatin1String( PHIDIR )+QLatin1String( "/bin/phiapp_debug" );
#endif
    qWarning( "Application name: %s", name );
    qWarning( "Version: %s", version );
    qWarning( "Settings file: %s", qPrintable( _settings->fileName() ) );
    qWarning( "Server settings file: %s", qPrintable( _serverSettings->fileName() ) );
    qWarning( "Plug-in dir: %s", qPrintable( _pluginsPath ) );
    qWarning( "Items dir: %s", qPrintable( _itemsPath ) );
    qWarning( "Server bin: %s", qPrintable( _serverBin ) );
    qWarning( "PhiApp bin: %s", qPrintable( _appBin ) );
    qWarning( "TS dir: %s", qPrintable( _tsPath ) );
    qWarning( "Root dir: %s", qPrintable( _rootPath ) );
    qWarning( "Lib dir: %s", qPrintable( _libPath ) );
    qWarning( "Tmp dir: %s", qPrintable( _tmpPath ) );
    qWarning( "Cache dir: %s", qPrintable( _cachePath ) );
#endif
    _usrDocPath=QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation );
    loadTranslations();
    new PHIItemFactory( _itemsPath, this );
}

PHIApplication::~PHIApplication()
{
    delete _app;
    _app=0;
    _instance=0;
    _settings=0;
    _serverSettings=0;
    qDebug( "PHIApplication::~PHIApplication()" );
}

void PHIApplication::invalidate()
{
    PHIItemFactory::instance()->invalidate();
}

void PHIApplication::loadTranslations()
{
    Q_ASSERT( _app );
    _lang=QLocale::system().name();
    Q_ASSERT( _settings );
    _lang=_settings->value( QStringLiteral( "Language" ), _lang ).toString();
    QLocale::setDefault( QLocale( _lang ) );
    qDebug( "SETTING LOCALE %s", qPrintable( _lang ) );
    QTranslator *tr=new QTranslator( _app );
    if ( !tr->load( QLatin1String( "qt_" )+_lang, _tsPath ) ) {
        qDebug( "Could not load %s in %s", qPrintable( QLatin1String( "qt_" )+_lang ), qPrintable( _tsPath ) );
        delete tr;
    } else {
        _app->installTranslator( tr );
    }
    tr=new QTranslator( _app );
    if ( !tr->load( QLatin1String( "qtbase_" )+_lang, _tsPath ) ) {
        qDebug( "Could not load %s in %s", qPrintable( QLatin1String( "qtbase_" )+_lang ), qPrintable( _tsPath ) );
        delete tr;
    } else {
        _app->installTranslator( tr );
    }
    tr=new QTranslator( _app );
    if ( !tr->load( QLatin1String( "qtscript_" )+_lang, _tsPath ) ) {
        qDebug( "Could not load %s in %s", qPrintable( QLatin1String( "qtscript_" )+_lang ), qPrintable( _tsPath ) );
        delete tr;
    } else {
        _app->installTranslator( tr );
    }
    tr=new QTranslator( _app );
    if ( !tr->load( QLatin1String( "qtmultimedia_" )+_lang, _tsPath ) ) {
        qDebug( "Could not load %s in %s", qPrintable( QLatin1String( "qtmultimedia_" )+_lang ), qPrintable( _tsPath ) );
        delete tr;
    } else {
        _app->installTranslator( tr );
    }
    tr=new QTranslator( _app );
    if ( !tr->load( QLatin1String( "phia_" )+_lang, _tsPath ) ) {
        qDebug( "Could not load %s in %s", qPrintable( QLatin1String( "phia_" )+_lang ), qPrintable( _tsPath ) );
        delete tr;
    } else {
        _app->installTranslator( tr );
    }
    tr=new QTranslator( _app );
    if ( !tr->load( _app->applicationName().toLower()+QLatin1Char( '_' )+_lang, _tsPath ) ) {
        qDebug( "Could not load %s in %s", qPrintable( _app->applicationName().toLower()+QLatin1Char( '_' )+_lang ), qPrintable( _tsPath ) );
    } else {
        _app->installTranslator( tr );
    }
}

// returns -1 on error, 0 not running, 1 running
int PHIApplication::checkPhisService()
{
    if ( !QFile::exists( _serverBin ) ) return -1;
#ifdef Q_OS_MAC
    QSettings *s=_settings;
    // Sandboxing in Mac OS X requires a little hack: we check if the server responds
    // to a QNetworkRequest to determine if the phis service is running
    s->beginGroup( PHI::defaultString() );
    QUrl url( QStringLiteral( "http://localhost/phi.phis?ping=1" ) );
    url.setPort( s->value( QStringLiteral( "ListenerPort" ), 8080 ).toInt() );
    s->endGroup();
    QNetworkRequest req;
    req.setUrl( url );
    QNetworkAccessManager mg;
    QNetworkReply *rep=mg.get( req );
    while ( !rep->isFinished() ) {
        _app->processEvents();
    }
    rep->deleteLater();
    if ( rep->error()!=QNetworkReply::NoError ) {
        // phis responses with "access denied" to phi.phis?ping=1
        // so we can asume the service is running
        if( rep->error()==QNetworkReply::ContentOperationNotPermittedError ) return 1;
        qWarning( "REPLY %d %s", rep->error(), qPrintable( rep->errorString() ) );
        return 0;
    }
    return 0;
#else // Unix or Windows
    QProcess proc;
    proc.setProcessChannelMode( QProcess::MergedChannels );
    proc.start( _serverBin, QStringList() << QStringLiteral( "-v" ) );
    if ( !proc.waitForStarted() ) return -1;
    if ( !proc.waitForFinished() ) return -1;
    QByteArray arr=proc.readAll();
    qDebug( "start process read all=%s", arr.data() );
    if ( arr.contains( "not running" ) ) return 0;
    return 1;
#endif
}

bool PHIApplication::startPhisService()
{
    if ( !QFile::exists( _serverBin ) ) return false;
#ifdef Q_OS_MAC
/*
#ifdef PHIAPPSTORE
    QSettings *s=app->settings();
    // Sandboxing in Mac OS X requires a little hack: we check if the server responds
    // to a QNetworkRequest to determine if the phis service is running
    s->beginGroup( defaultString() );
    const QString baseDir=s->value( QLatin1String( "BaseDir" ) ).toString();
    s->beginGroup( QLatin1String( "localhost" ) );
    const QString root=s->value( QLatin1String( "DocumentRoot" ), baseDir+QLatin1String( "/localhost" ) ).toString();
    s->endGroup();
    s->endGroup();
#endif
*/
    return QProcess::startDetached( _serverBin );
#else // Unix or Windows
    QProcess proc;
#ifdef Q_OS_WIN
    proc.execute( _serverBin, QStringList() << QStringLiteral( "-i" ) );
#endif
    proc.start( _serverBin );
    if ( !proc.waitForStarted() ) return false;
    if ( !proc.waitForFinished() ) return false;
    return true;
#endif
}

bool PHIApplication::stopPhisService()
{
    if ( !QFile::exists( _serverBin ) ) return false;
#ifdef Q_OS_MAC
    // Sandboxing in Mac OS X requires a little hack: we send
    // a QNetworkRequest to stop the phis service
    QSettings *s=_settings;
    s->beginGroup( PHI::defaultString() );
    QUrl url( QStringLiteral( "http://localhost/phi.phis?stop=1" ) );
    url.setPort( s->value( QStringLiteral( "ListenerPort" ), 8080 ).toInt() );
    s->endGroup();
    QNetworkRequest req;
    req.setUrl( url );
    QNetworkAccessManager mg;
    QNetworkReply *rep=mg.get( req );
    while ( !rep->isFinished() ) {
        _app->processEvents();
    }
    rep->deleteLater();
    if ( rep->error()!=QNetworkReply::NoError ) {
        // phis responses with "access denied" to phi.phis?stop=1
        if( rep->error()==QNetworkReply::ContentOperationNotPermittedError ) return true;
        qDebug( "REPLY %d %s", rep->error(), qPrintable( rep->errorString() ) );
        return false;
    }
    return false;
#else // Unix or Windows
    QProcess proc;
    int res=proc.execute( _serverBin, QStringList() << QStringLiteral( "-t" ) );
    if ( res ) return false;
    return true;
#endif
}

bool PHIApplication::clearPhisServiceCache()
{
    if ( !QFile::exists( _serverBin ) ) return false;
#ifdef Q_OS_MAC
    // sandboxing in Mac OS X requires a little hack: we send
    // a QNetworkRequest to invalidate the phis cache
    QSettings *s=_settings;
    s->beginGroup( PHI::defaultString() );
    QUrl url( QStringLiteral( "http://localhost/phi.phis?invalidate=1" ) );
    url.setPort( s->value( QStringLiteral( "ListenerPort" ), 8080 ).toInt() );
    s->endGroup();
    QNetworkRequest req;
    req.setUrl( url );
    QNetworkAccessManager mg;
    QNetworkReply *rep=mg.get( req );
    while ( !rep->isFinished() ) {
        _app->processEvents();
    }
    rep->deleteLater();
    if ( rep->error()!=QNetworkReply::NoError ) {
        // phis responses with "access denied" to phi.phis?invalidate=1
        if( rep->error()==QNetworkReply::ContentOperationNotPermittedError ) return 1;
        qDebug( "REPLY %d %s", rep->error(), qPrintable( rep->errorString() ) );
        return 0;
    }
    return 0;
#else // Unix or Windows
    QProcess proc;
    int res=proc.execute( _serverBin, QStringList() << QStringLiteral( "-c" ) );
    if ( res ) return false;
    return true;
#endif
}
