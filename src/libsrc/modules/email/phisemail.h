/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2014  Phisys AG, Switzerland
#    Copyright (C) 2012-2014  Phisketeer.org team
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
#ifndef PHISEMAIL_H
#define PHISEMAIL_H
#include <QTcpSocket>
#include "phismodule.h"

class PHISEmailModule : public PHISModule
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.phisketeer.phis.module.email" FILE "phisemail.json")
    Q_CLASSINFO( "Author", "Marius Schumacher" )
    Q_CLASSINFO( "Url", "http://www.phisketeer.org" )
    Q_CLASSINFO( "Version", "1.0" )
    Q_CLASSINFO( "License", "LGPL" )
    Q_CLASSINFO( "Copyright", "2013 Phisys AG, 2013-2014 Phisketeer Team" )

public:
    virtual PHISScriptObj* create( const QString &key, const PHISInterface* ) const;
    virtual QStringList keys() const;
};

class PHISEmailObj : public PHISScriptObj
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
    Q_PROPERTY( quint16 port READ port WRITE setPort )
    Q_PROPERTY( qint32 timeout READ timeout WRITE setTimeout )

public:
    explicit PHISEmailObj( const PHISInterface* );
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

    inline void setHtml( bool b ) { b ? _contentType=QStringLiteral( "text/html; charset=utf-8" )
        : _contentType=QStringLiteral( "text/plain; charset=utf-8" ); }
    inline bool html() const { return _contentType==QStringLiteral( "text/html; charset=utf-8" ); }
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
    QTcpSocket *_socket;
};

inline PHISScriptObj* PHISEmailModule::create( const QString &key, const PHISInterface *interface ) const
{
    if ( key==QLatin1String( "email" ) ) return new PHISEmailObj( interface );
    return 0;
}

inline QStringList PHISEmailModule::keys() const
{
    return QStringList() << QStringLiteral( "email" );
}

inline qint16 PHISEmailObj::disconnect()
{
    QTextStream t( _socket );
    t.setCodec( "utf-8" );
    return disconnectFromServer( 0, t );
}

#endif // PHISEMAIL_H
