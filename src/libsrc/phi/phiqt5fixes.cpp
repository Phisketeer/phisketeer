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
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "phiqt5fixes.h"
#include <QFileInfo>

#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
extern QString qAppFileName();
#else
static QString qAppFileName();
#endif

void phiSetPluginPath( int argc, char **argv )
{
    Q_UNUSED( argc );
    Q_UNUSED( argv );
    QDir plugins=QFileInfo( qAppFileName() ).dir();
#ifdef Q_OS_WIN
    plugins.cd( QLatin1String( "plugins" ) );
#endif
    qDebug( "APPNAME=%s", qPrintable( plugins.canonicalPath() ) );
    qputenv( "QT_PLUGIN_PATH", plugins.canonicalPath().toLocal8Bit() );
}

#if defined(Q_OS_WIN) || defined(Q_OS_MAC)

#else
static QString qAppFileName()
{
#ifndef Q_OS_LINUX
#error qAppFileName is not defined correctly.
#endif
    QFileInfo fi( QLatin1String( "/proc/%1/exe" ).arg( getpid() ) );
    if ( fi.exists() && pfi.isSymLink() ) {
        return fi.canonicalFilePath();
    }
    return QString();
}
#endif
