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
#ifndef PHISSCRIPTOBJECTS_H
#define PHISSCRIPTOBJECTS_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QtScript>
#include <QFile>
#include <QDir>
#include <QTcpSocket>
#include <QCryptographicHash>
#include <QUuid>
#include <QUrl>
#include <QDateTime>
#include <QLocale>
#include "phisrequest.h"
#include "phiresponserec.h"
#include "phisysinfo.h"

class PHISServerObj : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString documentroot READ documentRoot )
    Q_PROPERTY( QString servername READ serverName )
    Q_PROPERTY( QString serverhost READ serverHost )
    Q_PROPERTY( QString serverdef READ serverDef )
    Q_PROPERTY( QString admin READ admin )
    Q_PROPERTY( QString localaddress READ localAddress )
    Q_PROPERTY( QString remoteaddress READ remoteAddress )
    Q_PROPERTY( QString tempdir READ tempDir )
    Q_PROPERTY( QString today READ today )
    Q_PROPERTY( QString nowutc READ nowUtc )
    Q_PROPERTY( QDateTime utc READ utc )
    Q_PROPERTY( qint32 port READ port )
    Q_PROPERTY( qint32 keepalive READ keepAlive )
    Q_PROPERTY( QStringList properties READ properties )

public:
    explicit PHISServerObj( const PHISRequest *req, QScriptEngine *e, PHIResponseRec *resp );
    virtual ~PHISServerObj();

public slots:
    inline QString documentRoot() const { return _req->documentRoot(); }
    inline QString serverName() const { return _req->serverDescription(); }
    inline QString serverHost() const { return _req->serverHostname(); }
    inline QString serverDef() const { return _req->serverDefname(); }
    inline QString admin() const { return _req->admin(); }
    inline QString localAddress() const { return _req->localIP(); }
    inline QString remoteAddress() const { return _req->remoteIP(); }
    inline QString tempDir() const { return _req->tmpDir(); }
    inline QString today() const { return QDate::currentDate().toString( PHI::isoDateFormat() ); }
    inline QString nowUtc() const { return QDateTime::currentDateTime().toUTC().toString( PHI::dtFormat() ); }
    inline QDateTime utc() const { return QDateTime::currentDateTime().toUTC(); }
    inline qint32 port() const { return _req->port(); }
    inline qint32 keepAlive() const { return _req->keepAlive(); }
    inline QStringList properties() const { return PHI::properties( this ); }
    void log( const QString &t );

protected:
    const PHISRequest *_req;
    PHIResponseRec *_resp;
};

class PHISRequestObj : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString contenttype READ contentType )
    Q_PROPERTY( QString user READ user )
    Q_PROPERTY( QString password READ password )
    Q_PROPERTY( QString url READ url )
    Q_PROPERTY( QString hostname READ hostName )
    Q_PROPERTY( QString method READ method )
    Q_PROPERTY( QString postdata READ postData )
    Q_PROPERTY( QString filename READ fileName )
    Q_PROPERTY( QString agent READ agent )
    Q_PROPERTY( QString accept READ accept )
    Q_PROPERTY( QString lang READ lang )
    Q_PROPERTY( QString scheme READ scheme )
    Q_PROPERTY( QString self READ self )
    Q_PROPERTY( qint64 contentlength READ contentLength )
    Q_PROPERTY( QDateTime started READ started )
    Q_PROPERTY( QDateTime modified READ modified )
    Q_PROPERTY( QStringList languages READ languages )
    Q_PROPERTY( QStringList properties READ properties )
    Q_PROPERTY( quint8 ostype WRITE setOsType READ osType )
    Q_PROPERTY( quint8 agentid WRITE setAgentId READ agentId )
    Q_PROPERTY( quint8 agentengine WRITE setAgentEngine READ agentEngine )
    Q_PROPERTY( qint32 enginemajorversion WRITE setEngineMajorVersion READ engineMajorVersion )
    Q_PROPERTY( qint32 engineminorversion WRITE setEngineMinorVersion READ engineMinorVersion )

public:
    explicit PHISRequestObj( const PHISRequest *req, QScriptEngine *e, PHIResponseRec *resp );
    virtual ~PHISRequestObj();

public slots:
    inline QString contentType() const { return _req->contentType(); }
    inline QString user() const { return _req->user(); }
    inline QString password() const { return _req->password(); }
    inline QString url() const { return _req->url().toString(); }
    inline QString hostName() const { return _req->hostname(); }
    inline QString method() const { return _req->method(); }
    inline QString postData() const { return _req->serverValue( "postdata" ); }
    inline QString fileName() const { return _req->canonicalFilename(); }
    inline QString agent() const { return _req->agent(); }
    inline QString accept() const { return _req->accept(); }
    inline QString lang() const { return _req->currentLang(); }
    inline QString scheme() const { return _req->scheme(); }
    inline QString self() const { return _req->serverValue( "self" ); }

    inline QDateTime started() const { return _req->started(); }
    inline QDateTime modified() const { return _req->lastModified(); }
    inline qint64 contentLength() const { return _req->contentLength(); }
    inline QStringList languages() const { return _req->acceptedLanguages();  } // without qualifier ';q=x.x'
    inline QStringList properties() const { return PHI::properties( this ); }
    inline quint8 osType() const { return _req->osType(); }
    inline void setOsType( quint8 type ) { _req->setOSType( type ); }
    inline quint8 agentId() const { return _req->agentId(); }
    inline void setAgentId( quint8 aid ) { _req->setAgentId( aid ); }
    inline quint8 agentEngine() const { return _req->agentEngine(); }
    inline void setAgentEngine( quint8 ae ) { _req->setAgentEngine( ae ); }
    inline qint32 engineMajorVersion() const { return _req->engineMajorVersion(); }
    inline void setEngineMajorVersion( qint32 emv ) { _req->setEngineMajorVersion( emv ); }
    inline qint32 engineMinorVersion() const { return _req->engineMinorVersion(); }
    inline void setEngineMinorVersion( qint32 emv ) { _req->setEngineMinorVersion( emv ); }

protected:
    const PHISRequest *_req;
    PHIResponseRec *_resp;
};

class PHISReplyObj : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QString content READ content WRITE setContent )
    Q_PROPERTY( QString contenttype READ contentType WRITE setContentType )
    Q_PROPERTY( QString charset READ charset WRITE setCharset )
    Q_PROPERTY( QString filename READ fileName WRITE setFileName )

public:
    explicit PHISReplyObj( QScriptEngine*, PHIResponseRec* );
    virtual ~PHISReplyObj();
    inline bool hasOwnContent() const { return _ownContent; }

public slots:
    inline QString contentType() const { return _contentType; }
    inline QString charset() const { return _charset; }
    inline QString content() const { return _content; }
    inline QString fileName() const { return _fileName; }
    inline QString percentEncoded( const QString &u ) const {
        return QString::fromUtf8( QUrl::toPercentEncoding( u ) ); }
    inline QString percentDecoded( const QString &u ) const {
        return QUrl::fromPercentEncoding( u.toUtf8() ); }

    void setFileName( const QString &f );
    void setCharset( const QString &c );
    void setContent( const QString &c );
    void setContentType( const QString &t );
    void setCookie( const QString &name, const QString &value, const QDateTime &expires,
        const QString &path=QChar( '/' ), const QString &domain=QString(), bool secure=false, bool discard=false );
    void setCookie( const QString &name, const QString &value, int maxage, const QString &path=QChar( '/' ),
        const QString &domain=QString(), bool secure=false, bool discard=false );

private:
    bool _ownContent;
    QString _content, _contentType, _charset, _fileName;
    PHIResponseRec *_resp;
};

class PHISSystemObj : public QObject
{
    Q_OBJECT
    Q_PROPERTY( int exitcode READ exitCode )
    Q_PROPERTY( QString lasterror READ lastError )
    Q_PROPERTY( QString systemname READ systemName )
    Q_PROPERTY( QStringList properties READ properties )

public:
    explicit PHISSystemObj( QScriptEngine *e, PHIResponseRec *resp );
    virtual ~PHISSystemObj();

public slots:
    inline int exitCode() const { return _exitCode; }
    inline QString lastError() const { return _lastError; }
    inline QString systemName() const { return PHISysInfo::systemString(); }
    inline QStringList properties() const { return PHI::properties( this ); }
    void log( const QString &t );
    QString exec( const QString &procName, const QString &attributes=QString(),
        const QString &codec=QString( "utf-8" ) );
    QString exec( const QString &procName, const QStringList &atts,
        const QString &codec=QString( "utf-8" ) );

    inline QString md5( const QString &s ) const { return QString::fromUtf8(
        QCryptographicHash::hash( s.toUtf8(), QCryptographicHash::Md5 ) ); }
    inline QString md4( const QString &s ) const { return QString::fromUtf8(
        QCryptographicHash::hash( s.toUtf8(), QCryptographicHash::Md4 ) ); }
    inline QString sha1( const QString &s ) const { return QString::fromUtf8(
        QCryptographicHash::hash( s.toUtf8(), QCryptographicHash::Sha1 ) ); }
    inline QString createUuid() const {
        return QUuid::createUuid().toString().mid( 1, 36 ); }
    inline QString toLocaleString( const QDateTime &dt, const QString &lang=QString( "C" ) ) {
        return dt.toString( QLocale( toLocaleFormat( lang ) ).dateTimeFormat() ); }
    inline QString dayName( int day, const QString &lang=QString( "C" ) ) {
        return QLocale( toLocaleFormat( lang ) ).dayName( day+1 ); }
    inline QString monthName( int month, const QString &lang=QString( "C" ) ) {
        return QLocale( toLocaleFormat( lang ) ).monthName( month+1 ); }
    inline QString countryName( const QString &lang=QString( "C" ) ) {
        return QLocale::countryToString( QLocale( toLocaleFormat( lang ) ).country() ); }
    inline QString languageName( const QString &lang=QString( "C" ) ) {
        return QLocale::languageToString( QLocale( toLocaleFormat( lang ) ).language() ); }
    inline QString toFormatedString( const QDateTime &dt, const QString &format,
            const QString &lang=QString( "C" ) ) {
        return QLocale( toLocaleFormat( lang ) ).toString( dt, format ); }

protected:
    QString toLocaleFormat( const QString& );

private:
    QString _lastError;
    PHIResponseRec *_resp;
    int _exitCode;
};

class PHISFileObj : public QObject
{
    Q_OBJECT
    Q_PROPERTY( int readonly READ readOnly )
    Q_PROPERTY( int writeonly READ writeOnly )
    Q_PROPERTY( int readwrite READ readWrite )
    Q_PROPERTY( int append READ append )
    Q_PROPERTY( QString lasterror READ lastError )
    Q_PROPERTY( QString tempdir READ tempDir )
    Q_PROPERTY( QStringList properties READ properties )

public:
    explicit PHISFileObj( QScriptEngine *e, PHIResponseRec *resp );
    virtual ~PHISFileObj();

public slots:
    inline bool exists( const QString &f ) const { return QFile::exists( f ); }
    inline bool copy( const QString &src, const QString &dest ) const { return QFile::copy( src, dest ); }
    inline int readOnly() const { return 0x01; }
    inline int writeOnly() const { return 0x02; }
    inline int readWrite() const { return 0x03; }
    inline int append() const { return 0x04; }
    inline bool isOpen( int handle ) const { return _files.contains( handle ) ?
        _files.value( handle )->isOpen() : false; }
    inline void close( int handle ) { if ( _files.contains( handle ) ) _files.value( handle )->close(); }
    inline QString lastError() const { return _error; }
    inline QString error( int handle ) const { return _files.contains( handle ) ?
        _files.value( handle )->errorString() : QString(); }
    inline bool atEnd( int handle ) const { return _files.contains( handle ) ?
        _files.value( handle )->atEnd() : true; }
    inline QStringList properties() const { return PHI::properties( this ); }
    inline bool remove( const QString &f ) const { return QFile::remove( f ); }
    inline QString tempDir() const { return QDir::tempPath(); }
    int open( const QString &fileName, int mode );
    bool write( int handle, const QString &data, const QString &codec="utf-8" );
    QString read( int handle, const QString &codec="utf-8" ) const;

private:
    PHIResponseRec *_resp;
    QHash <int, QFile*> _files;
    int _handle;
    mutable QString _error;
};

class PHISSqlObj : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString lasterror READ lastError )
    Q_PROPERTY( QStringList tables READ tables )
    Q_PROPERTY( bool next READ next )
    Q_PROPERTY( int numrowsaffected READ numRowsAffected )
    Q_PROPERTY( QList<QVariant> values READ values )
    Q_PROPERTY( int rows READ rows )
    Q_PROPERTY( QStringList properties READ properties )

public:
    explicit PHISSqlObj( const QSqlDatabase &db, QScriptEngine*, PHIResponseRec* , bool useSql );
    explicit PHISSqlObj( const QSqlDatabase &db, QScriptEngine*, PHIResponseRec* );
    virtual ~PHISSqlObj();

public slots:
    inline QString lastError() const { return _query.lastError().text(); }
    inline QStringList tables() const { return _db.isOpen() ? _db.tables() : QStringList(); }
    inline bool next() const { return _query.next(); }
    inline QVariant value( int index ) const { return _query.value( index ); }
    inline int numRowsAffected() const { return _query.numRowsAffected(); }
    inline QStringList properties() const { return PHI::properties( this ); }
    inline int rows() const { return _query.isValid() ? _query.size() : 0; }
    inline bool transaction() { return _db.isOpen() ? _db.transaction() : false; }
    inline bool commit() { return _db.isOpen() ? _db.commit() : false; }
    inline bool rollback() { return _db.isOpen() ? _db.rollback() : false; }
    bool query( const QString& ) const;
    QList<QVariant> values() const;
    QScriptValue clone() const;

private:
    QSqlDatabase _db;
    mutable QSqlQuery _query;
    PHIResponseRec *_resp;
};

class PHISEmailObj : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString from READ from WRITE setFrom )
    Q_PROPERTY( QString to READ to WRITE setTo )
    Q_PROPERTY( QStringList recipients READ recipients WRITE setRecipients )
    Q_PROPERTY( QString server READ server WRITE setServer )
    Q_PROPERTY( QString password READ password WRITE setPassword )
    Q_PROPERTY( QString user READ user WRITE setUser )
    Q_PROPERTY( QString subject READ subject WRITE setSubject )
    Q_PROPERTY( QString lasterror READ lastError )
    Q_PROPERTY( QString data READ data WRITE setData )
    Q_PROPERTY( QString contenttype READ contentType WRITE setContentType )

public:
    explicit PHISEmailObj( const PHISRequest *req, QScriptEngine *e, PHIResponseRec *resp );
    virtual ~PHISEmailObj();

public slots:
    inline QString from() const { return _from; }
    inline void setFrom( const QString &f ) { _from=f; }
    inline QString to() const { return _to; }
    inline void setTo( const QString &to ) { _to=to; }
    inline QStringList recipients() const { return _recipients; }
    inline void setRecipients( const QStringList &l ) { _recipients=l; }
    inline QString server() const { return _server; }
    inline void setServer( const QString &s ) { _server=s; }
    inline QString password() const { return _password; }
    inline void setPassword( const QString &p ) { _password=p; }
    inline QString user() const { return _user; }
    inline void setUser( const QString &u ) { _user=u; }
    inline QString subject() const { return _subject; }
    inline void setSubject( const QString &s ) { _subject=s; }
    inline QString data() const { return _data; }
    inline void setData( const QString &d ) { _data=d; }
    inline QString lastError() const { return _lastError; }
    inline qint32 timeout() const { return _timeout; }
    inline void setTimeout( qint32 t ) { _timeout=t; }
    inline quint16 port() const { return _port; }
    inline void setPort( quint16 p ) { _port=p; }
    inline void addRecipient( const QString &r ) { _recipients.append( r ); }
    inline QString contentType() const { return _contentType; }
    inline void setContentType( const QString &c ) { _contentType=c; }

    inline void setHtml( bool b ) { b ? _contentType="text/html; charset=utf-8"
        : _contentType="text/plain; charset=utf-8"; }
    inline bool html() const { return _contentType=="text/html; charset=utf-8"; }
    qint16 connect();
    qint16 disconnect();
    qint16 send();
    qint16 sendTo( const QString &to, const QString &subject=QString(), const QString &data=QString() );

protected:
    qint16 waitForResponseLine( bool *hasMoreLines=0 );
    qint16 waitForDataWritten( const QString &data, QTextStream &t );
    qint16 waitForMessageWritten( QTextStream &t, const QString &to=QString(),
        const QString &subject=QString(), const QString &data=QString() );
    qint16 disconnectFromServer( qint16 err, QTextStream &t );

private:
    QString _from, _to, _server, _password, _user, _subject, _lastError, _data, _contentType;
    QStringList _recipients;
    qint32 _timeout;
    quint16 _port;
    PHIResponseRec *_resp;
    const PHISRequest *_req;
    QTcpSocket *_socket;
};

#endif // PHISSCRIPTOBJECTS_H
