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
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PHISMODULE_H
#define PHISMODULE_H
#include <QObject>
#include <QScriptEngine>
#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include "phisrequest.h"
#include "phi.h"

#ifdef PHISLIB
#define PHIS_EXPORT Q_DECL_EXPORT
#else
#define PHIS_EXPORT Q_DECL_IMPORT
#endif
#define PHIS_DEPRECATED(msg) interface()->deprecated(__FILE__,__LINE__,QDateTime::currentDateTime(),(msg));
#define PHIS_LOG(type,msg) interface()->log((type),__FILE__,__LINE__,QDateTime::currentDateTime(),(msg));
#define PHIS_ERROR(msg) interface()->log(PHISInterface::LTError,__FILE__,__LINE__,QDateTime::currentDateTime(),(msg));
#define PHIS_WARN(msg) interface()->log(PHISInterface::LTWarning,__FILE__,__LINE__,QDateTime::currentDateTime(),(msg));
#define PHIS_CRIT(msg) interface()->log(PHISInterface::LTCritical,__FILE__,__LINE__,QDateTime::currentDateTime(),(msg));
#define PHIS_DEBUG(msg) interface()->log(PHISInterface::LTDebug,__FILE__,__LINE__,QDateTime::currentDateTime(),(msg));

class PHISInterface;

class PHIS_EXPORT PHISScriptObj : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QStringList properties READ properties )

public:
    explicit PHISScriptObj( const PHISInterface *interface ) : _if( interface ) {}
    inline virtual quint32 version() const { return 0x00010000; }

public slots:
    inline QStringList properties() const { return PHI::properties( this ); }

protected:
    inline const PHISInterface* const interface() const { return _if; }

private:
    const PHISInterface *_if;
};

class PHIS_EXPORT PHISModuleIF
{
public:
    virtual PHISScriptObj* create( const QString &key, const PHISInterface* ) const=0;
    virtual QStringList keys() const=0;
};

Q_DECLARE_INTERFACE( PHISModuleIF, "org.phisketeer.phis.module/1.0" )

class PHIS_EXPORT PHISModule : public QObject, public PHISModuleIF
{
    Q_OBJECT
    Q_INTERFACES( PHISModuleIF )

public:
    virtual PHISScriptObj* create( const QString &key, const PHISInterface* ) const=0;
    virtual QStringList keys() const=0;
};

class PHIS_EXPORT PHISInterface : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( PHISInterface )
    friend class PHIProcessor;
    friend class PHISGlobalScriptObj;

protected:
    explicit PHISInterface( const PHISRequest *req, QScriptEngine *engine )
        : QObject( engine ), _req( req ) {}

private:
    const PHISRequest *_req;

public:
    enum LogType { LTWarning, LTError, LTCritical, LTUser, LTDebug, LTTrace };
    inline QScriptEngine* scriptEngine() const { return qobject_cast<QScriptEngine*>(parent()); }
    inline quint8 interfaceVersion() const { return 0x01; }

    void log( LogType, const char *file, int line, const QDateTime &dt, const QString &message ) const;
    void deprecated( const char *file, int line, const QDateTime &dt, const QString &message ) const;
    void setContentType( const QByteArray &contenttype ) const;
    void setContent( const QByteArray &content ) const;
    inline QString pageLanguage() const { return _req->currentLang(); }
    inline void setPageLanguage( const QString &l ) { _req->setCurrentLang( l ); }
    inline QString admin() const { return _req->admin(); }
    inline QString hostname() const { return _req->hostname(); }
    inline QString documentRoot() const { return _req->documentRoot(); }
    inline QString serverName() const { return _req->serverDescription(); }
    inline QString serverHost() const { return _req->serverHostname(); }
    inline QString serverDef() const { return _req->serverDefname(); }
    inline QHostAddress localAddress() const { return _req->localHostAddress(); }
    inline QHostAddress remoteAddress() const { return _req->remoteHostAddress(); }
    inline QString tempDir() const { return _req->tmpDir(); }
    inline QString today() const { return QDate::currentDate().toString( QString::fromLatin1( PHI::isoDateFormat() ) ); }
    inline QString nowUtc() const { return QDateTime::currentDateTime().toUTC().toString( QString::fromLatin1( PHI::dtFormat() ) ); }
    inline QDateTime utc() const { return QDateTime::currentDateTime().toUTC(); }
    inline qint32 port() const { return _req->port(); }
    inline qint32 keepAlive() const { return _req->keepAlive(); }
    inline QString localIP() const { return _req->localIP(); }
    inline QString remoteIP() const { return _req->remoteIP(); }
    inline QString contentType() const { return _req->contentType(); }
    inline QString user() const { return _req->user(); }
    inline QString password() const { return _req->password(); }
    inline QUrl url() const { return _req->url(); }
    inline QString method() const { return _req->method(); }
    inline QString postData() const { return _req->serverValue( QStringLiteral( "postdata" ) ); }
    inline QString fileName() const { return _req->canonicalFilename(); }
    inline QString agent() const { return _req->agent(); }
    inline QString accept() const { return _req->accept(); }
    inline QString scheme() const { return _req->scheme(); }
    inline QString self() const { return _req->serverValue( QStringLiteral( "self" ) ); }
    inline QDateTime started() const { return _req->started(); }
    inline QDateTime modified() const { return _req->lastModified(); }
    inline qint64 contentLength() const { return _req->contentLength(); }
    inline QStringList languages() const { return _req->acceptedLanguages();  } // without qualifier ';q=x.x'
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
};

inline void PHISInterface::setContent( const QByteArray &content ) const {
    _req->responseRec()->setBody( content );
    _req->responseRec()->setContentLength( content.size() );
}

inline void PHISInterface::setContentType( const QByteArray &contenttype ) const {
    _req->responseRec()->setContentType( contenttype );
}

inline void PHISInterface::deprecated( const char* file, int line, const QDateTime &dt, const QString &m ) const {
    _req->responseRec()->log( 0x04, file, line, dt, PHIRC_MODULE_DEPRECATED, m );
}

#endif // PHISMODULE_H
