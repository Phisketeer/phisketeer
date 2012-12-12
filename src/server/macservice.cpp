/*
#    Copyright (C) 2010-2012  Marius B. Schumacher
#    Copyright (C) 2011-2012  Phisys AG, Switzerland
#    Copyright (C) 2012  Phisketeer.org team
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
#include <QApplication>
#include <QLocalServer>
#include <QTimer>
#include <QDesktopServices>
#include <QDir>
#include "macservice.h"
#include "phimanager.h"
#include "phi.h"
#include "phis.h"

MACService::MACService( int argc, char **argv, const QString &name )
    : QObject( 0 ), _name( name )
{
    qDebug( "MACService::MACService" );
    _app=new QApplication( argc, argv, false );
    _app->setApplicationVersion( PHIS::libVersion() );
    _app->setApplicationName( "Phis" );
    PHI::setupApplication( _app );
    QTimer::singleShot( 0, this, SLOT( start() ) );
}

MACService::~MACService()
{
    qDebug( "MACService::~MACService" );
}

void MACService::processServiceCommand( int code )
{
    qDebug( "processServiceCommand %d", code );
    if ( code==1 ) { // stop service
        stop();
    } else if ( code==2 ) { // invalidate cache
        _manager->updateConfig();
    }
}

int MACService::exec()
{
    return _app->exec();
}

void MACService::start()
{
    QString n( "default" );
    _manager=new PHIManager( this, n );
    connect( _manager, SIGNAL( processServiceCommand( int ) ), this,
        SLOT( processServiceCommand( int ) ) );
    _manager->start();
}

void MACService::stop()
{
    _manager->stop();
    delete _manager;
    _app->quit();
}
