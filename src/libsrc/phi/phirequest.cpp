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
#include <QFileInfo>
#include <QDir>
#include "phirequest.h"

quint8 PHIRequest::osType() const
{
    if ( _osType!=UnknownOS ) return _osType;
    const QByteArray arr=_headers.value( "User-Agent" );
    if ( arr.indexOf( "IEMobile" )!=-1 )_osType=WindowsMobile;
    else if ( arr.indexOf( "Windows" )!=-1 ) _osType=Windows;
    else if ( arr.indexOf( "iPhone" )!=-1 ) _osType=iOS;
    else if ( arr.indexOf( "iPad" )!=-1 ) _osType=iOS;
    else if ( arr.indexOf( "Mac OS" )!=-1 ) _osType=MacOS;
    else if ( arr.indexOf( "Android" )!=-1 ) _osType=Android;
    else if ( arr.indexOf( "Linux" )!=-1 ) _osType=Linux;
    else if ( arr.indexOf( "Symbian" )!=-1 ) _osType=Symbian;
    else if ( arr.indexOf( "Solaris" )!=-1 ) _osType=Solaris;
    else if ( arr.indexOf( "HPUX" )!=-1 ) _osType=HPUX;
    else if ( arr.indexOf( "AIX" )!=-1 ) _osType=AIX;
    return _osType;
}

void PHIRequest::init() // must be executed AFTER all GET and POST var extractions
{
    int pos;
    _modified=QFileInfo( _canonicalFilename ).lastModified();
    // Extract user agent info
    QByteArray arr=_headers.value( BL( "User-Agent" ) );
    if ( arr.isEmpty() ) {
        _agentEngine=UnknownEngine;
        _osType=UnknownOS;
        _agentId=UnknownAgent;
    } else {
        if ( (pos=arr.indexOf( BL( "MSIE" ) ))!=-1 ) {
            _agentEngine=Trident;
            _agentId=IE;
            _engineMajorVersion=arr.mid( pos+5, 1 ).toInt();
            _engineMinorVersion=0;
            if ( _engineMajorVersion==1 ) {
                if ( arr.mid( pos+6 ).toInt()>0 ) _engineMajorVersion=7; // MSIE 11
                else _engineMajorVersion=6; // MSIE 10
            } else if ( _engineMajorVersion>8 ) _engineMajorVersion=5; // MSIE 9
            else if ( _engineMajorVersion>7 ) _engineMajorVersion=4; // MSIE 8
            else if ( _engineMajorVersion>5 ) {
                _engineMajorVersion=3; // MSIE 6-7
                _engineMinorVersion=1;
            } else _engineMajorVersion=2; // MSIE <6
            pos=-1;
        } else if ( (pos=arr.indexOf( BL( "WebKit" ) ))!=-1 ) {
            if ( arr.indexOf( BL( "Chrome" ) )!=-1 ) _agentId=Chrome;
            else if ( arr.indexOf( BL( "Konqueror" ) )!=-1 ) _agentId=Konqueror;
            else if ( arr.indexOf( BL( "Android" ) )!=-1 ) _agentId=Chrome;
            else if ( arr.indexOf( BL( "Safari" ) )!=-1 ) _agentId=Safari;
            else if ( arr.indexOf( BL( "Opera" ) )!=-1 ) _agentId=Opera;
            else if ( arr.indexOf( BL( "Amphibia" ) )!=-1 ) _agentId=Amphibia;
            else if ( arr.indexOf( BL( "Phis" ) )!=-1 ) _agentId=Phis;
            else _agentId=UnknownAgent;
            _agentEngine=WebKit;
            pos+=7;
        } else if ( (pos=arr.indexOf( BL( "Blink" ) ))!=-1 ) {
            if ( arr.indexOf( BL( "Chrome" ) )!=-1 ) _agentId=Chrome;
            else if ( arr.indexOf( BL( "Opera" ) )!=-1 ) _agentId=Opera;
            _agentEngine=Blink;
            pos+=6;
        } else if ( (pos=arr.indexOf( BL( "Firefox" ) ))!=-1 ) {
            _agentEngine=Gecko;
            _agentId=Firefox;
            pos+=8;
        } else if ( (pos=arr.indexOf( BL( "Presto" ) ))!=-1 ) {
            _agentEngine=Presto;
            _agentId=Opera;
            pos+=7;
        } else if ( (pos=arr.indexOf( BL( "KHTML" ) ))!=-1 ) {
            _agentEngine=WebKit;
            _agentId=Konqueror;
            pos+=6;
        } else if ( (pos=arr.indexOf( BL( "SeaMonkey" ) ))!=-1 ) {
            _agentEngine=Gecko;
            _agentId=SeaMonkey;
            pos+=10;
        } else if ( (pos=arr.indexOf( BL( "Trident" ) ))!=-1 ) {
            _agentEngine=Trident;
            _agentId=UnknownAgent;
            pos+=8;
        } else {
            pos=-1;
            _agentEngine=UnknownEngine;
            _agentId=UnknownAgent;
        }

        if ( pos!=-1 ) {
            int start=pos;
            pos=arr.indexOf( '.', pos );
            if ( pos>0 ) {
                _engineMajorVersion=arr.mid( start, pos-start ).toInt();
                start=pos+1;
                pos=arr.indexOf( '.', start );
                if ( pos>0 && pos<(start+4) ) {
                    //qDebug( "----<%s>---", arr.mid( start, pos-start ).data() );
                    _engineMinorVersion=arr.mid( start, pos-start ).toInt();
                } else {
                    pos=arr.indexOf( ';', start );
                    if ( pos>0 && pos<(start+4) ) {
                        _engineMinorVersion=arr.mid( start, pos-start ).toInt();
                    } else {
                        pos=arr.indexOf( ' ', start );
                        if ( pos>0 && pos<(start+4) ) {
                            _engineMinorVersion=arr.mid( start, pos-start ).toInt();
                        } else {
                            pos=arr.length();
                            if ( pos>start ) _engineMinorVersion=arr.mid( start, pos-start ).toInt();
                        }
                    }
                }
            }
            if ( _agentId==Firefox ) {
                if ( _engineMajorVersion>9 ) { // >=10.x series
                    _engineMajorVersion=10;
                    _engineMinorVersion=0;
                } else if ( _engineMajorVersion>4 ) { // >=5.x series
                    _engineMinorVersion=0;
                } else if ( _engineMajorVersion>3 ) { // 4.x series
                    _engineMajorVersion=2;
                    _engineMinorVersion=0;
                } else if ( _engineMajorVersion>2 ) { // 3.x series
                    _engineMajorVersion=1;
                    _engineMinorVersion=9;
                } else if ( _engineMajorVersion>1 ) { // 2.x series
                    _engineMajorVersion=1;
                    _engineMinorVersion=8;
                } else { // 0-1.x series
                    _engineMajorVersion=1;
                    _engineMinorVersion=7;
                }
            } else if ( _agentId==SeaMonkey ) {
                if ( _engineMajorVersion==2 ) {
                    if ( _engineMinorVersion>6 ) {
                        _engineMajorVersion=10;
                        _engineMinorVersion=0;
                    } else if ( _engineMinorVersion>0 ) {
                        _engineMajorVersion=2;
                        _engineMinorVersion=0;
                    } else {
                        _engineMajorVersion=1;
                        _engineMinorVersion=9;
                    }
                } else if ( _engineMajorVersion<2 ) {
                    _engineMajorVersion=1;
                    _engineMinorVersion=8;
                } else {
                    _engineMajorVersion=2;
                    _engineMinorVersion=0;
                }
            }
        }
    }
    // Extract languages (excluding qualifier ie. ';q=0.8')
    PHIByteArrayList langs=_headers.value( BL( "Accept-Language" ) ).split( ',' );
    foreach ( arr, langs ) {
        pos=arr.indexOf( ';' );
        if ( pos>0 ) arr=arr.left( pos ); // remove qualifier
        if ( arr.size()>4 ) {
            QByteArray subLang=arr.mid( 3 ).toUpper();
            arr=arr.left( 2 )+'_'+subLang;
            _acceptedLangs.append( arr );
        }
        _acceptedLangs.append( arr.left( 2 ) );
    }
    if ( _acceptedLangs.isEmpty() ) _acceptedLangs.append( BL( "C" ) );
    else _lang=QString::fromLatin1( _acceptedLangs.first() );

    // Extract cookies
    static const QString philang=L1( "philang" );
    if ( _headers.contains( BL( "Cookie" ) ) ) {
        QStringList list=QString::fromUtf8( _headers.value( BL( "Cookie" ) ) ).split( QLatin1Char( ';' ), QString::SkipEmptyParts );
        _cookies.clear();
        QString value, cookie;
        int start;
        foreach ( cookie, list ) {
            if ( cookie.startsWith( QLatin1Char( ' ' ) ) ) cookie.remove( 0, 1 );
            if ( cookie.startsWith( QLatin1Char( '$' ) ) ) continue;
            start=cookie.indexOf( QLatin1Char( '=' ) );
            value=cookie.mid( start+1 );
            if ( value.startsWith( QLatin1Char( '"' ) ) ) value.remove( 0, 1 );
            if ( value.endsWith( QLatin1Char( '"' ) ) ) value.chop( 1 );
            _cookies.insert( cookie.left( start ), value );
        }
        if ( Q_UNLIKELY( _cookies.contains( philang ) ) ) {
            _lang=_cookies.value( philang );
            _philang=true;
        }
    }
    if ( _postData.contains( philang ) ) {
        _lang=_postData.value( philang );
        _philang=true;
    }
    if ( !QUrlQuery( _url ).queryItemValue( philang ).isEmpty() ) {
        _lang=QUrlQuery( _url ).queryItemValue( philang );
        _philang=true;
    }
    _langByteArray=_lang.toLatin1();

    switch( _agentEngine ) {
    case Trident:
        _agentPrefix=BL( "-ms-" );
        if ( Q_UNLIKELY( _engineMajorVersion < 5 ) ) { // IE678
            _agentFeatures = None | IE678;
        } else if ( Q_UNLIKELY( _engineMajorVersion==5 ) ) { // IE9
            _agentFeatures = None | BoxShadow | RGBA | Transform2D
            | BorderRadius | SVG | Canvas | H264 | Audio;
        }
        break;
    case WebKit:
        _agentPrefix=BL( "-webkit-" );
        if ( Q_UNLIKELY( _engineMajorVersion < 534 ) ) { // Safari 4.0, Chrome 3 & 4
            _agentFeatures = None | BoxShadow | TextShadow | RGBA | Transform2D
            | Transform3D | Transitions | BorderRadius | Canvas | H264 | Audio;
            if ( _engineMajorVersion==533 ) _agentFeatures |= HTML5;
        } else {
            if ( _agentId==Chrome ) _agentFeatures |= H264 | Ogg; // Chrome >= 10
            else if ( _agentId==Safari ) _agentFeatures |= H264; // Safari >= 5.1
            else if ( _agentId==Opera ) _agentFeatures |= Ogg; // Opera >= 15
        }
        break;
    case Gecko:
        _agentPrefix=BL( "-moz-" );
        if ( Q_UNLIKELY( _engineMajorVersion < 2 ) ) {
            if ( Q_UNLIKELY( _engineMinorVersion < 9 ) ) { // Firefox 2.x
                _agentFeatures = None | RGBA | BorderRadius;
            } else { // Firefox 3.x
                _agentFeatures = None | BoxShadow | TextShadow | RGBA | Transform2D
                | BorderRadius | Canvas | MathML | Ogg | Audio;
            }
        } else if ( Q_UNLIKELY( _engineMajorVersion < 10 ) ) { // Firefox 4-9
                _agentFeatures &= ~Transform3D;
                _agentFeatures |= Ogg;
        } else { // Firefox >= 10
            if ( _osType==Windows && _engineMajorVersion > 15 ) _agentFeatures |= H264;
        }
        break;
    case Presto:
        _agentPrefix=BL( "-o-" );
        if ( Q_UNLIKELY( _engineMinorVersion < 3 ) ) { // Opera 9 & 10.0
            _agentFeatures = None | TextShadow | Canvas;
        } else if ( Q_UNLIKELY( _engineMinorVersion < 9 ) ) { // Opera 10.5-11.5
            _agentFeatures &= ~Gradients & ~Transform3D;
            _agentFeatures |= Ogg;
        } else { // Opera 11.6-12.1 (Opera >=15 uses WebKit
            _agentFeatures &= ~Transform3D;
            _agentFeatures |= Ogg;
        }
        break;
    default:;
    }
}

QString PHIRequest::serverValue( const QString &key ) const
{
    if ( key==L1( "documentroot" ) ) return documentRoot();
    if ( key==L1( "self" ) ) return _url.path();
    if ( key==L1( "url" ) ) return _url.toString();
    if ( key==L1( "filename" ) ) return canonicalFilename();
    if ( key==L1( "hostname" ) ) return hostname();
    if ( key==L1( "tempdir" ) ) return tmpDir();
    if ( key==L1( "contentlength" ) ) return QString::number( _contentLength );
    if ( key==L1( "contenttype" ) ) {
        QString tmp=contentType();
        int pos=tmp.indexOf( L1( "; boundary" ) );
        if ( pos>0 ) tmp=tmp.left( pos ); // removing boundary=...
        return tmp;
    }
    if ( key==L1( "postdata" ) ) {
        QString key, tmp;
        QUrl url;
        QUrlQuery query;
        QStringList list;
        foreach( key, _postData.uniqueKeys() ) {
            list=_postData.values( key );
            foreach( tmp, list ) query.addQueryItem( key, tmp );
        }
        url.setQuery( query );
        return url.toString();
    }
    if ( key==L1( "lang" ) ) return _lang;
    if ( key==L1( "agent" ) ) return QString::fromUtf8( _headers.value( "User-Agent" ) );
    if ( key==L1( "started" ) ) return _started.toLocalTime().toString( PHI::dtFormatString() );
    if ( key==L1( "today" ) ) return QDate::currentDate().toString( QString::fromLatin1( PHI::isoDateFormat() ) );
    if ( key==L1( "nowutc" ) ) return QDateTime::currentDateTime().toUTC().toString( PHI::dtFormatString() );
    if ( key==L1( "modified" ) ) return _modified.toLocalTime().toString( PHI::dtFormatString() );
    if ( key==L1( "user" ) ) return user();
    if ( key==L1( "password" ) ) return password();
    if ( key==L1( "accept" ) ) return QString::fromUtf8( _headers.value( "Accept" ) );
    if ( key==L1( "referer" ) ) return QString::fromUtf8( _headers.value( "Referer" ) );
    if ( key==L1( "scheme" ) ) return scheme();
    if ( key==L1( "method" ) ) return method();
    if ( key==L1( "servername" ) ) return serverDescription();
    if ( key==L1( "serverdef" ) ) return serverDefname();
    if ( key==L1( "admin" ) ) return admin();
    if ( key==L1( "serverhost" ) ) return serverHostname();
    if ( key==L1( "port" ) ) return QString::number( _url.port() );
    if ( key==L1( "keepalive" ) ) return QString::number( _keepAlive );
    if ( key==L1( "localaddress" ) ) return _localIP.toString();
    if ( key==L1( "remoteaddress" ) ) return _remoteIP.toString();
    if ( key==L1( "version" ) ) return version();
    if ( key==L1( "useragent" ) ) return userAgent();
    if ( key==L1( "userengine" ) ) return userEngine();
    if ( key==L1( "useros" ) ) return userOS();
    return QString();
}

const QString& PHIRequest::userEngine() const
{
    static QString trident=L1( "Trident" );
    static QString webkit=L1( "WebKit" );
    static QString gecko=L1( "Gecko" );
    static QString presto=L1( "Presto" );
    static QString blink=L1( "Blink" );
    static QString unknown=L1( "Unknown" );

    switch ( _agentEngine ) {
    case Trident: return trident;
    case WebKit: return webkit;
    case Gecko: return gecko;
    case Blink: return blink;
    case Presto: return presto;
    default:;
    }
    return unknown;
}

const QString& PHIRequest::userAgent() const
{
    static QString msie=L1( "MSIE" );
    static QString safari=L1( "Safari" );
    static QString ff=L1( "Firefox" );
    static QString chrome=L1( "Chrome" );
    static QString konq=L1( "Konqueror" );
    static QString opera=L1( "Opera" );
    static QString sea=L1( "SeaMonkey" );
    static QString amph=L1( "Amphibia" );
    static QString unknown=L1( "Unknown" );

    switch( _agentId ) {
    case IE: return msie;
    case Safari: return safari;
    case Firefox: return ff;
    case Chrome: return chrome;
    case Konqueror: return konq;
    case Opera: return opera;
    case SeaMonkey: return sea;
    case Amphibia: return amph;
    default:;
    }
    return unknown;
}

// @todo: make them static
QString PHIRequest::userOS() const
{
    switch( osType() ) {
    case PHIRequest::Windows: return SL( "Windows" );
    case PHIRequest::MacOS: return SL( "MacOSX" );
    case PHIRequest::iOS: return SL( "iOS" );
    case PHIRequest::Android: return SL( "Android" );
    case PHIRequest::WindowsMobile: return SL( "WindowsMobile" );
    case PHIRequest::Linux: return SL( "Linux" );
    case PHIRequest::Symbian: return SL( "Symbian" );
    case PHIRequest::Solaris: return SL( "Solaris" );
    case PHIRequest::HPUX: return SL( "HPUX" );
    case PHIRequest::AIX: return SL( "AIX" );
    default:;
    }
    return SL( "Unknown" );
}

bool PHIRequest::setRedirectedFile( const QString &file )
{

    QUrl url( SL( "file://" )+file );
    QFileInfo fi( url.toLocalFile() );
    if ( !fi.exists() ) return false;
    _canonicalFilename=fi.canonicalFilePath();
    _modified=fi.lastModified();
    QString path=_canonicalFilename;
    path.replace( _documentRoot, QString() );
    _url.setPath( path );
    _url.setQuery( url.query() );
    qDebug() << "REDIRECTED" << _url.toString() << _postData;
    return true;
}

QDateTime PHIRequest::dateTimeFromHeader( const QByteArray &modified )
{
    QByteArray tmp=modified.mid( 5 ), arr;
    if ( tmp.endsWith( " GMT" ) ) arr=tmp.left( tmp.length()-4 );
    else arr=tmp;
    tmp=arr.right( 8 );
    QTime time=QTime::fromString( QString::fromLatin1( tmp ), SL( "hh:mm:ss" ) ).addMSecs( 999 );
    if ( !time.isValid() ) {
        qDebug( "------------------>>>> If-Modified-Since is invalid" );
        return QDateTime( QDate( 1970, 1, 1 ) );
    }
    if ( arr.length()==20 ) {
        tmp=arr.left( 2 );
        arr.remove( 0, 3 );
    } else {
        tmp=arr.left( 1 );
        arr.remove( 0, 2 );
    }
    arr.chop( 9 );
    int d=tmp.toInt();
    int y=arr.right( 4 ).toInt();
    int m=1;
    arr.chop( 5 );
    if ( arr=="Feb" ) m=2;
    else if ( arr=="Mar" ) m=3;
    else if ( arr=="Apr" ) m=4;
    else if ( arr=="May" ) m=5;
    else if ( arr=="Jun" ) m=6;
    else if ( arr=="Jul" ) m=7;
    else if ( arr=="Aug" ) m=8;
    else if ( arr=="Sep" ) m=9;
    else if ( arr=="Oct" ) m=10;
    else if ( arr=="Nov" ) m=11;
    else if ( arr=="Dec" ) m=12;
    //qDebug( "DATETIME %s", qPrintable( QDateTime( QDate( y, m, d ), time, Qt::UTC ).toString() ) );
    return QDateTime( QDate ( y, m, d ), time, Qt::UTC );
}

void PHIRequest::dump() const
{
    QStringList list;
    QString str, key;
    quint8 id;
    qWarning( "[Config]" );
    qWarning( "Started: %s", qPrintable( _started.toLocalTime().toString( PHI::dtFormatString() ) ) );
    qWarning( "UTC: %s", qPrintable( serverValue( SL( "nowutc" ) ) ) );
    qWarning( "Filename: %s", qPrintable( _canonicalFilename ) );
    qWarning( "TmpDir: %s", qPrintable( _tmpDir ) );
    qWarning( "ImgDir: %s", qPrintable( _imgDir ) );
    qWarning( "Keep-Alive: %d", _keepAlive );
    qWarning( "DocumentRoot: %s", qPrintable( _documentRoot ) );
    qWarning( "URL: %s", _url.toEncoded().data() );
    qWarning( "POST data: %s", qPrintable( serverValue( SL( "postdata" ) ) ) );
    qWarning( "HTTP major: %d", _httpMajor );
    qWarning( "HTTP minor: %d", _httpMinor );
    qWarning( "Content length: %lld", _contentLength );
    qWarning( "Current language: %s", qPrintable( _lang ) );
    //QStringList engines=QStringList() << "Unknown" << "Trident" << "Gecko" << "WebKit" << "Presto";
    //qWarning( "Engine: %s", qPrintable( engines.at( agentEngine() ) ) );
    qWarning( "Engine: %d", agentEngine() );
    qWarning( "Engine version: %d.%d", _engineMajorVersion, _engineMinorVersion );
    //QStringList agents=QStringList() << "Unknown" << "InternetExplorer" << "Firefox" << "Konqueror"
    //    << "Safari" << "Chrome" << "Opera" << "Amphibia";
    //qWarning( "Agent: %s", qPrintable( agents.at( agentId() ) ) );
    qWarning( "Agent: %d", agentId() );
    //QStringList ostypes=QStringList() << "Unknown" << "Windows" << "Linux" << "Mac OS" << "iOS" << "Symbian"
    //    << "WindowsMobile" << "Android";
    //qWarning( "OS type: %s", qPrintable( ostypes.at( osType() ) ) );
    qWarning( "OS type: %d", osType() );
    qWarning( "User: %s", qPrintable( user() ) );
    qWarning( "Password: %s", qPrintable( password() ) );
    qWarning( "[Keywords]" );
    foreach ( id, _keywords.keys() ) {
        qWarning( "%d %s", id, _keywords.value( id ).data() );
    }
    qWarning( "[Languages]" );
    foreach ( QByteArray lang, acceptedLanguages() ) qWarning( "%s", lang.constData() );
    qWarning( "[Header]" );
    foreach ( str, headerKeys() ) {
        qWarning( "%s=%s", qPrintable( str ), headerValue( str.toLatin1() ).constData() );
    }
    qWarning( "[Cookies]" );
    foreach ( str, cookieKeys() ) {
        list=cookieValues( str );
        foreach( key, list ) qWarning( "%s=%s", qPrintable( str ), qPrintable( key ) );
    }
    qWarning( "[GET]" );
    foreach ( str, getKeys() ) {
        list=getValues( str );
        foreach( key, list ) qWarning( "%s=%s", qPrintable( str ), qPrintable( key ) );
    }
    qWarning( "[POST]" );
    foreach ( str, postKeys() ) {
        list=getValues( str );
        foreach( key, list ) qWarning( "%s=%s", qPrintable( str ), qPrintable( key ) );
    }
}
