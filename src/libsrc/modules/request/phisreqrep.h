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
#ifndef PHISREQREP_H
#define PHISREQREP_H
#include "phismodule.h"

class PHISRequestModule : public PHISModule
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.phisketeer.phis.module.request" FILE "phisrequest.json")
    Q_CLASSINFO( "Author", "Marius Schumacher" )
    Q_CLASSINFO( "Url", "http://www.phisketeer.org" )
    Q_CLASSINFO( "Version", "1.0" )
    Q_CLASSINFO( "License", "LGPL" )
    Q_CLASSINFO( "Copyright", "2013 Phisys AG, 2013 Phisketeer Team" )

public:
    virtual PHISScriptObj* create( const QString &key, const PHISInterface* ) const;
    virtual QStringList keys() const;
};

class PHISRequestObj : public PHISScriptObj
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
    explicit PHISRequestObj( const PHISInterface *interface ) : PHISScriptObj( interface ) {}
    virtual QScriptValue initObject( const QString &key );

public slots:
    inline QString contentType() const { return interface()->contentType(); }
    inline QString user() const { return interface()->user(); }
    inline QString password() const { return interface()->password(); }
    inline QString url() const { return interface()->url().toString(); }
    inline QString hostName() const { return interface()->hostname(); }
    inline QString method() const { return interface()->method(); }
    inline QString postData() const { return interface()->postData(); }
    inline QString fileName() const { return interface()->fileName(); }
    inline QString agent() const { return interface()->agent(); }
    inline QString accept() const { return interface()->accept(); }
    inline QString lang() const { return interface()->pageLanguage(); }
    inline QString scheme() const { return interface()->scheme(); }
    inline QString self() const { return interface()->self(); }

    inline QDateTime started() const { return interface()->started(); }
    inline QDateTime modified() const { return interface()->modified(); }
    inline qint64 contentLength() const { return interface()->contentLength(); }
    inline QStringList languages() const { return interface()->acceptedLanguages();  } // without qualifier ';q=x.x'
    inline quint8 osType() const { return interface()->osType(); }
    inline void setOsType( quint8 type ) const { interface()->setOsType( type ); }
    inline quint8 agentId() const { return interface()->agentId(); }
    inline void setAgentId( quint8 aid ) { interface()->setAgentId( aid ); }
    inline quint8 agentEngine() const { return interface()->agentEngine(); }
    inline void setAgentEngine( quint8 ae ) { interface()->setAgentEngine( ae ); }
    inline qint32 engineMajorVersion() const { return interface()->engineMajorVersion(); }
    inline void setEngineMajorVersion( qint32 emv ) { interface()->setEngineMajorVersion( emv ); }
    inline qint32 engineMinorVersion() const { return interface()->engineMinorVersion(); }
    inline void setEngineMinorVersion( qint32 emv ) const { interface()->setEngineMinorVersion( emv ); }
};

class PHISReplyObj : public PHISScriptObj
{
    Q_OBJECT

    Q_PROPERTY( QString content READ content WRITE setContent )
    Q_PROPERTY( QString contenttype READ contentType WRITE setContentType )
    Q_PROPERTY( QString charset READ charset WRITE setCharset )
    Q_PROPERTY( QString filename READ fileName WRITE setFileName )

public:
    explicit PHISReplyObj( const PHISInterface* );

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
    inline void setCookie( const QString &name, const QString &value, const QDateTime &expires,
        const QString &path=_slash, const QString &domain=QString(),
        bool secure=false, bool discard=false ) const {
        interface()->setCookie( name, value, expires, path, domain, secure, discard );
    }
    void setCookie( const QString &name, const QString &value, int maxage, const QString &path=_slash,
        const QString &domain=QString(), bool secure=false, bool discard=false ) const {
        interface()->setCookie( name, value, maxage, path, domain, secure, discard );
    }
    inline void setHttpHeader( const QString &name, const QString &value ) {
        interface()->setHttpHeader( name, value );
    }

private:
    QString _content, _contentType, _charset, _fileName;
    static QString _slash;
};

inline PHISScriptObj* PHISRequestModule::create( const QString &key, const PHISInterface *interface ) const
{
    if ( key==QLatin1String( "request" ) ) return new PHISRequestObj( interface );
    else if ( key==QLatin1String( "reply" ) ) return new PHISReplyObj( interface );
    return 0;
}

inline QStringList PHISRequestModule::keys() const
{
    return QStringList() << QStringLiteral( "request" ) << QStringLiteral( "reply" );
}

inline void PHISReplyObj::setFileName( const QString &f )
{
    interface()->setFileName( f );
    _fileName=f;
}

inline void PHISReplyObj::setContentType( const QString &t )
{
    interface()->setContentType( t.toUtf8() );
    _contentType=t;
}

#endif // PHISREQREP_H
