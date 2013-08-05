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
#ifndef PHIGRAPHICSSCENE_H
#define PHIGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QList>
#include "phi.h"

class PHIBaseItem;
class PHIBasePage;
class QMimeData;
class QGraphicsSceneDragDropEvent;
class QUndoStack;
class QTimer;

class PHIEXPORT PHIGraphicsScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit PHIGraphicsScene( QObject *parent );
    virtual ~PHIGraphicsScene();

    inline PHIBasePage* page() const { return _page; }
    inline QString fileName() const { return objectName(); }
    inline QUndoStack* undoStack() const { return _undoStack; }
    void saveAs( const QString &f );
    void save();
    void open( const QString &f );
    void setAlignment( Qt::Alignment );
    void deleteSelectedBaseItems();
    void setSelectAnimation( bool );
    QList<PHIBaseItem*> selectedBaseItems() const;

    static PHIWID widFromMimeData( const QMimeData *md );
    static QPixmap pixmapFromMimeData( const QMimeData *md );
    static QString pathFromMimeData( const QMimeData *md );
    static QUrl urlFromMimeData( const QMimeData *md );
    static QColor colorFromMimeData( const QMimeData *md );
    static qreal selectAnimationOffset() { return _selectAnimOff; }

protected:
    virtual void drawBackground( QPainter *painter, const QRectF &rect );
    virtual void drawForeground( QPainter *painter, const QRectF &rect );
    virtual void dragEnterEvent( QGraphicsSceneDragDropEvent *event );
    virtual void dragMoveEvent( QGraphicsSceneDragDropEvent *event );
    virtual void dragLeaveEvent( QGraphicsSceneDragDropEvent *event );
    virtual void dropEvent( QGraphicsSceneDragDropEvent *event );
    virtual void keyPressEvent( QKeyEvent *event );

signals:
    void cleanChanged( bool );
    void newBaseItemAdded( PHIBaseItem* );

protected slots:
    void updateSelectAnimation();
    
private:
    static qreal _selectAnimOff;
    PHIBasePage *_page;
    QUndoStack *_undoStack;
    QTimer *_selectAnimTimer;
    bool _handleOwnDragDropEvent;
};

#endif // PHIGRAPHICSSCENE_H
