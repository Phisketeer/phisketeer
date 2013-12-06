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
#ifndef PHIMISCITEMS_H
#define PHIMISCITEMS_H
#include "phiabstractitems.h"

class PHIHiddenItem : public PHIAbstractInputItem
{
    Q_OBJECT

public:
    enum Wid { Hidden=11 };
    explicit PHIHiddenItem( const PHIBaseItemPrivate &p ) : PHIAbstractInputItem( p ) {}
    PHIHiddenItem( const PHIHiddenItem &it ) : PHIAbstractInputItem( it ) {}
    virtual ~PHIHiddenItem() {}

    virtual QString listName() const { return tr( "Hidden" ); }
    virtual QString description() const { return tr( "Item with input type <hidden>." ); }
    virtual PHIWID wid() const { return Hidden; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/hidden" ) ); }
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;
    virtual bool isWidthChangeable() const { return false; }
    virtual bool isHeightChangeable() const { return false; }

protected:
    virtual QRectF boundingRect() const;
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual void paint( QPainter *painter, const QRectF &exposed );
    virtual void setWidgetText( const QString &t ) { Q_UNUSED( t ) }
    virtual void ideInit();
    virtual void squeeze();
};

class PHIRolloverItem : public PHIAbstractShapeItem
{
    Q_OBJECT
    Q_PROPERTY( QString _text READ realText WRITE setText SCRIPTABLE false )
    Q_PROPERTY( PHIImageHash _images READ realImages WRITE setImages SCRIPTABLE false )
    Q_PROPERTY( QColor _bgColor READ realBgColor WRITE setBgColor SCRIPTABLE false )
    Q_PROPERTY( QColor _hoverColor READ realHoverColor WRITE setHoverColor SCRIPTABLE false )
    Q_PROPERTY( QColor _hoverBgColor READ realHoverBgColor WRITE setHoverBgColor SCRIPTABLE false )

public:
    enum Wid { Rollover=37 };
    enum ItemData { DText=1, DImages=2, DBgColor=3, DHoverColor=4, DHoverBgColor=5 };
    explicit PHIRolloverItem( const PHIBaseItemPrivate &p ) : PHIAbstractShapeItem( p ), _hover( false ) {}
    PHIRolloverItem( const PHIRolloverItem &it ) : PHIAbstractShapeItem( it ),
        _textData( it._textData ), _imageBookData( it._imageBookData ), _bgColorRole( it._bgColorRole ),
        _hoverColorRole( it._hoverColorRole ), _hoverBgColorRole( it._hoverBgColorRole ), _hover( false ) {}
    virtual ~PHIRolloverItem() {}

    virtual QString listName() const { return tr( "Rollover" ); }
    virtual QString description() const { return tr( "Creates a button with rollover effect." ); }
    virtual PHIWID wid() const { return Rollover; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/rollover" ) ); }

    virtual void setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col );
    virtual QColor color( PHIPalette::ItemRole role ) const;
    virtual PHIPalette::ColorRole colorRole( PHIPalette::ItemRole role ) const;
    inline QColor realBgColor() const { return data( DBgColor, QColor( Qt::transparent ) ).value<QColor>(); }
    inline QColor realHoverColor() const { return data( DHoverColor, QColor( Qt::black ) ).value<QColor>(); }
    inline QColor realHoverBgColor() const { return data( DHoverBgColor, QColor( Qt::transparent ) ).value<QColor>(); }
    inline void setBgColor( const QColor &col ) { setColor( PHIPalette::WidgetBase, _bgColorRole, col ); }
    inline void setHoverColor( const QColor &col ) { setColor( PHIPalette::Hover, _hoverColorRole, col ); }
    inline void setHoverBgColor( const QColor &col ) { setColor( PHIPalette::HoverBackground, _hoverBgColorRole, col ); }

    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;
    virtual bool hasText() const { return true; }
    virtual PHITextData* textData() { return &_textData; }
    inline QString realText() const { return QString::fromUtf8( data( DText ).toByteArray() ); }
    inline void setText( const QString &s ) { setData( DText, s.toUtf8() ); update(); }
    virtual bool hasImages() const { return true; }
    virtual PHIImageBookData* imageBookData() { return &_imageBookData; }
    PHIImageHash realImages() const { return data( DImages ).value<PHIImageHash>(); }
    void setImages( const PHIImageHash &imgs ) { QVariant v; v.setValue( imgs ); setData( DImages, v ); update(); }

protected:
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual void drawShape( QPainter *p, const QRectF &exposed );
    virtual void ideInit();
    virtual void squeeze();
    virtual void saveItemData( QDataStream &out, int version );
    virtual void loadItemData( QDataStream &in, int version );
    virtual void ideUpdateData();

private:
    virtual void ideSetText( const QString &t, const QByteArray &lang );
    virtual QString ideText( const QByteArray &lang ) const;

    PHITextData _textData;
    PHIImageBookData _imageBookData;
    PHIPalette::ColorRole _bgColorRole, _hoverColorRole, _hoverBgColorRole;
    bool _hover;
};

#endif // PHIMISCITEMS_H
