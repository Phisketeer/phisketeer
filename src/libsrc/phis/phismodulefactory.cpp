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
#include <QSettings>
#include <QCoreApplication>
#include <QDir>
#include <QPluginLoader>
#include "phismodulefactory.h"
#include "phismodule.h"
#include "phi.h"

PHISModuleFactory* PHISModuleFactory::_instance=0;

PHISModuleFactory::PHISModuleFactory( QObject *parent )
    : QObject( parent )
{
    qDebug( "PHISModuleFactore::PHISModuleFactory()" );
    invalidate();
}

PHISModuleFactory::~PHISModuleFactory()
{
    qDebug( "PHISModuleFactore::~PHISModuleFactory()" );
}

void PHISModuleFactory::invalidate()
{
    qDebug( "PHISModuleFactory::invalidate()" );
    QSettings *s=PHI::globalSettings();
    QString path=s->value( "PluginsPath", QString() ).toString();
    QDir dir ( path+QDir::separator()+"modules" );
    if ( !dir.exists() ) {
        path=QCoreApplication::instance()->applicationDirPath();
#ifdef Q_OS_WIN
        dir.setPath( path+QDir::separator()+"plugins"+QDir::separator()+"modules" );
#elif defined Q_OS_UNIX
#ifdef Q_OS_MAC
        dir.setPath( path ); // MacOS
        dir.cdUp(); //
        dir.cdUp(); // Contents
        dir.setPath( dir.path()+"/PlugIns/modules" );
#endif
#endif
        if ( !dir.exists() ) {
            qDebug( "Did not find modules path (%s)", qPrintable( dir.path() ) );
            return;
        }
    }
    QStringList files=dir.entryList( QDir::Files );
    QString file;
    PHISModule *mod;
    _lock.lockForWrite();
    _modules.clear();
    foreach ( file, files ) {
        QPluginLoader loader( file );
        if ( loader.isLoaded() ) loader.unload();
        mod=qobject_cast<PHISModule*>(loader.instance());
        if ( mod ) {
            QStringList keys=mod->keys();
            foreach ( QString key, keys ) {
                _modules.insert( key, mod );
            }
        }
    }
    _lock.unlock();
}
