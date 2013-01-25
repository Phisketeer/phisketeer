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
#include "phierror.h"

class PHISGlobalScriptObj : public QObject
{
    Q_OBJECT

public:
    explicit PHISGlobalScriptObj( const PHISRequest *req, QScriptEngine *e );
    virtual ~PHISGlobalScriptObj();

public slots:
    inline void log( const QString &t ) { _req->responseRec()->log( PHILOGUSER, PHIRC_USER, t ); }
    void loadModule( const QString& );

protected:
    const PHISRequest *_req;
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

#endif // PHISSCRIPTOBJECTS_H
