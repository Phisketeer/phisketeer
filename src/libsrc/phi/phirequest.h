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
#ifndef PHIREQUEST_H
#define PHIREQUEST_H

#include <QByteArray>
#include <QStringList>
#include <QHash>
#include <QHostAddress>
#include <QUrl>
#include <QUrlQuery>
#include <QDateTime>
#include <QTemporaryFile>
#include <QHostAddress>
#include "phi.h"

class PHIResponseRec;

class PHIEXPORT PHIRequest
{
public:
    enum AgentEngine { UnknownEngine=0, Trident=1, Gecko=2, WebKit=3, Presto=4, MaxAgentEngine=5 };
    enum AgentId { UnknownAgent=0, IE=1, Firefox=2, Konqueror=3, Safari=4, Chrome=5, Opera=6,
        Amphibia=7, SeaMonkey=8, Phis=9, Blink=10, MaxAgentId=11 };
    enum OSType { UnknownOS=0, Windows=1, Linux=2, MacOS=3, iOS=4, Symbian=5, WindowsMobile=6,
        Android=7, Solaris=8, AIX=9, HPUX=10, MaxOSType=11 };
    enum Key { KMethod=1, KContentType, KDefName, KAdmin, KServerDesc, KServerHostname };
    enum AgentFeature { None=0, HTML5=0x1, SVG=0x2, BorderRadius=0x4, Canvas=0x8,
        Transitions=0x10, Gradients=0x20, Transform2D=0x40, RGBA=0x80, BoxShadow=0x100,
        IE678=0x200, Transform3D=0x400, H264=0x800, Ogg=0x1000, MathML=0x2000,
        Audio=0x4000, TextShadow=0x8000, All=0x0FFFFFFF };

#ifdef PHIDEBUG
    Q_DECLARE_FLAGS( AgentFeatures, AgentFeature ) // qint32
#else
    typedef quint32 AgentFeatures;
#endif

    explicit PHIRequest();
    virtual ~PHIRequest();

    inline quint8 agentEngine() const { return _agentEngine; }
    inline void setAgentEngine( quint8 id ) const { id < MaxAgentEngine ? _agentEngine=id : _agentEngine=UnknownEngine; }
    inline quint8 agentId() const { return _agentId; }
    inline void setAgentId( quint8 id ) const { id < MaxAgentId ? _agentId=id : _agentId=UnknownAgent; }
    inline void setOSType( quint8 type ) const { type < MaxOSType ? _osType=type : _osType=0; }
    inline qint32 engineMajorVersion() const { return _engineMajorVersion; }
    inline void setEngineMajorVersion( qint32 major ) const { _engineMajorVersion=major; }
    inline qint32 engineMinorVersion() const { return _engineMinorVersion; }
    inline void setEngineMinorVersion( qint32 minor ) const { _engineMinorVersion=minor; }
    inline const QString& currentLang() const { return _lang; }
    inline void setCurrentLang( const QString &l ) const { _lang=l; _langByteArray=_lang.toLatin1(); }
    inline void setCurrentLang( const QByteArray &l ) const { _langByteArray=l; _lang=QString::fromLatin1( l ); }
    inline const QByteArray& currentLangByteArray() const { return _langByteArray; }
    inline qint8 httpServerMajorVersion() const { return _httpMajor; }
    inline qint8 httpServerMinorVersion() const { return _httpMinor; }
    inline qint32 keepAlive() const { return _keepAlive; }
    inline qint64 contentLength() const { return _contentLength; }
    inline bool isPhiLangRequest() const { return _philang; }
    inline const QByteArray& defaultLang() const { return _defaultLang; }
    inline void setDefaultLang( const QByteArray &l ) const { _defaultLang=l; }
    inline const QByteArray& agentPrefix() const { return _agentPrefix; }

    //inline QTemporaryFile* tmpFile( const QString &key ) const { return _tmpFiles.value( key.toUtf8(), 0 ); }
    inline const QUrl& url() const { return _url; }
    inline int port() const { return _url.port(); }
    inline const QDateTime& started() const { return _started; }
    inline const QDateTime& lastModified() const { return _modified; }
    inline QByteArray keyword( Key k ) const { return _keywords.value( k ); }
    inline const PHIByteArrayList& acceptedLanguages() const { return _acceptedLangs; } // without qualifier ';q=x.x'
    inline PHIResponseRec* responseRec() const { return _resp; }
    inline AgentFeatures agentFeatures() const { return _agentFeatures; }

    inline const QString& documentRoot() const { return _documentRoot; }
    inline const QString& tmpDir() const { return _tmpDir; }
    inline const QString& imgDir() const { return _imgDir; }
    inline const QString& canonicalFilename() const { return _canonicalFilename; }
    inline QString header( const QString &key ) const { return QString::fromUtf8( _headers.value( key.toUtf8() ) ); }
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
    inline const QHostAddress& localHostAddress() const { return _localIP; }
    inline const QHostAddress& remoteHostAddress() const { return _remoteIP; }
    inline QString referer() const { return QString::fromUtf8( _headers.value( QByteArrayLiteral( "Referer" ) ) ); }
    inline QString agent() const { return QString::fromUtf8( _headers.value( QByteArrayLiteral( "User-Agent" ) ) ); }
    inline QString accept() const { return QString::fromUtf8( _headers.value( QByteArrayLiteral( "Accept" ) ) ); }
    inline const QString& version() const { return PHI::libVersion(); }

    inline QString cookieValue( const QString &key ) const {  return _cookies.value( key ); }
    inline QString getValue( const QString &key ) const { return QUrlQuery( _url ).queryItemValue( key ); }
    inline QString postValue( const QString &key ) const { return _postData.value( key ); }
    inline QByteArray headerValue( const QByteArray &key ) const { return _headers.value( key ); }
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
    const QString& userAgent() const;
    const QString& userEngine() const;
    QString userOS() const;
    QStringList requestValues( const QString &key ) const;
    QStringList getKeys() const;
    const QStringList& serverKeys() const;
    QStringList headerKeys() const;
    QStringList fileKeys() const;
    QDateTime ifModifiedSince() const;
    quint8 osType() const;

    void dump() const;
    void init();
    bool setRedirectedFile( const QString &file );
    static QDateTime dateTimeFromHeader( const QByteArray &modified );

protected:
    mutable QByteArray _defaultLang, _agentPrefix;
    mutable QString _lang; // can be overwritten by the Serverscript engine
    mutable QByteArray _langByteArray; //                       "
    mutable quint8 _agentEngine, _agentId, _osType; //          "
    mutable qint32 _engineMajorVersion, _engineMinorVersion; // "
    bool _philang;
    qint8 _httpMajor, _httpMinor;
    qint64 _contentLength;
    qint32 _keepAlive;
    QDateTime _started, _modified;
    QUrl _url;
    PHIByteArrayList _acceptedLangs;
    QString _documentRoot, _tmpDir, _imgDir, _canonicalFilename;
    QHostAddress _localIP, _remoteIP;
    QHash <QByteArray, QByteArray> _headers;
    QHash <QByteArray, QTemporaryFile*> _tmpFiles;
    QHash <QString, QString> _cookies, _postData;
    QHash <quint8, QByteArray> _keywords;
    PHIResponseRec *_resp;
    AgentFeatures _agentFeatures;
};

#ifdef PHIDEBUG
Q_DECLARE_OPERATORS_FOR_FLAGS( PHIRequest::AgentFeatures )
#endif

inline PHIRequest::PHIRequest()
    : _agentEngine( PHIRequest::UnknownEngine ), _agentId( PHIRequest::UnknownAgent ),
    _osType( PHIRequest::UnknownOS ), _engineMajorVersion( -1 ), _engineMinorVersion( -1 ),
    _philang( false ), _contentLength( 0 ), _started( QDateTime::currentDateTime() ),
    _agentFeatures( All & ~IE678 & ~MathML & ~H264 & ~Ogg )
{
}

inline PHIRequest::~PHIRequest()
{
    QTemporaryFile *tmpFile;
    foreach ( tmpFile, _tmpFiles ) {
        qDebug( "Closing %s", qPrintable( tmpFile->fileName() ) );
        tmpFile->close();
        delete tmpFile;
    }
    _tmpFiles.clear();
    qDebug( "PHIRequest::~PHIRequest" );
}

inline QDateTime PHIRequest::ifModifiedSince() const
{
    QByteArray s=_headers.value( QByteArrayLiteral( "If-Modified-Since" ) );
    if ( s.isEmpty() ) return QDateTime( QDate( 1970, 1, 1 ) );
    return dateTimeFromHeader( s );
}

inline QStringList PHIRequest::getKeys() const
{
    QList <QPair <QString, QString> > list=QUrlQuery( _url ).queryItems();
    QPair <QString, QString> pair;
    QStringList keys;
    foreach ( pair, list ) keys.append( pair.first );
    keys.removeDuplicates();
    return keys;
}

inline QStringList PHIRequest::headerKeys() const
{
    QStringList list;
    QByteArray key;
    foreach ( key, _headers.keys() ) list.append( QString::fromUtf8( key ) );
    return list;
}

inline QStringList PHIRequest::fileKeys() const
{
    QStringList list;
    QByteArray key;
    foreach ( key, _tmpFiles.keys() ) list.append( QString::fromUtf8( key ) );
    return list;
}

inline const QStringList& PHIRequest::serverKeys() const
{
    static QStringList list=QStringList() << SL( "documentroot" )
        << SL( "self" ) << SL( "url" ) << SL( "filename" )
        << SL( "hostname" ) << SL( "tempdir" ) << SL( "contentlength" )
        << SL( "contentType" ) << SL( "postdata" ) << SL( "agent" )
        << SL( "started" ) << SL( "today" ) << SL( "nowutc" )
        << SL( "modified" ) << SL( "user" ) << SL( "password" )
        << SL( "accept" ) << SL( "scheme" ) << SL( "method" )
        << SL( "servername" ) << SL( "serverdef" ) << SL( "admin" )
        << SL( "serverhost" ) << SL( "port" ) << SL( "keepalive" )
        << SL( "localaddress" ) << SL( "remoteaddress" )
        << SL( "version" ) << SL( "userengine" )
        << SL( "useragent" ) << SL( "useros" );
    return list;
}

inline QString PHIRequest::requestValue( const QString &key ) const
{
    if ( _cookies.contains( key ) ) return _cookies.value( key );
    if ( _postData.contains( key ) ) return _postData.value( key );
    return QUrlQuery( _url ).queryItemValue( key );
}

inline QStringList PHIRequest::requestValues( const QString &key ) const
{
    if ( _cookies.contains( key ) ) return _cookies.values( key );
    if ( _postData.contains( key ) ) return _postData.values( key );
    return QUrlQuery( _url ).allQueryItemValues( key );
}

#endif // PHIREQUEST_H
