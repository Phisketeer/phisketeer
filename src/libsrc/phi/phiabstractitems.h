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
#ifndef PHIABSTRACTITEMS_H
#define PHIABSTRACTITEMS_H
#include "phibaseitem.h"
#include "phipalette.h"
#include "phi.h"

class PHIBasePage;
class PHITextData;

class PHIEXPORT PHIAbstractTextItem : public PHIBaseItem
{
    Q_OBJECT
    Q_PROPERTY( QFont font READ font WRITE setFont )
    Q_PROPERTY( QColor color READ color WRITE setColor )
    Q_PROPERTY( QColor backgroundColor READ backgroundColor WRITE setBackgroundColor )

public:
    enum ItemData { DColor=-100, DBackgroundColor=-101, DTmpColor=-102, DTmpBackgroundColor=-103 };
    explicit PHIAbstractTextItem();
    virtual ~PHIAbstractTextItem();
    virtual void setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col );
    virtual QColor color( PHIPalette::ItemRole role ) const;
    virtual PHIPalette::ColorRole colorRole( PHIPalette::ItemRole role ) const;

public:
    virtual bool hasText() const { return true; }
    virtual void setText( const QString &t, const QString &lang );
    virtual QString text( const QString &lang ) const;

public slots:
    inline void setColor( const QColor &col ) { setColor( PHIPalette::WidgetText, _colorRole, col ); }
    inline void setBackgroundColor( const QColor &col ) { setColor( PHIPalette::WidgetBase, _backgroundColorRole, col ); }
    inline QColor color() const { return _variants.value( DColor, QColor( Qt::black ) ).value<QColor>(); }
    inline QColor backgroundColor() const { return _variants.value( DBackgroundColor, QColor( Qt::black ) ).value<QColor>(); }

protected:
    virtual bool isSingleLine() const { return true; }
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual void setWidgetText( const QString &t )=0;
    virtual void ideDragEnterEvent( QGraphicsSceneDragDropEvent *event );
    virtual void ideDragMoveEvent( QGraphicsSceneDragDropEvent *event );
    virtual void ideDragLeaveEvent( QGraphicsSceneDragDropEvent *event );
    virtual void ideDropEvent( QGraphicsSceneDragDropEvent *event );
    virtual PHITextData* textData() const { return _textData; }

protected:
    PHIPalette::ColorRole _colorRole, _backgroundColorRole;
    QColor _orgColor, _orgBackgroundColor;
    static qreal _dropRegion;
    PHITextData *_textData;
};

class PHIEXPORT PHIAbstractShapeItem : public PHIBaseItem
{
    Q_OBJECT
    Q_PROPERTY( quint8 line READ line WRITE setLine )
    Q_PROPERTY( quint8 pattern READ pattern WRITE setPattern )
    Q_PROPERTY( QColor color READ color WRITE setColor )
    Q_PROPERTY( QColor backgroundColor READ outlineColor WRITE setOutlineColor )
    Q_PROPERTY( QColor outlineColor READ outlineColor WRITE setOutlineColor )
    Q_PROPERTY( qreal penWidth READ penWidth WRITE setPenWidth )

public:
    enum ItemData { DLineStyle=-100, DPatternStyle=-101, DGradientType=-102,
        DColor=-103, DOutlineColor=-104, DPenWidth=-105, DTmpColor=-106,
        DTmpOutlineColor=-107, DTmpPatternStyle=-108, DTmpLineStyle=-109,
        DTmpPenWidth=-110 };
    explicit PHIAbstractShapeItem();
    virtual void setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col );
    virtual QColor color( PHIPalette::ItemRole role ) const;
    virtual PHIPalette::ColorRole colorRole( PHIPalette::ItemRole role ) const;

public slots:
    void setLine( quint8 l );
    void setPattern( quint8 p );
    void setPenWidth( qreal w );
    inline void setColor( const QColor &col ) { setColor( PHIPalette::Foreground, _colorRole, col ); }
    inline void setOutlineColor( const QColor &col ) { setColor( PHIPalette::Background, _outlineColorRole, col ); }
    inline quint8 line() const { return _variants.value( DLineStyle, 0 ).value<quint8>(); }
    inline quint8 pattern() const { return _variants.value( DPatternStyle, 1 ).value<quint8>(); }
    inline QColor color() const { return _variants.value( DColor, QColor( Qt::black ) ).value<QColor>(); }
    inline QColor outlineColor() const { return _variants.value( DOutlineColor, QColor( Qt::black ) ).value<QColor>(); }
    inline qreal penWidth() const { return _variants.value( DPenWidth, 1. ).toReal(); }
    inline virtual bool isDraggable() const { return true; }
    inline virtual bool isDroppable() const { return true; }

protected:
    virtual QRectF boundingRect() const;
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual void ideDragEnterEvent( QGraphicsSceneDragDropEvent *event );
    virtual void ideDragMoveEvent( QGraphicsSceneDragDropEvent *event );
    virtual void ideDragLeaveEvent( QGraphicsSceneDragDropEvent *event );
    virtual void ideDropEvent( QGraphicsSceneDragDropEvent *event );
    virtual void drawShape( QPainter *p, const QRectF &exposed )=0;
    virtual void paint( QPainter *p, const QRectF &exposed );

protected:
    PHIPalette::ColorRole _colorRole, _outlineColorRole;
    QColor _orgColor, _orgOutlineColor;
    static qreal _dropRegion;
};

#endif // PHIABSTRACTITEMS_H
