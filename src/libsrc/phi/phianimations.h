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
#ifndef PHIANIMATIONS_H
#define PHIANIMATIONS_H
#include <QPropertyAnimation>

class PHIBaseItem;

class PHISlideAnimation : public QPropertyAnimation
{
    Q_OBJECT

public:
    explicit PHISlideAnimation( PHIBaseItem *item, qreal orgHeight, bool upDirection );

protected slots:
    void slotFinished();
    void slotStateChanged( QAbstractAnimation::State newState, QAbstractAnimation::State oldState );

private:
    PHIBaseItem *_item;
    qreal _orgHeight;
    bool _upDir;
};

#endif // PHIANIMATIONS_H
