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
#include <QApplication>
#include <QFont>
#include "phigraphicsitem.h"
#include "phibaseitem.h"
#include "phiundo.h"

PHIGraphicsItem::Mode PHIGraphicsItem::_mode=PHIGraphicsItem::Normal;
quint8 PHIGraphicsItem::_gripSize=8;
QColor PHIGraphicsItem::_selectionColor=QColor( Qt::red );
bool PHIGraphicsItem::_showItemInfo=true;
bool PHIGraphicsItem::_newlySelected=true;
PHI::Origin PHIGraphicsItem::_currentGrip=PHI::Invalid;
QPointF PHIGraphicsItem::_startPos=QPointF();
QHash <PHIBaseItem*, QRectF> PHIGraphicsItem::_selectedItemRects;
const QList <Qt::CursorShape> PHIGraphicsItem::_cursorShapes={
    Qt::ArrowCursor,
    Qt::SizeFDiagCursor, Qt::SizeVerCursor, Qt::SizeBDiagCursor,
    Qt::SizeHorCursor, Qt::ArrowCursor, Qt::SizeHorCursor,
    Qt::SizeBDiagCursor, Qt::SizeVerCursor, Qt::SizeFDiagCursor
};
const QList <PHI::Origin> PHIGraphicsItem::_gripOrigins={
    PHI::TopLeft, PHI::TopMid, PHI::TopRight, PHI::MidLeft, PHI::MidMid,
    PHI::MidRight, PHI::BottomLeft, PHI::BottomMid, PHI::BottomRight
};

PHIGraphicsItem::PHIGraphicsItem( PHIBaseItem *it )
    : QGraphicsProxyWidget(), _it( it )
{
    Q_ASSERT( it );
    if ( it->isIdeItem() ) {
        setFlag( ItemIsFocusable, true );
        setFlag( ItemIsSelectable, true );
        setFlag( ItemIsMovable, true );
        setAcceptDrops( true );
        setAcceptHoverEvents( true );
    } else if ( it->isClientItem() ) {
        setFlag( ItemIsFocusable, it->isFocusable() );
        setFlag( ItemIsSelectable, true );
        setFlag( ItemIsMovable, false );
        setAcceptDrops( it->isDroppable() );
        setAcceptHoverEvents( true );
    } else if ( it->isTemplateItem() ) {
        setFlag( ItemIsFocusable, false );
        setFlag( ItemIsSelectable, false );
        setFlag( ItemIsMovable, false );
        setAcceptDrops( false );
        setAcceptHoverEvents( false );
    }
    setCacheMode( ItemCoordinateCache );
}

PHIGraphicsItem::~PHIGraphicsItem()
{
}

QPainterPath PHIGraphicsItem::shape() const
{
    QPainterPath path;
    if ( _it->isIdeItem() ) path.addRect( boundingRect() );
    else path.addRect( _it->rect() );
    return path;
}

bool PHIGraphicsItem::sceneEvent( QEvent *e )
{
    if ( _it->sceneEvent( e ) ) return true;
    if ( _it->isIdeItem() ) {
        switch( e->type() ) {
        case QEvent::GraphicsSceneMousePress:
            ideMousePressEvent( static_cast<QGraphicsSceneMouseEvent*>(e) );
            return true;
        case QEvent::GraphicsSceneMouseMove:
            ideMouseMoveEvent( static_cast<QGraphicsSceneMouseEvent*>(e) );
            return true;
        case QEvent::GraphicsSceneMouseRelease:
            ideMouseReleaseEvent( static_cast<QGraphicsSceneMouseEvent*>(e) );
            return true;
        case QEvent::GraphicsSceneHoverEnter:
            ideHoverEnterEvent( static_cast<QGraphicsSceneHoverEvent*>(e) );
            return true;
        case QEvent::GraphicsSceneHoverLeave:
            ideHoverLeaveEvent( static_cast<QGraphicsSceneHoverEvent*>(e) );
            return true;
        case QEvent::GraphicsSceneHoverMove:
            ideHoverMoveEvent( static_cast<QGraphicsSceneHoverEvent*>(e) );
            return true;
        default:
            return QGraphicsProxyWidget::sceneEvent( e );
        }
    }
    return QGraphicsProxyWidget::sceneEvent( e );
}

void PHIGraphicsItem::ideMousePressEvent( QGraphicsSceneMouseEvent *e )
{
    if ( !_it->isIdeItem() ) return e->ignore();
    _newlySelected=!isSelected();
    _selectedItemRects.clear();
    _currentGrip=PHI::Invalid;
    foreach ( PHI::Origin o, _gripOrigins ) {
        if ( gripRect( o ).contains( e->pos() ) ) {
            _currentGrip=o;
            scene()->clearSelection();
            _selectedItemRects.insert( _it, QRectF( _it->pos(), _it->size() ) );
        }
    }
    QGraphicsItem::mousePressEvent( e );
    if ( _currentGrip==PHI::Invalid ) { // move operation
        QList <PHIBaseItem*> list=phiGraphicsScene()->selectedBaseItems();
        foreach( PHIBaseItem *it, list ) {
            _selectedItemRects.insert( it, QRectF( it->pos(), it->size() ) );
        }
        _startPos=pos();
    }
}

void PHIGraphicsItem::ideMouseMoveEvent( QGraphicsSceneMouseEvent *e )
{
    if ( _currentGrip==PHI::Invalid ) { // move operation
        QGraphicsItem::mouseMoveEvent( e );
    } else { // Resize or Transform operation

    }
}

void PHIGraphicsItem::ideMouseReleaseEvent( QGraphicsSceneMouseEvent *e )
{
    if ( _currentGrip==PHI::Invalid ) { // move or click operation
        QGraphicsItem::mouseReleaseEvent( e );
        if ( !_newlySelected && e->scenePos()==e->buttonDownScenePos( Qt::LeftButton ) ) {
            if ( _mode==Normal ) _mode=Transform;
            else if ( _mode==Transform ) _mode=Normal;
            return;
        } else { // move
            _it->setPos( phiGraphicsScene()->gridPos( pos() ) );
            QPointF delta=phiGraphicsScene()->gridPos( pos() )-_startPos;
            if ( _selectedItemRects.count()>1 ) _it->undoStack()->beginMacro( tr( "Move items" ) );
            foreach ( PHIBaseItem *it, _selectedItemRects.keys() ) {
                QRectF old=_selectedItemRects.value( it );
                it->setPos( old.topLeft()+delta );
                if ( delta==QPointF() ) continue;
                it->undoStack()->push( new PHIUndoMove( it, old.topLeft() ) );
            }
            if ( _selectedItemRects.count()>1 ) _it->undoStack()->endMacro();
        }
    } else { // Resize or Transform operation
        if ( e->scenePos()==e->buttonDownScenePos( Qt::LeftButton ) ) {
            _it->undoStack()->push( new PHIUndoProperty( _it,
                PHIUndoCommand::TransformPos, static_cast<quint8>(_currentGrip) ) );
        }
    }
}

void PHIGraphicsItem::ideHoverEnterEvent( QGraphicsSceneHoverEvent *e )
{
    setCursor( Qt::ArrowCursor );
    qDebug() << "enter";
}

void PHIGraphicsItem::ideHoverMoveEvent( QGraphicsSceneHoverEvent *e )
{
    if ( !isSelected() ) return;
    foreach( PHI::Origin o, _gripOrigins ) {
        if ( gripRect( o ).contains( e->pos() ) ) {
            if ( !_it->widthIsChangeable() ) {
                if ( o!=PHI::MidLeft && o!=PHI::MidRight ) continue;
            }
            if ( !_it->heightIsChangeable() ) {
                if ( o!=PHI::TopMid && o!=PHI::BottomMid ) continue;
            }
            setCursor( _cursorShapes.at( static_cast<int>(o) ) );
            return;
        }
    }
    setCursor( Qt::ArrowCursor );
}

void PHIGraphicsItem::ideHoverLeaveEvent( QGraphicsSceneHoverEvent *e )
{
    qDebug() << "leave";
    unsetCursor();
}

QRectF PHIGraphicsItem::boundingRect() const
{
    QRectF r=_it->boundingRect();
    if ( _it->isIdeItem() ) {
        r.adjust( -10, -20, 10, 10 );
        //if ( _showItemInfo ) if ( r.width()<100 ) r.setWidth( 100 );
    }
    return r;
}

QSizeF PHIGraphicsItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    QSizeF size=_it->sizeHint( which, constraint );
    if ( size.isValid() ) return size;
    return QGraphicsProxyWidget::sizeHint( which, constraint );
}

void PHIGraphicsItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    if ( !isVisible() ) return;
    painter->save();
    _it->paint( painter, option, widget );
    painter->setOpacity( 1. );
    if ( _mode==TabOrder ) paintTabOrder( painter );
    if ( !_it->isIdeItem() ) {
        painter->restore();
        return;
    }
    if ( isSelected() ) paintSelectionItems( painter );
    if ( isSelected() && _showItemInfo ) paintItemInfo( painter );
    painter->restore();
}

void PHIGraphicsItem::paintTabOrder( QPainter *painter )
{

}

void PHIGraphicsItem::paintSelectionItems( QPainter *p )
{
    p->setRenderHint( QPainter::Antialiasing, false );
    QPen pen;
    pen.setStyle( Qt::DashLine );
    pen.setCapStyle( Qt::RoundCap );
    pen.setDashOffset( phiGraphicsScene()->selectAnimationOffset() );
    pen.setColor( _selectionColor );
    pen.setWidth( 1 );
    p->setPen( pen );
    p->drawRoundedRect( 0, 0, _it->width(), _it->height(), 5., 5. );
    if ( _mode==Transform ) paintRoundedGrips( p );
    else paintRectGrips( p );
}

void PHIGraphicsItem::paintRoundedGrips( QPainter *p )
{
    QPen pen;
    pen.setColor( Qt::black );
    pen.setStyle( Qt::SolidLine );
    p->setPen( pen );
    QBrush brush( _selectionColor );
    p->setBrush( brush );
    p->setRenderHint( QPainter::Antialiasing );
    foreach( PHI::Origin o, _gripOrigins ) {
        p->drawEllipse( gripRect( o ) );
    }
    p->setBrush( Qt::blue );
    p->drawEllipse( gripRect( static_cast<PHI::Origin>(_it->transformPos()) ) );
}

void PHIGraphicsItem::paintRectGrips( QPainter *p )
{
    QPen pen;
    pen.setColor( Qt::black );
    pen.setStyle( Qt::SolidLine );
    p->setPen( pen );
    QBrush brush( _selectionColor );
    p->setBrush( brush );
    p->setRenderHint( QPainter::Antialiasing, false );
    foreach ( PHI::Origin o, _gripOrigins ) {
        p->drawRect( gripRect( o ) );
    }
    p->setBrush( Qt::blue );
    p->drawRect( gripRect( static_cast<PHI::Origin>(_it->transformPos()) ) );
}

void PHIGraphicsItem::paintItemInfo( QPainter *p )
{
    p->setRenderHint( QPainter::Antialiasing, false );
    p->setRenderHint( QPainter::TextAntialiasing, true );
    QFont f=qApp->font();
    f.setPointSizeF( PHI::adjustedFontSize( 8 ) );
    p->setFont( f );
    QString info;
    if ( _currentGrip!=PHI::Invalid ) {
        info=QString( L1( "%1|%2 %3" ) ).arg( _it->width() ).arg( _it->height() ).arg( _it->name() );
    } else {
        info=QString( L1( "%1|%2 %3" ) ).arg( x() ).arg( y() ).arg( _it->name() );
    }
    QRectF r=_it->rect();
    QFontMetricsF fm( p->font() );
    QRectF bgr=fm.boundingRect( info );
    bgr.adjust( -1, 0, 3, 1 );
    QPoint lc( r.x(), r.y()-10 );
    bgr.translate( lc );
    QBrush bg( Qt::black );
    p->fillRect( bgr, bg );
    p->setPen( Qt::white );
    p->drawText( QPointF( r.x(), r.y()-10 ), info );
}

QRectF PHIGraphicsItem::gripRect( PHI::Origin o ) const
{
    QRectF d;
    qreal s=static_cast<qreal>( _gripSize );
    qreal s2=s/2.;
    switch ( o ) {
    case PHI::TopLeft: d.setTopLeft( QPointF( -s2, -s2 ) ); break;
    case PHI::TopMid: d.setTopLeft( QPointF( _it->width()/2.-s2, -s2 ) ); break;
    case PHI::TopRight: d.setTopLeft( QPointF( _it->width()-s2, -s2 ) ); break;
    case PHI::MidLeft: d.setTopLeft( QPointF( -s2, _it->height()/2.-s2 ) ); break;
    case PHI::MidMid: d.setTopLeft( QPointF( _it->width()/2.-s2, _it->height()/2.-s2 ) ); break;
    case PHI::MidRight: d.setTopLeft( QPointF( _it->width()-s2, _it->height()/2.-s2 ) ); break;
    case PHI::BottomLeft: d.setTopLeft( QPointF( -s2, _it->height()-s2 ) ); break;
    case PHI::BottomMid: d.setTopLeft( QPointF( _it->width()/2.-s2, _it->height()-s2 ) ); break;
    case PHI::BottomRight: d.setTopLeft( QPointF( _it->width()-s2, _it->height()-s2 ) ); break;
    default:; // custom:
        QPointF origin=_it->transformOrigin();
        d.setLeft( origin.x()-s2 );
        d.setTop( origin.y()-s2 );
    }
    d.setWidth( s );
    d.setHeight( s );
    return d;
}
