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
#ifndef PHIEFFECT_H
#define PHIEFFECT_H

#include <QByteArray>
#include <QVariant>
#include "phi.h"

class PHIEXPORT PHIEffect
{
public:
    enum Effect { ENone=0x0, EFadeIn=0x1, EFadeOut=0x2, EGraphics=0x4, EMoveTo=0x8, ERotateIn=0x10,
        ERotateOut=0x20, ERotate=0x40, EMoveBy=0x80 };
    enum EffectData { DNone=0, DFadeInStart=1, DFadeInDuration=2, DFadeOutStart=3, DFadeOutDuration=4,
        DFadeMaxOpacity=5, DFadeMinOpacity=6, DBlurRadius=7, DColor=8, DGraphicsType=9,
        DStrength=10, DXOffset=11, DYOffset=12, DMoveLeft=13, DMoveTop=14, DMoveStart=15,
        DMoveDuration=16, DRotateInAxis=17, DRotateInStart=18, DRotateInDuration=19,
        DRotateOutAxis=20, DRotateOutStart=21, DRotateOutDuration=22, DRotateAxis=23,
        DRotateStepX=24, DRotateStepY=25, DRotateStepZ=26, DEaseMoveTo=27, DEaseFadeIn=28,
        DEaseFadeOut=29, DEaseMoveBy=30, DMoveByX=31, DMoveByY=32, DMoveByWidth=33,
        DMoveByHeight=34, DMoveByStart=35, DMoveByDuration=36 };
    enum GraphicsType { GTUnknown=0, GTBlur=1, GTShadow=2, GTColorize=3, GTSurface=4 };

    PHIEffect();
    virtual ~PHIEffect();
    PHIEffect& operator =( const PHIEffect& );
    PHIEffect( const PHIEffect& );

#ifdef PHIDEBUG
    Q_DECLARE_FLAGS( Effects, Effect ) //qint32 in DataStream
#else
    typedef qint32 Effects;
#endif

    QByteArray save() const;
    void load( const QByteArray &data );
    inline Effects effects() const { return _effects; }
    inline void clearEffect( Effect eff ) { _effects &= ~eff; }
    inline void clearAll() { _effects=ENone; _v.clear(); }
    inline GraphicsType graphicsType() const {
        return static_cast<GraphicsType>(_v.value( DGraphicsType, 0 ).value<quint8>()); }

    inline void fadeIn( qint32 &start, qint32 &duration, qreal &maxOpac, quint8 &ease ) const {
        start=_v.value( DFadeInStart, 0 ).value<qint32>();
        duration=_v.value( DFadeInDuration, 1000 ).value<qint32>();
        maxOpac=_v.value( DFadeMaxOpacity, 1. ).value<qreal>();
        ease=_v.value( DEaseFadeIn, PHI::defaultEasingCurveType() ).value<quint8>();
    }
    inline void fadeOut( qint32 &start, qint32 &duration, qreal &minOpac, quint8 &ease ) const {
        start=_v.value( DFadeOutStart, 0 ).value<qint32>();
        duration=_v.value( DFadeOutDuration, 1000 ).value<qint32>();
        minOpac=_v.value( DFadeMinOpacity, 0. ).value<qreal>();
        ease=_v.value( DEaseFadeOut, PHI::defaultEasingCurveType() ).value<quint8>();
    }
    inline void blur( qreal &radius ) const {
        radius=_v.value( DBlurRadius, 5. ).value<qreal>(); }
    inline void surface( qreal &yOff, qreal &size ) const {
        yOff=_v.value( DYOffset, 0. ).value<qreal>();
        size=_v.value( DXOffset, 30 ).value<qreal>(); }
    inline void colorize( QColor &c, qreal &strength ) const {
        c=_v.value( DColor, QColor( 0, 0, 192 ) ).value<QColor>();
        strength=_v.value( DStrength, 1. ).value<qreal>(); }
    void shadow( QColor &c, qreal &xOff, qreal &yOff, qreal &radius ) const;

    inline void moveTo( qint32 &start, qint32 &duration, qint32 &left, qint32 &top, quint8 &ease ) const {
        start=_v.value( DMoveStart, 0 ).value<qint32>(); duration=_v.value( DMoveDuration,
        1000 ).value<qint32>(); left=_v.value( DMoveLeft, 100 ).value<qint32>();
        top=_v.value( DMoveTop, 100 ).value<qint32>();
        ease=_v.value( DEaseMoveTo, PHI::defaultEasingCurveType() ).value<quint8>();
    }
    inline void moveBy( qint32 &start, qint32 &duration, qint32 &x, qint32 &y,
            qint32 &width, qint32 &height, quint8 &ease ) const {
        start=_v.value( DMoveByStart, 0 ).value<qint32>(); duration=_v.value( DMoveByDuration,
        1000 ).value<qint32>(); x=_v.value( DMoveByX, 0 ).value<qint32>();
        y=_v.value( DMoveByY, 0 ).value<qint32>();
        width=_v.value( DMoveByWidth, 0 ).value<qint32>();
        height=_v.value( DMoveByHeight, 0 ).value<qint32>();
        ease=_v.value( DEaseMoveBy, PHI::defaultEasingCurveType() ).value<quint8>();
    }
    inline void rotateIn( quint8 &axis, qint32 &start, qint32 &duration ) const {
        axis=_v.value( DRotateInAxis, 2 ).value<quint8>(); start=_v.value( DRotateInStart, 0 )
        .value<qint32>(); duration=_v.value( DRotateInDuration, 1000 ).value<qint32>(); }
    inline void rotateOut( quint8 &axis, qint32 &start, qint32 &duration ) const {
        axis=_v.value( DRotateOutAxis, 2 ).value<quint8>(); start=_v.value( DRotateOutStart, 0 )
        .value<qint32>(); duration=_v.value( DRotateOutDuration, 1000 ).value<qint32>(); }
    inline void rotate( quint8 &axis, qreal &stepX, qreal &stepY, qreal &stepZ ) const {
        axis=_v.value( DRotateAxis, 4 ).value<quint8>(); stepX=_v.value( DRotateStepX, 1. )
        .value<qreal>(); stepY=_v.value( DRotateStepY, 1. ).value<qreal>();
        stepZ=_v.value( DRotateStepZ, 1. ).value<qreal>(); }

    void setFadeIn( qint32 start, qint32 duration, qreal maxOpac, quint8 ease );
    void setFadeOut( qint32 start, qint32 duration, qreal minOpac, quint8 ease );
    void setBlur( qreal radius );
    void setShadow( const QColor &c, qreal xOff, qreal YOff, qreal radius );
    void setSurface( qreal yOff, qreal size );
    void setColorize( const QColor &c, qreal strength );
    void setMoveTo( qint32 start, qint32 duration, qint32 left, qint32 top, quint8 ease );
    void setMoveBy( qint32 start, qint32 duration, qint32 x, qint32 y, qint32 w, qint32 h, quint8 ease );
    void setRotateIn( quint8 axis, qint32 start, qint32 duration );
    void setRotateOut( quint8 axis, qint32 start, qint32 duration );
    void setRotate( quint8 axis, qreal stepX, qreal stepY, qreal stepZ );

protected:
    Effects _effects; // qint32 in DataStream
    mutable QHash <quint8, QVariant> _v;
};

#ifdef PHIDEBUG
Q_DECLARE_OPERATORS_FOR_FLAGS( PHIEffect::Effects )
#endif

#endif // PHIEFFECT_H
