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
#ifndef PHISREQUEST_H
#define PHISREQUEST_H

#include <QByteArray>
#include <QStringList>
#include <QHash>
#include <QHostAddress>
#include <QUrl>
#include <QUrlQuery>
#include <QDateTime>
#include <QTemporaryFile>
#include <QHostAddress>
#include "phis.h"
#include "phiresponserec.h"

class PHISEXPORT PHISRequest
{
public:
    enum AgentEngine { UnknownEngine=0, Trident=1, Gecko=2, WebKit=3, Presto=4, MaxAgentEngine=5 };
    enum AgentId { UnknownAgent=0, IE=1, Firefox=2, Konqueror=3, Safari=4, Chrome=5, Opera=6,
        Amphibia=7, SeaMonkey=8, MaxAgentId=9 };
    enum OSType { UnknownOS=0, Windows=1, Linux=2, MacOS=3, iOS=4, Symbian=5, WindowsMobile=6,
        Android=7, Solaris=8, AIX=9, HPUX=10, MaxOSType=11 };
    enum Key { KMethod=1, KContentType, KDefName, KAdmin, KServerDesc, KServerHostname };

    explicit PHISRequest();
    virtual ~PHISRequest();

    inline quint8 agentEngine() const { return _agentEngine; }
    inline void setAgentEngine( quint8 id ) const { id < MaxAgentEngine ? _agentEngine=id : _agentEngine=UnknownEngine; }
    inline quint8 agentId() const { return _agentId; }
    inline void setAgentId( quint8 id ) const { id < MaxAgentId ? _agentId=id : _agentId=UnknownAgent; }
    inline void setOSType( quint8 type ) const { type < MaxOSType ? _osType=type : _osType=0; }
    inline qint32 engineMajorVersion() const { return _engineMajorVersion; }
    inline void setEngineMajorVersion( qint32 major ) const { _engineMajorVersion=major; }
    inline qint32 engineMinorVersion() const { return _engineMinorVersion; }
    inline void setEngineMinorVersion( qint32 minor ) const { _engineMinorVersion=minor; }
    inline QString currentLang() const { return _lang; }
    inline void setCurrentLang( const QString &l ) const { _lang=l; _langByteArray=_lang.toLatin1(); }

    inline QByteArray currentLangByteArray() const { return _langByteArray; }
    inline qint8 httpServerMajorVersion() const { return _httpMajor; }
    inline qint8 httpServerMinorVersion() const { return _httpMinor; }
    inline qint32 keepAlive() const { return _keepAlive; }
    inline qint64 contentLength() const { return _contentLength; }
    inline bool isPhiLangRequest() const { return _philang; }

    //inline QTemporaryFile* tmpFile( const QString &key ) const { return _tmpFiles.value( key.toUtf8(), 0 ); }
    inline QUrl url() const { return _url; }
    inline int port() const { return _url.port(); }
    inline QDateTime started() const { return _started; }
    inline QDateTime lastModified() const { return _modified; }
    inline QByteArray keyword( Key k ) const { return _keywords.value( k ); }
    inline QStringList acceptedLanguages() const { return _acceptedLangs; } // without qualifier ';q=x.x'
    inline PHIResponseRec* responseRec() const { return _resp; }

    inline QString documentRoot() const { return _documentRoot; }
    inline QString tmpDir() const { return _tmpDir; }
    inline QString imgDir() const { return _imgDir; }
    inline QString header( const QString &key ) const { return QString::fromUtf8( _headers.value( key.toUtf8() ) ); }
    inline QString canonicalFilename() const { return _canonicalFilename; }
    inline QString hostname() const { return _url.host(); }
    inline QString user() const { return _url.userName(); }
    inline QString password() const { return _url.password(); }
    inline QString method() const { return QString::fromLatin1( _keywords.value( KMethod ) ); }
    inline QString contentType() const { return QString::fromLatin1( _keywords.value( KContentType ) ); }
    inline QString admin() const { return QString::fromUtf8( _keywords.value( KAdmin ) ); }
    inline QString serverDefname() const { return QString::fromUtf8( _keywords.value( KDefName ) ); }
    inline QString scheme() const { return _url.scheme(); }
    inline QString serverDescription() const { return QString::fromUtf8( _keywords.value( KServerDesc ) ); }
    inline QString serverHostname() const { return QString::fromUtf8( _keywords.value( KServerHostname ) ); }
    inline QString remoteIP() const { return _remoteIP.toString(); }
    inline QString localIP() const { return _localIP.toString(); }
    inline QString referer() const { return QString::fromUtf8( _headers.value( QByteArrayLiteral( "Referer" ) ) ); }
    inline QString agent() const { return QString::fromUtf8( _headers.value( QByteArrayLiteral( "User-Agent" ) ) ); }
    inline QString accept() const { return QString::fromUtf8( _headers.value( QByteArrayLiteral( "Accept" ) ) ); }
    inline QString version() const { return PHIS::libVersion(); }

    inline QString cookieValue( const QString &key ) const {  return _cookies.value( key ); }
    inline QString getValue( const QString &key ) const { return QUrlQuery( _url ).queryItemValue( key ); }
    inline QString postValue( const QString &key ) const { return _postData.value( key ); }
    inline QString headerValue( const QString &key ) const { return QString::fromUtf8( _headers.value( key.toUtf8() ) ); }
    inline QStringList postKeys() const { return _postData.uniqueKeys(); }
    inline QStringList cookieKeys() const { return _cookies.uniqueKeys(); }
    inline QStringList requestKeys() const { return cookieKeys()+postKeys()+getKeys(); }
    inline QStringList getValues( const QString &key ) const { return QUrlQuery( _url ).allQueryItemValues( key ); }
    inline QStringList postValues( const QString &key ) const { return _postData.values( key ); }
    inline QStringList cookieValues( const QString &key ) const {  return _cookies.values( key ); }

    inline QString fileName( const QString &key ) const {
        QTemporaryFile *f=_tmpFiles.value( key.toUtf8(), 0 );
        if ( f ) return f->objectName();
        return QString();
    }
    inline QString tmpFile( const QString &key ) const {
        QTemporaryFile *f=_tmpFiles.value( key.toUtf8(), 0 );
        if ( f ) return f->fileName();
        return QString();
    }
    inline QString fileSize( const QString &key ) const {
        QTemporaryFile *f=_tmpFiles.value( key.toUtf8(), 0 );
        if ( f ) return QString::number( f->size() );
        return QString();
    }

    QString serverValue( const QString &key ) const;
    QString requestValue( const QString &key ) const;
    QString userAgent() const;
    QString userEngine() const;
    QString userOS() const;
    QStringList requestValues( const QString &key ) const;
    QStringList getKeys() const;
    QStringList serverKeys() const;
    QStringList headerKeys() const;
    QStringList fileKeys() const;
    QDateTime ifModifiedSince() const;
    quint8 osType() const;

    void dump() const;
    void init();
    bool setRedirectedFile( const QString &file );

protected:
    mutable QString _lang;
    mutable QByteArray _langByteArray;
    mutable quint8 _agentEngine, _agentId, _osType;
    mutable qint32 _engineMajorVersion, _engineMinorVersion;
    bool _philang;
    qint8 _httpMajor, _httpMinor;
    qint64 _contentLength;
    qint32 _keepAlive;
    QDateTime _started, _modified;
    QUrl _url;
    QStringList _acceptedLangs;
    QString _documentRoot, _tmpDir, _imgDir, _canonicalFilename;
    QHostAddress _localIP, _remoteIP;
    QHash <QByteArray, QByteArray> _headers;
    QHash <QByteArray, QTemporaryFile*> _tmpFiles;
    QHash <QString, QString> _cookies, _postData;
    QHash <quint8, QByteArray> _keywords;
    PHIResponseRec *_resp;
};

inline QDateTime PHISRequest::ifModifiedSince() const
{
    QByteArray s=_headers.value( QByteArrayLiteral( "If-Modified-Since" ) );
    if ( s.isEmpty() ) return QDateTime( QDate( 1970, 1, 1 ) );
    return PHI::dateTimeFromHeader( s );
}

inline QStringList PHISRequest::getKeys() const
{
    QList <QPair <QString, QString> > list=QUrlQuery( _url ).queryItems();
    QPair <QString, QString> pair;
    QStringList keys;
    foreach ( pair, list ) keys.append( pair.first );
    keys.removeDuplicates();
    return keys;
}

inline QStringList PHISRequest::headerKeys() const
{
    QStringList list;
    QByteArray key;
    foreach ( key, _headers.keys() ) list.append( QString::fromUtf8( key ) );
    return list;
}

inline QStringList PHISRequest::fileKeys() const
{
    QStringList list;
    QByteArray key;
    foreach ( key, _tmpFiles.keys() ) list.append( QString::fromUtf8( key ) );
    return list;
}

inline QStringList PHISRequest::serverKeys() const
{
    static QStringList list=QStringList() << QStringLiteral( "documentroot" )
        << QStringLiteral( "self" ) << QStringLiteral( "url" ) << QStringLiteral( "filename" )
        << QStringLiteral( "hostname" ) << QStringLiteral( "tempdir" ) << QStringLiteral( "contentlength" )
        << QStringLiteral( "contentType" ) << QStringLiteral( "postdata" ) << QStringLiteral( "agent" )
        << QStringLiteral( "started" ) << QStringLiteral( "today" ) << QStringLiteral( "nowutc" )
        << QStringLiteral( "modified" ) << QStringLiteral( "user" ) << QStringLiteral( "password" )
        << QStringLiteral( "accept" ) << QStringLiteral( "scheme" ) << QStringLiteral( "method" )
        << QStringLiteral( "servername" ) << QStringLiteral( "serverdef" ) << QStringLiteral( "admin" )
        << QStringLiteral( "serverhost" ) << QStringLiteral( "port" ) << QStringLiteral( "keepalive" )
        << QStringLiteral( "localaddress" ) << QStringLiteral( "remoteaddress" )
        << QStringLiteral( "version" ) << QStringLiteral( "userengine" )
        << QStringLiteral( "useragent" ) << QStringLiteral( "useros" );
    return list;
}

inline QString PHISRequest::requestValue( const QString &key ) const
{
    if ( _cookies.contains( key ) ) return _cookies.value( key );
    if ( _postData.contains( key ) ) return _postData.value( key );
    return QUrlQuery( _url ).queryItemValue( key );
}

inline QStringList PHISRequest::requestValues( const QString &key ) const
{
    if ( _cookies.contains( key ) ) return _cookies.values( key );
    if ( _postData.contains( key ) ) return _postData.values( key );
    return QUrlQuery( _url ).allQueryItemValues( key );
}

#endif // PHISREQUEST_H
