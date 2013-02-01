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
#include <QAction>
#include "gwidgetitem.h"
#include "phiawebview.h"
#include "phiaitem.h"
#include "phia.h"
#include "phi.h"

GWidgetItem::GWidgetItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : QGraphicsProxyWidget( parent ), _checked( false ), _readOnly( false ), _item( it )
{
    Q_ASSERT( it );
    setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    setMinimumSize( 0, 0 );
    //setAcceptHoverEvents( true );
    if ( PHI::isTabOrderItem( static_cast<PHI::Widget>(it->wid()) ) ) setFlag( ItemIsFocusable, true );
    else setFlag( ItemIsFocusable, false );
    resize( w, h );
    connect( this, SIGNAL( onClick( QGraphicsSceneMouseEvent* ) ),
        it, SLOT( slotOnClick( QGraphicsSceneMouseEvent* ) ) );
    connect( this, SIGNAL( onDblClick( QGraphicsSceneMouseEvent* ) ),
        it, SLOT( slotOnDblClick( QGraphicsSceneMouseEvent* ) ) );
    connect( this, SIGNAL( onFocus( QFocusEvent* ) ), it, SLOT( slotOnFocus( QFocusEvent* ) ) );
    connect( this, SIGNAL( onBlur( QFocusEvent* ) ), it, SLOT( slotOnBlur( QFocusEvent* ) ) );
    connect( this, SIGNAL( onMouseUp( QGraphicsSceneMouseEvent* ) ),
        it, SLOT( slotOnMouseUp( QGraphicsSceneMouseEvent* ) ) );
    connect( this, SIGNAL( onMouseDown( QGraphicsSceneMouseEvent* ) ),
        it, SLOT( slotOnMouseDown( QGraphicsSceneMouseEvent* ) ) );
    connect( this, SIGNAL( onMouseOver( QGraphicsSceneHoverEvent* ) ),
        it, SLOT( slotOnMouseOver( QGraphicsSceneHoverEvent* ) ) );
    connect( this, SIGNAL( onMouseOut( QGraphicsSceneHoverEvent* ) ),
        it, SLOT( slotOnMouseOut( QGraphicsSceneHoverEvent* ) ) );
    connect( this, SIGNAL( onKeyPress( QKeyEvent* ) ), it, SLOT( slotOnKeyPress( QKeyEvent* ) ) );
    connect( this, SIGNAL( onKeyDown( QKeyEvent* ) ), it, SLOT( slotOnKeyDown( QKeyEvent* ) ) );
    connect( this, SIGNAL( onKeyUp( QKeyEvent* ) ), it, SLOT( slotOnKeyUp( QKeyEvent* ) ) );
    connect( this, SIGNAL( onMouseMove( QGraphicsSceneHoverEvent* ) ),
        it, SLOT( slotOnMouseMove( QGraphicsSceneHoverEvent* ) ) );
    connect( this, SIGNAL( onChange() ), it, SLOT( slotOnChange() ) );
}

GWidgetItem::~GWidgetItem()
{
}

void GWidgetItem::setWidth( qreal w )
{
    setMinimumWidth( w );
    setMaximumWidth( w );
}

void GWidgetItem::setHeight( qreal h )
{
    setMinimumHeight( h );
    setMaximumHeight( h );
}

quint16 GWidgetItem::maxLength() const
{
    return PHI::maxLength();
}

void GWidgetItem::setStyleSheet( const QString &s )
{
    if ( widget() ) widget()->setStyleSheet( s );
}

QString GWidgetItem::styleSheet() const
{
    if ( widget() ) return widget()->styleSheet();
    return QString();
}

void GWidgetItem::setAccessKey( const QString &k )
{
    if ( !k.length()==1 ) return;
    QAction *act=0;
    QList<QAction*> list=actions();
    if ( !list.isEmpty() ) act=list.first();
    else act=new QAction( this );
    act->setShortcut( QKeySequence( "Shift+Alt+"+k ) );
    addAction( act );
}

QColor GWidgetItem::color() const
{
    return palette().windowText().color();
}

QColor GWidgetItem::outlineColor() const
{
    return palette().window().color();
}

void GWidgetItem::setColor( const QColor &c )
{
    QPalette p=palette();
    p.setColor( QPalette::WindowText, c );
    setPalette( p );
}

void GWidgetItem::setOutlineColor( const QColor &c )
{
    QPalette p=palette();
    p.setColor( QPalette::Window, c );
    setPalette( p );
}

void GWidgetItem::mouseReleaseEvent( QGraphicsSceneMouseEvent *e )
{
    QGraphicsProxyWidget::mouseReleaseEvent( e );
    emit onMouseUp( e );
    if ( boundingRect().contains( e->pos() ) ) emit onClick( e );
}

void GWidgetItem::mouseDoubleClickEvent( QGraphicsSceneMouseEvent *e )
{
    QGraphicsProxyWidget::mouseDoubleClickEvent( e );
    emit onDblClick( e );
}

void GWidgetItem::mousePressEvent( QGraphicsSceneMouseEvent *e )
{
    _item->checkForDragInMousePressEvent( e );
    qDebug( "buttons %d", int( e->buttons()) );
    QGraphicsProxyWidget::mousePressEvent( e );
    emit onMouseDown( e );
    e->accept();
}

void GWidgetItem::mouseMoveEvent( QGraphicsSceneMouseEvent *e )
{
    _item->checkForDragInMouseMoveEvent( e );
    QGraphicsProxyWidget::mouseMoveEvent( e );
    //emit onMouseMove( e );
}

void GWidgetItem::focusInEvent( QFocusEvent *e )
{
    QGraphicsProxyWidget::focusInEvent( e );
    emit onFocus( e );
}

void GWidgetItem::focusOutEvent( QFocusEvent *e )
{
    QGraphicsProxyWidget::focusOutEvent( e );
    emit onBlur( e );
}

void GWidgetItem::hoverEnterEvent( QGraphicsSceneHoverEvent *e )
{
    QGraphicsProxyWidget::hoverEnterEvent( e );
    emit onMouseOver( e );
}

void GWidgetItem::hoverLeaveEvent( QGraphicsSceneHoverEvent *e )
{
    QGraphicsProxyWidget::hoverLeaveEvent( e );
    emit onMouseOut( e );
}

void GWidgetItem::hoverMoveEvent( QGraphicsSceneHoverEvent *e )
{
    QGraphicsProxyWidget::hoverMoveEvent( e );
    emit onMouseMove( e );
}

void GWidgetItem::keyPressEvent( QKeyEvent *e )
{
    emit onKeyDown( e );
    emit onKeyPress( e );
    QGraphicsProxyWidget::keyPressEvent( e );
}

void GWidgetItem::keyReleaseEvent( QKeyEvent *e )
{
    QGraphicsProxyWidget::keyReleaseEvent( e );
    emit onKeyUp( e );
}

void GWidgetItem::dragEnterEvent( QGraphicsSceneDragDropEvent *e )
{
    QGraphicsProxyWidget::dragEnterEvent( e );
    _item->checkDragEnterEvent( e );
    qDebug() << "dragEnter " << e->isAccepted();
}

void GWidgetItem::dropEvent( QGraphicsSceneDragDropEvent *e )
{
    QGraphicsProxyWidget::dropEvent( e );
    _item->checkDropEvent( e );
}

void GWidgetItem::dragMoveEvent( QGraphicsSceneDragDropEvent *e )
{
    QGraphicsProxyWidget::dragMoveEvent( e );
    _item->checkDragMoveEvent( e );
}

void GWidgetItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    painter->setRenderHint( QPainter::Antialiasing, true );
    painter->setRenderHint( QPainter::TextAntialiasing, true );
    //painter->setRenderHint( QPainter::SmoothPixmapTransform, true );
    //painter->setRenderHint( QPainter::SmoothPixmapTransform, true );
    //    painter->setRenderHint( QPainter::HighQualityAntialiasing, true );
    //    qDebug( "not identity !!!!!!!" );
    //} else {
    //    painter->setRenderHint( QPainter::Antialiasing, false );
    //    painter->setRenderHint( QPainter::HighQualityAntialiasing, false );
    //}
    QGraphicsProxyWidget::paint( painter, option, widget );
}

QSizeF GWidgetItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    QSizeF size=QGraphicsProxyWidget::sizeHint( which, constraint );
    if ( which!=Qt::PreferredSize ) return size;
    return PHI::sizeHint( size, static_cast<PHI::Widget>(_item->wid()), font() );
}
