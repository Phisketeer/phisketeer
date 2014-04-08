/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2014  Phisys AG, Switzerland
#    Copyright (C) 2012-2014  Phisketeer.org team
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QDir>
#include "apachereqparent.h"
#include "phiapplication.h"
#include "phismodulefactory.h"
#include "phiimagecache.h"
#include "phispagecache.h"
#include "phisession.h"
#include "phierror.h"
#include "phisysinfo.h"

ApacheReqParent* ApacheReqParent::_instance=0;

ApacheReqParent::ApacheReqParent( QObject *parent )
    : QObject( parent )
{
    qDebug( "ApacheReqParent::ApacheReqParent()" );
    QStringList argList;
    argList << L1( "libmod_phi.so" );
#ifdef Q_OS_UNIX
    argList << L1( "-platform" ) << L1( "offscreen" );
#endif
    int p_argc=argList.size();
    QVector<char *> p_argv( p_argc );
    QList<QByteArray> argvData;
    for ( int i=0; i<p_argc; ++i ) argvData.append( argList.at( i ).toLatin1() );
    for ( int i=0; i<p_argc; ++i ) p_argv[i]=argvData[i].data();
    new PHIApplication( p_argc, p_argv.data(), "phis", PHIVERSION, PHIApplication::ApacheModule );
    PHIError::instance( this );
    PHISModuleFactory::instance( this )->invalidate();
}

ApacheReqParent::~ApacheReqParent()
{
    delete phiApp;
    _instance=0;
    qDebug( "ApacheReqParent::~ApacheReqParent()" );
}

QStringList ApacheReqParent::moduleLoadErrors() const
{
    return PHISModuleFactory::instance()->loadErrors();
}

QStringList ApacheReqParent::loadedModules() const
{
    return PHISModuleFactory::instance()->loadedModules();
}

QString ApacheReqParent::tempDir( const QString &domain )
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
    s->beginGroup( PHI::defaultString() );
    s->beginGroup( domain );
    tmp=phiApp->tmpPath()+L1( "/" )+domain;
    tmp=s->value( SL( "TempDir" ), tmp ).toString();
    s->setValue( SL( "TempDir" ), QDir::toNativeSeparators( tmp ) );
    s->endGroup();
    s->endGroup();
    _tmpDirs.insert( domain, tmp );
    QStringList subdirs;
    subdirs << SL( "/img" ) << SL( "/css" ) << SL( "/js" );
    foreach ( QString subdir, subdirs ) {
        QDir sub( tmp+subdir );
        sub.mkpath( tmp+subdir );
    }
    return tmp;
}

ApacheReqChild* ApacheReqChild::_instance=0;

ApacheReqChild::ApacheReqChild( QObject *parent )
    : QObject( parent )
{
    qDebug( "ApacheReqChild::ApacheReqChild()" );
    QString error;
    PHISPageCache::instance();
    PHIRC rc=PHIImageCache::instance()->init( error, this );
    if ( Q_UNLIKELY( rc!=PHIRC_OK ) ) _errors << error;
    rc=PHISession::instance()->init( error, this );
    if ( Q_UNLIKELY( rc!=PHIRC_OK ) ) _errors << error;
    QDir cache( phiApp->cachePath() );
    if ( !cache.exists() ) cache.mkpath( phiApp->cachePath() );
}

ApacheReqChild::~ApacheReqChild()
{
    delete PHISession::instance();
    delete PHIImageCache::instance();
    delete PHISPageCache::instance();
    _instance=0;
    qDebug( "ApacheReqChild::~ApacheReqChild()" );
}
