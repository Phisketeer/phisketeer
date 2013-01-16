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
#include <QMetaType>
#include "phibaseitem.h"

QScriptValue baseItemToScriptValue( QScriptEngine *engine, PHIBaseItem * const &it )
{
    return engine->newQObject( it, QScriptEngine::QtOwnership,
        QScriptEngine::PreferExistingWrapperObject |
        QScriptEngine::ExcludeSuperClassContents | QScriptEngine::ExcludeDeleteLater );
}

void baseItemFromScriptValue( const QScriptValue &obj, PHIBaseItem* &it )
{
    it=qobject_cast<PHIBaseItem*>(obj.toQObject());
}

PHIBaseItem::PHIBaseItem( QObject *parent )
    : QObject( parent ), PHIItem()
{
    //qDebug( "PHIBaseItem::PHIBaseItem()" );
    _effect=new PHIEffect();
    new PHIBaseStyle( this );
    new PHIBaseEffect( this );
}

PHIBaseItem::PHIBaseItem( const PHIItem &it, QObject *parent )
    : QObject( parent ), PHIItem( it )
{
    //qDebug( "PHIBaseItem::PHIBaseItem( const PHIItem& )" );
    _effect=new PHIEffect();
    new PHIBaseStyle( this );
    new PHIBaseEffect( this );
    _attributes &= ~PHIItem::ADynamicData;
    _attributes &= ~PHIItem::AEditorData;
    _effect->load( _effectData );
}

PHIBaseItem::PHIBaseItem( PHI::Widget wid, const QByteArray &id, QObject *parent )
    : QObject( parent ), PHIItem()
{
    _properties |= PHIItem::PNoCache;
    _effect=new PHIEffect();
    new PHIBaseStyle( this );
    new PHIBaseEffect( this );
    _wid=wid;
    _id=id;
}

PHIBaseItem::~PHIBaseItem()
{
    delete _effect;
    //qDebug( "PHIBaseItem::~PHIBaseItem()" );
}

QGradient PHIBaseItem::gradient() const
{
    QGradient g;

    QGradient::Type type=static_cast<QGradient::Type>(_variants.value(
        PHIItem::DGradientType, 0 ).value<quint8>());
    if ( type==QGradient::ConicalGradient ) {
        g=QConicalGradient( _variants.value( PHIItem::DCenterPoint, QPointF( 0.5, 0.5 ) ).toPointF(),
        _variants.value( PHIItem::DAngle, 0. ).value<qreal>() );
    } else if ( type==QGradient::RadialGradient ) {
        g=QRadialGradient( _variants.value( PHIItem::DCenterPoint, QPointF( 0.5, 0.5 ) ).toPointF(),
        _variants.value( PHIItem::DRadius, 0.5 ).value<qreal>(),
        _variants.value( PHIItem::DFocalPoint, QPointF( 0.1, 0.1 ) ).toPointF() );
    } else {
        g=QLinearGradient( _variants.value( PHIItem::DStartPoint, QPointF( 0., 0. ) ).toPointF(),
        _variants.value( PHIItem::DFinalStopPoint, QPointF( 1.0, 1.0 ) ).toPointF() );
    }
    g.setCoordinateMode( QGradient::ObjectBoundingMode );
    QGradientStops stops=_variants.value( PHIItem::DStopPoints ).value<QGradientStops>();
    if ( stops.count() ) g.setStops( stops );
    g.setSpread( static_cast<QGradient::Spread>(_variants.value( PHIItem::DSpreadType, 0 )
        .value<quint8>()) );
    return g;
}

void PHIBaseItem::setAlignment( quint8 a )
{
    if ( a==0 ) {
        _variants.remove( DAlignment );
        _properties &= ~PHIItem::PAlignment;
        return;
    }
    _variants.insert( DAlignment, a );
    _properties |= PHIItem::PAlignment;
}

void PHIBaseItem::setPenWidth( qreal w )
{
    if ( w==1. ) {
        _variants.remove( DPenWidth );
        _properties&= ~PHIItem::PPenWidth;
        return;
    }
    _variants.insert( DPenWidth, static_cast<double>(w) );
    _properties|= PHIItem::PPenWidth;
}

void PHIBaseItem::setColor( const QColor &c )
{
    if ( c==QColor( Qt::black ) || !c.isValid() ) {
        _variants.remove( DColor );
        _properties&= ~PHIItem::PColor;
        return;
    }
    _variants.insert( DColor, c );
    _properties|= PHIItem::PColor;
}

void PHIBaseItem::setOutlineColor( const QColor &c )
{
    if ( c==QColor( Qt::black ) || !c.isValid() ) {
        _variants.remove( DOutlineColor );
        _properties&= ~PHIItem::POutlineColor;
        return;
    }
    _variants.insert( DOutlineColor, c );
    _properties|= PHIItem::POutlineColor;
}

void PHIBaseItem::setRolloverTextColor( const QColor &c )
{
    _variants.insert( DRolloverTextColor, c );
    _properties |= PRolloverColors;
}

void PHIBaseItem::setRolloverBackgroundColor( const QColor &c )
{
    _variants.insert( DRolloverBackgroundColor, c );
    _properties |= PRolloverColors;
}

void PHIBaseItem::setPattern( quint8 p )
{
    if ( p > PHI::maxPatternStyle() || p==1 ) {
        _variants.remove( DPattern );
        _properties&= ~PHIItem::PPattern;
        return;
    }
    _variants.insert( DPattern, p );
    _properties|= PHIItem::PPattern;
}

void PHIBaseItem::setLine( quint8 l )
{
    if ( l > PHI::maxLineStyle() || l==0 ) {
        _variants.remove( DLine );
        _properties&= ~PHIItem::PLine;
        return;
    }
    _variants.insert( DLine, l );
    _properties|= PHIItem::PLine;
}

void PHIBaseItem::setStartAngle( qint16 a )
{
    if ( a==0 ) {
        _variants.remove( DStartAngle );
        _properties&= ~PHIItem::PStartAngle;
        return;
    }
    _variants.insert( DStartAngle, a );
    _properties|= PHIItem::PStartAngle;
}

void PHIBaseItem::setSpanAngle( qint16 a )
{
    if ( a==5760 ) {
        _variants.remove( DSpanAngle );
        _properties&= ~PHIItem::PSpanAngle;
        return;
    }
    _variants.insert( DSpanAngle, a );
    _properties|= PHIItem::PSpanAngle;
}

QDataStream& operator<<( QDataStream &out, const PHIBaseItem *it )
{
    //qRegisterMetaTypeStreamOperators<PHIRectHash>("PHIRectHash");
    //qRegisterMetaTypeStreamOperators<PHIByteArrayList>("PHIByteArrayList");
    //qRegisterMetaTypeStreamOperators<PHIImageHash>("PHIImageHash");
    //qRegisterMetaTypeStreamOperators<QGradientStops>("QGradientStops");
/*
    quint8 vid;
    foreach ( vid, it->_variants.keys() ) {
        qDebug( "%d=%d", vid, it->_variants.value( vid ).toInt() );
    }
*/
    //qDebug( "<< VARIANT count %d", it->_variants.count() );
    if ( it->_effect->effects()!=PHIEffect::ENone ) {
        it->_effectData=it->_effect->save();
    } else it->_effectData.clear();
    return out << dynamic_cast<const PHIItem*>(it) << it->_variants;
}

QDataStream& operator>>( QDataStream &in, PHIBaseItem *it )
{
    //qRegisterMetaTypeStreamOperators<PHIRectHash>("PHIRectHash");
    //qRegisterMetaTypeStreamOperators<PHIByteArrayList>("PHIByteArrayList");
    //qRegisterMetaTypeStreamOperators<PHIImageHash>("PHIImageHash");
    //qRegisterMetaTypeStreamOperators<QGradientStops>("QGradientStops");
    in >> dynamic_cast<PHIItem*>(it) >> it->_variants;
    //qDebug( ">> VARIANT count %d", it->_variants.count() );
    if ( it->attributes() & PHIItem::AEffectData ) it->_effect->load( it->_effectData );
    else it->_effect->clearAll();
    return in;
}

PHIBaseStyle::PHIBaseStyle( PHIBaseItem *it )
    : QObject( it ), _it( it )
{
    setObjectName( "style" );
}

PHIBaseStyle::~PHIBaseStyle()
{
}

void PHIBaseStyle::setVisibility( const QString &s )
{
    if ( s.toLower()==QString( "hidden" ) ) _it->setVisible( false );
    else _it->setVisible( true );
}

QString PHIBaseStyle::visibility() const
{
    if ( _it->visible() ) return QString( "visible" );
    else return QString( "hidden" );
}

PHIBaseEffect::PHIBaseEffect( PHIBaseItem *it )
    : QObject( it ), _it( it )
{
    setObjectName( "effect" );
}

PHIBaseEffect::~PHIBaseEffect()
{
}

void PHIBaseEffect::shadow( const QString &color, qreal opac, qreal xOff, qreal yOff, qreal radius )
{
    QColor c=QColor( color );
    c.setAlphaF( opac );
    _it->setShadow( c, xOff, yOff, radius );
}

void PHIBaseEffect::rotate( quint8 axis, qreal step, const QString &ease )
{
    qreal xstep=0, ystep=0, zstep=0;
    if ( axis & 0x1 ) xstep=step;
    if ( axis & 0x2 ) ystep=step;
    if ( axis & 0x4 ) zstep=step;
    _it->setRotate( axis, xstep, ystep, zstep, ease );
}
