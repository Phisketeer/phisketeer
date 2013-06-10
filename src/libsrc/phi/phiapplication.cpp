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
#include "phiapplication.h"
#include "phisysinfo.h"
#include "phi.h"

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
        qDebug( "FileOpenevent" );
        emit openFileRequest( dynamic_cast<QFileOpenEvent*>(e)->file() );
    }
#endif
    return QApplication::event( e );
}

PHIApplication* PHIApplication::_instance=0;

PHIApplication::PHIApplication(int &argc, char **argv, const char *name , const char *version )
    : QObject( 0 ), _settings( 0 )
{
    Q_ASSERT( argc>0 );
    _instance=this;
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
    Q_UNUSED( argv )
    _settings=new QSettings( QSettings::NativeFormat, QSettings::SystemScope, org, phis, this );
    rootDir=QFileInfo( qAppFileName() ).dir();
    rootDir.cdUp();
    _rootPath=rootDir.canonicalPath();
    _binPath=_rootPath+QLatin1String( "/bin" );
    _libPath=_binPath;
    _pluginsPath=_binPath+QLatin1String( "/plugins" );
    _modulesPath=_pluginsPath+QLatin1String( "/modules" );
    _itemsPath=_pluginsPath+QLatin1String( "/items" );
    _tsPath=_rootPath+QLatin1String( "/ts" );
    _serverBin=_binPath+QLatin1String( "/phis.exe" );
#elif defined Q_OS_MAC
    _settings=new QSettings( domain, phis, this );
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
    _serverBin=_binPath+QLatin1String( "/phis" );
#elif defined Q_OS_UNIX
    QString conf;
    if ( PHISysInfo::effUserId()==0 ) conf=QLatin1String( "/etc/" )+org+QLatin1String( "/phis.conf" );
    else conf=QStandardPaths::writableLocation( QStandardPaths::HomeLocation )
        +QLatin1String( "/.config/" )+org+QLatin1String( "/phis.conf" );
    _settings=new QSettings( conf, QSettings::IniFormat, this );
    if ( name!=QLatin1String( "mod_phi" ) ) { // usual application
        QFileInfo fi( QString::fromLatin1( "/proc/%1/exe" ).arg( PHISysInfo::processId() ) );
        if ( fi.exists() && fi.isSymLink() ) rootDir=fi.dir();
        else rootDir=QFileInfo( QString::fromLocal8Bit( argv[0] ) ).dir();
    } else { // Apache module
        conf="/opt/phi/bin"; // fallback
        _binPath=_settings->value( QLatin1String( "BinDir" ), conf ).toString();
        _pluginsPath=_settings->value( QLatin1String( "PluginsPath" ), QString() ).toString();
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
#else
#error Unsupported system
#endif

    Q_ASSERT( _settings );
#ifdef PHIDEBUG
    qWarning( "Application name: %s", name );
    qWarning( "Version: %s", version );
    qWarning( "Settings file: %s", qPrintable( _settings->fileName() ) );
    qWarning( "Plug-in dir: %s", qPrintable( _pluginsPath ) );
    qWarning( "Items dir: %s", qPrintable( _itemsPath ) );
    qWarning( "Server bin: %s", qPrintable( _serverBin ) );
    qWarning( "TS dir: %s", qPrintable( _tsPath ) );
    qWarning( "Root dir: %s", qPrintable( _rootPath ) );
    qWarning( "Lib dir: %s", qPrintable( _libPath ) );
#endif
    // Fix QPA (Qt5 needs to find platform plugins)
    qputenv( "QT_PLUGIN_PATH", _pluginsPath.toLocal8Bit() );
    if ( name=="mod_phi" ) { // Apache module
        _app=new QGuiApplication( argc, argv );
    } else {
        PHIPrivateApplication *pApp=new PHIPrivateApplication( argc, argv );
        _app=pApp;
        connect( pApp, SIGNAL(openFileRequest(QString)), this, SIGNAL(openFileRequest(QString)) );
    }
    Q_ASSERT( _app );
    _app->setApplicationName( QString::fromLatin1( name ) );
    _app->setApplicationVersion( QString::fromLatin1( version ) );
    _app->setOrganizationDomain( domain );
    _app->setOrganizationName( org );
    _app->addLibraryPath( _pluginsPath );
    loadTranslations();

    qRegisterMetaTypeStreamOperators<PHIRectHash>("PHIRectHash");
    qRegisterMetaTypeStreamOperators<PHIByteArrayList>("PHIByteArrayList");
    qRegisterMetaTypeStreamOperators<PHIImageHash>("PHIImageHash");
    qRegisterMetaTypeStreamOperators<QGradientStops>("QGradientStops");
}

PHIApplication::~PHIApplication()
{
    delete _app;
    _app=0;
    _instance=0;
    qDebug( "PHIApplication::~PHIApplication()" );
}

void PHIApplication::loadTranslations()
{
    Q_ASSERT( _app );
    QString lang=QLocale::system().name();
    QSettings s;
    lang=s.value( QStringLiteral( "Language" ), lang ).toString();
    QLocale::setDefault( QLocale( lang ) );
    qDebug( "SETTING LOCALE %s", qPrintable( lang ) );
    QTranslator *tr=new QTranslator( _app );
    if ( !tr->load( QLatin1String( "qt_" )+lang, _tsPath ) ) {
        qDebug( "Could not load %s in %s", qPrintable( QLatin1String( "qt_" )+lang ), qPrintable( _tsPath ) );
        delete tr;
    } else {
        _app->installTranslator( tr );
    }
    tr=new QTranslator( _app );
    if ( !tr->load( QLatin1String( "qtbase_" )+lang, _tsPath ) ) {
        qDebug( "Could not load %s in %s", qPrintable( QLatin1String( "qtbase_" )+lang ), qPrintable( _tsPath ) );
        delete tr;
    } else {
        _app->installTranslator( tr );
    }
    tr=new QTranslator( _app );
    if ( !tr->load( QLatin1String( "qtscript_" )+lang, _tsPath ) ) {
        qDebug( "Could not load %s in %s", qPrintable( QLatin1String( "qtscript_" )+lang ), qPrintable( _tsPath ) );
        delete tr;
    } else {
        _app->installTranslator( tr );
    }
    tr=new QTranslator( _app );
    if ( !tr->load( QLatin1String( "qtmultimedia_" )+lang, _tsPath ) ) {
        qDebug( "Could not load %s in %s", qPrintable( QLatin1String( "qtmultimedia_" )+lang ), qPrintable( _tsPath ) );
        delete tr;
    } else {
        _app->installTranslator( tr );
    }
    tr=new QTranslator( _app );
    if ( !tr->load( QLatin1String( "phia_" )+lang, _tsPath ) ) {
        qDebug( "Could not load %s in %s", qPrintable( QLatin1String( "phia_" )+lang ), qPrintable( _tsPath ) );
        delete tr;
    } else {
        _app->installTranslator( tr );
    }
    tr=new QTranslator( _app );
    if ( !tr->load( _app->applicationName().toLower()+QLatin1Char( '_' )+lang, _tsPath ) ) {
        qDebug( "Could not load %s in %s", qPrintable( _app->applicationName().toLower()+QLatin1Char( '_' )+lang ), qPrintable( _tsPath ) );
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
