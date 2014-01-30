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
#include <QWebSettings>
#include <QSizePolicy>
#include "phiapplication.h"
#include "phisysinfo.h"
#include "phi.h"
#include "phiitemfactory.h"
#include "phidatasources.h"
#include "phinetmanager.h"

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
    const QString phis=L1( "phis" );
    _usrDocPath=QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation );
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
    _binPath=_rootPath+L1( "/bin" );
    _libPath=_binPath;
    _pluginsPath=_binPath+L1( "/plugins" );
    _modulesPath=_pluginsPath+L1( "/modules" );
    _itemsPath=_pluginsPath+L1( "/items" );
    _tsPath=_rootPath+L1( "/ts" );
    _serverBin=_binPath+L1( "/Phis.exe" );
    _appBin=_binPath+L1( "/PhiApp.exe" );
    _dataPath=_rootPath+L1( "/data" );
#elif defined Q_OS_MAC
    _serverSettings=new QSettings( domain, phis, this );
    rootDir=QFileInfo( QString::fromLocal8Bit( argv[0] ) ).dir(); // bundleID/Contents/MacOS
    _binPath=rootDir.canonicalPath();
    rootDir.cdUp(); // bundleID/Contents
    _libPath=rootDir.canonicalPath()+L1( "/Frameworks" );
    rootDir.cdUp(); // bundleID
    _rootPath=rootDir.canonicalPath();
    _pluginsPath=_rootPath+L1( "/Contents/PlugIns" );
    _modulesPath=_pluginsPath+L1( "/modules" );
    _itemsPath=_pluginsPath+L1( "/items" );
    _tsPath=_rootPath+L1( "/Contents/Resources/ts" );
    _serverBin=_binPath+L1( "/Phis" );
    _appBin=_binPath+L1( "/PhiApp" );
    _dataPath=QStandardPaths::writableLocation( QStandardPaths::DataLocation )+L1( "/" )+objectName();
#elif defined Q_OS_LINUX
    QString conf;
    if ( PHISysInfo::effUserId()==0 ) conf=L1( "/etc/phi/phis.conf" );
    else conf=QStandardPaths::writableLocation( QStandardPaths::HomeLocation )+L1( "/.phi/phis.conf" );
    _serverSettings=new QSettings( conf, QSettings::IniFormat, this );
    if ( type!=ApacheModule ) { // normal application
        qFatal( "not implemented" );
        QFileInfo fi( QString::fromLatin1( "/proc/%1/exe" ).arg( PHISysInfo::processId() ) );
        if ( fi.exists() && fi.isSymLink() ) rootDir=fi.dir();
        else rootDir=QFileInfo( QString::fromLocal8Bit( argv[0] ) ).dir();
    } else { // Apache module
        conf="/opt/phi/bin"; // fallback
        _binPath=_serverSettings->value( L1( "BinDir" ), conf ).toString();
        conf="/opt/phi/plugins"; // fallback
        _pluginsPath=_serverSettings->value( L1( "PluginsPath" ), conf ).toString();
        rootDir.setPath( _binPath );
        rootDir.cdUp();
    }
    _rootPath=rootDir.canonicalPath();
    if ( _pluginsPath.isEmpty() ) _pluginsPath=_rootPath+L1( "/plugins" );
    _modulesPath=_pluginsPath+L1( "/modules" );
    _itemsPath=_pluginsPath+L1( "/items" );
    _libPath=_rootPath+L1( "/lib" );
    _tsPath=_rootPath+L1( "/ts" );
    _serverBin=_binPath+L1( "/phis" );
    _appBin=_binPath+L1( "/phiapp" );
    if ( PHISysInfo::effUserId()==0 )  _dataPath=_rootPath+L1( "/var/phis/" );
    else _dataPath=QStandardPaths::writableLocation( QStandardPaths::HomeLocation )+L1( ".phi/phis" );
#else
#error Unsupported system
#endif

    Q_ASSERT( _serverSettings );
    // Fix for QPA (Qt5 needs to find platform plugins - better way could be qt.conf)
    _pluginsPath=_serverSettings->value( L1( "PluginsPath" ), _pluginsPath ).toString();
    qputenv( "QT_PLUGIN_PATH", _pluginsPath.toUtf8() );

    if ( type==ApacheModule ) {
        QStringList argList;
        argList << _libPath+L1( "/libmod_phi.so" );
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
    qRegisterMetaTypeStreamOperators<PHIListChar>("PHIListChar");
    qRegisterMetaTypeStreamOperators<PHIListWord>("PHIListWord");
    qRegisterMetaType<QDateTime>("QDateTime");
    qRegisterMetaType<PHIRC>("PHIRC");
    qRegisterMetaType<PHIWID>("PHIWID");
    qRegisterMetaType<QSizePolicy>("QSizePolicy");

#ifdef PHIDEBUG
    _pluginsPath=L1( PHIDIR )+L1( "/plugins" );
    _itemsPath=_pluginsPath+L1( "/items" );
    _serverBin=L1( PHIDIR )+L1( "/bin/Phisd" );
    _appBin=L1( PHIDIR )+L1( "/bin/PhiAppd" );
    _tsPath=L1( PHIDIR )+L1( "/src/ts" );
    _libPath=L1( PHIDIR )+L1( "/lib" );
    _modulesPath=_libPath+L1( "/modules" );
#ifdef Q_OS_WIN
    _libPath=L1( PHIDIR )+L1( "/bin" );
    _pluginsPath=_libPath+L1( "/plugins" );
    _itemsPath=_pluginsPath+L1( "/items" );
    _modulesPath=_pluginsPath+L1( "/modules" );
    _serverBin=L1( PHIDIR )+L1( "/bin/Phisd.exe" );
    _appBin=L1( PHIDIR )+L1( "/bin/PhiAppd.exe" );
#elif defined Q_OS_LINUX
    _serverBin=L1( PHIDIR )+L1( "/bin/phis_debug" );
    _appBin=L1( PHIDIR )+L1( "/bin/phiapp_debug" );
#endif
#ifdef PHIEMBEDEDSERVER
    _serverBin=QString();
#endif
#ifdef PHIEMBEDEDAPP
    _appBin=QString();
#endif
    qWarning( "Application name: %s", name );
    qWarning( "Version: %s", version );
    qWarning( "Settings file: %s", qPrintable( _settings->fileName() ) );
    qWarning( "Server settings file: %s", qPrintable( _serverSettings->fileName() ) );
    qWarning( "Plug-in dir: %s", qPrintable( _pluginsPath ) );
    qWarning( "Items dir: %s", qPrintable( _itemsPath ) );
    qWarning( "Modules dir: %s", qPrintable( _modulesPath ) );
    qWarning( "Server bin: %s", qPrintable( _serverBin ) );
    qWarning( "PhiApp bin: %s", qPrintable( _appBin ) );
    qWarning( "TS dir: %s", qPrintable( _tsPath ) );
    qWarning( "Root dir: %s", qPrintable( _rootPath ) );
    qWarning( "Lib dir: %s", qPrintable( _libPath ) );
    qWarning( "Tmp dir: %s", qPrintable( _tmpPath ) );
    qWarning( "Cache dir: %s", qPrintable( _cachePath ) );
    qWarning( "Data dir: %s", qPrintable( _dataPath ) );
    qWarning( "User doc dir: %s", qPrintable( _usrDocPath ) );
#endif
    loadTranslations();
    PHINetManager::instance();
    new PHIItemFactory( _itemsPath, this );
    QWebSettings *ws=QWebSettings::globalSettings();
    ws->setAttribute( QWebSettings::PluginsEnabled, true );
    ws->setAttribute( QWebSettings::JavascriptCanOpenWindows, true );
    ws->setAttribute( QWebSettings::LocalContentCanAccessRemoteUrls, true );
    ws->setAttribute( QWebSettings::JavascriptEnabled, true );
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
    if ( !tr->load( L1( "qt_" )+_lang, _tsPath ) ) {
        qDebug( "Could not load %s in %s", qPrintable( L1( "qt_" )+_lang ), qPrintable( _tsPath ) );
        delete tr;
    } else {
        _app->installTranslator( tr );
    }
    tr=new QTranslator( _app );
    if ( !tr->load( L1( "qtbase_" )+_lang, _tsPath ) ) {
        qDebug( "Could not load %s in %s", qPrintable( L1( "qtbase_" )+_lang ), qPrintable( _tsPath ) );
        delete tr;
    } else {
        _app->installTranslator( tr );
    }
    tr=new QTranslator( _app );
    if ( !tr->load( L1( "qtscript_" )+_lang, _tsPath ) ) {
        qDebug( "Could not load %s in %s", qPrintable( L1( "qtscript_" )+_lang ), qPrintable( _tsPath ) );
        delete tr;
    } else {
        _app->installTranslator( tr );
    }
    tr=new QTranslator( _app );
    if ( !tr->load( L1( "qtmultimedia_" )+_lang, _tsPath ) ) {
        qDebug( "Could not load %s in %s", qPrintable( L1( "qtmultimedia_" )+_lang ), qPrintable( _tsPath ) );
        delete tr;
    } else {
        _app->installTranslator( tr );
    }
    tr=new QTranslator( _app );
    if ( !tr->load( L1( "phia_" )+_lang, _tsPath ) ) {
        qDebug( "Could not load %s in %s", qPrintable( L1( "phia_" )+_lang ), qPrintable( _tsPath ) );
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
#ifdef PHIEMBEDEDSERVER
    return -1;
#else
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
#ifdef PHIEMBEDEDSERVER
    return false;
#else
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
#ifdef PHIEMBEDEDSERVER
    return false;
#else
    if ( !QFile::exists( _serverBin ) ) return false;
    QProcess proc;
    int res=proc.execute( _serverBin, QStringList() << L1( "-t" ) );
    if ( res ) return false;
    return true;
#endif
}

bool PHIApplication::clearPhisServiceCache()
{
#ifdef PHIEMBEDEDSERVER
    return false;
#else
    if ( !QFile::exists( _serverBin ) ) return false;
    QProcess proc;
    int res=proc.execute( _serverBin, QStringList() << QStringLiteral( "-c" ) );
    if ( res ) return false;
    return true;
#endif
}
