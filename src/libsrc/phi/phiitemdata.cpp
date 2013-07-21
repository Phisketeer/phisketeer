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
    // qDebug( "PHIItemData::PHIItemData()" );
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

PHIItemData& PHIItemData::operator =( const PHIItemData &d )
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
    //qDebug( "operator=(TEXT=%s)", qPrintable( textData()->text() ) );
    return *this;
}

PHIItemData::PHIItemData( const PHIItemData &d )
    : QObject( 0 ), _imageData( 0 ), _textData( 0 ), _styleSheetData( 0 ), _toolTipData( 0 ),
    _disabledData( 0 ), _visibleData( 0 ), _checkedData( 0 ), _readOnlyData( 0 ), _startAngleData( 0 ),
    _spanAngleData( 0 ), _imageBookData( 0 ), _stringListData( 0 )
{
    //qDebug( "copy constructor(TEXT=%s)", qPrintable( d.textData()->text() ) );
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

QByteArray PHIItemData::save( PHIItem::Properties p ) const
{
    QByteArray arr;
    QDataStream out( &arr, QIODevice::WriteOnly );
    out.setVersion( PHI_DSV );

/*
#ifdef PHIDEBUG
    if ( p & PHIItem::PImage ) qDebug( "image" );
    if ( p & PHIItem::PText ) qDebug( "TEXT=%s", qPrintable( textData()->text() ) );
    if ( p & PHIItem::PStyleSheet ) qDebug( "stylesheet" );
    if ( p & PHIItem::PToolTip ) qDebug( "tooltip" );
    if ( p & PHIItem::PDisable ) qDebug( "disable" );
    if ( p & PHIItem::PVisible ) qDebug( "visible" );
    if ( p & PHIItem::PCheckable ) qDebug( "checkable" );
    if ( p & PHIItem::PReadOnly ) qDebug( "readonly" );
    if ( p & PHIItem::PStartAngle ) qDebug( "startangle" );
    if ( p & PHIItem::PSpanAngle ) qDebug( "spanangle" );
    if ( p & PHIItem::PImageBook ) qDebug( "imagebook" );
    if ( p & PHIItem::PStringList ) qDebug( "stringlist" );
#endif
*/

    if ( p & PHIItem::PImage ) out << imageData();
    if ( p & PHIItem::PText ) out << textData();
    if ( p & PHIItem::PStyleSheet ) out << styleSheetData();
    if ( p & PHIItem::PToolTip ) out << toolTipData();
    if ( p & PHIItem::PDisable ) out << disabledData();
    if ( p & PHIItem::PVisible ) out << visibleData();
    if ( p & PHIItem::PCheckable ) out << checkedData();
    if ( p & PHIItem::PReadOnly ) out << readOnlyData();
    if ( p & PHIItem::PStartAngle ) out << startAngleData();
    if ( p & PHIItem::PSpanAngle ) out << spanAngleData();
    if ( p & PHIItem::PImageBook ) out << imageBookData();
    if ( p & PHIItem::PStringList ) out << stringListData();

    if ( !(p & PHIItem::PColor) ) _variants.remove( PHIItem::DColor );
    if ( !(p & PHIItem::POutlineColor) ) _variants.remove( PHIItem::DOutlineColor );
    if ( !(p & PHIItem::PRolloverColors) ) {
        _variants.remove( PHIItem::DRolloverTextColor );
        _variants.remove( PHIItem::DRolloverBackgroundColor );
    }
    if ( !(p & PHIItem::PFont) ) _variants.remove( PHIItem::DFont );
    if ( !(p & PHIItem::PPenWidth) ) _variants.remove( PHIItem::DPenWidth );
    if ( !(p & PHIItem::PLine) ) _variants.remove( PHIItem::DLine );
    if ( !(p & PHIItem::PPattern) ) _variants.remove( PHIItem::DPattern );
    if ( _variants.value( PHIItem::DPattern, 0 ).value<quint8>() < 15 ) {
        _variants.remove( PHIItem::DGradientType );
        _variants.remove( PHIItem::DStartPoint );
        _variants.remove( PHIItem::DFinalStopPoint );
        _variants.remove( PHIItem::DCenterPoint );
        _variants.remove( PHIItem::DFocalPoint );
        _variants.remove( PHIItem::DAngle );
        _variants.remove( PHIItem::DRadius );
        _variants.remove( PHIItem::DStopPoints );
        _variants.remove( PHIItem::DSpreadType );
    }
    // PPalette is used globaly by page
    //if ( !(p & PHIItem::PPalette) ) _variants.remove( PHIItem::DPalette );
    if ( !(p & PHIItem::PAlignment) ) _variants.remove( PHIItem::DAlignment );
    if ( !(p & PHIItem::PMaxSize) ) _variants.remove( PHIItem::DMaxSize );
    if ( !(p & PHIItem::PUrl ) ) _variants.remove( PHIItem::DUrl );
    if ( !(p & PHIItem::PTabOrder ) ) _variants.remove( PHIItem::DTabOrder );
    // PLabel, PBuddy, PShortCut are stored in _textData:
    //if ( p & PHIItem::PLabel ) out << textData();
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
    _variants.insert( PHIItem::DStartPoint, g.start() );
    _variants.insert( PHIItem::DFinalStopPoint, g.finalStop() );
    _variants.insert( PHIItem::DSpreadType, static_cast<quint8>(g.spread()) );
    QVariant v;
    v.setValue( g.stops() );
    _variants.insert( PHIItem::DStopPoints, v );
    _variants.insert( PHIItem::DGradientType, static_cast<quint8>(g.type()) );
}

void PHIItemData::setGradient( QConicalGradient g )
{
    _variants.insert( PHIItem::DAngle, g.angle() );
    _variants.insert( PHIItem::DCenterPoint, g.center() );
    _variants.insert( PHIItem::DSpreadType, static_cast<quint8>(g.spread()) );
    QVariant v;
    v.setValue( g.stops() );
    _variants.insert( PHIItem::DStopPoints, v );
    _variants.insert( PHIItem::DGradientType, static_cast<quint8>(g.type()) );
}

void PHIItemData::setGradient( QRadialGradient g )
{
    _variants.insert( PHIItem::DFocalPoint, g.focalPoint() );
    _variants.insert( PHIItem::DRadius, g.radius() );
    _variants.insert( PHIItem::DCenterPoint, g.center() );
    _variants.insert( PHIItem::DSpreadType, static_cast<quint8>(g.spread()) );
    QVariant v;
    v.setValue( g.stops() );
    _variants.insert( PHIItem::DStopPoints, v );
    _variants.insert( PHIItem::DGradientType, static_cast<quint8>(g.type()) );
}

QGradient PHIItemData::gradient() const
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
    QGradientStops stops=_variants.value( PHIItem::DStopPoints ).value<QGradientStops>();
    if ( stops.count() ) g.setStops( stops );
    g.setSpread( static_cast<QGradient::Spread>(_variants.value( PHIItem::DSpreadType, 0 )
        .value<quint8>()) );
    g.setCoordinateMode( QGradient::ObjectBoundingMode );
    return g;
}

QLinearGradient PHIItemData::linearGradient() const
{
    QLinearGradient g=QLinearGradient(
        _variants.value( PHIItem::DStartPoint, QPointF( 0., 0. ) ).toPointF(),
        _variants.value( PHIItem::DFinalStopPoint, QPointF( 1.0, 1.0 ) ).toPointF() );
    QGradientStops stops=_variants.value( PHIItem::DStopPoints ).value<QGradientStops>();
    if ( stops.count() ) g.setStops( stops );
    g.setSpread( static_cast<QGradient::Spread>(_variants.value( PHIItem::DSpreadType, 0 )
        .value<quint8>()) );
    g.setCoordinateMode( QGradient::ObjectBoundingMode );
    return g;
}

QConicalGradient PHIItemData::conicalGradient() const
{
    QConicalGradient g=QConicalGradient(
        _variants.value( PHIItem::DCenterPoint, QPointF( 0.5, 0.5 ) ).toPointF(),
        _variants.value( PHIItem::DAngle, 0. ).value<qreal>() );
    QGradientStops stops=_variants.value( PHIItem::DStopPoints ).value<QGradientStops>();
    if ( stops.count() ) g.setStops( stops );
    g.setSpread( static_cast<QGradient::Spread>(_variants.value( PHIItem::DSpreadType, 0 )
        .value<quint8>()) );
    g.setCoordinateMode( QGradient::ObjectBoundingMode );
    return g;
}

QRadialGradient PHIItemData::radialGradient() const
{
    QRadialGradient g=QRadialGradient(
        _variants.value( PHIItem::DCenterPoint, QPointF( 0.5, 0.5 ) ).toPointF(),
        _variants.value( PHIItem::DRadius, 0.5 ).value<qreal>(),
        _variants.value( PHIItem::DFocalPoint, QPointF( 0.1, 0.1 ) ).toPointF() );
    QGradientStops stops=_variants.value( PHIItem::DStopPoints ).value<QGradientStops>();
    if ( stops.count() ) g.setStops( stops );
    g.setSpread( static_cast<QGradient::Spread>(_variants.value( PHIItem::DSpreadType, 0 )
        .value<quint8>()) );
    g.setCoordinateMode( QGradient::ObjectBoundingMode );
    return g;
}

void PHIItemData::load( PHIItem::Properties p, const QByteArray &data )
{
    QByteArray arr=data;
    QDataStream in( &arr, QIODevice::ReadOnly );
    in.setVersion( PHI_DSV );
    if ( p & PHIItem::PImage ) {
        if ( !_imageData ) _imageData=new PHIImageData();
        in >> _imageData;
        //qDebug( "image" );
    }
    if ( p & PHIItem::PText ) {
        if ( !_textData ) _textData=new PHITextData();
        in >> _textData;
        //qDebug( "TEXT=%s", qPrintable( textData()->text() ) );
    }
    if ( p & PHIItem::PStyleSheet ) {
        if ( !_styleSheetData ) _styleSheetData=new PHITextData();
        in >> _styleSheetData;
        //qDebug( "stylesheet" );
    }
    if ( p & PHIItem::PToolTip ) {
        if ( !_toolTipData ) _toolTipData=new PHITextData();
        in >> _toolTipData;
        //qDebug( "tooltip" );
    }
    if ( p & PHIItem::PDisable ) {
        if ( !_disabledData ) _disabledData=new PHIBooleanData();
        in >> _disabledData;
        //qDebug( "disable" );
    }
    if ( p & PHIItem::PVisible ) {
        if ( !_visibleData ) _visibleData=new PHIBooleanData();
        in >> _visibleData;
        //qDebug( "visible" );
    }
    if ( p & PHIItem::PCheckable ) {
        if ( !_checkedData ) _checkedData=new PHIBooleanData();
        in >> _checkedData;
        //qDebug( "checkable" );
    }
    if ( p & PHIItem::PReadOnly ) {
        if ( !_readOnlyData ) _readOnlyData=new PHIBooleanData();
        in >> _readOnlyData;
        //qDebug( "readonly" );
    }
    if ( p & PHIItem::PStartAngle ) {
        if ( !_startAngleData ) _startAngleData=new PHIIntData();
        in >> _startAngleData;
        //qDebug( "startangle" );
    }
    if ( p & PHIItem::PSpanAngle ) {
        if ( !_spanAngleData ) _spanAngleData=new PHIIntData();
        in >> _spanAngleData;
        //qDebug( "spanangle" );
    }
    if ( p & PHIItem::PImageBook ) {
        if ( !_imageBookData ) _imageBookData=new PHIImageBookData();
        //qDebug( "before imagebook" );
        in >> _imageBookData;
        qDebug( "imagebook" );
    }
    if ( p & PHIItem::PStringList ) {
        if ( !_stringListData ) _stringListData=new PHIStringListData();
        in >> _stringListData;
        //qDebug( "stringlist" );
    }
    in >> _variants;

#ifdef PHIDEBUG
    quint8 id;
    foreach ( id, _variants.keys() ) qDebug( "%d %s", id, qPrintable( _variants.value( id ).toString() ) );
    qDebug( "variants count %d", _variants.count() );
#endif
}
