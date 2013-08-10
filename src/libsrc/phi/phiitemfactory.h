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
#ifndef PHIITEMFACTORY_H
#define PHIITEMFACTORY_H
#include <QObject>
#include <QHash>
#include <QReadWriteLock>
#include <QReadLocker>
#include "phibaseitem.h"
#include "phiitemplugin.h"

class PHIItemFactory : public QObject
{
    Q_OBJECT

public:
    explicit PHIItemFactory( const QString &itemspath, QObject *parent );
    virtual ~PHIItemFactory();
    static inline PHIItemFactory* instance() { return _instance; }

    void invalidate();
    inline QStringList keys() const { QReadLocker l( &_lock ); return _keys; }
    inline QStringList categoryList() const { QReadLocker l( &_lock ); return _categories.toList(); }
    inline QPixmap pixmapForCategory( const QString &cat ) const {
        QReadLocker l( &_lock ); return _pixmapForCategory.value( cat ); }
    PHIBaseItem* item( const QString &key ) const;
    PHIBaseItem* item( PHIWID wid ) const;
    QString category( const QString &key ) const;
    QStringList keysForCategory( const QString &cat ) const;
    
private:
    static PHIItemFactory *_instance;
    mutable QReadWriteLock _lock;
    QString _itemspath;
    QStringList _keys;
    QHash <QString, PHIItemPlugin*> _plugins;
    QHash <PHIWID, PHIItemPlugin*> _wids;
    QHash <QString, QStringList> _keysForCategory;
    QSet <QString> _categories;
    QHash <QString, QPixmap> _pixmapForCategory;
};

inline PHIBaseItem* PHIItemFactory::item( const QString &key ) const
{
    QReadLocker l( &_lock );
    PHIItemPlugin *plugin=_plugins.value( key, 0 );
    if ( plugin ) return plugin->create( plugin->wid( key ) );
    return 0;
}

inline PHIBaseItem* PHIItemFactory::item( PHIWID wid ) const
{
    QReadLocker l( &_lock );
    PHIItemPlugin *plugin=_wids.value( wid, 0 );
    if ( plugin ) return plugin->create( wid );
    return 0;
}

inline QStringList PHIItemFactory::keysForCategory( const QString &cat ) const
{
    QReadLocker l( &_lock );
    return _keysForCategory.value( cat, QStringList() );
}

#endif // PHIITEMFACTORY_H
