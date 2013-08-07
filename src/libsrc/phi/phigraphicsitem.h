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
    enum Mode { Normal, Transform, TabOrder };
    explicit PHIGraphicsItem( PHIBaseItem* );
    virtual ~PHIGraphicsItem();
    virtual QRectF boundingRect() const;
    PHIGraphicsScene* phiGraphicsScene() const { return qobject_cast<PHIGraphicsScene*>(scene()); }
    PHIBaseItem* phiBaseItem() const { return _it; }
    static void setSelectionColor( const QColor &c ) { _selectionColor=c; }
    static void setGripSize( quint8 g ) { _gripSize=g; }
    static QColor selectionColor() { return _selectionColor; }
    static quint8 gripSize() { return _gripSize; }
    static void showItemInfo( bool show ) { _showItemInfo=show; }

protected:
    virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );
    void paintTabOrder( QPainter *painter );
    void paintSelectionItems( QPainter *painter );
    void paintRoundedGrips( QPainter *painter );
    void paintRectGrips( QPainter *painter );
    void paintItemInfo( QPainter *painter );
    QRectF gripRect( PHI::Origin o ) const;

    // void paintBorder( QPainter *painter );
    virtual bool sceneEvent( QEvent *event );
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual void ideMousePressEvent( QGraphicsSceneMouseEvent *event );
    virtual void ideMouseMoveEvent( QGraphicsSceneMouseEvent *event );
    virtual void ideMouseReleaseEvent( QGraphicsSceneMouseEvent *event );
    virtual void ideHoverEnterEvent( QGraphicsSceneHoverEvent *event );
    virtual void ideHoverLeaveEvent( QGraphicsSceneHoverEvent *event );
    virtual void ideHoverMoveEvent( QGraphicsSceneHoverEvent *event );
    virtual QPainterPath shape() const;

private:
    static quint8 _gripSize;
    static QColor _selectionColor;
    static Mode _mode;
    static bool _showItemInfo, _newlySelected;
    static const QList <Qt::CursorShape> _cursorShapes;
    static const QList <PHI::Origin> _gripOrigins;
    static PHI::Origin _currentGrip;
    static QPointF _startPos;
    static QHash <PHIBaseItem*, QRectF> _selectedItemRects;
    PHIBaseItem *_it;
};

#endif // PHIGRAPHICSITEM_H
