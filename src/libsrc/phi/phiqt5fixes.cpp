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
#include <QSettings>

#ifdef Q_OS_WIN
extern QString qAppFileName();
#endif

void phiSetPluginPath( int argc, char **argv )
{
    Q_UNUSED( argc )
    QDir plugins;

#ifdef Q_OS_WIN
    Q_UNUSED( argv )
    plugins=QFileInfo( qAppFileName() ).dir();
    plugins.cd( QLatin1String( "plugins" ) );
#elif defined Q_OS_LINUX
    Q_UNUSED( argv )
    QSettings s( QLatin1String( "/etc/phi/phis.conf" ), QSettings::IniFormat );
    QString fallback=QString::fromLatin1( "/opt/phisketeer/plugins" );
    plugins.setPath( s.value( QLatin1String( "PluginsPath" ), fallback ).toString() );
#elif defined Q_OS_MAC
    plugins=QFileInfo( QString::fromLocal8Bit( argv[0] ) ).dir();
    plugins.cdUp();
    plugins.cd( QLatin1String( "PlugIns" ) );
#else
#error Unsupported OS
#endif
    qDebug( "Plugin path=%s", qPrintable( plugins.canonicalPath() ) );
    qputenv( "QT_PLUGIN_PATH", plugins.canonicalPath().toLocal8Bit() );
}
