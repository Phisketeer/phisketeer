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
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QPainter>
#include <QLinearGradient>
#include <QGraphicsOpacityEffect>
#include "phibaseitem.h"
#include "phieffects.h"
#include "phi.h"

PHIReflectionEffect::PHIReflectionEffect( QObject *parent )
    : QGraphicsEffect( parent ), _it( 0 )
{
    qDebug( "PHIReflectionEffect::PHIReflectionEffect()" );
}

void PHIReflectionEffect::sourceChanged( QGraphicsEffect::ChangeFlags flags )
{
    QPixmapCache::remove( _key );
    QGraphicsEffect::sourceChanged( flags );
}

void PHIReflectionEffect::draw( QPainter *p )
{
    if ( !_it ) return;
    QRectF br=_it->adjustedRect();
    QPixmap pix;
    if ( QPixmapCache::find( _key, &pix ) ) {
        p->drawPixmap( br.x(), br.y(), pix );
        return;
    }
    QImage dest( qRound(br.width()), qRound(br.height()), QImage::Format_ARGB32_Premultiplied );
    dest.fill( 0 );
    QPainter pp( &dest );
    pp.translate( -br.x(), -br.y() );
    drawSource( &pp );
    pp.end();
    QRectF r; // ignore adjustment:
    dest=PHI::reflectedImage( dest, _yOff, _size, r );
    p->drawImage( br.x(), br.y(), dest );
    _key=QPixmapCache::insert( QPixmap::fromImage( dest ) );
    if ( _it->isIdeItem() ) drawSource( p ); // draw complete grips in IDE
}

QRectF PHIReflectionEffect::boundingRectFor( const QRectF &sourceRect ) const
{
    QRectF r=sourceRect;
    r.setHeight( qMax(sourceRect.height(), sourceRect.height()+_size+_yOff) );
    return r;
}
