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
#ifndef PHIRECTITEM_H
#define PHIRECTITEM_H

#include "phishapeitem.h"

class PHIRectItem : public PHIShapeItem
{
    Q_OBJECT
    Q_PROPERTY( qint16 borderRadius READ borderRadius WRITE setBorderRadius NOTIFY borderRadiusChanged )

public:
    enum ItemData { DBorderRadius=1 };
    enum Wid { Rect=14, RoundedRect=25 };
    explicit PHIRectItem( Type type, PHIBasePage *page );
    inline virtual PHIWID wid() const { return Rect; }
    virtual QString listName() const { return tr( "Rect" ); }
    virtual QString description() const { return tr( "Draws a box or rect with optional rounded courners" ); }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/editor/images/elements/ellipse" ) ); }

public slots:
    inline virtual qint16 borderRadius() const { return _variants.value( DBorderRadius, 0 ).value<qint16>(); }
    inline virtual void setBorderRadius( qint16 r ) { _variants.insert( DBorderRadius, r ); }

signals:
    void borderRadiusChanged( qint16 );

protected:
    virtual void paint( QPainter *painter, const QRectF &r );
};

#endif // PHIRECTITEM_H
