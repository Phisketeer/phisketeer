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
#include "phiitem.h"
#include "phi.h"

class PHIEXPORT PHIItemData : public QObject
{
    Q_OBJECT

public:
    PHIItemData( QObject *parent=0 );
    virtual ~PHIItemData();
    PHIItemData& operator =( const PHIItemData& );
    PHIItemData( const PHIItemData& );

    QByteArray save( PHIItem::Properties ) const;
    void load( PHIItem::Properties, const QByteArray& );
    inline QHash <quint8, QVariant> variants() const { return _variants; }
    inline void setVariants( const QHash <quint8, QVariant> &h ) { _variants=h; }
    inline void removeData( PHIItem::DataType t ) { _variants.remove( t ); }

    // some helper functions
    inline void setChildIds( const PHIByteArrayList &list ) {
        _variants.insert( PHIItem::DChildIds, qVariantFromValue( list ) ); }
    inline PHIByteArrayList childIds() const {
        return _variants.value( PHIItem::DChildIds ).value<PHIByteArrayList>(); }
    inline void setGridLayoutInfo( const PHIRectHash &h ) {
        _variants.insert( PHIItem::DGridLayoutInfo,  qVariantFromValue( h ) ); }
    inline PHIRectHash gridLayoutInfo() const {
        return _variants.value( PHIItem::DGridLayoutInfo ).value<PHIRectHash>(); }

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
        return _variants.value( PHIItem::DColor, QColor( Qt::black ) ).value<QColor>(); }
    inline QColor outlineColor() const {
        return _variants.value( PHIItem::DOutlineColor, QColor( Qt::black ) ).value<QColor>(); }
    inline QColor rolloverTextColor() const {
        return _variants.value( PHIItem::DRolloverTextColor, QColor() ).value<QColor>(); }
    inline QColor rolloverBackgroundColor() const {
        return _variants.value( PHIItem::DRolloverBackgroundColor, QColor() ).value<QColor>(); }
    inline QFont font() const {
        return _variants.value( PHIItem::DFont, PHI::invalidFont() ).value<QFont>(); }
    inline qreal penWidth() const {
        return static_cast<qreal>(_variants.value( PHIItem::DPenWidth, 1. ).toDouble()); }
    inline quint8 line() const { return _variants.value( PHIItem::DLine, 0 ).value<quint8>(); }
    inline quint8 pattern() const { return _variants.value( PHIItem::DPattern, 1 ).value<quint8>(); }
    inline quint8 alignment() const { return _variants.value( PHIItem::DAlignment, 0 ).value<quint8>(); }
    //inline QPalette palette() const {
    //    return _variants.value( PHIItem::DPalette, qApp->palette() ).value<QPalette>(); }
    inline quint16 maxSize() const {
        return _variants.value( PHIItem::DMaxSize, 100 /* PHI::maxLength() */ ).value<quint16>(); }
    inline quint16 tabOrder() const { return _variants.value( PHIItem::DTabOrder, 0 ).value<quint16>(); }
    inline QString url() const { return QString::fromUtf8( _variants.value( PHIItem::DUrl ).toByteArray() ); }
    inline QString shortCut() const { return textData()->shortCut(); }
    inline QString label() const { return textData()->label(); }
    inline QString buddy() const { return textData()->buddy(); }
    inline QVariant data( PHIItem::DataType t ) const { return _variants.value( t ); }

    inline void setData( PHIItem::DataType t, const QVariant &v ) { _variants.insert( t, v ); }
    inline void setColor( const QColor &c ) { _variants.insert( PHIItem::DColor, c ); }
    inline void setOutlineColor( const QColor &c ) { _variants.insert( PHIItem::DOutlineColor, c ); }
    inline void setRolloverTextColor( const QColor &c )
        { _variants.insert( PHIItem::DRolloverTextColor, c ); }
    inline void setRolloverBackgroundColor( const QColor &c )
        { _variants.insert( PHIItem::DRolloverBackgroundColor, c ); }
    inline void setFont( const QFont &f ) { _variants.insert( PHIItem::DFont, f ); }
    inline void setPenWidth( qreal p ) { _variants.insert( PHIItem::DPenWidth, static_cast<double>(p) ); }
    inline void setLine( quint8 l ) { _variants.insert( PHIItem::DLine, l ); }
    inline void setPattern( quint8 p ) { _variants.insert( PHIItem::DPattern, p ); }
    inline void setAlignment( quint8 a ) { _variants.insert( PHIItem::DAlignment, a ); }
    inline void setMaxSize( quint16 ms ) { _variants.insert( PHIItem::DMaxSize, ms ); }
    inline void setTabOrder( quint16 to ) { _variants.insert( PHIItem::DTabOrder, to ); }
    inline void setUrl( const QString &url ) { _variants.insert( PHIItem::DUrl, url.toUtf8() ); }
    inline void setShortCut( const QString &sc ) { textData()->setShortCut( sc.left(1) ); }
    inline void setLabel( const QString &l ) { textData()->setLabel( l ); }
    inline void setBuddy( const QString &b ) { textData()->setBuddy( b ); }

    void setGradient( QLinearGradient g );
    void setGradient( QConicalGradient g );
    void setGradient( QRadialGradient g );
    QGradient gradient() const;
    QLinearGradient linearGradient() const;
    QConicalGradient conicalGradient() const;
    QRadialGradient radialGradient() const;
    inline bool hasGradient() const { return _variants.contains( PHIItem::DGradientType ); }

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
