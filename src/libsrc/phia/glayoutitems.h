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
#ifndef GLAYOUTITEMS_H
#define GLAYOUTITEMS_H

#include <QList>
#include <QLineEdit>
#include <QComboBox>
#include "gwidgetitem.h"
#include "phi.h"

class PHIAItem;

class GAbstractLayoutItem : public GWidgetItem
{
    Q_OBJECT

public:
    GAbstractLayoutItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent=0 );
    virtual ~GAbstractLayoutItem();
    virtual void insertChild( const PHIAItem* )=0;
    inline QList <const PHIAItem*> children() const { return _children; }
    virtual void activate()=0;
    virtual QList <QLineEdit*> lineEditItems() const=0;
    virtual QList <QComboBox*> comboBoxItems() const=0;
    virtual void setWidth( qreal w );
    virtual void setHeight( qreal h );
    virtual void prepareSlideEffect();
    virtual void restoreSlideEffect();

protected slots:
    void slotUpdateSize( int ); // workaround to adjust sizes

signals:
    void updateSize( int );

protected:
    QList <const PHIAItem*> _children;
    qreal _width, _height;
    QHash <int, QSizePolicy> _policyHash;
    QHash <int, QSizeF> _minimumSizeHash;
};

class GLinearLayoutItem : public GAbstractLayoutItem
{
public:
    GLinearLayoutItem( qreal w, qreal h, const PHIAItem *it, const PHIByteArrayList &childIds,
        bool horizontal=false, QGraphicsItem *parent=0 );
    virtual ~GLinearLayoutItem();
    virtual void insertChild( const PHIAItem* );
    virtual void activate();
    virtual QList <QLineEdit*> lineEditItems() const;
    virtual QList <QComboBox*> comboBoxItems() const;

protected:
    QGraphicsLinearLayout *_layout;
    PHIByteArrayList _childIds;
};

class GGridLayoutItem : public GAbstractLayoutItem
{
public:
    GGridLayoutItem( qreal w, qreal h, const PHIAItem *it, const PHIRectHash &itemPos,
        QGraphicsItem *parent=0 );
    virtual ~GGridLayoutItem();
    virtual void insertChild( const PHIAItem* );
    virtual void activate();
    virtual QList <QLineEdit*> lineEditItems() const;
    virtual QList <QComboBox*> comboBoxItems() const;

protected:
    QGraphicsGridLayout *_layout;
    PHIRectHash _itemPos;
    QMap <int, QLineEdit*> _lineEditItems;
    QMap <int, QComboBox*> _comboBoxItems;
};

class GHSpacerItem : public GWidgetItem
{
public:
    GHSpacerItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent=0 );
    virtual ~GHSpacerItem();
};

class GVSpacerItem : public GWidgetItem
{
public:
    GVSpacerItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent=0 );
    virtual ~GVSpacerItem();
};

#endif // GLAYOUTITEMS_H
