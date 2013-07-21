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
#ifndef PHISHAPEITEM_H
#define PHISHAPEITEM_H
#include <QPen>
#include <QGradient>
#include <QBrush>
#include "phibaseitem.h"

class PHIShapeItem : public PHIBaseItem
{
    Q_OBJECT
    Q_PROPERTY( quint8 line READ line WRITE setLine NOTIFY lineChanged )

public:
    enum ItemData { DLineStyle=-100, DPatternStyle=-101, DGradientType=-102 };
    explicit PHIShapeItem( Type type, PHIBasePage *page );
    
signals:
    void lineChanged( quint8 );

public slots:
    void setLine( quint8 l );
    inline quint8 line() const { return _variants.value( DLineStyle, 0 ).value<quint8>(); }

protected:
    virtual bool ideDragEnterEvent( QGraphicsSceneDragDropEvent *event );

protected:
    QPen _pen;
    QBrush _brush;
};

#endif // PHISHAPEITEM_H
