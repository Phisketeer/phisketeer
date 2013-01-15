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
#include "phishtml5base.h"

PHISHtml5Base::PHISHtml5Base( const PHISRequest *req, const PHIBasePage *page )
    : PHISHtmlGeneric( req, page )
{
}

PHISHtml5Base::~PHISHtml5Base()
{
}

QByteArray PHISHtml5Base::rgba( const QColor &c ) const
{
    QByteArray arr;
    arr.reserve( 50 );
    arr+="rgba(";
    arr+=QByteArray::number( c.red() )+','+QByteArray::number( c.green() )+','
        +QByteArray::number( c.blue() )+','+QByteArray::number( c.alphaF(), 'f', 3 )+")";
    return arr;
}

QByteArray PHISHtml5Base::phiObject( const QUrl &url, const QByteArray &codebase ) const
{
    QByteArray arr="<div class=\"phicontent\" id=\"phiobjectdiv\" style=\"z-index:1;visibility:hidden\">\n"
        "\t<object id=\"phiobject\" type=\"application/x-phi\" width=\""
        +QByteArray::number( _p->width() )+"\" height=\""+QByteArray::number( _p->height() )
        +"\">\n\t\t<param name=\"src\" value=\""+url.toEncoded()+_endtag
        +"\t\t<param name=\"codebase\" value=\""+codebase+_endtag
        +"\t</object>\n</div>\n";
    return arr;
}

QByteArray PHISHtml5Base::phiObjectExclusive( const QUrl &url, const QByteArray &codebase ) const
{
    QByteArray arr="<div class=\"phicontent\" id=\"phiobjectdiv\">\n"
        "\t<object id=\"phiobject\" type=\"application/x-phi\" width=\""
        +QByteArray::number( _p->width() )+"\" height=\""+QByteArray::number( _p->height() )
        +"\">\n\t\t<param name=\"src\" value=\""+url.toEncoded()+_endtag
        +"\t\t<param name=\"codebase\" value=\""+codebase+_endtag
        +"\t</object>\n</div>\n";
    return arr;
}

QByteArray PHISHtml5Base::effectStyle() const
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
                arr+=QByteArray::number( static_cast<int>(radius) )+"px "+rgba( c )+';';
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

QByteArray PHISHtml5Base::textShadowStyle() const
{
    QByteArray arr;
    if ( _it->effect()->graphicsType()!=PHIEffect::GTShadow ) return arr;
    if ( !PHI::isTextItem( _it->wid() ) ) return arr;
    arr.reserve( 300 );
    QColor c;
    qreal xOff, yOff, radius;
    _it->effect()->shadow( c, xOff, yOff, radius );
    arr+="text-shadow:"+QByteArray::number( static_cast<int>(xOff) )+"px ";
    arr+=QByteArray::number( static_cast<int>(yOff) )+"px ";
    arr+=QByteArray::number( static_cast<int>(radius) )+"px "+rgba( c )+';';
    return arr;
}

void PHISHtml5Base::youtubeVideo() const
{
    QByteArray val=_it->valueData();
    val.replace( "&", "&amp;" );
    val+="&amp;hl="+_lang.left( 2 )+"&amp;cc_lang_pref="+_lang.left( 2 );
    _out+=_indent+"<iframe"+id()+" src=\"http://www.youtube-nocookie.com/embed/"+val+"\"";
    _out+=startStyle()+effectStyle()+"border:none;\"></iframe>\n";
}

void PHISHtml5Base::textEmail() const
{
    _out+=_indent+"<input type=\"email\" class=\"phitext\""+maxLength()
        +name()+title()+disabled()+readOnly()+value()+accessKey()+tabIndex()
        +startStyle( adjustLineEditSize() )
        +fontStyle()+colorStyle()+effectStyle()+_endtag;
    _js+=textValidatorJS( "/^"+PHI::emailRegExp()+"$/i", "name@domain.com" );
}

void PHISHtml5Base::textSearch() const
{
    _out+=_indent+"<input type=\"search\" class=\"phitext\""+maxLength()
        +name()+title()+disabled()+readOnly()+value()+accessKey()+tabIndex()
        +startStyle( adjustLineEditSize() )
        +fontStyle()+colorStyle()+effectStyle()+_endtag;
}

void PHISHtml5Base::textPhone() const
{
    _out+=_indent+"<input type=\"tel\" class=\"phitext\""+maxLength()
        +name()+title()+disabled()+readOnly()+value()+accessKey()+tabIndex()
        +startStyle( adjustLineEditSize() )
        +fontStyle()+colorStyle()+effectStyle()+_endtag;
    _js+=textValidatorJS( "/^"+PHI::phoneNumberRegExp()+"$/", "+## ### ###..." );
}

void PHISHtml5Base::decimalNumber() const
{
    int v, max, min, step;
    PHI::extractNumbers( _it->valueData(), v, min, max, step );
    _out+=_indent+"<input type=\"number\" class=\"phitext\" value=\""+QByteArray::number( v )
        +"\" min=\""+QByteArray::number( min )+"\" max=\""+QByteArray::number( max )
        +"\" step=\""+QByteArray::number( step )+'\"'
        +name()+title()+disabled()+readOnly()+accessKey()+tabIndex()
        +startStyle( adjustLineEditSize() )
        +fontStyle()+colorStyle()+effectStyle()+_endtag;
    _js+=decimalValidatorJS( min, max );
}

void PHISHtml5Base::realNumber() const
{
    qreal v, max, min, step;
    int prec;
    PHI::extractRealNumbers( _it->valueData(), v, min, max, step, prec );
    _out+=_indent+"<input type=\"number\" class=\"phitext\" value=\""+QByteArray::number( v )
        +"\" min=\""+QByteArray::number( min )+"\" max=\""+QByteArray::number( max )
        +"\" step=\""+QByteArray::number( step )+'\"'
        +name()+title()+disabled()+readOnly()+accessKey()+tabIndex()
        +startStyle( adjustLineEditSize() )
        +fontStyle()+colorStyle()+effectStyle()+_endtag;
    _js+=realValidatorJS( min, max, prec );
}

void PHISHtml5Base::label() const
{
    QColor c=_it->outlineColor();
    if ( !c.isValid() ) c=QColor( Qt::transparent );
    _out+=_indent+"<div "+id()+startStyle()+effectStyle()+"\">\n";
    _out+=_indent+"\t<table id=\""+_it->id()+"_phitbl\" style=\"width:100%;\"><tr><td style=\""
        +alignmentStyle()+colorStyle()+"background-color:"+rgba( c )+";"+fontStyle()+heightStyle()
        +"overflow:visible;white-space:nowrap;\" onmouseover=\"this.style.cursor='default';\">";
        //" onmouseout=\"this.style.cursor='auto';\">";
    _out+=_it->valueData();
    _out+="</td></tr></table>\n"+_indent+"</div>\n";
}

void PHISHtml5Base::menuLink() const
{
    QColor c=_it->outlineColor();
    if ( !c.isValid() ) c=QColor( Qt::transparent );
    _out+=_indent+"<div "+id()+title()+onClickUrl()+tabIndex()+startStyle()+effectStyle()+"\">\n";
    _out+=_indent+"\t<table style=\"width:100%;\" "
        +"id=\""+_it->id()+"_phitbl\"><tr><td style=\""+alignmentStyle()+colorStyle()
        +fontStyle()+heightStyle()+"overflow:visible;white-space:nowrap;";
    _out+="background-color:"+rgba( c )+'"';
    if ( _it->rolloverTextColor().isValid() || _it->rolloverBackgroundColor().isValid() ) {
        _out+=" onmouseover=\"this.style.cursor='default';";
        if ( _it->rolloverTextColor().isValid() )
            _out+="this.style.color='"+_it->rolloverTextColor().name().toLatin1()+"';";
        if ( _it->rolloverBackgroundColor().isValid() ) {
            QColor rc=_it->rolloverBackgroundColor();
            _out+="this.style.backgroundColor='"+rgba( rc )+"';";
        }
        _out+='"';
        _out+=" onmouseout=\"";
        QColor windowText=_it->color();
        if ( !windowText.isValid() ) windowText=_p->palette().color( QPalette::WindowText );
        _out+="this.style.color='"+windowText.name().toLatin1()+"';";
        _out+="this.style.backgroundColor='"+rgba( c )+"';";
        _out+='"';
    }
    _out+='>'+_it->valueData();
    _out+="</td></tr></table>\n"+_indent+"</div>\n";
}

void PHISHtml5Base::table() const
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
    _out+=_indent+"\t<table id=\""+_it->id()+"_phitbl\" style=\"width:100%;"
        "background-image:url(/phi.phis?phiimg="+imgId.toUtf8();
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
            //arr+=_indent+"\t<thead><tr class=\"phibutton phibuttontext\">";
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

void PHISHtml5Base::checkBoxList() const
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
    _out+=_indent+"\t<table id=\""+_it->id()+"_phitbl\" style=\"width:100%;"
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
                _out+=disabled();
                _out+=" value=\""+v.toUtf8()+endtag+"</td>";
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

void PHISHtml5Base::checkBox() const
{
    QByteArray na, endtag=_endtag;
    endtag.chop( 1 ); // remove: \n
    endtag.remove( 0, 1 ); // remove: "
    QByteArray checked=QByteArray::fromRawData( " checked=\"checked\"", 18 );
    if ( !_it->checked() ) checked="";
    if ( !_it->parentId().isEmpty() ) na=" id=\""+_it->id()+"\" name=\""+_it->parentId()+"\"";
    else na=name();
    _out+=_indent+"<table id=\""+_it->id()
        +"_phitbl\""+startStyle()+effectStyle()+"\">\n"+_indent+"\t<tr>\n";
    _out+=_indent+"\t<td style=\"width:20px;margin-top:auto;margin-bottom:auto;"+heightStyle()+outlineColorStyle()
        +"overflow:visible;\"><input type=\"checkbox\""+na+value()+checked
        +tabIndex()+accessKey()+disabled()+endtag+"</td>\n";
    _out+=_indent+"\t<td style=\"vertical-align:middle;"+heightStyle()+colorStyle()
        +outlineColorStyle()+fontStyle()
        +"overflow:visible;\"><label id=\""+_it->id()+"_philabel\""+title()+" for=\""+_it->id()
        +"\" style=\"margin:0px;\">";
    _out+=_it->labelData();
    _out+="</label></td>\n";
    _out+=_indent+"\t</tr>\n"+_indent+"</table>\n";
}

void PHISHtml5Base::radioButton() const
{
    QByteArray na, endtag=_endtag;
    endtag.chop( 1 ); // remove: \n
    endtag.remove( 0, 1 ); // remove: "
    QByteArray checked=QByteArray::fromRawData( " checked=\"checked\"", 18 );
    if ( !_it->checked() ) checked="";
    if ( !_it->parentId().isEmpty() ) na=" id=\""+_it->id()+"\" name=\""+_it->parentId()+"\"";
    else na=name();
    _out+=_indent+"<table id=\""+_it->id()
        +"_phitbl\""+startStyle()+effectStyle()+"\">\n"+_indent+"\t<tr>\n";
    _out+=_indent+"\t<td class=\"phiradioitem\" style=\""+heightStyle()+outlineColorStyle()
        +"\"><input type=\"radio\""+na+value()+checked+tabIndex()+accessKey()
        +disabled()+endtag+"</td>\n";
    _out+=_indent+"\t<td style=\"vertical-align:middle;"+heightStyle()+colorStyle()
        +outlineColorStyle()+fontStyle()
        +"overflow:visible;\"><label id=\""+_it->id()+"_philabel\""+title()+" for=\""+_it->id()
        +"\" style=\"margin:0px;\">";
    _out+=_it->labelData();
    _out+="</label></td>\n";
    _out+=_indent+"\t</tr>\n"+_indent+"</table>\n";
}

void PHISHtml5Base::svg() const
{
    _out+=_indent+"<div class=\"phi_42\""+id()+title()+startStyle()+effectStyle()+"\">\n";
    _out+=_it->valueData();
    _out+=_indent+"</div>\n";
    _jquery+="\t$('#"+_it->id()+" svg').css({width:'100%',height:'100%'});\n";
}

void PHISHtml5Base::rect() const
{
    if ( _it->hasGraphicEffect() && _it->effect()->graphicsType()!=PHIEffect::GTShadow )
        return PHISHtmlGeneric::rect();
    int pw=static_cast<int>(_it->penWidth());
    if ( pw>1 ) return PHISHtmlGeneric::rect();
    if ( _it->line()>3 ) return PHISHtmlGeneric::rect();
    if ( _it->pattern()>1 ) return PHISHtmlGeneric::rect();
    if ( !transformation().isAffine() ) return PHISHtmlGeneric::rect();
    QByteArray c="background-color:"+rgba( _it->color().isValid() ? _it->color() : QColor( Qt::black ) )+';';
    if ( _it->pattern()==0 ) c="background-color:transparent;";
    QByteArray bc=QByteArray( _it->outlineColor().isValid() ? _it->outlineColor().name().toLatin1()+' ' : "black " );
    QByteArray border;
    if ( _it->line()==1 ) border="border:"+bc+QByteArray::number( pw )+"px solid;";
    else if ( _it->line()==2 ) border="border:"+bc+QByteArray::number( pw )+"px dashed;";
    else if ( _it->line()==3 ) border="border:"+bc+QByteArray::number( pw )+"px dotted;";
    QRect r;
    if ( !border.isEmpty() ) if ( pw==1 ) r.adjust( 0, 0, -1, -1 );
    _out+=_indent+"<div "+id()+startStyle( r )+effectStyle()+c+border+"\"></div>\n";
}

void PHISHtml5Base::roundedRect() const
{
    if ( _it->hasGraphicEffect() && _it->effect()->graphicsType()!=PHIEffect::GTShadow )
        return PHISHtmlGeneric::roundedRect();
    int pw=static_cast<int>(_it->penWidth());
    if ( pw>1 ) return PHISHtmlGeneric::roundedRect();
    if ( _it->line()>3 ) return PHISHtmlGeneric::roundedRect();
    if ( _it->pattern()>1 ) return PHISHtmlGeneric::roundedRect();
    if ( !transformation().isAffine() ) return PHISHtmlGeneric::roundedRect();
    QByteArray c="background-color:"+rgba( _it->color().isValid() ? _it->color() : QColor( Qt::black ) )+';';
    if ( _it->pattern()==0 ) c="background-color:transparent;";
    QByteArray bc=QByteArray( _it->outlineColor().isValid() ? _it->outlineColor().name().toLatin1()+' ' : "black " );
    QByteArray border;
    if ( _it->line()==1 ) border="border:"+bc+QByteArray::number( pw )+"px solid;";
    else if ( _it->line()==2 ) border="border:"+bc+QByteArray::number( pw )+"px dashed;";
    else if ( _it->line()==3 ) border="border:"+bc+QByteArray::number( pw )+"px dotted;";
    QRect r;
    if ( !border.isEmpty() ) if ( pw==1 ) r.adjust( 0, 0, -1, -1 );
    _out+=_indent+"<div "+id()+startStyle( r )+effectStyle()+c+border;
    _out+="border-radius:"+QByteArray::number( _it->borderRadius() )+"px;\"></div>\n";
}
