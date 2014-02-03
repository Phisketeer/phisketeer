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
#ifndef PHIAGRAPHICSITEM_H
#define PHIAGRAPHICSITEM_H
#include "phigraphicsitem.h"
#include "phia.h"

class QEvent;
class QKeyEvent;
class QFocusEvent;
class QGraphicsSceneMouseEvent;
class QGraphicsSceneDragDropEvent;
class QGraphicsSceneHoverEvent;

class PHIAEXPORT PHIAGraphicsItem : public PHIGraphicsItem
{
    Q_OBJECT

public:
    PHIAGraphicsItem() : PHIGraphicsItem() {}
    virtual ~PHIAGraphicsItem() {}

protected:
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual bool sceneEvent( QEvent *event );

    virtual void keyPressEvent( QKeyEvent *event );
    virtual void keyReleaseEvent( QKeyEvent *event );
    virtual void mouseDoubleClickEvent( QGraphicsSceneMouseEvent *event );
    virtual void mouseMoveEvent( QGraphicsSceneMouseEvent *event );
    virtual void mousePressEvent( QGraphicsSceneMouseEvent *event );
    virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent *event );
    virtual void hoverEnterEvent( QGraphicsSceneHoverEvent *event );
    virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent *event );
    virtual void hoverMoveEvent( QGraphicsSceneHoverEvent *event );
    virtual void focusInEvent( QFocusEvent *event );
    virtual void focusOutEvent( QFocusEvent *event );
    virtual void dragEnterEvent( QGraphicsSceneDragDropEvent *event );
    virtual void dragMoveEvent( QGraphicsSceneDragDropEvent *event );
    virtual void dragLeaveEvent( QGraphicsSceneDragDropEvent *event );
    virtual void dropEvent( QGraphicsSceneDragDropEvent *event );
};

#endif // PHIAGRAPHICSITEM_H
