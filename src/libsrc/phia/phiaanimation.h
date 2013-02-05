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
#ifndef PHIAANIMATION_H
#define PHIAANIMATION_H

#include <QPropertyAnimation>
#include <QPointer>
#include <QSizeF>
#include <QSizePolicy>
#include <phiaitem.h>

class PHIAAnimation : public QPropertyAnimation
{
    Q_OBJECT

public:
    explicit PHIAAnimation( PHIAItem*, PHIAItem::EffectProperty, const QVariant &endVal, int start,
        int duration, const QString &ease=PHI::defaultEasingCurve(), bool relative=false,
        bool infinite=false );
    virtual ~PHIAAnimation();

protected slots:
    virtual void startExclusive();
    virtual void remove();

protected:
    void stopRunningAnimations();

protected:
    //PHIAItem *_item;
    QPointer <PHIAItem>_item;
    PHIAItem::EffectProperty _ep;
    bool _relative, _makeVisible;
};

class PHIAFaderAnimation : public PHIAAnimation
{
    Q_OBJECT

public:
    enum FType { FIn, FOut };
    explicit PHIAFaderAnimation( PHIAItem*, FType, qreal endVal, int start, int duration,
        const QString &ease=PHI::defaultEasingCurve() );

public slots:
    virtual void startExclusive();
    virtual void remove();

protected:
    FType _type;
};

class PHIAVSlideAnimation : public PHIAAnimation
{
    Q_OBJECT

public:
    enum SDirection { SUp, SDown };
    explicit PHIAVSlideAnimation( PHIAItem*, SDirection, int start, int duration,
        const QString &ease=PHI::defaultEasingCurve() );

protected slots:
    virtual void startExclusive();
    void slotStateChanged( QAbstractAnimation::State, QAbstractAnimation::State );

protected:
    qreal _orgHeight;
    SDirection _direction;
    QSizePolicy _sizePolicy;
    QSizeF _minimumSize;
};

#endif // PHIAANIMATION_H
