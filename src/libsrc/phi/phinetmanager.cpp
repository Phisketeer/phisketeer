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
#include <QNetworkDiskCache>
#include <QWebPage>
#include "phinetmanager.h"
#include "phisysinfo.h"
#include "phiapplication.h"

class PHIDiskCache : public QNetworkDiskCache
{
public:
    static PHIDiskCache* instance();
    virtual ~PHIDiskCache();

protected:
    PHIDiskCache( QObject *parent );

private:
    static PHIDiskCache *_instance;
};

PHIDiskCache* PHIDiskCache::_instance=0;

PHIDiskCache::PHIDiskCache( QObject *parent )
    : QNetworkDiskCache( parent )
{
    qDebug( "PHIDiskCache::PHIDiskCache()" );
    setCacheDirectory( phiApp->cachePath() );
    QSettings *s=phiApp->settings();
    s->beginGroup( L1( "Client" ) );
    setMaximumCacheSize( s->value( L1( "MaxCacheSize" ), 100*1024*1024 ).toLongLong() );
    s->endGroup();
}

PHIDiskCache::~PHIDiskCache()
{
    _instance=0;
    qDebug( "PHIDiskCache::~PHIDiskCache()" );
}

PHIDiskCache* PHIDiskCache::instance()
{
    if ( _instance ) return _instance;
    _instance=new PHIDiskCache( phiApp );
    return _instance;
}

QByteArray PHINetworkAccessManager::_accept=PHI::phiMimeType();
QByteArray PHINetworkAccessManager::_agent="Mozilla/5.0";
QByteArray PHINetworkAccessManager::_charset="utf-8,*;q=0.8";
QByteArray PHINetworkAccessManager::_acceptedLangs="en-us,en";

PHINetworkAccessManager::PHINetworkAccessManager( QObject *parent, bool serverMode )
    : QNetworkAccessManager( parent )
{
    qDebug( "PHINetworkAccessManager::PHINetworkAccessManager()" );
    if ( !serverMode )  {
        setCookieJar( PHINetworkCookies::instance() );
        setCache( PHIDiskCache::instance() );
    }
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
    : QObject( parent ), _defaultNam( 0 )
{
    QString tmp;
    tmp=L1( "Mozilla/5.0 (" )+PHISysInfo::systemString()+L1( ") " )
        +L1( "AppleWebKit/" )+qWebKitVersion()+L1( " (KHTML, like Gecko) " )
        +phiApp->applicationName()+L1( "/" )+PHI::libVersion();
    PHINetworkAccessManager::setUserAgent( tmp );
    tmp=QString::fromLatin1( PHI::phiMimeType() )+L1( ",text/html,application/xml;q=0.9,application/xhtml+xml;q=0.8,"
        "text/plain;q=0.8,image/png,image/*;q=0.6,*/*;q=0.5" );
    PHINetworkAccessManager::setAccept( tmp );
    QSettings *s=phiApp->settings();
    s->beginGroup( SL( "Client" ) );
    tmp=s->value( SL( "AcceptedLanguages" ), SL( "en-us,en") ).toString();
    s->endGroup();
    PHINetworkAccessManager::setAcceptedLangs( tmp );
    if ( phiApp->type()!=PHIApplication::ApacheModule ) _defaultNam=new PHINetworkAccessManager( this, false );
    updateProxy();
    qDebug( "PHINetManager::PHINetManager()" );
}

PHINetManager::~PHINetManager()
{
    delete _defaultNam;
    _instance=0;
    qDebug( "PHINetManager::~PHINetManager()" );
}

void PHINetManager::updateProxy()
{
    QSettings *s=phiApp->settings();
    s->beginGroup( SL( "Client" ) );
    QString proxyHost=s->value( SL( "ProxyHost" ), QString() ).toString();
    quint16 port=s->value( SL( "ProxyPort" ) ).value<quint16>();
    s->endGroup();
    if ( proxyHost.isEmpty() ) {
        _proxy=QNetworkProxy();
        if ( _defaultNam ) _defaultNam->setProxy( _proxy );
        return;
    }
    _proxy.setType( QNetworkProxy::HttpProxy );
    _proxy.setHostName( proxyHost );
    _proxy.setPort( port );
    if ( _defaultNam ) _defaultNam->setProxy( _proxy );
}

PHINetworkAccessManager *PHINetManager::networkAccessManager( QObject *parent )
{
    PHINetworkAccessManager *nam=new PHINetworkAccessManager( parent, true );
    nam->setProxy( _proxy );
    return nam;
}

PHINetworkCookies* PHINetworkCookies::_instance=0;

PHINetworkCookies* PHINetworkCookies::instance()
{
    if ( _instance ) return _instance;
    _instance=new PHINetworkCookies( phiApp );
    return _instance;
}

PHINetworkCookies::PHINetworkCookies( QObject *parent )
    : QNetworkCookieJar( parent )
{
    qDebug( "PHINetworkCookies::PHINetworkCookies()" );
    _dbPath=phiApp->cachePath()+L1( "/cookies.db" );
    QFile f( _dbPath );
    if ( f.open( QIODevice::ReadOnly ) ) {
        QList<QNetworkCookie> list;
        QDataStream ds( &f );
        QByteArray arr;
        qint32 count;
        ds >> count;
        for ( qint32 i=0; i<count; i++ ) {
            ds >> arr;
            QList<QNetworkCookie> cookies=QNetworkCookie::parseCookies( arr );
            list.append( cookies.first() );
        }
        f.close();
        setAllCookies( list );
    }
}

PHINetworkCookies::~PHINetworkCookies()
{
    QFile f( _dbPath );
    if ( f.open( QIODevice::WriteOnly ) ) {
        QList<QNetworkCookie> list=allCookies();
        QDataStream ds( &f );
        ds << static_cast<qint32>(list.count());
        QNetworkCookie cookie;
        foreach ( cookie, list ) ds << cookie.toRawForm();
        f.close();
    }
    _instance=0;
    qDebug( "PHINetworkCookies::~PHINetworkCookies()" );
}

bool PHINetworkCookies::setCookiesFromUrl( const QList<QNetworkCookie> &cookieList, const QUrl &url )
{
    QMutexLocker lock( &_mutex );
    QNetworkCookie cookie;
    foreach ( cookie, cookieList ) {
        qDebug( "set cookie name<%s> path<%s> domain<%s> for url<%s>", cookie.name().data(), qPrintable( cookie.path() ),
            qPrintable( cookie.domain() ), qPrintable( url.toString() ) );
    }
    return QNetworkCookieJar::setCookiesFromUrl( cookieList, url );
}

QList<QNetworkCookie> PHINetworkCookies::cookiesForUrl( const QUrl &url ) const
{
    QMutexLocker lock( &_mutex );
    return QNetworkCookieJar::cookiesForUrl( url );
}

bool PHINetworkCookies::removeCookie( const QString &domain, const QString &name )
{
    QList<QNetworkCookie> list=allCookies();
    QNetworkCookie cookie;
    int row=-1;
    for ( int i=0; i<list.count(); i++ ) {
        cookie=list.at( i );
        if ( cookie.domain()==domain && cookie.name()==name.toUtf8() ) {
            row=i;
            break;
        }
    }
    if ( row>-1 ) list.removeAt( row );
    setAllCookies( list );
    if ( row>-1 ) return true;
    return false;
}
