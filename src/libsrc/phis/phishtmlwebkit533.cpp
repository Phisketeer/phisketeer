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
#include <QLocale>
#include "phishtmlwebkit533.h"

PHISHtmlWebKit533::PHISHtmlWebKit533( const PHISRequest *req, const PHIBasePage *page )
    : PHISHtml5Base( req, page ), _isMacOSX( req->osType()==PHISRequest::MacOS
    || req->osType()==PHISRequest::iOS ), _isSafari( _req->agentId()==PHISRequest::Safari ),
    _isiOS( req->osType()==PHISRequest::iOS )
{
}

PHISHtmlWebKit533::~PHISHtmlWebKit533()
{
}

QRect PHISHtmlWebKit533::adjustButtonSize() const
{
    //if ( _isMacOSX ) return QRect( 1, 1, -2, -2 );
    return QRect( 0, 0, 0, 0 );
}

QRect PHISHtmlWebKit533::adjustLineEditSize() const
{
    if ( _isiOS ) return QRect( 0, 0, -14, -6 );
    if ( _isMacOSX ) return QRect( 0, 0, -6, -6 );
    return QRect( 0, 0, -2, -2 );
}

QRect PHISHtmlWebKit533::adjustMultiLineEditSize() const
{
    if ( _isMacOSX ) return QRect( 0, 0, -6, -6 );
    return QRect( 0, 0, -2, -2 );
}

QRect PHISHtmlWebKit533::adjustSelectSize() const
{
    if ( _isMacOSX ) return QRect( 0, 0, 0, 0 );
    return QRect( 0, 0, 0, static_cast<int>(-_it->height()) );
}

QRect PHISHtmlWebKit533::adjustFileButton() const
{
    if ( _isMacOSX ) return QRect( 2, 2, -4, -4 );
    return QRect( 0, 0, 0, static_cast<int>(-_it->height()) );
}

qint8 PHISHtmlWebKit533::adjustDateEditButtonSpace() const
{
    if ( _isMacOSX ) return static_cast<qint8>( 0 );
    return static_cast<qint8>(-4);
}

QByteArray PHISHtmlWebKit533::phisEngine() const
{
    static QByteArray arr="Phis "+PHIS::libVersion().toLatin1()+"/WebKit 533";
    return arr;
}

QByteArray PHISHtmlWebKit533::effectStyle() const
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
                arr+="-webkit-box-shadow:"+QByteArray::number( static_cast<int>(xOff) )+"px ";
                arr+=QByteArray::number( static_cast<int>(yOff) )+"px ";
                arr+=QByteArray::number( static_cast<int>(radius) )+"px ";
                arr+=c.name().toLatin1()+';';
            }
        }
    }

    QTransform t=transformation();
    if ( t.isIdentity() ) return arr;
    arr+="-webkit-transform-origin:0 0;-webkit-transform:matrix3d("
        +QByteArray::number( t.m11(), 'f' )+','
        +QByteArray::number( t.m12(), 'f' )+",0,"
        +QByteArray::number( t.m13(), 'f' )+','
        +QByteArray::number( t.m21(), 'f' )+','
        +QByteArray::number( t.m22(), 'f' )+",0,"
        +QByteArray::number( t.m23(), 'f' )+",0,0,1,0,"
        +QByteArray::number( t.m31(), 'f' )+','
        +QByteArray::number( t.m32(), 'f' )+",0,"
        +QByteArray::number( t.m33(), 'f' )+");";
    return arr;
}

void PHISHtmlWebKit533::selectBox() const
{
    QByteArray font=fontStyle();
    if ( font.isNull() && _isMacOSX ) {
        QFont f=_p->font();
        font.reserve( 200 );
        font+="font-family:'"+f.family().toUtf8();
        if ( !f.lastResortFamily().isEmpty() ) font+="','"+f.lastResortFamily().toUtf8();
        font+="';";
        if ( f.pointSize() > -1 ) font+="font-size:"+QByteArray::number( f.pointSize()+1 )+"pt;";
    }
    _out+=_indent+"<select class=\"phitext\"";
    _out+=name()+title()+disabled()+accessKey()+tabIndex()
        +startStyle( adjustSelectSize() )+effectStyle()
        +font+colorStyle()+"\">\n";
    _out+=selectOptions();
    _out+=_indent+"</select>\n";
}

void PHISHtmlWebKit533::button() const
{
    static QByteArray type=QByteArray::fromRawData( "-webkit-appearance:button;", 26 );
    _out+=_indent+"<input type=\"button\" class=\"phibuttontext\""+name()
        +title()+disabled()+value()+accessKey()+tabIndex()+onClickUrl()
        +startStyle( adjustButtonSize() )+colorStyle();
    if ( _isMacOSX ) {
        if ( _it->height()>35. ) _out+=type;
        QByteArray font=fontStyle();
        if ( font.isNull() ) {
            QFont f=_p->font();
            font.reserve( 200 );
            font+="font-family:'"+f.family().toUtf8();
            if ( !f.lastResortFamily().isEmpty() ) font+="','"+f.lastResortFamily().toUtf8();
            font+="';";
            if ( f.pointSize() > -1 ) font+="font-size:"+QByteArray::number( f.pointSize()+1 )+"pt;";
        }
        _out+=font;
    } else _out+=fontStyle();
    _out+=effectStyle()+_endtag;
}

void PHISHtmlWebKit533::imageButton() const
{
    _out+=_indent+"<button type=\"button\" class=\"phibuttontext\""
        +name()+disabled()+title()+accessKey()+tabIndex()
        +onClickUrl()+startStyle( adjustButtonSize() )+effectStyle();
    _out+="padding:0px;-webkit-appearance:button;\">\n";
    QByteArray url;
    if ( _it->imageId().startsWith( QLatin1String( "phi" ) ) )
        url="/phi.phis?phiimg="+_it->imageIdData()+"&amp;phitmp=1";
    else url="/phi.phis?phiimg="+_it->imageIdData();
    QByteArray style=" style=\"position:relative;vertical-align:middle;";
    _out+='\t';
    imageSource( url, style, _it->id()+"_phiimg", _it->toolTipData() );
    _out+=_indent+"\t<span style=\""+colorStyle()+fontStyle()
        +"\">"+_it->valueData()+"</span>\n";
    _out+=_indent+"</button>\n";
}

void PHISHtmlWebKit533::dateEdit() const
{
    return PHISHtmlGeneric::dateEdit();
    /*
    QByteArray arr;
    arr.reserve( 1000 );
    QDate now, start, end;
    int i;
    QStringList dates=_it->value().split( ':', QString::SkipEmptyParts );
    if ( dates.count()>0 ) now=QDate::fromString( dates[0], PHI::isoDateFormat() );
    if ( !now.isValid() ) now=QDate::currentDate();
    if ( dates.count()>1 ) start=QDate::fromString( dates[1], PHI::isoDateFormat() );
    if ( !start.isValid() ) start=now;
    if ( dates.count()>2 ) end=QDate::fromString( dates[2], PHI::isoDateFormat() );
    if ( !end.isValid() ) end=QDate( 9999, 12, 31 );

    QLocale locale( _lang.left( 2 ) );
    QString monthnames, daynames;
    monthnames.reserve( 200 );
    daynames.reserve( 200 );

    for( i=1; i<13; i++ ) monthnames+=locale.monthName( i, QLocale::LongFormat )+':';
    monthnames.chop( 1 );
    for ( i=1; i<8; i++ ) daynames+=locale.dayName( i, QLocale::ShortFormat )+':';
    daynames.chop( 1 );

    QRect r=adjustLineEditSize();
    qint16 lwidth=static_cast<qint16>(_it->width())+r.width()-30+adjustDateEditButtonSpace();
    qint16 lheight=static_cast<qint16>(_it->height())+r.height();
    qint16 ltop=r.y();
    qint16 lleft=r.x();
    r=adjustButtonSize();
    qint16 btop=r.y();
    qint16 bleft=r.x()+lwidth+6;
    qint16 bheight=static_cast<qint16>(_it->height())+r.height();
    qint16 bwidth=30+r.width();
    arr+=_indent+"<div "+id()+title()+startStyle()+effectStyle()+"\">\n";
    arr+=_indent+"\t<div id=\""+_it->id()+"_phidiv\" style=\"position:absolute;left:1px; "
        "top:"+QByteArray::number( _it->height()+3 )+"px;width:200px;height:160px;"
        "overflow:visible;display:none;font-size:10pt;\"></div>\n";
    arr+=_indent+"\t<input type=\"text\" id=\""+_it->id()+"_phical\" readonly=\"readonly\" name=\""
        +_it->id()+"\" value=\""+now.toString( PHI::isoDateFormat() ).toLatin1()
        +"\" style=\"position:absolute;left:"+QByteArray::number( lleft )+"px;top:"
        +QByteArray::number( ltop )+"px;width:"+QByteArray::number( lwidth )+"px;height:"
        +QByteArray::number( lheight )+"px;margin:0px;"+fontStyle()+_endtag;
    arr+=_indent+"\t<input type=\"button\" id=\""+_it->id()+"_phibut\""+tabIndex()+accessKey()
        +" value=\"&equiv;\" style=\"position:absolute;left:"+QByteArray::number( bleft )
        +"px;top:"+QByteArray::number( btop )+"px;width:"+QByteArray::number( bwidth )+"px;height:"
        +QByteArray::number( bheight )+"px;-webkit-appearance:button;margin:0px;"+fontStyle()
        +"\" onclick=\"new phiCalendar().init('"+_it->id()+"_phidiv','"
        +_it->id()+"_phical','close','"+daynames.toUtf8()+"','"+monthnames.toUtf8()
        +"',window.document.getElementById('"+_it->id()+"_phical').value,'"
        +start.toString( PHI::isoDateFormat() ).toLatin1()+"','"
        +end.toString( PHI::isoDateFormat() ).toLatin1()+"');"+_endtag;
    arr+=_indent+"</div>\n";
        //"window.document.getElementById( '"+it->id()+"_phidiv' ).style.display="
        //"'block';\" />\n";
    qDebug( "size %s %d %d", _it->id().data(), arr.capacity(), arr.size() );
    return arr;
    */
}
