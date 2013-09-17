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
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PHIABSTRACTLAYOUTITEM_H
#define PHIABSTRACTLAYOUTITEM_H
#include <QGraphicsLinearLayout>
#include <QGraphicsGridLayout>
#include "phiabstractitems.h"

class PHIHorizontalLayoutItem : public PHIAbstractLayoutItem
{
    Q_OBJECT
    Q_PROPERTY( PHIByteArrayList childIds READ childIds WRITE setChildIds SCRIPTABLE false )

public:
    enum Wid { HorizontalLayout=21 };
    explicit PHIHorizontalLayoutItem();
    virtual PHIWID wid() const { return HorizontalLayout; }
    virtual QString listName() const { return tr( "HBox layout" ); }
    virtual QPixmap pixmap() const { return QPixmap(); }
    virtual QString description() const { return tr( "Manages a horizontal layout." ); }
    virtual void addBaseItems( const QList <PHIBaseItem*> &list );
    virtual void activateLayout();

protected:
    virtual void loadItemData( QDataStream &in, int version );
    virtual void saveItemData( QDataStream &out, int version );
    virtual bool isPrivateItem() const { return true; }

protected slots:
    inline void setChildIds( const PHIByteArrayList &ids ) { _childIds=ids; }
    inline PHIByteArrayList childIds() const { return _childIds; }

private:
    PHIByteArrayList _childIds;
};

class PHIVerticalLayoutItem : public PHIHorizontalLayoutItem
{
    Q_OBJECT

public:
    enum Wid { VerticalLayout=20 };
    explicit PHIVerticalLayoutItem();
    virtual PHIWID wid() const { return VerticalLayout; }
    virtual QString listName() const { return tr( "VBox layout" ); }
    virtual QPixmap pixmap() const { return QPixmap(); }
    virtual QString description() const { return tr( "Manages a vertical layout." ); }
    virtual void addBaseItems( const QList <PHIBaseItem*> &list );
};

#endif // PHIABSTRACTLAYOUTITEM_H
