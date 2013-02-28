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
#ifndef PHIASCENE_H
#define PHIASCENE_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QNetworkAccessManager>

class PHIAWebView;

class PHIAScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit PHIAScene( QObject *parent );
    virtual ~PHIAScene();
    QNetworkAccessManager* networkAccessManager() const;
    inline QPointF mousePos() const { return _mousePos; }
    inline QPoint screenPos() const { return _screenPos; }
    inline quint8 pressedButtons() const { return _pressedButtons; }
    inline void setWebView( PHIAWebView *v ) { _view=v; }
    inline void setBgImage( const QImage &img ) { _bgImage=img; invalidate(); }
    inline void setBgOffset( const QPoint &p ) { _bgOffset=p; }
    inline void setBgOptions( qint32 opts ) { _bgOptions=opts; }

protected:
    virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent *event );
    virtual void mouseMoveEvent( QGraphicsSceneMouseEvent *event );
    virtual void mousePressEvent( QGraphicsSceneMouseEvent *event );
    virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent *event );
    virtual void keyPressEvent( QKeyEvent *event );
    virtual void dragEnterEvent( QGraphicsSceneDragDropEvent *event );
    virtual void dropEvent( QGraphicsSceneDragDropEvent *event );
    virtual void dragMoveEvent( QGraphicsSceneDragDropEvent *event );
    virtual void drawBackground( QPainter *painter, const QRectF &rect );

signals:
    void xPositionChanged( qreal );
    void yPositionChanged( qreal );
    void returnButtonPressed();

private:
    QPointF _mousePos;
    QPoint _screenPos;
    quint8 _pressedButtons;
    PHIAWebView *_view;
    QImage _bgImage;
    QPoint _bgOffset;
    qint32 _bgOptions;
};

class PHIAGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit PHIAGraphicsView( QWidget *parent );
    virtual ~PHIAGraphicsView();
};

#endif // PHIASCENE_H
