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
#include <QLabel>
#include "phitextitems.h"
#include "phidatasources.h"
#include "phibasepage.h"
#include "phirequest.h"
#include "phi.h"

void PHILabelItem::initWidget()
{
    setWidget( new QLabel() );
    setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed ) );
}

void PHILabelItem::ideInit()
{
    textData()->setText( L1( "Label" ) );
    setColor( PHIPalette::WidgetBase, PHIPalette::Window, page()->phiPalette().color( PHIPalette::Window ) );
    setColor( PHIPalette::WidgetText, PHIPalette::WindowText, page()->phiPalette().color( PHIPalette::WindowText ) );
}

void PHILabelItem::setWidgetText( const QString &t )
{
    QLabel *l=qobject_cast<QLabel*>(widget());
    l->setText( t );
}

void PHILabelItem::setWidgetAligment( Qt::Alignment align )
{
    QLabel *l=qobject_cast<QLabel*>(widget());
    Q_ASSERT( l );
    l->setAlignment( align );
}

void PHILabelItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    out+=indent+BL( "<div" );
    htmlBase( req, out, script );
    if ( Q_LIKELY( req->agentFeatures() & PHIRequest::RGBA ) ) {
        if ( colorRole( PHIPalette::WidgetBase )!=PHIPalette::Window )
            out+=BL( "background-color:" )+cssRgba( realBackgroundColor() )+';';
        if ( colorRole( PHIPalette::WidgetText )!=PHIPalette::Text )
            out+=BL( "color:" )+cssRgba( realColor() )+';';
    }
    out+=BL( "\"><table><tr><td style=\"" );
    Qt::Alignment a=static_cast<Qt::Alignment>(realAlignment());
    if ( data( DFont ).isValid() ) {
        QFont f=data( DFont ).value<QFont>();
        if ( f.underline() ) {
            out+=BL( "text-decoration:underline;" );
        }
    }
    if ( a & Qt::AlignJustify ) out+=BL( "vertical-align:top;" );
    else out+=BL( "vertical-align:middle;" );
    if ( a & Qt::AlignHCenter ) out+=BL( "text-align:center;" );
    else if ( a & Qt::AlignRight ) out+=BL( "text-align:right;" );
    out+=BL( "\">" )+data( DText ).toByteArray().replace( '\n', BL( "<br>" ) )
        +BL( "</td></tr></table></div>\n" );
}

void PHILabelItem::cssStatic( const PHIRequest *req, QByteArray &out ) const
{
    Q_UNUSED( req )
    out+=BL( "overflow:hidden;white-space:nowrap;" );
    // fallback:
    if ( colorRole( PHIPalette::WidgetText )!=PHIPalette::WindowText ) out+=BL( "color:" )+realColor().name().toLatin1()+';';
    if ( colorRole( PHIPalette::WidgetBase )!=PHIPalette::Window ) out+=BL( "background-color:" )+realBackgroundColor().name().toLatin1()+';';
}

void PHILabelItem::cssGraphicEffect( const PHIRequest *req, QByteArray &out, QByteArray &script ) const
{
    if ( effect()->graphicsType()==PHIEffect::GTShadow && !(req->agentFeatures() & PHIRequest::IE678) ) {
        QColor c;
        qreal radius, xoff, yoff;
        effect()->shadow( c, xoff, yoff, radius );
        QByteArray col=cssRgba( c );
        if ( !(req->agentFeatures() & PHIRequest::RGBA) ) col=c.name().toLatin1();
        if ( colorRole( PHIPalette::WidgetBase )==PHIPalette::Window ) {
            out+=BL( "text-shadow:" )+QByteArray::number( qRound(xoff) )
                +BL( "px " )+QByteArray::number( qRound(yoff) )+BL( "px " )
                +QByteArray::number( qRound(radius) )+BL( "px " )+col+';';
        } else {
            QByteArray prefix=req->agentPrefix();
            if ( req->agentEngine()==PHIRequest::Gecko && req->engineMajorVersion()>1 ) prefix=QByteArray();
            else if ( req->agentEngine()==PHIRequest::WebKit && req->engineMajorVersion()>534 ) prefix=QByteArray();
            else if ( req->agentEngine()==PHIRequest::Presto ) prefix=QByteArray();
            out+=prefix+BL( "box-shadow:" )+QByteArray::number( qRound(xoff) )+"px ";
            out+=QByteArray::number( qRound(yoff) )+"px ";
            out+=QByteArray::number( qRound(radius) )+"px "+col+';';
        }
    } else PHIAbstractTextItem::cssGraphicEffect( req, out, script );
}

void PHILabelItem::setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col )
{
    if ( ir==PHIPalette::WidgetText ) {
        setData( DColor, col );
        setColorRole( cr );
    } else if ( ir==PHIPalette::WidgetBase ) {
        setData( DBackgroundColor, col );
        setBackgroundColorRole( cr );
    } else return;
    QWidget *w=widget();
    if ( !w ) return;
    QPalette::ColorRole role=QPalette::WindowText;
    if ( ir==PHIPalette::WidgetBase ) role=QPalette::Window;
    QPalette pal=w->palette();
    pal.setColor( role, col );
    w->setPalette( pal );
}
