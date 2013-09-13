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
#include "phiabstractinputitem.h"
#include "phipalette.h"

class PHIBooleanData;

class PHICheckBoxItem : public PHIAbstractInputItem
{
    Q_OBJECT

public:
    enum Wid { Checkbox=4 };
    explicit PHICheckBoxItem();
    virtual ~PHICheckBoxItem();
    virtual QString listName() const { return tr( "Checkbox" ); }
    virtual QString description() const { return tr( "Check box with input type <checkbox>" ); }
    virtual PHIWID wid() const { return Checkbox; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/checkbox" ) ); }

protected:
    virtual void updateData();
    virtual void loadItemData( QDataStream &in, int version );
    virtual void saveItemData( QDataStream &out, int version );
    virtual bool isCheckable() const { return true; }
    virtual void setWidgetText( const QString &s );
    virtual void setChecked( bool b );
    virtual PHIBooleanData* checkedData() const { return _checkedData; }
    virtual void setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col );

private:
    PHIBooleanData *_checkedData;
};

#endif // PHICHECKITEMS_H
