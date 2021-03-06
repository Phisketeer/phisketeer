/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2014  Phisys AG, Switzerland
#    Copyright (C) 2012-2014  Phisketeer.org team
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
#ifndef PHIGRAPHICSSCENE_H
#define PHIGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QList>
#include "phipalette.h"
#include "phi.h"

class PHIBaseItem;
class PHIBasePage;
class QMimeData;
class QGraphicsSceneMouseEvent;

class PHIEXPORT PHIGraphicsScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit PHIGraphicsScene( QObject *parent );
    virtual ~PHIGraphicsScene();

    inline PHIBasePage* page() const { return _page; }
    inline void setPage( PHIBasePage *page ) { _page=page; }
    inline QString fileName() const { return objectName(); }
    inline QPointF dragMovePos() const { return _dragMovePos; }
    QList<PHIBaseItem*> selectedBaseItems() const;
    PHIBaseItem* focusBaseItem() const;

protected slots:
    virtual void documentSizeChanged();
    virtual void setAlignment( Qt::Alignment );

protected:
    virtual void mouseMoveEvent( QGraphicsSceneMouseEvent *event );
    virtual void drawBackground( QPainter *painter, const QRectF &rect );
    inline void setDragMovePos( const QPointF &pos ) { _dragMovePos=pos; }

signals:
    void mousePos( const QPointF& );
    void languagesChanged( const QStringList &langs );
    void pagePaletteChanged( const PHIPalette &pal );
    void pageFontChanged( const QFont &font );

private:
    PHIBasePage *_page;
    QPointF _dragMovePos;
};

#endif // PHIGRAPHICSSCENE_H
