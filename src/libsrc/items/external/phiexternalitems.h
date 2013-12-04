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
#ifndef PHIEXTERNALITEMS_H
#define PHIEXTERNALITEMS_H
#include "phiitemplugin.h"
#include "phigoogleitems.h"
#include "phihtmlitems.h"
#include "phi.h"

class PHIExternalItems : public PHIItemPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.phisketeer.phi.items.external" FILE "phiexternal.json")
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
    virtual QString category() const { return tr( "External" ); }
};

inline PHIBaseItem* PHIExternalItems::create( PHIWID wid, const PHIBaseItemPrivate &p ) const
{
    switch ( wid ) {
    case PHIFacebookItem::Facebook: return new PHIFacebookItem( p );
    case PHIGoogleStaticMapItem::GoogleStaticMap: return new PHIGoogleStaticMapItem( p );
    case PHIGooglePlusItem::GooglePlus: return new PHIGooglePlusItem( p );
    case PHIYouTubeItem::YouTube: return new PHIYouTubeItem( p );
    case PHIHtmlDocItem::HtmlDoc: return new PHIHtmlDocItem( p );
    case PHITwitterItem::Twitter: return new PHITwitterItem( p );
    case PHIGoogleMapsItem::GoogleMaps: return new PHIGoogleMapsItem( p );
    case PHIGoogleCalendarItem::GoogleCalendar: return new PHIGoogleCalendarItem( p );
    }
    return 0;
}

inline PHIBaseItem* PHIExternalItems::copy( const PHIBaseItem *it ) const
{
    switch ( it->wid() ) {
    case PHIFacebookItem::Facebook: return new PHIFacebookItem( *qobject_cast<const PHIFacebookItem*>(it) );
    case PHIGoogleStaticMapItem::GoogleStaticMap: return new PHIGoogleStaticMapItem( *qobject_cast<const PHIGoogleStaticMapItem*>(it) );
    case PHIGooglePlusItem::GooglePlus: return new PHIGooglePlusItem( *qobject_cast<const PHIGooglePlusItem*>(it) );
    case PHIYouTubeItem::YouTube: return new PHIYouTubeItem( *qobject_cast<const PHIYouTubeItem*>(it) );
    case PHIHtmlDocItem::HtmlDoc: return new PHIHtmlDocItem( *qobject_cast<const PHIHtmlDocItem*>(it) );
    case PHITwitterItem::Twitter: return new PHITwitterItem( *qobject_cast<const PHITwitterItem*>(it) );
    case PHIGoogleMapsItem::GoogleMaps: return new PHIGoogleMapsItem( *qobject_cast<const PHIGoogleMapsItem*>(it) );
    case PHIGoogleCalendarItem::GoogleCalendar: return new PHIGoogleCalendarItem( *qobject_cast<const PHIGoogleCalendarItem*>(it) );
    }
    return 0;
}

inline QStringList PHIExternalItems::keys() const
{
    return QStringList() << SL( "facebook" ) << SL( "staticmap" ) << SL( "googleplus" ) << SL( "youtube" )
        << SL( "htmldoc" ) << SL( "twitter" ) << SL( "googlemaps" ) << SL( "googlecal" );
}

inline PHIWID PHIExternalItems::wid( const QString &key ) const
{
    if ( key==L1( "facebook" ) ) return PHIFacebookItem::Facebook;
    if ( key==L1( "staticmap" ) ) return PHIGoogleStaticMapItem::GoogleStaticMap;
    if ( key==L1( "googleplus" ) ) return PHIGooglePlusItem::GooglePlus;
    if ( key==L1( "youtube" ) ) return PHIYouTubeItem::YouTube;
    if ( key==L1( "htmldoc" ) ) return PHIHtmlDocItem::HtmlDoc;
    if ( key==L1( "twitter" ) ) return PHITwitterItem::Twitter;
    if ( key==L1( "googlemaps" ) ) return PHIGoogleMapsItem::GoogleMaps;
    if ( key==L1( "googlecal" ) ) return PHIGoogleCalendarItem::GoogleCalendar;
    return 0;
}

#endif // PHIEXTERNALITEMS_H
