/*
#    Copyright (C) 2010-2012  Marius B. Schumacher
#    Copyright (C) 2011-2012  Phisys AG, Switzerland
#    Copyright (C) 2012  Phisketeer.org team
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
#include <QTimer>
#include "phiaanimation.h"
#include "phi.h"
#include "gwidgetitem.h"
#include "glayoutitems.h"

PHIAAnimation::PHIAAnimation( PHIAItem *it, PHIAItem::EffectProperty ep, const QVariant &endVal,
    int start, int duration, const QString &ease, bool relative, bool infinite )
    : QPropertyAnimation( it ), _item( it ), _ep( ep ), _relative( relative )
{
    qDebug( "PHIAAnimation::PHIAAnimation()" );
    setEndValue( endVal );
    setDuration( duration );
    if ( infinite ) setLoopCount( -1 );
    setTargetObject( it );
    setEasingCurve( PHI::toEasingCurveType( ease ) );
    it->_animations.insert( ep, this );
    connect( this, SIGNAL( finished() ), this, SLOT( remove() ) );
    QTimer::singleShot( start, this, SLOT( startExclusive() ) );
}

PHIAAnimation::~PHIAAnimation()
{
    //qDebug( "PHIAAnimation::~PHIAAnimation()" );
}

void PHIAAnimation::remove()
{
    if ( !_item ) return;
    QMultiHash <PHIAItem::EffectProperty,QPropertyAnimation*>::iterator i=_item->_animations.begin();
    QPropertyAnimation *anim;
    while ( i!=_item->_animations.end() ) {
        anim=i.value();
        if ( anim==this ) {
            _item->_animations.erase( i );
            deleteLater();
            //qDebug( "removed myself from animations" );
            break;
        }
        ++i;
    }
    //qDebug( "removeEnd" );
}

void PHIAAnimation::stopRunningAnimations()
{
    if ( !_item ) return;
    QMultiHash <PHIAItem::EffectProperty,QPropertyAnimation*>::iterator i=_item->_animations.begin();
    QPropertyAnimation *anim;
    while ( i!=_item->_animations.end() ) {
        anim=i.value();
        if ( anim==this ) {
            i++; continue;
        }
        if ( i.key()==_ep ) { // Check if anim uses the same property
            if ( anim->state()==Running ) {
                qDebug( "stopping running animation" );
                anim->stop();
                anim->deleteLater();
                i=_item->_animations.erase( i );
                continue;
            }
        }
        i++;
    }
}

void PHIAAnimation::startExclusive()
{
    stopRunningAnimations();
    qDebug( "starting exclusive %d", _ep );
    switch ( _ep ) {
    case PHIAItem::EPMoveX: setStartValue( _item->x() ); setPropertyName( "left" ); break;
    case PHIAItem::EPMoveY: setStartValue( _item->y() ); setPropertyName( "top" ); break;
    case PHIAItem::EPMoveW: setStartValue( _item->width() ); setPropertyName( "width" ); break;
    case PHIAItem::EPMoveH: setStartValue( _item->height() ); setPropertyName( "height" ); break;
    case PHIAItem::EPOpacity: setStartValue( _item->opacity() ); setPropertyName( "opacity" ); break;
    case PHIAItem::EPRotateX: setStartValue( _item->xRotation() ); setPropertyName( "xRotation" ); break;
    case PHIAItem::EPRotateY: setStartValue( _item->yRotation() ); setPropertyName( "yRotation" ); break;
    case PHIAItem::EPRotateZ: setStartValue( _item->zRotation() ); setPropertyName( "zRotation" ); break;
    default:;
    }
    if ( _relative ) setEndValue( endValue().toReal()+startValue().toReal() );
    start();
}

PHIAFaderAnimation::PHIAFaderAnimation( PHIAItem *it, FType t, qreal endVal, int start, int duration,
    const QString &ease ) : PHIAAnimation( it, PHIAItem::EPOpacity, QVariant( endVal ), start, duration,
    ease, false, false ), _type( t )
{
}

void PHIAFaderAnimation::remove()
{
    if ( !_item ) return;
    if ( _item->opacity()<0.1 ) _item->setVisible( false );
    PHIAAnimation::remove();
}

void PHIAFaderAnimation::startExclusive()
{
    stopRunningAnimations();
    /*
    if ( _type==FIn ) {
        if ( _item->opacity()>.9 ) _item->setOpacity( 0 );
    } else {
        if ( _item->opacity()<.1 ) _item->setOpacity( 1 );
    }
    */

    setStartValue( _item->opacity() );
    _item->setVisible( true );
    setPropertyName( "opacity" );
    start();
}

PHIAVSlideAnimation::PHIAVSlideAnimation( PHIAItem *it, SDirection dir, int start, int duration,
    const QString &ease ) : PHIAAnimation( it, PHIAItem::EPMoveH, 0, start, duration,
    ease, false, false ), _direction( dir )
{
    connect( this, SIGNAL( stateChanged( QAbstractAnimation::State, QAbstractAnimation::State ) ),
        this, SLOT( slotStateChanged( QAbstractAnimation::State, QAbstractAnimation::State ) ) );
}

void PHIAVSlideAnimation::startExclusive()
{
    stopRunningAnimations(); // restoreSlideEffect and remove other "height" animations
    _orgHeight=_item->height();
    if ( PHI::isLayoutContainer( _item->wid() ) ) {
        GAbstractLayoutItem *l=qgraphicsitem_cast<GAbstractLayoutItem*>(_item->graphicsItem());
        l->prepareSlideEffect();
    } else if ( PHI::isWidgetItem( _item->wid() ) ) {
        GWidgetItem *git=qgraphicsitem_cast<GWidgetItem*>(_item->graphicsItem());
        _sizePolicy=git->sizePolicy();
        _minimumSize=git->minimumSize();
        git->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
        git->setMinimumSize( 0, 0 );
    }
    if ( _direction==SUp ) {
        setEndValue( 0 );
        setStartValue( _item->height() );
    } else { // SDown
        _item->setHeight( 0 );
        _item->setVisible( true );
        setStartValue( 0 );
        setEndValue( _orgHeight );
    }
    qDebug( "start excluive %s", _item->id().data() );
    setPropertyName( "height" );
    start();
}

void PHIAVSlideAnimation::slotStateChanged( QAbstractAnimation::State newState,
    QAbstractAnimation::State oldState )
{
    if ( !_item ) return;
    if ( newState!=Stopped && oldState!=Running ) return;
    qDebug( "restoring slide effect %s", _item->id().data() );
    if ( _direction==SUp ) {
        _item->setVisible( false );
    }
    if ( PHI::isLayoutContainer( _item->wid() ) ) {
        GAbstractLayoutItem *l=qgraphicsitem_cast<GAbstractLayoutItem*>(_item->graphicsItem());
        l->restoreSlideEffect();
    } else if ( PHI::isWidgetItem( _item->wid() ) ) {
        GWidgetItem *git=qgraphicsitem_cast<GWidgetItem*>(_item->graphicsItem());
        git->setSizePolicy( _sizePolicy );
        git->setMinimumSize( _minimumSize );
    }
    _item->setHeight( _orgHeight );
}
