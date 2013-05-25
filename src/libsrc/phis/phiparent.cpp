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
#include <QVector>
#include <QSettings>
#include <QCryptographicHash>
#include <QMutexLocker>
#include "phiparent.h"
#include "phierror.h"
#include "phispagecache.h"
#include "phisitemcache.h"
#include "phismodulefactory.h"
#include "phiqt5fixes.h"

PHIParent* PHIParent::_instance=0;
//QHash <QString, QString> PHIParent::_tmpDirs=QHash <QString, QString>();

PHIParent* PHIParent::instance( QObject *parent )
{
    if ( _instance ) return _instance;
    _instance=new PHIParent( parent );
    return _instance;
}

PHIParent::PHIParent( QObject *parent )
    : QObject( parent ), _app( 0 ), _internalApp( false )
{
    qDebug( "PHIParent::PHIParent()" );
    if ( !qApp ) { // not set in apache module so instanciate QApplication here
        QStringList argList;
        argList << QStringLiteral( "mod_phi" ) << QStringLiteral( "-platform" )
                << QStringLiteral( "linuxfb" );
        int argc=argList.size();
        QVector<char *> argv( argc );
        QList<QByteArray> argvData;
        for ( int i=0; i<argc; ++i ) argvData.append( argList.at(i).toLocal8Bit() );
        for ( int i=0; i<argc; ++i ) argv[i]=argvData[i].data();
        //_app=new QApplication( argc, argv.data(), false );
        _app=new QGuiApplication( argc, argv.data() );
        _app->setApplicationName( QStringLiteral( "mod_phi" ) );
        _app->setApplicationVersion( PHIS::libVersion() );
        PHI::setupApplication( _app );
        _internalApp=true;
    } else {
        _app=qApp;
        _internalApp=false;
    }

    PHIError::instance( this );
    //PHISPageCache::instance( this );
    PHISItemCache::instance( this );
    _loadedModules=PHISModuleFactory::instance( this )->loadedModules();
    _moduleLoadErrors=PHISModuleFactory::instance( this )->loadErrors();

    //_licenses.insert( "localhost", new PHILicense() );
    //_validLicenses.insert( "localhost", true );
    _invalidateTouch=QDateTime::currentDateTime();
}

PHIParent::~PHIParent()
{
    _lock.lockForWrite();
    _instance=0;
    PHISPageCache::invalidate();
/*
    PHILicense *l;
    foreach ( l, _licenses ) delete l;
*/
    if ( _internalApp ) delete _app;
    _app=0;
    _lock.unlock();
    qDebug( "PHIParent::~PHIParent()" );
}

QString PHIParent::tempDir( const QString &domain, const QString &def )
{
    QString tmp;
    _lock.lockForRead();
    if ( _tmpDirs.contains( domain ) ) {
        tmp=_tmpDirs.value( domain );
        _lock.unlock();
        if ( _internalApp ) {
            QFileInfo info( tmp+QDir::separator()+QLatin1String( ".invalidate" ) );
            if ( info.exists() ) {
                if ( _invalidateTouch < info.lastModified() ) {
                   invalidate( domain );
                   _invalidateTouch=info.lastModified();
               }
            }
        }
        return tmp;
    }
    _lock.unlock();
    QWriteLocker l( &_lock );
    QSettings *s=PHI::globalSettings();
    s->beginGroup( def );
    s->beginGroup( domain );
    tmp=PHI::stdTmpPath()+QDir::separator()+domain;
    tmp=s->value( QStringLiteral( "TempDir" ), tmp ).toString();
    s->setValue( QStringLiteral( "TempDir" ), tmp );
    s->endGroup();
    s->endGroup();
    if ( !tmp.isEmpty() ) _tmpDirs.insert( domain, tmp );
    else return tmp;

    QDir imgDir( tmp+QDir::separator()+QLatin1String( "img" ) );
    imgDir.mkpath( tmp+QDir::separator()+QLatin1String( "img" ) );
    QDir cssDir( tmp+QDir::separator()+QLatin1String( "css" ) );
    cssDir.mkpath( tmp+QDir::separator()+QLatin1String( "css" ) );
    QDir dbDir( tmp+QDir::separator()+QLatin1String( "db" ) );
    dbDir.mkpath( tmp+QDir::separator()+QLatin1String( "db" ) );
    QDir jsDir( tmp+QDir::separator()+QLatin1String( "js" ) );
    jsDir.mkpath( tmp+QDir::separator()+QLatin1String( "js" ) );
    return tmp;
}

/*
void PHIParent::setDatabase( const QString &db, const QString &name, const QString &host,
    const QString &user, const QString &pwd, const QString &connOpts, int port, const QString &domain )
{
    DBSettings dbs;
    dbs._db=db;
    dbs._name=name;
    dbs._host=host;
    dbs._user=user;
    dbs._passwd=pwd;
    dbs._opts=connOpts;
    dbs._port=port;
    _lock.lockForWrite();
    _dbSettings.insert( domain, dbs );
    _lock.unlock();
}
*/

/*
void PHIParent::database( QString &db, QString &name, QString &host, QString &user, QString &pwd,
    QString &connOpts, int &port, const QString &domain )
{
    DBSettings dbs;
    _lock.lockForRead();
    if ( _dbSettings.contains( domain ) ) {
        dbs=_dbSettings.value( domain );
        _lock.unlock();
        db=dbs._db;
        name=dbs._name;
        host=dbs._host;
        user=dbs._user;
        pwd=dbs._passwd;
        connOpts=dbs._opts;
        port=dbs._port;
        return;
    }
    _lock.unlock();
    QString conf="phis";
    if ( isApacheModule() ) conf="phi";
    QSettings s( QSettings::SystemScope, PHI::organisation(), conf );
    s.setFallbacksEnabled( false );
    s.beginGroup( domain );
    const char* dbtype="DefaultDbType";
    const char* dbname="DefaultDbName";
    const char* dbhost="DefaultDbHost";
    const char* dbuser="DefaultDbUser";
    const char* dbpass="DefaultDbPasswd";
    const char* dbconn="DefaultDbConnOptions";
    const char* dbport="DefaultDbPort";

    s.setValue( dbtype, s.value( dbtype, "QMYSQL" ).toString() );
    s.setValue( dbname, s.value( dbname, "" ).toString() );
    s.setValue( dbhost, s.value( dbhost, "localhost" ).toString() );
    s.setValue( dbuser, s.value( dbuser, "user" ).toString() );
    s.setValue( dbpass, s.value( dbpass, "password" ).toString() );
    s.setValue( dbconn, s.value( dbconn, "").toString() );
    s.setValue( dbport, s.value( dbport, 3306 ).toInt() );

    dbs._db=s.value( dbtype, "QMYSQL" ).toString();
    dbs._name=s.value( dbname ).toString();
    dbs._host=s.value( dbhost, "localhost" ).toString();
    dbs._user=s.value( dbuser, "user" ).toString();
    dbs._passwd=s.value( dbpass, "password" ).toString();
    dbs._opts=s.value( dbconn, "" ).toString();
    dbs._port=s.value( dbport, 3306 ).toInt();
    s.endGroup();
    _lock.lockForWrite();
    _dbSettings.insert( domain, dbs );
    _lock.unlock();
    db=dbs._db;
    name=dbs._name;
    host=dbs._host;
    user=dbs._user;
    pwd=dbs._passwd;
    connOpts=dbs._opts;
    port=dbs._port;
}
*/

void PHIParent::invalidate( const QString &domain )
{
    qDebug( "invalidate '%s'", qPrintable( domain.isEmpty() ? QLatin1String( "all" ) : domain ) );
    PHISModuleFactory::instance( this )->invalidate();
    _lock.lockForWrite();
    _loadedModules=PHISModuleFactory::instance( this )->loadedModules();
    _moduleLoadErrors=PHISModuleFactory::instance( this )->loadErrors();
    if ( domain.isEmpty() ) {
        QString tmpDir;
        foreach( tmpDir, _tmpDirs ) clearTmpDir( tmpDir );
        _tmpDirs.clear();
        _dbSettings.clear();
        /*
        PHILicense *l;
        foreach ( l, _licenses ) delete l;
        _licenses.clear();
        _licenses.insert( "localhost", new PHILicense() );
        _validLicenses.clear();
        _validLicenses.insert( "localhost", true );
        */
    } else {
        clearTmpDir( _tmpDirs.value( domain ) );
        _tmpDirs.remove( domain );
        _dbSettings.remove( domain );
        /*
        if ( domain!="localhost" ) {
            PHILicense *l;
            l=_licenses.value( domain, 0 );
            delete l;
            _licenses.remove( domain );
            _validLicenses.remove( domain );
        }
        */
    }
    _lock.unlock();
    PHISPageCache::invalidate();
    // PHISItemCache::instance()->invalidate(); // done by PHISPageCache::invalidate()
}

void PHIParent::clearTmpDir( const QString &tmp )
{
    qDebug( "clearTmpDir '%s'", qPrintable( tmp ) );
    // _lock should be locked for write
    QString entry;
    QDir imgDir( tmp+QDir::separator()+QLatin1String( "img" ) );
    QStringList entries=imgDir.entryList( QDir::Files );
    foreach ( entry, entries ) {
        QString fn( imgDir.absolutePath()+QDir::separator()+entry );
        if ( _internalApp ) {
            QFileInfo info( fn );
            //qDebug( "FILE %s %s %s", qPrintable( info.fileName() ),
            //        qPrintable( info.lastModified().toString() ),
            //        qPrintable( _invalidateTouch.toString() ) );
            if ( _invalidateTouch > info.lastModified() ) {
                qDebug( "-----------removing FILE %s", qPrintable( info.fileName() ) );
                QFile::remove( fn );
            }
        } else QFile::remove( fn );
    }
    QDir cssDir( tmp+QDir::separator()+QLatin1String( "css" ) );
    entries=cssDir.entryList( QDir::Files );
    foreach ( entry, entries )  {
        QString fn( cssDir.absolutePath()+QDir::separator()+entry );
        if ( _internalApp ) {
            QFileInfo info( fn );
            if ( _invalidateTouch < info.lastModified() ) QFile::remove( fn );
        } else QFile::remove( fn );
    }
    //QDir dbDir( tmp+QDir::separator()+"db" );
    //entries=dbDir.entryList( QDir::Files );
    //foreach ( entry, entries ) QFile::remove( dbDir.absolutePath()
    //    +QDir::separator()+entry );
    QDir jsDir( tmp+QDir::separator()+QLatin1String( "js" ) );
    entries=jsDir.entryList( QDir::Files );
    foreach ( entry, entries ) {
        QString fn( imgDir.absolutePath()+QDir::separator()+entry );
        if ( _internalApp ) {
            QFileInfo info( fn );
            if ( _invalidateTouch < info.lastModified() ) QFile::remove( fn );
        } else QFile::remove( fn );
    }
}

/*
void PHIParent::readLicenseFile( const QString &documentRoot, const QString &domain )
{
    //qDebug( "readLicenseFile(%s)", qPrintable( domain ) );
    _lock.lockForRead();
    if ( _licenses.contains( domain ) ) {
        //qDebug( "contains lic" );
        _lock.unlock();
        return;
    }
    _lock.unlock();

    QFile f( documentRoot+"/philicense.dat" );
    _lock.lockForWrite();
    if ( f.open( QIODevice::ReadOnly ) ) {
        QDataStream ds( &f );
        ds.setVersion( PHI_DSV );
        while ( !f.atEnd() ) {
            PHILicense *lic=new PHILicense();
            ds >> *lic;
            if ( ds.status()!=QDataStream::Ok ) {
                delete lic;
                break;
            }
            if ( lic->domain()==domain.toUtf8() ) {
                _validLicenses.insert( domain, true );
                // Check also for *.domain.tl
                QString topDomain=domain.mid( domain.indexOf( '.' )+1 );
                if ( topDomain.contains( '.' ) ) _validLicenses.insert( topDomain, true );
                qDebug( "Domains %s %s", qPrintable( domain ), qPrintable( topDomain ) );
            }
            _licenses.insert( domain, lic );
        }
        f.close();
    }
    _lock.unlock();
}
*/
