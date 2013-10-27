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
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PHICHECKITEMS_H
#define PHICHECKITEMS_H
#include "phiabstractitems.h"
#include "phipalette.h"

class PHICheckBoxItem : public PHIAbstractInputItem
{
    Q_OBJECT
    Q_PROPERTY( bool checked READ checked WRITE setChecked )

public:
    enum Wid { Checkbox=4 };
    explicit PHICheckBoxItem( const PHIBaseItemPrivate &p ) : PHIAbstractInputItem( p ) { if ( isGuiItem() ) initWidget(); }
    PHICheckBoxItem( const PHICheckBoxItem &it ) : PHIAbstractInputItem( it ), _checkedData( it._checkedData ) { if ( isGuiItem() ) initWidget(); }
    virtual ~PHICheckBoxItem() {}

    virtual QString listName() const { return tr( "Checkbox" ); }
    virtual QString description() const { return tr( "Check box with input type <checkbox>" ); }
    virtual PHIWID wid() const { return Checkbox; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/checkbox" ) ); }
    virtual void initIDE();

public slots:
    virtual void setChecked( bool b );

protected:
    virtual void updateData();
    virtual void initWidget();
    virtual void loadItemData( QDataStream &in, int version );
    virtual void saveItemData( QDataStream &out, int version );
    virtual bool isCheckable() const { return true; }
    virtual void setWidgetText( const QString &s );
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual PHIBooleanData* checkedData() { return &_checkedData; }
    virtual void setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col );

private:
    PHIBooleanData _checkedData;
};

class PHIRadioButtonItem : public PHICheckBoxItem
{
    Q_OBJECT

public:
    enum Wid { RadioButton=5 };
    explicit PHIRadioButtonItem( const PHIBaseItemPrivate &p ) : PHICheckBoxItem( p ) { if ( isGuiItem() ) initWidget(); }
    PHIRadioButtonItem( const PHIRadioButtonItem &it ) : PHICheckBoxItem( it ) { if ( isGuiItem() ) initWidget(); }
    virtual ~PHIRadioButtonItem() {}

    virtual QString listName() const { return tr( "Radio button" ); }
    virtual QString description() const { return tr( "Radio button with input type <radio>" ); }
    virtual PHIWID wid() const { return RadioButton; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/radiobutton" ) ); }
    virtual void initIDE();

protected:
    virtual void initWidget();
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
};

#endif // PHICHECKITEMS_H
