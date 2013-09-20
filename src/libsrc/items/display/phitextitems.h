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
#ifndef PHITEXTITEMS_H
#define PHITEXTITEMS_H
#include "phiabstractitems.h"

class PHILabelItem : public PHIAbstractTextItem
{
    Q_OBJECT

public:
    enum Wid { Label=30 };
    explicit PHILabelItem();
    virtual QString listName() const { return tr( "Label" ); }
    virtual QString description() const { return tr( "Displays a label (single line)" ); }
    virtual PHIWID wid() const { return Label; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/label" ) ); }

protected:
    virtual void setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col );
    virtual void setWidgetText( const QString &t );
};

#endif // PHITEXTITEMS_H