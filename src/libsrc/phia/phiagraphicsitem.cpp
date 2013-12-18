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
#include <QGraphicsSceneMouseEvent>
#include "phiagraphicsitem.h"
#include "phidomevent.h"

bool PHIAGraphicsItem::sceneEvent( QEvent *event )
{
    return QGraphicsItem::sceneEvent( event );
}

void PHIAGraphicsItem::keyPressEvent( QKeyEvent *event )
{
    PHIDomEvent keydown( L1( "keydown" ), baseItem(), true );
    keydown.setKeyEvent( event );
    baseItem()->trigger( L1( "keydown" ), QScriptValue(), &keydown );
}

void PHIAGraphicsItem::keyReleaseEvent( QKeyEvent *event )
{
}

void PHIAGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{

}

void PHIAGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // event->ignore() or event->accept() has no effect!

}

void PHIAGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if ( baseItem()->realDisabled() ) return event->ignore();
    PHIGraphicsItem::mousePressEvent( event );
}

void PHIAGraphicsItem::mouseReleaseEvent( QGraphicsSceneMouseEvent *event )
{
    // event->ignore() or event->accept() has no effect!
    PHIDomEvent mouseup( L1( "mouseup" ), baseItem(), true );
    mouseup.setMouseEvent( event );
    baseItem()->trigger( L1( "mouseup" ), QScriptValue(), &mouseup );
    PHIGraphicsItem::mouseReleaseEvent( event );
    if ( mouseup.isDefaultPrevented() ) return;
    if ( boundingRect().contains( event->pos() ) ) {
        PHIDomEvent click( L1( "click" ), baseItem(), true );
        click.setMouseEvent( event );
        baseItem()->trigger( L1( "click" ), QScriptValue(), &click );
        if ( click.isDefaultPrevented() ) return;
        emit baseItem()->itemClicked();
    }
}

