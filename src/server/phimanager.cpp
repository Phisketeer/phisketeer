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
#include <QTimer>
#include <QFile>
#include "phimanager.h"
#include "phiconfig.h"
#include "philogwriter.h"
#include "phierror.h"
#include "philistener.h"
#include "phiparent.h"

#ifndef Q_OS_MAC
#include "phiservice.h"
#endif

PHIManager::PHIManager( QObject *parent, const QString &mgrName )
    : QObject( parent ), _logWriterStart( false ), _sslListenerStart( false )
{
    qDebug( "PHIManager::PHIManager()" );
    setObjectName( mgrName );
    PHIRC rc=PHIConfig::instance()->init( this );
    if ( rc!=PHIRC_OK ) {
#ifdef Q_OS_MAC
        qWarning( "%d: %s", rc, qPrintable( PHIError::instance()->longDesc( rc ) ) );
#else
        PHIService::instance()->logMessage( PHIError::instance()->longDesc( rc ),
            QtServiceBase::Error, static_cast<int>(rc) );
#endif
        return;
    }

    rc=PHILogWriterThread::instance()->init( this );
    if ( rc!=PHIRC_OK ) {
#ifdef Q_OS_MAC
        qWarning( "%d: %s", rc, qPrintable( PHIError::instance()->longDesc( rc ) ) );
#else
        PHIService::instance()->logMessage( PHIError::instance()->longDesc( rc ),
            QtServiceBase::Error, static_cast<int>(rc) );
#endif
        return;
    }
    _logWriterStart=true;

    PHIParent::instance( this );
}

PHIManager::~PHIManager()
{
    if ( _logWriterStart ) delete PHILogWriterThread::instance();
    delete PHIConfig::instance();
    qDebug( "PHIManager::~PHIManager()" );
}

void PHIManager::start()
{
    if ( !_logWriterStart ) return;
    PHILogWriter::instance()->log( PHILOGTRACE, PHIRC_OK, tr( "Using mime type file <%1>." )
        .arg( PHIConfig::instance()->mimeTypesFile() ) );

    PHIRC rc=PHIListener::instance()->init( this );
    if ( rc==PHIRC_OK ) {
        PHILogWriter::instance()->log( PHILOGTRACE, PHIRC_MGR_START,
            tr( "Phi manager '%1' ready for service." ).arg( objectName() ) );
        connect( PHIListener::instance(), SIGNAL( processServiceCommand( int ) ),
            this, SIGNAL( processServiceCommand( int ) ) );
    }
    if ( PHIConfig::instance()->configValue( "SSLEnabled", false ).toBool() ) {
        rc=PHISslListener::instance()->init( this );
        _sslListenerStart=true;
    }
}

void PHIManager::updateConfig()
{
    PHIConfig::instance()->updateConfig();
    PHIParent::instance()->invalidate();
}

void PHIManager::stop()
{
    if ( !_logWriterStart ) return;
    qDebug( "PHIManager::stop()" );
    PHILogWriter::instance()->log( PHILOGTRACE, PHIRC_MGR_STOP,
        tr( "Shuting down phis service '%1'." ).arg( objectName() ) );
    delete PHIListener::instance();
    if ( _sslListenerStart ) delete PHISslListener::instance();
}
