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
#ifndef PHIINPUTITEMS_H
#define PHIINPUTITEMS_H
#include "phiabstractinputitem.h"

class PHILineEditItem : public PHIAbstractInputItem
{
    Q_OBJECT

public:
    enum Wid { LineEdit=1 };
    explicit PHILineEditItem();
    virtual QString listName() const { return tr( "Lineedit" ); }
    virtual QString description() const { return tr( "Line edit with input type <text>" ); }
    virtual PHIWID wid() const { return LineEdit; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/lineedit" ) ); }
    //virtual bool isHeightChangeable() const { return false; }

protected:
    virtual void setWidgetText( const QString &s );
};

class PHITextAreaItem : public PHIAbstractInputItem
{
    Q_OBJECT

public:
    enum Wid { TextArea=2 };
    explicit PHITextAreaItem();
    virtual QString listName() const { return tr( "Textarea" ); }
    virtual QString description() const { return tr( "Input type <textarea>" ); }
    virtual PHIWID wid() const { return TextArea; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/textarea" ) ); }

protected:
    virtual bool isSingleLine() const { return false; }
    virtual void setWidgetText( const QString &t );
};

#endif // PHIINPUTITEMS_H
