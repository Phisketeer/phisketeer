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
#ifndef GWIDGETITEM_H
#define GWIDGETITEM_H

#include <QtWidgets>
#include <QGraphicsProxyWidget>
#include <QObject>
#include "phi.h"

class PHIAItem;

class GWidgetItem : public QGraphicsProxyWidget
{
    Q_OBJECT
    Q_DISABLE_COPY( GWidgetItem )

public:
    GWidgetItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent=0 );
    virtual ~GWidgetItem();
    //enum { Type=UserType };
    //inline virtual int type() const { return Type; }
    const PHIAItem* item() const { return _item; }

    inline qreal width() const { return size().width(); }
    inline qreal height() const { return size().height(); }

    inline virtual void setValue( const QString &v ) { _value=v; }
    inline virtual QString value() const { return _value; }
    inline virtual void setLabel( const QString &l ) { _label=l; }
    inline virtual QString label() const { return _label; }
    inline virtual void setUrl( const QString& ) {;}
    inline virtual QString url() const { return QString(); }
    inline virtual bool checked() const { return _checked; }
    inline virtual void setChecked( bool c ) { _checked=c; }
    inline virtual bool readOnly() const { return _readOnly; }
    inline virtual void setReadOnly( bool ro ) { _readOnly=ro; }
    inline virtual void setDelimiter( const QString &d ) { _delimiter=d; }
    inline virtual QString delimiter() const { return _delimiter; }
    inline virtual QStringList selectedItems() const { return QStringList(); }
    inline virtual void setAlignment( quint8 ) {;}
    inline virtual quint8 alignment() const { return 0; }
    inline virtual void setPixmap( const QPixmap&, int number=-1 ) { Q_UNUSED( number ); }
    inline virtual void setRolloverTextColor( const QColor& ) {;}
    inline virtual QColor rolloverTextColor() const { return QColor(); }
    inline virtual void setRolloverBackgroundColor( const QColor& ) {;}
    inline virtual QColor rolloverBackgroundColor() const { return QColor(); }
    inline virtual void setMaxLength( quint16 ) {;}
    inline virtual void overwritePalette( const QPalette& ) {;}
    inline virtual void setValueChecked( const QString&, bool ) {;}
    inline virtual bool valueChecked( const QString& ) const { return false; }

    virtual quint16 maxLength() const;
    virtual void setWidth( qreal w );
    virtual void setHeight( qreal h );
    virtual void setAccessKey( const QString &k );
    virtual void setStyleSheet( const QString &s );
    virtual QString styleSheet() const;
    virtual QColor color() const;
    virtual QColor outlineColor() const;
    virtual void setOutlineColor( const QColor &c );
    virtual void setColor( const QColor &c );

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
    virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint) const;    

    inline QGraphicsView* view() const { return scene()->views().first(); }

protected:
    QString _value, _label, _delimiter;
    bool _checked, _readOnly;
    const PHIAItem *_item;
};

#endif // GWIDGETITEM_H
