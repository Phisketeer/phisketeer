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
#include <QDialog>
#include <QGraphicsView>
#include <QGraphicsScene>
#include "phicalendaritems.h"
#include "phidatasources.h"
#include "phibasepage.h"
#include "phiinputtools.h"

void PHICalendarItem::initWidget()
{
    QCalendarWidget *cw=new QCalendarWidget();
    setWidget( cw );
    setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
    if ( !isClientItem() ) return;
    connect( cw, &QCalendarWidget::selectionChanged, this, &PHICalendarItem::slotDateChanged );
    cw->setLocale( QLocale( page()->lang() ) );
}

void PHICalendarItem::ideInit()
{
    setColor( PHIPalette::WidgetText, PHIPalette::Text, page()->phiPalette().color( PHIPalette::Text ) );
    setColor( PHIPalette::WidgetBase, PHIPalette::Base, page()->phiPalette().color( PHIPalette::Base ) );
    textData()->setText( L1( "$PHISERVER[today][1]:$PHISERVER[today]:9999-12-31" ) );
    textData()->setOption( PHITextData::Parse );
}

void PHICalendarItem::setWidgetText( const QString &t )
{
    QCalendarWidget *cw=qobject_cast<QCalendarWidget*>(widget());
    if ( !cw ) return;
    QDate now, start, end;
    QString isoFmt=QString::fromLatin1( PHI::isoDateFormat() );
    QStringList dates=t.split( L1( ":" ) );
    if ( dates.count()>0 ) now=QDate::fromString( dates[0], isoFmt );
    if ( !now.isValid() ) now=QDate::currentDate();
    if ( dates.count()>1 ) start=QDate::fromString( dates[1], isoFmt );
    if ( !start.isValid() ) start=now;
    if ( dates.count()>2 ) end=QDate::fromString( dates[2], isoFmt );
    if ( !end.isValid() ) end=QDate( 9999, 12, 31 );
    cw->setDateRange( start, end );
    cw->setSelectedDate( now );
    setData( DValue, now.toString( isoFmt ) );
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

QScriptValue PHICalendarItem::val( const QScriptValue &v )
{
    if ( !v.isValid() ) return realValue();
    date( v.toString() );
    return self();
}

QScriptValue PHICalendarItem::data( const QScriptValue &v )
{
    if ( !isServerItem() ) return QScriptValue( QScriptValue::UndefinedValue );
    if ( !v.isValid() ) return realText();
    setText( v.toString() );
    return self();
}

QScriptValue PHICalendarItem::date( const QScriptValue &v )
{
    if ( !isClientItem() ) return scriptEngine()->undefinedValue();
    if ( !v.isValid() ) {
        QDate cur=QDate::fromString( realValue(), QString::fromLatin1( PHI::isoDateFormat() ) );
        return scriptEngine()->newDate( QDateTime( cur ) );
    }
    if ( v.isDate() ) {
        QDate d=v.toDateTime().date();
        setValue( d.toString( QString::fromLatin1( PHI::isoDateFormat() ) ) );
    } else setValue( v.toString() );
    return self();
}

QScriptValue PHICalendarItem::minDate( const QScriptValue &v )
{
    if ( !isClientItem() ) return QScriptValue( QScriptValue::UndefinedValue );
    QCalendarWidget *cw=qobject_cast<QCalendarWidget*>(widget());
    if ( !v.isValid() ) return scriptEngine()->newDate( QDateTime( cw->minimumDate() ) );
    if ( v.isDate() ) cw->setMinimumDate( v.toDateTime().date() );
    else cw->setMinimumDate( QDate::fromString( v.toString(), QString::fromLatin1( PHI::isoDateFormat() ) ) );
    return self();
}

QScriptValue PHICalendarItem::maxDate( const QScriptValue &v )
{
    if ( !isClientItem() ) return QScriptValue( QScriptValue::UndefinedValue );
    QCalendarWidget *cw=qobject_cast<QCalendarWidget*>(widget());
    if ( !v.isValid() ) return scriptEngine()->newDate( QDateTime( cw->maximumDate() ) );
    if ( v.isDate() ) cw->setMaximumDate( v.toDateTime().date() );
    else cw->setMaximumDate( QDate::fromString( v.toString(), QString::fromLatin1( PHI::isoDateFormat() ) ) );
    return self();
}

void PHICalendarItem::setValue( const QString &v )
{
    QDate date=QDate::fromString( v, QString::fromLatin1( PHI::isoDateFormat() ) );
    if ( !date.isValid() ) return;
    setData( DValue, date.toString( QString::fromLatin1( PHI::isoDateFormat() ) ) );
    QCalendarWidget *cw=qobject_cast<QCalendarWidget*>(widget());
    if ( !cw ) return;
    cw->setSelectedDate( date );
}

QString PHICalendarItem::realValue() const
{
    return PHIBaseItem::data( DValue ).toString();
}

void PHICalendarItem::slotDateChanged()
{
    QCalendarWidget *cw=qobject_cast<QCalendarWidget*>(widget());
    Q_ASSERT( cw );
    QDate date=cw->selectedDate();
    setData( DValue, date.toString( QString::fromLatin1( PHI::isoDateFormat() ) ) );
    if ( flags() & FHasChangeEventHandler ) trigger( L1( "change" ) );
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

void PHICalendarItem::cssStatic(const PHIRequest *req, QByteArray &out) const
{
    Q_UNUSED( req )
    out+='#'+id()+BL( " .ui-corner-all{border-radius:0px}\n" );
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
    int i;
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
    return static_cast<PHIWID>(Calendar);
}

void PHICalendarItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    QDate now, start, end;
    static QString isoFmt=QString::fromLatin1( PHI::isoDateFormat() );
    PHIByteArrayList dates=PHIBaseItem::data( DText ).toByteArray().split( ':' );
    if ( dates.count()>0 ) now=QDate::fromString( QString::fromLatin1( dates[0] ), isoFmt );
    if ( !now.isValid() ) now=QDate::currentDate();
    if ( dates.count()>1 ) start=QDate::fromString( QString::fromLatin1( dates[1] ), isoFmt );
    if ( !start.isValid() ) start=now;
    if ( dates.count()>2 ) end=QDate::fromString( QString::fromLatin1( dates[2] ), isoFmt );
    if ( !end.isValid() ) end=QDate( 9999, 12, 31 );

    htmlInitItem( script );
    script+=BL( "jQuery('#" )+id()+BL( "').datepicker({minDate:new Date(" )
        +QByteArray::number( start.year() )+','+QByteArray::number( start.month()-1 )+','
        +QByteArray::number( start.day() )+BL( "),maxDate:new Date(" )
        +QByteArray::number( end.year() )+','+QByteArray::number( end.month()-1 )+','
        +QByteArray::number( end.day() )+BL( "),altField:'#" )+id()
            +BL( "_phi'});\njQuery(function($){$('#" )+id()+BL( "').datepicker('setDate',new Date(" )
        +QByteArray::number( now.year() )+','+QByteArray::number( now.month()-1 )+','
        +QByteArray::number( now.day() )+BL( "))});\n" );
    out+=indent+BL( "<div" );
    htmlBase( req, out, script );
    out+=BL( "\"></div>\n" );
    out+=indent+BL( "<input type=\"hidden\" name=\"" )+id()+BL( "\" id=\"" )+id()+BL( "_phi\" value=\"" )
        +now.toString( isoFmt ).toLatin1()+BL( "\">\n" );
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
    QPalette pal=w->palette();
    pal.setColor( QPalette::Window, Qt::transparent );
    w->setPalette( pal );
    if ( !isClientItem() ) return;
    connect( _date, &QDateEdit::dateChanged, this, &PHIDateEditItem::slotDateChanged );
    connect( _button, &QToolButton::clicked, this, &PHIDateEditItem::slotButtonClicked );
    _date->setDisplayFormat( QLocale( page()->lang() ).dateFormat( QLocale::ShortFormat ) );
}

void PHIDateEditItem::setWidgetText( const QString &t )
{
    if ( !_date ) return;
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
    setData( DValue, now.toString( isoFmt ) );
}

void PHIDateEditItem::ideUpdateData()
{
    PHIAbstractInputItem::ideUpdateData();
    _date->setDisplayFormat( QLocale( page()->lang() ).dateFormat( QLocale::ShortFormat ) );
}

void PHIDateEditItem::setValue( const QString &v )
{
    QDate date=QDate::fromString( v, QString::fromLatin1( PHI::isoDateFormat() ) );
    if ( !date.isValid() ) return;
    setData( DValue, date.toString( QString::fromLatin1( PHI::isoDateFormat() ) ) );
    if ( !_date ) return;
    _date->setDate( date );
}

QScriptValue PHIDateEditItem::minDate( const QScriptValue &v )
{
    if ( !isClientItem() ) return QScriptValue( QScriptValue::UndefinedValue );
    if ( !v.isValid() ) return scriptEngine()->newDate( QDateTime( _date->minimumDate() ) );
    if ( v.isDate() ) _date->setMinimumDate( v.toDateTime().date() );
    else _date->setMinimumDate( QDate::fromString( v.toString(), QString::fromLatin1( PHI::isoDateFormat() ) ) );
    return self();
}

QScriptValue PHIDateEditItem::maxDate( const QScriptValue &v )
{
    if ( !isClientItem() ) return QScriptValue( QScriptValue::UndefinedValue );
    if ( !v.isValid() ) return scriptEngine()->newDate( QDateTime( _date->maximumDate() ) );
    if ( v.isDate() ) _date->setMaximumDate( v.toDateTime().date() );
    else _date->setMaximumDate( QDate::fromString( v.toString(), QString::fromLatin1( PHI::isoDateFormat() ) ) );
    return self();
}

QSizeF PHIDateEditItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    if ( which==Qt::MinimumSize ) return QSizeF( 120., 24. );
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
    PHIByteArrayList dates=PHIBaseItem::data( DText ).toByteArray().split( ':' );
    if ( dates.count()>0 ) now=QDate::fromString( QString::fromLatin1( dates[0] ), isoFmt );
    if ( !now.isValid() ) now=QDate::currentDate();
    if ( dates.count()>1 ) start=QDate::fromString( QString::fromLatin1( dates[1] ), isoFmt );
    if ( !start.isValid() ) start=now;
    if ( dates.count()>2 ) end=QDate::fromString( QString::fromLatin1( dates[2] ), isoFmt );
    if ( !end.isValid() ) end=QDate( 9999, 12, 31 );

    htmlInitItem( script );
    script+=BL( "jQuery('#" )+id()+BL( "_phit').css({cursor:'default'}).datepicker({minDate:new Date(" )
        +QByteArray::number( start.year() )+','+QByteArray::number( start.month()-1 )+','
        +QByteArray::number( start.day() )+BL( "),maxDate:new Date(" )
        +QByteArray::number( end.year() )+','+QByteArray::number( end.month()-1 )+','
        +QByteArray::number( end.day() )+BL( "),altField:'#" )+id()
        +BL( "_phi'});\njQuery(function($){$('#" )+id()+BL( "_phit').datepicker('setDate',new Date(" )
        +QByteArray::number( now.year() )+','+QByteArray::number( now.month()-1 )+','
        +QByteArray::number( now.day() )+BL( "));});\njQuery('#" )+id()
        +BL( "_phib').button({icons:{primary:'ui-icon-calendar'},text:false})"
            ".click(function(e){e.preventDefault();jQuery('#" )+id()+BL( "_phit').datepicker('show')});\n" );
    QRectF le=rect();
    le.setWidth( realWidth()-26 );
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
        +QByteArray::number( qRound(le.width()+6) )+BL( "px;width:24px;height:" )
        +QByteArray::number( qRound(realHeight()) )+BL( "px\"></button>\n" );
        //+indent+BL( "<span class=\"ui-icon ui-icon-calendar\" style=\"position:absolute;top:0;left:20px\"></span>\n" );
    out+=indent+BL( "</div>\n" );
}

void PHIDateEditItem::slotDateChanged( const QDate &date )
{
    setData( DValue, date.toString( QString::fromLatin1( PHI::isoDateFormat() ) ) );
    if ( flags() & FHasChangeEventHandler ) trigger( L1( "change" ) );
}

void PHIDateEditItem::slotButtonClicked()
{
    QGraphicsView *view=graphicsWidget()->scene()->views().first();
    Q_ASSERT( view );
    QPoint pos;
    if ( isChild() ) pos=view->mapFromScene( graphicsWidget()->mapToScene( QPointF() ) );
    else pos=view->mapFromScene( realX(), realY() );
    pos=view->mapToGlobal( pos );

    QDialog dlg( view, Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint );
    dlg.move( pos );
    dlg.setSizeGripEnabled( true );
    QVBoxLayout *vbox=new QVBoxLayout();
    vbox->setContentsMargins( 0, 0, 0, 0 );
    QCalendarWidget *cal=new QCalendarWidget();
    cal->setLocale( QLocale( page()->lang() ) );
    cal->setDateRange( _date->minimumDate(), _date->maximumDate() );
    cal->setSelectedDate( _date->date() );
    qDebug() << _date->date().toString( QString::fromLatin1( PHI::isoDateFormat() ) );
    connect( cal, &QCalendarWidget::selectionChanged, &dlg, &QDialog::reject );
    vbox->addWidget( cal );
    dlg.setLayout( vbox );
    dlg.exec();
    if ( cal->selectedDate()==_date->date() ) return;
    setValue( cal->selectedDate().toString( QString::fromLatin1( PHI::isoDateFormat() ) ) );
}
