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
//#include <QMatrix4x4>
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
    if ( !isIdeItem() && _gw ) _gw->setVisible( b );
}

void PHIBaseItem::privateUpdateData()
{
    Q_ASSERT( page() );
    if ( _styleSheetData.unparsedStatic() ) setStyleSheet( _styleSheetData.text() );
    else if ( _styleSheetData.translated() ) setStyleSheet( _styleSheetData.text( page()->currentLang() ) );
    else setStyleSheet( QString() );
    if ( _disabledData.unparsedStatic() ) setDisabled( _disabledData.boolean() );
    else if ( _disabledData.translated() ) setDisabled( _disabledData.boolean( page()->currentLang() ) );
    else setDisabled( false );
    updateData();
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
    if ( Q_UNLIKELY( version<3 ) ) return loadVersion1_x( arr );
    Q_ASSERT( page() );
    QByteArray a( arr );
    QDataStream in( &a, QIODevice::ReadOnly );
    in.setVersion( PHI_DSV2 );
    in >> _x >> _y >> _width >> _height >> _zIndex >> _variants;
    setObjectName( QString::fromUtf8( _id ) );
    QSizeF preserve=QSizeF( _width, _height );
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

    setTransformPos( _variants.value( DTransformPos, 1 ).value<quint8>() );
    setFont( font() ); // setFont may change height depending on sizeHint
    resize( preserve );
    if ( isIdeItem() ) privateUpdateData();
    if ( _gw ) {
        _gw->setPos( _x, _y );
        _gw->setTransform( computeTransformation() );
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
    privateSqueeze();
    if ( _xRot ) _variants.insert( DXRot, _xRot );
    if ( _yRot ) _variants.insert( DYRot, _yRot );
    if ( _zRot ) _variants.insert( DZRot, _zRot );
    if ( _hSkew ) _variants.insert( DHSkew, _hSkew );
    if ( _vSkew ) _variants.insert( DVSkew, _vSkew );
    if ( _transformOrigin!=QPointF() ) _variants.insert( DTransformOrigin, _transformOrigin );
    if ( !_parentId.isEmpty() ) _variants.insert( DParentId, _parentId );

    if ( _visibleData.unparsedStatic() && _visibleData.boolean() ) _flags &= ~FStoreVisibleData;
    else _flags |= FStoreVisibleData;
    if ( _titleData.unparsedStatic() && _titleData.text().isEmpty() ) _flags &= ~FStoreTitleData;
    else _flags |= FStoreTitleData;
    if ( _disabledData.unparsedStatic() && !_disabledData.boolean() ) _flags &= ~FStoreDisabledData;
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
    if ( hasGradient() ) {
        QGradient g=d.gradient();
        if ( g.type()==QGradient::ConicalGradient ) setGradient( d.conicalGradient() );
        else if ( g.type()==QGradient::LinearGradient ) setGradient( d.linearGradient() );
        else if ( g.type()==QGradient::RadialGradient ) setGradient( d.radialGradient() );
    }
    if ( isLayoutItem() ) {
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

void PHIBaseItem::setWidget( QWidget *w )
{
    QGraphicsProxyWidget *proxy=qgraphicsitem_cast<QGraphicsProxyWidget*>(_gw);
    if ( !proxy ) {
        delete w;
        return;
    }
    proxy->setWidget( w );
    proxy->setCacheMode( QGraphicsItem::ItemCoordinateCache );
    proxy->setFlag( QGraphicsItem::ItemUsesExtendedStyleOption, true );
}

QTransform PHIBaseItem::computeTransformation() const
{
    QTransform trans, shear, rotX, rotY, rotZ;
    trans.translate( -_transformOrigin.x(), -_transformOrigin.y() );
    shear.shear( _hSkew, _vSkew );
    rotZ.rotate( _zRot, Qt::ZAxis );
    rotY.rotate( _yRot, Qt::YAxis );
    rotX.rotate( _xRot, Qt::XAxis );
    return trans*shear*rotZ*rotY*rotX*trans.inverted();
    /*
    QMatrix4x4 m;
    m.translate( QVector3D( _transformOrigin ) );
    m.shear()
    m.rotate( _xRot, 1, 0, 0 );
    m.rotate( _yRot, 0, 1, 0 );
    m.rotate( _zRot, 0, 0, 1 );
    m.translate( QVector3D( -_transformOrigin ) );
    return t*m.toTransform();
    */
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
    if ( !page() ) qWarning( "setFont: page not set" );
    if ( page() ) pf=page()->font();
    if ( pf==font ) {
        _variants.remove( DFont );
    } else {
        _variants.insert( DFont, font );
        pf=font;
    }
    if ( _gw ) {
        pf.setPointSizeF( PHI::adjustedFontSize( font.pointSizeF() ) );
        _gw->setFont( pf );
        _gw->resize( _width, sizeHint( Qt::PreferredSize ).height() );
        _height=_gw->size().height();
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
    default:;
        return; // o==PHI::Custom (user defined origin)
    }
    QPointF org=computeTransformation().map( _transformOrigin );
    _transformOrigin=o;
    QTransform t=computeTransformation();
    QPointF delta=t.map( o )-org; // new - old
    setPos( realPos()-delta );
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

void PHIBaseItem::setText( const QString &t, const QByteArray &lang )
{
    // used in IDE only
    Q_UNUSED( t )
    Q_UNUSED( lang )
}

QString PHIBaseItem::text( const QByteArray &lang ) const
{
    // used in IDE only
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
    painter->setOpacity( realOpacity() );
    paint( painter, options->exposedRect );
    painter->restore();
}

void PHIBaseItem::paint( QPainter *painter, const QRectF &exposed )
{
    QGraphicsProxyWidget *proxy=qgraphicsitem_cast<QGraphicsProxyWidget*>(_gw);
    if ( !proxy ) return;
    if ( !proxy->widget() ) return;
    const QRect exposedRect=( exposed & rect() ).toAlignedRect();
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
            return true;
        default:;
        }
    }
    //qDebug( "Event %d", e->type() );
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

void PHIBaseItem::privateCreateTmpData( const PHIDataParser &parser )
{
    parser.setCurrentItem( this );
    privateSqueeze();
    _dirtyFlags=DFClean;
    if ( _titleData.unparsedStatic() ) _variants.insert( DTitle, _titleData.variant() );
    else _dirtyFlags |= DFTitleData;
    if ( _flags & FUseStyleSheet ) {
        if ( _styleSheetData.unparsedStatic() ) _variants.insert( DStyleSheet, _styleSheetData.variant() );
        else _dirtyFlags |= DFStyleSheetData;
    }
    if ( _visibleData.unparsedStatic() ) setVisible( _visibleData.boolean() );
    else _dirtyFlags |= DFVisibleData;
    if ( _disabledData.unparsedStatic() ) setDisabled( _disabledData.boolean() );
    else _dirtyFlags |= DFDisabledData;
    createTmpData( parser );
}

void PHIBaseItem::privateParseData( const PHIDataParser &parser )
{
    parser.setCurrentItem( this );
    if ( _dirtyFlags & DFTitleData ) _variants.insert( DTitle, parser.text( &_titleData ) );
    if ( Q_UNLIKELY( _dirtyFlags & DFStyleSheetData ) ) _variants.insert( DStyleSheet, parser.text( &_styleSheetData ) );
    if ( Q_UNLIKELY( _dirtyFlags & DFVisibleData ) ) setVisible( parser.text( &_visibleData ).toBool() );
    if ( Q_UNLIKELY( _dirtyFlags & DFDisabledData ) ) setDisabled( parser.text( &_disabledData ).toBool() );
    parseData( parser );
}

void PHIBaseItem::html( const PHIRequest *req, QByteArray &out, QByteArray &jquery, const QByteArray& indent ) const
{
    out+=indent+"<div";
    startCSS( req, out, jquery );
    out+="background-color:gray\">Unknown WID "+QByteArray::number( wid() )+"</div>\n";
}

void PHIBaseItem::privateStaticCSS( const PHIRequest *req, QByteArray &out ) const
{
    out+='#'+_id+BL( " {position:absolute;left:" )
        +QByteArray::number( qRound(_x) )+BL( "px;top:" )
        +QByteArray::number( qRound(_y) )+BL( "px;width:" )
        +QByteArray::number( qRound(_width) )+BL( "px;height:" )
        +QByteArray::number( qRound(_height) )+BL( "px;z-index:" )
        +QByteArray::number( _zIndex )+';';
    staticCSS( req, out );
    out+=BL( "}\n" );
    if ( _flags & FUseStyleSheet && !(_dirtyFlags & DFStyleSheetData) ) {
        genCustomStyleSheet( out );
    }
}

void PHIBaseItem::genCustomStyleSheet( QByteArray &out ) const
{
    QByteArray el=_variants.value( DStyleSheet ).toByteArray().simplified();
    PHIByteArrayList list=el.split( '}' );
    if ( list.count()==1 ) {
        if ( el.contains( '{' ) ) out+='#'+_id+' '+el+'\n';
        else out+='#'+_id+BL( " {" )+el+BL( "}\n" );
    } else {
        foreach( el, list ) {
            out+='#'+_id+' '+el+BL( "}\n" );
        }
    }
}

void PHIBaseItem::staticCSS( const PHIRequest *req, QByteArray &out ) const
{
    Q_UNUSED( req )
    Q_UNUSED( out )
}

void PHIBaseItem::startCSS( const PHIRequest *req, QByteArray &out, QByteArray &jquery, bool useTransform ) const
{
    if ( realOpacity()<1. ) jquery+=BL( "$('" )+_id+BL( "').opacity(" )
        +QByteArray::number( realOpacity(), 'f', 3 )+BL( ");\n" );
    out+=BL( " id=\"" )+_id+BL( "\" style=\"" );
    if ( hasGraphicEffect() ) graphicEffectCSS( req, out, jquery );
    if ( hasTransformation() && useTransform ) {
        QTransform t=computeTransformation();
        if ( Q_LIKELY( req->agentFeatures() & PHIRequest::Transform3D ) ) {
            QByteArray prefix=req->agentPrefix();
            if ( req->agentEngine()==PHIRequest::Trident ) prefix=QByteArray();
            out+=prefix+BL( "transform-origin:0 0;" )+prefix+BL( "transform:matrix3d(" )
                +QByteArray::number( t.m11(), 'f', 3 )+','
                +QByteArray::number( t.m12(), 'f', 3 )+BL( ",0," )
                +QByteArray::number( t.m13(), 'f', 3 )+','
                +QByteArray::number( t.m21(), 'f', 3 )+','
                +QByteArray::number( t.m22(), 'f', 3 )+BL( ",0," )
                +QByteArray::number( t.m23(), 'f', 3 )+BL( ",0,0,1,0," )
                +QByteArray::number( t.m31(), 'f', 3 )+','
                +QByteArray::number( t.m32(), 'f', 3 )+BL( ",0," )
                +QByteArray::number( t.m33(), 'f', 3 )+BL( ");" );
            if ( transformPos()!=1 ) {
                QPointF o=transformOrigin();
                out+=prefix+BL( "transform-origin:" )
                    +QByteArray::number( qRound(o.x()) )+' '
                    +QByteArray::number( qRound(o.y()) )+';';
            }
        } else if ( Q_UNLIKELY( req->agentFeatures() & PHIRequest::IE678 ) ) {
            QByteArray filter;
            // Qt has row major ordering, MSIE has col major ordering so we need to swap m21 with m12
            filter+="progid:DXImageTransform.Microsoft.Matrix(M11="+QByteArray::number( t.m11(), 'f', 3 )
                 +",M12="+QByteArray::number( t.m21(), 'f', 3 )+",M21="+QByteArray::number( t.m12(), 'f', 3 )
                 +",M22="+QByteArray::number( t.m22(), 'f', 3 )+BL( ",sizingMethod='auto expand')" );
            if ( _variants.value( DIEFilter ).isValid() ) filter+=_variants.value( DIEFilter ).toByteArray();
            _variants.insert( DIEFilter, filter );
        } else { // 2D transformation
            out+=req->agentPrefix()+BL( "transform-origin:0 0;" )
                +req->agentPrefix()+BL( ":matrix(" )+QByteArray::number( t.m11(), 'f', 3 )
                +','+QByteArray::number( t.m12(), 'f', 3 )+','+QByteArray::number( t.m21(), 'f', 3 )
                +','+QByteArray::number( t.m22(), 'f', 3 )+','+QByteArray::number( qRound(t.m31()) )
                +','+QByteArray::number( qRound(t.m32()) )+BL( ");" );
            if ( transformPos()!=1 ) {
                QPointF o=transformOrigin();
                out+=req->agentPrefix()+BL( "transform-origin:" )
                    +QByteArray::number( qRound(o.x()) )+' '
                    +QByteArray::number( qRound(o.y()) )+';';
            }
        }
    }
    if ( Q_UNLIKELY( req->agentFeatures() & PHIRequest::IE678 ) ) {
        if ( _variants.value( DIEFilter ).isValid() ) {
            out+=BL( "filter:" )+_variants.value( DIEFilter ).toByteArray()+';';
        }
    }
    if ( Q_UNLIKELY( _dirtyFlags & DFStyleSheetData ) ) genCustomStyleSheet( out );
}

void PHIBaseItem::graphicEffectCSS( const PHIRequest *req, QByteArray &out, QByteArray &jquery ) const
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
            if ( xOff<0 ) jquery+=BL( "$('" )+_id+BL( "').x(" )
                +QByteArray::number( qRound(_x+xOff) )+BL( ");\n" );
            if ( yOff<0 ) jquery+=BL( "$('" )+_id+BL( "').y(" )
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
            out+=QByteArray::number( qRound(radius) )+"px "+rgba( c )+';';
        }
    } else if ( _effect->graphicsType()==PHIEffect::GTBlur ) {
        if ( Q_UNLIKELY( req->agentFeatures() & PHIRequest::IE678 ) ) {
            QByteArray filter;
            qreal radius;
            _effect->blur( radius );
            filter+="progid:DXImageTransform.Microsoft.Blur(PixelRadius=";
            filter+=QByteArray::number( radius )+')';
            jquery+=BL( "$('" )+_id+BL( "').x(" )
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

void PHIBaseItem::genLinearGradient( const PHIRequest *req, QByteArray &out ) const
{
    QLinearGradient g=linearGradient();

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
            shadow->setBlurRadius( eRadius );
            shadow->setColor( eColor );
            shadow->setXOffset( eXOff );
            shadow->setYOffset( eYOff );
            _gw->setGraphicsEffect( shadow );
            break;
        }
        case PHIEffect::GTReflection: {
            _effect->surface( eYOff, eRadius );
            PHIReflectionEffect *surface=new PHIReflectionEffect();
            surface->setYOffset( eYOff );
            surface->setSize( eRadius );
            _gw->setGraphicsEffect( surface );
            break;
        }
        default:;
        }
    } else _gw->setGraphicsEffect( 0 );
}

QImage PHIBaseItem::createImage()
{
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
