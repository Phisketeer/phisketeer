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
#include <QWidget>
#include "phianimations.h"
#include "phibaseitem.h"

PHISlideAnimation::PHISlideAnimation(PHIBaseItem *item, qreal orgHeight, bool upDir )
    : QPropertyAnimation( item ), _item( item ), _orgHeight( orgHeight ), _upDir( upDir )
{
    setTargetObject( item );
    setPropertyName( "_height" );
    connect( this, &QPropertyAnimation::finished, this, &PHISlideAnimation::slotFinished );
    connect( this, &QPropertyAnimation::stateChanged, this, &PHISlideAnimation::slotStateChanged );
    if ( _upDir ) {
        setStartValue( _orgHeight );
        setEndValue( 0 );
    } else {
        setStartValue( 0 );
        setEndValue( _orgHeight );
    }
}

void PHISlideAnimation::slotFinished()
{
    _item->setHeight( _orgHeight );
    if ( _upDir ) {
        _item->hide();
    }
}

void PHISlideAnimation::slotStateChanged( QAbstractAnimation::State newState, QAbstractAnimation::State oldState )
{
    Q_UNUSED( oldState )
    if ( newState==Running ) {
        if ( _upDir ) {
            if ( !_item->realVisible() ) {
                stop();
                emit finished();
            }
        } else {
            if ( _item->realVisible() ) {
                stop();
                emit finished();
            } else _item->show();
        }
    }
}
