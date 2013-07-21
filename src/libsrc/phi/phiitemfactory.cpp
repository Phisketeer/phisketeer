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
#include <QPluginLoader>
#include <QMetaClassInfo>
#include <QStringList>
#include <QDir>
#include "phiitemfactory.h"

PHIItemFactory* PHIItemFactory::_instance=0;

PHIItemFactory::PHIItemFactory( const QString &itemspath, QObject *parent )
    : QObject( parent ), _itemspath( itemspath )
{
    qDebug( "PHIItemFactory::PHIItemFactory()" );
    _instance=this;
    invalidate();
}

PHIItemFactory::~PHIItemFactory()
{
    _instance=0;
    _plugins.clear();
    _keys.clear();
    _wids.clear();
    qDebug( "PHIItemFactory::~PHIItemFactory()" );
}

void PHIItemFactory::invalidate()
{
    qDebug( "PHIItemFactory::invalidate()" );
    QDir dir ( _itemspath );
    if ( !dir.exists() ) {
        qWarning( "Did not find items path (%s)", qPrintable( dir.path() ) );
        return;
    }
    QStringList files=dir.entryList( QDir::Files );
    QString file;
    PHIItemPlugin *plugin;
    _lock.lockForWrite();
    _plugins.clear();
    _keys.clear();
    _wids.clear();
    _categories.clear();
    _keysForCategory.clear();
    _pixmapForCategory.clear();
    foreach ( file, files ) {
        QPluginLoader loader( dir.absoluteFilePath( file ) );
        if ( loader.isLoaded() ) loader.unload();
        plugin=qobject_cast<PHIItemPlugin*>(loader.instance());
        if ( !loader.isLoaded() ) {
            qWarning( "Could not load item plug-in '%s': %s", qPrintable( file ), qPrintable( loader.errorString() ) );
            continue;
        }
        if ( plugin ) {
            QStringList keys=plugin->keys();
            _keysForCategory.insert( plugin->category(), keys );
            _categories.insert( plugin->category() );
            _pixmapForCategory.insert( plugin->category(), plugin->pixmap() );
            foreach ( QString key, keys ) {
                _keys << key;
                _plugins.insert( key, plugin );
                _wids.insert( plugin->wid( key ), plugin );
            }
        }
    }
#ifdef PHIDEBUG
    qDebug() << _keys;
    qDebug() << _categories;
#endif
    _lock.unlock();
}

QString PHIItemFactory::category( const QString &key ) const
{
    QReadLocker l( &_lock );
    PHIItemPlugin *plugin=_plugins.value( key, 0 );
    if ( plugin ) return plugin->category();
    return QString();
}

/*
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
*/
