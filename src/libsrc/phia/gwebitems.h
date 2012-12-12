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
#ifndef GWEBITEMS_H
#define GWEBITEMS_H

#include <QtGui>
#include <QWebView>
#include <QGraphicsWebView>
#include "gwidgetitem.h"

class PHIAItem;

class PHIAWebPage : public QWebPage
{
    Q_OBJECT

public:
    explicit PHIAWebPage( QWidget *parent=0 );
    virtual ~PHIAWebPage();

protected slots:
    void slotUpdateGeometry( const QRect& );

protected:
    virtual QWebPage* createWindow( WebWindowType type );

private:
    QWebView *_view;
};

class GWebItem : public GWidgetItem
{
    Q_OBJECT
    Q_DISABLE_COPY( GWebItem )

public:
    GWebItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent=0 );
    virtual ~GWebItem();
    //enum { Type=UserType+PHI::HTML_DOC };
    //inline virtual int type() const { return Type; }

    virtual void setUrl( const QString& );
    inline virtual QString url() const { return _url.toString(); }

protected:
    QWebView *_view;
    QUrl _url;
};

class GExternalWebItem : public GWidgetItem
{
    Q_OBJECT
    Q_DISABLE_COPY( GExternalWebItem )

public:
    GExternalWebItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent=0 );
    virtual ~GExternalWebItem();
    virtual void setValue( const QString& )=0;
    virtual void setWidth( qreal w );
    virtual void setHeight( qreal h );
    inline virtual qreal width() const { return size().width(); }
    inline virtual qreal height() const { return size().height(); }
    inline virtual QString value() const { return _value; }

protected:
    void setContent( const QByteArray &arr );

protected:
    QWebView *_view;
    QString _value;
};

class GStaticGoogleMapItem : public GExternalWebItem
{
    Q_OBJECT
    Q_DISABLE_COPY( GStaticGoogleMapItem )

public:
    GStaticGoogleMapItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent=0 );
    virtual ~GStaticGoogleMapItem();
    virtual void setValue( const QString& );
};

class GTwitterItem : public GExternalWebItem
{
    Q_OBJECT

public:
    GTwitterItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent=0 );
    virtual ~GTwitterItem();
    virtual void setValue( const QString& );
};

class GFBLikeItem : public GExternalWebItem
{
    Q_OBJECT

public:
    GFBLikeItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent=0 );
    virtual ~GFBLikeItem();
    virtual void setValue( const QString& );
};

class GGooglePlusItem : public GExternalWebItem
{
    Q_OBJECT

public:
    GGooglePlusItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent=0 );
    virtual ~GGooglePlusItem();
    virtual void setValue( const QString& );
};

class GExternalView : public QGraphicsWebView
{
    Q_OBJECT
    Q_DISABLE_COPY( GExternalView )

public:
    GExternalView( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent=0 );
    virtual ~GExternalView();

    virtual void setValue( const QString& )=0;
    virtual void setHeight( qreal h );
    virtual void setWidth( qreal w );
    virtual void setAccessKey( const QString& );
    inline virtual qreal width() const { return size().width(); }
    inline virtual qreal height() const { return size().height(); }
    inline virtual QString value() const { return _value; }

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
    virtual void paint( QPainter*, const QStyleOptionGraphicsItem *options, QWidget *widget );

protected:
    const PHIAItem *_item;
    QString _value;
    PHIAWebPage *_page;
};

class GYouTubeItem : public GExternalView
{
    Q_OBJECT
    Q_DISABLE_COPY( GYouTubeItem )

public:
    GYouTubeItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent=0 );
    virtual ~GYouTubeItem();

    virtual void setValue( const QString& );
};

#endif // GWEBITEMS_H
