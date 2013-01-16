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
#ifndef PHIEFFECTS_H
#define PHIEFFECTS_H

#include <QObject>
#include <QGraphicsEffect>
#include "phi.h"

class PHIEXPORT PHISurfaceEffect : public QGraphicsEffect
{
    Q_OBJECT

public:
    explicit PHISurfaceEffect( QObject *parent=0 );
    virtual ~PHISurfaceEffect();
    inline virtual qreal yOffset() const { return _yOff; }
    inline virtual qreal size() const { return _size; }
    virtual QRectF boundingRectFor( const QRectF &sourceRect ) const;

public slots:
    inline virtual void setYOffset( qreal yOff ) { updateBoundingRect(); _yOff=yOff; }
    inline virtual void setSize( qreal size ) { updateBoundingRect(); _size=size; }

protected:
    virtual void draw( QPainter *painter );
    virtual void sourceChanged( ChangeFlags flags );

private:
    qreal _yOff, _size;
};

#endif // PHIEFFECTS_H
