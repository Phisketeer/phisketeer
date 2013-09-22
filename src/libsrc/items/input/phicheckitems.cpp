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
#include <QCheckBox>
#include <QRadioButton>
#include "phicheckitems.h"
#include "phidatasources.h"
#include "phibasepage.h"

PHICheckBoxItem::PHICheckBoxItem()
    : PHIAbstractInputItem(), _checkedData( 0 )
{
    _checkedData=new PHIBooleanData();
    setWidget( new QCheckBox() );
    textData()->setText( tr( "Checkbox label" ) );
    setBackgroundColorRole( PHIPalette::Window );
    setColorRole( PHIPalette::WindowText );
    setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed, QSizePolicy::CheckBox ) );
}

PHICheckBoxItem::~PHICheckBoxItem()
{
    delete _checkedData;
}

void PHICheckBoxItem::loadItemData( QDataStream &in, int version )
{
    PHIAbstractInputItem::loadItemData( in, version );
    in >> _checkedData;
}

void PHICheckBoxItem::saveItemData(QDataStream &out, int version )
{
    PHIAbstractInputItem::saveItemData( out, version );
    out << _checkedData;
}

QSizeF PHICheckBoxItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    QSizeF s=PHIAbstractTextItem::sizeHint( which, constraint );
    if ( which==Qt::PreferredSize ) s.setWidth( s.width()+18 );
    return s;
}

void PHICheckBoxItem::setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col )
{
    if ( ir==PHIPalette::WidgetText ) {
        setData( DColor, col );
        setColorRole( cr );
    } else if ( ir==PHIPalette::WidgetBase ) {
        setData( DBackgroundColor, col );
        setBackgroundColorRole( cr );
    } else return;
    QWidget *w=widget();
    if ( !w ) return;
    QPalette::ColorRole role=QPalette::WindowText;
    if ( ir==PHIPalette::WidgetBase ) role=QPalette::Window;
    QPalette pal=w->palette();
    pal.setColor( role, col );
    w->setPalette( pal );
}

void PHICheckBoxItem::setChecked( bool b )
{
    PHIAbstractInputItem::setChecked( b );
    QAbstractButton *ab=qobject_cast<QAbstractButton*>(widget());
    ab->setChecked( b );
}

void PHICheckBoxItem::setWidgetText( const QString &s )
{
    QAbstractButton *ab=qobject_cast<QAbstractButton*>(widget());
    ab->setText( s );
}

void PHICheckBoxItem::updateData()
{
    QAbstractButton *ab=qobject_cast<QAbstractButton*>(widget());
    if ( isIdeItem() ) {
        if ( _checkedData->unparsedStatic() )
            ab->setChecked( _checkedData->boolean() );
        else if ( checkedData()->translated() )
            ab->setChecked( _checkedData->boolean( page()->currentLang() ) );
        else ab->setChecked( false );
    } else {
        ab->setChecked( flags() & FChecked );
    }
    PHIAbstractInputItem::updateData();
}

PHIRadioButtonItem::PHIRadioButtonItem()
    : PHICheckBoxItem()
{
    setWidget( new QRadioButton() );
    textData()->setText( tr( "Radio button" ) );
    setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed, QSizePolicy::RadioButton ) );
}

QSizeF PHIRadioButtonItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    QSizeF s=PHIAbstractTextItem::sizeHint( which, constraint );
    if ( which==Qt::PreferredSize ) s.setWidth( s.width()+22 );
    return s;
}
