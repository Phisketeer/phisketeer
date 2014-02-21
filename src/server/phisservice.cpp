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
#include <QSettings>
#include "phisservice.h"
#include "phiapplication.h"
#include "phisparent.h"
#include "phis.h"

PHISService::PHISService( int argc, char **argv, const QString &name )
    : QtServiceBase( argc, argv, name ), _app( 0 )
{
    qDebug( "PHISService::PHISService" );
    setServiceDescription( QObject::tr( "Phis Web service V%1" ).arg( L1( PHIVERSION ) ) );
}

PHISService::~PHISService()
{
    delete _app;
    qDebug( "PHISService::~PHISService" );
}

void PHISService::createApplication( int &argc, char **argv )
{
    _app=new PHIApplication( argc, argv, "phis", PHIVERSION, PHIApplication::Service );
    PHISParent::instance( _app, PHI::defaultString() );
}

int PHISService::executeApplication()
{
    return _app->exec();
}

void PHISService::start()
{
    PHISParent::instance()->startService();
}

void PHISService::stop()
{
    PHISParent::instance()->stopService();
}

void PHISService::pause()
{
    PHISParent::instance()->stopService();
}

void PHISService::resume()
{
    PHISParent::instance()->startService();
}

void PHISService::processCommand( int code )
{
    if ( code==2 ) setStartupType( QtServiceController::AutoStartup );
    else if ( code==3 ) setStartupType( QtServiceController::ManualStartup );
    else {
        PHISParent::instance()->invalidate();
        return;
    }
#ifdef Q_OS_WIN
    QSettings *cs=phiApp->serverSettings();
    cs->beginGroup( PHI::defaultString() ); // "default"
    cs->setValue( L1( "StartMode" ), code );
    cs->endGroup();

    QSettings s( L1( "HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\services\\phis" ), QSettings::NativeFormat );
    s.setValue( QStringLiteral( "Start" ), code );
#endif
}
