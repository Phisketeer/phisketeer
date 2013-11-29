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
#include <QCalendarWidget>
#include <QLocale>
#include <QToolButton>
#include <QDateEdit>
#include <QHBoxLayout>
#include "phicalendaritems.h"
#include "phidatasources.h"
#include "phibasepage.h"
#include "phiinputtools.h"

void PHICalendarItem::initWidget()
{
    setWidget( new QCalendarWidget() );
    setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
}

void PHICalendarItem::ideInit()
{
    setColor( PHIPalette::WidgetText, PHIPalette::Text, page()->phiPalette().color( PHIPalette::Text ) );
    setColor( PHIPalette::WidgetBase, PHIPalette::Base, page()->phiPalette().color( PHIPalette::Base ) );
    textData()->setText( L1( "$PHISERVER[today][1]:$PHISERVER[today]:9999-12-31" ) );
}

void PHICalendarItem::setWidgetText( const QString &t )
{
    QCalendarWidget *cw=qobject_cast<QCalendarWidget*>(widget());
    Q_ASSERT( cw );
    QDate now, start, end;
    static QString isoFmt=QString::fromLatin1( PHI::isoDateFormat() );
    QStringList dates=t.split( L1( ":" ) );
    if ( dates.count()>0 ) now=QDate::fromString( dates[0], isoFmt );
    if ( !now.isValid() ) now=QDate::currentDate();
    if ( dates.count()>1 ) start=QDate::fromString( dates[1], isoFmt );
    if ( !start.isValid() ) start=now;
    if ( dates.count()>2 ) end=QDate::fromString( dates[2], isoFmt );
    if ( !end.isValid() ) end=QDate( 9999, 12, 31 );
    cw->setDateRange( start, end );
    cw->setSelectedDate( now );
}

void PHICalendarItem::ideUpdateData()
{
    PHIAbstractInputItem::ideUpdateData();
    QCalendarWidget *cw=qobject_cast<QCalendarWidget*>(widget());
    cw->setLocale( QLocale( page()->lang() ) );
}

void PHICalendarItem::setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col )
{
    PHIAbstractInputItem::setColor( ir, cr, col );
}

QSizeF PHICalendarItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    if ( which==Qt::MinimumSize ) return QSizeF( 240., 160. );
    else if ( which==Qt::PreferredSize ) return QSizeF( 272., 160. );
    return PHIAbstractInputItem::sizeHint( which, constraint );
}

void PHICalendarItem::phisCreateData(const PHIDataParser &parser)
{
    PHIAbstractInputItem::phisCreateData( parser );
    setAdjustedRect( QRectF( 0, 0, realWidth()-6, realHeight()-4 ) );
}

PHIWID PHICalendarItem::htmlHeaderExtension( const PHIRequest *req, QByteArray &header ) const
{
    Q_UNUSED( req )
    header+=BL( "<script type=\"text/javascript\" src=\"phi.phis?j=ui-datepicker\"></script>\n" );
    return static_cast<PHIWID>(Calendar);
}

PHIWID PHICalendarItem::htmlScriptExtension( const PHIRequest *req, QByteArray &script ) const
{
    Q_UNUSED( req )
    QLocale locale( page()->lang() );
    QString monthnames, shortmonthnames, shortdaynames, daynames;
    QLatin1String sep=L1( "'," );
    QLatin1Char s=QLatin1Char( '\'' );
    register int i;
    for( i=1; i<13; i++ ) monthnames+=s+locale.monthName( i, QLocale::LongFormat )+sep;
    monthnames.chop( 1 );
    for( i=1; i<13; i++ ) {
        shortmonthnames+=s+locale.monthName( i, QLocale::LongFormat )+sep;
    }
    shortmonthnames.chop( 1 );
    shortdaynames+=s+locale.dayName( 7, QLocale::ShortFormat )+sep;
    for ( i=1; i<7; i++ ) shortdaynames+=s+locale.dayName( i, QLocale::ShortFormat )+sep;
    shortdaynames.chop( 1 );
    daynames+=s+locale.dayName( 7, QLocale::LongFormat )+sep;
    for ( i=1; i<7; i++ ) daynames+=s+locale.dayName( i, QLocale::LongFormat )+sep;
    daynames.chop( 1 );

    QString format=locale.dateFormat( QLocale::ShortFormat );
    format.replace( L1( "yyyy" ), L1( "yy" ) );
    format.replace( L1( "MMM" ), L1( "M" ) );
    format.replace( L1( "MM" ), L1( "M" ) );
    format.replace( L1( "M" ), L1( "m" ) );
    QByteArray dayOfWeek=QByteArray::number( locale.firstDayOfWeek()==Qt::Sunday ? 0 :
        static_cast<int>(locale.firstDayOfWeek()) );

    script+=BL( "jQuery(function($){\n\t$.datepicker.setDefaults({nextText:'',prevText:'',closeText:'',\n\tcurrentText:'',showWeek:true,"
        "showOtherMonths:true,selectOtherMonths:true,\n\tchangeMonth:true,changeYear:true,firstDay:" )
        +dayOfWeek+BL( ",weekHeader:'',yearSuffix:'',\n\tdateFormat:'" )+format.toUtf8()
        +BL( "',altFormat:'yy-mm-dd',isRTL:" )+(locale.textDirection()==Qt::RightToLeft?"true":"false")
        +BL( ",showMonthAfterYear:false,yearSuffix:'',\n\tmonthNamesShort:[" )+shortmonthnames.toUtf8()
        +BL( "],\n\tmonthNames:[" )+monthnames.toUtf8()+BL( "],\n\tdayNames:[" )+daynames.toUtf8()
        +BL( "],\n\tdayNamesShort:[" )+shortdaynames.toUtf8()+BL( "],\n\tdayNamesMin:[" )
        +shortdaynames.toUtf8()+BL( "]});\n});\n" );
    return wid();
}

void PHICalendarItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    out+=indent+BL( "<div" );
    htmlBase( req, out, script );
    out+=BL( "\"></div>\n" );
    QDate now, start, end;
    static QString isoFmt=QString::fromLatin1( PHI::isoDateFormat() );
    PHIByteArrayList dates=data( DText ).toByteArray().split( ':' );
    if ( dates.count()>0 ) now=QDate::fromString( QString::fromLatin1( dates[0] ), isoFmt );
    if ( !now.isValid() ) now=QDate::currentDate();
    if ( dates.count()>1 ) start=QDate::fromString( QString::fromLatin1( dates[1] ), isoFmt );
    if ( !start.isValid() ) start=now;
    if ( dates.count()>2 ) end=QDate::fromString( QString::fromLatin1( dates[2] ), isoFmt );
    if ( !end.isValid() ) end=QDate( 9999, 12, 31 );
    out+=indent+BL( "<input type=\"hidden\" name=\"" )+id()+BL( "\" id=\"" )+id()+BL( "_phi\" value=\"" )
        +now.toString( isoFmt ).toLatin1()+BL( "\">\n" );
    script+=BL( "jQuery('#" )+id()+BL( "').datepicker({minDate:new Date(" )
        +QByteArray::number( start.year() )+','+QByteArray::number( start.month()-1 )+','
        +QByteArray::number( start.day() )+BL( "),maxDate:new Date(" )
        +QByteArray::number( end.year() )+','+QByteArray::number( end.month()-1 )+','
        +QByteArray::number( end.day() )+BL( "),altField:'#" )+id()
            +BL( "_phi'});\njQuery(function($){$('#" )+id()+BL( "').datepicker('setDate',new Date(" )
        +QByteArray::number( now.year() )+','+QByteArray::number( now.month()-1 )+','
        +QByteArray::number( now.day() )+BL( "))});\n" );
    htmlInitItem( script );
}

void PHIDateEditItem::initWidget()
{
    QWidget *w=new QWidget();
    _date=new QDateEdit();
    if ( page() ) _date->setDisplayFormat( QLocale( page()->lang() ).dateFormat( QLocale::ShortFormat ) );
    _date->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum, QSizePolicy::LineEdit ) );
    _button=new QToolButton();
    _button->setMinimumHeight( 24 );
    _button->setFixedWidth( 24 );
    _button->setIconSize( QSize( 24, 24 ) );
    _button->setIcon( QIcon( L1( ":/items/calendar" ) ) );
    _button->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding, QSizePolicy::ToolButton ) );
    QHBoxLayout *l=new QHBoxLayout();
    l->setContentsMargins( 0, 0, 0, 0 );
    l->setSpacing( 3 );
    l->addWidget( _date );
    l->addWidget( _button );
    w->setLayout( l );
    w->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed, QSizePolicy::LineEdit ) );
    setWidget( w );
    setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed, QSizePolicy::LineEdit ) );
}

void PHIDateEditItem::setWidgetText( const QString &t )
{
    Q_ASSERT( _date );
    QDate now, start, end;
    QString isoFmt=QString::fromLatin1( PHI::isoDateFormat() );
    QStringList dates=t.split( L1( ":" ) );
    if ( dates.count()>0 ) now=QDate::fromString( dates[0], isoFmt );
    if ( !now.isValid() ) now=QDate::currentDate();
    if ( dates.count()>1 ) start=QDate::fromString( dates[1], isoFmt );
    if ( !start.isValid() ) start=now;
    if ( dates.count()>2 ) end=QDate::fromString( dates[2], isoFmt );
    if ( !end.isValid() ) end=QDate( 9999, 12, 31 );
    _date->setDate( now );
}

void PHIDateEditItem::ideUpdateData()
{
    PHIAbstractInputItem::ideUpdateData();
    _date->setDisplayFormat( QLocale( page()->lang() ).dateFormat( QLocale::ShortFormat ) );
}

QSizeF PHIDateEditItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    if ( which==Qt::MinimumSize ) return QSizeF( 60., 24. );
    QSizeF s=PHIAbstractInputItem::sizeHint( which, constraint );
    if ( s.height()<24. ) s.setHeight( 24. );
    return s;
}

void PHIDateEditItem::phisCreateData(const PHIDataParser &parser)
{
    PHIAbstractInputItem::phisCreateData( parser );
}

void PHIDateEditItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    QDate now, start, end;
    static QString isoFmt=QString::fromLatin1( PHI::isoDateFormat() );
    PHIByteArrayList dates=data( DText ).toByteArray().split( ':' );
    if ( dates.count()>0 ) now=QDate::fromString( QString::fromLatin1( dates[0] ), isoFmt );
    if ( !now.isValid() ) now=QDate::currentDate();
    if ( dates.count()>1 ) start=QDate::fromString( QString::fromLatin1( dates[1] ), isoFmt );
    if ( !start.isValid() ) start=now;
    if ( dates.count()>2 ) end=QDate::fromString( QString::fromLatin1( dates[2] ), isoFmt );
    if ( !end.isValid() ) end=QDate( 9999, 12, 31 );

    QRectF le=rect();
    le.setWidth( realWidth()-25 );
    le=PHIInputTools::adjustedLineEdit( req, le );
    out+=indent+BL( "<div" );
    htmlBase( req, out, script );
    out+=BL( "\">\n" )+indent+BL( "\t<input type=\"hidden\" id=\"" )+id()
        +BL( "_phi\" name=\"" )+id()+BL( "\">\n" )+indent
        +BL( "\t<input type=\"text\" class=\"phi\" id=\"" )+id()
        +BL( "_phit\" readonly=\"readonly\" style=\"width:" )
        +QByteArray::number( qRound(le.width()) )+BL( "px;height:" )
        +QByteArray::number( qRound(le.height()) )+BL( "px\">\n" )+indent
        +BL( "\t<button id=\"" )+id()+BL( "_phib\" style=\"position:absolute;top:0;left:" )
        +QByteArray::number( qRound(le.width()+5) )+BL( "px;width:24px;height:" )
        +QByteArray::number( qRound(realHeight()) )+BL( "px\"></button>\n" );
        //+indent+BL( "<span class=\"ui-icon ui-icon-calendar\" style=\"position:absolute;top:0;left:20px\"></span>\n" );
    out+=indent+BL( "</div>\n" );
    script+=BL( "jQuery('#" )+id()+BL( "_phit').css({cursor:'default'}).datepicker({minDate:new Date(" )
        +QByteArray::number( start.year() )+','+QByteArray::number( start.month()-1 )+','
        +QByteArray::number( start.day() )+BL( "),maxDate:new Date(" )
        +QByteArray::number( end.year() )+','+QByteArray::number( end.month()-1 )+','
        +QByteArray::number( end.day() )+BL( "),altField:'#" )+id()
        +BL( "_phi'});\njQuery(function($){$('#" )+id()+BL( "_phit').datepicker('setDate',new Date(" )
        +QByteArray::number( now.year() )+','+QByteArray::number( now.month()-1 )+','
        +QByteArray::number( now.day() )+BL( "));});\njQuery('#" )+id()
        +BL( "_phib').button({icons:{primary:'ui-icon-calendar'},text:false})"
            ".click(function(){jQuery('#" )+id()+BL( "_phit').datepicker('show')});\n" );
}
