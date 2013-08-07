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
#include <QUndoStack>
#include "phibaseitem.h"
#include "phibasepage.h"
#include "phigraphicsitem.h"
#include "qpixmapfilter_p.h"
#include "phiitemstylecss.h"
#include "phisrequest.h"

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

PHIBaseItem::PHIBaseItem( Type type, PHIBasePage *page )
    : QObject( page ), _type( type ), _gw( 0 )
{
    _x=0;
    _y=0;
    _width=100.;
    _height=100.;
    if ( type==TIDEItem || type==TClientItem || type==TTemplateItem ) {
        _gw=new PHIGraphicsItem( this );
    }
    setTransformPos( PHI::TopLeft );
}

PHIBaseItem::~PHIBaseItem()
{
    if ( _gw ) {
        QGraphicsScene *s=_gw->scene();
        if ( s ) s->removeItem( _gw ); // get ownership back from scene
        delete _gw;
        _gw=0;
    }
    //qDebug( "PHIBaseItem::~PHIBaseItem()" );
}

void PHIBaseItem::load( QDataStream &in, quint8 version )
{
    in >> _id >> _parentId >> _x >> _y >> _width >> _height >> _variants;
    setObjectName( QString::fromUtf8( _id ) );
    loadItemData( in, version );
    loadEditorData( in, version );
}

QStringList PHIBaseItem::properties() const
{
    return _myproperties( this );
}

QUndoStack* PHIBaseItem::undoStack() const
{
    Q_ASSERT( _gw );
    PHIGraphicsScene *scene=qobject_cast<PHIGraphicsScene*>(_gw->scene());
    Q_ASSERT( scene );
    return scene->undoStack();
}

void PHIBaseItem::setFont( const QFont &font )
{
    if ( _type==TTemplateItem ) return;
    Q_ASSERT( parent() );
    QFont pf=qobject_cast<PHIBasePage*>(parent())->font();
    if ( pf==font ) {
        _variants.remove( DFont );
        if ( _gw ) _gw->setFont( pf );
        return;
    }
    _variants.insert( DFont, font );
    if ( _gw ) _gw->setFont( font );
}

QFont PHIBaseItem::font() const
{
    Q_ASSERT( parent() );
    return _variants.value( DFont, qobject_cast<PHIBasePage*>(parent())->font() ).value<QFont>();
}

void PHIBaseItem::setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &color )
{
    Q_UNUSED( ir )
    Q_UNUSED( cr )
    Q_UNUSED( color )
}

void PHIBaseItem::phiPaletteChanged( const PHIPalette &pal )
{
    Q_UNUSED( pal )
}

QRectF PHIBaseItem::boundingRect() const
{
    return rect();
}

void PHIBaseItem::setTransformPos( quint8 pos )
{
    int p=qBound( 0, static_cast<int>(pos), 9 );
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
    default:
        o=_variants.value( DTransformOrigin, QPointF( 0, 0 ) ).toPointF();
    }
    _variants.insert( DTransformOrigin, o );
    if ( !_gw ) return;
    _gw->setTransformOriginPoint( o );
}

void PHIBaseItem::setTransformOrigin( const QPointF &pos )
{
    _variants.insert( DTransformPos, 0 );
    _variants.insert( DTransformOrigin, pos );
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
    Q_UNUSED( which )
    Q_UNUSED( constraint )
    return QSizeF(); // invalid size: call base implementation of QGraphicsProxyWidget
}

void PHIBaseItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *options, QWidget *widget )
{
    Q_UNUSED( widget );
    painter->setOpacity( opacity() );
    paint( painter, options->exposedRect );
    //if ( _gw->isSelected() && _type==TIDEItem ) idePaintSelection( painter );
}

void PHIBaseItem::paint( QPainter *painter, const QRectF &exposed )
{
    Q_ASSERT( _gw );
    if ( !_gw->widget() ) return;
    const QRect exposedRect=( exposed & rect() ).toAlignedRect();
    if ( exposedRect.isEmpty() ) return;
    _gw->widget()->render( painter, exposedRect.topLeft(), exposedRect );
}

void PHIBaseItem::idePaintSelection( QPainter *painter )
{
    Q_UNUSED( painter )
}

void PHIBaseItem::paintHighlight( QPainter *painter )
{
    Q_UNUSED( painter )
}

void PHIBaseItem::loadItemData( QDataStream &in, quint8 version )
{
    Q_UNUSED( in )
    Q_UNUSED( version )
}

void PHIBaseItem::saveItemData( QDataStream &out, quint8 version ) const
{
    Q_UNUSED( out )
    Q_UNUSED( version )
}

void PHIBaseItem::loadEditorData( QDataStream &in, quint8 version )
{
    Q_UNUSED( in )
    Q_UNUSED( version )
}

void PHIBaseItem::saveEditorData( QDataStream &out, quint8 version ) const
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
    e->ignore();
}

void PHIBaseItem::squeeze( const PHISRequest* const req )
{
    Q_UNUSED( req )
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

}



/*
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

PHIBaseItem::PHIBaseItem( quint16 wid, const QByteArray &id, QObject *parent )
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
