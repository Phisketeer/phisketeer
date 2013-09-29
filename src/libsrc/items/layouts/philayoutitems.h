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
    inline virtual PHIWID wid() const { return HorizontalLayout; }
    inline virtual QString listName() const { return tr( "HBox layout" ); }
    inline virtual QPixmap pixmap() const { return QPixmap(); }
    inline virtual QString description() const { return tr( "Manages a horizontal layout." ); }
    virtual void addBaseItems( const QList <PHIBaseItem*> &list );
    virtual void activateLayout();
    virtual void updateChildId( const QString &oldId, const QString &newId );

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
    inline virtual PHIWID wid() const { return VerticalLayout; }
    inline virtual QString listName() const { return tr( "VBox layout" ); }
    inline virtual QPixmap pixmap() const { return QPixmap(); }
    inline virtual QString description() const { return tr( "Manages a vertical layout." ); }
    virtual void addBaseItems( const QList <PHIBaseItem*> &list );
};

class PHIFormLayoutItem : public PHIAbstractLayoutItem
{
    Q_OBJECT
    Q_PROPERTY( PHIRectHash childRects READ childRects WRITE setChildRects SCRIPTABLE false )

public:
    enum Wid { FormLayout=22 };
    explicit PHIFormLayoutItem();
    inline virtual PHIWID wid() const { return FormLayout; }
    inline virtual QString listName() const { return tr( "Form layout" ); }
    inline virtual QPixmap pixmap() const { return QPixmap(); }
    inline virtual QString description() const { return tr( "Manages a double column layout." ); }
    virtual void addBaseItems( const QList <PHIBaseItem*> &list );
    virtual void activateLayout();
    virtual void updateChildId( const QString &oldId, const QString &newId );

public slots:
    inline void setChildRects( const PHIRectHash &r ) { _childRects=r; }
    inline PHIRectHash childRects() const { return _childRects; }

protected:
    virtual void loadItemData( QDataStream &in, int version );
    virtual void saveItemData( QDataStream &out, int version );
    virtual bool isPrivateItem() const { return true; }

private:
    PHIRectHash _childRects;
};

class PHIGridLayoutItem : public PHIFormLayoutItem
{
    Q_OBJECT

public:
    enum Wid { GridLayout=19 };
    explicit PHIGridLayoutItem();
    inline virtual PHIWID wid() const { return GridLayout; }
    inline virtual QString listName() const { return tr( "Grid layout" ); }
    inline virtual QPixmap pixmap() const { return QPixmap(); }
    inline virtual QString description() const { return tr( "Manages items in a grid layout." ); }
    virtual void addBaseItems( const QList <PHIBaseItem*> &list );

protected:
    virtual void loadItemData( QDataStream &in, int version );
    virtual void saveItemData( QDataStream &out, int version );
};

#endif // PHIABSTRACTLAYOUTITEM_H
