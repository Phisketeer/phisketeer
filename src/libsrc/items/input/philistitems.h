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
#ifndef PHILISTITEMS_H
#define PHILISTITEMS_H
#include "phiabstractitems.h"

class PHISelectItem : public PHIAbstractInputItem
{
    Q_OBJECT
    Q_PROPERTY( QString delimiter READ delimiter WRITE setDelimiter )

public:
    enum Wid { Select=10 };
    enum ItemData { DDelimiter=1 };
    explicit PHISelectItem( const PHIBaseItemPrivate &p ) : PHIAbstractInputItem( p ) { if ( isGuiItem() ) initWidget(); }
    PHISelectItem( const PHISelectItem &it ) : PHIAbstractInputItem( it ) { if ( isGuiItem() ) initWidget(); }
    virtual ~PHISelectItem() {}

    virtual QString listName() const { return tr( "Select" ); }
    virtual QString description() const { return tr( "Input type <select>" ); }
    virtual PHIWID wid() const { return Select; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/selectbox" ) ); }
    virtual void ideInit();

public slots:
    inline void setDelimiter( const QString &d ) { setData( DDelimiter, d.toUtf8() ); }
    inline QString delimiter() const { return QString::fromUtf8( data( DDelimiter, L1( "\n" ) ).toByteArray() ); }

protected:
    virtual void initWidget();
    virtual bool hasDelimiter() const { return true; }
    virtual void squeeze();
    virtual void setWidgetText( const QString &t );
    virtual void setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col );
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
};

class PHISelectCountryItem : public PHISelectItem
{
    Q_OBJECT

public:
    enum Wid { CountrySelect=47 };
    explicit PHISelectCountryItem( const PHIBaseItemPrivate &p ) : PHISelectItem( p ) {}
    PHISelectCountryItem( const PHISelectCountryItem &it ) : PHISelectItem( it ) {}
    virtual ~PHISelectCountryItem() {}

    virtual QString listName() const { return tr( "Country" ); }
    virtual QString description() const { return tr( "Displays a list of countries for selection." ); }
    virtual PHIWID wid() const { return CountrySelect; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/selectbox" ) ); }
    virtual void ideInit();
};

class PHIMultiSelectItem : public PHISelectItem
{
    Q_OBJECT

public:
    enum Wid { MultiSelect=18 };
    explicit PHIMultiSelectItem( const PHIBaseItemPrivate &p ) : PHISelectItem( p ) { if ( isGuiItem() ) initWidget(); }
    PHIMultiSelectItem( const PHIMultiSelectItem &it ) : PHISelectItem( it ) { if ( isGuiItem() ) initWidget(); }
    virtual ~PHIMultiSelectItem() {}

    virtual QString listName() const { return tr( "List box" ); }
    virtual QString description() const { return tr( "Displays a list box with input type <select>." ); }
    virtual PHIWID wid() const { return MultiSelect; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/listbox" ) ); }
    virtual void ideInit();

protected:
    virtual void initWidget();
    inline virtual bool isSingleLine() const { return false; }
    virtual void setWidgetText( const QString &t );
    virtual void setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col );
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
};

#endif // PHILISTITEMS_H
