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
#ifndef PHICALENDARITEMS_H
#define PHICALENDARITEMS_H
#include "phiabstractitems.h"

class QDateEdit;
class QToolButton;

class PHICalendarItem : public PHIAbstractInputItem
{
    Q_OBJECT

public:
    enum Wid { Calendar=32 };
    explicit PHICalendarItem( const PHIBaseItemPrivate &p ) : PHIAbstractInputItem( p ) { if ( isGuiItem() ) initWidget(); }
    PHICalendarItem( const PHICalendarItem &it ) : PHIAbstractInputItem( it ) { if ( isGuiItem() ) initWidget(); }
    virtual ~PHICalendarItem() {}

    virtual QString listName() const { return tr( "Calendar" ); }
    virtual QString description() const { return tr( "Displays a calendar item." ); }
    virtual PHIWID wid() const { return Calendar; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/calendar" ) ); }
    virtual void ideInit();
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;
    virtual PHIWID htmlHeaderExtension( const PHIRequest *req, QByteArray &header ) const;
    virtual PHIWID htmlScriptExtension( const PHIRequest *req, QByteArray &script ) const;
    virtual bool hasHtmlExtension() const { return true; }
    virtual void setValue( const QString &v );
    virtual QString realValue() const;

public slots:
    virtual QScriptValue val( const QScriptValue &v=QScriptValue() );
    virtual QScriptValue data( const QScriptValue &v=QScriptValue() );
    virtual QScriptValue date( const QScriptValue &v=QScriptValue() );
    virtual QScriptValue minDate( const QScriptValue &v=QScriptValue() );
    virtual QScriptValue maxDate( const QScriptValue &v=QScriptValue() );

protected:
    virtual void ideUpdateData();
    virtual void phisCreateData( const PHIDataParser &parser );
    virtual void cssStatic( const PHIRequest *req, QByteArray &out ) const;
    virtual void setWidgetText( const QString &t );
    virtual void setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col );
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;

private slots:
    void slotDateChanged();

private:
    virtual void initWidget();
};

class PHIDateEditItem : public PHICalendarItem
{
    Q_OBJECT

public:
    enum Wid { DateEdit=31 };
    explicit PHIDateEditItem( const PHIBaseItemPrivate &p ) : PHICalendarItem( p ), _date( 0 ), _button( 0 ) { if ( isGuiItem() ) initWidget(); }
    PHIDateEditItem( const PHIDateEditItem &it ) : PHICalendarItem( it ), _date( 0 ), _button( 0 ) { if ( isGuiItem() ) initWidget(); }
    virtual ~PHIDateEditItem() {}

    virtual QString listName() const { return tr( "Date edit" ); }
    virtual QString description() const { return tr( "Date edit item with calendar popup." ); }
    virtual PHIWID wid() const { return DateEdit; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/dateedit" ) ); }
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;
    virtual void phisCreateData( const PHIDataParser &parser );
    virtual void setValue( const QString &v );

public slots:
    virtual QScriptValue minDate( const QScriptValue &v=QScriptValue() );
    virtual QScriptValue maxDate( const QScriptValue &v=QScriptValue() );

protected:
    virtual void ideUpdateData();
    virtual void setWidgetText( const QString &t );
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;

private slots:
    void slotDateChanged( const QDate &d );
    void slotButtonClicked();

private:
    virtual void initWidget();

private:
    QDateEdit *_date;
    QToolButton *_button;
};

#endif // PHICALENDARITEMS_H
