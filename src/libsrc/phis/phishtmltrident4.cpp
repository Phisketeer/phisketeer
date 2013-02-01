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
#include "phishtmltrident4.h"
#include "phierror.h"

PHISHtmlTrident4::PHISHtmlTrident4( const PHISRequest *req, const PHIBasePage *page )
    : PHISHtmlGeneric( req, page )
{
}

PHISHtmlTrident4::~PHISHtmlTrident4()
{
}

QByteArray PHISHtmlTrident4::phisEngine() const
{
    static QByteArray arr="Phis "+PHIS::libVersion().toLatin1()+"/Trident 4";
    return arr;
}

QByteArray PHISHtmlTrident4::phiObject( const QUrl &url, const QByteArray &codebase ) const
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

QByteArray PHISHtmlTrident4::postProcessPhiObjectJS() const
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

QByteArray PHISHtmlTrident4::phiObjectExclusive( const QUrl &url, const QByteArray &codebase ) const
{
    QByteArray arr="<div class=\"phicontent\" id=\"phiobjectdiv\">\n"
        "\t<object classid=\"clsid:F4463E56-F3CA-45E4-93FE-B8F007376088\" id=\"phiobject\""
        " type=\"application/x-phi\" width=\""
        +QByteArray::number( _p->width() )+"\" height=\""+QByteArray::number( _p->height() )
        +_endtag+"\t\t<param name=\"src\" value=\""+url.toEncoded()+_endtag
        +"\t\t<param name=\"codebase\" value=\""+codebase+_endtag+"\t</object>\n</div>\n";
    return arr;
}

QByteArray PHISHtmlTrident4::postProcessPhiObjectExclusiveJS() const
{
    static QByteArray arr="var phiobject=phi.getElementById('phiobject');\n"
        "phiobject.titleChanged=function(t){document.title=t;};\n"
        "phiobject.linkHovered=function(l){window.status=l;};\n";
    return arr;
}

void PHISHtmlTrident4::diaShow() const
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
        if ( !postfix.isNull() || imageId.startsWith( QLatin1String( "phi" ) )
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
    QByteArray style=" style=\""+visibilityStyle();
    QList <QByteArray> titles=_it->toolTipData().split(':');
    _out+=_indent+"<div "+id();
    if ( titles.count()==1 ) _out+=title();
    _out+=style+"\">\n";
    foreach ( src, ids ) {
        src="/phi.phis?phiimg="+src+"";
        if ( useTmp ) src+="&amp;phitmp=1";
        imgid=_it->id()+"_phi_"+QByteArray::number( i );
        _out+='\t';
        imageSource( src, QByteArray(), imgid, (titles.count()>i ? titles.at( i ): QByteArray()) );
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
    QByteArray count=QByteArray::number( ids.count() );

    _js+="\nvar "+imgs+"c=0;var "+imgs+"t=0;\n"
         "for(i=1;i<"+count+";i++){$('"+imgn+"'+i).opacity(0);}\n"
         "phi.getElementById('"+_it->id()+"').onmouseover=function(){\n"
         "\tvar n="+imgs+"c-1;if(n<0)n="+count+"-1;\n"
         "\tvar c="+imgs+"c;\n"
         "\tclearTimeout("+imgs+"t);\n"
         "\tclearTimeout(phi.getElementById('"+imgn+"'+c).timer);\n"
         "\tclearTimeout(phi.getElementById('"+imgn+"'+n).timer);\n"
         "\t$('"+imgn+"'+c).opacity(1);\n"
         "\t$('"+imgn+"'+n).opacity(0);\n"
         "};\n";
    _js+="function "+_it->id()+"_phif(){\n"
         "\tvar n="+imgs+"c+1;if(n>="+count+")n=0;\n"
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

void PHISHtmlTrident4::imageButton() const
{
    _out+=_indent+"<button type=\"button\" class=\"phibuttontext\""
        +name()+disabled()+title()+accessKey()+tabIndex()
        +onClickUrl()+startStyle( adjustButtonSize() )+effectStyle();
    _out+="padding:0;\">\n";
    QByteArray url;
    if ( _it->imageId().startsWith( QLatin1String( "phi" ) ) )
        url="/phi.phis?phiimg="+_it->imageIdData()+"&amp;phitmp=1";
    else url="/phi.phis?phiimg="+_it->imageIdData();
    _out+=_indent+"\t<span style=\"display:inline-block;vertical-align:middle;width:"
        +QByteArray::number( static_cast<int>(_it->height()/1.5) )
        +"px;height:"+QByteArray::number( static_cast<int>(_it->height()/1.5) )+"px;"
        "filter:progid:DXImageTransform.Microsoft.AlphaImageLoader(src='"
        +url+"');\"></span>\n";
    if ( !_it->valueData().isEmpty() ) {
        _out+=_indent+"\t<span style=\"vertical-align:middle;margin-left:8px;"+colorStyle()+fontStyle();
        _out+="\">"+_it->valueData()+"</span>\n";
    }
    _out+=_indent+"</button>\n";
}

QByteArray PHISHtmlTrident4::positionStyle( const QRect &off ) const
{
    QByteArray arr, filter;
    arr.reserve( 300 );
    filter.reserve( 200 );
    filter+=" filter:";
    QRectF r( 0, 0, _it->width(), _it->height() );
    QTransform t=transformation();
    if ( _it->hasGraphicEffect() ) {
        if ( _it->effect()->graphicsType()==PHIEffect::GTShadow ) {
            QColor c;
            qreal xOff, yOff, radius;
            _it->effect()->shadow( c, xOff, yOff, radius );
            filter+="progid:DXImageTransform.Microsoft.DropShadow(Color='#";
            filter+=QByteArray::number( c.rgba(), 16 )+"',OffX="+QByteArray::number( xOff );
            filter+=",OffY="+QByteArray::number( yOff )+")";
            if ( xOff<0 ) r.adjust( xOff, 0, 0, 0 );
            if ( yOff<0 ) r.adjust( 0, yOff, 0, 0 );
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

void PHISHtmlTrident4::youtubeVideo() const
{
    QByteArray val=_it->valueData();
    val.replace( "&", "&amp;" );
    val+="&amp;hl="+_lang.left( 2 )+"&amp;cc_lang_pref="+_lang.left( 2 );
    _out+=_indent;
    _out+="<object classid=\"clsid:D27CDB6E-AE6D-11cf-96B8-444553540000\" type=\"application/x-shockwave-flash\"";
    _out+=id()+startStyle()+effectStyle()+"border:none;\">\n";
    _out+=_indent+"\t<param name=\"movie\" value=\"http://www.youtube-nocookie.com/v/"+val+_endtag;
    _out+=_indent+"\t<param name=\"allowFullScreen\" value=\"true"+_endtag;
    _out+=_indent+"\t<param name=\"allowscriptaccess\" value=\"always"+_endtag;
    _out+=_indent+"</object>\n";
}

void PHISHtmlTrident4::externalHtmlDoc() const
{
    _out+=_indent+"<iframe "+id()+startStyle()+effectStyle()+"border:none;\" frameBorder=\"0\" src=\""
        +_it->urlData()+"\" type=\"text/html\"></iframe>\n";
}

void PHISHtmlTrident4::facebookLikeButton() const
{
    QByteArray val=_it->valueData(), url, lang=_lang;
    val.replace( "&", "&amp;" );
    lang.replace( "-", "_" );
    if ( _lang!="C" ) val+="&amp;locale="+(_lang.size()==2?_lang+"_"+_lang.toUpper():lang);
    url="http://www.facebook.com/plugins/like.php?"
        +val+"&amp;width="+QByteArray::number( static_cast<int>(_it->width()) )+"&amp;height="
        +QByteArray::number( static_cast<int>(_it->height()) );
    _out+=_indent+"<iframe allowTransparency=\"true\" src=\""+url+"\""+id()+startStyle()+effectStyle()
        +"border:none;\" frameBorder=\"0\" scrolling=\"no\"></iframe>\n";
    //arr+=_indent+"\t<param name=\"src\" value=\""+url+_endtag;
    //arr+=_indent+"</object>\n";
}

void PHISHtmlTrident4::imageSource( const QByteArray &src1, const QByteArray&,
    const QByteArray &iid, const QByteArray &t, const QByteArray &src2 ) const
{
    // style must be without width and height
    QByteArray title, id, style, scale;
    style.reserve( 100 );
    if ( !t.isEmpty() ) title=" title=\""+t+'"';
    if ( !iid.isEmpty() ) id=" id=\""+iid+'"';
    QTransform trans=transformation();
    QRect r=transformationPosition( trans );
    //if ( _it->wid()==PHI::DIA_SHOW ) {
    //    style=" style=\"position:relative;left:0px;top:0px;";
    //} else {
        style+=" style=\"position:absolute;left:"
            +QByteArray::number( static_cast<int>(_it->x())+r.x() )+"px;top:"
            +QByteArray::number( static_cast<int>(_it->y())+r.y() )+"px;z-index:"
            +QByteArray::number( static_cast<int>(_it->zValue()) )+";margin:0;width:"
            +QByteArray::number( r.width() )+"px;height:"
            +QByteArray::number( r.height() )+"px;";
    //}
    if ( _it->wid()!=PHI::DIA_SHOW ) style+=visibilityStyle();

    //if ( trans.isIdentity() ) scale=QByteArray::fromRawData( ",sizingMethod=scale", 19 );
    scale=QByteArray::fromRawData( ",sizingMethod=scale", 19 );

    if ( src2.isEmpty() ) {
        _out+=_indent+"<div"+id+title;
        if ( !_it->url().isEmpty() ) {
            _out+=onClickUrl()+accessKey()+tabIndex();
            _out+=" onmouseover=\"this.style.cursor='pointer';\"";
        }
        _out+=style+"\"><div style=\"width:100%;height:100%;";
        _out+="filter:progid:DXImageTransform.Microsoft.AlphaImageLoader(src=";
        _out+=src1+scale+");\"></div></div>\n";
        return;
    }
    // we have a rollover
    _out+=_indent+"<div"+id+title;
    if ( !_it->url().isEmpty() ) {
        _out+=onClickUrl()+accessKey()+tabIndex();
    }

    _out+=style+"\"><div style=\"width:100%;height:100%;";
    _out+="filter:progid:DXImageTransform.Microsoft.AlphaImageLoader(src=";
    _out+=src1+scale+");\"";
    QByteArray cursorover;
    if ( !_it->url().isEmpty() ) {
        cursorover="this.style.cursor='pointer';";
    }
    _out+=" onmouseover=\"this.style.filter='progid:DXImageTransform.Microsoft.AlphaImageLoader(src="
         +src2+scale+")';"+cursorover;
    _out+="\" onmouseout=\"this.style.filter='progid:DXImageTransform.Microsoft.AlphaImageLoader(src="
         +src1+scale+")';\"></div></div>\n";
}
