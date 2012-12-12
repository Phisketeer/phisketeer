/*
#    Copyright (C) 2010-2012  Marius B. Schumacher
#    Copyright (C) 2011-2012  Phisys AG, Switzerland
#    Copyright (C) 2012  Phisketeer.org team
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
#include "phishtmltrident5.h"

PHISHtmlTrident5::PHISHtmlTrident5( const PHISRequest *req, const PHIBasePage *page )
    : PHISHtml5Base( req, page )
{
}

PHISHtmlTrident5::~PHISHtmlTrident5()
{
}

QByteArray PHISHtmlTrident5::phisEngine() const
{
    static QByteArray arr="Phis "+PHIS::libVersion().toAscii()+"/Trident 5";
    return arr;
}

QByteArray PHISHtmlTrident5::phiObject( const QUrl &url, const QByteArray &codebase ) const
{
    QByteArray arr="<div class=\"phicontent\" id=\"phiobjectdiv\" style=\"z-index:1;visibility:hidden;\">\n"
        "\t<object classid=\"clsid:F4463E56-F3CA-45E4-93FE-B8F007376088\" id=\"phiobject\" "
        "type=\"application/x-phi\" width=\""
        +QByteArray::number( _p->width()+1 )+"\" height=\""+QByteArray::number( _p->height()+1 )
        +_endtag+"\t\t<param name=\"src\" value=\""+url.toEncoded()+_endtag
        +"\t\t<param name=\"codebase\" value=\""+codebase+_endtag+"\t</object>\n</div>\n";
    return arr;
}

/*
QByteArray PHISHtmlTrident5::phiObject( const QUrl &url, const QByteArray &codebase ) const
{
    QByteArray arr="<div id=\"phiobjectdiv\" style=\"z-index:1;visibility:hidden;\">\n"
        "\t<embed id=\"phiobject\" type=\"application/x-phi\" width=\""
        +QByteArray::number( _p->width() )+"\" height=\""+QByteArray::number( _p->height() )
        +"\" src=\""+url.toEncoded()+_endtag
        +"\t</embed>\n</div>\n";
    return arr;
}

QByteArray PHISHtmlTrident5::postProcessPhiObjectJS() const
{
    static QByteArray arr=
        "var phiobject=phi.getElementById('phiobject');\n"
        "function phiobject::titleChanged(t){document.title=t;};\n"
        "function phiobject::linkHovered(l){window.status=l;};\n"
        "function phiobject::contentChanged(c){"
        "phi.getElementById('phiobjectdiv').style.visibility='visible';"
        "phi.getElementById('phihtml').style.display='none';}\n"
        "phiobject.contentChanged=function(c){alert(c);};\n";
    return arr;
        "var phiobject=phi.getElementById('phiobject');\n"
        "function phiobject::titleChanged(t){document.title=t;};\n"
        "function phiobject::linkHovered(l){window.status=l;};\n"
}
*/

QByteArray PHISHtmlTrident5::postProcessPhiObjectJS() const
{
    static QByteArray arr=
        "var phiobject=phi.getElementById('phiobject');\n"
        "phiobject.titleChanged=function(t){document.title=t;};\n"
        "phiobject.linkHovered=function(l){window.status=l;};\n"
        "if(window.ActiveXObject){\n"
        "\tvar c=null;\n"
        "\ttry{c=new ActiveXObject('npphiplugin1.PhiPlugIn');}catch(e){;}\n"
        "\tif(c){"
        "phi.getElementById('phiobjectdiv').style.visibility='visible';"
        "phi.getElementById('phihtml').style.display='none';}\n"
        "}\n";
    return arr;
}

QByteArray PHISHtmlTrident5::phiObjectExclusive( const QUrl &url, const QByteArray &codebase ) const
{
    QByteArray arr="<div class=\"phicontent\" id=\"phiobjectdiv\">\n"
        "\t<object classid=\"clsid:F4463E56-F3CA-45E4-93FE-B8F007376088\" id=\"phiobject\""
        " type=\"application/x-phi\" width=\""
        +QByteArray::number( _p->width() )+"\" height=\""+QByteArray::number( _p->height() )
        +_endtag+"\t\t<param name=\"src\" value=\""+url.toEncoded()+_endtag
        +"\t\t<param name=\"codebase\" value=\""+codebase+_endtag+"\t</object>\n</div>\n";
    return arr;
}

QByteArray PHISHtmlTrident5::postProcessPhiObjectExclusiveJS() const
{
    static QByteArray arr="var phiobject=phi.getElementById('phiobject');\n"
        "phiobject.titleChanged=function(t){document.title=t;};\n"
        "phiobject.linkHovered=function(l){window.status=l;};\n";
    return arr;
}

bool PHISHtmlTrident5::useBoxShadow() const
{
    switch ( _it->wid() ) {
    case PHI::RECT:
    case PHI::ROUNDED_RECT:
    case PHI::SVG:
    case PHI::YOUTUBE:
    case PHI::GOOGLE_STATIC_MAP:
    case PHI::HTML_DOC: return true;
    default:;
    }
    return false;
}

/*
QByteArray PHISHtmlTrident5::positionStyle( const QRect &off ) const
{
    QByteArray arr, filter;
    arr.reserve( 300 );
    filter.reserve( 200 );
    filter+=" filter:";
    QRectF r( 0, 0, _it->width(), _it->height() );
    QTransform t=transformation();
    if ( _it->hasGraphicEffect() ) {
        if ( _it->effect()->graphicsType()==PHIEffect::GTShadow ) {
            if ( useBoxShadow() && t.isIdentity() ) {
                QColor c;
                qreal xOff, yOff, radius;
                _it->effect()->shadow( c, xOff, yOff, radius );
                arr+=boxShadowKeyword()+':'+QByteArray::number( static_cast<int>(xOff) )+"px ";
                arr+=QByteArray::number( static_cast<int>(yOff) )+"px ";
                arr+=QByteArray::number( static_cast<int>(radius) )+"px rgba(";
                arr+=QByteArray::number( c.red() )+','+QByteArray::number( c.green() )+','
                    +QByteArray::number( c.blue() )+','+QByteArray::number( c.alphaF(), 'f', 3 )+");";
            } else {
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
        }
    }
    if ( !t.isIdentity() ) {
        r=t.mapRect( r );
        filter+="progid:DXImageTransform.Microsoft.Matrix(M11="+QByteArray::number( t.m11(), 'f' )
             +",M12="+QByteArray::number( t.m21(), 'f' )+",M21="+QByteArray::number( t.m12(), 'f' )
             +",M22="+QByteArray::number( t.m22(), 'f' );
        // Qt has row major ordering, MS has col major ordering so we swap m21 with m12
        filter+=",sizingMethod='auto expand')";
    }
    arr+="position:absolute;left:";
    arr+=QByteArray::number( static_cast<int>(_it->x()+r.x())+off.x() )+"px;top:"
        +QByteArray::number( static_cast<int>(_it->y()+r.y())+off.y() )+"px;z-index:"
        +QByteArray::number( static_cast<int>(_it->zValue()) )+";margin:0px;";
    if ( static_cast<int>(_it->height())+off.height()>0 )
        arr+="height:"+QByteArray::number( static_cast<int>(_it->height())+off.height() )+"px;";
    if ( static_cast<int>(_it->width())+off.width()>1 )
        arr+="width:"+QByteArray::number( static_cast<int>(_it->width())+off.width() )+"px;";
    if ( filter.size()>8 ) arr+=filter+';';
    qDebug( "size %s %d %d", _it->id().data(), arr.capacity(), arr.size() );
    qDebug( "filter size %s %d %d", _it->id().data(), filter.capacity(), filter.size() );
    return arr;
}
*/

QByteArray PHISHtmlTrident5::effectStyle() const
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
            } else {
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
    arr+="-ms-transform-origin:0 0;";
    arr+="-ms-transform:matrix("+QByteArray::number( t.m11(), 'f' )
        +','+QByteArray::number( t.m12(), 'f' )+','+QByteArray::number( t.m21(), 'f' )
        +','+QByteArray::number( t.m22(), 'f' )+','+QByteArray::number( static_cast<int>(t.m31()) )
        +','+QByteArray::number( static_cast<int>(t.m32()) )+");";
    arr+=filter;
    return arr;
}
