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
#include "phishtmltrident6.h"

PHISHtmlTrident6::PHISHtmlTrident6( const PHISRequest *req, const PHIBasePage *page )
    : PHISHtmlTrident5( req, page )
{
}

PHISHtmlTrident6::~PHISHtmlTrident6()
{
}

QByteArray PHISHtmlTrident6::phisEngine() const
{
    static QByteArray arr="Phis "+PHIS::libVersion().toLatin1()+"/Trident 6";
    return arr;
}

QByteArray PHISHtmlTrident6::effectStyle() const
{
    QByteArray arr, filter;
    arr.reserve( 300 );
    QTransform t=transformation();
    if ( _it->hasGraphicEffect() && t.isIdentity() ) {
        filter.reserve( 200 );
        filter+=" filter:";
        QRectF r( 0, 0, _it->width(), _it->height() );
        if ( _it->effect()->graphicsType()==PHIEffect::GTShadow ) {
            if ( useBoxShadow() ) {
                QColor c;
                qreal xOff, yOff, radius;
                _it->effect()->shadow( c, xOff, yOff, radius );
                filter.clear();
                filter+="box-shadow:"+QByteArray::number( static_cast<int>(xOff) )+"px ";
                filter+=QByteArray::number( static_cast<int>(yOff) )+"px ";
                filter+=QByteArray::number( static_cast<int>(radius) )+"px rgba(";
                filter+=QByteArray::number( c.red() )+','+QByteArray::number( c.green() )+','
                    +QByteArray::number( c.blue() )+','+QByteArray::number( c.alphaF(), 'f', 3 )+");";
            } else { /** @todo Change: Trident 6 does not support DX transformations anymore in standard mode. */
                QColor c;
                qreal xOff, yOff, radius;
                _it->effect()->shadow( c, xOff, yOff, radius );
                filter+="progid:DXImageTransform.Microsoft.DropShadow(Color='#";
                filter+=QByteArray::number( c.rgba(), 16 )+"',OffX="+QByteArray::number( xOff );
                filter+=",OffY="+QByteArray::number( yOff )+")";
                if ( xOff<0 ) r.adjust( xOff, 0, 0, 0 );
                if ( yOff<0 ) r.adjust( 0, yOff, 0, 0 );
            }
        } else if ( _it->effect()->graphicsType()==PHIEffect::GTBlur ) {
            qreal radius;
            _it->effect()->blur( radius );
            filter+="progid:DXImageTransform.Microsoft.Blur(PixelRadius=";
            filter+=QByteArray::number( radius )+')';
            r.adjust( -radius, -radius, 0, 0 );
        } else return arr;
        if ( !filter.endsWith( ';' ) ) filter+=';';
        filter+="-ms-transform:translate("+QByteArray::number( static_cast<int>(r.x()) )+"px,"
              +QByteArray::number( static_cast<int>(r.y()) )+"px);";
        return filter;
    }
    if ( t.isIdentity() ) return arr;
    arr+="-ms-transform-origin:0 0;-ms-transform:matrix3d("
        +QByteArray::number( t.m11(), 'f' )+','
        +QByteArray::number( t.m12(), 'f' )+",0,"
        +QByteArray::number( t.m13(), 'f' )+','
        +QByteArray::number( t.m21(), 'f' )+','
        +QByteArray::number( t.m22(), 'f' )+",0,"
        +QByteArray::number( t.m23(), 'f' )+",0,0,1,0,"
        +QByteArray::number( t.m31(), 'f' )+','
        +QByteArray::number( t.m32(), 'f' )+",0,"
        +QByteArray::number( t.m33(), 'f' )+");";
    arr+=filter;
    return arr;
}
