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
#ifndef PHIELLIPSEITEM_H
#define PHIELLIPSEITEM_H
#include "phiabstractitems.h"

class PHIEllipseItem : public PHIAbstractShapeItem
{
    Q_OBJECT
    Q_PROPERTY( qint16 startAngle READ startAngle WRITE setStartAngle NOTIFY startAngleChanged )
    Q_PROPERTY( qint16 spanAngle READ spanAngle WRITE setSpanAngle NOTIFY spanAngleChanged )

public:
    enum ItemData { DStartAngle=1, DSpanAngle=2 };
    enum Wid { Ellipse=13 };
    explicit PHIEllipseItem();
    inline virtual PHIWID wid() const { return Ellipse; }
    virtual QString listName() const { return tr( "Ellipse" ); }
    virtual QString description() const { return tr( "Draws an ellipse, pie or circle" ); }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/ellipse" ) ); }

public slots:
    inline void setStartAngle( qint16 a ) { _variants.insert( DStartAngle, a ); }
    inline void setSpanAngle( qint16 a ) { _variants.insert( DSpanAngle, a ); }
    inline qint16 startAngle() const { return _variants.value( DStartAngle, 0 ).value<qint16>(); }
    inline qint16 spanAngle() const { return _variants.value( DSpanAngle, 5760 ).value<qint16>(); }

signals:
    void startAngleChanged( qint16 );
    void spanAngleChanged( qint16 );

protected:
    virtual void drawShape( QPainter *painter, const QRectF &r );
};

#endif // PHIELLIPSEITEM_H
