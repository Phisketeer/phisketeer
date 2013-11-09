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

void PHIAbstractShapeItem::html( const PHIRequest *req, QByteArray &out, QByteArray &jquery, const QByteArray &indent ) const
{
    QTransform t;
    bool needCalc=false, cssTrans=true;
    if ( hasGraphicEffect() ) needCalc=true;
    if ( hasTransformation() ) {
        t=computeTransformation();
        if ( !t.isAffine() ) {
            if ( Q_UNLIKELY( !req->agentFeatures() & PHIRequest::Transform3D ) ) {
                needCalc=true;
                cssTrans=false;
            }
        } else if ( Q_UNLIKELY( !req->agentFeatures() & PHIRequest::Transform2D ) ) {
            needCalc=true;
            cssTrans=false;
        }
    }
    if ( Q_UNLIKELY( req->agentFeatures() & PHIRequest::IE678 ) ) {
        if ( needCalc ) {
            QRectF br=boundingRect();
            imageIEFilterCSS( PHIDataParser::createTransformedImageId( req, this, 0, br ) );
            setAdjustedRect( br );
        } else imageIEFilterCSS( imagePath() );
        out+=indent+BL( "<div" );
        startCSS( req, out, jquery, t.isAffine() );
        out+=BL( "\"></div>\n" );
    } else {
        QByteArray imgId;
        if ( needCalc ) {
            QRectF br=boundingRect();
            imgId=PHIDataParser::createTransformedImageId( req, this, 0, br );
            setAdjustedRect( br );
        } else imgId=imagePath();
        out+=indent+BL( "<img" );
        startCSS( req, out, jquery, cssTrans );
        out+=BL( "\" src=\"phi.phis?i=" )+imgId+BL( ".png&amp;t=1\">\n" );
    }
    if ( needCalc || !cssTrans ) genAdjustedPos( jquery );
    if ( hasGraphicEffect() ) genAdjustedSize( jquery );
}

void PHIAbstractLayoutItem::html( const PHIRequest *req, QByteArray &out, QByteArray &jquery, const QByteArray &indent ) const
{
    bool needImage=false;
    out+=indent+BL( "<div" );
    startCSS( req, out, jquery );
    if ( static_cast<Qt::BrushStyle>(realPattern())==Qt::SolidPattern ) {
        if ( Q_UNLIKELY( !( req->agentFeatures() & PHIRequest::RGBA ) ) ) needImage=true;
        else out+=BL( "background-color:" )+rgba( realColor() )+';';
    }
    if ( Q_UNLIKELY( realLine()>3 ) ) needImage=true;
    else if ( Q_UNLIKELY( hasBorderRadius() && !(req->agentFeatures() & PHIRequest::BorderRadius) ) ) needImage=true;
    // else if ( Q_UNLIKELY( req->agentFeatures() & PHIRequest::IE678 ) ) needImage=true;
    else if ( realPattern()==15 ) {
        if ( Q_UNLIKELY( !(req->agentFeatures() & PHIRequest::Gradients) ) ) needImage=true;
        else if ( data( DGradientType, 3 ).toInt()!=0 ) needImage=true;
        else genLinearGradient( req, out );
    }
    if ( Q_LIKELY( !needImage ) ) {
        if ( realLine()>0 && realPenWidth()>0 ) { // border
            if ( Q_UNLIKELY( !( req->agentFeatures() & PHIRequest::RGBA ) ) ) needImage=true;
            else {
                QByteArray style=BL( "solid" );
                if ( realLine()==2 ) style=BL( "dashed" );
                else if ( realLine()==3 ) style=BL( "dotted" );
                out+=BL( "border:" )+QByteArray::number( qRound(realPenWidth()) )+BL( "px " )+style+' '+rgba( realOutlineColor() )+';';
                if ( hasBorderRadius() ) {
                    QByteArray prefix=QByteArray();
                    if ( req->agentEngine()==PHIRequest::WebKit && req->engineMajorVersion()<534 ) prefix=req->agentPrefix();
                    else if ( req->agentEngine()==PHIRequest::Gecko && req->engineMajorVersion()<2 ) prefix=req->agentPrefix();
                    out+=prefix+BL( "border-radius:" )+QByteArray::number( qRound(topLeftRadius()) )+BL( "px " )
                        +QByteArray::number( qRound(topRightRadius()) )+BL( "px " )
                        +QByteArray::number( qRound(bottomRightRadius()) )+BL( "px " )
                        +QByteArray::number( qRound(bottomLeftRadius()) )+BL( "px;" );
                }
            }
        }
        out+=BL( "\">\n" );
    } else {
        out+=BL( "\">\n" );
        QByteArray imgId=imagePath();
        if ( Q_UNLIKELY( req->agentFeatures() & PHIRequest::IE678 ) ) {
            imageIEFilterCSS( imgId );
            out+=indent+BL( "\t<div id=\"phibgi_" )+_id+BL( "\" style=\"" );
            out+=data( DIEFilter ).toByteArray();
            out+=BL( "\"></div>\n" );
        } else {
            out+=indent+BL( "\t<img id=\"phibgi_" )+_id+BL( "\" src=\"" )+imgId+BL( "\">\n" );
        }
    }
    PHIBaseItem *it;
    foreach( it, _children ) it->html( req, out, jquery, indent+'\t' );
    out+=indent+BL( "</div>\n" );
    if ( realLine()>0 ) genAdjustedPos( jquery );
}
