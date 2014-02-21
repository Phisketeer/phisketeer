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
#include "phigraphictext.h"
#include "phiellipseitem.h"
#include "phirectitem.h"
#include "phiprogressitem.h"
#include "phi.h"

class PHIDisplayItems : public PHIItemPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.phisketeer.phi.items.display" FILE "phidisplay.json")
    Q_CLASSINFO( "Author", "Marius Schumacher" )
    Q_CLASSINFO( "Url", "http://www.phisketeer.org" )
    Q_CLASSINFO( "Version", "1.0" )
    Q_CLASSINFO( "License", "LGPL" )
    Q_CLASSINFO( "Copyright", "2013 Phisys AG, 2013-2014 Phisketeer Team" )

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
    case PHIRectItem::Rect: return new PHIRectItem( p );
    case PHIRoundedRectItem::RoundedRect: return new PHIRoundedRectItem( p );
    case PHIEllipseItem::Ellipse: return new PHIEllipseItem( p );
    case PHIGraphicTextItem::GraphText: return new PHIGraphicTextItem( p );
    case PHILabelItem::Label: return new PHILabelItem( p );
    case PHILinkItem::Link: return new PHILinkItem( p );
    case PHIImageItem::Image: return new PHIImageItem( p );
    case PHIRichTextItem::RichText: return new PHIRichTextItem( p );
    case PHIGraphicRichTextItem::GraphicRichText: return new PHIGraphicRichTextItem( p );
    case PHIRolloverItem::Rollover: return new PHIRolloverItem( p );
    case PHISvgItem::Svg: return new PHISvgItem( p );
    case PHISlideShowItem::SlideShow: return new PHISlideShowItem( p );
    case PHICanvasItem::Canvas: return new PHICanvasItem( p );
    case PHIProgressItem::Progress: return new PHIProgressItem( p );
    case PHISponsorItem::Sponsor: return new PHISponsorItem( p );
    }
    return 0;
}

inline PHIBaseItem* PHIDisplayItems::copy( const PHIBaseItem *it ) const
{
    switch ( it->wid() ) {
    case PHIRectItem::Rect: return new PHIRectItem( *qobject_cast<const PHIRectItem*>(it) );
    case PHIRoundedRectItem::RoundedRect: return new PHIRoundedRectItem( *qobject_cast<const PHIRoundedRectItem*>(it) );
    case PHIEllipseItem::Ellipse: return new PHIEllipseItem( *qobject_cast<const PHIEllipseItem*>(it) );
    case PHIGraphicTextItem::GraphText: return new PHIGraphicTextItem( *qobject_cast<const PHIGraphicTextItem*>(it) );
    case PHILabelItem::Label: return new PHILabelItem( *qobject_cast<const PHILabelItem*>(it) );
    case PHILinkItem::Link: return new PHILinkItem( *qobject_cast<const PHILinkItem*>(it) );
    case PHIImageItem::Image: return new PHIImageItem( *qobject_cast<const PHIImageItem*>(it) );
    case PHIRichTextItem::RichText: return new PHIRichTextItem( *qobject_cast<const PHIRichTextItem*>(it) );
    case PHIGraphicRichTextItem::GraphicRichText: return new PHIGraphicRichTextItem( *qobject_cast<const PHIGraphicRichTextItem*>(it) );
    case PHIRolloverItem::Rollover: return new PHIRolloverItem( *qobject_cast<const PHIRolloverItem*>(it) );
    case PHISvgItem::Svg: return new PHISvgItem( *qobject_cast<const PHISvgItem*>(it) );
    case PHISlideShowItem::SlideShow: return new PHISlideShowItem( *qobject_cast<const PHISlideShowItem*>(it) );
    case PHICanvasItem::Canvas: return new PHICanvasItem( *qobject_cast<const PHICanvasItem*>(it) );
    case PHIProgressItem::Progress: return new PHIProgressItem( *qobject_cast<const PHIProgressItem*>(it) );
    case PHISponsorItem::Sponsor: return new PHISponsorItem( *qobject_cast<const PHISponsorItem*>(it) );
    }
    return 0;
}

inline QStringList PHIDisplayItems::keys() const
{
    return QStringList() << SL( "rect" ) << SL( "ellipse" ) << SL( "rollover" )
        << SL( "roundedrect" ) << SL( "graphtext" ) << SL( "richtext" )
        << SL( "label" ) << SL( "link" ) << SL( "image" )  << SL( "svg" )
        << SL( "slideshow" ) << SL( "sponsor" ) << SL( "canvas" )
        << SL( "progress" ) << SL( "graphrichtext" );
}

inline PHIWID PHIDisplayItems::wid( const QString &key ) const
{
    if ( key==L1( "rect" ) ) return PHIRectItem::Rect;
    if ( key==L1( "ellipse" ) ) return PHIEllipseItem::Ellipse;
    if ( key==L1( "roundedrect" ) ) return PHIRoundedRectItem::RoundedRect;
    if ( key==L1( "graphtext" ) ) return PHIGraphicTextItem::GraphText;
    if ( key==L1( "label" ) ) return PHILabelItem::Label;
    if ( key==L1( "link" ) ) return PHILinkItem::Link;
    if ( key==L1( "image" ) ) return PHIImageItem::Image;
    if ( key==L1( "richtext" ) ) return PHIRichTextItem::RichText;
    if ( key==L1( "graphrichtext" ) ) return PHIGraphicRichTextItem::GraphicRichText;
    if ( key==L1( "rollover" ) ) return PHIRolloverItem::Rollover;
    if ( key==L1( "svg" ) ) return PHISvgItem::Svg;
    if ( key==L1( "slideshow" ) ) return PHISlideShowItem::SlideShow;
    if ( key==L1( "canvas" ) ) return PHICanvasItem::Canvas;
    if ( key==L1( "progress" ) ) return PHIProgressItem::Progress;
    if ( key==L1( "sponsor" ) ) return PHISponsorItem::Sponsor;
    return 0;
}

#endif // PHIDISPLAYITEMS_H
