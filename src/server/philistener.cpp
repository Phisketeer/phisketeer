/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2013  Phisys AG, Switzerland
#    Copyright (C) 2012-2013  Phisketeer.org team
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QTcpSocket>
#include "philistener.h"
#include "phierror.h"
#include "philogwriter.h"
#include "phiconfig.h"
#include "phiconnthread.h"
#include "phiobject.h"

PHIListener* PHIListener::_instance=0;

PHIListener::PHIListener( QObject *parent )
    : QTcpServer( parent )
{
    qDebug( "PHIListener::PHIListener()" );
    connect( this, SIGNAL( log( int, const char*, int,
        const QDateTime&, PHIRC, const QString& ) ), PHILogWriter::instance(),
        SLOT( log( int, const char*, int, const QDateTime&, PHIRC, const QString& ) ) );
}

PHIListener::~PHIListener()
{
    _instance=0;
    qDebug( "PHIListener::~PHIListener()" );
}

PHIListener* PHIListener::instance()
{
    if ( !_instance ) _instance=new PHIListener();
    return _instance;
}

PHIRC PHIListener::init( QObject *parent )
{
    if ( !parent ) return PHIRC_ARGUMENT_ERROR;
    setParent( parent );
    QHostAddress host;
    QString todo=tr( "Check '%1' in your PHI manager configuration." );
    QString address=PHIConfig::instance()->configValue(
        QStringLiteral( "ListenerIF" ), QStringLiteral( "Any" ) ).toString();
    if ( address.toUpper()==QLatin1String( "ANY" ) )
        host.setAddress( QHostAddress::Null );
    else if ( address.toUpper()==QLatin1String( "LOCALHOST" ) )
        host.setAddress( QHostAddress::LocalHost );
    else if ( !host.setAddress( address ) ){
        todo=todo.arg( QStringLiteral( "ListenerIF" ) );
        emit log( PHILOGERR, PHIRC_TCP_IP_ADDRESS_ERROR, todo );
        return PHIRC_TCP_IP_ADDRESS_ERROR;
    }
    qint16 port=PHIConfig::instance()->configValue( QStringLiteral( "ListenerPort" ), 80 ).toInt();
    if ( !listen( host, port ) ){
        todo=tr( "Could not bind port. Check '%1' in your PHI manager configuration." )
            .arg( QStringLiteral( "ListenerPort" ) );
        emit log( PHILOGERR, PHIRC_TCP_IP_ADDRESS_ERROR, todo );
        return PHIRC_TCP_IP_ADDRESS_ERROR;
    }
    todo=tr( "Listener '%1' on port '%2' started." ).arg( host.toString() ).arg( port );
    emit log( PHILOGTRACE, PHIRC_TCP_LISTENER_START, todo );
    return PHIRC_OK;
}

void PHIListener::incomingConnection( qintptr sockdesc )
{
    qDebug( "PHIListener::incomingConnection %lld", sockdesc );
    try {
        PHIConnThread *conn=new PHIConnThread( this, sockdesc, false );
#ifdef Q_OS_MAC
        connect( conn, SIGNAL( processServiceCommand( int ) ),
            this, SIGNAL( processServiceCommand( int ) ) );
#endif
        conn->start();
    } catch ( std::bad_alloc& ) {
        emit log( PHILOGCRIT, PHIRC_OUT_OF_MEMORY, tr( _phioutofmemory ) );
        /** @todo close socket */
    }
}

PHISslListener* PHISslListener::_instance=0;

PHISslListener::PHISslListener( QObject *parent )
    : QTcpServer( parent )
{
    qDebug( "PHISslListener::PHISslListener()" );
    connect( this, SIGNAL( log( int, const char*, int,
        const QDateTime&, PHIRC, const QString& ) ), PHILogWriter::instance(),
        SLOT( log( int, const char*, int, const QDateTime&, PHIRC, const QString& ) ) );
}

PHISslListener::~PHISslListener()
{
    _instance=0;
    qDebug( "PHISslListener::~PHISslListener()" );
}

PHISslListener* PHISslListener::instance()
{
    if ( !_instance ) _instance=new PHISslListener();
    return _instance;
}

PHIRC PHISslListener::init( QObject *parent )
{
    if ( !parent ) return PHIRC_ARGUMENT_ERROR;
    setParent( parent );
    QHostAddress host;
    QString todo=tr( "Check '%1' in your PHI manager configuration." );
    QString address=PHIConfig::instance()->configValue(
        QStringLiteral( "SSLListenerIF" ), QStringLiteral( "Any" ) ).toString();
    if ( address.toUpper()==QLatin1String( "ANY" ) )
        host.setAddress( QHostAddress::Null );
    else if ( address.toUpper()==QLatin1String( "LOCALHOST" ) )
        host.setAddress( QHostAddress::LocalHost );
    else if ( !host.setAddress( address ) ){
        todo=todo.arg( QStringLiteral( "SSLListenerIF" ) );
        emit log( PHILOGERR, PHIRC_TCP_IP_ADDRESS_ERROR, todo );
        return PHIRC_TCP_IP_ADDRESS_ERROR;
    }
    qint16 port=PHIConfig::instance()->configValue( QStringLiteral( "SSLListenerPort" ), 443 ).toInt();
    if ( !listen( host, port ) ){
        todo=tr( "Could not bind port. Check '%1' in your PHI manager configuration." )
            .arg( QStringLiteral( "SSLListenerPort" ) );
        emit log( PHILOGERR, PHIRC_TCP_IP_ADDRESS_ERROR, todo );
        return PHIRC_TCP_IP_ADDRESS_ERROR;
    }
    todo=tr( "SSL listener '%1' on port '%2' started." ).arg( host.toString() ).arg( port );
    emit log( PHILOGTRACE, PHIRC_TCP_LISTENER_START, todo );
    return PHIRC_OK;
}

void PHISslListener::incomingConnection( qintptr sockdesc )
{
    try {
        PHIConnThread *conn=new PHIConnThread( this, sockdesc, true );
        conn->start();
    } catch ( std::bad_alloc& ) {
        emit log( PHILOGCRIT, PHIRC_OUT_OF_MEMORY, tr( _phioutofmemory ) );
        /** @todo close socket */
    }
}
