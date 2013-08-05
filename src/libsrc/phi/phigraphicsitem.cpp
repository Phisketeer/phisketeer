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
#include <QWidget>
#include <QGraphicsSceneEvent>
#include <QPainter>
#include <QTime>
#include "phigraphicsitem.h"
#include "phibaseitem.h"

PHIGraphicsItem::PHIGraphicsItem( PHIBaseItem *it )
    : QGraphicsProxyWidget(), _it( it )
{
    //qDebug( "PHIGraphicsItem::PHIGraphicsItem()" );
    Q_ASSERT( it );
    Q_ASSERT( it->itemType()!=PHIBaseItem::TServerItem );
    //Q_ASSERT( it->itemType()!=PHIBaseItem::ServerParserItem );

    if ( it->itemType()==PHIBaseItem::TIDEItem ) {
        setFlag( ItemIsFocusable, true );
        setFlag( ItemIsSelectable, true );
        setFlag( ItemIsMovable, true );
        setAcceptDrops( true );
    } else {
        setFlag( ItemIsFocusable, it->isFocusable() );
        setFlag( ItemIsSelectable, true );
        setFlag( ItemIsMovable, false );
        setAcceptDrops( it->isDroppable() );
    }
    setAcceptHoverEvents( true );
    setCacheMode( ItemCoordinateCache );
}

PHIGraphicsItem::~PHIGraphicsItem()
{
    //qDebug( "PHIGraphicsItem::~PHIGraphicsItem()" );
}

QPainterPath PHIGraphicsItem::shape() const
{
    return _it->shape();
}

bool PHIGraphicsItem::sceneEvent( QEvent *e )
{
    if ( _it->sceneEvent( e ) ) return true;
    return QGraphicsProxyWidget::sceneEvent( e );
}

void PHIGraphicsItem::mousePressEvent( QGraphicsSceneMouseEvent *e )
{
    QGraphicsItem::mousePressEvent( e );
}

void PHIGraphicsItem::mouseMoveEvent( QGraphicsSceneMouseEvent *e )
{
    QGraphicsItem::mouseMoveEvent( e );
}

void PHIGraphicsItem::mouseReleaseEvent( QGraphicsSceneMouseEvent *e )
{
    QGraphicsItem::mouseReleaseEvent( e );
}

QRectF PHIGraphicsItem::boundingRect() const
{
    return _it->boundingRect();
}

QSizeF PHIGraphicsItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    QSizeF size=_it->sizeHint( which, constraint );
    if ( size.isValid() ) return size;
    return QGraphicsProxyWidget::sizeHint( which, constraint );
}

void PHIGraphicsItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    _it->paint( painter, option, widget );
    if ( _it->selectMode()==PHIBaseItem::STabOrder ) paintTabOrder( painter );
    else if ( _it->isSelected() ) paintSelectionItems( painter );
}

void PHIGraphicsItem::paintTabOrder( QPainter *painter )
{

}

void PHIGraphicsItem::paintSelectionItems( QPainter *painter )
{
    painter->save();
    painter->setOpacity( 1. );
    painter->setRenderHint( QPainter::Antialiasing, false );
    QPen pen;
    pen.setStyle( Qt::DashLine );
    pen.setCapStyle( Qt::RoundCap );
    pen.setDashOffset( phiGraphicsScene()->selectAnimationOffset() );
    pen.setColor( _it->selectionColor() );
    //pen.setStyle( Qt::DotLine );
    pen.setWidth( 1 );
    painter->setPen( pen );
    painter->drawRoundedRect( 0, 0, _it->width(), _it->height(), 5., 5. );
    painter->restore();
}

/*
QRectF PHIGraphicsItem::topLeftGrip( const QRectF &r )
{
    return QRectF( r.left(), r.top(), _gripSize, _gripSize );
}

QRectF PHIGraphicsItem::topMidGrip( const QRectF &r )
{
    return QRectF( r.width()/2.0-_gripSize/2.0, r.top(), _gripSize, _gripSize );
}

QRectF PHIGraphicsItem::topRightGrip( const QRectF &r )
{
    return QRectF( r.width()-_gripSize, r.top(), _gripSize, _gripSize );
}

QRectF PHIGraphicsItem::midLeftGrip( const QRectF &r )
{
    return QRectF( r.left(), r.height()/2.0-_gripSize/2.0, _gripSize, _gripSize );
}

QRectF PHIGraphicsItem::midMidGrip( const QRectF &r )
{
    return QRectF( r.width()/2.0-_gripSize/2.0, r.height()/2.0-_gripSize/2.0, _gripSize, _gripSize );
}

QRectF PHIGraphicsItem::midRightGrip( const QRectF &r )
{
    return QRectF( r.width()-_gripSize, r.height()/2.0-_gripSize/2.0, _gripSize, _gripSize );
}

QRectF PHIGraphicsItem::bottomLeftGrip( const QRectF &r )
{
    return QRectF( r.left(), r.height()-_gripSize, _gripSize, _gripSize );
}

QRectF PHIGraphicsItem::bottomMidGrip( const QRectF &r )
{
    return QRectF( r.width()/2.0-_gripSize/2.0, r.height()-_gripSize, _gripSize, _gripSize );
}

QRectF PHIGraphicsItem::bottomRightGrip( const QRectF &r )
{
    return QRectF( r.width()-_gripSize, r.height()-_gripSize, _gripSize, _gripSize );
}
*/
