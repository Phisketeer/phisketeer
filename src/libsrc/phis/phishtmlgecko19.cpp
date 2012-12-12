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
#include "phishtmlgecko19.h"

PHISHtmlGecko19::PHISHtmlGecko19( const PHISRequest *req, const PHIBasePage *page )
    : PHISHtmlGeneric( req, page ), _isMacOSX( req->osType()==PHISRequest::MacOS ),
      _isLinux( req->osType()==PHISRequest::Linux )
{
}

PHISHtmlGecko19::~PHISHtmlGecko19()
{
}

QRect PHISHtmlGecko19::adjustButtonSize() const
{
    if ( _isMacOSX ) {
        if ( _it->wid()==PHI::DATEEDIT ) return QRect( 1, 1, -2, -2 );
        if ( _it->height()<33 ) return QRect( 1, 1, -2, -_it->height() );
        return QRect( 1, 1, -2, -2 );
    }
    return QRect( 0, 0, 0, 0 );
}

QRect PHISHtmlGecko19::adjustLineEditSize() const
{
    if ( _isMacOSX || _isLinux ) return QRect( 0, 0, -6, -8 );
    return QRect( 0, 0, -6, -6 );
}

QRect PHISHtmlGecko19::adjustMultiLineEditSize() const
{
    if ( _isMacOSX ) return QRect( 0, 0, -2, -2 );
    return QRect( 0, 0, -6, -6 );
}

QRect PHISHtmlGecko19::adjustFileButton() const
{
    if ( _isMacOSX ) return QRect( 0, 1, -1, -2 );
    return QRect( 0, 0, 0, 0 );
}

qint8 PHISHtmlGecko19::adjustDateEditButtonSpace() const
{
    //if ( _isMacOSX ) return static_cast<qint8>(0);
    return static_cast<qint8>(0);
}

QRect PHISHtmlGecko19::adjustSelectSize() const
{
    if ( _isMacOSX ) return QRect( 0, 0, 0, 0 );
    return QRect( 0, 0, 0, -_it->height() );
}

QByteArray PHISHtmlGecko19::phisEngine() const
{
    static QByteArray arr="Phis "+PHIS::libVersion().toAscii()+"/Gecko 19";
    return arr;
}

QByteArray PHISHtmlGecko19::phiObject( const QUrl &url, const QByteArray &codebase ) const
{
    if ( _isMacOSX ) return QByteArray();
    return PHISHtmlBase::phiObject( url, codebase );
}

QByteArray PHISHtmlGecko19::postProcessPhiObjectJS() const
{
    if ( _isMacOSX ) return QByteArray();
    return PHISHtmlBase::postProcessPhiObjectJS();
}

QByteArray PHISHtmlGecko19::effectStyle() const
{
    QByteArray arr;
    arr.reserve( 400 );
    if ( _it->hasGraphicEffect() ) {
        if ( _it->effect()->graphicsType()==PHIEffect::GTShadow ) {
            arr+=textShadowStyle();
            if ( arr.isEmpty() ) {
                QColor c;
                qreal xOff, yOff, radius;
                _it->effect()->shadow( c, xOff, yOff, radius );
                arr+="-moz-box-shadow:"+QByteArray::number( static_cast<int>(xOff) )+"px ";
                arr+=QByteArray::number( static_cast<int>(yOff) )+"px ";
                arr+=QByteArray::number( static_cast<int>(radius) )+"px ";
                arr+=c.name().toAscii()+';';
            }
        }
    }
    QTransform t=transformation();
    if ( t.isIdentity() ) return arr;
    arr+="-moz-transform-origin:0 0;";
    arr+="-moz-transform:matrix("+QByteArray::number( t.m11(), 'f' )
        +','+QByteArray::number( t.m12(), 'f' )+','+QByteArray::number( t.m21(), 'f' )
        +','+QByteArray::number( t.m22(), 'f' )+','+QByteArray::number( static_cast<int>(t.m31()) )
        +"px,"+QByteArray::number( static_cast<int>(t.m32()) )+"px);";
    return arr;
}

void PHISHtmlGecko19::youtubeVideo() const
{
    QByteArray val=_it->valueData();
    val.replace( "&", "&amp;" );
    val+="&amp;hl="+_lang.left( 2 )+"&amp;cc_lang_pref="+_lang.left( 2 );
    _out+=_indent+"<object type=\"application/x-shockwave-flash\" data=\"http://www.youtube-nocookie.com/v/"
       +val+"\""+id()+startStyle()+effectStyle()+"border:none;\">\n";
    _out+=_indent+"\t<param name=\"movie\" value=\"http://www.youtube-nocookie.com/v/"+val+_endtag;
    _out+=_indent+"\t<param name=\"allowFullScreen\" value=\"true"+_endtag;
    _out+=_indent+"\t<param name=\"allowscriptaccess\" value=\"always"+_endtag;
    _out+=_indent+"</object>\n";
}
