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
#include <QSizePolicy>
#include <QMessageBox>
#include "phiapplication.h"
#include "phisysinfo.h"
#include "phi.h"
#include "phiitemfactory.h"
#include "phidatasources.h"
#include "phinetmanager.h"

#ifdef PHIWEBKIT
#include <QWebSettings>
#endif

#ifdef Q_OS_WIN
extern QString qAppFileName();
#endif

PHIPrivateApplication::PHIPrivateApplication( int &argc, char **argv )
    : QApplication( argc, argv )
{
    qDebug( "PHIPrivateApplication::PHIPrivateApplication()" );
}

PHIPrivateApplication::~PHIPrivateApplication()
{
    qDebug( "PHIPrivateApplication::~PHIPrivateApplication()" );
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
    : QObject( 0 ), _app( 0 ), _settings( 0 ), _serverSettings( 0 ), _type( type )
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
    Q_UNUSED( phis )
    QString conf;
    if ( PHISysInfo::effUserId()==0 ) conf=L1( "/etc/phi/phis.conf" );
    else conf=QStandardPaths::writableLocation( QStandardPaths::HomeLocation )+L1( "/.phi/phis.conf" );
    _serverSettings=new QSettings( conf, QSettings::IniFormat, this );
    if ( type!=ApacheModule ) { // normal application
        QFileInfo fi( QString::fromLatin1( "/proc/%1/exe" ).arg( PHISysInfo::processId() ) );
        if ( fi.exists() && fi.isSymLink() ) _binPath=fi.canonicalPath();
        else _binPath=QFileInfo( QString::fromLocal8Bit( argv[0] ) ).canonicalPath();
        rootDir.setPath( _binPath );
        rootDir.cdUp();
        _rootPath=rootDir.canonicalPath();
    } else { // Apache module
        conf=L1( "/opt/phi" ); // fallback
        _rootPath=_serverSettings->value( L1( "RootDir" ), conf ).toString();
        _binPath=_rootPath+L1( "/bin" );
        conf=_rootPath+L1( "/plugins" ); // fallback
        _usrDocPath=L1( "/root/Documents" );
    }
    _pluginsPath=_rootPath+L1( "/plugins" );
    _modulesPath=_pluginsPath+L1( "/modules" );
    _itemsPath=_pluginsPath+L1( "/items" );
    _libPath=_rootPath+L1( "/lib" );
    _tsPath=_rootPath+L1( "/ts" );
    _serverBin=_binPath+L1( "/phis" );
    _appBin=_binPath+L1( "/phiapp" );
    if ( PHISysInfo::effUserId()==0 )  _dataPath=L1( "/var/phis/" );
    else _dataPath=QStandardPaths::writableLocation( QStandardPaths::HomeLocation )+L1( "/.phi/phis" );
#else
#error Unsupported system
#endif

    Q_ASSERT( _serverSettings );
    // Fix for QPA (Qt5 needs to find platform plugins - better way could be qt.conf)
    _pluginsPath=_serverSettings->value( L1( "PluginsPath" ), _pluginsPath ).toString();
    qputenv( "QT_PLUGIN_PATH", _pluginsPath.toUtf8() );

    if ( type==ApacheModule || type==Service ) {
#ifdef Q_OS_MAC
        qputenv( "QT_MAC_DISABLE_FOREGROUND_APPLICATION_TRANSFORM", "1" );
#elif defined Q_OS_LINUX
        QByteArray fdir=_serverSettings->value( L1( "FontDir" ), _libPath+L1( "/fonts" ) ).toString().toUtf8();
        qputenv( "QT_QPA_FONTDIR", fdir );
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
            +L1( "/.phi/" )+objectName()+L1( ".conf" );
        _settings=new QSettings( s, QSettings::IniFormat, this );
        qobject_cast<QApplication*>(_app)->setStyle( L1( "fusion" ) );
#else
#error Unsupported system
#endif
    }

    Q_ASSERT( _settings );
    _cachePath=QStandardPaths::writableLocation( QStandardPaths::CacheLocation );
#ifdef Q_OS_WIN
    _dataPath=_settings->value( L1( "BaseDir" ), _rootPath+L1( "/data" ) ).toString();
    _tmpPath=_dataPath+L1( "/temp" );
#elif defined Q_OS_MAC
    _tmpPath=QStandardPaths::writableLocation( QStandardPaths::TempLocation )+QLatin1Char( '/' )+objectName();
#else
    _tmpPath=L1( "/tmp/" )+objectName();
#endif
    _tmpPath=_settings->value( L1( "TempDir" ), _tmpPath ).toString();
    if ( type==Service || type==ApacheModule ) {
        _cachePath=_tmpPath+L1( "/cache" );
        _settings->beginGroup( PHI::defaultString() );
        _dataPath=_settings->value( L1( "BaseDir" ), _dataPath ).toString();
        _settings->endGroup();
    } else {
        if ( !QFileInfo( _cachePath ).exists() ) QDir( _cachePath ).mkpath( _cachePath );
        if ( !QFileInfo( _tmpPath ).exists() ) QDir( _tmpPath ).mkpath( _tmpPath );
    }
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
    _rootPath=L1( PHIDIR );
    _binPath=L1( PHIDIR )+L1( "/bin" );
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

#endif
#ifdef PHIEMBEDEDSERVER
    _serverBin=QString();
#endif
#ifdef PHIEMBEDEDAPP
    _appBin=QString();
#endif
    _pluginsPath=QDir::toNativeSeparators( _pluginsPath );
    _itemsPath=QDir::toNativeSeparators( _itemsPath );
    _modulesPath=QDir::toNativeSeparators( _modulesPath );
    _binPath=QDir::toNativeSeparators( _binPath );
    _serverBin=QDir::toNativeSeparators( _serverBin );
    _appBin=QDir::toNativeSeparators( _appBin );
    _tsPath=QDir::toNativeSeparators( _tsPath );
    _rootPath=QDir::toNativeSeparators( _rootPath );
    _libPath=QDir::toNativeSeparators( _libPath );
    _tmpPath=QDir::toNativeSeparators( _tmpPath );
    _cachePath=QDir::toNativeSeparators( _cachePath );
    _dataPath=QDir::toNativeSeparators( _dataPath );
    _usrDocPath=QDir::toNativeSeparators( _usrDocPath );

#ifdef PHIPRINTDIRS
    qWarning( "App name:       %s", name );
    qWarning( "Version:        %s", version );
    qWarning( "Settings file:  %s", qPrintable( _settings->fileName() ) );
    qWarning( "Server setings: %s", qPrintable( _serverSettings->fileName() ) );
    qWarning( "Root dir:       %s", qPrintable( _rootPath ) );
    qWarning( "Plug-in dir:    %s", qPrintable( _pluginsPath ) );
    qWarning( "Items dir:      %s", qPrintable( _itemsPath ) );
    qWarning( "Modules dir:    %s", qPrintable( _modulesPath ) );
    qWarning( "Bin dir:        %s", qPrintable( _binPath ) );
    qWarning( "Server bin:     %s", qPrintable( _serverBin ) );
    qWarning( "PhiApp bin:     %s", qPrintable( _appBin ) );
    qWarning( "TS dir:         %s", qPrintable( _tsPath ) );
    qWarning( "Lib dir:        %s", qPrintable( _libPath ) );
    qWarning( "Tmp dir:        %s", qPrintable( _tmpPath ) );
    qWarning( "Cache dir:      %s", qPrintable( _cachePath ) );
    qWarning( "Data dir:       %s", qPrintable( _dataPath ) );
    qWarning( "User doc dir:   %s", qPrintable( _usrDocPath ) );
#endif

    Q_ASSERT( _app );
    _app->setApplicationName( objectName() );
    _app->setApplicationVersion( QString::fromLatin1( version ) );
    _app->setOrganizationDomain( domain );
    _app->setOrganizationName( org );
    _app->addLibraryPath( _pluginsPath );
    loadTranslations();
    PHINetManager::instance();
    new PHIItemFactory( _itemsPath, this );

#ifdef PHIWEBKIT
    QWebSettings *ws=QWebSettings::globalSettings();
    ws->setAttribute( QWebSettings::PluginsEnabled, true );
    ws->setAttribute( QWebSettings::JavascriptCanOpenWindows, true );
    ws->setAttribute( QWebSettings::LocalContentCanAccessRemoteUrls, true );
    ws->setAttribute( QWebSettings::JavascriptEnabled, true );
#endif
}

PHIApplication::~PHIApplication()
{
    delete PHINetManager::instance();
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
    _lang=_settings->value( L1( "Language" ), _lang ).toString();
    _settings->setValue( L1( "Language" ), _lang );
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
    if ( !tr->load( L1( "phi_" )+_lang, _tsPath ) ) {
        qDebug( "Could not load %s in %s", qPrintable( L1( "phi_" )+_lang ), qPrintable( _tsPath ) );
        delete tr;
    } else {
        _app->installTranslator( tr );
    }
    tr=new QTranslator( _app );
    if ( !tr->load( L1( "phis_" )+_lang, _tsPath ) ) {
        qDebug( "Could not load %s in %s", qPrintable( L1( "phis_" )+_lang ), qPrintable( _tsPath ) );
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
    qWarning() << "phiapp start" << _serverBin;
#ifdef PHIEMBEDEDSERVER
    return false;
#else
    if ( !QFile::exists( _serverBin ) ) return false;
    qWarning() << "phiapp start" << _serverBin;
    QProcess proc;
#ifdef Q_OS_WIN
    proc.execute( _serverBin, QStringList() << L1( "-i" ) );
#endif
    if ( proc.execute( _serverBin, QStringList() )!=0 ) return false;
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
    if ( proc.execute( _serverBin, QStringList() << L1( "-t" ) )!=0 ) return false;
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
