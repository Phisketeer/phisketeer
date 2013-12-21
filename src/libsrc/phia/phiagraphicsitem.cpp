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
#include <QKeyEvent>
#include <QFocusEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneDragDropEvent>
#include "phiagraphicsitem.h"
#include "phidomevent.h"

bool PHIAGraphicsItem::sceneEvent( QEvent *event )
{
    return QGraphicsItem::sceneEvent( event );
}

QSizeF PHIAGraphicsItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    if ( which==Qt::MinimumSize ) return QSizeF( 0, 0 );
    QSizeF size=baseItem()->sizeHint( which, constraint );
    if ( size.isValid() ) return size;
    return QGraphicsProxyWidget::sizeHint( which, constraint );
}

void PHIAGraphicsItem::keyPressEvent( QKeyEvent *event )
{
    if ( baseItem()->realDisabled() ) return event->ignore();
    if ( baseItem()->flags() & PHIBaseItem::FHasKeyEventHandler ) {
        PHIDomEvent keydown( SL( "keydown" ), baseItem(), true );
        keydown.setKeyEvent( event );
        baseItem()->trigger( SL( "keydown" ), QScriptValue(), &keydown );
        if ( keydown.isDefaultPrevented() ) return event->accept();
    }
    if ( !event->text().isEmpty() ) { // real key - not only a metakey
        if ( baseItem()->flags() & PHIBaseItem::FHasKeyEventHandler ) {
            PHIDomEvent keypress( SL( "keypress" ), baseItem(), true );
            keypress.setKeyEvent( event );
            baseItem()->trigger( SL( "keypress" ), QScriptValue(), &keypress );
            if ( keypress.isDefaultPrevented() ) return event->accept();
        }
    }
    PHIGraphicsItem::keyPressEvent( event );
}

void PHIAGraphicsItem::keyReleaseEvent( QKeyEvent *event )
{
    PHIGraphicsItem::keyReleaseEvent( event );
    if ( baseItem()->flags() & PHIBaseItem::FHasKeyEventHandler ) {
        PHIDomEvent keyup( SL( "keyup" ), baseItem(), false );
        keyup.setKeyEvent( event );
        baseItem()->trigger( SL( "keyup" ), QScriptValue(), &keyup );
    }
}

void PHIAGraphicsItem::mousePressEvent( QGraphicsSceneMouseEvent *event )
{
    if ( baseItem()->realDisabled() ) return event->ignore();
    if ( baseItem()->flags() & PHIBaseItem::FHasMouseEventHandler ) {
        PHIDomEvent mousedown( SL( "mousedown" ), baseItem(), true );
        mousedown.setMouseEvent( event );
        baseItem()->trigger( SL( "mousedown" ), QScriptValue(), &mousedown );
        if ( mousedown.isDefaultPrevented() ) return event->ignore();
    }
    baseItem()->checkForDragInMousePressEvent( event );
    PHIGraphicsItem::mousePressEvent( event );
}

void PHIAGraphicsItem::mouseDoubleClickEvent( QGraphicsSceneMouseEvent *event )
{
    // event->ignore() or event->accept() has no effect!
    if ( baseItem()->realDisabled() ) return;
    if ( baseItem()->flags() & PHIBaseItem::FHasMouseEventHandler ) {
        PHIDomEvent dblclick( SL( "dblclick" ), baseItem(), true );
        dblclick.setMouseEvent( event );
        baseItem()->trigger( SL( "dblclick" ), QScriptValue(), &dblclick );
        if ( dblclick.isDefaultPrevented() ) return;
    }
    PHIGraphicsItem::mouseDoubleClickEvent( event );
}

void PHIAGraphicsItem::mouseMoveEvent( QGraphicsSceneMouseEvent *event )
{
    // event->ignore() or event->accept() has no effect!
    baseItem()->checkForDragInMouseMoveEvent( event );
    PHIGraphicsItem::mouseMoveEvent( event );
}

void PHIAGraphicsItem::mouseReleaseEvent( QGraphicsSceneMouseEvent *event )
{
    // event->ignore() or event->accept() has no effect!
    if ( baseItem()->flags() & PHIBaseItem::FHasMouseEventHandler ) {
        PHIDomEvent mouseup( SL( "mouseup" ), baseItem(), true );
        mouseup.setMouseEvent( event );
        baseItem()->trigger( SL( "mouseup" ), QScriptValue(), &mouseup );
        PHIGraphicsItem::mouseReleaseEvent( event );
        if ( mouseup.isDefaultPrevented() ) return;
    } else PHIGraphicsItem::mouseReleaseEvent( event );

    if ( boundingRect().contains( event->pos() ) ) {
        if ( baseItem()->flags() & PHIBaseItem::FHasMouseEventHandler ) {
            PHIDomEvent click( SL( "click" ), baseItem(), true );
            click.setMouseEvent( event );
            baseItem()->trigger( SL( "click" ), QScriptValue(), &click );
            if ( click.isDefaultPrevented() ) return;
        }
        emit baseItem()->itemClicked();
    }
}

void PHIAGraphicsItem::hoverEnterEvent( QGraphicsSceneHoverEvent *event )
{
    // event->ignore() or event->accept() has no effect!
    if ( baseItem()->flags() & PHIBaseItem::FHasHoverEventHandler ) {
        PHIDomEvent mouseover( SL( "mouseover" ), baseItem(), true );
        mouseover.setHoverEvent( event );
        baseItem()->trigger( SL( "mouseover" ), QScriptValue(), &mouseover );
        if ( mouseover.isDefaultPrevented() ) return;
    }
    PHIGraphicsItem::hoverEnterEvent( event );
}

void PHIAGraphicsItem::hoverLeaveEvent( QGraphicsSceneHoverEvent *event )
{
    // event->ignore() or event->accept() has no effect!
    if ( baseItem()->flags() & PHIBaseItem::FHasHoverEventHandler ) {
        PHIDomEvent mouseout( SL( "mouseout" ), baseItem(), true );
        mouseout.setHoverEvent( event );
        baseItem()->trigger( SL( "mouseout" ), QScriptValue(), &mouseout );
        if ( mouseout.isDefaultPrevented() ) return;
    }
    PHIGraphicsItem::hoverLeaveEvent( event );
}

void PHIAGraphicsItem::hoverMoveEvent( QGraphicsSceneHoverEvent *event )
{
    // event->ignore() or event->accept() has no effect!
    if ( baseItem()->flags() & PHIBaseItem::FHasHoverEventHandler ) {
        PHIDomEvent mousemove( SL( "mousemove" ), baseItem(), true );
        mousemove.setHoverEvent( event );
        baseItem()->trigger( SL( "mousemove" ), QScriptValue(), &mousemove );
        if ( mousemove.isDefaultPrevented() ) return;
    }
    PHIGraphicsItem::hoverMoveEvent( event );
}

void PHIAGraphicsItem::focusInEvent( QFocusEvent *event )
{
    PHIDomEvent focus( SL( "focus" ), baseItem(), true );
    if ( baseItem()->flags() & PHIBaseItem::FHasFocusEventHandler ) {
        focus.setFocusEvent( event );
        baseItem()->trigger( SL( "focus" ), QScriptValue(), &focus );
        if ( focus.isDefaultPrevented() ) return event->ignore();
    }
    PHIGraphicsItem::focusInEvent( event );
}

void PHIAGraphicsItem::focusOutEvent( QFocusEvent *event )
{
    if ( baseItem()->flags() & PHIBaseItem::FHasFocusEventHandler ) {
        PHIDomEvent blur( SL( "blur" ), baseItem(), true );
        blur.setFocusEvent( event );
        baseItem()->trigger( SL( "blur" ), QScriptValue(), &blur );
        if ( blur.isDefaultPrevented() ) return event->ignore();
    }
    PHIGraphicsItem::focusOutEvent( event );
}

void PHIAGraphicsItem::dragEnterEvent( QGraphicsSceneDragDropEvent *event )
{
    baseItem()->checkDragEnterEvent( event );
}

void PHIAGraphicsItem::dragMoveEvent( QGraphicsSceneDragDropEvent *event )
{
    baseItem()->checkDragMoveEvent( event );
}

void PHIAGraphicsItem::dragLeaveEvent( QGraphicsSceneDragDropEvent *event )
{
    // event->ignore() or event->accept() has no effect!
    baseItem()->checkDragLeaveEvent( event );
}

void PHIAGraphicsItem::dropEvent( QGraphicsSceneDragDropEvent *event )
{
    // event->ignore() or event->accept() has no effect!
    baseItem()->checkDropEvent( event );
}

