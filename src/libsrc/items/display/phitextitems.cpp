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
#include <QLabel>
#include "phitextitems.h"
#include "phidatasources.h"
#include "phibasepage.h"
#include "phi.h"

PHILabelItem::PHILabelItem( const PHIBaseItemPrivate &p )
    : PHIAbstractTextItem( p )
{
    setWidget( new QLabel() );
    setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed ) );
}

void PHILabelItem::initIDE()
{
    textData()->setText( L1( "Label" ) );
    setColor( PHIPalette::WidgetBase, PHIPalette::Window, page()->phiPalette().color( PHIPalette::Window ) );
    setColor( PHIPalette::WidgetText, PHIPalette::WindowText, page()->phiPalette().color( PHIPalette::WindowText ) );
}

void PHILabelItem::setWidgetText( const QString &t )
{
    QLabel *l=qobject_cast<QLabel*>(widget());
    l->setText( t );
}

void PHILabelItem::setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col )
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
