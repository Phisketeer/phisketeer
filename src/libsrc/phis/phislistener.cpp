/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2014  Phisys AG, Switzerland
#    Copyright (C) 2012-2014  Phisketeer.org team
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
#include <QDateTime>
#include <QHostAddress>
#include "phislistener.h"
#include "phierror.h"
#include "phislogwriter.h"
#include "phisconnthread.h"
#include "phiapplication.h"

PHISListener* PHISListener::_instance=0;

PHISListener::PHISListener( QObject *parent )
    : QTcpServer( parent )
{
    qDebug( "PHISListener::PHISListener()" );
    connect( this, SIGNAL( log( int, const char*, int, const QDateTime&, PHIRC, const QString& ) ),
        PHISLogWriter::instance(), SLOT( log( int, const char*, int, const QDateTime&, PHIRC, const QString& ) ) );
}

PHISListener::~PHISListener()
{
    _instance=0;
    qDebug( "PHISListener::~PHISListener()" );
}

PHISListener* PHISListener::instance()
{
    if ( !_instance ) _instance=new PHISListener();
    return _instance;
}

PHIRC PHISListener::init( QObject *parent, const QString &name )
{
    Q_ASSERT( parent );
    setParent( parent );
    QHostAddress host;
    QSettings *s=phiApp->serverSettings();
    s->beginGroup( name );
    QString address=s->value( SL( "ListenerIF" ), SL( "Any" ) ).toString();
    quint16 port=s->value( SL( "ListenerPort" ), 80 ).value<quint16>();
    s->endGroup();
    QString todo=tr( "Check '%1' in your Phis service configuration." );
    if ( address.toUpper()==L1( "ANY" ) ) host.setAddress( QHostAddress::Null );
    else if ( address.toUpper()==L1( "LOCALHOST" ) ) host.setAddress( QHostAddress::LocalHost );
    else if ( address==L1( "127.0.0.1" ) ) host.setAddress( QHostAddress::LocalHost );
    else if ( address==L1( "::1" ) ) host.setAddress( QHostAddress::LocalHostIPv6 );
    else if ( !host.setAddress( address ) ){
        todo=todo.arg( SL( "ListenerIF" ) );
        emit log( PHILOGERR, PHIRC_TCP_IP_ADDRESS_ERROR, todo );
        return PHIRC_TCP_IP_ADDRESS_ERROR;
    }
    if ( !listen( host, port ) ){
        todo=tr( "Could not bind port. Check '%1' in your Phis service configuration." ).arg( SL( "ListenerPort" ) );
        emit log( PHILOGERR, PHIRC_TCP_IP_ADDRESS_ERROR, todo );
        return PHIRC_TCP_IP_ADDRESS_ERROR;
    }
    todo=tr( "Listener '%1' on port '%2' started." ).arg( host.toString() ).arg( port );
    emit log( PHILOGTRACE, PHIRC_TCP_LISTENER_START, todo );
    return PHIRC_OK;
}

void PHISListener::incomingConnection( qintptr sockdesc )
{
    try {
        PHISConnThread *conn=new PHISConnThread( this, sockdesc, false );
        conn->start();
    } catch ( std::bad_alloc& ) {
        emit log( PHILOGCRIT, PHIRC_OUT_OF_MEMORY, tr( "Out of memory" ) );
        // @todo: close socket
    }
}

PHISslListener* PHISslListener::_instance=0;

PHISslListener::PHISslListener( QObject *parent ) : QTcpServer( parent )
{
    qDebug( "PHISslListener::PHISslListener()" );
    connect( this, SIGNAL( log( int, const char*, int, const QDateTime&, PHIRC, const QString& ) ),
        PHISLogWriter::instance(), SLOT( log( int, const char*, int, const QDateTime&, PHIRC, const QString& ) ) );
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

PHIRC PHISslListener::init( QObject *parent , const QString &name )
{
    Q_ASSERT( parent );
    setParent( parent );
    QHostAddress host;
    QSettings *s=phiApp->serverSettings();
    s->beginGroup( name );
    QString address=s->value( SL( "SSLListenerIF" ), SL( "Any" ) ).toString();
    quint16 port=s->value( SL( "SSLListenerPort" ), 443 ).value<quint16>();
    s->endGroup();
    QString todo=tr( "Check '%1' in your Phis service configuration." );
    if ( address.toUpper()==L1( "ANY" ) ) host.setAddress( QHostAddress::Null );
    else if ( address.toUpper()==L1( "LOCALHOST" ) ) host.setAddress( QHostAddress::LocalHost );
    else if ( address==L1( "127.0.0.1" ) ) host.setAddress( QHostAddress::LocalHost );
    else if ( address==L1( "::1" ) ) host.setAddress( QHostAddress::LocalHostIPv6 );
    else if ( !host.setAddress( address ) ){
        todo=todo.arg( SL( "SSLListenerIF" ) );
        emit log( PHILOGERR, PHIRC_TCP_IP_ADDRESS_ERROR, todo );
        return PHIRC_TCP_IP_ADDRESS_ERROR;
    }
    if ( !listen( host, port ) ){
        todo=tr( "Could not bind port. Check '%1' in your Phis service configuration." ).arg( SL( "SSLListenerPort" ) );
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
        PHISConnThread *conn=new PHISConnThread( this, sockdesc, true );
        conn->start();
    } catch ( std::bad_alloc& ) {
        emit log( PHILOGCRIT, PHIRC_OUT_OF_MEMORY, tr( "Out of memory" ) );
        // @todo: close socket
    }
}
