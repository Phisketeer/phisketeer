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
#include "phiitem.h"

PHIItem::PHIItem()
    : _wid( PHI::NO_ITEM ), _x( 0. ), _y( 0. ), _width( 0. ), _height( 0. ),
    _transform( QTransform() ), _attributes( PHIItem::ADynamicData ),
    _sh( 0. ), _sv( 0. ), _xRotate( 0. ), _yRotate( 0. ), _zRotate( 0. ), _xTranslate( 0. ),
    _yTranslate( 0. ), _opacity( 1. ), _transformPos( 1 ), _zValue( 0 ), _properties( PNone ),
    _extensions( XNone )
{
    //qDebug( "PHIItem::PHIItem()" );
}

PHIItem::~PHIItem()
{
    //qDebug( "PHIItem::~PHIItem()" );
}

PHIItem::PHIItem( const PHIItem &it )
{
    //qDebug( "PHIItem::PHIItem( const PHIItem& )" );
    _wid=it._wid;
    _id=it._id;
    _parent=it._parent;
    _x=it._x;
    _y=it._y;
    _width=it._width;
    _height=it._height;
    _attributes=it._attributes;
    _sh=it._sh;
    _sv=it._sv;
    _xRotate=it._xRotate;
    _yRotate=it._yRotate;
    _zRotate=it._zRotate;
    _xTranslate=it._xTranslate;
    _yTranslate=it._yTranslate;
    _opacity=it._opacity;
    _transformPos=it._transformPos;
    _zValue=it._zValue;
    _transform=it._transform;
    _properties=it._properties;
    _dynamicData=it._dynamicData;
    _editorData=it._editorData;
    _effectData=it._effectData;
    _extensionData=it._extensionData;
    _variants=it._variants;
    _extensions=it._extensions;
}

QDataStream& PHIItem::save( QDataStream &out ) const
{
    QByteArray arr;
    QDataStream ds( &arr, QIODevice::WriteOnly );
    ds.setVersion( PHI_DSV );

    _attributes|= AWidth | AHeight | AZValue | AProperties; // may change later
    if ( _parent.isEmpty() ) _attributes&= ~AParent;
    else _attributes|= AParent;
    if ( _opacity==1. ) _attributes&= ~AOpacity;
    else _attributes|= AOpacity;

    ds  << static_cast<qint32>(ANone) << _x << _y;
    //qDebug( "SAVING item %s wid=%d width=%f", _id.data(), _wid, _width );
    if ( _attributes & AWidth ) ds << _width;
    if ( _attributes & AHeight ) ds << _height;
    if ( _attributes & AZValue ) ds << _zValue;
    if ( _attributes & AProperties ) ds << static_cast<qint32>(_properties);

    if ( _xRotate || _yRotate || _zRotate ) {
        qDebug( "PHIItem::save: writing rotation infos" );
        _attributes|= ARotate;
        ds << _xRotate << _yRotate << _zRotate;
    } else _attributes &= ~ARotate;
    if ( _sh || _sv ) {
        qDebug( "PHIItem::save: writing shearing infos" );
        _attributes|= AShear;
        ds << _sh << _sv;
    } else _attributes &= ~AShear;
    if ( _xTranslate || _yTranslate ) {
        qDebug( "PHIItem::save: writing translation infos" );
        _attributes|= ATranslate;
        ds << _xTranslate << _yTranslate;
    } else _attributes &= ~ATranslate;
    if ( _attributes & ATransform ) ds << _transform; // currently unused
    if ( _attributes & AParent ) ds << _parent;
    if ( _attributes & AOpacity ) ds << _opacity;

    if ( _attributes & ADynamicData ) ds << _dynamicData;
    if ( _attributes & AExtensionData ) ds << static_cast<qint32>(_extensions) << _extensionData;
    if ( _attributes & AEffectData ) ds << _effectData;
    if ( _attributes & AEditorData ) ds << _editorData;
    // Variants are stored via PHISItemData
    // if ( _attributes & AVariantData ) ds << _variants;
    if ( _transformPos!=1 ) {
        _attributes|= ATransformPos;
        ds << _transformPos;
    } else _attributes &= ~ATransformPos;

    ds.device()->reset();
    ds << static_cast<qint32>(_attributes);
    //qDebug( "STORE att %lld", static_cast<quint64>(_attributes) );

    out << _id << static_cast<quint8>(_wid) << arr;
    qDebug( "ARRAY SIZE %d", arr.size() );
    return out;
}

QDataStream& PHIItem::load( QDataStream &in )
{
    quint8 wid;
    QByteArray arr;
    in >> _id >> wid >> arr;
    loadFromByteArray( static_cast<quint16>(wid), _id, arr );
    return in;
}

// note: wid is quint16 here if we load two bytes in future releases
void PHIItem::loadFromByteArray( quint16 wid, const QByteArray &id, const QByteArray &array ) 
{
    qint32 flags;
    _id=id;
    QByteArray arr=array;
    _wid=static_cast<PHI::Widget>(wid);

    QDataStream ds( &arr, QIODevice::ReadOnly );
    ds.setVersion( PHI_DSV );
    ds >> flags >> _x >> _y;
    _attributes=static_cast<PHIItem::Attributes>(flags);
    //qDebug( "LOAD att %d", static_cast<qint32>(_attributes) );

    if ( _attributes & AWidth ) ds >> _width;
    if ( _attributes & AHeight ) ds >> _height;
    if ( _attributes & AZValue ) ds >> _zValue;
    if ( _attributes & AProperties ) ds >> flags;
    else flags=0;
    _properties=static_cast<PHIItem::Properties>(flags);

    if ( _attributes & ARotate ) ds >> _xRotate >> _yRotate >> _zRotate;
    else _xRotate=_yRotate=_zRotate=0.;
    if ( _attributes & AShear ) ds >> _sh >> _sv;
    else _sh=_sv=0.;
    if ( _attributes & ATranslate ) ds >> _xTranslate >> _yTranslate;
    else _xTranslate=_yTranslate=0.;
    if ( _attributes & ATransform ) ds >> _transform;
    else _transform=QTransform();
    if ( _attributes & AParent ) ds >> _parent;
    else _parent=QByteArray();
    if ( _attributes & AOpacity ) ds >> _opacity;
    else _opacity=1.;

    if ( _attributes & ADynamicData ) ds >> _dynamicData;
    else _dynamicData.clear();
    if ( _attributes & AExtensionData ) {
        ds >> flags >> _extensionData;
        _extensions=static_cast<PHIItem::Extensions>(flags);
    } else {
        _extensionData.clear();
        _extensions=XNone;
    }
    if ( _attributes & AEffectData ) ds >> _effectData;
    else _effectData.clear();

    if ( _attributes & AEditorData ) ds >> _editorData;
    else _editorData.clear();
    // Variants are stored along with PHISItemData
    //if ( _attributes & AVariantData ) ds >> _variants;
    //else _variants.clear();
    if ( _attributes & ATransformPos) ds >> _transformPos;
    else _transformPos=1;
}

QDataStream& operator<<( QDataStream &out, const PHIItem *it )
{
    // only used for sending over TCP/IP, so remove EditorData and DynamicData
    it->_attributes &= ~PHIItem::AEditorData;
    it->_attributes &= ~PHIItem::ADynamicData;
    it->save( out );
    return out;
}

QDataStream& operator>>( QDataStream &in, PHIItem *it )
{
    it->load( in );
    return in;
}
