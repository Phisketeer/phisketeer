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

void PHIDateEditItem::ideInit()
{
    textData()->setText( L1( "$PHISERVER[today][1]:$PHISERVER[today]:9999-12-31" ) );
    setColor( PHIPalette::WidgetText, PHIPalette::Text, page()->phiPalette().color( PHIPalette::Text ) );
    setColor( PHIPalette::WidgetBase, PHIPalette::Base, page()->phiPalette().color( PHIPalette::Base ) );
}

void PHIDateEditItem::setWidgetText( const QString &t )
{
    _date->setDate( QDate::currentDate() );
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
