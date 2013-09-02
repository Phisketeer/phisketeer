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
#include <QMatrix4x4>
#include <QGuiApplication>
#include "phibaseitem.h"
#include "phibasepage.h"
#include "phigraphicsitem.h"
#include "qpixmapfilter_p.h"
#include "phiitemstylecss.h"
#include "phisrequest.h"
#include "phiitemdata.h"

QScriptValue baseItemToScriptValue( QScriptEngine *engine, PHIBaseItem* const &it )
{
    new PHIItemStyleCSS( it );
    return engine->newQObject( it, QScriptEngine::AutoOwnership,
        QScriptEngine::PreferExistingWrapperObject
        | QScriptEngine::ExcludeDeleteLater );
}

static QStringList _myproperties( const QObject *obj )
{
    Q_ASSERT( obj );
    const QMetaObject *mo=obj->metaObject();
    QStringList properties;
    for( int i=1; i < mo->propertyCount(); ++i )
        properties << QString::fromLatin1( mo->property( i ).name() );
    properties << QStringLiteral( "style" ) << QStringLiteral( "effect" );
    properties.sort();
    return properties;
}

PHIBaseItem::PHIBaseItem()
    : QObject( 0 ), _type( TUndefined ), _gw( 0 )
{
    _x=_y=_xRot=_yRot=_zRot=_hSkew=_vSkew=0;
    setTransformPos( PHI::TopLeft );
    if ( !PHIGraphicsItemProvider::instance() ) return;
    setParent( PHIGraphicsItemProvider::instance()->currentBasePage() );
    _type=PHIGraphicsItemProvider::instance()->currentItemType();
    _gw=PHIGraphicsItemProvider::instance()->createGraphicsItem( this );
}

PHIBaseItem::~PHIBaseItem()
{
    if ( _gw ) {
        QGraphicsScene *s=_gw->scene();
        if ( s ) s->removeItem( _gw ); // get ownership back from scene
        delete _gw;
        _gw=0;
    }
}

PHIBasePage* PHIBaseItem::page() const
{
    return qobject_cast<PHIBasePage*>(parent());
}

void PHIBaseItem::squeeze()
{
    _variants.remove( DTransformOrigin );
    _variants.remove( DXRot );
    _variants.remove( DYRot );
    _variants.remove( DZRot );
    _variants.remove( DHSkew );
    _variants.remove( DVSkew );
    _variants.remove( DParentId );
    _variants.remove( DFlags );
    _variants.squeeze();
}

void PHIBaseItem::load( const QByteArray &arr, int version )
{
    if ( version<3 ) return loadVersion1_x( arr );
    QByteArray a( arr );
    QDataStream in( &a, QIODevice::ReadOnly );
    in.setVersion( PHI_DSV2 );
    in >> _x >> _y >> _width >> _height >> _zIndex >> _variants;
    setObjectName( QString::fromUtf8( _id ) );
    _xRot=_variants.value( DXRot, 0 ).toReal();
    _yRot=_variants.value( DYRot, 0 ).toReal();
    _zRot=_variants.value( DZRot, 0 ).toReal();
    _hSkew=_variants.value( DHSkew, 0 ).toReal();
    _vSkew=_variants.value( DVSkew, 0 ).toReal();
    _transformOrigin=_variants.value( DTransformOrigin, QPointF() ).toPointF();
    _parentId=_variants.value( DParentId, QByteArray() ).toByteArray();
    _flags=static_cast<Flags>(_variants.value( DFlags, 0 ).value<qint32>());
    loadItemData( in, version );
    if ( hasTransformation() && _gw ) _gw->setTransform( computeTransformation() );
}

QByteArray PHIBaseItem::save( int version )
{
    QByteArray arr;
    QDataStream out( &arr, QIODevice::WriteOnly );
    out.setVersion( PHI_DSV2 );
    Q_ASSERT( version>2 );
    if ( _xRot ) _variants.insert( DXRot, _xRot );
    else _variants.remove( DXRot );
    if ( _yRot ) _variants.insert( DYRot, _yRot );
    else _variants.remove( DYRot );
    if ( _zRot ) _variants.insert( DZRot, _zRot );
    else _variants.remove( DZRot );
    if ( _hSkew ) _variants.insert( DHSkew, _hSkew );
    else _variants.remove( DHSkew );
    if ( _vSkew ) _variants.insert( DVSkew, _vSkew );
    else _variants.remove( DVSkew );
    if ( _transformOrigin==QPointF() ) _variants.remove( DTransformOrigin );
    else _variants.insert( DTransformOrigin, _transformOrigin );
    if ( _parentId.isEmpty() ) _variants.remove( DParentId );
    else _variants.insert( DParentId, _parentId );
    if ( _flags==FNone ) _variants.remove( DFlags );
    else _variants.insert( DFlags, static_cast<qint32>(_flags) );
    out << _x << _y << _width << _height << _zIndex << _variants;
    saveItemData( out, version );
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
    qDebug() << _id << wid() << _x << _y << width << height << zIndex;
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
    if ( attributes & ATransform ) in >> transform;
    if ( attributes & AParent ) in >> parent;
    if ( attributes & AOpacity ) in >> opacity;
    else opacity=1;
    if ( attributes & ADynamicData ) in >> dynData;
    if ( attributes & AExtensionData ) in >> extensions >> extensionData;
    else extensions=0;
    if ( attributes & AEffectData ) in >> effectData;
    if ( attributes & AEditorData ) in >> editorData;
    // loading color IDs from editorData
    if ( !editorData.isEmpty() ) loadEditorData1_x( editorData );
    if ( attributes & ATransformPos) in >> transformPos;
    else transformPos=1;
    setOpacity( opacity );
    setTransformOrigin( QPointF( xTrans, yTrans ) );
    setTransformPos( transformPos );
    setTransformation( sH, sV, xRot, yRot, zRot );
    qDebug() << sH << sV << xRot << yRot << zRot;
    loadDynData1_x( properties, dynData );
}

void PHIBaseItem::loadDynData1_x( qint32 properties, const QByteArray &arr )
{
    PHIItemData d;
    d.load( properties, arr );

}

void PHIBaseItem::loadEditorData1_x( const QByteArray &arr )
{
    if ( arr.isEmpty() ) return;
    QByteArray a( arr );
    QDataStream in( &a, QIODevice::ReadOnly );
    in.setVersion( PHI_DSV );
    quint8 col, outCol;
    in >> col >> outCol;
    QList <PHIPalette::ColorRole> map;
    map << PHIPalette::Black << PHIPalette::White << PHIPalette::User1_100 << PHIPalette::User1_80
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

QStringList PHIBaseItem::properties() const
{
    return _myproperties( this );
}

void PHIBaseItem::setWidget( QWidget *w )
{
    QGraphicsProxyWidget *proxy=qgraphicsitem_cast<QGraphicsProxyWidget*>(_gw);
    if ( !proxy ) return;
    proxy->setWidget( w );
    if ( !w ) return;
    if ( sizeHint( Qt::PreferredSize ).isValid() )
        resize( sizeHint( Qt::PreferredSize ) );
    else resize( w->sizeHint() );
    proxy->setCacheMode( QGraphicsItem::ItemCoordinateCache );
}

QWidget* PHIBaseItem::widget() const
{
    QGraphicsProxyWidget *proxy=qgraphicsitem_cast<QGraphicsProxyWidget*>(_gw);
    if ( !proxy ) return 0;
    return proxy->widget();
}

QTransform PHIBaseItem::computeTransformation() const
{
    QTransform t, trans, shear, rot;
    trans.translate( -_transformOrigin.x(), -_transformOrigin.y() );
    shear.shear( _hSkew, _vSkew );
    rot.rotate( _zRot, Qt::ZAxis );
    rot.rotate( _yRot, Qt::YAxis );
    rot.rotate( _xRot, Qt::XAxis );
    t=trans*shear*rot*trans.inverted();
    return t;
    /*
    QMatrix4x4 m;
    m.translate( QVector3D( _transformOrigin ) );
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
    if ( pf!=font() ) return;
    pf.setPointSizeF( PHI::adjustedFontSize( pf.pointSizeF() ) );
    _gw->setFont( pf );
    _gw->resize( _width, sizeHint( Qt::PreferredSize ).height() );
    _height=_gw->size().height();
}

void PHIBaseItem::setFont( const QFont &font )
{
    if ( _type==TTemplateItem ) return;
    QFont pf=QGuiApplication::font();
    if ( !page() ) qWarning( "setFont: page not set" );
    if ( page() ) pf=page()->font();
    if ( pf==font ) {
        _variants.remove( DFont );
        pf.setPointSizeF( PHI::adjustedFontSize( font.pointSizeF() ) );
        if ( _gw ) {
            _gw->setFont( pf );
            _gw->resize( _width, sizeHint( Qt::PreferredSize ).height() );
            _height=_gw->size().height();
        }
        return;
    }
    _variants.insert( DFont, font );
    pf=font;
    pf.setPointSizeF( PHI::adjustedFontSize( font.pointSizeF() ) );
    if ( _gw ) {
        _gw->setFont( pf );
        _gw->resize( _width, sizeHint( Qt::PreferredSize ).height() );
        _height=_gw->size().height();
    }
}

QFont PHIBaseItem::font() const
{
    QFont f=QGuiApplication::font();
    if ( !page() ) qWarning( "font: page not set" );
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
    QWidget *w=widget();
    if ( w ) {
        w->setPalette( pal.palette() );
    }
    for ( int i=0; i<PHIPalette::ItemRoleMax; i++ ) {
        PHIPalette::ItemRole itemRole=static_cast<PHIPalette::ItemRole>(i);
        PHIPalette::ColorRole role=colorRole( itemRole );
        if ( role==PHIPalette::NoRole ) continue;
        setColor( itemRole, role, pal.color( role ) );
    }
}

QRectF PHIBaseItem::boundingRect() const
{
    return rect();
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
    QPointF org=computeTransformation().map( QPointF() ); // old top left corner
    _transformOrigin=o;
    QTransform t=computeTransformation();
    QPointF delta=t.map( QPointF() )-org; // new top left - old top left
    setPos( pos()-delta );
    if ( _gw ) _gw->setTransform( t );
}

void PHIBaseItem::setTransformOrigin( const QPointF &position )
{   // user defined origin
    QPointF org=computeTransformation().map( QPointF() ); // old top left corner
    _transformOrigin=position;
    QTransform t=computeTransformation();
    QPointF delta=t.map( QPointF() )-org; // new top left - old top left
    setPos( pos()-delta );
    if ( _gw ) _gw->setTransform( t );
    setTransformPos( 0 );
}

void PHIBaseItem::setTransformation( qreal hs, qreal vs, qreal xRot, qreal yRot, qreal zRot )
{
    _hSkew=hs; _vSkew=vs; _xRot=xRot; _yRot=yRot; _zRot=zRot;
    if ( _gw ) _gw->setTransform( computeTransformation() );
}

void PHIBaseItem::setText( const QString &t, const QString &lang )
{
    Q_UNUSED( t )
    Q_UNUSED( lang )
}

QString PHIBaseItem::text( const QString &lang ) const
{
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
    painter->setOpacity( opacity() );
    paint( painter, options->exposedRect );
    painter->restore();
}

void PHIBaseItem::paint( QPainter *painter, const QRectF &exposed )
{
    Q_ASSERT( _gw );
    QGraphicsProxyWidget *proxy=qgraphicsitem_cast<QGraphicsProxyWidget*>(_gw);
    if ( !proxy ) return;
    if ( !proxy->widget() ) return;
    const QRect exposedRect=( exposed & rect() ).toAlignedRect();
    if ( exposedRect.isEmpty() ) return;
    painter->setRenderHint( QPainter::TextAntialiasing );
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
    if ( _type==TIDEItem ) {
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

void PHIBaseItem::html5( const PHISRequest* const req, QByteArray &out, const QByteArray& indent )
{
    Q_UNUSED( req );
    out+=indent+"<div id=\""+_id+"\">Unknown WID "+QByteArray::number( wid() )+"</div>\n";
}

void PHIBaseItem::strictHtml( const PHISRequest* const req, QByteArray &out, const QByteArray &indent )
{
    return html5( req, out, indent );
}

void PHIBaseItem::jQuery( const PHISRequest* const req, QByteArray &out )
{
    Q_UNUSED( req )
    Q_UNUSED( out )
}

void PHIBaseItem::css( const PHISRequest* const req, QByteArray &out )
{
    Q_UNUSED( req )
    Q_UNUSED( out )
}

/*
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
