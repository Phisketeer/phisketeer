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
#ifndef PHIGRAPHICSITEM_H
#define PHIGRAPHICSITEM_H

#include <QGraphicsProxyWidget>
#include "phigraphicsscene.h"

class PHIBaseItem;
class QEvent;

class PHIGraphicsItem : public QGraphicsProxyWidget
{
    Q_OBJECT

public:
    explicit PHIGraphicsItem( PHIBaseItem* );
    virtual ~PHIGraphicsItem();
    virtual QRectF boundingRect() const;
    PHIGraphicsScene* phiGraphicsScene() const { return qobject_cast<PHIGraphicsScene*>(scene()); }
    PHIBaseItem* phiBaseItem() const { return _it; }

protected:
    virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );
    void paintTabOrder( QPainter *painter );
    void paintSelectionItems( QPainter *painter );
    // void paintBorder( QPainter *painter );
    virtual bool sceneEvent( QEvent *event );
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual void mousePressEvent( QGraphicsSceneMouseEvent *event );
    virtual void mouseMoveEvent( QGraphicsSceneMouseEvent *event );
    virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent *event );
    virtual QPainterPath shape() const;

private:
    PHIBaseItem*  _it;

};

#endif // PHIGRAPHICSITEM_H
