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
#ifndef PHIGRAPHICSITEM_H
#define PHIGRAPHICSITEM_H

#include <QGraphicsProxyWidget>
#include "phigraphicsscene.h"
#include "phibaseitem.h"

class PHIBasePage;
class QEvent;

class PHIEXPORT PHIGraphicsItem : public QGraphicsProxyWidget
{
    Q_OBJECT

public:
    explicit PHIGraphicsItem( PHIBaseItem* );
    virtual QRectF boundingRect() const;
    inline PHIBaseItem* baseItem() const { return _it; }
    virtual void updateGeometry() { QGraphicsProxyWidget::updateGeometry(); }

protected:
    virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );
    virtual bool sceneEvent( QEvent *event )=0;
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual QPainterPath shape() const;

private:
    PHIBaseItem *_it;
};

class PHIEXPORT PHIGraphicsItemProvider : public QObject
{
    Q_OBJECT

public:
    static PHIGraphicsItemProvider* instance() { return _instance; }
    virtual QGraphicsWidget* createGraphicsItem( PHIBaseItem* )=0;
    inline PHIBasePage* currentBasePage() const { return _page; }
    inline void setCurrentBasePage( PHIBasePage *page ) { _page=page; }
    inline void setCurrentItemType( PHIBaseItem::Type t ) { _type=t; }
    inline PHIBaseItem::Type currentItemType() const { return _type; }
    virtual ~PHIGraphicsItemProvider();

protected:
    explicit PHIGraphicsItemProvider( QObject *parent );
    static PHIGraphicsItemProvider *_instance;

private:
    PHIBasePage *_page;
    PHIBaseItem::Type _type;
};

#endif // PHIGRAPHICSITEM_H
