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
#ifndef PHIITEM_H
#define PHIITEM_H

#include <QDataStream>
#include <QFlags>
#include <QTransform>
#include <QHash>
#include <QVariant>
#include "phi.h"

class PHIEXPORT PHIItem
{
    friend class SItem;
    friend PHIEXPORT QDataStream& operator<<( QDataStream&, const PHIItem* );
    friend PHIEXPORT QDataStream& operator>>( QDataStream&, PHIItem* );

public:
    PHIItem();
    virtual ~PHIItem();
    PHIItem( const PHIItem& );

    /** Basic attributes of the base class.
      * @warning  bit 32 is not useable since QFlag uses "int" internally. */
    enum Attribute { ANone=0x0, AChild=0x1, AWidth=0x2, AHeight=0x4, ATransform=0x8,
        AShear=0x10, AZValue=0x20, ATranslate=0x40, ARotate=0x80, ATransformPos=0x100,
        ADynamicData=0x200, AExtensionData=0x400, AEffectData=0x800, AEditorData=0x1000,
        AParent=0x2000, ADisabled=0x4000, AChecked=0x8000, AReadOnly=0x10000,
        AProperties=0x20000, AOpacity=0x40000, AVisible=0x80000, ATemplateItem=0x100000,
        ADraggable=0x200000, ADroppable=0x400000, AEnd=0x40000000 }; // AEnd is max value

    enum Property { PNone=0x0, PNoCache=0x1, PFont=0x2, PCheckable=0x4, PColor=0x8,
        POutlineColor=0x10, PWidth=0x20, PHeight=0x40, PDisable=0x80, PMaxSize=0x100,
        PStartAngle=0x200, PSpanAngle=0x400, PBuddy=0x800, PPattern=0x1000,
        PLine=0x2000, PPenWidth=0x4000, PDelimiter=0x8000, PText=0x10000, PImage=0x20000,
        PAlignment=0x40000, PUrl=0x80000, PStyleSheet=0x100000, PToolTip=0x200000,
        PVisible=0x400000, PReadOnly=0x800000, PLabel=0x1000000, PShortCut=0x2000000,
        PImageBook=0x4000000, PStringList=0x8000000, PTabOrder=0x10000000,
        PRolloverColors=0x20000000, PEnd=0x40000000 }; // PEnd is max value
    enum Extension { XNone=0 };

    enum DragDropOption { DDNone=0, DDMoveAction=0x1, DDRevertOnIgnore=0x2, DDRevertOnAccept=0x4,
        DDHighlightOnMouseOver=0x8 };

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
        DDropAcceptedIds=50, DDragHotSpotType=51, DCursor=52
    }; //quint8 in DataStream

#ifdef PHIDEBUG
    Q_DECLARE_FLAGS( Properties, Property ) //qint32 in DataStream
    Q_DECLARE_FLAGS( Extensions, Extension ) //qint32 in DataStream
    Q_DECLARE_FLAGS( Attributes, Attribute ) //qint32 in DataStream
    Q_DECLARE_FLAGS( DragDropOptions, DragDropOption ) //stored in variant data
#else
    typedef qint32 Properties;
    typedef qint32 Extensions;
    typedef qint32 Attributes;
    typedef qint32 DragDropOptions;
#endif

    virtual QDataStream& load( QDataStream& );
    virtual QDataStream& save( QDataStream& ) const;
    virtual void loadFromByteArray( quint16 wid, const QByteArray &id, const QByteArray &arr );

    inline QByteArray id() const { return _id; }
    inline virtual void setId( const QByteArray &id ) { _id=id; }
    inline QByteArray parentId() const { return _parent; }
    inline virtual void setParentId( const QByteArray &pid ) { _parent=pid; }
    inline Attributes attributes() const { return _attributes; }
    inline Properties itemProperties() const { return _properties; }
    inline PHI::Widget wid() const { return _wid; }
    inline QVariant data( DataType dt ) const { return _variants.value( static_cast<quint8>(dt) ); }
    inline QVariant data( DataType dt, const QVariant &def ) const {
        return _variants.value( static_cast<quint8>(dt), def ); }
    inline void setData( DataType dt, const QVariant &v ) {
        _variants.insert( static_cast<quint8>(dt), v ); }

    inline qreal x() const { return _x; }
    inline virtual void setX( qreal x ) { _x=x; }
    inline qreal y() const { return _y; }
    inline virtual void setY( qreal y ) { _y=y; }
    inline qint16 zValue() const { return _zValue; }
    inline virtual void setZValue( qint16 z ) { _zValue=z; }
    inline qreal height() const { return _height; }
    inline virtual void setHeight( qreal h ) { _height=h; }
    inline qreal width() const { return _width; }
    inline virtual void setWidth( qreal w ) { _width=w; }
    inline QTransform transform() const { return _transform; }
    inline virtual void setTransform( const QTransform &t ) { _transform=t; }
    inline qreal opacity() const { return _opacity; }
    inline virtual void setOpacity( qreal o ) { _opacity=o; }

    inline virtual void setXRotation( qreal xr ) { _xRotate=xr; }
    inline virtual void setYRotation( qreal yr ) { _yRotate=yr; }
    inline virtual void setZRotation( qreal zr ) { _zRotate=zr; }
    inline virtual void setHShear( qreal hs ) { _sh=hs; }
    inline virtual void setVShear( qreal vs ) { _sv=vs; }
    inline virtual void setXTranslation( qreal xt ) { _xTranslate=xt; }
    inline virtual void setYTranslation( qreal yt ) { _yTranslate=yt; }
    inline qreal xRotation() const { return _xRotate; }
    inline qreal yRotation() const { return _yRotate; }
    inline qreal zRotation() const { return _zRotate; }
    inline qreal hShear() const { return _sh; }
    inline qreal vShear() const { return _sv; }
    inline qreal xTranslation() const { return _xTranslate; }
    inline qreal yTranslation() const { return _yTranslate; }

protected:
    PHI::Widget _wid;
    QByteArray _id, _parent;
    qreal _x, _y, _width, _height;
    QTransform _transform;
    mutable Attributes _attributes; // qint32 in DataStream
    qreal _sh, _sv, _xRotate, _yRotate, _zRotate, _xTranslate, _yTranslate, _opacity;
    quint8 _transformPos;
    qint16 _zValue;
    QByteArray _dynamicData, _editorData;
    mutable QByteArray _extensionData, _effectData;
    Properties _properties; // qint32 in DataStream
    mutable QHash <quint8, QVariant> _variants;
    Extensions _extensions; // qint32 in DataStream
};

#ifdef PHIDEBUG
Q_DECLARE_OPERATORS_FOR_FLAGS( PHIItem::Attributes )
Q_DECLARE_OPERATORS_FOR_FLAGS( PHIItem::Properties )
Q_DECLARE_OPERATORS_FOR_FLAGS( PHIItem::Extensions )
Q_DECLARE_OPERATORS_FOR_FLAGS( PHIItem::DragDropOptions )
#endif

Q_DECLARE_METATYPE( PHIItem::DragDropOptions )

PHIEXPORT QDataStream& operator<<( QDataStream&, const PHIItem* );
PHIEXPORT QDataStream& operator>>( QDataStream&, PHIItem* );

#endif // PHIITEM_H
