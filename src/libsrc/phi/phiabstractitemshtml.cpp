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
    if ( static_cast<Qt::BrushStyle>(realPattern())==Qt::SolidPattern ) {
        if ( Q_UNLIKELY( !( req->agentFeatures() & PHIRequest::RGBA ) ) ) needImage=true;
        else out+=BL( "background-color:" )+cssRgba( realColor() )+';';
    }
    if ( Q_UNLIKELY( realLine()>3 ) ) needImage=true;
    else if ( Q_UNLIKELY( hasBorderRadius() && !(req->agentFeatures() & PHIRequest::BorderRadius) ) ) needImage=true;
    // else if ( Q_UNLIKELY( req->agentFeatures() & PHIRequest::IE678 ) ) needImage=true;
    else if ( realPattern()==15 ) {
        if ( Q_UNLIKELY( !(req->agentFeatures() & PHIRequest::Gradients) ) ) needImage=true;
        else if ( data( DGradientType, 3 ).toInt()!=0 ) needImage=true;
        else cssLinearGradient( req, out );
    }
    if ( Q_LIKELY( !needImage ) ) {
        if ( realLine()>0 ) { // border
            QByteArray style=BL( "solid" );
            if ( realLine()==2 ) style=BL( "dashed" );
            else if ( realLine()==3 ) style=BL( "dotted" );
            out+=BL( "border:" )+QByteArray::number( qRound(realPenWidth()) )+BL( "px " )+style+' '+cssRgba( realOutlineColor() )+';';
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
    } else {
        out+=BL( "\">\n" );
        QByteArray imgId=imagePath();
        if ( Q_UNLIKELY( req->agentFeatures() & PHIRequest::IE678 ) ) {
            cssImageIEFilter( imgId );
            out+=indent+BL( "\t<div id=\"phibgi_" )+_id+BL( "\" style=\"" );
            out+=data( DIEFilter ).toByteArray();
            out+=BL( "\"></div>\n" );
        } else {
            out+=indent+BL( "\t<img id=\"phibgi_" )+_id+BL( "\" src=\"phi.phis?i=" )+imgId+BL( "&t=1\">\n" );
        }
    }
    if ( realLine()>0 ) htmlAdjustedPos( script );
    // generate child items:
    PHIBaseItem *it;
    foreach( it, _children ) it->html( req, out, script, indent+'\t' );
    out+=indent+BL( "</div>\n" );
}
