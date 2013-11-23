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
    virtual QString description() const { return tr( "Item with input type <hidden>" ); }
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

class PHIDecoratedTableItem : public PHIAbstractInputItem
{
    Q_OBJECT

public:
    enum Wid { DecoratedTable=46 };
    enum DataType { DOptions=1 };
    explicit PHIDecoratedTableItem( const PHIBaseItemPrivate &p ) : PHIAbstractInputItem( p ) { if ( isGuiItem() ) initWidget(); }
    PHIDecoratedTableItem( const PHIDecoratedTableItem &it ) : PHIAbstractInputItem( it ) {}
    virtual ~PHIDecoratedTableItem() {}

    virtual QString listName() const { return tr( "Decorated table" ); }
    virtual QString description() const { return tr( "Table with selectable rows." ); }
    virtual PHIWID wid() const { return DecoratedTable; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/table" ) ); }
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;

protected:
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual void setWidgetText( const QString &t );
    virtual void ideInit();

private:
    void initWidget();
};

#endif // PHIMISCITEMS_H
