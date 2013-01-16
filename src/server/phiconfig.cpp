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
    _rootDir=PHI::compiledRoot();
    s->setValue( "RootDir", s->value( "RootDir", _rootDir ) );
    _rootDir=s->value( "RootDir" ).toString();
    qDebug( "RootDir=%s", qPrintable( _rootDir ) );

    s->beginGroup( _mgrName );
    s->setValue( "BaseDir", s->value( "BaseDir", _rootDir+QDir::separator()+_mgrName ) );
    _baseDir=s->value( "BaseDir" ).toString();
    qDebug( "BaseDir=%s", qPrintable( _baseDir ) );
#ifdef PHIAPPSTORE
    PHISecFile::loadSecurityBookmarkForDir( _baseDir+"/localhost" );
#endif
    s->setValue( "ListenerPort", s->value( "ListenerPort", 8080 ) );
    s->setValue( "ListenerIF", s->value( "ListenerIF", "Any" ) );
    s->setValue( "SSLEnabled", s->value( "SSLEnabled", false ) );
    s->setValue( "SSLListenerPort", s->value( "SSLListenerPort", 443 ) );
    s->setValue( "SSLListenerIF", s->value( "SSLListenerIF", "Any" ) );
    s->setValue( "SSLCertificate", s->value( "SSLCertificate",
        _baseDir+QDir::separator()+"ssl"+QDir::separator()+"localhost.crt" ) );
    s->setValue( "SSLPrivateKey", s->value( "SSLPrivateKey", _baseDir+QDir::separator()
        +"ssl"+QDir::separator()+"localhost.key" ) );
    s->setValue( "MimeTypesFile", s->value( "MimeTypesFile", _rootDir+QDir::separator()+"mimetypes.txt" ) );
    s->setValue( "KeepAlive", s->value( "KeepAlive", 60000 ) );
    s->setValue( "Admin", s->value( "Admin", "webmaster@localhost" ) );
    s->setValue( "LogDir", s->value( "LogDir", _rootDir+QDir::separator()+"log" ) );
    s->setValue( "LogFilter", s->value( "LogFilter", 0 ) );
    s->setValue( "Index", s->value( "Index", "index.phis,index.html,index.htm") );
    s->setValue( "MimeTypesFile", s->value( "MimeTypesFile",
        _rootDir+QDir::separator()+"mimetypes.txt" ) );

#ifdef Q_OS_WIN
    int autostart=s->value( "StartMode", 2 ).toInt();
    QSettings mode( "HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\services\\phis",
        QSettings::NativeFormat );
    if ( mode.contains( "Start" ) ) {
        mode.setValue( "Start", autostart );
        s->setValue( "StartMode", autostart );
    }
#endif

    s->beginGroup( "localhost" );
    s->setValue( "DocumentRoot", s->value( "DocumentRoot", _baseDir+QDir::separator()+"localhost" ) );
    s->setValue( "TempDir", s->value( "TempDir", PHI::stdTmpPath()+QDir::separator()
        +_mgrName+QDir::separator()+"localhost" ) );
    qDebug( "DocumentRoot=%s", qPrintable( s->value( "DocumentRoot" ).toString() ) );
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
    tmp=s->value( "DocumentRoot", "" ).toString();
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
    _rootDir=s->value( "RootDir" ).toString();
    s->beginGroup( _mgrName );
    _baseDir=s->value( "BaseDir" ).toString();
    _logDir=s->value( "LogDir" ).toString();
    _admin=s->value( "Admin" ).toString();
    _keepAlive=s->value( "KeepAlive" ).toInt();
    _logFilter=static_cast<quint8>(s->value( "LogFilter" ).toUInt());
    _index=s->value( "Index" ).toString();
    s->endGroup();
    PHIRC rc=loadMimeTypes();
    qDebug( "Configuration updated." );
    return rc;
}

PHIRC PHIConfig::loadMimeTypes()
{
    _settings->beginGroup( _mgrName );
    _mimeTypesFile=_settings->value( "MimeTypesFile" ).toString();
    _settings->endGroup();
    qDebug( "loadMimeTypes %s", qPrintable( _mimeTypesFile ) );
    PHI::createMimeTypes( _mimeTypesFile );
    return PHIRC_OK;
}
