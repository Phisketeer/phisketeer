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
#include <QPainter>
#include <QLabel>
#include "phimiscitems.h"
#include "phirequest.h"

void PHIHiddenItem::ideInit()
{
    textData()->setText( QString() );
}

QRectF PHIHiddenItem::boundingRect() const
{
    return rect();
}

QSizeF PHIHiddenItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    Q_UNUSED( which )
    Q_UNUSED( constraint )
    return QSizeF( 150., 16. );
}

bool PHIHiddenItem::paint( QPainter *painter, const QRectF &exposed )
{
    Q_UNUSED( exposed )
    if ( !isIdeItem() ) return true;
    painter->setRenderHint( QPainter::TextAntialiasing );
    painter->setPen( QPen( Qt::darkBlue, 1., Qt::SolidLine ) );
    painter->setOpacity( .5 );
    painter->fillRect( rect(), QBrush( QColor( Qt::blue ).lighter( 180 ) ) );
    painter->drawRect( rect() );
    QFont f=page()->font();
    f.setPointSizeF( PHI::adjustedFontSize( 10 ) );
    painter->setFont( f );
    QString t=name()+QLatin1Char( '=' )+realText();
    painter->drawText( rect(), t, QTextOption( Qt::AlignCenter ) );
    return true;
}

void PHIHiddenItem::squeeze()
{
    setOpacity( 1. );
    removeData( DFont );
    setXRotation( 0 );
    setYRotation( 0 );
    setZRotation( 0 );
    setHSkew( 0 );
    setVSkew( 0 );
    PHIAbstractInputItem::squeeze();
}

void PHIHiddenItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    Q_UNUSED( req )
    htmlInitItem( script );
    out+=indent+BL( "<input type=\"hidden\" id=\"" )+id()+BL( "\" value=\"" )
        +data( DText ).toByteArray()+BL( "\" name=\"" )+id()+BL( "\">\n" );
}
