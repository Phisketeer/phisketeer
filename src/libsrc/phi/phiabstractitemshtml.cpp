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
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "phiabstractitems.h"
#include "phirequest.h"
#include "phidataparser.h"

void PHIAbstractShapeItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    htmlImg( req, out, script, indent );
}

void PHIAbstractImageItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    htmlImg( req, out, script, indent );
}

void PHIAbstractImageItem::cssGraphicEffect( const PHIRequest *req, QByteArray &out, QByteArray &script ) const
{
    Q_UNUSED( req )
    Q_UNUSED( out )
    Q_UNUSED( script )
}

void PHIAbstractImageBookItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    htmlImages( req, out, script, indent );
}

void PHIAbstractImageBookItem::cssGraphicEffect( const PHIRequest *req, QByteArray &out, QByteArray &script ) const
{
    Q_UNUSED( req )
    Q_UNUSED( out )
    Q_UNUSED( script )
}

void PHIAbstractShapeItem::cssGraphicEffect( const PHIRequest *req, QByteArray &out, QByteArray &script ) const
{
    Q_UNUSED( req )
    Q_UNUSED( out )
    Q_UNUSED( script )
}

void PHIAbstractLayoutItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    bool needImage=false;
    out+=indent+BL( "<div" );
    htmlBase( req, out, script, true );
    if ( Q_UNLIKELY( realLine()>3 ) ) needImage=true;
    else if ( Q_UNLIKELY( hasBorderRadius() && !(req->agentFeatures() & PHIRequest::BorderRadius) ) ) needImage=true;
    // else if ( Q_UNLIKELY( req->agentFeatures() & PHIRequest::IE678 ) ) needImage=true;
    else if ( realPattern()==15 ) {
        if ( cssGradientCreateable( req ) ) cssLinearGradient( req, out );
        else needImage=true;
    } else if ( realPattern()>1 ) needImage=true;
    if ( Q_LIKELY( !needImage ) ) {
        if ( realLine()>0 ) { // border
            QByteArray style=BL( "solid" );
            if ( realLine()==2 ) style=BL( "dashed" );
            else if ( realLine()==3 ) style=BL( "dotted" );
            out+=BL( "border:" )+QByteArray::number( qRound(realPenWidth()) )+BL( "px " )+style+';';
            if ( hasBorderRadius() ) {
                QByteArray prefix=QByteArray();
                if ( req->agentEngine()==PHIRequest::WebKit && req->engineMajorVersion()<534 ) prefix=req->agentPrefix();
                else if ( req->agentEngine()==PHIRequest::Gecko && req->engineMajorVersion()<2 ) prefix=req->agentPrefix();
                out+=prefix+BL( "border-radius:" )+QByteArray::number( qRound(topLeftRadius()) )+BL( "px " )
                    +QByteArray::number( qRound(topRightRadius()) )+BL( "px " )
                    +QByteArray::number( qRound(bottomRightRadius()) )+BL( "px " )
                    +QByteArray::number( qRound(bottomLeftRadius()) )+BL( "px;" );
            }
            QPointF off=boundingRect().topLeft();
            if ( hasTransformation() ) off=computeTransformation( false ).map( off );
            setAdjustedRect( QRectF( off, realSize() ) );
        }
        out+=BL( "\">\n" );
        htmlInitItem( script, false );
        if ( colorRole( PHIPalette::Foreground )!=PHIPalette::Window ) script+=BL( ".color('" )+cssColor( realColor() )+BL( "')" );
        if ( realLine()>0 ) script+=BL( ".borderColor('" )+cssColor( realOutlineColor() )+BL( "')" );
        script+=BL( ";\n" );
    } else { // create an image
        out+=BL( "\">\n" );
        QByteArray imgId=imagePath();
        if ( Q_UNLIKELY( req->agentFeatures() & PHIRequest::IE678 ) ) {
            cssImageIEFilter( imgId );
            out+=indent+BL( "\t<div id=\"phibgi_" )+_id+BL( "\" style=\"" );
            out+=data( DIEFilter ).toByteArray();
            out+=BL( "\"></div>\n" );
        } else {
            out+=indent+BL( "\t<img alt=\"\" id=\"phibgi_" )+_id+BL( "\" src=\"phi.phis?i=" )+imgId+BL( "&amp;t=1\">\n" );
        }
        htmlInitItem( script );
    }
    // generate child items:
    PHIBaseItem *it;
    foreach( it, _children ) it->html( req, out, script, indent+'\t' );
    out+=indent+BL( "</div>\n" );
}

void PHIAbstractExternalItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    if ( Q_LIKELY( req->agentFeatures() & PHIRequest::HTML5 || req->agentFeatures() & PHIRequest::IE678 ) ) {
        out+=indent+BL( "<iframe width=\"" )+QByteArray::number( qRound(realWidth()) )
            +BL( "\" height=\"" )+QByteArray::number( qRound(realHeight()) )+'"';
        htmlBase( req, out, script );
        out+=BL( "\" src=\"" )+source()+data( DText ).toByteArray()
            +BL( "\"></iframe>\n" );
    } else {
        out+=indent+BL( "<object" );
        htmlBase( req, out, script );
        out+=BL( "\" data=\"" )+source()+data( DText ).toByteArray()
            +BL( "\" type=\"text/html\"></object>\n" );
    }
    htmlInitItem( script );
}
