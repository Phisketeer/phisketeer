/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2013  Phisys AG, Switzerland
#    Copyright (C) 2012-2013  Phisketeer.org team
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QSettings>
#include <QDateTime>
#include <QStringList>
#include <QHostInfo>
#include <QFile>
#include <QDir>
#include <QBuffer>
#include "phiconfig.h"
#include "phierror.h"
#include "phisysinfo.h"
#include "phi.h"

#ifdef PHIAPPSTORE
#include "macfilebookmark.h"
#endif

PHIConfig* PHIConfig::_instance=0;

PHIConfig::PHIConfig()
    : _maxConnections(0)
{
    qDebug( "PHIConfig::PHIConfig()" );
    _settings=PHI::globalSettings();
    qDebug( "PHIConfig::_settings=%s", qPrintable( _settings->fileName() ) );
}

PHIConfig::~PHIConfig()
{
    _lock.lockForWrite();
    _instance=0;
    //_settings->setValue( "Shutdown", true );
    //_settings->setValue( "Creation", _creation );
    //_settings->setValue( "Modification", _modification );
    //_settings->sync();
    _lock.unlock();
    qDebug( "PHIConfig::~PHIConfig()" );
}

PHIConfig* PHIConfig::instance()
{
    if ( !_instance ) _instance=new PHIConfig();
    return _instance;
}

QVariant PHIConfig::configValue( const QString &key, const QVariant &v ) const
{
    QWriteLocker lock( &_lock );
    _settings->beginGroup( _mgrName );
    QVariant tmp=_settings->value( key, v );
    _settings->endGroup();
    return tmp;
}

/*
void PHIConfig::setConfigValue( const QString &key, const QVariant &v )
{
    QWriteLocker lock( &_lock );
    _settings->beginGroup( _mgrName );
    _settings->setValue( key, v );
    //_settings->setValue( "Modification", QDateTime::currentDateTime() );
    _settings->endGroup();
    _settings->sync();
    //_modification=_settings->value( "Modification" ).toDateTime();
}
*/

PHIRC PHIConfig::init( QObject *mgr )
{
    if ( !mgr ) return PHIRC_MGR_NAME_ERROR;
    _mgrName=mgr->objectName();
    qRegisterMetaType<QDateTime>( "QDateTime" );
    qRegisterMetaType<PHIRC>( "PHIRC" );

    QSettings *s=_settings;
    /*
    if ( !s->isWritable() ){
        PHIError::instance()->print( PHIRC_IO_FILE_ACCESS_ERROR,
            QObject::tr( "Check if the config file '%1' is writable." ).arg( s->fileName() ) );
        return PHIRC_IO_FILE_ACCESS_ERROR;
    }
    */
    _rootDir=PHI::applicationRoot();
    s->setValue( QStringLiteral( "RootDir" ), s->value( QStringLiteral( "RootDir" ), _rootDir ) );
    _rootDir=s->value( QStringLiteral( "RootDir" ) ).toString();
    qDebug( "RootDir=%s", qPrintable( _rootDir ) );

    s->beginGroup( _mgrName );
    s->setValue( QStringLiteral( "BaseDir" ), s->value( QStringLiteral( "BaseDir" ),
        _rootDir+QDir::separator()+_mgrName ) );
    _baseDir=s->value( QStringLiteral( "BaseDir" ) ).toString();
    qDebug( "BaseDir=%s", qPrintable( _baseDir ) );
#ifdef PHIAPPSTORE
    PHISecFile::loadSecurityBookmarkForDir( _baseDir+"/localhost" );
#endif
    s->setValue( QStringLiteral( "ListenerPort" ), s->value( QStringLiteral( "ListenerPort" ), 8080 ) );
    s->setValue( QStringLiteral( "ListenerIF" ), s->value( QStringLiteral( "ListenerIF" ), QStringLiteral( "Any" ) ) );
    s->setValue( QStringLiteral( "SSLEnabled" ), s->value( QStringLiteral( "SSLEnabled" ), false ) );
    s->setValue( QStringLiteral( "SSLListenerPort" ), s->value( QStringLiteral( "SSLListenerPort" ), 443 ) );
    s->setValue( QStringLiteral( "SSLListenerIF" ), s->value( QStringLiteral( "SSLListenerIF" ), QStringLiteral( "Any" ) ) );
    s->setValue( QStringLiteral( "SSLCertificate" ), s->value( QStringLiteral( "SSLCertificate" ),
        _baseDir+QDir::separator()+QLatin1String( "ssl" )+QDir::separator()+QLatin1String( "localhost.crt" ) ) );
    s->setValue( QStringLiteral( "SSLPrivateKey" ), s->value( QStringLiteral( "SSLPrivateKey" ), _baseDir+QDir::separator()
        +QLatin1String( "ssl" )+QDir::separator()+QLatin1String( "localhost.key" ) ) );
    s->setValue( QStringLiteral( "KeepAlive" ), s->value( QStringLiteral( "KeepAlive" ), 60000 ) );
    s->setValue( QStringLiteral( "Admin" ), s->value( QStringLiteral( "Admin" ), QStringLiteral( "webmaster@localhost" ) ) );
    s->setValue( QStringLiteral( "LogDir" ), s->value( QStringLiteral( "LogDir" ), _rootDir+QDir::separator()+QLatin1String( "log" ) ) );
    s->setValue( QStringLiteral( "LogFilter" ), s->value( QStringLiteral( "LogFilter" ), 0 ) );
    s->setValue( QStringLiteral( "Index" ), s->value( QStringLiteral( "Index" ), QStringLiteral( "index.phis,index.html,index.htm" ) ) );
    s->setValue( QStringLiteral( "MimeTypesFile" ), s->value( QStringLiteral( "MimeTypesFile" ),
        _rootDir+QDir::separator()+QLatin1String( "mimetypes.txt" ) ) );

#ifdef Q_OS_WIN
    int autostart=s->value( QStringLiteral( "StartMode" ), 2 ).toInt();
    QSettings mode( QStringLiteral( "HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\services\\phis" ),
        QSettings::NativeFormat );
    if ( mode.contains( QStringLiteral( "Start" ) ) ) {
        mode.setValue( QStringLiteral( "Start" ), autostart );
        s->setValue( QStringLiteral( "StartMode" ), autostart );
    }
#endif

    s->beginGroup( QStringLiteral( "localhost" ) );
    s->setValue( QStringLiteral( "DocumentRoot" ), s->value( QStringLiteral( "DocumentRoot" ),
        _baseDir+QDir::separator()+QLatin1String( "localhost" ) ) );
    s->setValue( QStringLiteral( "TempDir" ), s->value( QStringLiteral( "TempDir" ), PHI::stdTmpPath()+QDir::separator()
        +_mgrName+QDir::separator()+QLatin1String( "localhost" ) ) );
    qDebug( "DocumentRoot=%s", qPrintable( s->value( QStringLiteral( "DocumentRoot" ) ).toString() ) );
    s->endGroup();
    s->endGroup();

    return updateConfig();
}

QString PHIConfig::documentRoot( const QString &host ) const
{
    QString tmp;
    _lock.lockForRead();
    if ( _docRoots.contains( host ) ) {
        tmp=_docRoots.value( host );
        _lock.unlock();
        return tmp;
    }
    _lock.unlock();
    QWriteLocker lock( &_lock );
    QSettings *s=_settings;
    s->beginGroup( _mgrName );
    s->beginGroup( host );
    tmp=s->value( QStringLiteral( "DocumentRoot" ), QString() ).toString();
    s->endGroup();
    s->endGroup();
    if ( !tmp.isEmpty() ) _docRoots.insert( host, tmp );
    return tmp;
}

PHIRC PHIConfig::updateConfig()
{
    QWriteLocker lock( &_lock );
    QSettings *s=_settings;
    s->sync();
    _docRoots.clear();
    _rootDir=s->value( QStringLiteral( "RootDir" ) ).toString();
    s->beginGroup( _mgrName );
    _baseDir=s->value( QStringLiteral( "BaseDir" ) ).toString();
    _logDir=s->value( QStringLiteral( "LogDir" ) ).toString();
    _admin=s->value( QStringLiteral( "Admin" ) ).toString();
    _keepAlive=s->value( QStringLiteral( "KeepAlive" ) ).toInt();
    _logFilter=static_cast<quint8>(s->value( QStringLiteral( "LogFilter" ) ).toUInt());
    _index=s->value( QStringLiteral( "Index" ) ).toString();
    s->endGroup();
    qDebug( "Configuration updated." );
    return PHIRC_OK;
}
