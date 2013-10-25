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
#include <QSettings>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkProxy>
#include "phinetmanager.h"
#include "phisysinfo.h"
#include "phiapplication.h"

PHINetworkAccessManager::PHINetworkAccessManager( QObject *parent )
    : QNetworkAccessManager( parent )
{
    qDebug( "PHIANetworkAccessManager::PHIANetworkAccessManager()" );
    // @todo: update current WebKit version
    _agent="Mozilla/5.0 ("+PHISysInfo::systemString().toUtf8()+") "
        +QByteArray( "AppleWebKit/" )+"537.36"+QByteArray( " (KHTML, like Gecko) " )
        +phiApp->applicationName().toLatin1()+"/"+PHI::libVersion().toLatin1();
    _accept=PHI::phiMimeType()+",text/html,application/xml;q=0.9,application/xhtml+xml;q=0.8,"
        "text/plain;q=0.8,image/png,image/*;q=0.6,*/*;q=0.5";
    _charset="utf-8,*;q=0.7";
    QSettings *s=phiApp->settings();
    s->beginGroup( SL( "Client" ) );
    _acceptedLangs=s->value( SL( "AcceptedLanguages" ), QByteArray( "en-us,en") ).toByteArray();
    s->endGroup();
}

PHINetworkAccessManager::~PHINetworkAccessManager()
{
    qDebug( "PHIANetworkAccessManager::~PHIANetworkAccessManager()" );
}

QNetworkReply* PHINetworkAccessManager::createRequest( QNetworkAccessManager::Operation op,
    const QNetworkRequest &req, QIODevice *outgoingData )
{
    QNetworkRequest request=req;
    qDebug( "REQUEST %s", qPrintable( request.url().toString() ) );
    request.setRawHeader( BL( "User-Agent" ), _agent );
    if ( !request.hasRawHeader( BL( "Accept" ) ) ) request.setRawHeader( BL( "Accept" ), _accept );
    request.setRawHeader( BL( "Accept-Language" ), _acceptedLangs );
    request.setRawHeader( BL( "Accept-Charset" ), _charset );
    return QNetworkAccessManager::createRequest( op, request, outgoingData );
}

PHINetManager* PHINetManager::_instance=0;

PHINetManager* PHINetManager::instance()
{
    if ( _instance ) return _instance;
    _instance=new PHINetManager( phiApp );
    return _instance;
}

PHINetManager::PHINetManager( QObject *parent )
    : QObject( parent )
{
    qDebug( "PHINetManager::PHINetManager()" );
    _networkAccessManager=new PHINetworkAccessManager( this );
    // @todo: setup cookies and disk cache
    // _networkAccessManager->setCookieJar( PHIACookies::instance() );
    //PHIACookies::instance()->setParent( qApp );
    //_networkAccessManager->setCache( new PHIADiskCache( _networkAccessManager ) );
    //_networkAccessManagerNoPhiAccept->setCache( new PHIADiskCache( _networkAccessManagerNoPhiAccept ) );
    updateProxy();
}

void PHINetManager::updateProxy()
{
    QSettings *s=phiApp->settings();
    s->beginGroup( SL( "Client" ) );
    QString proxyHost=s->value( SL( "ProxyHost" ), QString() ).toString();
    quint16 port=s->value( SL( "ProxyPort" ) ).value<quint16>();
    s->endGroup();
    if ( proxyHost.isEmpty() ) {
        _networkAccessManager->setProxy( QNetworkProxy() );
        return;
    }
    QNetworkProxy proxy;
    proxy.setType( QNetworkProxy::HttpProxy );
    proxy.setHostName( proxyHost );
    proxy.setPort( port );
    _networkAccessManager->setProxy( proxy );
}

PHINetManager::~PHINetManager()
{
    _instance=0;
    qDebug( "PHINetManager::~PHINetManager()" );
}
