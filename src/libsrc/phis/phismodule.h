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

#ifdef PHISLIB
#define PHIS_EXPORT Q_DECL_EXPORT
#else
#define PHIS_EXPORT Q_DECL_IMPORT
#endif
#define PHIS_DEPRECATED(msg) interface()->deprecated(__FILE__,__LINE__,QDateTime::currentDateTime(),(msg));
#define PHIS_LOG(type,msg) interface()->log((type),__FILE__,__LINE__,QDateTime::currentDateTime(),(msg));
#define PHIS_ERROR(msg) interface()->log(PHISInterface::LTError,__FILE__,__LINE__,QDateTime::currentDateTime(),(msg));
#define PHIS_WARN(msg) interface()->log(PHISInterface::LTWarning,__FILE__,__LINE__,QDateTime::currentDateTime(),(msg));

class PHISInterface;

class PHIS_EXPORT PHISScriptObj : public QObject
{
    Q_OBJECT

public:
    explicit PHISScriptObj( PHISInterface *interface ) : _if( interface ) {}
    inline virtual quint32 version() const { return 0x00010000; }

protected:
    inline PHISInterface* interface() const { return _if; }

private:
    PHISInterface *_if;
};

class PHIS_EXPORT PHISModuleIF
{
public:
    virtual PHISScriptObj* create( const QString &key, PHISInterface* ) const=0;
    virtual QStringList keys() const=0;
};

Q_DECLARE_INTERFACE( PHISModuleIF, "org.phisketeer.phis.module/1.0" )

class PHIS_EXPORT PHISModule : public QObject, public PHISModuleIF
{
    Q_OBJECT
    Q_INTERFACES( PHISModuleIF )

public:
    virtual PHISScriptObj* create( const QString &key, PHISInterface* ) const=0;
    virtual QStringList keys() const=0;
};

class PHIS_EXPORT PHISInterface : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( PHISInterface )
    friend class PHIProcessor;
    friend class PHISGlobalScriptObj;

protected:
    explicit PHISInterface( const PHISRequest *, QScriptEngine* );
    virtual ~PHISInterface();

private:
    const PHISRequest *_req;

public:
    enum LogType { LTWarning, LTError, LTCritical, LTUser, LTDebug, LTTrace };
    inline QScriptEngine* scriptEngine() const { return qobject_cast<QScriptEngine*>(parent()); }
    inline quint8 version() const { return 0x01; }

    void log( LogType, const char *file, int line, const QDateTime &dt, const QString &message );
    void deprecated( const char *file, int line, const QDateTime &dt, const QString &message );
    void setContentType( const QByteArray &contenttype );
    void setContent( const QByteArray &content );
    inline QString pageLanguage() const { return _req->currentLang(); }
    inline void setPageLanguage( const QString &l ) { _req->setCurrentLang( l ); }
    inline QString admin() const { return _req->admin(); }
    inline QString hostname() const { return _req->hostname(); }

};


inline void PHISInterface::setContent( const QByteArray &content ) {
    _req->responseRec()->setBody( content );
    _req->responseRec()->setContentLength( content.size() );
}

inline void PHISInterface::setContentType( const QByteArray &contenttype ) {
    _req->responseRec()->setContentType( contenttype );
}

inline void PHISInterface::deprecated(const char* file, int line, const QDateTime &dt, const QString &m ) {
    _req->responseRec()->log( 0x04, file, line, dt, PHIRC_MODULE_DEPRECATED, m );
}

#endif // PHISMODULE_H
