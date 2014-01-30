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
#include <QSettings>
#include <QMutexLocker>
#include <QDir>
#include <QTimer>
#include "phisparent.h"
#include "phiapplication.h"
#include "phierror.h"
#include "phismodulefactory.h"
#include "phislogwriter.h"
#include "phislistener.h"
#include "phispagecache.h"
#include "phiimagecache.h"
#include "phisession.h"

PHISParent* PHISParent::_instance=0;

PHISParent::PHISParent( QObject *parent, const QString &name )
    : QObject( parent ), _name( name ), _listenerRunning( false )
{
    qDebug( "PHISParent::PHISParent()" );
    PHIError::instance( this );
    PHISModuleFactory::instance( this );
    QSettings *s=phiApp->serverSettings();
    s->setValue( SL( "RootDir" ), s->value( SL( "RootDir" ), phiApp->rootPath() ) );
    s->beginGroup( name );
    s->setValue( SL( "BaseDir" ), s->value( SL( "BaseDir" ), phiApp->dataPath() ) );
    s->setValue( SL( "ListenerPort" ), s->value( SL( "ListenerPort" ), 8080 ) );
    s->setValue( SL( "ListenerIF" ), s->value( SL( "ListenerIF" ), SL( "Any" ) ) );
    s->setValue( SL( "SSLEnabled" ), s->value( SL( "SSLEnabled" ), false ) );
    s->setValue( SL( "SSLListenerPort" ), s->value( SL( "SSLListenerPort" ), 443 ) );
    s->setValue( SL( "SSLListenerIF" ), s->value( SL( "SSLListenerIF" ), SL( "Any" ) ) );
    s->setValue( SL( "SSLCertificate" ), s->value( SL( "SSLCertificate" ), phiApp->dataPath()+L1( "/ssl/localhost.crt" ) ) );
    s->setValue( SL( "SSLPrivateKey" ), s->value( SL( "SSLPrivateKey" ), phiApp->dataPath()+L1( "/ssl/localhost.key" ) ) );
    s->setValue( SL( "KeepAlive" ), s->value( SL( "KeepAlive" ), 60000 ) );
    s->setValue( SL( "Admin" ), s->value( SL( "Admin" ), SL( "webmaster@localhost" ) ) );
    s->setValue( SL( "LogDir" ), s->value( SL( "LogDir" ), phiApp->cachePath()+L1( "/log" ) ) );
    s->setValue( SL( "LogFilter" ), s->value( SL( "LogFilter" ), 0x10 ) ); // supress debug messages
    s->setValue( SL( "Index" ), s->value( SL( "Index" ), SL( "index.phis,index.html,index.htm" ) ) );
    _index=s->value( SL( "Index" ) ).toString();
    _keepAlive=s->value( SL( "KeepAlive" ) ).value<qint32>();
    _admin=s->value( SL( "Admin" ) ).toString();
    s->endGroup();
    s->sync();

    PHISLogWriterThread::instance()->init( this, name );
    QString error;
    PHIRC rc;
    rc=PHIImageCache::instance()->init( error, this );
    if ( rc!=PHIRC_OK ) PHISLogWriter::instance()->log( PHILOGCRIT, rc, error );
    rc=PHISession::instance()->init( error, this );
    if ( rc!=PHIRC_OK ) PHISLogWriter::instance()->log( PHILOGCRIT, rc, error );
    QTimer::singleShot( 0, this, SLOT( startService() ) );
}

PHISParent::~PHISParent()
{
    stopService();
    _lock.lockForWrite();
    _instance=0;
    _lock.unlock();
    qDebug( "PHISParent::~PHISParent()" );
}

void PHISParent::stopService()
{
    if ( !_listenerRunning ) return;
    // delete listener before cleaning page cache
    if ( value( SL( "SSLEnabled" ) ).toBool() ) {
        delete PHISslListener::instance();
    }
    delete PHISListener::instance(); // disconnect all clients
    PHISPageCache::invalidate(); // PHIPageCache is static so invalidate only
    _listenerRunning=false;
}

void PHISParent::startService()
{
    if ( _listenerRunning ) return;
    invalidate();
    PHIRC rc=PHISListener::instance()->init( this );
    if ( rc==PHIRC_OK ) {
        PHISLogWriter::instance()->log( PHILOGTRACE, PHIRC_MGR_START,
            tr( "Phis '%1' ready for service." ).arg( _name ) );
        _listenerRunning=true;
    }
    if ( value( SL( "SSLEnabled" ) ).toBool() ) {
        rc=PHISslListener::instance()->init( this );
    }
}

QVariant PHISParent::value( const QString &key ) const
{
    QVariant v;
    QSettings *s=phiApp->serverSettings();
    QWriteLocker l( &_lock );
    s->beginGroup( _name );
    v=s->value( key );
    s->endGroup();
    return v;
}

QString PHISParent::tempDir( const QString &domain )
{
    QString tmp;
    _lock.lockForRead();
    tmp=_tmpDirs.value( domain, QString() );
    if ( !tmp.isNull() ) {
        _lock.unlock();
        return tmp;
    }
    _lock.unlock();
    QWriteLocker l( &_lock );
    QSettings *s=phiApp->serverSettings();
    s->beginGroup( _name );
    s->beginGroup( domain );
    tmp=phiApp->tmpPath()+L1( "/" )+_name+L1( "/" )+domain;
    tmp=s->value( SL( "TempDir" ), tmp ).toString();
    s->setValue( SL( "TempDir" ), QDir::toNativeSeparators( tmp ) );
    s->endGroup();
    s->endGroup();
    _tmpDirs.insert( domain, tmp );
    QStringList subdirs;
    subdirs << SL( "/img" ) << SL( "/css" ) << SL( "/db" ) << SL( "/js" );
    foreach ( QString subdir, subdirs ) {
        QDir sub( tmp+subdir );
        sub.mkpath( tmp+subdir );
    }
    return tmp;
}

QString PHISParent::documentRoot( const QString &domain )
{
    QString root;
    _lock.lockForRead();
    root=_docRootDirs.value( domain, QString() );
    if ( !root.isEmpty() ) {
        _lock.unlock();
        return root;
    }
    _lock.unlock();
    QSettings *s=phiApp->serverSettings();
    QWriteLocker l( &_lock );
    s->beginGroup( _name );
    s->beginGroup( domain );
    root=s->value( SL( "DocumentRoot" ), QString() ).toString();
    s->endGroup();
    s->endGroup();
    if ( !root.isEmpty() ) _docRootDirs.insert( domain, root );
    return root;
}

void PHISParent::invalidate( const QString &domain )
{
    qDebug( "PHISParent::invalidate '%s'", qPrintable( domain.isEmpty() ? L1( "all" ) : domain ) );
    _invalidateTouch=QDateTime::currentDateTime();
    PHISModuleFactory::instance()->invalidate();
    PHISPageCache::invalidate();
    _lock.lockForWrite();
    QSettings *s=phiApp->serverSettings();
    s->beginGroup( _name );
    _index=s->value( SL( "Index" ), SL( "index.phis" ) ).toString();
    _keepAlive=s->value( SL( "KeepAlive" ), 60000 ).value<qint32>();
    _admin=s->value( SL( "Admin" ), SL( "webmaster@localhost" ) ).toString();
    s->endGroup();
    QStringList loadedModules=PHISModuleFactory::instance( this )->loadedModules();
    QStringList moduleLoadErrors=PHISModuleFactory::instance( this )->loadErrors();
    foreach ( QString merr, moduleLoadErrors ) {
        PHISLogWriter::instance()->log( PHILOGTRACE, PHIRC_MODULE_LOAD_ERROR, merr );
    }
    foreach ( QString module, loadedModules ) {
        PHISLogWriter::instance()->log( PHILOGTRACE, PHIRC_MODULE_LOG, module );
    }
    if ( domain.isEmpty() ) {
        QString tmpDir;
        foreach( tmpDir, _tmpDirs ) clearTmpDir( tmpDir );
        _tmpDirs.clear();
        _docRootDirs.clear();
    } else {
        clearTmpDir( _tmpDirs.value( domain ) );
        _tmpDirs.remove( domain );
        _docRootDirs.remove( domain );
    }
    _lock.unlock();
}

void PHISParent::clearTmpDir( const QString &tmp )
{
    qDebug( "PHISParent::clearTmpDir: '%s'", qPrintable( tmp ) );
    // _lock must be locked for write!
    QString entry;
    QDir imgDir( tmp+SL( "/img" ) );
    QStringList entries=imgDir.entryList( QDir::Files );
    foreach ( entry, entries ) {
        QString fn( imgDir.absolutePath()+QDir::separator()+entry );
        QFile::remove( fn );
    }
    QDir cssDir( tmp+SL( "/css" ) );
    entries=cssDir.entryList( QDir::Files );
    foreach ( entry, entries )  {
        QString fn( cssDir.absolutePath()+QDir::separator()+entry );
        QFile::remove( fn );
    }
    QDir jsDir( tmp+SL( "/js" ) );
    entries=jsDir.entryList( QDir::Files );
    foreach ( entry, entries ) {
        QString fn( imgDir.absolutePath()+QDir::separator()+entry );
        QFile::remove( fn );
    }
}
