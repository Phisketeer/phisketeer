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
#include "phigraphicsview.h"

PHIGraphicsView::PHIGraphicsView( PHIGraphicsScene *scene, QWidget *parent )
    : QGraphicsView( scene, parent )
{
    // qDebug( "PHIGraphicsView::PHIGraphicsView()" );
    setAcceptDrops( true );
    setInteractive( true );
    setDragMode( RubberBandDrag );
    setOptimizationFlag( DontSavePainterState );
    setOptimizationFlag( DontAdjustForAntialiasing );
    // setRenderHint( QPainter::SmoothPixmapTransform, true );
    // setRenderHint( QPainter::TextAntialiasing, true );
    // setRenderHint( QPainter::HighQualityAntialiasing, true );
    setViewportUpdateMode( SmartViewportUpdate );
    // setViewportUpdateMode( FullViewportUpdate );
    setCacheMode( CacheNone );
    setAlignment( Qt::AlignHCenter | Qt::AlignTop );
    centerOn( 0, 0 );
}
