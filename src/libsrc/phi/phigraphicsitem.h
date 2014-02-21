/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2014  Phisys AG, Switzerland
#    Copyright (C) 2012-2014  Phisketeer.org team
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
#ifndef PHIGRAPHICSITEM_H
#define PHIGRAPHICSITEM_H

#include <QGraphicsProxyWidget>
#include "phigraphicsscene.h"
#include "phibaseitem.h"

class PHIGraphicsItem;
class PHIBasePage;
class QEvent;

class PHIEXPORT PHIGraphicsItemProvider : public QObject
{
public:
    explicit PHIGraphicsItemProvider( QObject *parent );
    virtual ~PHIGraphicsItemProvider();
    virtual PHIGraphicsItem* createGraphicsItem() { return 0; }
    static PHIGraphicsItemProvider* instance() { return _instance; }

private:
    static PHIGraphicsItemProvider *_instance;
};

class PHIEXPORT PHIGraphicsItem : public QGraphicsProxyWidget
{
    Q_OBJECT

public:
    explicit PHIGraphicsItem();
    virtual QRectF boundingRect() const;
    virtual void updateGeometry() { QGraphicsProxyWidget::updateGeometry(); }
    inline PHIBaseItem* baseItem() const { return _it; }
    virtual void setBaseItem( PHIBaseItem *it );

protected:
    virtual bool sceneEvent( QEvent *event )=0;
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual QPainterPath shape() const;
    virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );

private:
    PHIBaseItem *_it;
};

#endif // PHIGRAPHICSITEM_H
