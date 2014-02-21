/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2014  Phisys AG, Switzerland
#    Copyright (C) 2012-2014  Phisketeer.org team
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
#include <QDir>
#include <QPluginLoader>
#include <QMetaClassInfo>
#include "phismodulefactory.h"
#include "phismodule.h"
#include "phiapplication.h"

PHISModuleFactory* PHISModuleFactory::_instance=0;

PHISModuleFactory::PHISModuleFactory( QObject *parent )
    : QObject( parent )
{
    qDebug( "PHISModuleFactory::PHISModuleFactory()" );
}

PHISModuleFactory::~PHISModuleFactory()
{
    _instance=0;
    qDebug( "PHISModuleFactory::~PHISModuleFactory()" );
}

void PHISModuleFactory::invalidate()
{
    qDebug( "PHISModuleFactory::invalidate()" );
    _loadErorrs.clear();
    QDir dir ( phiApp->modulesPath() );
    if ( !dir.exists() ) {
        _loadErorrs << tr( "Server module directory '%1' not found! " ).arg( phiApp->modulesPath() );
        return;
    }
    QStringList files=dir.entryList( QDir::Files );
    QString file;
    PHISModule *mod;
    _lock.lockForWrite();
    _modules.clear();
    foreach ( file, files ) {
        QPluginLoader loader( dir.absoluteFilePath( file ) );
        if ( loader.isLoaded() ) loader.unload();
        mod=qobject_cast<PHISModule*>(loader.instance());
        if ( !loader.isLoaded() ) {
            _loadErorrs << tr( "Could not load module '%1': %2" ).arg( file ).arg( loader.errorString() );
            continue;
        }
        if ( mod ) {
            QStringList keys=mod->keys();
            foreach ( QString key, keys ) {
                _modules.insert( key, mod );
                _libNames.insert( key, file );
                _keys << key;
            }
        }
    }
    _lock.unlock();
}

QStringList PHISModuleFactory::loadedModules() const
{
    _lock.lockForRead();
    QStringList list;
    PHISModule *mod;
    foreach ( QString key, _keys ) {
        mod=_modules.value( key );
        Q_ASSERT( mod );
        const QMetaObject *obj=mod->metaObject();
        QString na=tr( "n/a" );
        QString author=QString::fromUtf8( obj->classInfo( obj->indexOfClassInfo( "Author" ) ).value() );
        QString url=QString::fromUtf8( obj->classInfo( obj->indexOfClassInfo( "Url" ) ).value() );
        QString cr=QString::fromUtf8( obj->classInfo( obj->indexOfClassInfo( "Copyright" ) ).value() );
        QString v=QString::fromUtf8( obj->classInfo( obj->indexOfClassInfo( "Version" ) ).value() );
        QString lic=QString::fromUtf8( obj->classInfo( obj->indexOfClassInfo( "License" ) ).value() );
        list << tr( "Library '%1' provides module '%2' (%3).\nAuthor:\t  %4\nUrl:\t  %5"
                    "\nLicense:  %6\nCopyright %7" )
            .arg( _libNames.value( key ) ).arg( key ).arg( v )
            .arg( author.isEmpty() ? na : author ).arg( url.isEmpty() ? na : url )
            .arg( lic.isEmpty() ? na : lic ).arg( cr.isEmpty() ? na : cr );
    }
    _lock.unlock();
    return list;
}
