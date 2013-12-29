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
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QMetaProperty>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QGraphicsScene>
#include <QEvent>
#include <QGraphicsSceneEvent>
#include <QMimeData>
#include <QShortcut>
#include <QComboBox>
#include <math.h>
#include "phibaseitem.h"
#include "phibasepage.h"
#include "phigraphicsitem.h"
#include "qpixmapfilter_p.h"
#include "phirequest.h"
#include "phiitemdata.h"
#include "phieffects.h"
#include "phiabstractitems.h"
#include "phidataparser.h"

/*
static QStringList _myproperties( const QObject *obj )
{
    Q_ASSERT( obj );
    const QMetaObject *mo=obj->metaObject();
    QStringList properties;
    for( int i=1; i < mo->propertyCount(); ++i ) {
        if ( mo->property( i ).isScriptable() )
            properties << QString::fromLatin1( mo->property( i ).name() );
    }
    properties << QStringLiteral( "style" ) << QStringLiteral( "effect" );
    properties.sort();
    return properties;
}
*/

PHIBaseItemPrivate::PHIBaseItemPrivate( const PHIBasePage *page )
    : _type( TIDEItem ), _page( const_cast<PHIBasePage*>(page) )
{
    PHIGraphicsItemProvider *provider=PHIGraphicsItemProvider::instance();
    Q_ASSERT( provider );
    _gw=provider->createGraphicsItem();
}

PHIBaseItem::PHIBaseItem( const PHIBaseItemPrivate &p ) : QObject( p.page() ),
    _type( p.type() ), _gw( p.gw() ), _flags( FNone ),
    _effect( new PHIEffect() ), _dirtyFlags( DFClean )
{
    if ( _gw ) p.gw()->setBaseItem( this );
    _visibleData.setBoolean( true );
    _x=_y=_xRot=_yRot=_zRot=_hSkew=_vSkew=0;
    setTransformPos( PHI::TopLeft );
}

PHIBaseItem::PHIBaseItem( const PHIBaseItem &it )
    : QObject( 0 ), _type( it._type ), _gw( 0 ),
    _id( it._id ), _parentId( it._parentId ), _x( it._x ), _y( it._y ),
    _width( it._width ), _height( it._height ), _xRot( it._xRot ), _yRot( it._yRot ),
    _zRot( it._zRot ), _hSkew( it._hSkew ), _vSkew( it._vSkew ), _zIndex( it._zIndex ),
    _transformOrigin( it._transformOrigin ), _flags( it._flags ),
    _visibleData( it._visibleData ), _disabledData( it._disabledData ), _titleData( it._titleData ),
    _styleSheetData( it._styleSheetData ), _effect( new PHIEffect( *it._effect ) ),
    _dirtyFlags( it._dirtyFlags ), _variants( it._variants )
{
    if ( Q_UNLIKELY( _type==PHIBaseItemPrivate::TIDEItem ) ) {
        PHIGraphicsItemProvider *provider=PHIGraphicsItemProvider::instance();
        Q_ASSERT( provider );
        PHIGraphicsItem *gw=provider->createGraphicsItem();
        gw->setBaseItem( this );
        _gw=gw;
    }
}

PHIBaseItem::~PHIBaseItem()
{
    delete _effect;
    if ( _gw ) {
        QGraphicsScene *s=_gw->scene();
        if ( s ) s->removeItem( _gw ); // get ownership back from scene
        delete _gw;
        _gw=0;
    }
}

void PHIBaseItem::setId( const QString &id )
{
    if ( isChild() ) {
        PHIBaseItem *it=page()->findItem( realParentName() );
        PHIAbstractLayoutItem *lit=qobject_cast<PHIAbstractLayoutItem*>(it);
        if ( lit ) lit->updateChildId( name(), id );
    }
    PHIAbstractLayoutItem *lit=qobject_cast<PHIAbstractLayoutItem*>(this);
    if ( lit ) foreach ( PHIBaseItem *it, lit->childItems() ) it->setParentId( id );
    _id=id.toLatin1();
    setObjectName( id );
}

void PHIBaseItem::setVisible( bool b )
{
    if ( !b ) _variants.insert( DVisibility, false );
    else _variants.remove( DVisibility );
    if ( !isIdeItem() && _gw ) {
        _gw->setVisible( b );
        _gw->setPos( realPos() );
    }
}

void PHIBaseItem::privateUpdateData()
{
    Q_ASSERT( page() );
    if ( _styleSheetData.isUnparsedStatic() ) setStyleSheet( _styleSheetData.text() );
    else if ( _styleSheetData.isUnparsedTranslated() ) setStyleSheet( _styleSheetData.text( page()->currentLang() ) );
    else setStyleSheet( QString() );
    if ( _disabledData.isUnparsedStatic() ) setDisabled( _disabledData.boolean() );
    else if ( _disabledData.isUnparsedTranslated() ) setDisabled( _disabledData.boolean( page()->currentLang() ) );
    else setDisabled( false );
    ideUpdateData();
}

void PHIBaseItem::privateSqueeze()
{
    squeeze();
    _variants.remove( DTransformOrigin );
    _variants.remove( DXRot );
    _variants.remove( DYRot );
    _variants.remove( DZRot );
    _variants.remove( DHSkew );
    _variants.remove( DVSkew );
    _variants.remove( DParentId );
    _variants.remove( DFlags );
    _variants.remove( DTitle );
    _variants.remove( DStyleSheet );
    if ( !hasGradient() || property( "_pattern" ).value<quint8>()<15 ) {
        _variants.remove( DGradientType );
        _variants.remove( DGradientStartPoint );
        _variants.remove( DGradientFinalStopPoint );
        _variants.remove( DGradientSpreadType );
        _variants.remove( DGradientFocalPoint );
        _variants.remove( DGradientStopPoints );
        _variants.remove( DGradientAngle );
        _variants.remove( DGradientCenterPoint );
        _variants.remove( DGradientRadius );
    }
    if ( transformPos()==1 ) _variants.remove( DTransformPos );
    if ( realTabIndex()==0 ) _variants.remove( DTabIndex );
    _variants.squeeze();
}

void PHIBaseItem::load( const QByteArray &arr, int version )
{
    QByteArray a( arr );
    QDataStream in( &a, QIODevice::ReadOnly );
    in.setVersion( PHI_DSV2 );
    QSizeF preserve;
    if ( isClientItem() ) {
        _variants.clear();
        QByteArray eff;
        if ( Q_LIKELY( version>2 ) ) {
            quint32 flags;
            in >> _x >> _y >> _width >> _height >> _zIndex >> flags >> _parentId
                >> _xRot >> _yRot >> _zRot >> _hSkew >> _vSkew >> _transformOrigin
                >> _variants >> eff;
            preserve=QSizeF( _width, _height );
            _flags=static_cast<Flags>(flags);
            _effect->load( eff, version );
        } else {
            return loadVersion1_x( arr );
        }
    } else {
        if ( Q_UNLIKELY( version<3 ) ) return loadVersion1_x( arr );
        Q_ASSERT( page() );
        in >> _x >> _y >> _width >> _height >> _zIndex >> _variants;
        setObjectName( QString::fromUtf8( _id ) );
        preserve=QSizeF( _width, _height );
        _xRot=_variants.value( DXRot, 0 ).toReal();
        _yRot=_variants.value( DYRot, 0 ).toReal();
        _zRot=_variants.value( DZRot, 0 ).toReal();
        _hSkew=_variants.value( DHSkew, 0 ).toReal();
        _vSkew=_variants.value( DVSkew, 0 ).toReal();
        _transformOrigin=_variants.value( DTransformOrigin, QPointF() ).toPointF();
        _parentId=_variants.value( DParentId, QByteArray() ).toByteArray();
        _flags=static_cast<Flags>(_variants.value( DFlags, 0 ).value<quint32>());
        if ( _flags & FUseStyleSheet ) in >> &_styleSheetData;
        if ( _flags & FStoreTitleData ) in >> &_titleData;
        if ( _flags & FStoreVisibleData ) in >> &_visibleData;
        if ( _flags & FStoreDisabledData ) in >> &_disabledData;
        if ( _flags & FStoreEffectData ) {
            QByteArray effData;
            in >> effData;
            _effect->load( effData, version );
        }
        loadItemData( in, version );
    }
    setTransformPos( _variants.value( DTransformPos, 1 ).value<quint8>() );
    setFont( font() ); // setFont may change height depending on sizeHint
    resize( preserve );
    if ( isIdeItem() ) privateUpdateData();
    if ( isClientItem() ) privateClientInit();
    if ( _gw ) {
        _gw->setPos( _x, _y );
        _gw->setTransform( computeTransformation() );
        _gw->setZValue( _zIndex );
        _gw->update();
        updateEffect();
    }
}

QByteArray PHIBaseItem::save( int version )
{
    QByteArray arr;
    QDataStream out( &arr, QIODevice::WriteOnly );
    out.setVersion( PHI_DSV2 );
    Q_ASSERT( version>2 );
    Q_ASSERT( page() );
    if ( isServerItem() ) {
        clientPrepareData();
        out << _x << _y << _width << _height << _zIndex << static_cast<quint32>(_flags) << _parentId
            << _xRot << _yRot << _zRot << _hSkew << _vSkew << _transformOrigin
            << _variants << _effect->save( version );
        return arr;
    }

    privateSqueeze();
    if ( _xRot ) _variants.insert( DXRot, _xRot );
    if ( _yRot ) _variants.insert( DYRot, _yRot );
    if ( _zRot ) _variants.insert( DZRot, _zRot );
    if ( _hSkew ) _variants.insert( DHSkew, _hSkew );
    if ( _vSkew ) _variants.insert( DVSkew, _vSkew );
    if ( _transformOrigin!=QPointF() ) _variants.insert( DTransformOrigin, _transformOrigin );
    if ( !_parentId.isEmpty() ) _variants.insert( DParentId, _parentId );

    if ( _visibleData.isUnparsedStatic() && _visibleData.boolean() ) _flags &= ~FStoreVisibleData;
    else _flags |= FStoreVisibleData;
    if ( _titleData.isUnparsedStatic() && _titleData.text().isEmpty() ) _flags &= ~FStoreTitleData;
    else _flags |= FStoreTitleData;
    if ( _disabledData.isUnparsedStatic() && !_disabledData.boolean() ) _flags &= ~FStoreDisabledData;
    else _flags |= FStoreDisabledData;
    if ( _effect->effects()==PHIEffect::ENone ) _flags &= ~FStoreEffectData;
    else _flags |= FStoreEffectData;
    if ( _flags!=FNone ) storeFlags();
    if ( realPos()!=_gw->pos() ) qDebug() << "PROBLEM <-------------" << id();
    out << _x << _y << _width << _height << _zIndex << _variants;
    if ( _flags & FUseStyleSheet ) out << &_styleSheetData; // flag toggled by IDE
    if ( _flags & FStoreTitleData ) out << &_titleData;
    if ( _flags & FStoreVisibleData ) out << &_visibleData;
    if ( _flags & FStoreDisabledData ) out << &_disabledData;
    if ( _flags & FStoreEffectData ) out << _effect->save( version );
    saveItemData( out, version );
    if ( isIdeItem() ) privateUpdateData(); // restore all dynamic item data cleaned by squeeze()
    updateEffect();
    return arr;
}

void PHIBaseItem::loadVersion1_x( const QByteArray &arr )
{
    enum Attribute { ANone=0x0, AChild=0x1, AWidth=0x2, AHeight=0x4, ATransform=0x8,
        AShear=0x10, AZValue=0x20, ATranslate=0x40, ARotate=0x80, ATransformPos=0x100,
        ADynamicData=0x200, AExtensionData=0x400, AEffectData=0x800, AEditorData=0x1000,
        AParent=0x2000, ADisabled=0x4000, AChecked=0x8000, AReadOnly=0x10000,
        AProperties=0x20000, AOpacity=0x40000, AVisible=0x80000, ATemplateItem=0x100000,
        ADraggable=0x200000, ADroppable=0x400000, AEnd=0x40000000 }; // AEnd is max value
    // @todo: implement client loading for version 1.x: if ( isClientItem() )...
    QByteArray a( arr );
    QDataStream in( &a, QIODevice::ReadOnly );
    in.setVersion( PHI_DSV );
    qint16 zIndex( 0 );
    qint32 attributes, properties, extensions;
    qreal width=sizeHint( Qt::PreferredSize ).width();
    qreal height=sizeHint( Qt::PreferredSize ).height();
    in >> attributes >> _x >> _y;
    if ( attributes & AWidth ) in >> width;
    if ( attributes & AHeight ) in >> height;
    if ( attributes & AZValue ) in >> zIndex;
    resize( width, height );
    setPos( _x, _y );
    setZIndex( zIndex );
    if ( attributes & AProperties ) in >> properties;
    else properties=0;
    QByteArray parent, dynData, extensionData, effectData, editorData;
    qreal xRot, yRot, zRot, sH, sV, xTrans, yTrans, opacity;
    QTransform transform;
    quint8 transformPos;
    if ( attributes & ARotate ) in >> xRot >> yRot >> zRot;
    else xRot=yRot=zRot=0;
    if ( attributes & AShear ) in >> sH >> sV;
    else sH=sV=0;
    if ( attributes & ATranslate ) in >> xTrans >> yTrans;
    else xTrans=yTrans=0;
    if ( attributes & ATransform ) in >> transform; //unused
    if ( attributes & AParent ) in >> parent;
    if ( attributes & AOpacity ) in >> opacity;
    else opacity=1;
    if ( attributes & ADynamicData ) in >> dynData;
    if ( attributes & AExtensionData ) in >> extensions >> extensionData;
    else extensions=0;
    if ( attributes & AEffectData ) {
        in >> effectData;
        _effect->load( effectData, 1 );
    }
    if ( attributes & AEditorData ) in >> editorData;
    // loading preset palette color IDs from editorData:
    if ( !editorData.isEmpty() ) loadEditorData1_x( editorData );
    if ( attributes & ATransformPos) in >> transformPos;
    else transformPos=1;
    setOpacity( opacity );
    setTransformOrigin( QPointF( xTrans, yTrans ) );
    setTransformPos( transformPos );
    setTransformation( sH, sV, xRot, yRot, zRot );
    setParentId( parent );

    PHIItemData d;
    d.load( properties, dynData );
    _visibleData=*d.visibleData();
    _titleData=*d.toolTipData();
    _styleSheetData=*d.styleSheetData();
    _disabledData=*d.disabledData();
    if ( textData() ) *textData()=*d.textData();
    if ( readOnlyData() ) *readOnlyData()=*d.readOnlyData();
    if ( checkedData() ) *checkedData()=*d.checkedData();
    if ( imageData() ) *imageData()=*d.imageData();
    if ( imageBookData() ) *imageBookData()=*d.imageBookData();
    if ( intData_1() ) *intData_1()=*d.startAngleData();
    if ( intData_2() ) *intData_2()=*d.spanAngleData();
    if ( properties & PHIItemData::PStyleSheet ) _flags |= FUseStyleSheet;
    QSizeF s=realSize(); // preserve size
    if ( d.font()!=PHI::invalidFont() ) setFont( d.font() );
    else setFont( page() ? page()->font() : QGuiApplication::font() );
    resize( s );
    setColor( PHIPalette::Foreground, colorRole( PHIPalette::Foreground ), d.color() );
    setColor( PHIPalette::Background, colorRole( PHIPalette::Background ), d.outlineColor() );
    setTabIndex( static_cast<qint16>( d.tabOrder() ) );
    if ( property( "_penWidth" ).isValid() ) setProperty( "_penWidth", d.penWidth() );
    if ( property( "_line" ).isValid() ) setProperty( "_line", d.line() );
    if ( property( "_pattern" ).isValid() ) setProperty( "_pattern", d.pattern() );
    if ( property( "_label" ).isValid() ) setProperty( "_label", d.label() );
    if ( property( "_url" ).isValid() ) setProperty( "_url", d.url() );
    if ( property( "_accessKey" ).isValid() ) setProperty( "_accessKey", d.shortCut() );
    if ( property( "_maxLength" ).isValid() ) setProperty( "_maxLength", d.maxSize() );
    if ( hasGradient() ) {
        QGradient g=d.gradient();
        if ( g.type()==QGradient::ConicalGradient ) setGradient( d.conicalGradient() );
        else if ( g.type()==QGradient::LinearGradient ) setGradient( d.linearGradient() );
        else if ( g.type()==QGradient::RadialGradient ) setGradient( d.radialGradient() );
    }
    if ( qobject_cast<PHIAbstractLayoutItem*>(this) ) {
        if ( property( "_childIds" ).isValid() ) { // linear layout
            setProperty( "_childIds", qVariantFromValue( d.childIds() ) );
        } else if ( property( "_childRects" ).isValid() ) { // grid or form layout
            setProperty( "_childRects", qVariantFromValue( d.gridLayoutInfo() ) );
        }
        setProperty( "_line", 0 );
        setProperty( "_pattern", 0 );
        setProperty( "_leftMargin", 0 );
        setProperty( "_topMargin", 0 );
        setProperty( "_rightMargin", 0 );
        setProperty( "_bottomMargin", 0 );
    }
    if ( wid()==49 ) textData()->setText( d.url() ); // html doc
    updateEffect();
    if ( isIdeItem() ) privateUpdateData();
}

void PHIBaseItem::loadEditorData1_x( const QByteArray &arr )
{
    QByteArray a( arr );
    QDataStream in( &a, QIODevice::ReadOnly );
    in.setVersion( PHI_DSV );
    quint8 col, outCol;
    in >> col >> outCol;
    QList <PHIPalette::ColorRole> map;
    map << PHIPalette::Custom << PHIPalette::White << PHIPalette::User1_100 << PHIPalette::User1_80
        << PHIPalette::User1_60 << PHIPalette::User1_40 << PHIPalette::User1_20 << PHIPalette::User2_100 << PHIPalette::User2_80
        << PHIPalette::User2_60 << PHIPalette::User2_40 << PHIPalette::User2_20 << PHIPalette::User3_100 << PHIPalette::User3_80
        << PHIPalette::User3_60 << PHIPalette::User3_40 << PHIPalette::User3_20 << PHIPalette::User4_100 << PHIPalette::User4_80
        << PHIPalette::User4_60 << PHIPalette::User4_40 << PHIPalette::User4_20 << PHIPalette::User5_100 << PHIPalette::User5_80
        << PHIPalette::User5_60 << PHIPalette::User5_40 << PHIPalette::User5_20 << PHIPalette::User6_100 << PHIPalette::User6_80
        << PHIPalette::User6_60 << PHIPalette::User6_40 << PHIPalette::User6_20 << PHIPalette::Window << PHIPalette::WindowText
        << PHIPalette::Base << PHIPalette::Text << PHIPalette::Button << PHIPalette::ButtonText << PHIPalette::Highlight
        << PHIPalette::HighlightText << PHIPalette::Link << PHIPalette::LinkVisited;
    if ( col>=map.count() ) col=0;
    if ( outCol>=map.count() ) outCol=0;
    setColor( PHIPalette::Foreground, map.at( col ), color( PHIPalette::Foreground ) );
    setColor( PHIPalette::Background, map.at( outCol ), color( PHIPalette::Background ) );
    setColor( PHIPalette::WidgetText, map.at( col ), color( PHIPalette::WidgetText ) );
    setColor( PHIPalette::WidgetBase, map.at( outCol ), color( PHIPalette::WidgetBase ) );
}

void PHIBaseItem::privateClientInit()
{
    setChecked( realChecked() );
    setDisabled( realDisabled() );
    setVisible( realVisible() );
    clientInitData();
    phiPaletteChanged( page()->phiPalette() );
}

void PHIBaseItem::setWidget( QWidget *w )
{
    QGraphicsProxyWidget *proxy=qgraphicsitem_cast<QGraphicsProxyWidget*>(_gw);
    if ( !proxy ) {
        delete w;
        return;
    }
    proxy->setWidget( w );
    //proxy->setCacheMode( QGraphicsItem::ItemCoordinateCache ); // fixed with Qt patch
    proxy->setFlag( QGraphicsItem::ItemUsesExtendedStyleOption, true );
}

void PHIBaseItem::updatePageFont( const QFont &f )
{
    Q_UNUSED( f )
    if ( !page() ) qWarning( "updatePageFont: page not set" );
    if ( !page() || !_gw ) return;
    QFont pf=page()->font();
    if ( pf!=font() ) return; // font() returns the current page font if not set
    pf.setPointSizeF( PHI::adjustedFontSize( pf.pointSizeF() ) );
    _gw->setFont( pf );
    _gw->resize( _width, sizeHint( Qt::PreferredSize ).height() );
    _height=_gw->size().height();
}

void PHIBaseItem::setFont( const QFont &font )
{
    if ( _type==PHIBaseItemPrivate::TTemplateItem ) return;
    QFont pf=QGuiApplication::font();
#ifdef PHIDEBUG
    if ( !page() ) qDebug( "setFont: page not set" );
#endif
    if ( page() ) pf=page()->font();
    if ( pf==font ) {
        _variants.remove( DFont );
    } else {
        _variants.insert( DFont, font );
        pf=font;
    }
    if ( _gw ) {
        pf.setPointSizeF( PHI::adjustedFontSize( font.pointSizeF() ) );
#ifdef Q_OS_MAC
        QComboBox *cb=qobject_cast<QComboBox*>(widget());
        if ( cb ) pf.setPointSizeF( font.pointSizeF()*1.2 );
#endif
        _gw->setFont( pf );
        //_gw->resize( _width, sizeHint( Qt::PreferredSize ).height() );
        //_height=_gw->size().height();
    }
    if ( isIdeItem() ) privateUpdateData();
}

QFont PHIBaseItem::font() const
{
    QFont f=QGuiApplication::font();
    if ( !page() ) qWarning( "font: page not set for %s", id().constData() );
    if ( page() ) f=page()->font();
    return _variants.value( DFont, f ).value<QFont>();
}

void PHIBaseItem::setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &color )
{
    Q_UNUSED( ir )
    Q_UNUSED( cr )
    Q_UNUSED( color )
}

void PHIBaseItem::phiPaletteChanged( const PHIPalette &pal )
{
    if ( widget() ) widget()->setPalette( pal.palette() );
    for ( int i=0; i<PHIPalette::ItemRoleMax; i++ ) {
        PHIPalette::ItemRole itemRole=static_cast<PHIPalette::ItemRole>(i);
        PHIPalette::ColorRole role=colorRole( itemRole );
        if ( role==PHIPalette::NoRole ) continue;
        if ( role==PHIPalette::Custom ) setColor( itemRole, role, color( itemRole ) );
        else setColor( itemRole, role, pal.color( role ) );
    }
}

QRectF PHIBaseItem::boundingRect() const
{
    return rect();
}

void PHIBaseItem::setDisabled( bool b )
{
    b ? _flags|= FDisabled : _flags&= ~FDisabled;
    if ( widget() && !isIdeItem() ) widget()->setDisabled( b );
}

void PHIBaseItem::setStyleSheet( const QString &s )
{
    _variants.insert( DStyleSheet, s.toUtf8() );
    if ( widget() ) widget()->setStyleSheet( s );
}

void PHIBaseItem::setTransformPos( quint8 position )
{
    int p=qBound( 0, static_cast<int>(position), 9 );
    _variants.insert( DTransformPos, static_cast<quint8>(p) );
    QPointF o;
    switch( static_cast<PHI::Origin>(p) ) {
    case PHI::TopLeft: o=QPointF( 0, 0 ); break;
    case PHI::TopMid: o=QPointF( _width/2., 0 ); break;
    case PHI::TopRight: o=QPointF( _width, 0 ); break;
    case PHI::MidLeft: o=QPointF( 0, _height/2. ); break;
    case PHI::MidMid: o=QPointF( _width/2., _height/2. ); break;
    case PHI::MidRight: o=QPointF( _width, _height/2. ); break;
    case PHI::BottomLeft: o=QPointF( 0, _height ); break;
    case PHI::BottomMid: o=QPointF( _width/2., _height ); break;
    case PHI::BottomRight: o=QPointF( _width, _height ); break;
    default: return; // o==PHI::Custom (user defined origin)
    }
    QTransform t=computeTransformation();
    QPointF org=t.map( o );
    _transformOrigin=o;
    t=computeTransformation();
    setPos( realPos()-(o-org) );
    if ( _gw ) _gw->setTransform( t );
    if ( _gw ) _gw->setTransform( t );
}

void PHIBaseItem::setTransformOrigin( const QPointF &position )
{   // user defined origin
    QPointF org=computeTransformation().map( QPointF() ); // old top left corner
    _transformOrigin=position;
    QTransform t=computeTransformation();
    QPointF delta=t.map( QPointF() )-org; // new top left - old top left
    setPos( realPos()-delta );
    if ( _gw ) _gw->setTransform( t );
    setTransformPos( 0 );
}

void PHIBaseItem::setTransformation( qreal hs, qreal vs, qreal xRot, qreal yRot, qreal zRot )
{
    _hSkew=hs; _vSkew=vs; _xRot=xRot; _yRot=yRot; _zRot=zRot;
    if ( _gw ) _gw->setTransform( computeTransformation() );
}

void PHIBaseItem::ideSetText( const QString &t, const QByteArray &lang )
{
    // used for IDE only
    Q_UNUSED( t )
    Q_UNUSED( lang )
}

QString PHIBaseItem::ideText( const QByteArray &lang ) const
{
    // used for IDE only
    Q_UNUSED( lang )
    return QString();
}

/*
QPainterPath PHIBaseItem::shape() const
{
    QPainterPath path;
    path.addRect( boundingRect() );
    return path;
}
*/

void PHIBaseItem::setAccessKey( const QString &s )
{
    setData( DAccessKey, s.left(1).toUtf8() );
    if ( !widget() ) return;
    QShortcut *scut=widget()->findChild<QShortcut*>( L1( "shortcut" ) );
    if ( scut ) delete scut;
    if ( realAccessKey().isEmpty() ) return;
    scut=new QShortcut( QKeySequence( L1( "Alt+" )+realAccessKey() ), widget() );
    scut->setObjectName( L1( "shortcut" ) );
    connect( scut, &QShortcut::activated, this, &PHIBaseItem::accessKeyTriggered );
}

QSizeF PHIBaseItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    Q_UNUSED( constraint )
    switch ( which ) {
    case Qt::MinimumSize: return QSizeF( 16, 16 );
    case Qt::PreferredSize: return QSizeF( 300, 200 );
    case Qt::MaximumSize: return QSizeF( 4000, 4000 );
    default:;
    }
    return QSizeF(); // invalid size: call base implementation of QGraphicsProxyWidget
}

void PHIBaseItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *options, QWidget *widget )
{
    Q_UNUSED( widget );
    painter->save();
    paint( painter, options->exposedRect );
    painter->restore();
}

void PHIBaseItem::paint( QPainter *painter, const QRectF &exposed )
{
    QGraphicsProxyWidget *proxy=qgraphicsitem_cast<QGraphicsProxyWidget*>(_gw);
    if ( !proxy ) return;
    if ( !proxy->widget() ) return;
    const QRect exposedRect=( exposed & boundingRect() ).toAlignedRect();
    if ( exposedRect.isEmpty() ) return;
    painter->setRenderHint( QPainter::TextAntialiasing );
    painter->setRenderHint( QPainter::SmoothPixmapTransform );
    if ( hasTransformation() ) painter->setRenderHint( QPainter::Antialiasing );
    proxy->widget()->render( painter, exposedRect.topLeft(), exposedRect );
}

void PHIBaseItem::paintHighlight( QPainter *painter )
{
    Q_UNUSED( painter )
}

void PHIBaseItem::loadItemData( QDataStream &in, int version )
{
    Q_UNUSED( in )
    Q_UNUSED( version )
}

void PHIBaseItem::saveItemData( QDataStream &out, int version )
{
    Q_UNUSED( out )
    Q_UNUSED( version )
}

bool PHIBaseItem::sceneEvent( QEvent *e )
{
    if ( _type==PHIBaseItemPrivate::TIDEItem ) {
        switch ( e->type() ) {
        case QEvent::GraphicsSceneResize:
            ideResizeEvent( static_cast<QGraphicsSceneResizeEvent*>(e) );
            return true;
        case QEvent::GraphicsSceneDragEnter:
            ideDragEnterEvent( static_cast<QGraphicsSceneDragDropEvent*>(e) );
            return true;
        case QEvent::GraphicsSceneDragMove:
            ideDragMoveEvent( static_cast<QGraphicsSceneDragDropEvent*>(e) );
            return true;
        case QEvent::GraphicsSceneDragLeave:
            ideDragLeaveEvent( static_cast<QGraphicsSceneDragDropEvent*>(e) );
            return true;
        case QEvent::GraphicsSceneDrop:
            ideDropEvent( static_cast<QGraphicsSceneDragDropEvent*>(e) );
            return true;
        case QEvent::GraphicsSceneMousePress:
            return false; // forward to PHIGraphicsItem
        case QEvent::GraphicsSceneMouseMove:
            return false;
        case QEvent::GraphicsSceneMouseRelease:
            return false;
        case QEvent::KeyPress:
            ideKeyPressEvent( static_cast<QKeyEvent*>(e) );
            qDebug() << "kyp";
            return true;
        default:;
        }
    }
    return false;
}

void PHIBaseItem::ideResizeEvent( QGraphicsSceneResizeEvent *e )
{
    qDebug( "ideResizeEvent %f %f", e->newSize().width(), e->newSize().height() );
    _width=e->newSize().width();
    _height=e->newSize().height();
}

void PHIBaseItem::ideDragEnterEvent( QGraphicsSceneDragDropEvent *e )
{
    e->ignore();
}

void PHIBaseItem::ideDragMoveEvent( QGraphicsSceneDragDropEvent *e )
{
    Q_UNUSED( e )
}

void PHIBaseItem::ideDragLeaveEvent( QGraphicsSceneDragDropEvent *e )
{
    if ( e->isAccepted() ) qDebug( "baseitem::ideDragLeave isaccepted" );
    else qDebug( "baseitem::ideDragLeave is not accepted" );
}

void PHIBaseItem::ideDropEvent( QGraphicsSceneDragDropEvent *e )
{
    if ( e->isAccepted() ) qDebug( "baseitem::ideDrop isaccepted" );
    else qDebug( "baseitem::ideDrop is not accepted" );
}

void PHIBaseItem::ideKeyPressEvent( QKeyEvent *e )
{
    qDebug( "base item key press" );
    e->ignore();
}

void PHIBaseItem::slideAnimationFinished()
{
    restoreFromAnimation();
}

void PHIBaseItem::prepareForAnimation()
{
    if ( !_gw ) return;
    if ( _flags & FIsAnimating ) return;
    _flags |= FIsAnimating;
    setData( DAnimOrgGeometry, QRectF( realPos(), realSize() ) );
    setData( DAnimSizePolicy, _gw->sizePolicy() );
    setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    PHIAbstractLayoutItem *l=qobject_cast<PHIAbstractLayoutItem*>(this);
    if ( l ) {
        foreach ( PHIBaseItem *it, l->childItems() ) it->prepareForAnimation();
    }
    update();
}

void PHIBaseItem::restoreFromAnimation()
{
    _flags &= ~FIsAnimating;
    setSizePolicy( data( DAnimSizePolicy ).value<QSizePolicy>() );
    PHIAbstractLayoutItem *l=qobject_cast<PHIAbstractLayoutItem*>(this);
    if ( l ) {
        foreach ( PHIBaseItem *it, l->childItems() ) it->restoreFromAnimation();
    }
    update();
}

void PHIBaseItem::phisPrivateCreateData( const PHIDataParser &parser )
{
    parser.setCurrentItem( this );
    privateSqueeze();
    _dirtyFlags=DFClean;
    if ( _flags & FDoNotCache ) _dirtyFlags |= DFDoNotCache; // support version 1.x
    _variants.insert( DTitle, parser.text( &_titleData ) );
    if ( !_titleData.isUnparsedStatic() ) _dirtyFlags |= DFTitleData;
    if ( _flags & FUseStyleSheet ) {
        _variants.insert( DStyleSheet, parser.text( &_styleSheetData ) );
        if ( !_styleSheetData.isUnparsedStatic() ) _dirtyFlags |= DFStyleSheetData;
    }
    _variants.insert( DVisibility, parser.text( &_visibleData ) );
    if ( !_visibleData.isUnparsedStatic() ) _dirtyFlags |= DFVisibleData;
    setDisabled( parser.text( &_disabledData ).toBool() );
    if ( !_disabledData.isUnparsedStatic() ) _dirtyFlags |= DFDisabledData;
    phisCreateData( parser );
}

void PHIBaseItem::phisPrivateParseData( const PHIDataParser &parser )
{
    parser.setCurrentItem( this );
    if ( Q_UNLIKELY( _dirtyFlags & DFTitleData ) ) _variants.insert( DTitle, parser.text( &_titleData ) );
    if ( Q_UNLIKELY( _dirtyFlags & DFStyleSheetData ) ) _variants.insert( DStyleSheet, parser.text( &_styleSheetData ) );
    if ( Q_UNLIKELY( _dirtyFlags & DFVisibleData ) ) _variants.insert( DVisibility, parser.text( &_visibleData ) );
    if ( Q_UNLIKELY( _dirtyFlags & DFDisabledData ) ) setDisabled( parser.text( &_disabledData ).toBool() );
    phisParseData( parser );
}

void PHIBaseItem::phisCreateData( const PHIDataParser &parser )
{
    Q_UNUSED( parser )
}

void PHIBaseItem::phisParseData( const PHIDataParser &parser )
{
    Q_UNUSED( parser )
}

void PHIBaseItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray& indent ) const
{
    out+=indent+"<div";
    htmlBase( req, out, script );
    out+="background-color:gray\">Unknown WID "+QByteArray::number( wid() )+"</div>\n";
}

void PHIBaseItem::htmlImg( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    QTransform t;
    bool needCalc=false;
    if ( hasGraphicEffect() ) needCalc=true;
    else if ( hasTransformation() ) {
        t=computeTransformation( false );
        if ( !t.isAffine() ) {
            if ( Q_UNLIKELY( !(req->agentFeatures() & PHIRequest::Transform3D) ) ) {
                qDebug() << "t is not affine and no 3D support" << id();
                needCalc=true;
            }
        }
    }
    if ( Q_UNLIKELY( req->agentFeatures() & PHIRequest::IE678 ) ) {
        if ( needCalc ) {
            QRectF br=boundingRect();
            cssImageIEFilter( PHIDataParser::createTransformedImage( req, this, 0, br ) );
            setAdjustedRect( br );
        } else cssImageIEFilter( imagePath() );
        out+=indent+BL( "<div" );
        htmlBase( req, out, script, false );
        out+=BL( "\"></div>\n" );
    } else {
        QByteArray imgId;
        if ( needCalc ) {
            QRectF br=boundingRect();
            imgId=PHIDataParser::createTransformedImage( req, this, 0, br );
            setAdjustedRect( br );
        } else imgId=imagePath();
        out+=indent+BL( "<img" );
        htmlBase( req, out, script, false );
        out+=BL( "\" src=\"" );
        if ( _dirtyFlags & DFUseFilePathInHTML ) out+=imgId+BL( "\">\n" );
        else out+=BL( "phi.phis?i=" )+imgId+BL( "&t=1\">\n" );
    }
    htmlInitItem( script );
}

void PHIBaseItem::htmlImages( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    QTransform t;
    bool needCalc=false;
    if ( hasGraphicEffect() ) needCalc=true;
    else if ( hasTransformation() ) {
        t=computeTransformation( false );
        if ( !t.isAffine() ) {
            if ( Q_UNLIKELY( !(req->agentFeatures() & PHIRequest::Transform3D) ) ) {
                qDebug() << "t is not affine and no 3D support" << id();
                needCalc=true;
            }
        }
    }
    out+=indent+BL( "<div" );
    htmlBase( req, out, script, false );
    out+=BL( "\">\n" );
    if ( Q_UNLIKELY( req->agentFeatures() & PHIRequest::IE678 ) ) {
        QByteArray filter;
        for ( int i=0; i<imagePathes().count(); i++ ) {
            if ( needCalc ) {
                QRectF br=boundingRect();
                filter=cssImageIEFilter( PHIDataParser::createTransformedImage( req, this, i, br ), false );
                if ( i==0 ) setAdjustedRect( br );
            } else {
                filter=cssImageIEFilter( imagePathes().at( i ), false );
                out+=indent+BL( "\t<div class=\"phi\" id=\"" )+_id+BL( "_phi_" )
                    +QByteArray::number( i )+BL( "\" style=\"width:100%;height:100%" )
                    +filter+BL( "\"></div>\n" );
            }
        }
    } else {
        QByteArray imgId;
        for ( int i=0; i<imagePathes().count(); i++ ) {
            if ( needCalc ) {
                QRectF br=boundingRect();
                imgId=PHIDataParser::createTransformedImage( req, this, i, br );
                if ( i==0 ) setAdjustedRect( br );
                qDebug() << "resize" << boundingRect() << br;
            } else imgId=imagePathes().at( i );
            out+=indent+BL( "\t<img class=\"phi\" id=\"" )+_id+BL( "_phi_" )
                +QByteArray::number( i )+BL( "\" style=\"width:100%;height:100%\" src=\"" );
            if ( _dirtyFlags & DFUseFilePathInHTML ) out+=imgId+BL( "\">\n" );
            else out+=BL( "phi.phis?i=" )+imgId+BL( "&t=1\">\n" );
        }
    }
    out+=indent+BL( "</div>\n" );
    htmlInitItem( script );
}

void PHIBaseItem::privateStaticCSS( const PHIRequest *req, QByteArray &out ) const
{
    out+='#'+_id+BL( "{left:" )
        +QByteArray::number( qRound(_x) )+BL( "px;top:" )
        +QByteArray::number( qRound(_y) )+BL( "px;width:" )
        +QByteArray::number( qRound(_width) )+BL( "px;height:" )
        +QByteArray::number( qRound(_height) )+BL( "px;z-index:" )
        +QByteArray::number( _zIndex )+';';
    QFont f=_variants.value( DFont, page()->font() ).value<QFont>();
    if ( f!=page()->font() ) {
        out+=BL( "font-family:'" )+f.family().toUtf8();
        if ( !f.lastResortFamily().isEmpty() ) {
            out+=BL( "','" )+f.lastResortFamily().toUtf8();
        }
        out+=BL( "';font-size:" )+QByteArray::number( f.pointSize() )+BL( "pt;" );
        if ( f.bold() ) out+=BL( "font-weight:bold;" );
        if ( f.italic() ) out+=BL( "font-style:italic;" );
        if ( f.underline() ) out+=BL( "text-decoration:underline;" );
    }
    out+=BL( "}\n" );
    cssStatic( req, out );
    if ( _flags & FUseStyleSheet && !(_dirtyFlags & DFStyleSheetData) ) {
        cssCustomStyleSheet( out );
    }
}

void PHIBaseItem::cssCustomStyleSheet( QByteArray &out ) const
{
    QByteArray el=_variants.value( DStyleSheet ).toByteArray().simplified();
    PHIByteArrayList list=el.split( '}' );
    if ( list.count()==1 ) {
        if ( el.contains( '{' ) ) out+='#'+_id+el+'\n';
        else out+='#'+_id+BL( "{" )+el+BL( "}\n" );
    } else {
        foreach( el, list ) {
            out+='#'+_id+el+BL( "}\n" );
        }
    }
}

void PHIBaseItem::cssStatic( const PHIRequest *req, QByteArray &out ) const
{
    Q_UNUSED( req )
    Q_UNUSED( out )
}

void PHIBaseItem::htmlBase( const PHIRequest *req, QByteArray &out, QByteArray &script, bool project3D ) const
{
    if ( realOpacity()<1. ) script+=BL( "$('" )+_id+BL( "').opacity(" )
        +QByteArray::number( realOpacity(), 'f', 3 )+BL( ");\n" );
    if ( !realVisible() ) script+=BL( "$('" )+_id+BL( "').hide();\n" );
    if ( !realAccessKey().isEmpty() ) script+=BL( "$('" )+_id+BL( "').accessKey('" )
        +realAccessKey().toUtf8()+BL( "');\n" );
    out+=BL( " class=\"phi\" id=\"" )+_id;
    if ( _variants.value( DTitle ).isValid() ) out+=BL( "\" title=\"" )+_variants.value( DTitle ).toByteArray();
    out+=BL( "\" style=\"" );
    if ( hasGraphicEffect() ) cssGraphicEffect( req, out, script );
    if ( hasTransformation() ) {
        QTransform t=computeTransformation( false );
        if ( Q_LIKELY( req->agentFeatures() & PHIRequest::Transform3D ) ) {
            QByteArray prefix=req->agentPrefix();
            if ( req->agentEngine()==PHIRequest::Trident ) prefix=QByteArray();
            out+=prefix+BL( "transform:matrix3d(" )
                +QByteArray::number( t.m11(), 'f', 3 )+','
                +QByteArray::number( t.m12(), 'f', 3 )+BL( ",0," )
                +QByteArray::number( t.m13(), 'f', 6 )+',' //6
                +QByteArray::number( t.m21(), 'f', 3 )+','
                +QByteArray::number( t.m22(), 'f', 3 )+BL( ",0," )
                +QByteArray::number( t.m23(), 'f', 6 )+BL( ",0,0,1,0," ) //6
                +QByteArray::number( t.m31(), 'f', 3 )+','
                +QByteArray::number( t.m32(), 'f', 3 )+BL( ",0," )
                +QByteArray::number( t.m33(), 'f', 3 )+BL( ");" );
        } else if ( project3D || t.isAffine() ) {
            if ( Q_UNLIKELY( req->agentFeatures() & PHIRequest::IE678 ) ) {
                QByteArray filter;
                // Qt uses row major ordering, MSIE uses col major ordering so we need to swap m21 with m12
                filter+="progid:DXImageTransform.Microsoft.Matrix(M11="+QByteArray::number( t.m11(), 'f', 3 )
                     +",M12="+QByteArray::number( t.m21(), 'f', 3 )+",M21="+QByteArray::number( t.m12(), 'f', 3 )
                     +",M22="+QByteArray::number( t.m22(), 'f', 3 )+BL( ",sizingMethod='auto expand')" );
                if ( _variants.value( DIEFilter ).isValid() ) filter+=_variants.value( DIEFilter ).toByteArray();
                _variants.insert( DIEFilter, filter );
            } else { // 2D transformation
                out+=req->agentPrefix()+BL( "transform:matrix(" )+QByteArray::number( t.m11(), 'f', 3 )
                    +','+QByteArray::number( t.m12(), 'f', 3 )+','+QByteArray::number( t.m21(), 'f', 3 )
                    +','+QByteArray::number( t.m22(), 'f', 3 )+','+QByteArray::number( qRound(t.m31()) )
                    +','+QByteArray::number( qRound(t.m32()) )+BL( ");" );
            }
        }
    }
    if ( Q_UNLIKELY( req->agentFeatures() & PHIRequest::IE678 ) ) {
        if ( _variants.value( DIEFilter ).isValid() ) {
            out+=BL( "filter:" )+_variants.value( DIEFilter ).toByteArray()+';';
        }
    } else if ( Q_UNLIKELY( transformPos()!=1 ) ) {
        QPointF o=transformOrigin();
        out+=req->agentPrefix()+BL( "transform-origin:" )
            +QByteArray::number( qRound(o.x()) )+"px "
            +QByteArray::number( qRound(o.y()) )+"px;";
    }
    if ( Q_UNLIKELY( _dirtyFlags & DFStyleSheetData ) ) cssCustomStyleSheet( out );
    quint32 ddopts=dragDropOptions();
    if ( Q_UNLIKELY( ddopts & DDDragEnabled || ddopts & DDDropEnabled ) ) htmlDragDropItem( script );
}

void PHIBaseItem::htmlDragDropItem( QByteArray &script ) const
{
    quint32 opts=dragDropOptions();
    if ( opts & DDDragEnabled ) {
        script+=BL( "jQuery('#" )+_id+BL( "').draggable({zIndex:30000,cursor:'move'," );
        if ( !(opts & DDMoveAction) ) script+=BL( "helper:'clone'," );
        if ( opts & DDRevertOnAccept && opts & DDRevertOnIgnore ) script+=BL( "revert:true," );
        else if ( opts & DDRevertOnAccept ) script+=BL( "revert:'valid'," );
        else if ( opts & DDRevertOnIgnore ) script+=BL( "revert:'invalid'," );
        qreal opac=data( DDragOpacity, 1. ).toReal();
        if ( opac<1. ) script+=BL( "opacity:" )+QByteArray::number( opac, 'f', 2 )+',';
        QPoint hotspot=data( DDragHotSpot, PHI::defaultHotSpot() ).toPoint();
        if ( data( DDragHotSpotType ).toInt()==1 ) script+=BL( "cursorAt:{left:" )
            +QByteArray::number( qRound(realWidth()/2) )+BL( ",top:" )
            +QByteArray::number( qRound(realHeight()/2) )+BL( "}," );
        else script+=BL( "cursorAt:{left:" )+QByteArray::number( hotspot.x() )
            +BL( ",top:" )+QByteArray::number( hotspot.y() )+BL( "}," );
        script+=BL( "distance:" )+QByteArray::number(
            data( DDragDistance, PHI::defaultDragDistance() ).toInt() )+BL( "});\n" );
    }
    if ( opts & DDDropEnabled ) {
        script+=BL( "jQuery('#" )+_id+BL( "').droppable({" );
        QByteArray arr, acc;
        PHIByteArrayList list=data( DDropAcceptedIds ).value<PHIByteArrayList>();
        if ( !list.isEmpty() ) {
            foreach ( acc, list ) arr+='#'+acc+',';
            arr.chop( 1 ); // remove last ','
        }
        if ( !arr.isEmpty() ) script+=BL( "accept:'" )+arr+BL( "'," );
        if ( opts & DDHighlightOnMouseOver ) script+=BL( "hoverClass:'ui-state-hover'," );
        script+=BL( "tolerance:'pointer'});\n" );
    }
}

void PHIBaseItem::cssGraphicEffect( const PHIRequest *req, QByteArray &out, QByteArray &script ) const
{
    if ( _effect->graphicsType()==PHIEffect::GTShadow ) {
        if ( Q_UNLIKELY( req->agentFeatures() & PHIRequest::IE678 ) ) {
            QByteArray filter;
            QColor c;
            qreal xOff, yOff, radius;
            _effect->shadow( c, xOff, yOff, radius );
            filter+="progid:DXImageTransform.Microsoft.DropShadow(Color='#";
            filter+=QByteArray::number( c.rgba(), 16 )+"',OffX="+QByteArray::number( xOff );
            filter+=",OffY="+QByteArray::number( yOff )+')';
            if ( xOff<0 ) script+=BL( "$('" )+_id+BL( "').x(" )
                +QByteArray::number( qRound(_x+xOff) )+BL( ");\n" );
            if ( yOff<0 ) script+=BL( "$('" )+_id+BL( "').y(" )
                +QByteArray::number( qRound(_y+yOff) )+BL( ");\n" );
            if ( _variants.value( DIEFilter ).isValid() ) filter+=_variants.value( DIEFilter ).toByteArray();
            _variants.insert( DIEFilter, filter );
        } else {
            QByteArray prefix=req->agentPrefix();
            if ( req->agentEngine()==PHIRequest::Gecko && req->engineMajorVersion()>1 ) prefix=QByteArray();
            else if ( req->agentEngine()==PHIRequest::WebKit && req->engineMajorVersion()>534 ) prefix=QByteArray();
            else if ( req->agentEngine()==PHIRequest::Presto ) prefix=QByteArray();
            QColor c;
            qreal xOff, yOff, radius;
            _effect->shadow( c, xOff, yOff, radius );
            out+=prefix+BL( "box-shadow:" )+QByteArray::number( qRound(xOff) )+"px ";
            out+=QByteArray::number( qRound(yOff) )+"px ";
            out+=QByteArray::number( qRound(radius) )+"px "+cssRgba( c )+';';
        }
    } else if ( _effect->graphicsType()==PHIEffect::GTBlur ) {
        if ( Q_UNLIKELY( req->agentFeatures() & PHIRequest::IE678 ) ) {
            QByteArray filter;
            qreal radius;
            _effect->blur( radius );
            filter+="progid:DXImageTransform.Microsoft.Blur(PixelRadius=";
            filter+=QByteArray::number( radius )+')';
            script+=BL( "$('" )+_id+BL( "').x(" )
                +QByteArray::number( qRound(_x-radius) )+BL( ").y(" )
                +QByteArray::number( qRound(_y-radius) )+BL( ");\n" );
        }
    }
}

void PHIBaseItem::setGradient( QLinearGradient g )
{
    _variants.insert( DGradientStartPoint, g.start() );
    _variants.insert( DGradientFinalStopPoint, g.finalStop() );
    _variants.insert( DGradientSpreadType, static_cast<quint8>(g.spread()) );
    _variants.insert( DGradientStopPoints, qVariantFromValue(g.stops()) );
    _variants.insert( DGradientType, static_cast<quint8>(g.type()) );
    if ( isIdeItem() ) privateUpdateData();
    update();
}

void PHIBaseItem::setGradient( QConicalGradient g )
{
    _variants.insert( DGradientAngle, g.angle() );
    _variants.insert( DGradientCenterPoint, g.center() );
    _variants.insert( DGradientSpreadType, static_cast<quint8>(g.spread()) );
    _variants.insert( DGradientStopPoints, qVariantFromValue(g.stops()) );
    _variants.insert( DGradientType, static_cast<quint8>(g.type()) );
    if ( isIdeItem() ) privateUpdateData();
    update();
}

void PHIBaseItem::setGradient( QRadialGradient g )
{
    _variants.insert( DGradientFocalPoint, g.focalPoint() );
    _variants.insert( DGradientRadius, g.radius() );
    _variants.insert( DGradientCenterPoint, g.center() );
    _variants.insert( DGradientSpreadType, static_cast<quint8>(g.spread()) );
    _variants.insert( DGradientStopPoints, qVariantFromValue(g.stops()) );
    _variants.insert( DGradientType, static_cast<quint8>(g.type()) );
    if ( isIdeItem() ) privateUpdateData();
    update();
}

QGradient PHIBaseItem::gradient() const
{
    QGradient g;
    QGradient::Type type=static_cast<QGradient::Type>(_variants.value( DGradientType, 0 ).value<quint8>());
    if ( type==QGradient::ConicalGradient ) {
        g=QConicalGradient( _variants.value( DGradientCenterPoint, QPointF( 0.5, 0.5 ) ).toPointF(),
            _variants.value( DGradientAngle, 0. ).value<qreal>() );
    } else if ( type==QGradient::RadialGradient ) {
        g=QRadialGradient( _variants.value( DGradientCenterPoint, QPointF( 0.5, 0.5 ) ).toPointF(),
            _variants.value( DGradientRadius, 0.5 ).value<qreal>(),
            _variants.value( DGradientFocalPoint, QPointF( 0.1, 0.1 ) ).toPointF() );
    } else {
        g=QLinearGradient( _variants.value( DGradientStartPoint, QPointF( 0., 0. ) ).toPointF(),
            _variants.value( DGradientFinalStopPoint, QPointF( 1.0, 1.0 ) ).toPointF() );
    }
    QGradientStops stops=_variants.value( DGradientStopPoints ).value<QGradientStops>();
    if ( stops.count() ) g.setStops( stops );
    g.setSpread( static_cast<QGradient::Spread>(_variants.value( DGradientSpreadType, 0 ).value<quint8>()) );
    g.setCoordinateMode( QGradient::ObjectBoundingMode );
    return g;
}

QLinearGradient PHIBaseItem::linearGradient() const
{
    QLinearGradient g=QLinearGradient(
        _variants.value( DGradientStartPoint, QPointF( 0., 0. ) ).toPointF(),
        _variants.value( DGradientFinalStopPoint, QPointF( 1.0, 1.0 ) ).toPointF() );
    QGradientStops stops=_variants.value( DGradientStopPoints ).value<QGradientStops>();
    if ( stops.count() ) g.setStops( stops );
    g.setSpread( static_cast<QGradient::Spread>(_variants.value( DGradientSpreadType, 0 )
        .value<quint8>()) );
    g.setCoordinateMode( QGradient::ObjectBoundingMode );
    return g;
}

QConicalGradient PHIBaseItem::conicalGradient() const
{
    QConicalGradient g=QConicalGradient(
        _variants.value( DGradientCenterPoint, QPointF( 0.5, 0.5 ) ).toPointF(),
        _variants.value( DGradientAngle, 0. ).value<qreal>() );
    QGradientStops stops=_variants.value( DGradientStopPoints ).value<QGradientStops>();
    if ( stops.count() ) g.setStops( stops );
    g.setSpread( static_cast<QGradient::Spread>(_variants.value( DGradientSpreadType, 0 )
        .value<quint8>()) );
    g.setCoordinateMode( QGradient::ObjectBoundingMode );
    return g;
}

QRadialGradient PHIBaseItem::radialGradient() const
{
    QRadialGradient g=QRadialGradient(
        _variants.value( DGradientCenterPoint, QPointF( 0.5, 0.5 ) ).toPointF(),
        _variants.value( DGradientRadius, 0.5 ).value<qreal>(),
        _variants.value( DGradientFocalPoint, QPointF( 0.1, 0.1 ) ).toPointF() );
    QGradientStops stops=_variants.value( DGradientStopPoints ).value<QGradientStops>();
    if ( stops.count() ) g.setStops( stops );
    g.setSpread( static_cast<QGradient::Spread>(_variants.value( DGradientSpreadType, 0 )
        .value<quint8>()) );
    g.setCoordinateMode( QGradient::ObjectBoundingMode );
    return g;
}

bool PHIBaseItem::cssGradientCreateable( const PHIRequest *req ) const
{
    if( !(req->agentFeatures() & PHIRequest::Gradients) ) return false;
    if ( data( DGradientType, 0 ).toInt()!=0 ) return false;
    QLinearGradient g=linearGradient();
    if ( g.spread()!=QGradient::PadSpread ) return false;
    QRectF r( 0, 0, 1., 1. );
    if ( !r.contains( g.start() ) ) return false;
    if ( !r.contains( g.finalStop() ) ) return false;
    return true;
}

void PHIBaseItem::cssLinearGradient( const PHIRequest *req, QByteArray &out ) const
{
    // @todo: enable gradients
    static const qreal PI=3.14159265358979323846;
    QLinearGradient g=linearGradient();
    QPointF v=g.finalStop()-g.start();
    qDebug() << "linear gradient" << g.start() << g.finalStop() << -atan(v.y()/v.x())*180./PI;
    out+=BL( "background:" )+req->agentPrefix()+BL( "linear-gradient(" )
        +QByteArray::number( -atan(v.y()/v.x())*180./PI, 'f', 2 )+BL( "deg," );
    QGradientStop stop;
    foreach ( stop, g.stops() ) {
        out+=cssRgba( stop.second )+' '
            +QByteArray::number( stop.first*100., 'f', 2 )+BL( "%," );
        qDebug() << "stop" << stop.first << stop.second;
    }
    out.chop( 1 ); // remove last ','
    out+=BL( ");" );
}

void PHIBaseItem::updateEffect()
{
    if ( !_gw ) return;
    if ( _effect->effects() & PHIEffect::EGraphics ) {
        qreal eStrength, eRadius, eXOff, eYOff;
        QColor eColor;
        switch ( _effect->graphicsType() ) {
        case PHIEffect::GTBlur: {
            _effect->blur( eRadius );
            QGraphicsBlurEffect *blur=new QGraphicsBlurEffect();
            blur->setBlurHints( QGraphicsBlurEffect::AnimationHint );
            blur->setBlurRadius( eRadius );
            _gw->setGraphicsEffect( blur );
            break;
        }
        case PHIEffect::GTColorize: {
            _effect->colorize( eColor, eStrength );
            QGraphicsColorizeEffect *colorize=new QGraphicsColorizeEffect();
            colorize->setStrength( eStrength );
            colorize->setColor( eColor );
            _gw->setGraphicsEffect( colorize );
            break;
        }
        case PHIEffect::GTShadow: {
            _effect->shadow( eColor, eXOff, eYOff, eRadius );
            QGraphicsDropShadowEffect *shadow=new QGraphicsDropShadowEffect();
            shadow->setBlurRadius( eRadius*2. );
            shadow->setColor( eColor );
            shadow->setXOffset( eXOff );
            shadow->setYOffset( eYOff );
            _gw->setGraphicsEffect( shadow );
            break;
        }
        case PHIEffect::GTReflection: {
            _effect->reflection( eYOff, eRadius );
            PHIReflectionEffect *reflection=new PHIReflectionEffect();
            reflection->setYOffset( eYOff );
            reflection->setSize( eRadius );
            reflection->setBaseItem( this );
            _gw->setGraphicsEffect( reflection );
            break;
        }
        default:;
        }
    } else _gw->setGraphicsEffect( 0 );
}

QImage PHIBaseItem::createImage()
{
    qDebug() << "createImage" << boundingRect() << rect();
    QImage img( qRound(boundingRect().width()), qRound(boundingRect().height()), QImage::Format_ARGB32_Premultiplied );
    img.fill( 0 );
    QPainter p( &img );
    QPointF off=boundingRect().topLeft();
    p.translate( -off.x(), -off.y() );
    paint( &p, QRectF() );
    p.end();
    return img;
}

PHIWID PHIBaseItem::widFromMimeData( const QMimeData *md )
{
    Q_ASSERT( md );
    if ( md->hasFormat( QLatin1String( "application/x-phi-wid" ) ) ) {
        QByteArray arr=md->data( QLatin1String( "application/x-phi-wid" ) );
        QDataStream ds( &arr, QIODevice::ReadOnly );
        int id;
        ds >> id;
        if ( id ) return static_cast<PHIWID>( id );
    }
    return 0;
}

QImage PHIBaseItem::imageFromMimeData( const QMimeData *md )
{
    Q_ASSERT( md );
    if ( md->hasImage() ) return md->imageData().value<QImage>();
    QImage img( pathFromMimeData( md ) );
    if ( !img.isNull() ) return img;
    QUrl url=urlFromMimeData( md );
    img=QImage( url.toLocalFile() );
    return img;
}

QString PHIBaseItem::pathFromMimeData( const QMimeData *md )
{
    Q_ASSERT( md );
    if ( md->hasText() ) {
        QString file=md->text();
        if ( file.startsWith( L1( "file://" ) ) ) {
            file=file.mid( 7 );
            qDebug( "file=%s", qPrintable( file ) );
            return file;
        }
    }
    return QString();
}

QUrl PHIBaseItem::urlFromMimeData( const QMimeData *md )
{
    Q_ASSERT( md );
    if ( md->hasUrls() ) {
        QList<QUrl> list=md->urls();
        QUrl url=list.first();
        qDebug( "url=%s", qPrintable( url.toLocalFile() ) );
        return url;
    }
    return QUrl();
}

QColor PHIBaseItem::colorFromMimeData( const QMimeData *md )
{
    Q_ASSERT( md );
    if ( md->hasColor() ) {
        QColor c=md->colorData().value<QColor>();
        return c;
    }
    return QColor();
}

/*
QDataStream& operator<<( QDataStream &out, const PHIBaseItem *it )
{
    quint8 vid;
    foreach ( vid, it->_variants.keys() ) {
        qDebug( "%d=%d", vid, it->_variants.value( vid ).toInt() );
    }
    //qDebug( "<< VARIANT count %d", it->_variants.count() );
    if ( it->_effect->effects()!=PHIEffect::ENone ) {
        it->_effectData=it->_effect->save();
    } else it->_effectData.clear();
    return out << dynamic_cast<const PHIItem*>(it) << it->_variants;
}

QDataStream& operator>>( QDataStream &in, PHIBaseItem *it )
{
    in >> dynamic_cast<PHIItem*>(it) >> it->_variants;
    //qDebug( ">> VARIANT count %d", it->_variants.count() );
    if ( it->attributes() & PHIItem::AEffectData ) it->_effect->load( it->_effectData );
    else it->_effect->clearAll();
    return in;
}

PHIBaseStyle::PHIBaseStyle( PHIBaseItem *it )
    : QObject( it ), _it( it )
{
    setObjectName( QStringLiteral( "style" ) );
}

void PHIBaseStyle::setVisibility( const QString &s )
{
    if ( s.toLower()==QLatin1String( "hidden" ) ) _it->setVisible( false );
    else _it->setVisible( true );
}

QString PHIBaseStyle::visibility() const
{
    if ( _it->visible() ) return QStringLiteral( "visible" );
    else return QStringLiteral( "hidden" );
}

PHIBaseEffect::PHIBaseEffect( PHIBaseItem *it )
    : QObject( it ), _it( it )
{
    setObjectName( QStringLiteral( "effect" ) );
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
*/
