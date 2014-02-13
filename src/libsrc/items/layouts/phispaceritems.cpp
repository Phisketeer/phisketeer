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
#include <QPainter>
#include "phispaceritems.h"

void PHIHSpacerItem::initWidget()
{
    setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
}

QSizeF PHIHSpacerItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    if ( which==Qt::MinimumSize ) return QSizeF( 20., 20. );
    if ( which==Qt::PreferredSize ) return QSizeF( 80., 20. );
    return PHIBaseItem::sizeHint( which, constraint );
}

bool PHIHSpacerItem::paint( QPainter *painter, const QRectF &exposed )
{
    Q_UNUSED( exposed )
    if ( !isIdeItem() ) return true;
    QPen pen( Qt::blue );
    pen.setWidthF( 1. );
    painter->setPen( pen );
    painter->drawLine( 0, 0, 0, realHeight() );
    painter->drawLine( realWidth(), 0, realWidth(), realHeight() );
    pen.setWidth( 2. );
    painter->drawLine( 0, realHeight()/2., realWidth(), realHeight()/2. );
    return true;
}

void PHIHSpacerItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    Q_UNUSED( req )
    Q_UNUSED( script )
    Q_UNUSED( out )
    Q_UNUSED( indent )
}

void PHIVSpacerItem::initWidget()
{
    setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding ) );
}

QSizeF PHIVSpacerItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    if ( which==Qt::MinimumSize ) return QSizeF( 20., 20. );
    if ( which==Qt::PreferredSize ) return QSizeF( 20., 80. );
    return PHIBaseItem::sizeHint( which, constraint );
}

bool PHIVSpacerItem::paint( QPainter *painter, const QRectF &exposed )
{
    Q_UNUSED( exposed )
    if ( !isIdeItem() ) return true;
    QPen pen( Qt::blue );
    pen.setWidthF( 1. );
    painter->setPen( pen );
    painter->drawLine( 0, 0, realWidth(), 0 );
    painter->drawLine( 0, realHeight(), realWidth(), realHeight() );
    pen.setWidth( 2. );
    painter->drawLine( realWidth()/2., 0, realWidth()/2., realHeight() );
    return true;
}

void PHIVSpacerItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    Q_UNUSED( req )
    Q_UNUSED( script )
    Q_UNUSED( out )
    Q_UNUSED( indent )
}
