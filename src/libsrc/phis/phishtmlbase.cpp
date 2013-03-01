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
#include <QTransform>
#include <QLocale>
#include "phishtmlbase.h"
#include "phisitemcache.h"
#include "phisimagecache.h"
#include "phierror.h"
#include "phiparent.h"

PHISHtmlBase::PHISHtmlBase( const PHISRequest *req, const PHIBasePage *page )
    : _req( req ), _p( page ), _lang( req->currentLangByteArray() ), _it( 0 )
    //, _options( PHISHtmlBase::ONone )
{
    Q_ASSERT( _p );
    Q_ASSERT( _req);
    qDebug( "Browser %d %d.%d", _req->agentEngine(), _req->engineMajorVersion(),
        _req->engineMinorVersion() );
    _endtag=QByteArray::fromRawData( "\">\n", 3 );
}

PHISHtmlBase::~PHISHtmlBase()
{
}

void PHISHtmlBase::createHtmlForItem()
{
    Q_ASSERT(_it);
    if ( _it->opacity()<1. )
        _js+="$('"+_it->id()+"').opacity("+QByteArray::number( _it->opacity() )+");\n";
    _js+=effectJS();
    jQueryItemSetup();

    switch ( _it->wid() ) {
    case PHI::LABEL: return label();
    case PHI::LINK: return menuLink();
    case PHI::LINE_EDIT: return lineEdit();
    case PHI::TEXT: return richText();
    case PHI::RICH_TEXT: return richTextLayouted();
    case PHI::RECT: return rect();
    case PHI::IMAGE: return image();
    case PHI::BUTTON: return button();
    case PHI::ROLLOVER_BUTTON: return rolloverButton();
    case PHI::MULTI_LINE_EDIT: return multiLineEdit();
    case PHI::COMBO_BOX: return selectBox();
    case PHI::TABLE: return table();
    case PHI::CHECK_BOX: return checkBox();
    case PHI::RADIO_BUTTON: return radioButton();

    case PHI::HIDDEN: return hidden();
    case PHI::ROUNDED_RECT: return roundedRect();
    case PHI::GRAPH_TEXT: return graphicsText();
    case PHI::PASSWD: return passwordEdit();
    case PHI::SUBMIT_BUTTON: return submitButton();
    case PHI::CHECK_LIST: return checkBoxList();
    case PHI::LIST: return listBox();
    case PHI::ELLIPSE: return ellipse();

    case PHI::LAYOUT_HBOX:
    case PHI::LAYOUT_VBOX: return createLayout();

    case PHI::LAYOUT_LOGIN:
    case PHI::LAYOUT_ADDRESS:
    case PHI::LAYOUT_CREDIT_CARD:
    case PHI::LAYOUT_CONTACT:
    case PHI::LAYOUT_PERIOD:
    case PHI::LAYOUT_REGISTER:
    case PHI::LAYOUT_FORM:
    case PHI::LAYOUT_GRID: return createGridLayout();

    case PHI::LANG_SELECTOR: return langSelector();
    //case PHI::PHISYS_LINK: _options |= OPhisysLink; return phisysLink();
    case PHI::PHISYS_LINK: return phisysLink();
    case PHI::FILE_BUTTON: return fileButton();
    case PHI::FACEBOOK_LIKE: return facebookLikeButton();
    case PHI::GOOGLE_PLUS: return googlePlusButton();
    case PHI::TWITTER: return twitterButton();
    case PHI::DATEEDIT: return dateEdit();
    case PHI::CALENDAR: return calendar();
    case PHI::DIA_SHOW: return diaShow();
    case PHI::EMAIL: return textEmail();
    case PHI::REALNUMBER: return realNumber();
    case PHI::DECIMAL: return decimalNumber();
    case PHI::PHONE: return textPhone();
    case PHI::SEARCH: return textSearch();
    case PHI::HTML_DOC: return externalHtmlDoc();
    case PHI::COUNTRY: return countryBox();
    case PHI::IMAGE_BUTTON: return imageButton();
    case PHI::SVG: return svg();
    case PHI::RESET_BUTTON: return resetButton();
    case PHI::GOOGLE_STATIC_MAP: return staticGoogleMaps();
    case PHI::PROGRESSBAR: return progressBar();
    case PHI::YOUTUBE: return youtubeVideo();
    case PHI::CANVAS: return canvas();
    case PHI::GOOGLE_MAPS: return googleDynamicMaps();
    case PHI::GOOGLE_CALENDAR: return googleCalendar();

    // unused:
    case PHI::VSPACE:
    case PHI::HSPACE:
    case PHI::IMAGE_BOOK:
    case PHI::LINE:
    case PHI::TAB:
    case PHI::LAYOUT_DELIVERY:
    case PHI::NO_ITEM: return;
    }
    return unknownItem();
}

void PHISHtmlBase::createGridLayout()
{
    QByteArray childId;
    _out+=_indent+"<div "+id()+startStyle()+effectStyle()+"\">\n";
    _indent.append( '\t' );
    const PHIBaseItem *parent=_it;
    foreach ( childId, parent->gridLayoutInfo().keys() ) {
        _it=_p->items().value( childId );
        if ( !_it ) {
            _req->responseRec()->log( PHILOGWARN, PHIRC_OBJ_NOT_FOUND_ERROR,
                QObject::tr( "Child item '%1' of grid layout '%2' in page '%3' not found." )
                .arg( QString::fromUtf8( childId ) ).arg( parent->name() ).arg( _p->id() ) );
            continue;
        }
        createHtmlForItem();
    }
    _indent.chop( 1 );
    _it=parent;
    _out+=_indent+"</div>\n";
}

void PHISHtmlBase::createLayout()
{
    QByteArray childId;
    _out+=_indent+"<div "+id()+startStyle()+effectStyle()+"\">\n";
    _indent.append( '\t' );
    const PHIBaseItem *parent=_it;
    foreach ( childId, parent->childIds() ) {
        _it=_p->items().value( childId );
        if ( !_it ) {
            _req->responseRec()->log( PHILOGWARN, PHIRC_OBJ_NOT_FOUND_ERROR,
                QObject::tr( "Child item '%1' of linear layout '%2' in page '%3' not found." )
                .arg( QString::fromUtf8( childId ) ).arg( parent->name() ).arg( _p->id() ) );
            continue;
        }
        createHtmlForItem();
    }
    _indent.chop( 1 );
    _it=parent;
    _out+=_indent+"</div>\n";
}

QByteArray PHISHtmlBase::phiBaseScript() const
{
    static QByteArray arr="\t<script type=\"text/javascript\" src=\"/phi.phis?phijs=phibase\"></script>\n";
    return arr;
}

QByteArray PHISHtmlBase::jQueryScript() const
{
    static QByteArray arr1="\t<script type=\"text/javascript\" src=\"/phi.phis?phijs=jquery\"></script>\n";
    if ( !(_p->attributes() & PHIPage::AUsejQueryUI) ) return arr1;
    static QByteArray arr2="\t<script type=\"text/javascript\" src=\"/phi.phis?phijs=ui-core\"></script>\n"
        "\t<script type=\"text/javascript\" src=\"/phi.phis?phijs=ui-widget\"></script>\n"
        "\t<script type=\"text/javascript\" src=\"/phi.phis?phijs=ui-effects\"></script>\n";
    if ( !(_p->attributes() & PHIPage::AHasDragDrop) ) return arr1+arr2;
    static QByteArray arr3="\t<script type=\"text/javascript\" src=\"/phi.phis?phijs=ui-mouse\"></script>\n"
        "\t<script type=\"text/javascript\" src=\"/phi.phis?phijs=ui-draggable\"></script>\n"
        "\t<script type=\"text/javascript\" src=\"/phi.phis?phijs=ui-droppable\"></script>\n";
    return arr1+arr2+arr3;
}

QByteArray PHISHtmlBase::charset() const
{
    QByteArray arr="\t<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8"+_endtag;
    return arr;
}

void PHISHtmlBase::jQueryItemSetup() const
{
    if ( _it->draggable() ) {
        _jquery+="\t$('#"+_it->id()+"').draggable({zIndex:10000,cursor:'move',";
        if ( !_it->dragMoveAction() ) _jquery+="helper:'clone',";
        if ( _it->dragRevertOnAccept() && _it->dragRevertOnIgnore() ) _jquery+="revert:true,";
        else if ( _it->dragRevertOnAccept() ) _jquery+="revert:'valid',";
        else if ( _it->dragRevertOnIgnore() ) _jquery+="revert:'invalid',";
        if ( _it->dragOpacity()<1. ) _jquery+="opacity:"+QByteArray::number( _it->dragOpacity() )+',';
        if ( _it->dragHotSpotType()==1 ) _jquery+="cursorAt:{left:"
                +QByteArray::number( static_cast<int>(_it->width()/2) )+",top:"
                +QByteArray::number( static_cast<int>(_it->height()/2) )+"},";
        else _jquery+="cursorAt:{left:"+QByteArray::number( _it->dragHotSpotX() )+",top:"
                +QByteArray::number( _it->dragHotSpotY() )+"},";
        _jquery+="distance:"+QByteArray::number( _it->dragDistance() );
        _jquery+="});\n";
    }
    if ( _it->droppable() ) {
        _jquery+="\t$('#"+_it->id()+"').droppable({";
        QByteArray arr;
        if ( !_it->dropAcceptedIds().isEmpty() ) {
            arr=_it->dropAcceptedIds().join( QStringLiteral( ",#" ) ).toLatin1();
            arr.prepend( "#" );
            //arr.append( "'" );
        }
        if ( !arr.isEmpty() ) _jquery+="accept:'"+arr+"',";
        if ( _it->dropHighlightItem() ) _jquery+="hoverClass:'ui-state-hover',";
        _jquery+="tolerance:'pointer'});\n";
    }
}

QByteArray PHISHtmlBase::phiCanvasScript() const
{
    static QByteArray arr="\t<script type=\"text/javascript\" src=\"/phi.phis?phijs=excanvas\"></script>\n";
    if ( _req->agentEngine()==PHISRequest::Trident ) {
        if ( _req->engineMajorVersion()<5 ) return arr; // Trident 5 = MSIE 9
    }
    return QByteArray();
}

QByteArray PHISHtmlBase::phiProgressBarScript() const
{
    static QByteArray arr="\t<script type=\"text/javascript\" src=\"/phi.phis?phijs=ui-progressbar\"></script>\n";
    return arr;
}

QByteArray PHISHtmlBase::phiFacebookScript() const
{
    static QByteArray arr;
    return arr;
}

QByteArray PHISHtmlBase::phiTwitterScript() const
{
    static QByteArray arr="\t<script type=\"text/javascript\" charset=\"utf-8\" src=\"//platform.twitter.com/widgets.js\"></script>\n";
    return arr;
}

QByteArray PHISHtmlBase::phiGooglePlusScript() const
{
    static QByteArray arr="\t<script type=\"text/javascript\" src=\"https://apis.google.com/js/plusone.js\"></script>\n";
    return arr;
}

QByteArray PHISHtmlBase::phiCalendarScript() const
{
    static QByteArray arr="\t<script type=\"text/javascript\" src=\"/phi.phis?phijs=ui-datepicker\"></script>\n";
    QLocale locale( QString::fromLatin1( toLocaleFormat( _lang ) ) );
    QString monthnames, shortmonthnames, shortdaynames, daynames;
    monthnames.reserve( 200 );
    shortmonthnames.reserve( 100 );
    daynames.reserve( 200 );
    shortdaynames.reserve( 100 );
    QLatin1String sep=QLatin1String( "'," );
    QLatin1Char s=QLatin1Char( '\'' );
    int i;
    for( i=1; i<13; i++ ) monthnames+=s+locale.monthName( i, QLocale::LongFormat )+sep;
    monthnames.chop( 1 );
    for( i=1; i<13; i++ ) shortmonthnames+=s+locale.monthName( i, QLocale::ShortFormat )+sep;
    shortmonthnames.chop( 1 );
    shortdaynames+=s+locale.dayName( 7, QLocale::ShortFormat )+sep;
    for ( i=1; i<7; i++ ) shortdaynames+=s+locale.dayName( i, QLocale::ShortFormat )+sep;
    shortdaynames.chop( 1 );
    daynames+=s+locale.dayName( 7, QLocale::LongFormat )+sep;
    for ( i=1; i<7; i++ ) daynames+=s+locale.dayName( i, QLocale::LongFormat )+sep;
    daynames.chop( 1 );

    QString format=locale.dateFormat( QLocale::ShortFormat );
    format.replace( QStringLiteral( "yyyy" ), QStringLiteral( "yy" ) );
    format.replace( QStringLiteral( "MMM" ), QStringLiteral( "M" ) );
    format.replace( QStringLiteral( "MM" ), QStringLiteral( "M" ) );
    QByteArray dayOfWeek;

#if QT_VERSION >= 0x040800
    dayOfWeek=QByteArray::number( locale.firstDayOfWeek()==Qt::Sunday ? 0 :
            static_cast<int>(locale.firstDayOfWeek()) );
#else
    dayOfWeek="1";
#endif

    _jquery+="\t$.datepicker.setDefaults({\n\t"
        "nextText:'',prevText:'',closeText:'',currentText:'',showWeek:true,"
        "showOtherMonths:true,selectOtherMonths:true,\n\tchangeMonth:true,changeYear:true,firstDay:"
        +dayOfWeek+",weekHeader:'',yearSuffix:'',\n\t"
        "dateFormat:'"+format.toUtf8()+"',altFormat:'yy-mm-dd',"
        "isRTL:"+(locale.textDirection()==Qt::RightToLeft?"true":"false")+","
        "showMonthAfterYear:false,yearSuffix:'',\n\t"
        "monthNamesShort:["+shortmonthnames.toUtf8()+"],\n\t"
        "monthNames:["+monthnames.toUtf8()+"],\n\tdayNames:["+daynames.toUtf8()+"],\n\t"
        "dayNamesShort:["+shortdaynames.toUtf8()+"],\n\tdayNamesMin:["
        +shortdaynames.toUtf8()+"]\n\t});\n";
    return arr;
}

QByteArray PHISHtmlBase::phiObject( const QUrl &url, const QByteArray &codebase ) const
{
    QByteArray arr="<div class=\"phicontent\" id=\"phiobjectdiv\" style=\"z-index:1;visibility:hidden\">\n"
        "\t<object id=\"phiobject\" type=\"application/x-phi\" width=\""
        +QByteArray::number( _p->width() )+"\" height=\""+QByteArray::number( _p->height() )
        +"\" codebase=\""+codebase+"\">\n\t\t<param name=\"src\" value=\""
        +url.toEncoded()+_endtag+"\t</object>\n</div>\n";
    return arr;
}

QByteArray PHISHtmlBase::postProcessPhiObjectJS() const
{
    static QByteArray arr=
        "var phiobject=phi.getElementById('phiobject');\n"
        "phiobject.titleChanged=function(t){document.title=t;};\n"
        "phiobject.phiChanged=function(c){"
        "phi.getElementById('phiobjectdiv').style.visibility='visible';"
        "phi.getElementById('phihtml').style.display='none';};\n"
        "phiobject.linkHovered=function(l){window.status=l;};\n"
        "phiobject.phi=1;\n";
    return arr;
}

QByteArray PHISHtmlBase::phiObjectExclusive( const QUrl &url, const QByteArray &codebase ) const
{
    QByteArray arr="<div class=\"phicontent\" id=\"phiobjectdiv\">\n"
        "\t<object id=\"phiobject\" type=\"application/x-phi\" width=\""
        +QByteArray::number( _p->width() )+"\" height=\""+QByteArray::number( _p->height() )
        +"\" codebase=\""+codebase+"\">\n"+
        "\t\t<param name=\"src\" value=\""+url.toEncoded()+_endtag+"\t</object>\n</div>\n";
    return arr;
}

QByteArray PHISHtmlBase::postProcessPhiObjectExclusiveJS() const
{
    static QByteArray arr="var phiobject=phi.getElementById('phiobject');\n"
        "phiobject.linkHovered=function(l){window.status=l;};\n"
        "phiobject.titleChanged=function(t){document.title=t;};\n";
    return arr;
}

QByteArray PHISHtmlBase::formAction() const
{
    QByteArray arr;
    arr.reserve( 250 );
    arr+=QByteArray::fromRawData( "\t<form id=\"phiform\" action=\"", 28 );
    arr+=_p->variant( PHIBasePage::DAction ).toByteArray();
    arr+="\" method=\"post\" enctype=\"multipart/form-data\""
        " onsubmit=\"return phi.onsubmit();\" accept-charset=\"utf-8\">\n";
    return arr;
}

QByteArray PHISHtmlBase::toLocaleFormat( const QByteArray &s ) const
{
    // translate de-de -> de_DE
    if ( s.size()==2 ) return s;
    if ( s.size()==5 ) {
        QByteArray n=s.left( 3 );
        n.replace( 2, 1, "_" );
        n.append( s.right( 2 ).toUpper() );
        return n;
    }
    return s.left( 2 );
}

QByteArray PHISHtmlBase::decimalValidatorJS( int min, int max ) const
{
    QByteArray arr;
    arr.reserve( 400 );
    arr+="$('"+_it->id()+"').change(function(){\n"
        "\tvar i=parseInt(this.value);\n"
        "\tif (i<"+QByteArray::number( min )+"||i>"+QByteArray::number( max )
        +"){this.value='';this.title='"+QByteArray::number( min )+"<=x<="
        +QByteArray::number( max )+"';this.focus();}\n"
        "\telse this.value=i;\n"
        "});\n";
    return arr;
}

QByteArray PHISHtmlBase::realValidatorJS( qreal min, qreal max, int prec ) const
{
    QByteArray arr;
    QByteArray dec;
    for ( int i=0; i<prec; i++ ) dec+='x';
    arr.reserve( 400 );
    arr+="$('"+_it->id()+"').change(function(){\n"
        "\tvar i=parseFloat(this.value);\n"
        "\tif (i<"+QByteArray::number( min )+"||i>"+QByteArray::number( max )
        +"){this.value='';this.title='"+QByteArray::number( min )+"<=x."+dec+"<="
        +QByteArray::number( max )+"';this.focus();}\n"
        "\telse this.value=i.toFixed("+QByteArray::number( prec )+");\n"
        "});\n";
    return arr;
}

QByteArray PHISHtmlBase::textValidatorJS( const QByteArray &regexp, const QByteArray &title ) const
{
    QByteArray arr;
    arr.reserve( 400 );
    arr+="$('"+_it->id()+"').change(function(){\n"
        "\tvar r="+regexp+";\n"
        "\tif(!r.test(this.value)){this.value='';this.title='"+title+"';this.focus();}\n"
        "});\n";
    return arr;
}

QByteArray PHISHtmlBase::fontStyle() const
{
    const QFont f=_it->font();
    if ( PHI::invalidFont()==f ) return QByteArray();
    QByteArray arr;
    arr.reserve( 200 );
    arr+="font-family:'"+f.family().toUtf8();
    if ( !f.lastResortFamily().isEmpty() ) arr+="','"+f.lastResortFamily().toUtf8();
    arr+="';";
    if ( f.pointSize() > -1 ) arr+="font-size:"+QByteArray::number( f.pointSize() )+"pt;";
    if ( f.italic() ) arr+=italicStyle();
    if ( f.bold() ) arr+=boldStyle();
    if ( f.underline() ) arr+=underlineStyle();
    return arr;
}

QByteArray PHISHtmlBase::positionStyle( const QRect &off ) const
{
    QByteArray arr;
    arr.reserve( 100 );
    arr+="position:absolute;left:"
        +QByteArray::number( static_cast<int>(_it->x())+off.x() )+"px;top:"
        +QByteArray::number( static_cast<int>(_it->y())+off.y() )+"px;z-index:"
        +QByteArray::number( static_cast<int>(_it->zValue()) )+";margin:0px;";
    if ( static_cast<int>(_it->height())+off.height()>0 )
        arr+="height:"+QByteArray::number( static_cast<int>(_it->height())+off.height() )+"px;";
    if ( static_cast<int>(_it->width())+off.width()>1 )
        arr+="width:"+QByteArray::number( static_cast<int>(_it->width())+off.width() )+"px;";
    return arr;
}

QByteArray PHISHtmlBase::textShadowStyle() const
{
    QByteArray arr;
    if ( _it->effect()->graphicsType()!=PHIEffect::GTShadow ) return arr;
    if ( !PHI::isTextItem( static_cast<PHI::Widget>(_it->wid() ) ) ) return arr;
    arr.reserve( 300 );
    QColor c;
    qreal xOff, yOff, radius;
    _it->effect()->shadow( c, xOff, yOff, radius );
    arr+="text-shadow:"+QByteArray::number( static_cast<int>(xOff) )+"px ";
    arr+=QByteArray::number( static_cast<int>(yOff) )+"px ";
    arr+=QByteArray::number( static_cast<int>(radius) )+"px ";
    arr+=c.name().toLatin1()+';';
    return arr;
}

QByteArray PHISHtmlBase::selectOptions( const QStringList &l ) const
{
    QStringList list( l );
    if ( list.isEmpty() ) list=_it->value().split( _it->delimiter(), QString::SkipEmptyParts );
    QString option, v;
    bool isSelected;
    QByteArray arr;
    arr.reserve( _it->valueData().length()*3 );
    foreach( option, list ) {
        PHI::getItemCheckData( option, v, isSelected );
        arr+=_indent+"\t<option value=\""+v.toUtf8()+"\"";
        if ( isSelected ) arr+=" selected=\"selected\"";
        arr+=">"+option.toUtf8()+"</option>\n";
    }
    return arr;
}

QByteArray PHISHtmlBase::createOpenGraph() const
{
    QByteArray arr;
    arr.reserve( 300 );
    QStringList list=_p->openGraph().split( QLatin1Char( '\n' ), QString::SkipEmptyParts );
    QString entry;
    int pos;
    foreach ( entry, list ) {
        pos=entry.indexOf( QLatin1Char( '=' ) );
        if ( pos==-1 ) continue;
        arr+="\t<meta property=\""+entry.mid( 0, pos ).toUtf8()+"\" content=\"";
        arr+=entry.mid( ++pos ).toUtf8()+_endtag;
    }
    return arr;
}

QByteArray PHISHtmlBase::createHtmlCode()
{
    _out.reserve( 1024*20 );
    _js.reserve( 1024*2 );
    _jquery.reserve( 1024 );
    _out+=doctype()+htmlStartTag()+"<head>\n\t<title>"
        +_p->variant( PHIBasePage::DTitle ).toByteArray()+"</title>\n";
    _out+=charset()+"\t<meta name=\"generator\" content=\""+phisEngine()+_endtag;
    if ( !_p->author().isEmpty() ) _out+="\t<meta name=\"author\" content=\""+_p->author().toUtf8()+_endtag;
    if ( !_p->company().isEmpty() ) _out+="\t<meta name=\"publisher\" content=\""+_p->company().toUtf8()+_endtag;
    if ( !_p->copyright().isEmpty() ) _out+="\t<meta name=\"rights\" content=\""+_p->copyright().toUtf8()+_endtag;
    if ( !_p->version().isEmpty() ) _out+="\t<meta name=\"page-version\" content=\""+_p->version().toUtf8()+_endtag;
    if ( !_p->description().isEmpty() ) _out+="\t<meta name=\"description\" content=\""+_p->description().toUtf8()+_endtag;
    if ( !_p->keywords().isEmpty() ) _out+="\t<meta name=\"keywords\" content=\""+_p->keywords().toUtf8()+_endtag;
    if ( !_p->openGraph().isEmpty() ) _out+=createOpenGraph();
    _out+="\t<link rel=\"stylesheet\" type=\"text/css\" href=\"phi.phis?phicss="+_p->pageId()+_endtag;
    _out+=jQueryScript();
    _out+=phiBaseScript();
    if ( _p->attributes() & PHIPage::AUsejQueryUI ) {
        if ( _p->attributes() & PHIPage::AHasCalendar ) _out+=phiCalendarScript();
        if ( _p->extensions() & PHIPage::EProgressBar ) _out+=phiProgressBarScript();
    }
    if ( _p->extensions() & PHIPage::EHasFacebookLike ) _out+=phiFacebookScript();
    if ( _p->extensions() & PHIPage::EHasTwitter ) _out+=phiTwitterScript();
    if ( _p->extensions() & PHIPage::EHasGooglePlus ) _out+=phiGooglePlusScript();
    if ( _p->extensions() & PHIPage::EHasCanvas ) _out+=phiCanvasScript();
    if ( _p->attributes() & PHIPage::AIcon ) _out+="\t<link rel=\"shortcut icon\" href="
        "\"/phi.phis?phiimg="+_p->pageId()+".ico&amp;phitmp=1"+_endtag;
    _out+="</head>\n<body";
    if ( _p->attributes() & PHIPage::ABgImage ) {
        _out+=" style=\"top:0;left:0;"
            "z-index:"+QByteArray::number( -PHI::maxZValue() )+";background-image:url(/phi.phis?phiimg="
            +_p->variant( PHIPage::DBgImageUrl ).toByteArray()+"&amp;phitmp=1);";
        PHIPage::ImageOptions opts=static_cast<PHIPage::ImageOptions>(_p->variant( PHIPage::DBgImageOptions ).toInt());
        if ( opts & PHIPage::IRepeatX ) {
            if ( opts & PHIPage::IRepeatY ) _out+="background-repeat:repeat;";
            else _out+="background-repeat:repeat-x;";
        } else if ( opts & PHIPage::IRepeatY ) {
            _out+="background-repeat:repeat-y;";
        } else _out+="background-repeat:no-repeat;";
        if ( opts & PHIPage::IFixed ) _out+="background-attachment:fixed;";
        _out+="\"";
        _jquery+="\t$(window).resize(function(){\n"
            "\t\tvar off=$('#phihtml').offset();\n"
            "\t\tif(off===undefined){$('body').css('background-image','');return;}\n"
            "\t\tvar bgx=off.left+"+QByteArray::number( _p->bgImageXOff() )+";"
            "var bgy=off.top+"+QByteArray::number( _p->bgImageYOff() )+";\n"
            "\t\t$('body').css('background-position',bgx+'px '+bgy+'px');\n"
            "\t}).resize();\n";
    }
    _out+=">\n";

    if ( !_p->session().isEmpty() ) _js+="phi.setSession('"
        +_p->variant( PHIBasePage::DSession ).toByteArray()+"');\n";
    _js+="phi.setLang('"+_lang+"');\n";
    if ( _p->extensions() & PHIPage::EHasGooglePlus ) _js+="window.___gcfg={lang:'"+_lang.left( 2 )+"'};\n";

    qint8 useObjTag=1;
    if ( !(_p->attributes() & PHIPage::APhiNoObjectTag) ) {
        if ( _req->requestKeys().contains( QStringLiteral( "phis" ) ) ) {
            qDebug( "found phis=%d in PHISRequest", _req->requestValue( QStringLiteral( "phis" ) ).toInt() );
            useObjTag=_req->requestValue( QStringLiteral( "phis" ) ).toInt();
        } else {
            // prevent WebKit in Amphibia to load the Phi plug-in
            if ( _req->agentId()==PHISRequest::Amphibia ) useObjTag=0;
        }
    } else useObjTag=0;
    if ( useObjTag!=2 ) {
        _indent="\t";

        _out+="<div id=\"phihtml\" class=\"phicontent\">\n";
        if ( _p->attributes() & PHIPage::AFormAction ) {
            _out+=formAction()+"\t<div id=\"phiformdiv\">\n";
            _indent.append( '\t' );
            if ( !_p->session().isEmpty() ) {
                _out+="\t\t<input type=\"hidden\" name=\"phisid\" id=\"phisid\" value=\""
                    +_p->variant( PHIBasePage::DSession ).toByteArray()+_endtag;
            }
            if ( !_p->itemIds().contains( QStringLiteral( "philang" ), Qt::CaseSensitive ) ) {
                _out+="\t\t<input type=\"hidden\" name=\"philang\" id=\"philang\" value=\""+_lang+_endtag;
            }
            _out+="\t\t<input type=\"hidden\" name=\"phis\" id=\"phis\" value=\"0"+_endtag;
        }
        foreach( _it, _p->items() ) {
            //qDebug( "- %s", qPrintable( it->name() ) );
            if ( _it->isChild() ) continue;
            createHtmlForItem();
        }
        if ( _p->attributes() & PHIPage::AFormAction ) {
            _indent.chop( 1 );
            _out+="\t</div>\n\t</form>\n"; // div id="phiformdiv"
        }
        _out+="</div>\n"; // div id="phihtml"
    }

    if ( useObjTag>0 ) {
        static QString phisid=QStringLiteral( "phisid" );
        QUrl url( _req->url() );
        QUrlQuery query( _req->url() );
        query.removeQueryItem( QStringLiteral( "phis" ) );
        query.addQueryItem( QStringLiteral( "phis" ), QStringLiteral( "1" ) );
        if ( _p->attributes() & PHIPage::ARequiresSession ) {
            if ( !query.hasQueryItem( phisid ) )
                query.addQueryItem( phisid, _p->variant( PHIBasePage::DSession ).toString() );
        }
        if ( _p->attributes() & PHIPage::AApplication ) {
            static QString phiapp=QStringLiteral( "phiapp" );
            if ( !query.hasQueryItem( phiapp ) ) query.addQueryItem( phiapp, QStringLiteral( "1" ) );
        }
        QByteArray path;
        path.reserve( 60 );
        path+="http://www.phisketeer.org/downloads/";
        switch ( _req->osType() ) {
        case PHISRequest::Windows: path+="phisketeer.exe"; break;
        case PHISRequest::Linux: path+="phisketeer.bin"; break;
        case PHISRequest::MacOS: path+="phisketeer.dmg"; break;
        default: path="";
        }
        url.setQuery( query );
        if ( useObjTag==2 ) _out+=phiObjectExclusive( url, path );
        else _out+=phiObject( url, path );
    }
    _out+="<script type=\"text/javascript\">\n/* <![CDATA[ */\n";
    if ( useObjTag==2 ) _out+=postProcessPhiObjectExclusiveJS();
    else if ( useObjTag==1 ) _out+=postProcessPhiObjectJS();
    _out+="jQuery(function($){\n";
    _out+=_jquery;
    //_out+="\tphi.onload();\n";
    _out+="\t$(window).on('load',phi.onload);\n";
    _out+="});\n";
    _out+=_js;
    _out+='\n';
    if ( _p->attributes() & PHIPage::AJavascript ) _out+=_p->javascript().toUtf8();
    _out+="\n/* ]]> */\n</script>\n";
    _out+="</body>\n</html>\n";
    qDebug( "HTML out %d %d", _out.capacity(), _out.size() );
    return _out;
}

void PHISHtmlBase::imageSource( const QByteArray &src1, const QByteArray &style,
    const QByteArray &iid, const QByteArray &t, const QByteArray &src2 ) const
{
    QByteArray title, id;
    if ( !t.isEmpty() ) title=" title=\""+t+'"';
    if ( !iid.isEmpty() ) id=" id=\""+iid+'"';

    if ( src2.isEmpty() ) {
        _out+=_indent+"<img src=\""+src1+"\""+id+title;
        if ( !_it->url().isEmpty() ) {
            _out+=onClickUrl()+accessKey();
            _out+=" onmouseover=\"this.style.cursor='pointer';\"";
        }
        _out+=style+"\" alt=\""+t+_endtag;
        return;
    }
    // we have a rollover
    _out+=_indent+"<img src=\""+src1+"\"";
    _out+=id+title+onClickUrl()+accessKey();
    QByteArray cursorover, cursorout;
    if ( !_it->url().isEmpty() ) {
        cursorover="this.style.cursor='pointer';";
        cursorout="this.style.cursor='auto';";
    }
    _out+=" onmouseover=\"this.src='"+src2+"';"+cursorover;
    _out+="\" onmouseout=\"this.src='"+src1+"';\"";
    _out+=style+"\" alt=\"";
    _out+=t+_endtag;
    QByteArray preload=src2;
    preload.replace( "&amp;", "&" );
    _js+="var phipld_"+_it->id()+"=new Image(); phipld_"+_it->id()+".src='"+preload+"';\n";
}

QByteArray PHISHtmlBase::onClickUrl() const
{
    Q_ASSERT(_it);
    if ( _it->url().isEmpty() ) return QByteArray();
    QByteArray arr;
    arr.reserve( 100 );
    if ( _it->urlData().startsWith( "javascript:" ) ) {
        QByteArray u=_it->urlData();
        u.remove( 0, 11 );
        u.replace( '"', "\\\"" );
        arr+=" onclick=\""+u+'"';
    } else {
        //QUrl url=PHI::createUrlForLink( _req->url(), _it->url() );
        arr+=" onclick=\"phi.href('"+_it->urlData()+"');\"";
    }
    return arr;
}

QByteArray PHISHtmlBase::alignmentStyle( quint8 align ) const
{
    if ( align==255 ) align=_it->alignment();
    QByteArray arr;
    arr.reserve( 50 );
    arr+="text-align:";
    switch ( static_cast<PHI::Alignment>(align) ) {
    case PHI::AlignRightVCenter: arr+="right;vertical-align:middle;"; break;
    case PHI::AlignRight: arr+="right;"; break;
    case PHI::AlignRightBottom: arr+="right;vertical-align:bottom;"; break;
    case PHI::AlignJustify: arr+="justify;vertical-align:top;"; break;
    case PHI::AlignLeftBottom: arr+="left;vertical-align:bottom;"; break;
    case PHI::AlignCenter: arr+="center;vertical-align:middle;"; break;
    case PHI::AlignBottomHCenter: arr+"center;vertical-align:bottom;"; break;
    case PHI::AlignTopHCenter: arr+= "center;vertical-align:top;"; break;
    case PHI::AlignLeftVCenter: arr+="left;vertical-align:middle;"; break;
    default:
        arr+="left;";
    }
    return arr;
}

QTransform PHISHtmlBase::transformation() const
{
    Q_ASSERT( _it );
    if ( _it->attributes() & PHIBaseItem::ATransform ) return _it->transform();
    QTransform t;
    if ( _it->attributes() & PHIItem::ARotate
        || _it->attributes() & PHIItem::ATranslate
        || _it->attributes() & PHIItem::AShear ) {
        QTransform trans, xrot, yrot, zrot, shear;
        trans.translate( -_it->xTranslation(), -_it->yTranslation() );
        shear.shear( _it->hShear(), _it->vShear() );
        xrot.rotate( _it->xRotation(), Qt::XAxis );
        yrot.rotate( _it->yRotation(), Qt::YAxis );
        zrot.rotate( _it->zRotation(), Qt::ZAxis );
        t=trans*shear*zrot*yrot*xrot*trans.inverted();
    }
    return t;
}

const PHIBaseItem* PHISHtmlBase::createPhisysLinkItem() const
{
    static PHIBaseItem it( PHI::PHISYS_LINK, "phisyslink" );
    QStringList ids;
    PHISImageCache cache( _req->responseRec(), _req->tmpDir() );
    ids << cache.createId();

    QImage img( QStringLiteral( ":/phi/poweredby" ) );
    img.save( _req->imgDir()+QDir::separator()+ids.at( 0 ), "PNG" );
    it.setPictureBookIds( ids );
    it.setX( 10. );
    it.setY( 10. );
    it.setWidth( 110. );
    it.setHeight( 48. );
    it.setZValue( PHI::maxZValue()+1 );
    it.setUrl( QStringLiteral( "http://www.phisys.com" ) );
    it.setToolTip( QStringLiteral( "http://www.phisys.com" ) );
    it.setVisible( true );
    it.setDisabled( false );
    return &it;
}
