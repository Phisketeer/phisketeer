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
#include <QHostInfo>
#include "phisemail.h"

PHISEmailObj::PHISEmailObj( const PHISInterface *phisif )
    : PHISScriptObj( phisif ), _timeout( 20000 ), _port( 25 ), _socket( 0 )
{
    qDebug( "PHISEmailObj::PHISEmailObj()" );
    _from=interface()->admin();
    _contentType=QStringLiteral( "text/plain; charset=utf-8" );
    _socket=new QTcpSocket( this );
}

PHISEmailObj::~PHISEmailObj()
{
    if ( _socket->state()==QTcpSocket::ConnectedState ) {
        QTextStream t( _socket );
        t.setCodec( "utf-8" );
        disconnectFromServer( 0, t );
    }
    qDebug( "PHISEmailObj::~PHISEmailObj()" );
}

qint16 PHISEmailObj::disconnectFromServer( qint16 err, QTextStream &t )
{
    if ( _socket->state()!=QTcpSocket::ConnectedState ) return 0;
    if ( err ) PHIS_ERROR( tr( "Could not send email: <%1>." ).arg( _lastError ) );
    waitForDataWritten( QStringLiteral( "QUIT" ), t );
    if ( !_socket->waitForDisconnected( _timeout ) ) {
        _lastError=tr( "Could not cleanly disconnect from SMTP server '%1': <%2>." )
            .arg( _server ).arg( _socket->errorString() );
        PHIS_ERROR(_lastError);
        if ( err==0 ) err=1;
    }
    _socket->close();
    return err;
}

qint16 PHISEmailObj::connect()
{
    if ( _socket->isOpen() ) {
        _lastError=tr( "Connection to SMTP server '%1' already established." ).arg( _server );
        PHIS_WARN( _lastError );
        return 0;
    }
    QHostInfo info=QHostInfo::fromName( _server );
    if ( info.addresses().isEmpty() ) {
        _lastError=tr( "DNS lookup error for SMTP server '%1': <%2>." )
            .arg( _server ).arg( info.errorString() );
        PHIS_ERROR( _lastError );
        return 1;
    }
    QHostAddress server=info.addresses().first();
    _socket->connectToHost( server, _port );
    if ( !_socket->waitForConnected( _timeout ) ) {
        _lastError=tr( "Could not connect to SMTP server '%1': <%2>." ).arg( _server )
            .arg( _socket->errorString() );
        PHIS_ERROR( _lastError );
        return 1;
    }
    QTextStream t( _socket );
    t.setCodec( "utf-8" );
    bool moreLines;
    qint16 res=waitForResponseLine();
    //qDebug( "EMAIL %d %s", res, qPrintable( _lastError ) );
    QString data=QStringLiteral( "EHLO " )+interface()->hostname();
    if ( res!=220 ) return disconnectFromServer( res, t );
    res=waitForDataWritten( data, t );
    if ( res ) return disconnectFromServer( res, t );
    moreLines=true;
    while( moreLines ) {
        res=waitForResponseLine( &moreLines );
        if ( res!=250 ) return disconnectFromServer( res, t );
    }
    if ( !_user.isEmpty() && !_password.isEmpty() ) {
        res=waitForDataWritten( QStringLiteral( "AUTH LOGIN" ), t );
        if ( res ) return disconnectFromServer( res, t );
        res=waitForResponseLine();
        if ( res!=334 ) return disconnectFromServer( res, t );
        data=QString::fromLatin1( _user.toLatin1().toBase64() );
        res=waitForDataWritten( data, t );
        if ( res ) return disconnectFromServer( res, t );
        res=waitForResponseLine();
        if ( res!=334 ) return disconnectFromServer( res, t );
        data=QString::fromLatin1( _password.toLatin1().toBase64() );
        res=waitForDataWritten( data, t );
        if ( res ) return disconnectFromServer( res, t );
        res=waitForResponseLine();
        if ( res!=235 ) return disconnectFromServer( res, t );
    }
    return 0;
}

qint16 PHISEmailObj::send()
{
    if ( _to.isEmpty() && _recipients.isEmpty() ) {
        _lastError=tr( "No email recipient(s) specified." );
        PHIS_ERROR( _lastError );
        return 1;
    }
    qint16 res;
    res=connect();
    if ( res ) return res;
    QTextStream t( _socket );
    t.setCodec( "utf-8" );

    qDebug( "EMAIL %d %s", res, qPrintable( _lastError ) );
    QString data=QStringLiteral( "MAIL FROM: <" )+_from+QChar::fromLatin1( '>' );
    res=waitForDataWritten( data, t );
    if ( res ) return disconnectFromServer( res, t );
    res=waitForResponseLine();
    qDebug( "EMAIL %d %s", res, qPrintable( _lastError ) );
    if ( res!=250 ) return disconnectFromServer( res, t );
    if ( _recipients.count() ) {
        QString rcp;
        foreach ( rcp, _recipients ) {
            data=QStringLiteral( "RCPT TO: <" )+rcp+QChar::fromLatin1( '>' );
            res=waitForDataWritten( data, t );
            if ( res ) return disconnectFromServer( res, t );
            qDebug( "EMAIL %s %s", qPrintable( rcp ), qPrintable( _lastError ) );
            res=waitForResponseLine();
            if ( res!=250 ) {
                data=tr( "SMTP error for recipient '%1': <%2>." ).arg( rcp ).arg( _lastError );
                PHIS_WARN( data );
            }
        }
    } else {
        data=QStringLiteral( "RCPT TO: " )+_to;
        res=waitForDataWritten( data, t );
        if ( res ) return disconnectFromServer( res, t );
        res=waitForResponseLine();
        qDebug( "EMAIL %d %s", res, qPrintable( _lastError ) );
        if ( res!=250 ) return disconnectFromServer( res, t );
    }
    res=waitForDataWritten( QStringLiteral( "DATA" ), t );
    if ( res ) return disconnectFromServer( res, t );
    res=waitForResponseLine();
    qDebug( "EMAIL %d %s", res, qPrintable( _lastError ) );
    if ( res!=354 ) return disconnectFromServer( res, t );
    res=waitForMessageWritten( t );
    if ( res ) return disconnectFromServer( res, t );
    res=waitForResponseLine();
    qDebug( "EMAIL %d %s", res, qPrintable( _lastError ) );
    if ( res!=250 ) return disconnectFromServer( res, t );
    return disconnectFromServer( 0, t );
}

qint16 PHISEmailObj::sendTo( const QString &to, const QString &subject, const QString &body )
{
    if ( !_socket->isOpen() ) {
        _lastError=tr( "Could not execute sendTo(): not connected to SMTP server '%1'." ).arg( _server );
        PHIS_ERROR( _lastError );
        return 1;
    }
    if ( to.isEmpty() ) {
        _lastError=tr( "No email recipient specified." );
        PHIS_ERROR( _lastError );
        return 1;
    }
    QString fromEnvelope=_from;
    if ( fromEnvelope.isEmpty() ) {
        _lastError=tr( "No sender (FROM) specified." );
        PHIS_ERROR( _lastError );
        return 1;
    }
    qint16 res;
    QTextStream t( _socket );
    t.setCodec( "utf-8" );

    QString data=QStringLiteral( "MAIL FROM: <" )+fromEnvelope+QChar::fromLatin1('>');
    res=waitForDataWritten( data, t );
    if ( res ) return res;
    res=waitForResponseLine();
    qDebug( "EMAIL %d %s", res, qPrintable( _lastError ) );
    if ( res!=250 ) return res;
    data=QStringLiteral( "RCPT TO: " )+to;
    res=waitForDataWritten( data, t );
    if ( res ) return res;
    res=waitForResponseLine();
    qDebug( "EMAIL %d %s", res, qPrintable( _lastError ) );
    if ( res!=250 ) return res;
    res=waitForDataWritten( QStringLiteral( "DATA" ), t );
    if ( res ) return res;
    res=waitForResponseLine();
    qDebug( "EMAIL %d %s", res, qPrintable( _lastError ) );
    if ( res!=354 ) return res;
    res=waitForMessageWritten( t, to, subject, body );
    if ( res ) return res;
    res=waitForResponseLine();
    qDebug( "EMAIL %d %s", res, qPrintable( _lastError ) );
    if ( res!=250 ) return res;
    return 0;
}

qint16 PHISEmailObj::waitForResponseLine( bool *moreLines )
{
    Q_ASSERT( _socket );
    while ( 1 ) {
        if ( !_socket->canReadLine() && !_socket->waitForReadyRead( _timeout ) ) {
            _lastError=tr( "Could not read from SMTP server '%1': <%2>." )
                .arg( _server ).arg( _socket->errorString() );
            PHIS_ERROR( _lastError );
            if ( moreLines ) *moreLines=false;
            return 1;
        }
        if ( _socket->canReadLine() ) {
            _lastError=QString::fromLatin1( _socket->readLine( 1024 ) );
            _lastError.chop( 2 );
            //qDebug( "readLine %s", qPrintable( _lastError ) );
            if ( moreLines ) {
                if ( _lastError.at( 3 )==QChar::fromLatin1( '-' ) ) *moreLines=true;
                else *moreLines=false;
            }
            return _lastError.left( 3 ).toInt();
        }
    }
    return 1;
}

qint16 PHISEmailObj::waitForDataWritten( const QString &data, QTextStream &t )
{
    Q_ASSERT( _socket );
    t << data << "\r\n";
    t.flush();
    if ( !_socket->waitForBytesWritten( _timeout ) ) {
        _lastError=tr( "Could not write data to SMTP server '%1': <%2>." )
            .arg( _server ).arg( _socket->errorString() );
        PHIS_ERROR( _lastError );
        return 1;
    }
    return 0;
}

qint16 PHISEmailObj::waitForMessageWritten( QTextStream &t, const QString &to,
    const QString &subject, const QString &data )
{
    Q_ASSERT( _socket );
    QString toHeader=to;
    if ( toHeader.isEmpty() ) {
        if ( _to.isEmpty() && _recipients.count() ) toHeader=_recipients.first();
        else toHeader=_to;
    }
    QString fromHeader=_from;
    QString subjectHeader=subject;
    if ( subjectHeader.isEmpty() ) subjectHeader=_subject;
    QString body=data;
    if ( body.isEmpty() ) body=_data;
    QString nl=QStringLiteral( "\r\n" );
    QString d;
    d.reserve( body.size()+200 );
    d.append( QStringLiteral( "To: " )+toHeader+nl+QStringLiteral( "From: " )+fromHeader+nl );
    if ( !subjectHeader.isEmpty() ) d.append( QStringLiteral( "Subject: " )+subjectHeader+nl );
    d.append( QStringLiteral( "MIME-Version: 1.0\r\n" ) );
    d.append( QStringLiteral( "Content-type: " )+_contentType+nl );
    d.append( nl+body );
    d.replace( QStringLiteral( "\r\n.\r\n" ), QStringLiteral( "\r\n..\r\n" ) );
    t << d << "\r\n.\r\n";
    t.flush();
    if ( !_socket->waitForBytesWritten( _timeout ) ) {
        _lastError=tr( "Could not write data to smtp server '%1': <%2>." )
            .arg( _server ).arg( _socket->errorString() );
        PHIS_ERROR( _lastError );
        return 1;
    }
    return 0;
}
