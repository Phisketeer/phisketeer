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
#ifndef PHIEFFECTS_H
#define PHIEFFECTS_H

#include <QObject>
#include <QGraphicsEffect>
#include <QPixmapCache>
#include "phi.h"

class PHIBaseItem;

class PHIEXPORT PHIReflectionEffect : public QGraphicsEffect
{
    Q_OBJECT

public:
    explicit PHIReflectionEffect( QObject *parent=0 );
    virtual ~PHIReflectionEffect() {}
    inline qreal yOffset() const { return _yOff; }
    inline qreal size() const { return _size; }
    virtual QRectF boundingRectFor( const QRectF &sourceRect ) const;

public slots:
    inline void setYOffset( qreal yOff ) { _yOff=yOff; updateBoundingRect(); }
    inline void setSize( qreal size ) { _size=size; updateBoundingRect(); }
    inline void setBaseItem( const PHIBaseItem *it ) { _it=it; }

protected:
    virtual void draw( QPainter *painter );
    virtual void sourceChanged( ChangeFlags flags );

private:
    qreal _yOff, _size;
    const PHIBaseItem *_it;
    QPixmapCache::Key _key;
};

#endif // PHIEFFECTS_H
