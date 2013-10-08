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
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PHIDISPLAYITEMS_H
#define PHIDISPLAYITEMS_H
#include "phiitemplugin.h"
#include "phitextitems.h"
#include "phiimageitems.h"
#include "phi.h"

class PHIDisplayItems : public PHIItemPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.phisketeer.phi.items.display" FILE "phidisplay.json")
    Q_CLASSINFO( "Author", "Marius Schumacher" )
    Q_CLASSINFO( "Url", "http://www.phisketeer.org" )
    Q_CLASSINFO( "Version", "1.0" )
    Q_CLASSINFO( "License", "LGPL" )
    Q_CLASSINFO( "Copyright", "2013 Phisys AG, 2013 Phisketeer Team" )

public:
    virtual PHIBaseItem* create( PHIWID wid, const PHIBaseItemPrivate &p ) const;
    virtual PHIBaseItem* copy( const PHIBaseItem *it ) const;
    virtual QStringList keys() const;
    virtual PHIWID wid( const QString &key ) const;
    virtual QString category() const { return tr( "Display" ); }
};

inline PHIBaseItem* PHIDisplayItems::create( PHIWID wid, const PHIBaseItemPrivate &p ) const
{
    switch ( wid ) {
    case PHILabelItem::Label: return new PHILabelItem( p );
    case PHIPhisysItem::Phisys: return new PHIPhisysItem( p );
    }
    return 0;
}

inline PHIBaseItem* PHIDisplayItems::copy( const PHIBaseItem *it ) const
{
    switch ( it->wid() ) {
    case PHILabelItem::Label: return new PHILabelItem( *qobject_cast<const PHILabelItem*>(it) );
    case PHIPhisysItem::Phisys: return new PHIPhisysItem( *qobject_cast<const PHIPhisysItem*>(it) );
    }
    return 0;
}

inline QStringList PHIDisplayItems::keys() const
{
    return QStringList() << SL( "label" ) << SL( "phisys" );
}

inline PHIWID PHIDisplayItems::wid( const QString &key ) const
{
    if ( key==L1( "label" ) ) return PHILabelItem::Label;
    if ( key==L1( "phisys" ) ) return PHIPhisysItem::Phisys;
    return 0;
}

#endif // PHIDISPLAYITEMS_H
