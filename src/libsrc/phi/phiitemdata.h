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
#ifndef PHIITEMDATA_H
#define PHIITEMDATA_H

#include <QObject>
#include <QByteArray>
#include <QColor>
#include <QFont>
#include <QPalette>
#include <QHash>
#include <QApplication>
#include <QVariant>
#include "phidatasources.h"
#include "phi.h"

/*
    WARNING:
    This class is obsolete and mainly used to provide the old data structures for version 1_x.
    Do not use it in new code!
*/

class PHIEXPORT PHIItemData : public QObject
{
    Q_OBJECT

public:
    enum DataType { DNone=0, DPalette=1, DFont=2, DColor=3, DOutlineColor=4, DMaxSize=5,
        DStartAngle=6, DSpanAngle=7, DUrl=8, DPattern=9, DLine=10, DPenWidth=11, DStopPoints=12,
        DText=13, DImage=14, DAlignment=15, DStyleSheet=16, DToolTip=17, DLabel=18, DChildIds=19,
        DGridLayoutInfo=20, DDelimiter=21, DBuddy=22, DTabOrder=23, DShortCut=24,
        DRolloverTextColor=25, DRolloverBackgroundColor=26, DImageBook=27, DCurFadeIn=28,
        DCurFadeOut=29, DMaxFadeIn=30, DMinFadeOut=31, DCurMoveDeltaX=32, DCurMoveDeltaY=33,
        DCurMoveTime=34, DStartPoint=35, DFinalStopPoint=36, DCenterPoint=37, DFocalPoint=38,
        DAngle=39, DRadius=40, DSpreadType=41, DGradientType=42, DFadeTime=DStartAngle,
        DFadeInterval=DSpanAngle, DBorderRadius=DStartAngle, DDragStartPos=43, DDragOriginalPos=44,
        DDragHotSpot=45, DDragCursor=46, DDragDistance=47, DDragOpacity=48, DDragDropOptions=49,
        DDropAcceptedIds=50, DDragHotSpotType=51, DCursor=52, DLayoutOptions=53, DLayoutTitleHeight=54
    }; //quint8 in DataStream
    enum Property { PNone=0x0, PNoCache=0x1, PFont=0x2, PCheckable=0x4, PColor=0x8,
        POutlineColor=0x10, PWidth=0x20, PHeight=0x40, PDisable=0x80, PMaxSize=0x100,
        PStartAngle=0x200, PSpanAngle=0x400, PBuddy=0x800, PPattern=0x1000,
        PLine=0x2000, PPenWidth=0x4000, PDelimiter=0x8000, PText=0x10000, PImage=0x20000,
        PAlignment=0x40000, PUrl=0x80000, PStyleSheet=0x100000, PToolTip=0x200000,
        PVisible=0x400000, PReadOnly=0x800000, PLabel=0x1000000, PShortCut=0x2000000,
        PImageBook=0x4000000, PStringList=0x8000000, PTabOrder=0x10000000,
        PRolloverColors=0x20000000, PEnd=0x40000000 }; // PEnd is max value

    PHIItemData( QObject *parent=0 );
    virtual ~PHIItemData();
    PHIItemData& operator =( const PHIItemData& );
    PHIItemData( const PHIItemData& );

    //QByteArray save( qint32 properties ) const;
    void load( qint32 properties, const QByteArray& );
    //inline QHash <quint8, QVariant> variants() const { return _variants; }
    //inline void setVariants( const QHash <quint8, QVariant> &h ) { _variants=h; }
    //inline void removeData( quint8 t ) { _variants.remove( t ); }

    // some helper functions
    //inline void setChildIds( const PHIByteArrayList &list ) {
    //    _variants.insert( DChildIds, qVariantFromValue( list ) ); }
    inline PHIByteArrayList childIds() const {
        return _variants.value( DChildIds ).value<PHIByteArrayList>(); }
    //inline void setGridLayoutInfo( const PHIRectHash &h ) {
    //    _variants.insert( DGridLayoutInfo,  qVariantFromValue( h ) ); }
    inline PHIRectHash gridLayoutInfo() const {
        return _variants.value( DGridLayoutInfo ).value<PHIRectHash>(); }

    inline PHIImageData* imageData() const {
        return _imageData ? _imageData : _imageData=new PHIImageData(); }
    inline PHITextData* textData() const {
        return _textData ? _textData : _textData=new PHITextData(); }
    inline PHITextData* styleSheetData() const {
        return _styleSheetData ? _styleSheetData : _styleSheetData=new PHITextData(); }
    inline PHIStringListData* stringListData() const {
        return _stringListData ? _stringListData : _stringListData=new PHIStringListData(); }
    inline PHITextData* toolTipData() const {
        return _toolTipData ? _toolTipData : _toolTipData=new PHITextData(); }
    inline PHIBooleanData* disabledData() const {
        return _disabledData ? _disabledData : _disabledData=new PHIBooleanData(false); }
    inline PHIBooleanData* visibleData() const {
        return _visibleData ? _visibleData : _visibleData=new PHIBooleanData(true); }
    inline PHIBooleanData* checkedData() const {
        return _checkedData ? _checkedData : _checkedData=new PHIBooleanData(false); }
    inline PHIBooleanData* readOnlyData() const {
        return _readOnlyData ? _readOnlyData : _readOnlyData=new PHIBooleanData(false); }
    inline PHIIntData* startAngleData() const {
        return _startAngleData ? _startAngleData : _startAngleData=new PHIIntData(0); }
    inline PHIIntData* spanAngleData() const {
        return _spanAngleData ? _spanAngleData : _spanAngleData=new PHIIntData(5760); }
    inline PHIImageBookData* imageBookData() const {
        return _imageBookData ? _imageBookData : _imageBookData=new PHIImageBookData(); }

    inline QColor color() const {
        return _variants.value( DColor, QColor( Qt::black ) ).value<QColor>(); }
    inline QColor outlineColor() const {
        return _variants.value( DOutlineColor, QColor( Qt::black ) ).value<QColor>(); }
    inline QColor rolloverTextColor() const {
        return _variants.value( DRolloverTextColor, QColor() ).value<QColor>(); }
    inline QColor rolloverBackgroundColor() const {
        return _variants.value( DRolloverBackgroundColor, QColor() ).value<QColor>(); }
    inline QFont font() const {
        return _variants.value( DFont, PHI::invalidFont() ).value<QFont>(); }
    inline qreal penWidth() const {
        return static_cast<qreal>(_variants.value( DPenWidth, 1. ).toDouble()); }
    inline quint8 line() const { return _variants.value( DLine, 0 ).value<quint8>(); }
    inline quint8 pattern() const { return _variants.value( DPattern, 1 ).value<quint8>(); }
    inline quint8 alignment() const { return _variants.value( DAlignment, 0 ).value<quint8>(); }
    //inline QPalette palette() const {
    //    return _variants.value( DPalette, qApp->palette() ).value<QPalette>(); }
    inline quint16 maxSize() const {
        return _variants.value( DMaxSize, 100 ).value<quint16>(); }
    inline quint16 tabOrder() const { return _variants.value( DTabOrder, 0 ).value<quint16>(); }
    inline QString url() const { return QString::fromUtf8( _variants.value( DUrl ).toByteArray() ); }
    inline QString shortCut() const { return textData()->shortCut(); }
    inline QString label() const { return textData()->label(); }
    inline QString buddy() const { return textData()->buddy(); }
    inline QVariant data( DataType t ) const { return _variants.value( t ); }

    /*
    inline void setData( DataType t, const QVariant &v ) { _variants.insert( t, v ); }
    inline void setColor( const QColor &c ) { _variants.insert( DColor, c ); }
    inline void setOutlineColor( const QColor &c ) { _variants.insert( DOutlineColor, c ); }
    inline void setRolloverTextColor( const QColor &c )
        { _variants.insert( DRolloverTextColor, c ); }
    inline void setRolloverBackgroundColor( const QColor &c )
        { _variants.insert( DRolloverBackgroundColor, c ); }
    inline void setFont( const QFont &f ) { _variants.insert( DFont, f ); }
    inline void setPenWidth( qreal p ) { _variants.insert( DPenWidth, static_cast<double>(p) ); }
    inline void setLine( quint8 l ) { _variants.insert( DLine, l ); }
    inline void setPattern( quint8 p ) { _variants.insert( DPattern, p ); }
    inline void setAlignment( quint8 a ) { _variants.insert( DAlignment, a ); }
    inline void setMaxSize( quint16 ms ) { _variants.insert( DMaxSize, ms ); }
    inline void setTabOrder( quint16 to ) { _variants.insert( DTabOrder, to ); }
    inline void setUrl( const QString &url ) { _variants.insert( DUrl, url.toUtf8() ); }
    inline void setShortCut( const QString &sc ) { textData()->setShortCut( sc.left(1) ); }
    inline void setLabel( const QString &l ) { textData()->setLabel( l ); }
    inline void setBuddy( const QString &b ) { textData()->setBuddy( b ); }

    void setGradient( QLinearGradient g );
    void setGradient( QConicalGradient g );
    void setGradient( QRadialGradient g );
    */
    QGradient gradient() const;
    QLinearGradient linearGradient() const;
    QConicalGradient conicalGradient() const;
    QRadialGradient radialGradient() const;
    inline bool hasGradient() const { return _variants.contains( DGradientType ); }

private:
    mutable PHIImageData *_imageData;
    mutable PHITextData *_textData, *_styleSheetData, *_toolTipData;
    mutable PHIBooleanData *_disabledData, *_visibleData, *_checkedData, *_readOnlyData;
    mutable PHIIntData *_startAngleData, *_spanAngleData;
    mutable PHIImageBookData *_imageBookData;
    mutable PHIStringListData *_stringListData;
    mutable QHash <quint8, QVariant> _variants;
};

Q_DECLARE_METATYPE( PHIItemData )

#endif // PHIITEMDATA_H
