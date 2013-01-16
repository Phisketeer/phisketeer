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
#include <QDataStream>
#include "phieffect.h"

PHIEffect::PHIEffect() : _effects( PHIEffect::ENone )
{
    //qDebug( "PHIEffect::PHIEffect()" );
}

PHIEffect::~PHIEffect()
{
    //qDebug( "PHIEffect::~PHIEffect()" );
}

PHIEffect& PHIEffect::operator =( const PHIEffect &eff )
{
    _effects=eff._effects;
    _v=eff._v;
    return *this;
}

PHIEffect::PHIEffect( const PHIEffect &eff )
{
    _effects=eff._effects;
    _v=eff._v;
}

void PHIEffect::setFadeIn( qint32 start, qint32 duration, qreal maxOpac, quint8 ease )
{
    _effects |= EFadeIn;
    if ( start!=0 ) _v.insert( DFadeInStart, start );
    else _v.remove( DFadeInStart );
    if ( duration!=1000 ) _v.insert( DFadeInDuration, duration );
    else _v.remove( DFadeInDuration );
    if ( maxOpac!=1. ) _v.insert( DFadeMaxOpacity, maxOpac );
    else _v.remove( DFadeMaxOpacity );
    if ( ease!=PHI::defaultEasingCurveType() ) _v.insert( DEaseFadeIn, ease );
    else _v.remove( DEaseFadeIn );
}

void PHIEffect::setFadeOut( qint32 start, qint32 duration, qreal minOpac, quint8 ease )
{
    _effects |= EFadeOut;
    if ( start!=0 ) _v.insert( DFadeOutStart, start );
    else _v.remove( DFadeOutStart );
    if ( duration!=1000 ) _v.insert( DFadeOutDuration, duration );
    else _v.remove( DFadeOutDuration );
    if ( minOpac!=0. ) _v.insert( DFadeMinOpacity, minOpac );
    else _v.remove( DFadeMinOpacity );
    if ( ease!=PHI::defaultEasingCurveType() ) _v.insert( DEaseFadeOut, ease );
    else _v.remove( DEaseFadeOut );
}

void PHIEffect::setBlur( qreal r )
{
    _effects |= EGraphics;
    if ( r!=5. ) _v.insert( DBlurRadius, r );
    else _v.remove( DBlurRadius );
    _v.insert( DGraphicsType, static_cast<quint8>(GTBlur) );
}

void PHIEffect::shadow( QColor &c, qreal &xOff, qreal &yOff, qreal &radius ) const
{
    c=_v.value( DColor, QColor( 63, 63, 63, 180 ) ).value<QColor>();
    xOff=_v.value( DXOffset, 8. ).value<qreal>();
    yOff=_v.value( DYOffset, 8. ).value<qreal>();
    radius=_v.value( DBlurRadius, 1. ).value<qreal>();
}

void PHIEffect::setShadow( const QColor &c, qreal xOff, qreal yOff, qreal radius )
{
    _effects |= EGraphics;
    if ( c!=QColor( 63, 63, 63, 180 ) ) _v.insert( DColor, c );
    else _v.remove( DColor );
    if ( xOff!=8. ) _v.insert( DXOffset, xOff );
    else _v.remove( DXOffset );
    if ( yOff!=8. ) _v.insert( DYOffset, yOff );
    else _v.remove( DYOffset );
    if ( radius!=1. ) _v.insert( DBlurRadius, radius );
    else _v.remove( DBlurRadius );
    _v.insert( DGraphicsType, static_cast<quint8>(GTShadow) );
}

void PHIEffect::setColorize( const QColor &c, qreal strength )
{
    _effects |= EGraphics;
    if ( c!=QColor( 0, 0, 192 ) ) _v.insert( DColor, c );
    else _v.remove( DColor );
    if ( strength!=1. ) _v.insert( DStrength, strength );
    else _v.remove( DStrength );
    _v.insert( DGraphicsType, static_cast<quint8>(GTColorize) );
}

void PHIEffect::setSurface( qreal yOff, qreal size )
{
    _effects |= EGraphics;
    if ( yOff!=0. ) _v.insert( DYOffset, yOff );
    else _v.remove( DYOffset );
    if ( size!=30. ) _v.insert( DXOffset, size );
    else _v.remove( DXOffset );
    _v.insert( DGraphicsType, static_cast<quint8>(GTSurface) );
}

void PHIEffect::setMoveTo( qint32 start, qint32 duration, qint32 left, qint32 top, quint8 ease )
{
    _effects |= EMoveTo;
    if ( start!=0 ) _v.insert( DMoveStart, start );
    else _v.remove( DMoveStart );
    if ( duration!=1000 ) _v.insert( DMoveDuration, duration );
    else _v.remove( DMoveDuration );
    if ( left!=100 ) _v.insert( DMoveLeft, left );
    else _v.remove( DMoveLeft );
    if ( top!=100 ) _v.insert( DMoveTop, top );
    else _v.remove( DMoveTop );
    if ( ease!=PHI::defaultEasingCurveType() ) _v.insert( DEaseMoveTo, ease );
    else _v.remove( DEaseMoveTo );
}

void PHIEffect::setMoveBy( qint32 start, qint32 duration, qint32 x, qint32 y, qint32 w, qint32 h, quint8 ease )
{
    _effects |= EMoveBy;
    if ( start!=0 ) _v.insert( DMoveByStart, start );
    else _v.remove( DMoveByStart );
    if ( duration!=1000 ) _v.insert( DMoveByDuration, duration );
    else _v.remove( DMoveByDuration );
    if ( x!=0 ) _v.insert( DMoveByX, x );
    else _v.remove( DMoveByX );
    if ( y!=0 ) _v.insert( DMoveByY, y );
    else _v.remove( DMoveByY );
    if ( w!=0 ) _v.insert( DMoveByWidth, w );
    else _v.remove( DMoveByWidth );
    if ( h!=0 ) _v.insert( DMoveByHeight, h );
    else _v.remove( DMoveByHeight );
    if ( ease!=PHI::defaultEasingCurveType() ) _v.insert( DEaseMoveBy, ease );
    else _v.remove( DEaseMoveBy );
}

void PHIEffect::setRotateIn( quint8 axis, qint32 start, qint32 duration )
{
    _effects |= ERotateIn;
    if ( axis!=0x2 ) _v.insert( DRotateInAxis, axis );
    else _v.remove( DRotateInAxis );
    if ( start!=0 ) _v.insert( DRotateInStart, start );
    else _v.remove( DRotateInStart );
    if ( duration!=1000 ) _v.insert( DRotateInDuration, duration );
    else _v.remove( DRotateInDuration );
}

void PHIEffect::setRotateOut( quint8 axis, qint32 start, qint32 duration )
{
    _effects |= ERotateOut;
    if ( axis!=0x2 ) _v.insert( DRotateOutAxis, axis );
    else _v.remove( DRotateOutAxis );
    if ( start!=0 ) _v.insert( DRotateOutStart, start );
    else _v.remove( DRotateOutStart );
    if ( duration!=1000 ) _v.insert( DRotateOutDuration, duration );
    else _v.remove( DRotateOutDuration );
}

void PHIEffect::setRotate( quint8 axis, qreal stepX, qreal stepY, qreal stepZ )
{
    _effects |= ERotate;
    if ( axis!=4 ) _v.insert( DRotateAxis, axis );
    else _v.remove( DRotateAxis );
    if ( (axis & 0x1) && stepX != 1. ) _v.insert( DRotateStepX, stepX );
    else _v.remove( DRotateStepX );
    if ( (axis & 0x2) && stepY != 1. ) _v.insert( DRotateStepY, stepY );
    else _v.remove( DRotateStepY );
    if ( (axis & 0x4) && stepZ != 1. ) _v.insert( DRotateStepZ, stepZ );
    else _v.remove( DRotateStepZ );
}

QByteArray PHIEffect::save() const
{
    QByteArray data;
    QDataStream ds( &data, QIODevice::WriteOnly );
    ds.setVersion( PHI_DSV );
    ds << static_cast<qint32>(_effects);
    if ( !( _effects & EFadeIn ) ) {
        _v.remove( DFadeInDuration );
        _v.remove( DFadeInStart );
        _v.remove( DFadeMaxOpacity );
        _v.remove( DEaseFadeIn );
    }
    if ( !( _effects & EFadeOut ) ) {
        _v.remove( DFadeOutDuration );
        _v.remove( DFadeOutStart );
        _v.remove( DFadeMinOpacity );
        _v.remove( DEaseFadeOut );
    }
    if ( !( _effects & EGraphics ) ) {
        _v.remove( DGraphicsType );
        _v.remove( DBlurRadius );
        _v.remove( DColor );
        _v.remove( DStrength );
        _v.remove( DXOffset );
        _v.remove( DYOffset );
    }
    if ( !( _effects & EMoveTo ) ) {
        _v.remove( DMoveDuration );
        _v.remove( DMoveLeft );
        _v.remove( DMoveStart );
        _v.remove( DMoveTop );
        _v.remove( DEaseMoveTo );
    }
    if ( !( _effects & EMoveBy ) ) {
        _v.remove( DMoveByDuration );
        _v.remove( DMoveByStart );
        _v.remove( DMoveByX );
        _v.remove( DMoveByY );
        _v.remove( DMoveByWidth );
        _v.remove( DMoveByHeight );
        _v.remove( DEaseMoveBy );
    }
    if ( !( _effects & ERotateIn ) ) {
        _v.remove( DRotateInAxis );
        _v.remove( DRotateInDuration );
        _v.remove( DRotateInStart );
    }
    if ( !( _effects & ERotateOut ) ) {
        _v.remove( DRotateOutAxis );
        _v.remove( DRotateOutDuration );
        _v.remove( DRotateOutStart );
    }
    if ( !( _effects & ERotate ) ) {
        _v.remove( DRotateAxis );
        _v.remove( DRotateStepX );
        _v.remove( DRotateStepY );
        _v.remove( DRotateStepZ );
    }
    ds << _v;
    return data;
}

void PHIEffect::load( const QByteArray &data )
{
    QByteArray d=data;
    QDataStream ds( &d, QIODevice::ReadOnly );
    ds.setVersion( PHI_DSV );
    qint32 flags;
    ds >> flags >> _v;
    _effects=static_cast<Effects>(flags);
}
