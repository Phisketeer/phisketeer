/*
#    Copyright (C) 2010-2012  Marius B. Schumacher
#    Copyright (C) 2011-2012  Phisys AG, Switzerland
#    Copyright (C) 2012  Phisketeer.org team
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
#ifndef GITEMS_H
#define GITEMS_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QSvgRenderer>
#include <QGraphicsSvgItem>
#include <QAbstractGraphicsShapeItem>
#include <QPixmap>
#include <QTimer>
#include "phi.h"

class PHIAItem;

class GItemHandler : public QObject
{
    Q_OBJECT

public:
    GItemHandler( const PHIAItem* );
    virtual ~GItemHandler();

signals:
    void onClick( QGraphicsSceneMouseEvent *e );
    void onDblClick( QGraphicsSceneMouseEvent *e );
    void onFocus( QFocusEvent *e );
    void onBlur( QFocusEvent *e );
    void onMouseDown( QGraphicsSceneMouseEvent *e );
    void onMouseUp( QGraphicsSceneMouseEvent *e );
    void onMouseOver( QGraphicsSceneHoverEvent *e );
    void onMouseMove( QGraphicsSceneHoverEvent *e );
    void onMouseOut( QGraphicsSceneHoverEvent *e );
    void onKeyPress( QKeyEvent *e );
    void onKeyDown( QKeyEvent *e );
    void onKeyUp( QKeyEvent *e );
    void onChange();

protected:
    const PHIAItem *_item;
};

class GGraphicsTextItem : public GItemHandler, public QGraphicsSimpleTextItem
{
public:
    GGraphicsTextItem( const QString&, const QFont&, const PHIAItem*, QGraphicsItem *parent=0 );
    virtual ~GGraphicsTextItem();
    enum { Type=UserType+PHI::GRAPH_TEXT };
    inline virtual int type() const { return Type; }

protected:
    virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );

    virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent *e );
    virtual void mouseDoubleClickEvent( QGraphicsSceneMouseEvent *e );
    virtual void mousePressEvent( QGraphicsSceneMouseEvent *e );
    virtual void mouseMoveEvent( QGraphicsSceneMouseEvent *e );
    virtual void focusInEvent( QFocusEvent *e );
    virtual void focusOutEvent( QFocusEvent *e );
    virtual void hoverEnterEvent( QGraphicsSceneHoverEvent *e );
    virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent *e );
    virtual void hoverMoveEvent( QGraphicsSceneHoverEvent *e );
    virtual void keyPressEvent( QKeyEvent *e );
    virtual void keyReleaseEvent( QKeyEvent *e );
    virtual void dragEnterEvent( QGraphicsSceneDragDropEvent *e );
    virtual void dragMoveEvent( QGraphicsSceneDragDropEvent *e );
    virtual void dropEvent( QGraphicsSceneDragDropEvent *e );

    inline QGraphicsView* view() const { return scene()->views().first(); }
};

class GTextItem : public QGraphicsTextItem
{
    Q_OBJECT

public:
    GTextItem( const QString&, const QFont&, qreal width, const PHIAItem*, QGraphicsItem *parent=0 );
    virtual ~GTextItem();
    enum { Type=UserType+PHI::TEXT };
    inline virtual int type() const { return Type; }

    virtual void setStyleSheet( const QString &s );
    virtual QString styleSheet() const;
    virtual void setValue( const QString &v );
    inline virtual QString value() const { return _value; }

protected:
    virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );

    virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent *e );
    virtual void mouseDoubleClickEvent( QGraphicsSceneMouseEvent *e );
    virtual void mousePressEvent( QGraphicsSceneMouseEvent *e );
    virtual void mouseMoveEvent( QGraphicsSceneMouseEvent *e );
    virtual void focusInEvent( QFocusEvent *e );
    virtual void focusOutEvent( QFocusEvent *e );
    virtual void hoverEnterEvent( QGraphicsSceneHoverEvent *e );
    virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent *e );
    virtual void hoverMoveEvent( QGraphicsSceneHoverEvent *e );
    virtual void keyPressEvent( QKeyEvent *e );
    virtual void keyReleaseEvent( QKeyEvent *e );
    virtual void dragEnterEvent( QGraphicsSceneDragDropEvent *e );
    virtual void dragMoveEvent( QGraphicsSceneDragDropEvent *e );
    virtual void dropEvent( QGraphicsSceneDragDropEvent *e );

signals:
    void onClick( QGraphicsSceneMouseEvent *e );
    void onDblClick( QGraphicsSceneMouseEvent *e );
    void onFocus( QFocusEvent *e );
    void onBlur( QFocusEvent *e );
    void onMouseDown( QGraphicsSceneMouseEvent *e );
    void onMouseUp( QGraphicsSceneMouseEvent *e );
    void onMouseOver( QGraphicsSceneHoverEvent *e );
    void onMouseMove( QGraphicsSceneHoverEvent *e );
    void onMouseOut( QGraphicsSceneHoverEvent *e );
    void onKeyPress( QKeyEvent *e );
    void onKeyDown( QKeyEvent *e );
    void onKeyUp( QKeyEvent *e );
    void onChange();

protected:
    QString _value;
    const PHIAItem *_item;
};

class GEllipseItem : public GItemHandler, public QGraphicsEllipseItem
{
public:
    GEllipseItem( qreal width, qreal height, qint16, qint16, const PHIAItem*, QGraphicsItem *parent=0 );
    virtual ~GEllipseItem();
    enum { Type=UserType+PHI::ELLIPSE };
    inline virtual int type() const { return Type; }

protected:
    virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );

    virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent *e );
    virtual void mouseDoubleClickEvent( QGraphicsSceneMouseEvent *e );
    virtual void mousePressEvent( QGraphicsSceneMouseEvent *e );
    virtual void mouseMoveEvent( QGraphicsSceneMouseEvent *e );
    virtual void focusInEvent( QFocusEvent *e );
    virtual void focusOutEvent( QFocusEvent *e );
    virtual void hoverEnterEvent( QGraphicsSceneHoverEvent *e );
    virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent *e );
    virtual void hoverMoveEvent( QGraphicsSceneHoverEvent *e );
    virtual void keyPressEvent( QKeyEvent *e );
    virtual void keyReleaseEvent( QKeyEvent *e );
    virtual void dragEnterEvent( QGraphicsSceneDragDropEvent *e );
    virtual void dragMoveEvent( QGraphicsSceneDragDropEvent *e );
    virtual void dropEvent( QGraphicsSceneDragDropEvent *e );

    inline QGraphicsView* view() const { return scene()->views().first(); }
};

class GRectItem : public GItemHandler, public QGraphicsRectItem
{
public:
    GRectItem( qreal width, qreal height, const PHIAItem*, QGraphicsItem *parent=0 );
    virtual ~GRectItem();
    enum { Type=UserType+PHI::RECT };
    inline virtual int type() const { return Type; }

protected:
    virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );

    virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent *e );
    virtual void mouseDoubleClickEvent( QGraphicsSceneMouseEvent *e );
    virtual void mousePressEvent( QGraphicsSceneMouseEvent *e );
    virtual void mouseMoveEvent( QGraphicsSceneMouseEvent *e );
    virtual void focusInEvent( QFocusEvent *e );
    virtual void focusOutEvent( QFocusEvent *e );
    virtual void hoverEnterEvent( QGraphicsSceneHoverEvent *e );
    virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent *e );
    virtual void hoverMoveEvent( QGraphicsSceneHoverEvent *e );
    virtual void keyPressEvent( QKeyEvent *e );
    virtual void keyReleaseEvent( QKeyEvent *e );
    virtual void dragEnterEvent( QGraphicsSceneDragDropEvent *e );
    virtual void dragMoveEvent( QGraphicsSceneDragDropEvent *e );
    virtual void dropEvent( QGraphicsSceneDragDropEvent *e );

    inline QGraphicsView* view() const { return scene()->views().first(); }
};

class GPixmapItem : public GRectItem
{
public:
    GPixmapItem( const QPixmap &pix, const PHIAItem *it, QGraphicsItem *parent=0 );
    virtual ~GPixmapItem();
    enum { Type=UserType+PHI::IMAGE };
    inline virtual int type() const { return Type; }
    inline void setPixmap( const QPixmap &pix ) { _pix=pix; update(); }
    inline QPixmap pixmap() const { return _pix; }

protected:
    virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );

private:
    QPixmap _pix;
};

class GRoundedRectItem : public GRectItem
{
public:
    GRoundedRectItem( qreal width, qreal height, const PHIAItem*, QGraphicsItem *parent=0 );
    virtual ~GRoundedRectItem();
    enum { Type=UserType+PHI::ROUNDED_RECT };
    inline virtual int type() const { return Type; }
    inline void setRadius( int r ) { _radius=r; }

protected:
    virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );

private:
    int _radius;
};

class GDiaShowItem : public GRectItem
{
    Q_OBJECT

public:
    GDiaShowItem( qreal width, qreal height, const PHIAItem*, QGraphicsItem *parent=0 );
    virtual ~GDiaShowItem();
    enum { Type=UserType+PHI::DIA_SHOW };
    inline virtual int type() const { return Type; }
    void setPixmap( const QPixmap &pix, int i=-1 );
    void setFadeTime( qint16 ft );
    void setFadeInterval( qint16 fi );
    inline void setToolTipList( const QString &toolTip ) { _titles=toolTip.split( ':', QString::KeepEmptyParts ); }
    inline QString toolTipList() const { return _titles.join( ":" ); }

protected:
    virtual void hoverEnterEvent( QGraphicsSceneHoverEvent *e );
    virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent *e );
    virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );

protected slots:
    void slotPauseTimeout();
    void slotFadeTimeout();

private:
    QHash <int, QPixmap> _images;
    qint32 _currentImage, _ft, _fi;
    qreal _step, _opac;
    QTimer *_pauseTimer, *_fadeTimer;
    QStringList _titles;
};

class GSvgItem : public QGraphicsSvgItem
{
    Q_OBJECT

public:
    GSvgItem( const PHIAItem*, qreal inital_width, qreal inital_height, QGraphicsItem *parent=0 );
    virtual ~GSvgItem();
    enum { Type=UserType+PHI::SVG };
    inline virtual int type() const { return Type; }

    virtual void setValue( const QString &src );
    inline virtual QString value() const { return _value; }
    virtual void setSize( const QSizeF &s );
    virtual void scale( qreal x, qreal y );
    inline QSizeF size() const { return _size; }
    inline QRectF boundingRect() const { return _rect; }

protected:
    virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );
    virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent *e );
    virtual void mouseDoubleClickEvent( QGraphicsSceneMouseEvent *e );
    virtual void mousePressEvent( QGraphicsSceneMouseEvent *e );
    virtual void mouseMoveEvent( QGraphicsSceneMouseEvent *e );
    virtual void focusInEvent( QFocusEvent *e );
    virtual void focusOutEvent( QFocusEvent *e );
    virtual void hoverEnterEvent( QGraphicsSceneHoverEvent *e );
    virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent *e );
    virtual void hoverMoveEvent( QGraphicsSceneHoverEvent *e );
    virtual void keyPressEvent( QKeyEvent *e );
    virtual void keyReleaseEvent( QKeyEvent *e );
    virtual void dragEnterEvent( QGraphicsSceneDragDropEvent *e );
    virtual void dragMoveEvent( QGraphicsSceneDragDropEvent *e );
    virtual void dropEvent( QGraphicsSceneDragDropEvent *e );

signals:
    void onClick( QGraphicsSceneMouseEvent *e );
    void onDblClick( QGraphicsSceneMouseEvent *e );
    void onFocus( QFocusEvent *e );
    void onBlur( QFocusEvent *e );
    void onMouseDown( QGraphicsSceneMouseEvent *e );
    void onMouseUp( QGraphicsSceneMouseEvent *e );
    void onMouseOver( QGraphicsSceneHoverEvent *e );
    void onMouseMove( QGraphicsSceneHoverEvent *e );
    void onMouseOut( QGraphicsSceneHoverEvent *e );
    void onKeyPress( QKeyEvent *e );
    void onKeyDown( QKeyEvent *e );
    void onKeyUp( QKeyEvent *e );
    void onChange();

protected:
    QString _value;
    QSvgRenderer _renderer;
    const PHIAItem *_item;
    QSizeF _size;
    QRectF _rect;
};

#endif // GITEMS_H
