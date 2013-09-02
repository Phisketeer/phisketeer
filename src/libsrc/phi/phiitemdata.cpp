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
#include <QDataStream>
#include <QMetaType>
#include "phiitemdata.h"

PHIItemData::PHIItemData( QObject *parent )
    : QObject( parent ), _imageData( 0 ), _textData( 0 ), _styleSheetData( 0 ), _toolTipData( 0 ),
    _disabledData( 0 ), _visibleData( 0 ), _checkedData( 0 ), _readOnlyData( 0 ), _startAngleData( 0 ),
    _spanAngleData( 0 ), _imageBookData( 0 ), _stringListData( 0 )
{
    qDebug( "PHIItemData::PHIItemData()" );
}

PHIItemData::~PHIItemData()
{
    delete _imageData;
    delete _textData;
    delete _styleSheetData;
    delete _toolTipData;
    delete _disabledData;
    delete _visibleData;
    delete _checkedData;
    delete _readOnlyData;
    delete _startAngleData;
    delete _spanAngleData;
    delete _imageBookData;
    delete _stringListData;
    //qDebug( "PHIItemData::~PHIItemData()" );
}

PHIItemData& PHIItemData::operator=( const PHIItemData &d )
{
    if (d._imageData) *imageData()=*d.imageData();
    if (d._textData) *textData()=*d.textData();
    if (d._styleSheetData) *styleSheetData()=*d.styleSheetData();
    if (d._toolTipData) *toolTipData()=*d.toolTipData();
    if (d._disabledData) *disabledData()=*d.disabledData();
    if (d._visibleData) *visibleData()=*d.visibleData();
    if (d._checkedData) *checkedData()=*d.checkedData();
    if (d._readOnlyData) *readOnlyData()=*d.readOnlyData();
    if (d._startAngleData) *startAngleData()=*d.startAngleData();
    if (d._spanAngleData) *spanAngleData()=*d.spanAngleData();
    if (d._imageBookData) *imageBookData()=*d.imageBookData();
    if (d._stringListData) *stringListData()=*d.stringListData();
    _variants=d._variants;
    return *this;
}

PHIItemData::PHIItemData( const PHIItemData &d )
    : QObject( 0 ), _imageData( 0 ), _textData( 0 ), _styleSheetData( 0 ), _toolTipData( 0 ),
    _disabledData( 0 ), _visibleData( 0 ), _checkedData( 0 ), _readOnlyData( 0 ), _startAngleData( 0 ),
    _spanAngleData( 0 ), _imageBookData( 0 ), _stringListData( 0 )
{
    if (d._imageData) *imageData()=*d.imageData();
    if (d._textData) *textData()=*d.textData();
    if (d._styleSheetData) *styleSheetData()=*d.styleSheetData();
    if (d._toolTipData) *toolTipData()=*d.toolTipData();
    if (d._disabledData) *disabledData()=*d.disabledData();
    if (d._visibleData) *visibleData()=*d.visibleData();
    if (d._checkedData) *checkedData()=*d.checkedData();
    if (d._readOnlyData) *readOnlyData()=*d.readOnlyData();
    if (d._startAngleData) *startAngleData()=*d.startAngleData();
    if (d._spanAngleData) *spanAngleData()=*d.spanAngleData();
    if (d._imageBookData) *imageBookData()=*d.imageBookData();
    if (d._stringListData) *stringListData()=*d.stringListData();
    _variants=d._variants;
}

QByteArray PHIItemData::save( qint32 p ) const
{
    QByteArray arr;
    QDataStream out( &arr, QIODevice::WriteOnly );
    out.setVersion( PHI_DSV );

/*
#ifdef PHIDEBUG
    if ( p & PImage ) qDebug( "image" );
    if ( p & PText ) qDebug( "TEXT=%s", qPrintable( textData()->text() ) );
    if ( p & PStyleSheet ) qDebug( "stylesheet" );
    if ( p & PToolTip ) qDebug( "tooltip" );
    if ( p & PDisable ) qDebug( "disable" );
    if ( p & PVisible ) qDebug( "visible" );
    if ( p & PCheckable ) qDebug( "checkable" );
    if ( p & PReadOnly ) qDebug( "readonly" );
    if ( p & PStartAngle ) qDebug( "startangle" );
    if ( p & PSpanAngle ) qDebug( "spanangle" );
    if ( p & PImageBook ) qDebug( "imagebook" );
    if ( p & PStringList ) qDebug( "stringlist" );
#endif
*/

    if ( p & PImage ) out << imageData();
    if ( p & PText ) out << textData();
    if ( p & PStyleSheet ) out << styleSheetData();
    if ( p & PToolTip ) out << toolTipData();
    if ( p & PDisable ) out << disabledData();
    if ( p & PVisible ) out << visibleData();
    if ( p & PCheckable ) out << checkedData();
    if ( p & PReadOnly ) out << readOnlyData();
    if ( p & PStartAngle ) out << startAngleData();
    if ( p & PSpanAngle ) out << spanAngleData();
    if ( p & PImageBook ) out << imageBookData();
    if ( p & PStringList ) out << stringListData();

    if ( !(p & PColor) ) _variants.remove( DColor );
    if ( !(p & POutlineColor) ) _variants.remove( DOutlineColor );
    if ( !(p & PRolloverColors) ) {
        _variants.remove( DRolloverTextColor );
        _variants.remove( DRolloverBackgroundColor );
    }
    if ( !(p & PFont) ) _variants.remove( DFont );
    if ( !(p & PPenWidth) ) _variants.remove( DPenWidth );
    if ( !(p & PLine) ) _variants.remove( DLine );
    if ( !(p & PPattern) ) _variants.remove( DPattern );
    if ( _variants.value( DPattern, 0 ).value<quint8>() < 15 ) {
        _variants.remove( DGradientType );
        _variants.remove( DStartPoint );
        _variants.remove( DFinalStopPoint );
        _variants.remove( DCenterPoint );
        _variants.remove( DFocalPoint );
        _variants.remove( DAngle );
        _variants.remove( DRadius );
        _variants.remove( DStopPoints );
        _variants.remove( DSpreadType );
    }
    // PPalette is used globaly by page
    //if ( !(p & PPalette) ) _variants.remove( DPalette );
    if ( !(p & PAlignment) ) _variants.remove( DAlignment );
    if ( !(p & PMaxSize) ) _variants.remove( DMaxSize );
    if ( !(p & PUrl ) ) _variants.remove( DUrl );
    if ( !(p & PTabOrder ) ) _variants.remove( DTabOrder );
    out << _variants;

/*
#ifdef PHIDEBUG
    quint8 id;
    foreach ( id, _variants.keys() ) qDebug( "%d %s", id, qPrintable( _variants.value( id ).toString() ) );
    qDebug( "variants count %d", _variants.count() );
#endif
*/
    return arr;
}

void PHIItemData::setGradient( QLinearGradient g )
{
    _variants.insert( DStartPoint, g.start() );
    _variants.insert( DFinalStopPoint, g.finalStop() );
    _variants.insert( DSpreadType, static_cast<quint8>(g.spread()) );
    QVariant v;
    v.setValue( g.stops() );
    _variants.insert( DStopPoints, v );
    _variants.insert( DGradientType, static_cast<quint8>(g.type()) );
}

void PHIItemData::setGradient( QConicalGradient g )
{
    _variants.insert( DAngle, g.angle() );
    _variants.insert( DCenterPoint, g.center() );
    _variants.insert( DSpreadType, static_cast<quint8>(g.spread()) );
    QVariant v;
    v.setValue( g.stops() );
    _variants.insert( DStopPoints, v );
    _variants.insert( DGradientType, static_cast<quint8>(g.type()) );
}

void PHIItemData::setGradient( QRadialGradient g )
{
    _variants.insert( DFocalPoint, g.focalPoint() );
    _variants.insert( DRadius, g.radius() );
    _variants.insert( DCenterPoint, g.center() );
    _variants.insert( DSpreadType, static_cast<quint8>(g.spread()) );
    QVariant v;
    v.setValue( g.stops() );
    _variants.insert( DStopPoints, v );
    _variants.insert( DGradientType, static_cast<quint8>(g.type()) );
}

QGradient PHIItemData::gradient() const
{
    QGradient g;
    QGradient::Type type=static_cast<QGradient::Type>(_variants.value(
        DGradientType, 0 ).value<quint8>());
    if ( type==QGradient::ConicalGradient ) {
        g=QConicalGradient( _variants.value( DCenterPoint, QPointF( 0.5, 0.5 ) ).toPointF(),
            _variants.value( DAngle, 0. ).value<qreal>() );
    } else if ( type==QGradient::RadialGradient ) {
        g=QRadialGradient( _variants.value( DCenterPoint, QPointF( 0.5, 0.5 ) ).toPointF(),
            _variants.value( DRadius, 0.5 ).value<qreal>(),
            _variants.value( DFocalPoint, QPointF( 0.1, 0.1 ) ).toPointF() );
    } else {
        g=QLinearGradient( _variants.value( DStartPoint, QPointF( 0., 0. ) ).toPointF(),
            _variants.value( DFinalStopPoint, QPointF( 1.0, 1.0 ) ).toPointF() );
    }
    QGradientStops stops=_variants.value( DStopPoints ).value<QGradientStops>();
    if ( stops.count() ) g.setStops( stops );
    g.setSpread( static_cast<QGradient::Spread>(_variants.value( DSpreadType, 0 )
        .value<quint8>()) );
    g.setCoordinateMode( QGradient::ObjectBoundingMode );
    return g;
}

QLinearGradient PHIItemData::linearGradient() const
{
    QLinearGradient g=QLinearGradient(
        _variants.value( DStartPoint, QPointF( 0., 0. ) ).toPointF(),
        _variants.value( DFinalStopPoint, QPointF( 1.0, 1.0 ) ).toPointF() );
    QGradientStops stops=_variants.value( DStopPoints ).value<QGradientStops>();
    if ( stops.count() ) g.setStops( stops );
    g.setSpread( static_cast<QGradient::Spread>(_variants.value( DSpreadType, 0 )
        .value<quint8>()) );
    g.setCoordinateMode( QGradient::ObjectBoundingMode );
    return g;
}

QConicalGradient PHIItemData::conicalGradient() const
{
    QConicalGradient g=QConicalGradient(
        _variants.value( DCenterPoint, QPointF( 0.5, 0.5 ) ).toPointF(),
        _variants.value( DAngle, 0. ).value<qreal>() );
    QGradientStops stops=_variants.value( DStopPoints ).value<QGradientStops>();
    if ( stops.count() ) g.setStops( stops );
    g.setSpread( static_cast<QGradient::Spread>(_variants.value( DSpreadType, 0 )
        .value<quint8>()) );
    g.setCoordinateMode( QGradient::ObjectBoundingMode );
    return g;
}

QRadialGradient PHIItemData::radialGradient() const
{
    QRadialGradient g=QRadialGradient(
        _variants.value( DCenterPoint, QPointF( 0.5, 0.5 ) ).toPointF(),
        _variants.value( DRadius, 0.5 ).value<qreal>(),
        _variants.value( DFocalPoint, QPointF( 0.1, 0.1 ) ).toPointF() );
    QGradientStops stops=_variants.value( DStopPoints ).value<QGradientStops>();
    if ( stops.count() ) g.setStops( stops );
    g.setSpread( static_cast<QGradient::Spread>(_variants.value( DSpreadType, 0 )
        .value<quint8>()) );
    g.setCoordinateMode( QGradient::ObjectBoundingMode );
    return g;
}

void PHIItemData::load( qint32 p, const QByteArray &data )
{
    QByteArray arr=data;
    QDataStream in( &arr, QIODevice::ReadOnly );
    in.setVersion( PHI_DSV );
    if ( p & PImage ) {
        if ( !_imageData ) _imageData=new PHIImageData();
        in >> _imageData;
        //qDebug( "image" );
    }
    if ( p & PText ) {
        if ( !_textData ) _textData=new PHITextData();
        in >> _textData;
        //qDebug( "TEXT=%s", qPrintable( textData()->text() ) );
    }
    if ( p & PStyleSheet ) {
        if ( !_styleSheetData ) _styleSheetData=new PHITextData();
        in >> _styleSheetData;
        //qDebug( "stylesheet" );
    }
    if ( p & PToolTip ) {
        if ( !_toolTipData ) _toolTipData=new PHITextData();
        in >> _toolTipData;
        //qDebug( "tooltip" );
    }
    if ( p & PDisable ) {
        if ( !_disabledData ) _disabledData=new PHIBooleanData();
        in >> _disabledData;
        //qDebug( "disable" );
    }
    if ( p & PVisible ) {
        if ( !_visibleData ) _visibleData=new PHIBooleanData();
        in >> _visibleData;
        //qDebug( "visible" );
    }
    if ( p & PCheckable ) {
        if ( !_checkedData ) _checkedData=new PHIBooleanData();
        in >> _checkedData;
        //qDebug( "checkable" );
    }
    if ( p & PReadOnly ) {
        if ( !_readOnlyData ) _readOnlyData=new PHIBooleanData();
        in >> _readOnlyData;
        //qDebug( "readonly" );
    }
    if ( p & PStartAngle ) {
        if ( !_startAngleData ) _startAngleData=new PHIIntData();
        in >> _startAngleData;
        //qDebug( "startangle" );
    }
    if ( p & PSpanAngle ) {
        if ( !_spanAngleData ) _spanAngleData=new PHIIntData();
        in >> _spanAngleData;
        //qDebug( "spanangle" );
    }
    if ( p & PImageBook ) {
        if ( !_imageBookData ) _imageBookData=new PHIImageBookData();
        //qDebug( "before imagebook" );
        in >> _imageBookData;
        qDebug( "imagebook" );
    }
    if ( p & PStringList ) {
        if ( !_stringListData ) _stringListData=new PHIStringListData();
        in >> _stringListData;
        //qDebug( "stringlist" );
    }
    in >> _variants;

#ifdef PHIDEBUG
    qDebug() << _variants;
#endif
}
