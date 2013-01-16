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
#include <QLinearGradient>
#include <QGraphicsOpacityEffect>
#include "phieffects.h"
#include "phi.h"

PHISurfaceEffect::PHISurfaceEffect( QObject *parent )
    : QGraphicsEffect( parent )
{
    qDebug( "PHISurfaceEffect::PHISurfaceEffect()" );
}

PHISurfaceEffect::~PHISurfaceEffect()
{
    qDebug( "PHISurfaceEffect::~PHISurfaceEffect()" );
}

void PHISurfaceEffect::sourceChanged( QGraphicsEffect::ChangeFlags flags )
{
    QGraphicsEffect::sourceChanged( flags );
}

void PHISurfaceEffect::draw( QPainter *painter )
{
    /*
    QPoint offset;
    QPixmap pixmap;
    if ( sourceIsPixmap() ) {
    // No point in drawing in device coordinates (pixmap will be scaled anyways).
        pixmap=sourcePixmap( Qt::LogicalCoordinates, &offset );
    } else {
    // Draw pixmap in device coordinates to avoid pixmap scaling;
        pixmap=sourcePixmap( Qt::DeviceCoordinates, &offset );
        painter->setWorldTransform( QTransform() );
    }

    QImage img=pixmap.toImage();
    img=PHI::getSurfacedImage( img, _yOff, _size );
    painter->drawImage( offset, img );
*/
    QRectF brect=sourceBoundingRect( Qt::LogicalCoordinates );
    QImage img( static_cast<int>(brect.width()+1), static_cast<int>(brect.height()+_size+_yOff),
        QImage::Format_ARGB32_Premultiplied );
    QPainter pixPainter;
    pixPainter.begin( &img );

    pixPainter.setRenderHints( painter->renderHints() );
    pixPainter.setCompositionMode( QPainter::CompositionMode_Clear );
    pixPainter.fillRect( 0., 0., brect.width()+1., brect.height()+_size+_yOff+1, Qt::transparent );
    pixPainter.setCompositionMode( QPainter::CompositionMode_SourceOver );
    drawSource( &pixPainter );

    QTransform t;
    t.rotate( 180., Qt::XAxis );
    t.translate( 0., (-brect.height()*2.)-_yOff+1. );
    pixPainter.setTransform( t );
    drawSource( &pixPainter );

    pixPainter.resetTransform();
    pixPainter.translate( 0., brect.height()+_yOff );
    QLinearGradient gradient( 0., 0., 0., 1.0 );
    gradient.setColorAt( 0., QColor( 0, 0, 0, 220 ) );
    gradient.setColorAt( 0.78, QColor( 0, 0, 0, 30 ) );
    gradient.setColorAt( 1., Qt::transparent );
    gradient.setCoordinateMode( QGradient::ObjectBoundingMode );

    pixPainter.setCompositionMode( QPainter::CompositionMode_DestinationIn );
    pixPainter.fillRect( 0., 0., brect.width()+1, _size, gradient );
    pixPainter.end();
    painter->drawImage( 0, 0, img );
}

QRectF PHISurfaceEffect::boundingRectFor( const QRectF &sourceRect ) const
{
    QRectF r=sourceRect;
    //qDebug( "device %f %f %f %f", r.x(), r.y(), r.width(), r.height() );
    //QRectF r2=sourceBoundingRect();
    //qDebug( "device %f %f %f %f", r2.x(), r2.y(), r2.width(), r2.height() );
    r.setHeight( sourceRect.height()+2*_size+2*_yOff );
    return r;
}
