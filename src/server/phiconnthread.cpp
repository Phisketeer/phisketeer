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
#include "phiconnthread.h"
#include "phierr.h"
#include "phihttp.h"

PHIConnThread::PHIConnThread(QObject *parent, qintptr sockdesc, bool usessl )
    : QThread( parent ), _socketDesc( sockdesc ), _usessl( usessl )
{
    //qDebug( "PHIConnThread::PHIConnThread()" );
}

PHIConnThread::~PHIConnThread()
{
    quit();
    wait();
    //qDebug( "PHIConnThread::~PHIConnThread()" );
}

void PHIConnThread::run()
{
    //qDebug( "PHIConnThread::PHIConnThread(): run(), thread %p", QThread::currentThread() );
    QString err;
    PHIHttp http;
    if ( http.init( _socketDesc, _usessl, err )!=PHIRC_OK ) {
        deleteLater();
        return;
    }
    connect( &http, SIGNAL( close() ), this, SLOT( deleteLater() ) );
#ifdef Q_OS_MAC
    connect( &http, SIGNAL( processServiceCommand( int ) ),
        this, SIGNAL( processServiceCommand( int ) ) );
#endif
    exec();
}
