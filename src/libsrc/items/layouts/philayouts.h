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
#ifndef PHILAYOUTS_H
#define PHILAYOUTS_H
#include "phiitemplugin.h"
#include "philayoutitems.h"
#include "phicontaineritems.h"
#include "phispaceritems.h"
#include "phi.h"

class PHILayouts : public PHIItemPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.phisketeer.phi.items.layouts" FILE "philayouts.json")
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
    virtual QString category() const { return tr( "Layout" ); }
};

inline PHIBaseItem* PHILayouts::create( PHIWID wid, const PHIBaseItemPrivate &p ) const
{
    switch ( wid ) {
    case PHIVerticalLayoutItem::VerticalLayout: return new PHIVerticalLayoutItem( p );
    case PHIHorizontalLayoutItem::HorizontalLayout: return new PHIHorizontalLayoutItem( p );
    case PHIFormLayoutItem::FormLayout: return new PHIFormLayoutItem( p );
    case PHIGridLayoutItem::GridLayout: return new PHIGridLayoutItem( p );
    case PHIHSpacerItem::HorizontalSpacer: return new PHIHSpacerItem( p );
    case PHIVSpacerItem::VerticalSpacer: return new PHIVSpacerItem( p );
    case PHIAddressContainerItem::AddressContainer: return new PHIAddressContainerItem( p );
    case PHILoginContainerItem::LoginContainer: return new PHILoginContainerItem( p );
    case PHICCContainerItem::CreditCardContainer: return new PHICCContainerItem( p );
    case PHIContactContainerItem::ContactContainer: return new PHIContactContainerItem( p );
    case PHIPeriodContainerItem::PeriodContainer: return new PHIPeriodContainerItem( p );
    case PHIRegisterUserContainerItem::RegisterUserContainer: return new PHIRegisterUserContainerItem( p );
    }
    return 0;
}

inline PHIBaseItem* PHILayouts::copy( const PHIBaseItem *it ) const
{
    switch ( it->wid() ) {
    case PHIVerticalLayoutItem::VerticalLayout: return new PHIVerticalLayoutItem( *qobject_cast<const PHIVerticalLayoutItem*>(it) );
    case PHIHorizontalLayoutItem::HorizontalLayout: return new PHIHorizontalLayoutItem( *qobject_cast<const PHIHorizontalLayoutItem*>(it) );
    case PHIFormLayoutItem::FormLayout: return new PHIFormLayoutItem( *qobject_cast<const PHIFormLayoutItem*>(it) );
    case PHIGridLayoutItem::GridLayout: return new PHIGridLayoutItem( *qobject_cast<const PHIGridLayoutItem*>(it) );
    case PHIHSpacerItem::HorizontalSpacer: return new PHIHSpacerItem( *qobject_cast<const PHIHSpacerItem*>(it) );
    case PHIVSpacerItem::VerticalSpacer: return new PHIVSpacerItem( *qobject_cast<const PHIVSpacerItem*>(it) );
    case PHIAddressContainerItem::AddressContainer: return new PHIAddressContainerItem( *qobject_cast<const PHIAddressContainerItem*>(it) );
    case PHILoginContainerItem::LoginContainer: return new PHILoginContainerItem( *qobject_cast<const PHILoginContainerItem*>(it) );
    case PHICCContainerItem::CreditCardContainer: return new PHICCContainerItem( *qobject_cast<const PHICCContainerItem*>(it) );
    case PHIContactContainerItem::ContactContainer: return new PHIContactContainerItem( *qobject_cast<const PHIContactContainerItem*>(it) );
    case PHIPeriodContainerItem::PeriodContainer: return new PHIPeriodContainerItem( *qobject_cast<const PHIPeriodContainerItem*>(it) );
    case PHIRegisterUserContainerItem::RegisterUserContainer: return new PHIRegisterUserContainerItem( *qobject_cast<const PHIRegisterUserContainerItem*>(it) );
    }
    return 0;
}

inline QStringList PHILayouts::keys() const
{
    return QStringList() << SL( "hboxlayout" ) << SL( "vboxlayout" )
        << SL( "formlayout" ) << SL( "gridlayout" ) << SL( "login" )
        << SL( "address" ) << SL( "creditcard" ) << SL( "registeruser" )
        << SL( "contact" ) << SL( "period" ) << SL( "vspacer" ) << SL( "hspacer" );
}

inline PHIWID PHILayouts::wid( const QString &key ) const
{
    if ( key==L1( "hboxlayout" ) ) return PHIHorizontalLayoutItem::HorizontalLayout;
    if ( key==L1( "vboxlayout" ) ) return PHIVerticalLayoutItem::VerticalLayout;
    if ( key==L1( "formlayout" ) ) return PHIFormLayoutItem::FormLayout;
    if ( key==L1( "gridlayout" ) ) return PHIGridLayoutItem::GridLayout;
    if ( key==L1( "hspacer" ) ) return PHIHSpacerItem::HorizontalSpacer;
    if ( key==L1( "vspacer" ) ) return PHIVSpacerItem::VerticalSpacer;
    if ( key==L1( "address" ) ) return PHIAddressContainerItem::AddressContainer;
    if ( key==L1( "login" ) ) return PHILoginContainerItem::LoginContainer;
    if ( key==L1( "creditcard" ) ) return PHICCContainerItem::CreditCardContainer;
    if ( key==L1( "contact" ) ) return PHIContactContainerItem::ContactContainer;
    if ( key==L1( "period" ) ) return PHIPeriodContainerItem::PeriodContainer;
    if ( key==L1( "registeruser" ) ) return PHIRegisterUserContainerItem::RegisterUserContainer;
    return 0;
}

#endif // PHILAYOUTS_H
