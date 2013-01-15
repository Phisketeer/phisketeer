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
#include <QLocale>
#include "phishtmlgeneric.h"
#include "phierror.h"

PHISHtmlGeneric::PHISHtmlGeneric( const PHISRequest *req, const PHIBasePage *page )
    : PHISHtmlBase( req, page )
{
    qDebug( "PHISHtmlGeneric::PHISHtmlGeneric()" );
}

PHISHtmlGeneric::~PHISHtmlGeneric()
{
    qDebug( "PHISHtmlGeneric::~PHISHtmlGeneric()" );
}

QByteArray PHISHtmlGeneric::phisEngine() const
{
    static QByteArray arr="Phis "+PHIS::libVersion().toLatin1()+"/Generic 4.01";
    return arr;
}

QByteArray PHISHtmlGeneric::effectJS() const
{
    QByteArray arr;
    if ( _it->effects()==PHIEffect::ENone ) return arr;
    arr.reserve( 200 );
    if ( _it->effects() & PHIEffect::EFadeIn ) {
        qint32 start, duration;
        quint8 ease;
        qreal maxOpac;
        _it->fadeIn( start, duration, maxOpac, ease );
        arr+="$('"+_it->id()+"').fadeIn("+QByteArray::number( start )
            +','+QByteArray::number( duration )+','+QByteArray::number( maxOpac )
            +",'"+PHI::toEasingCurveByteArray( ease )+"');\n";
    }
    if ( _it->effects() & PHIEffect::EFadeOut ) {
        qint32 start, duration;
        quint8 ease;
        qreal minOpac;
        _it->fadeOut( start, duration, minOpac, ease );
        arr+="$('"+_it->id()+"').fadeOut("+QByteArray::number( start )+","
            +QByteArray::number( duration )+','+QByteArray::number( minOpac )
            +",'"+PHI::toEasingCurveByteArray( ease )+"');\n";
    }
    if ( _it->effects() & PHIEffect::EMoveTo ) {
        qint32 start, duration, left, top;
        quint8 ease;
        _it->effect()->moveTo( start, duration, left, top, ease );
        QRect r=transformationPosition( transformation() );
        arr+="$('"+_it->id()+"').moveTo("+QByteArray::number( left+r.x() )+','
            +QByteArray::number( top+r.y() )+','+QByteArray::number( start )+','
            +QByteArray::number( duration )
            +",'"+PHI::toEasingCurveByteArray( ease )+"');\n";
    }
    if ( _it->effects() & PHIEffect::EMoveBy ) {
        qint32 start, duration, x, y, w, h;
        quint8 ease;
        _it->effect()->moveBy( start, duration, x, y, w, h, ease );
        arr+="$('"+_it->id()+"').moveBy("+QByteArray::number( x )+','
            +QByteArray::number( y )+','+QByteArray::number( w )+','
            +QByteArray::number( h )+','+QByteArray::number( start )+','
            +QByteArray::number( duration )+",'"+PHI::toEasingCurveByteArray( ease )+"');\n";
    }
    if ( _it->effects() & PHIEffect::ERotateIn ) {
        quint8 axis;
        qint32 start, duration;
        _it->effect()->rotateIn( axis, start, duration );
        arr+="$('"+_it->id()+"').rotateIn("+QByteArray::number( axis )+','
            +QByteArray::number( start )+','+QByteArray::number( duration )+");\n";
    }
    if ( _it->effects() & PHIEffect::ERotateOut ) {
        quint8 axis;
        qint32 start, duration;
        _it->effect()->rotateOut( axis, start, duration );
        arr+="$('"+_it->id()+"').rotateOut("+QByteArray::number( axis )+','
            +QByteArray::number( start )+','+QByteArray::number( duration )+");\n";
    }
    return arr;
}

QByteArray PHISHtmlGeneric::effectStyle() const
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
                arr+=boxShadowKeyword()+':'+QByteArray::number( static_cast<int>(xOff) )+"px ";
                arr+=QByteArray::number( static_cast<int>(yOff) )+"px ";
                arr+=QByteArray::number( static_cast<int>(radius) )+"px ";
                arr+=c.name().toLatin1()+';';
            }
        }
    }
    QTransform t=transformation();
    if ( t.isIdentity() ) return arr;
    arr+=transformOriginKeyword()+":0 0;";
    arr+=transformKeyword()+":matrix("+QByteArray::number( t.m11(), 'f' )
        +','+QByteArray::number( t.m12(), 'f' )+','+QByteArray::number( t.m21(), 'f' )
        +','+QByteArray::number( t.m22(), 'f' )+','+QByteArray::number( static_cast<int>(t.m31()) )
        +','+QByteArray::number( static_cast<int>(t.m32()) )+");";
    return arr;
}

void PHISHtmlGeneric::label() const
{
    _out+=_indent+"<div "+id()+startStyle()+effectStyle()+"\">\n";
    _out+=_indent+"\t<table cellspacing=\"0\" cellpadding=\"0\" width=\"100%\" "
        +"id=\""+_it->id()+"_phitbl\"><tr><td style=\""+alignmentStyle()
        +colorStyle()+outlineColorStyle()+fontStyle()+heightStyle();
    _out+="overflow:visible;white-space:nowrap;\" onmouseover=\"this.style.cursor='default';\">";
        //" onmouseout=\"this.style.cursor='auto';\">";
    _out+=_it->valueData();
    _out+="</td></tr></table>\n"+_indent+"</div>\n";
}

void PHISHtmlGeneric::richText() const
{
    //return createGraphicsImage( GTRichText );
    _out+=_indent+"<div class=\"phi_"+QByteArray::number( PHI::TEXT )+"\""
       +id()+title()+startStyle()+colorStyle()+outlineColorStyle()+fontStyle()
       +alignmentStyle()+effectStyle()+"\">\n";
    _out+=_it->valueData()+'\n';
    _out+=_indent+"</div>\n";
}

void PHISHtmlGeneric::richTextLayouted() const
{
    _out+=_indent+"<div class=\"phi_"+QByteArray::number( PHI::RICH_TEXT )+"\""
        +id()+title()+startStyle()+colorStyle()+outlineColorStyle()+fontStyle()
        +alignmentStyle()+effectStyle()+"\">\n";
    _out+=_it->valueData()+'\n';
    _out+=_indent+"</div>\n";
}

void PHISHtmlGeneric::menuLink() const
{
    _out+=_indent+"<div "+id()+title()+onClickUrl()+startStyle()+effectStyle()+"\">\n";
    _out+=_indent+"\t<table cellspacing=\"0\" cellpadding=\"0\" width=\"100%\" "
        +"id=\""+_it->id()+"_phitbl\"><tr><td style=\""+alignmentStyle()
        +colorStyle()+outlineColorStyle()+fontStyle()+heightStyle()
        +"overflow:visible;white-space:nowrap;\"";
    if ( _it->rolloverTextColor().isValid() || _it->rolloverBackgroundColor().isValid() ) {
        _out+=" onmouseover=\"this.style.cursor='default';";
        if ( _it->rolloverTextColor().isValid() )
            _out+="this.style.color='"+_it->rolloverTextColor().name().toLatin1()+"';";
        if ( _it->rolloverBackgroundColor().isValid() )
            _out+="this.style.backgroundColor='"+_it->rolloverBackgroundColor().name().toLatin1()+"';";
        _out+='"';
        QColor tc=_it->color().isValid() ? _it->color() : _p->palette().color( QPalette::WindowText );
        _out+=" onmouseout=\"this.style.color='"+tc.name().toLatin1()+"';";
        if ( _it->outlineColor().isValid() )
            _out+="this.style.backgroundColor='"+_it->outlineColor().name().toLatin1()+"';";
        _out+='"';
    }
    _out+='>'+_it->valueData();
    _out+="</td></tr></table>\n"+_indent+"</div>\n";
}

void PHISHtmlGeneric::rolloverButton() const
{
    QByteArray postfix, url1, url2;
    QTransform t=transformation();
    if ( !_it->value().isEmpty() ) postfix=QByteArray::fromRawData( "_v", 2 );
    if ( _it->hasGraphicEffect() ) postfix=QByteArray::fromRawData( "_g", 2 );
    if ( !t.isIdentity() ) postfix=QByteArray::fromRawData( "_t", 2 );
    QString imageId, first, second;
    for ( int i=0; i<2; i++ ) {
        if ( _it->pictureBookIds().count() ) { // rollover with images
            imageId=checkForImageId( postfix, i );
            if ( imageId.isNull() ) {
                if ( i<_it->pictureBookIds().count() ) {
                    imageId=_it->pictureBookIds().at( i );
                    QImage img=createImageImage( imageId );
                    if ( !_it->value().isEmpty() ) img=createRolloverTextImage( i, img );
                    if ( _it->hasGraphicEffect() ) img=graphicsEffectImage( img );
                    if ( !t.isIdentity() ) img=img.transformed( t, Qt::SmoothTransformation );
                    imageId=storeImage( img, postfix, i );
                    if ( imageId.isNull() ) return;
                } else {
                    Q_ASSERT( i==1 );
                    if ( _it->itemProperties() & PHIItem::PRolloverColors ) {
                        QImage img=createRolloverTextImage( i );
                        if ( _it->hasGraphicEffect() ) img=graphicsEffectImage( img );
                        if ( !t.isIdentity() ) img=img.transformed( t, Qt::SmoothTransformation );
                        imageId=storeImage( img, postfix, i );
                        if ( imageId.isNull() ) return;
                    }
                }
            }
        } else { // rollover with text only
            imageId=checkForImageId( postfix, i );
            if ( imageId.isNull() ) {
                if ( i==0 ) {
                    QImage img=createRolloverTextImage( i );
                    if ( _it->hasGraphicEffect() ) img=graphicsEffectImage( img );
                    if ( !t.isIdentity() ) img=img.transformed( t, Qt::SmoothTransformation );
                    imageId=storeImage( img, postfix, i );
                    if ( imageId.isNull() ) return;
                } else {
                    if ( _it->itemProperties() & PHIItem::PRolloverColors ) {
                        QImage img=createRolloverTextImage( i );
                        if ( _it->hasGraphicEffect() ) img=graphicsEffectImage( img );
                        if ( !t.isIdentity() ) img=img.transformed( t, Qt::SmoothTransformation );
                        imageId=storeImage( img, postfix, i );
                        if ( imageId.isNull() ) return;
                    }
                }
            }
        }
        if ( i==0 ) first=imageId;
        else second=imageId;
    }
    if ( first.startsWith( "phi" ) || _it->itemProperties() & PHIItem::PNoCache ) {
        url1="/phi.phis?phiimg="+first.toUtf8()+"&amp;phitmp=1";
        if ( !second.isNull() ) url2="/phi.phis?phiimg="
            +second.toUtf8()+"&amp;phitmp=1";
    } else {
        url1="/phi.phis?phiimg="+first.toUtf8();
        if ( !second.isNull() ) url2="/phi.phis?phiimg="
            +second.toUtf8();
    }
    QByteArray style=" style=\""+transformationPositionStyle( t, t.isIdentity() )+visibilityStyle();
    return imageSource( url1, style, _it->id(), _it->toolTipData(), url2 );
}

void PHISHtmlGeneric::diaShow() const
{
    bool useTmp( false );
    int i;
    QByteArray postfix;
    QList <QByteArray> ids;
    QString imageId;
    QTransform t=transformation();
    if ( _it->hasGraphicEffect() ) postfix=QByteArray::fromRawData( "_g", 2 );
    if ( !t.isIdentity() ) postfix=QByteArray::fromRawData( "_t", 2 );
    for ( i=0; i<_it->pictureBookIds().count(); i++ ) {
        imageId=_it->pictureBookIds().at( i );
        // check if we have a graphical changed picture
        if ( !postfix.isNull() || imageId.startsWith( "phi" )
                || (_it->itemProperties() & PHIItem::PNoCache) ) {
            imageId=checkForImageId( postfix, i ); // image already cached?
            useTmp=true;
            if ( imageId.isNull() ) {
                imageId=_it->pictureBookIds().at( i );
                QImage img=createImageImage( imageId );
                if ( _it->hasGraphicEffect() ) img=graphicsEffectImage( img );
                if ( !t.isIdentity() ) img=img.transformed( t, Qt::SmoothTransformation );
                imageId=storeImage( img, postfix, i );
                if ( imageId.isNull() ) return;
            }
        }
        ids.append( imageId.toUtf8() );
    }
    i=0;
    QByteArray src, imgid;
    QByteArray style=" style=\"position:absolute;"+transformationPositionStyle( t, true );
    QList <QByteArray> titles=_it->toolTipData().split(':');
    _out+=_indent+"<div "+id();
    if ( titles.count()==1 ) _out+=title();
    _out+=style+"\">\n";
    style=" style=\"left:0;top:0;";
    if ( t.isIdentity() ) style+="width:"+QByteArray::number( _it->width() )+"px;height:"
        +QByteArray::number( _it->height() )+"px;";
    foreach ( src, ids ) {
        src="/phi.phis?phiimg="+src+"";
        if ( useTmp ) src+="&amp;phitmp=1";
        imgid=_it->id()+"_phi_"+QByteArray::number( i );
        _out+='\t';
        imageSource( src, style, imgid, (titles.count()>i ? titles.at( i ): QByteArray()) );
        i++;
    }
    _out+=_indent+"</div>\n";
    if ( ids.count()<2 ) return;
    QByteArray ftime=QByteArray::number( _it->fadeTime()*1000 );
    if ( _it->fadeInterval() < _it->fadeTime() ) {
        _req->responseRec()->log( PHILOGERR, PHIRC_OBJ_ACCESS_ERROR,
            QObject::tr( "Fading time must be lesser or equal then fading interval for item '%1' in page '%2'." )
                .arg( _it->name() ).arg( _p->id() ) );
        ftime=QByteArray::number( _it->fadeInterval()*1000 );
    }
    QByteArray imgs=_it->id()+"_phid";
    QByteArray imgn=_it->id()+"_phi_";
    _js+="\nvar "+imgs+"c=0;var "+imgs+"=new Array();var "+imgs+"t=0;\n"
         +imgs+"=phi.getElementById('"+_it->id()+"').getElementsByTagName('img');\n"
         "for(i=1;i<"+imgs+".length;i++){$('"+imgn+"'+i).opacity(0);}\n"
         "phi.getElementById('"+_it->id()+"').onmouseover=function(){\n"
         "\tvar n="+imgs+"c-1;if(n<0)n="+imgs+".length-1;\n"
         "\tvar c="+imgs+"c;\n"
         "\tclearTimeout("+imgs+"t);\n"
         "\tclearTimeout(phi.getElementById('"+imgn+"'+c).timer);\n"
         "\tclearTimeout(phi.getElementById('"+imgn+"'+n).timer);\n"
         "\t$('"+imgn+"'+c).opacity(1);\n"
         "\t$('"+imgn+"'+n).opacity(0);\n"
         "};\n";
    _js+="function "+_it->id()+"_phif(){\n"
         "\tvar n="+imgs+"c+1;if(n>="+imgs+".length)n=0;\n"
         "\tvar c="+imgs+"c;\n"
         "\t$('"+imgn+"'+c).fadeOut(0,"+ftime+");\n"
         "\t$('"+imgn+"'+n).fadeIn(0,"+ftime+");\n"
         "\t"+imgs+"c=n;\n"
         "\t"+imgs+"t=setTimeout("+_it->id()+"_phif,"+QByteArray::number( _it->fadeInterval()*1000 )+");\n"
         "};\n"
         "phi.getElementById('"+_it->id()+"').onmouseout=function(){\n"
         "\t"+imgs+"t=setTimeout("+_it->id()+"_phif,100);\n"
         "};\n"+imgs+"t=setTimeout("+_it->id()+"_phif,"+QByteArray::number( _it->fadeInterval()*100 )+");\n";
}

void PHISHtmlGeneric::phisysLink() const
{
    if ( _it->pictureBookIds().count()==0 ) return;
    QString imageId=_it->pictureBookIds().at( 0 );
    if ( imageId.isNull() ) return;
    QByteArray src="/phi.phis?phiimg="+imageId.toUtf8()+"&amp;phitmp=1";
    return imageSource( src, startStyle(), QByteArray(), _it->toolTipData() );
}

void PHISHtmlGeneric::dateEdit() const
{
    QDate now, start, end;
    QStringList dates=_it->value().split( ':', QString::SkipEmptyParts );
    if ( dates.count()>0 ) now=QDate::fromString( dates[0], PHI::isoDateFormat() );
    if ( !now.isValid() ) now=QDate::currentDate();
    if ( dates.count()>1 ) start=QDate::fromString( dates[1], PHI::isoDateFormat() );
    if ( !start.isValid() ) start=now;
    if ( dates.count()>2 ) end=QDate::fromString( dates[2], PHI::isoDateFormat() );
    if ( !end.isValid() ) end=QDate( 9999, 12, 31 );

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
    _out+=_indent+"<div "+id()+title()+startStyle()+effectStyle()+"\">\n";
    _out+=_indent+"\t<input type=\"hidden\" name=\""+_it->id()+"\" id=\""
        +_it->id()+"_phical"+_endtag;
    _out+=_indent+"\t<input type=\"text\" class=\"phical\" id=\""
        +_it->id()+"_phicaltext\" readonly=\"readonly\""
        +" style=\"position:absolute;left:"+QByteArray::number( lleft )+"px;top:"
        +QByteArray::number( ltop )+"px;width:"+QByteArray::number( lwidth )+"px;height:"
        +QByteArray::number( lheight )+"px;margin:0px;"+fontStyle()+_endtag;
    _out+=_indent+"\t<input type=\"button\" id=\""+_it->id()+"_phibut\""+tabIndex()+accessKey()
        +" value=\"&equiv;\" style=\"position:absolute;left:"+QByteArray::number( bleft )
        +"px;top:"+QByteArray::number( btop )+"px;width:"+QByteArray::number( bwidth )+"px;height:"
        +QByteArray::number( bheight )+"px;margin:0px;"+fontStyle()+_endtag;
    _out+=_indent+"</div>\n";
    _jquery+="\t$('#"+_it->id()+"_phicaltext').css({cursor:'default'}).datepicker({minDate:new Date("
        +QByteArray::number( start.year() )+','+QByteArray::number( start.month()-1 )+','
        +QByteArray::number( start.day() )+"),maxDate:new Date("
        +QByteArray::number( end.year() )+','+QByteArray::number( end.month()-1 )+','
        +QByteArray::number( end.day() )+"),altField:'#"+_it->id()
        +"_phical'});\n\t$('#"+_it->id()+"_phicaltext').datepicker(\"setDate\","
        "new Date("+QByteArray::number( now.year() )+','+QByteArray::number( now.month()-1 )+','
            +QByteArray::number( now.day() )+"));\n"
        "\t$('#"+_it->id()+"_phibut').click(function(){$('#"+_it->id()+"_phicaltext')"
        ".datepicker('show');});\n";
}

void PHISHtmlGeneric::calendar() const
{
    QDate now, start, end;
    QStringList dates=_it->value().split( ':', QString::SkipEmptyParts );
    if ( dates.count()>0 ) now=QDate::fromString( dates[0], PHI::isoDateFormat() );
    if ( !now.isValid() ) now=QDate::currentDate();
    if ( dates.count()>1 ) start=QDate::fromString( dates[1], PHI::isoDateFormat() );
    if ( !start.isValid() ) start=now;
    if ( dates.count()>2 ) end=QDate::fromString( dates[2], PHI::isoDateFormat() );
    if ( !end.isValid() ) end=QDate( 9999, 12, 31 );

    _out+=_indent+"<input type=\"hidden\" name=\""+_it->id()+"\" id=\""+_it->id()+"_phical\" value=\""
        +now.toString( PHI::isoDateFormat() ).toLatin1()+_endtag;
    _out+=_indent+"<div class=\"phical\" "+id()+accessKey()+tabIndex()
        +startStyle( QRect( 0, 0, -6, -4 ) )+fontStyle()+effectStyle()+"\"></div>\n";
    _jquery+="\t$('#"+_it->id()+"').datepicker({minDate:new Date("
        +QByteArray::number( start.year() )+','+QByteArray::number( start.month()-1 )+','
        +QByteArray::number( start.day() )+"),maxDate:new Date("
        +QByteArray::number( end.year() )+','+QByteArray::number( end.month()-1 )+','
        +QByteArray::number( end.day() )+"),altField:'#"+_it->id()+"_phical'});\n"
        "\t$('#"+_it->id()+"').datepicker(\"setDate\","
        "new Date("+QByteArray::number( now.year() )+','+QByteArray::number( now.month()-1 )+','
        +QByteArray::number( now.day() )+"));\n";
}

void PHISHtmlGeneric::progressBar() const
{
    _out+=_indent+"<input type=\"hidden\" name=\""+_it->id()+"\" id=\""+_it->id()+"_phibar\""
        " value=\"0"+_endtag;
    _out+=_indent+"<div class=\"phi_"+QByteArray::number( PHI::PROGRESSBAR )+"\" "
        +id()+startStyle()+fontStyle()+effectStyle()+"\"></div>\n";
    _jquery+="\t$('#"+_it->id()+"').progressbar({value:0});\n";
}

void PHISHtmlGeneric::imageButton() const
{
    _out+=_indent+"<button type=\"button\" class=\"phibuttontext\""
        +name()+disabled()+title()+accessKey()+tabIndex()
        +onClickUrl()+startStyle( adjustButtonSize() )+effectStyle();
    _out+="padding:0;\">\n";
    QByteArray url;
    if ( _it->imageId().startsWith( "phi" ) )
        url="/phi.phis?phiimg="+_it->imageIdData()+"&amp;phitmp=1";
    else url="/phi.phis?phiimg="+_it->imageIdData();
    QByteArray style=" style=\"position:relative;vertical-align:middle;left:-2px;";
    _out+='\t';
    imageSource( url, style, _it->id()+"_phiimg", _it->toolTipData() );
    _out+=_indent+"\t<span style=\""+colorStyle()+fontStyle()
        +"\">"+_it->valueData()+"</span>\n";
    _out+=_indent+"</button>\n";
}

void PHISHtmlGeneric::fileButton() const
{
    _out+=_indent+"<input type=\"file\""+name()+disabled()+title()+accessKey()+tabIndex()
        +startStyle( adjustFileButton() )+fontStyle()+effectStyle()+_endtag;
}

void PHISHtmlGeneric::listBox() const
{
    _out+=_indent+"<select class=\"phitext\" multiple=\"multiple\"";
    _out+=name()+title()+disabled()+accessKey()+tabIndex()+startStyle()
         +fontStyle()+"\">\n";
    _out+=selectOptions();
    _out+=_indent+"</select>\n";
}

void PHISHtmlGeneric::checkBox() const
{
    QByteArray na, endtag=_endtag;
    endtag.chop( 1 ); // remove: \n
    endtag.remove( 0, 1 ); // remove: "
    QByteArray checked=QByteArray::fromRawData( " checked=\"checked\"", 18 );
    if ( !_it->checked() ) checked="";
    if ( !_it->parentId().isEmpty() ) na=" id=\""+_it->id()+"\" name=\""+_it->parentId()+"\"";
    else na=name();
    _out+=_indent+"<table cellspacing=\"0\" cellpadding=\"0\" id=\""+_it->id()
        +"_phitbl\""+startStyle()+effectStyle()+"\">\n"+_indent+"\t<tr>\n";
    _out+=_indent+"\t<td style=\"width:20px;margin-top:auto;margin-bottom:auto;"+heightStyle()+outlineColorStyle()
        +"overflow:visible;\"><input type=\"checkbox\""+na+value()+checked
        +tabIndex()+accessKey()+disabled()+endtag+"</td>\n";
    _out+=_indent+"\t<td style=\"vertical-align:middle;"+heightStyle()+colorStyle()
        +outlineColorStyle()+fontStyle()
        +"overflow:hidden;\"><label id=\""+_it->id()+"_philabel\""+title()+" for=\""+_it->id()
        +"\" style=\"margin:0px;\">";
    _out+=_it->labelData();
    _out+="</label></td>\n";
    _out+=_indent+"\t</tr>\n"+_indent+"</table>\n";
}

void PHISHtmlGeneric::radioButton() const
{
    QByteArray na, endtag=_endtag;
    endtag.chop( 1 ); // remove: \n
    endtag.remove( 0, 1 ); // remove: "
    QByteArray checked=QByteArray::fromRawData( " checked=\"checked\"", 18 );
    if ( !_it->checked() ) checked="";
    if ( !_it->parentId().isEmpty() ) na=" id=\""+_it->id()+"\" name=\""+_it->parentId()+"\"";
    else na=name();
    _out+=_indent+"<table cellspacing=\"0\" cellpadding=\"0\" id=\""+_it->id()
        +"_phitbl\""+startStyle()+effectStyle()+"\">\n"+_indent+"\t<tr>\n";
    _out+=_indent+"\t<td class=\"phiradioitem\" style=\""+heightStyle()+outlineColorStyle()
        +"\"><input type=\"radio\""+na+value()+checked+tabIndex()+accessKey()
        +disabled()+endtag+"</td>\n";
    _out+=_indent+"\t<td style=\"vertical-align:middle;"+heightStyle()+colorStyle()
        +outlineColorStyle()+fontStyle()
        +"overflow:hidden;\"><label id=\""+_it->id()+"_philabel\""+title()+" for=\""+_it->id()
        +"\" style=\"margin:0px;\">";
    _out+=_it->labelData();
    _out+="</label></td>\n";
    _out+=_indent+"\t</tr>\n"+_indent+"</table>\n";
}

void PHISHtmlGeneric::externalHtmlDoc() const
{
    _out+=_indent+"<object "+id()+startStyle()+effectStyle()+"border:none;\" data=\""
        +_it->urlData()+"\" type=\"text/html\"></object>\n";
}

void PHISHtmlGeneric::staticGoogleMaps() const
{
    return createGraphicsImage( GTStaticGoogleMap );
}

void PHISHtmlGeneric::table() const
{
    QString imgId=checkForImageId();
    if ( imgId.isNull() ) {
        QImage img=createHeaderImage();
        imgId=storeImage( img, QByteArray() );
    }

    QByteArray values, current;
    values="phiarr_"+_it->id()+"=new Array(";
    if ( !_it->disabled() ) _out+=_indent+"<input type=\"hidden\" name=\""+_it->id()+"\" id=\""
        +_it->id()+"_phival\" value=\""+_endtag;
    _out+=_indent+"<div"+id()+accessKey()+tabIndex()+title()+startStyle()
        +effectStyle()+fontStyle()+"overflow:auto;\">\n";
    _out+=_indent+"\t<table id=\""+_it->id()+"_phitbl\" cellspacing=\"0\" cellpadding=\"0\" style=\""
        "width:100%;background-image:url(/phi.phis?phiimg="+imgId.toUtf8();
    _out+="&amp;phitmp=1);background-repeat:repeat-x;\">\n";
    int colCount=0, r;
    bool hasHeader=false, isChecked;
    QString col, row, v;
    QByteArray highlight=_p->palette().color( QPalette::Highlight ).name().toLatin1();

    QStringList rows=_it->value().split( _it->delimiter(), QString::SkipEmptyParts );
    for ( r=0; r<rows.count(); r++ ) {
        row=rows.at( r );
        QStringList cols=row.split( "|", QString::KeepEmptyParts );
        if ( r==0 ) {
            colCount=cols.count();
            //_out+=_indent+"\t<thead><tr class=\"phibutton phibuttontext\">";
            _out+=_indent+"\t<thead><tr class=\"phibuttontext\">";
            for ( int c=0; c<colCount; c++ ) {
                col=cols.at( c );
                _out+="<th class=\"phitableth\">"+col.toUtf8()+"</th>";
            }
            _out+="</tr></thead>\n"+_indent+"\t<tbody>\n";
            hasHeader=true;
            continue;
        }
        if ( _it->disabled() ) _out+=_indent+"\t<tr>";
        else _out+=_indent+"\t<tr onclick=\"phitbl_"+_it->id()+'('+QByteArray::number( r )+");\">";
        for ( int c=0; c<colCount; c++ ) {
            if ( c < cols.count() ) col=cols.at( c );
            else col="";
            _out+="<td class=\"phitabletd\"";
            if ( c==0 ) {
                PHI::getItemCheckData( col, v, isChecked );
                if ( isChecked ) current="phitbl_"+_it->id()+'('+QByteArray::number( r )+");\n";
                if ( v.isEmpty() ) values+="'"+col.toUtf8()+"',";
                else values+="'"+v.toUtf8()+"',";
                if ( !_it->disabled() ) _out+=" onmouseover=\"this.style.border='dotted 1px "+highlight
                    +"';this.style.cursor='default';\" onmouseout=\"this.style.border='none'\"";
            }
            _out+='>'+col.toUtf8()+"</td>";
        }
        _out+="</tr>\n";
    }
    _out+=_indent+"\t<tr><td colspan=\""+QByteArray::number(colCount)+"\" "
        "style=\"height:100%;\"></td></tr>\n";
    if ( hasHeader ) _out+=_indent+"\t</tbody>\n";
    _out+=_indent+"\t</table>\n"+_indent+"</div>\n";
    if ( !_it->disabled() ) {
        if ( r>0 ) values.chop( 1 ); // remove last ','
        _js+=values+");\n";
        _js+="function phitbl_"+_it->id()+"(index) {\n"
             "\tvar table=phi.getElementById('"+_it->id()+"_phitbl');\n"
             "\tvar trlist=table.getElementsByTagName('tr');\n"
             "\tvar i=0;\n"
             "\twhile (trlist[i]){\n"
             "\t\ttrlist[i].style.backgroundColor='transparent';\n"
             "\t\tif (i==index) trlist[i].style.backgroundColor='"+highlight+"';\n"
             "\t\ti++;\n"
             "\t}\n"
             "\tphi.getElementById('"+_it->id()+"_phival').value=phiarr_"
                +_it->id()+"[index-1];\n"
             "};\n";
        _js+=current;
    }
}

void PHISHtmlGeneric::checkBoxList() const
{
    QString imgId=checkForImageId();
    if ( imgId.isNull() ) {
        QImage img=createHeaderImage();
        imgId=storeImage( img, QByteArray() );
    }
    QByteArray endtag=_endtag;
    endtag.chop( 1 ); // remove: \n
    _out+=_indent+"<div class=\"phitext\""+id()+title()+accessKey()+tabIndex()
        +startStyle()+fontStyle()+effectStyle()+"overflow:auto;\">\n";
    _out+=_indent+"\t<table cellspacing=\"0\" cellpadding=\"0\" width=\"100%\""
        " id=\""+_it->id()+"_phitbl\" style=\""
        "background-image:url(/phi.phis?phiimg="+imgId.toUtf8();
    _out+="&amp;phitmp=1);background-repeat:repeat-x;\">\n";
    int colCount=0;
    bool hasHeader=false, isChecked;
    QString col, row, v;
    QStringList rows=_it->value().split( _it->delimiter(), QString::SkipEmptyParts );
    for ( int r=0; r<rows.count(); r++ ) {
        row=rows.at( r );
        QStringList cols=row.split( "|", QString::KeepEmptyParts );
        if ( r==0 ) {
            colCount=cols.count();
            _out+=_indent+"\t<thead><tr class=\"phibuttontext\">";
            for ( int c=0; c<colCount; c++ ) {
                col=cols.at( c );
                if ( c==0 ) _out+="<th class=\"phitableth\" colspan=\"2\">";
                else _out+="<th class=\"phitableth\">";
                _out+=col.toUtf8()+"</th>";
            }
            _out+="</tr></thead>\n"+_indent+"\t<tbody>\n";
            hasHeader=true;
            continue;
        }
        _out+=_indent+"\t<tr>";
        for ( int c=0; c<colCount; c++ ) {
            if ( c < cols.count() ) col=cols.at( c );
            else col="";
            qDebug( "col=%s", qPrintable( col ) );
            if ( c==0 ) {
                PHI::getItemCheckData( col, v, isChecked );
                _out+="<td class=\"phichklistitem\">"
                    "<input id=\""+_it->id()+"_phi_"+QByteArray::number( r )+"\" "
                    "type=\"checkbox\" name=\""+_it->id()+"\"";
                if ( isChecked ) _out+=" checked=\"checked\"";
                _out+=disabled()+" value=\""+v.toUtf8()+endtag+"</td>";
                _out+="<td class=\"phitabletd\"><label for=\""
                    +_it->id()+"_phi_"+QByteArray::number( r )+"\">"
                    +col.toUtf8()+"</label></td>";
                continue;
            }
            _out+="<td class=\"phitabletd\">"+col.toUtf8()+"</td>";
        }
        _out+="</tr>\n";
    }
    if ( hasHeader ) _out+=_indent+"\t</tbody>\n";
    _out+=_indent+"\t</table>\n"+_indent+"</div>\n";
}

void PHISHtmlGeneric::selectBox() const
{
    _out+=_indent+"<select class=\"phitext\"";
    _out+=name()+title()+readOnly()+disabled()+accessKey()+tabIndex()
        +startStyle( adjustSelectSize() )+effectStyle()
        +fontStyle()+colorStyle()+"\">\n";
    _out+=selectOptions();
    _out+=_indent+"</select>\n";
}

void PHISHtmlGeneric::langSelector() const
{
    QString lang, langs, localeLang( _lang );
    langs.reserve( 400 );
    if ( _lang.size()==5 ) localeLang=_lang.left(2)+'_'+_lang.right(2).toUpper(); // de-de -> de_DE
    QLocale current( localeLang );

    foreach( lang, _p->languages() ) {
        localeLang=lang;
        if ( lang.size()==5 ) localeLang=lang.left(2)+'_'+lang.right(2).toUpper();
        if ( lang==_lang ) {
            langs.append( current.languageToString( current.language() ) );
            if ( lang.size()==5 ) langs.append( " ("+current.countryToString( current.country() )+')' );
            langs.append( '['+lang+"][1]\n" );
        } else {
            QLocale locale( localeLang );
            langs.append( current.languageToString( locale.language() ) );
            if ( lang.size()==5 ) langs.append( " ("+current.countryToString( locale.country() )+')' );
            langs.append( '['+lang+"]\n" );
        }
    }
    langs.chop( 1 ); // remove last '\n'

    _out+=_indent+"<select class=\"phitext\""+name()+readOnly()+disabled()+title()
        +startStyle( adjustSelectSize() )+fontStyle()+effectStyle()+"\">\n";
    _out+=selectOptions( langs.split( '\n' ) );
    if ( _it->id()=="philangselector" ) {
        QUrlQuery query( _req->url() );
        query.removeQueryItem( "philang" );
        QUrl url( _req->url() );
        url.setQuery( query );
        QByteArray u=url.toEncoded();
        if ( url.hasQuery() ) u.endsWith( '&' ) ? u+="philang=" : u+="&philang=";
        else u+="?philang=";
        _js+="var philangselector=phi.getElementById('philangselector');\n"
             "philangselector.onchange=function(){"
             "window.location.href='"+u+"'+philangselector.value;"
             "};\n";
    }
    _out+=_indent+"</select>\n";
}

void PHISHtmlGeneric::lineEdit() const
{
    _out+=_indent+"<input type=\"text\" class=\"phitext\""+maxLength()
        +name()+title()+readOnly()+disabled()+value()+accessKey()+tabIndex()
        +startStyle( adjustLineEditSize() ) //+"display:block;"
        +fontStyle()+colorStyle()+effectStyle()+_endtag;
}

void PHISHtmlGeneric::textEmail() const
{
    _out+=_indent+"<input type=\"text\" class=\"phitext\""+maxLength()
        +name()+title()+readOnly()+disabled()+value()+accessKey()+tabIndex()
        +startStyle( adjustLineEditSize() )
        +fontStyle()+colorStyle()+effectStyle()+_endtag;
    _js+=textValidatorJS( "/^"+PHI::emailRegExp()+"$/i", "name@domain.com" );
}

void PHISHtmlGeneric::textSearch() const
{
    _out+=_indent+"<input type=\"text\" class=\"phitext\""+maxLength()
        +name()+title()+readOnly()+disabled()+value()+accessKey()+tabIndex()
        +startStyle( adjustLineEditSize() )
        +fontStyle()+colorStyle()+effectStyle()+_endtag;
}

void PHISHtmlGeneric::textPhone() const
{
    _out+=_indent+"<input type=\"text\" class=\"phitext\""+maxLength()
        +name()+title()+readOnly()+disabled()+value()+accessKey()+tabIndex()
        +startStyle( adjustLineEditSize() )
        +fontStyle()+colorStyle()+effectStyle()+_endtag;
    _js+=textValidatorJS( "/^"+PHI::phoneNumberRegExp()+"$/", "+## ### ###..." );
}

void PHISHtmlGeneric::decimalNumber() const
{
    int v, max, min, step;
    PHI::extractNumbers( _it->valueData(), v, min, max, step );
    _out+=_indent+"<input type=\"text\" class=\"phitext\" value=\""
        +QByteArray::number( v )+"\""
        +name()+title()+readOnly()+disabled()+accessKey()+tabIndex()
        +startStyle( adjustLineEditSize() )
        +fontStyle()+colorStyle()+effectStyle()+_endtag;
    _js+=decimalValidatorJS( min, max );
}

void PHISHtmlGeneric::realNumber() const
{
    qreal v, max, min, step;
    int prec;
    PHI::extractRealNumbers( _it->valueData(), v, min, max, step, prec );
    _out+=_indent+"<input type=\"text\" class=\"phitext\" value=\""
        +QByteArray::number( v )+"\""
        +name()+title()+readOnly()+disabled()+accessKey()+tabIndex()
        +startStyle( adjustLineEditSize() )
        +fontStyle()+colorStyle()+effectStyle()+_endtag;
    _js+=realValidatorJS( min, max, prec );
}

void PHISHtmlGeneric::multiLineEdit() const
{
    _out+=_indent+"<textarea class=\"phitext\""+name()+title()
        +disabled()+readOnly()+accessKey()+tabIndex()
        +" cols=\""+QByteArray::number( static_cast<int>(_it->width()/10) )
        +"\" rows=\""+QByteArray::number( static_cast<int>(_it->height()/10) )
        +'"'+startStyle( adjustMultiLineEditSize() )
        +fontStyle()+colorStyle()+effectStyle()+"\">";
    _out+=_it->valueData()+"</textarea>\n";
}

void PHISHtmlGeneric::passwordEdit() const
{
    _out+=_indent+"<input type=\"password\" class=\"phitext\""+maxLength()
        +name()+title()+disabled()+readOnly()+value()+accessKey()+tabIndex()
        +startStyle( adjustLineEditSize() )+fontStyle()
        +colorStyle()+effectStyle()+_endtag;
}

void PHISHtmlGeneric::image() const
{
    qDebug( "imageId=%s", qPrintable( _it->imageId() ) );
    // inline stored image:
    if ( _it->hasGraphicEffect() ) return createGraphicsImage( GTImage );
    if ( _it->itemProperties() & PHIItem::PNoCache ) return createGraphicsImage( GTImage );
    if ( _it->imageId().startsWith( "phi" ) ) return createGraphicsImage( GTImage );
    QTransform t=transformation();
    if ( !t.isIdentity() ) return createGraphicsImage( GTImage );
    // seems we have an image on local disk
    QByteArray style=" style=\""+positionStyle()+visibilityStyle();
    return imageSource( _it->imageIdData(), style, _it->id(), _it->toolTipData() );
}

void PHISHtmlGeneric::createGraphicsImage( GraphicType type ) const
{
    QTransform t=transformation();
    QByteArray src, style=" style=\""+transformationPositionStyle( t )+visibilityStyle();
    QByteArray postfix;
    if ( _it->hasGraphicEffect() ) postfix=QByteArray::fromRawData( "_g", 2 );
    if ( !t.isIdentity() ) postfix=QByteArray::fromRawData( "_t", 2 );
    QString imgId=checkForImageId( postfix );
    if ( imgId.isNull() ) {
        QImage img;
        switch( type ) {
        case GTImage: img=createImageImage( _it->imageId() ); break;
        case GTRect: img=createRectImage(); break;
        case GTRoundedRect: img=createRoundedRectImage(); break;
        case GTEllipse: img=createEllipseImage(); break;
        case GTSvg: img=createSvgImage(); break;
        case GTGraphicsText: img=createGraphicsTextImage(); break;
        case GTRichText: img=createRichTextImage(); break;
        case GTStaticGoogleMap: img=createStaticGoogleMapImage(); break;
        default: img=createRectImage();
        }
        if ( _it->hasGraphicEffect() ) img=graphicsEffectImage( img );
        if ( !t.isIdentity() ) img=img.transformed( t, Qt::SmoothTransformation );
        imgId=storeImage( img, postfix );
    }
    if ( imgId.isNull() ) return;
    src="/phi.phis?phiimg="+imgId.toUtf8()+"&amp;phitmp=1";
    return imageSource( src, style, _it->id(), _it->toolTipData() );
}

void PHISHtmlGeneric::twitterButton() const
{
    QByteArray val=_it->valueData();
    val.replace( "&", "&amp;" );
    _out+=_indent+"<div"+id()+startStyle()+effectStyle()+"overflow:hidden;\">"
        "<a href=\"https://twitter.com/share?"+val;
    if ( _lang.left( 2 )!="C" ) _out+="&amp;lang="+_lang.left( 2 );
    _out+="\" class=\"twitter-share-button\">Tweet</a></div>\n";
}

void PHISHtmlGeneric::youtubeVideo() const
{
    QByteArray val=_it->valueData();
    val.replace( "&", "&amp;" );
    val+="&amp;hl="+_lang.left( 2 )+"&amp;cc_lang_pref="+_lang.left( 2 );
    _out+=_indent+"<object type=\"application/x-shockwave-flash\""+id()+startStyle()+effectStyle()+"border:none;\">\n";
    _out+=_indent+"\t<param name=\"movie\" value=\"http://www.youtube-nocookie.com/v/"+val+_endtag;
    _out+=_indent+"\t<param name=\"allowFullScreen\" value=\"true"+_endtag;
    _out+=_indent+"\t<param name=\"allowscriptaccess\" value=\"always"+_endtag;
    _out+=_indent+"</object>\n";
}

void PHISHtmlGeneric::facebookLikeButton() const
{
    QByteArray val=_it->valueData(), lang=_lang;
    lang.replace( "-", "_" );
    val.replace( "&", "&amp;" );
    if ( _lang!="C" ) val+="&amp;locale="+(_lang.size()==2?_lang+"_"+_lang.toUpper():lang);
    _out+=_indent+"<object type=\"text/html\""+id()+startStyle()+effectStyle()+"border:none;\""
        " data=\"http://www.facebook.com/plugins/like.php?"
        +val+"&amp;width="+QByteArray::number( static_cast<int>(_it->width()) )+"&amp;height="
        +QByteArray::number( static_cast<int>(_it->height()) )+"\"></object>\n";
}

void PHISHtmlGeneric::googlePlusButton() const
{
    QStringList list=_it->value().split( '&', QString::SkipEmptyParts );
    QString tmp, size, href, cb, annotation;
    foreach ( tmp, list ) {
        if ( tmp.startsWith( "href=" ) ) href=tmp.mid( 5 );
        else if ( tmp.startsWith( "size=") ) size=tmp.mid( 5 );
        else if ( tmp.startsWith( "callback=" ) ) cb=tmp.mid( 9 );
        else if ( tmp.startsWith( "annotation=" ) ) annotation=tmp.mid( 11 );
    }
    _out+=_indent+"<div"+id()+startStyle()+effectStyle()+"border:none;overflow:hidden;\">"
        "<g:plusone href=\""+href.toUtf8()+"\" size=\""+size.toUtf8()+"\" callback=\""
        +cb.toUtf8()+"\" annotation=\""+annotation.toUtf8()+"\"></g:plusone></div>\n";
}

/*
QByteArray PHISHtmlGeneric::rect() const
{
    return createGraphicsImage( GTRect );

    if ( t.isIdentity() ) {
        if ( _it->line()<4 && _it->pattern()<2 ) {
            arr+="<div "+id()+style+visibilityStyle()
    out="\t<div "+_id+" style=\""+_vis+genPosition( it, QPoint( -penWidth, -penWidth ) )+_height+_width;
    if ( it->pattern() ) out+=" background-color: "+it->color().name().toLatin1()+';';
    if ( it->line() ) {
        out+=" border: "+line+' '+QByteArray::number( penWidth )
            +"px; border-color: "+it->outlineColor().name().toLatin1();
    }
    out+="\"></div>\n";
        }

    }

}
*/

void PHISHtmlGeneric::hidden() const
{
    _out+=_indent+"<input"+name()+value()+" type=\"hidden"+_endtag;
}

void PHISHtmlGeneric::button() const
{
    _out+=_indent+"<input type=\"button\" class=\"phibuttontext\""+name()
        +title()+disabled()+value()+accessKey()+tabIndex()+onClickUrl()
        +startStyle( adjustButtonSize() )+fontStyle()
        +colorStyle()+effectStyle()+_endtag;
}

void PHISHtmlGeneric::submitButton() const
{
    _out+=_indent+"<input type=\"submit\" class=\"phibuttontext\""+name()
        +title()+disabled()+value()+accessKey()+tabIndex()
        +startStyle( adjustButtonSize() )+fontStyle()+colorStyle()
        +effectStyle()+_endtag;
}

void PHISHtmlGeneric::resetButton() const
{
    _out+=_indent+"<input type=\"reset\" class=\"phibuttontext\""+name()
        +title()+disabled()+value()+accessKey()+tabIndex()
        +startStyle( adjustButtonSize() )+fontStyle()+colorStyle()
        +effectStyle()+_endtag;
}

void PHISHtmlGeneric::unknownItem() const
{
    _out+=QByteArray( _indent+"<div"+id()+startStyle()+"\">PHI ITEM "
        +QByteArray::number( _it->wid() )+"</div>\n" );
}
