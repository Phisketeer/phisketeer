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
    Q_PROPERTY( QString _delimiter READ realDelimiter WRITE setDelimiter SCRIPTABLE false )

public:
    enum Wid { Select=10, MultiSelect=18 };
    enum ItemData { DDelimiter=1 };
    explicit PHISelectItem( const PHIBaseItemPrivate &p ) : PHIAbstractInputItem( p ) { if ( isGuiItem() ) initWidget(); }
    PHISelectItem( const PHISelectItem &it ) : PHIAbstractInputItem( it ) { if ( isGuiItem() ) initWidget(); }
    virtual ~PHISelectItem() {}

    virtual QString listName() const { return tr( "Select" ); }
    virtual QString description() const { return tr( "Input type <select>." ); }
    virtual PHIWID wid() const { return Select; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/selectbox" ) ); }
    virtual QString realValue() const;
    virtual void setValue( const QString &v );
    virtual void ideInit();
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;
    inline void setDelimiter( const QString &d ) { setData( DDelimiter, d.toUtf8() ); }
    inline QString realDelimiter() const { return QString::fromUtf8( PHIBaseItem::data( DDelimiter, BL( "\n" ) ).toByteArray() ); }

public slots:
    virtual QScriptValue delimiter( const QScriptValue &d=QScriptValue() );
    virtual QScriptValue selectOptions( const QString &opts, const QString &d=L1( "\n" ) );
    virtual QScriptValue selected( const QScriptValue &v=QScriptValue(), const QScriptValue &b=true );
    virtual QScriptValue data( const QScriptValue &d=QScriptValue() );

protected:
    virtual void initWidget();
    virtual bool hasDelimiter() const { return true; }
    virtual void squeeze();
    virtual void setWidgetText( const QString &t );
    virtual void setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col );
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual void clientPostData( QHttpMultiPart *multiPart ) const;
    void htmlSelectOptions( QByteArray &out, const QByteArray &indent ) const;

protected slots:
    virtual void slotChanged();
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

class PHISelectLangItem : public PHISelectItem
{
    Q_OBJECT

public:
    enum Wid { LangSelect=33 };
    explicit PHISelectLangItem( const PHIBaseItemPrivate &p ) : PHISelectItem( p ) {}
    PHISelectLangItem( const PHISelectLangItem &it ) : PHISelectItem( it ) {}
    virtual ~PHISelectLangItem() {}

    virtual QString listName() const { return tr( "Lang select" ); }
    virtual QString description() const { return tr( "Displays a list of available languages for this page." ); }
    virtual PHIWID wid() const { return LangSelect; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/selectbox" ) ); }
    virtual void ideInit();
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;

public slots:
    virtual QScriptValue selectOptions( const QString &opts, const QString &d );
    virtual QScriptValue data( const QScriptValue &v=QScriptValue() );
    virtual QScriptValue delimiter(const QScriptValue &d=QScriptValue() ) { Q_UNUSED( d ) return QScriptValue( QScriptValue::UndefinedValue ); }

protected slots:
    virtual void slotChanged();

protected:
    virtual void squeeze();
    virtual void ideUpdateData();
    virtual void phisParseData( const PHIDataParser &parser );
};

class PHIMultiSelectItem : public PHISelectItem
{
    Q_OBJECT

public:
    explicit PHIMultiSelectItem( const PHIBaseItemPrivate &p ) : PHISelectItem( p ) { if ( isGuiItem() ) initWidget(); }
    PHIMultiSelectItem( const PHIMultiSelectItem &it ) : PHISelectItem( it ) { if ( isGuiItem() ) initWidget(); }
    virtual ~PHIMultiSelectItem() {}

    virtual QString listName() const { return tr( "List box" ); }
    virtual QString description() const { return tr( "Displays a list box with input type <select>." ); }
    virtual PHIWID wid() const { return MultiSelect; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/listbox" ) ); }
    virtual void ideInit();
    virtual QString realValue() const { return QString(); }
    virtual void setValue( const QString &v ) { Q_UNUSED( v ) }

public slots:
    virtual QScriptValue val( const QScriptValue &v ) { Q_UNUSED( v ) return QScriptValue( QScriptValue::UndefinedValue ); }
    virtual QScriptValue selected( const QScriptValue &v=QScriptValue(), const QScriptValue &b=QScriptValue() );

protected:
    virtual void initWidget();
    virtual void setWidgetText( const QString &t );
    virtual void setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col );
    virtual void clientPostData( QHttpMultiPart *multiPart ) const;
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual bool isSingleLine() const { return false; }
};

#endif // PHILISTITEMS_H
