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
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QFileInfo>
#include "phisrequest.h"

PHISRequest::PHISRequest()
    : _agentEngine( PHISRequest::UnknownEngine ), _agentId( PHISRequest::UnknownAgent ),
    _osType( PHISRequest::UnknownOS ), _engineMajorVersion( -1 ), _engineMinorVersion( -1 ),
    _philang( false ), _contentLength( 0 ), _started( QDateTime::currentDateTime() )
{
    //qDebug( "PHISRequest::PHISRequest" );
}

PHISRequest::~PHISRequest()
{
    QTemporaryFile *tmpFile;
    foreach ( tmpFile, _tmpFiles ) {
        qDebug( "Closing %s", qPrintable( tmpFile->fileName() ) );
        tmpFile->close();
        delete tmpFile;
    }
    _tmpFiles.clear();
    //qDebug( "PHISRequest::~PHISRequest" );
}

quint8 PHISRequest::osType() const
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

void PHISRequest::init() // should be executed AFTER all GET and POST var extractions
{
    QStringList list;
    QString str;
    int pos;

    _modified=QFileInfo( _canonicalFilename ).lastModified();
    // Extract user agent info
    const QByteArray arr=_headers.value( "User-Agent" );
    if ( arr.isEmpty() ) {
        _agentEngine=UnknownEngine;
        _osType=UnknownOS;
        _agentId=UnknownAgent;
    } else {
        if ( (pos=arr.indexOf( "MSIE" ))!=-1 ) {
            _agentEngine=Trident;
            _agentId=IE;
            _engineMajorVersion=arr.mid( pos+5, 1 ).toInt();
            _engineMinorVersion=0;
            if ( _engineMajorVersion==1 ) _engineMajorVersion=6; // MSIE 10
            else if ( _engineMajorVersion>8 ) _engineMajorVersion=5;
            else if ( _engineMajorVersion>7 ) _engineMajorVersion=4;
            else if ( _engineMajorVersion>5 ) {
                _engineMajorVersion=3;
                _engineMinorVersion=1;
            } else _engineMajorVersion=2;
            pos=-1;
        } else if ( (pos=arr.indexOf( "WebKit" ))!=-1 ) {
            if ( arr.indexOf( "Chrome" )!=-1 ) _agentId=Chrome;
            else if ( arr.indexOf( "Konqueror" )!=-1 ) _agentId=Konqueror;
            else if ( arr.indexOf( "Android" )!=-1 ) _agentId=Chrome;
            else if ( arr.indexOf( "Safari" )!=-1 ) _agentId=Safari;
            else if ( arr.indexOf( "Amphibia" )!=-1 ) _agentId=Amphibia;
            else _agentId=UnknownAgent;
            _agentEngine=WebKit;
            pos+=7;
        } else if ( (pos=arr.indexOf( "Firefox" ))!=-1 ) {
            _agentEngine=Gecko;
            _agentId=Firefox;
            pos+=8;
        } else if ( (pos=arr.indexOf( "Presto" ))!=-1 ) {
            _agentEngine=Presto;
            _agentId=Opera;
            pos+=7;
        } else if ( (pos=arr.indexOf( "KHTML" ))!=-1 ) {
            _agentEngine=WebKit;
            _agentId=Konqueror;
            pos+=6;
        } else if ( (pos=arr.indexOf( "SeaMonkey"))!=-1 ) {
            _agentEngine=Gecko;
            _agentId=SeaMonkey;
            pos+=10;
        } else if ( (pos=arr.indexOf( "Trident"))!=-1 ) {
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
    list=QString::fromAscii( _headers.value( "Accept-Language" ) ).split( ',', QString::SkipEmptyParts );
    foreach ( str, list ) {
        pos=str.indexOf( ';' );
        if( pos>0 ) str=str.left( pos ); // remove qualifier
        _acceptedLangs.append( str.toLower() );
    }
    if ( _acceptedLangs.isEmpty() ) _lang="C";
    else _lang=_acceptedLangs.first();

    // Extract cookies
    list=QString::fromUtf8( _headers.value( "Cookie" ) ).split( ';', QString::SkipEmptyParts );
    if ( list.count()>0 ) {
        _cookies.clear();
        QString value, cookie;
        int start;
        foreach ( cookie, list ) {
            if ( cookie.startsWith( ' ' ) ) cookie.remove( 0, 1 );
            if ( cookie.startsWith( '$' ) ) continue;
            start=cookie.indexOf( '=' );
            value=cookie.mid( start+1 );
            if ( value.startsWith( '"' ) ) value.remove( 0, 1 );
            if ( value.endsWith( '"' ) ) value.chop( 1 );
            _cookies.insert( cookie.left( start ), value );
        }
        if ( _cookies.contains( "philang" ) ) {
            _lang=_cookies.value( "philang" );
            _philang=true;
        }
    }
    if ( _postData.contains( "philang" ) ) {
        _lang=_postData.value( "philang" );
        _philang=true;
    }
    if ( !_url.queryItemValue( "philang" ).isEmpty() ) {
        _lang=_url.queryItemValue( "philang" );
        _philang=true;
    }
    _langByteArray=_lang.toAscii();
}

QString PHISRequest::serverValue( const QString &key ) const
{
    if ( key=="documentroot" ) return documentRoot();
    if ( key=="self" ) return _url.path();
    if ( key=="url" ) return _url.toString();
    if ( key=="filename" ) return canonicalFilename();
    if ( key=="hostname" ) return hostname();
    if ( key=="tempdir" ) return tmpDir();
    if ( key=="contentlength" ) return QString::number( _contentLength );
    if ( key=="contenttype" ) {
        QString tmp=contentType();
        int pos=tmp.indexOf( "; boundary" );
        if ( pos>0 ) tmp=tmp.left( pos ); // removing boundary=...
        return tmp;
    }
    if ( key=="postdata" ) {
        QString key, tmp;
        QUrl url;
        QStringList list;
        foreach( key, _postData.uniqueKeys() ) {
            list=_postData.values( key );
            foreach( tmp, list ) url.addQueryItem( key, tmp );
        }
        return url.toString();
    }
    if ( key=="lang" ) return _lang;
    if ( key=="agent" ) return QString::fromUtf8( _headers.value( "User-Agent" ) );
    if ( key=="started" ) return _started.toLocalTime().toString( PHI::dtFormat() );
    if ( key=="today" ) return QDate::currentDate().toString( PHI::isoDateFormat() );
    if ( key=="nowutc" ) return QDateTime::currentDateTime().toUTC().toString( PHI::dtFormat() );
    if ( key=="modified" ) return _modified.toLocalTime().toString( PHI::dtFormat() );
    if ( key=="user" ) return user();
    if ( key=="password" ) return password();
    if ( key=="accept" ) return QString::fromUtf8( _headers.value( "Accept" ) );
    if ( key=="referer" ) return QString::fromUtf8( _headers.value( "Referer" ) );
    if ( key=="scheme" ) return scheme();
    if ( key=="method" ) return method();
    if ( key=="servername" ) return serverDescription();
    if ( key=="serverdef" ) return serverDefname();
    if ( key=="admin" ) return admin();
    if ( key=="serverhost" ) return serverHostname();
    if ( key=="port" ) return QString::number( _url.port() );
    if ( key=="keepalive" ) return QString::number( _keepAlive );
    if ( key=="localaddress" ) return _localIP.toString();
    if ( key=="remoteaddress" ) return _remoteIP.toString();
    if ( key=="version" ) return version();
    if ( key=="useragent" ) return userAgent();
    if ( key=="userengine" ) return userEngine();
    if ( key=="useros" ) return userOS();
    return QString();
}

QString PHISRequest::userEngine() const
{
    switch ( _agentEngine ) {
    case Trident: return QString( "Trident" );
    case WebKit: return QString( "WebKit" );
    case Gecko: return QString( "Gecko" );
    case Presto: return QString( "Presto" );
    default:;
    }
    return QString( "Unknown" );
}

QString PHISRequest::userAgent() const
{
    switch( _agentId ) {
    case IE: return QString( "MSIE" );
    case Safari: return QString( "Safari" );
    case Firefox: return QString( "Firefox" );
    case Chrome: return QString( "Chrome" );
    case Konqueror: return QString( "Konqueror" );
    case Opera: return QString( "Opera" );
    case SeaMonkey: return QString( "SeaMonkey" );
    case Amphibia: return QString( "Amphibia" );
    default:;
    }
    return QString( "Unknown" );
}

QString PHISRequest::userOS() const
{
    switch( osType() ) {
    case PHISRequest::Windows: return QString( "Windows" );
    case PHISRequest::MacOS: return QString( "MacOSX" );
    case PHISRequest::iOS: return QString( "iOS" );
    case PHISRequest::Android: return QString( "Android" );
    case PHISRequest::WindowsMobile: return QString( "WindowsMobile" );
    case PHISRequest::Linux: return QString( "Linux" );
    case PHISRequest::Symbian: return QString( "Symbian" );
    case PHISRequest::Solaris: return QString( "Solaris" );
    case PHISRequest::HPUX: return QString( "HPUX" );
    case PHISRequest::AIX: return QString( "AIX" );
    default:;
    }
    return QString( "Unknown" );
}

bool PHISRequest::setRedirectedFile( const QString &file )
{
    QFileInfo info( _canonicalFilename );
    if ( file.startsWith( '/' ) || file.startsWith( '\\' ) ) {
        info.setFile( _documentRoot+file );
        if ( !info.exists() ) return false;
    } else {
        info.setFile( info.absolutePath(), file );
        if ( !info.exists() ) return false;
    }
    _canonicalFilename=info.absoluteFilePath();
    _modified=info.lastModified();
    QString path=_canonicalFilename;
    qDebug( "path=%s (%s)", qPrintable( path ), qPrintable( _documentRoot ) );
    path.replace( _documentRoot, "" );
    _url.setPath( path );
    qDebug( "REDIRECTED: %s", qPrintable( _url.toString() ) );
    return true;
}

void PHISRequest::dump() const
{
    QStringList list;
    QString str, key;
    quint8 id;
    qWarning( "[Config]" );
    qWarning( "Started: %s", qPrintable( _started.toLocalTime().toString( PHI::dtFormat() ) ) );
    qWarning( "UTC: %s", qPrintable( serverValue( "nowutc" ) ) );
    qWarning( "Filename: %s", qPrintable( _canonicalFilename ) );
    qWarning( "TmpDir: %s", qPrintable( _tmpDir ) );
    qWarning( "ImgDir: %s", qPrintable( _imgDir ) );
    qWarning( "Keep-Alive: %d", _keepAlive );
    qWarning( "DocumentRoot: %s", qPrintable( _documentRoot ) );
    qWarning( "URL: %s", _url.toEncoded().data() );
    qWarning( "POST data: %s", qPrintable( serverValue( "postdata" ) ) );
    qWarning( "HTTP major: %d", _httpMajor );
    qWarning( "HTTP minor: %d", _httpMinor );
    qWarning( "Content length: %lld", _contentLength );
    qWarning( "Current language: %s", qPrintable( _lang ) );
    QStringList engines=QStringList() << "Unknown" << "Trident" << "Gecko" << "WebKit" << "Presto";
    qWarning( "Engine: %s", qPrintable( engines.at( agentEngine() ) ) );
    qWarning( "Engine version: %d.%d", _engineMajorVersion, _engineMinorVersion );
    QStringList agents=QStringList() << "Unknown" << "InternetExplorer" << "Firefox" << "Konqueror"
        << "Safari" << "Chrome" << "Opera" << "Amphibia";
    qWarning( "Agent: %s", qPrintable( agents.at( agentId() ) ) );
    QStringList ostypes=QStringList() << "Unknown" << "Windows" << "Linux" << "Mac OS" << "iOS" << "Symbian"
        << "WindowsMobile" << "Android";
    qWarning( "OS type: %s", qPrintable( ostypes.at( osType() ) ) );
    qWarning( "User: %s", qPrintable( user() ) );
    qWarning( "Password: %s", qPrintable( password() ) );
    qWarning( "[Keywords]" );
    foreach ( id, _keywords.keys() ) {
        qWarning( "%d %s", id, _keywords.value( id ).data() );
    }
    qWarning( "[Languages]" );
    foreach ( str, acceptedLanguages() ) qWarning( "%s", qPrintable( str ) );
    qWarning( "[Header]" );
    foreach ( str, headerKeys() ) {
        qWarning( "%s=%s", qPrintable( str ), qPrintable( headerValue( str ) ) );
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
