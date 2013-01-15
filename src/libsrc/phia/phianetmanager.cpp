/*
#    Copyright (C) 2010-2012  Marius B. Schumacher
#    Copyright (C) 2011-2012  Phisys AG, Switzerland
#    Copyright (C) 2012  Phisketeer.org team
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
#include <QApplication>
#include <QSettings>
#include <QNetworkProxy>
#include <QtWebKit>
#include "phianetmanager.h"
#include "phiatools.h"
#include "phisysinfo.h"
#include "phi.h"
#include "phia.h"
#include "phiaconfig.h"

PHIANetworkAccessManager::PHIANetworkAccessManager( QObject *parent )
    : QNetworkAccessManager( parent )
{
    qDebug( "PHIANetworkAccessManager::PHIANetworkAccessManager()" );
    _agent="Mozilla/5.0 ("+PHISysInfo::systemString().toUtf8()+") "
        +"AppleWebKit/"+QString( qWebKitVersion() ).toLatin1()+" (KHTML, like Gecko) "
        +PHIA::browserName().toUtf8()+"/"+PHIA::libVersion().toUtf8();
    _accept=PHI::mimeType()+",text/html,application/xml;q=0.9,application/xhtml+xml;q=0.8,"
        "text/plain;q=0.8,image/png,image/*;q=0.6,*/*;q=0.5";
    _charset="utf-8,*;q=0.7";
    PHIAConfig::instance( this ); // create PHIA config instance
}

PHIANetworkAccessManager::~PHIANetworkAccessManager()
{
    qDebug( "PHIANetworkAccessManager::~PHIANetworkAccessManager()" );
}

QNetworkReply* PHIANetworkAccessManager::createRequest( QNetworkAccessManager::Operation op,
    const QNetworkRequest &req, QIODevice *outgoingData )
{
    QNetworkRequest request=req;
    QStringList langs=PHIAConfig::instance()->languages();
    if ( langs.isEmpty() ) langs << "en-us" << "en";
    qDebug( "REQUEST %s", qPrintable( request.url().toString() ) );
    request.setRawHeader( "User-Agent", _agent );
    QByteArray acc=request.rawHeader( "Accept" );
    if ( acc.isEmpty() ) request.setRawHeader( "Accept", _accept );
    else {
        acc.prepend( PHI::mimeType()+',' );
        request.setRawHeader( "Accept", acc );
    }
    request.setRawHeader( "Accept-Language", langs.join( "," ).toUtf8() );
    request.setRawHeader( "Accept-Charset", _charset );
    return QNetworkAccessManager::createRequest( op, request, outgoingData );
}

PHIANetworkAccessManagerNoPhiAccept::PHIANetworkAccessManagerNoPhiAccept( QObject *parent )
    : PHIANetworkAccessManager( parent )
{
    qDebug( "PHIANetworkAccessManagerNoPhiAccept::PHIANetworkAccessManagerNoPhiAccept()" );
    _accept="text/html,application/xml;q=0.9,application/xhtml+xml;q=0.8,"
        "text/plain;q=0.8,image/png,image/*;q=0.6,*/*;q=0.5";
}

PHIANetworkAccessManagerNoPhiAccept::~PHIANetworkAccessManagerNoPhiAccept()
{
    qDebug( "PHIANetworkAccessManagerNoPhiAccept::~PHIANetworkAccessManagerNoPhiAccept()" );
}

QNetworkReply* PHIANetworkAccessManagerNoPhiAccept::createRequest( QNetworkAccessManager::Operation op,
    const QNetworkRequest &req, QIODevice *outgoingData )
{
    QNetworkRequest request=req;
    QStringList langs=PHIAConfig::instance()->languages();
    if ( langs.isEmpty() ) langs << "en-us" << "en";
    qDebug( "REQUEST NO PHI ACCEPT %s", qPrintable( request.url().toString() ) );
    request.setRawHeader( "User-Agent", _agent );
    qDebug( "Agent:%s", request.rawHeader( "User-Agent" ).data() );
    QByteArray acc=request.rawHeader( "Accept" );
    if ( acc.isEmpty() ) request.setRawHeader( "Accept", _accept );

    request.setRawHeader( "Accept-Language", langs.join( "," ).toUtf8() );
    request.setRawHeader( "Accept-Charset", _charset );
    return QNetworkAccessManager::createRequest( op, request, outgoingData );
}

PHIANetManager* PHIANetManager::_instance=0;

PHIANetManager* PHIANetManager::instance()
{
    if ( _instance ) return _instance;
    _instance=new PHIANetManager( qApp );
    return _instance;
}

PHIANetManager::PHIANetManager( QObject *parent )
    : QObject( parent )
{
    qDebug( "PHIANetManager::PHIANetManager()" );
    _networkAccessManager=new PHIANetworkAccessManager( this );
    _networkAccessManagerNoPhiAccept=new PHIANetworkAccessManagerNoPhiAccept( this );

    _networkAccessManager->setCookieJar( PHIACookies::instance() );
    _networkAccessManagerNoPhiAccept->setCookieJar( PHIACookies::instance() );
    //PHIACookies::instance()->setParent( qApp );
    //_networkAccessManager->setCache( new PHIADiskCache( _networkAccessManager ) );
    //_networkAccessManagerNoPhiAccept->setCache( new PHIADiskCache( _networkAccessManagerNoPhiAccept ) );
    updateProxy();
}

void PHIANetManager::updateProxy()
{
    QSettings s;
    QString proxyHost=s.value( PHIA::configName( PHIA::ProxyHost ), QString() ).toString();
    if ( proxyHost.isEmpty() ) {
        _networkAccessManager->setProxy( QNetworkProxy() );
        _networkAccessManagerNoPhiAccept->setProxy( QNetworkProxy() );
        return;
    }
    QNetworkProxy proxy;
    proxy.setType( QNetworkProxy::HttpProxy );
    proxy.setHostName( proxyHost );
    proxy.setPort( static_cast<quint16>(s.value( PHIA::configName( PHIA::ProxyPort )).toInt()) );
    _networkAccessManager->setProxy( proxy );
    _networkAccessManagerNoPhiAccept->setProxy( proxy );
}

PHIANetManager::~PHIANetManager()
{
    _instance=0;
    qDebug( "PHIANetManager::~PHIANetManager()" );
}
