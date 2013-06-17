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
#include <QSettings>
#include "phiservice.h"
#include "phi.h"
#include "phis.h"

PHIService::PHIService( int argc, char **argv, const QString &name )
    : QtServiceBase( argc, argv, name )
{
    qDebug( "PHIService::PHIService" );
    setServiceDescription( QObject::tr( "Phis web service V%1").arg( PHIS::libVersion() ) );
}

PHIService::~PHIService()
{
    qDebug( "PHIService::~PHIService" );
}

void PHIService::createApplication( int &argc, char **argv )
{
    _app=new QGuiApplication( argc, argv );
    _app->setApplicationVersion( PHIS::libVersion() );
    _app->setApplicationName( QStringLiteral( "Phis" ) );
    PHI::setupApplication( _app );
}

int PHIService::executeApplication()
{
    return _app->exec();
}

void PHIService::start()
{
    QString name=PHI::defaultString();
    _manager=new PHIManager( 0, name );
    _manager->start();
}

void PHIService::stop()
{
    _manager->stop();
    delete _manager;
}

void PHIService::pause()
{
    QtServiceBase::pause();
}

void PHIService::resume()
{
    _manager->updateConfig();
}

void PHIService::processCommand( int code )
{
    if ( code==2 ) setStartupType( QtServiceController::AutoStartup );
    else if ( code==3 ) setStartupType( QtServiceController::ManualStartup );
    else {
        _manager->updateConfig();
        return;
    }
#ifdef Q_OS_WIN
    QSettings *cs=PHI::globalSettings();
    cs->beginGroup( _manager->objectName() ); // "default"
    cs->setValue( QStringLiteral( "StartMode" ), code );
    cs->endGroup();

    QSettings s( QStringLiteral( "HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\services\\phis" ),
        QSettings::NativeFormat );
    s.setValue( QStringLiteral( "Start" ), code );
#endif
}
