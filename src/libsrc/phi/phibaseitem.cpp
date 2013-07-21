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
#include "phibaseitem.h"
#include "phibasepage.h"
#include "phigraphicsitem.h"
#include "qpixmapfilter_p.h"
#include "phiitemstylecss.h"

bool PHIBaseItem::_tabModeOn=false;
quint8 PHIBaseItem::_gripSize=8;
QColor PHIBaseItem::_selectionColor=QColor( Qt::darkRed );

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
    qDebug( "PHIBaseItem::PHIBaseItem()" );
    _x=0;
    _y=0;
    _width=100.;
    _height=100.;
    if ( type==TIDEItem || type==TClientItem ) {
        _gw=new PHIGraphicsItem( this );
    }
}

PHIBaseItem::~PHIBaseItem()
{
    if ( _gw ) {
        QGraphicsScene *s=_gw->scene();
        if ( s ) s->removeItem( _gw ); // get ownership back from scene
        delete _gw;
        _gw=0;
    }
    qDebug( "PHIBaseItem::~PHIBaseItem()" );
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

void PHIBaseItem::setFont( const QFont &font )
{
    Q_ASSERT( parent() );
    QFont f=qobject_cast<PHIBasePage*>(parent())->font();
    f.resolve( font );
    _variants.insert( DFont, f );
    if ( _gw ) _gw->setFont( f );
}

QFont PHIBaseItem::font() const
{
    Q_ASSERT( parent() );
    return _variants.value( DFont, qobject_cast<PHIBasePage*>(parent())->font() ).value<QFont>();
}

void PHIBaseItem::paletteColorChange( PHIPalette::ColorRole role, const QColor &color )
{
    Q_UNUSED( role )
    Q_UNUSED( color )
}

void PHIBaseItem::setColor( PHIPalette::ColorRole role, const QColor &color, quint8 percent )
{
    Q_UNUSED( role )
    Q_UNUSED( color )
    Q_UNUSED( percent )
}

QRectF PHIBaseItem::boundingRect() const
{
    return QRectF( -1., -1., _width+1, _height+1 );
}

QPainterPath PHIBaseItem::shape() const
{
    QPainterPath path;
    path.addRect( rect() );
    return path;
}

QSizeF PHIBaseItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    Q_UNUSED( which )
    Q_UNUSED( constraint )
    return QSizeF(); // invalid size: call base implementation of QGraphicsProxyWidget
}

void PHIBaseItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *options, QWidget *widget )
{
    Q_UNUSED( widget );
    Q_ASSERT( _gw );
    if ( !_gw->isVisible() ) return;
    paint( painter, options->exposedRect );
    if ( _gw->isSelected() ) paintSelection( painter );
}

void PHIBaseItem::paint( QPainter *painter, const QRectF &exposed )
{
    Q_ASSERT( _gw );
    if ( !_gw->widget() ) return;
    const QRect exposedRect=( exposed & rect() ).toAlignedRect();
    if ( exposedRect.isEmpty() ) return;
    _gw->widget()->render( painter, exposedRect.topLeft(), exposedRect );
}

void PHIBaseItem::paintSelection( QPainter *painter )
{
    painter->save();
    painter->setOpacity( 1. );
    painter->setRenderHint( QPainter::Antialiasing, false );
    QPen pen;
    pen.setColor( _selectionColor );
    pen.setStyle( Qt::DashLine );
    painter->setPen( pen );
    painter->drawRoundedRect( 0, 0, _width, _height, 5., 5. );
    painter->restore();
    QPixmapColorizeFilter f;
    f.setColor( Qt::darkMagenta );
    f.setStrength( 1. );
    QPixmap p( QLatin1String( ":/items/images/button.png" ) );
    f.draw( painter, QPointF(), p );
}

void PHIBaseItem::paintHighlight( QPainter *painter )
{

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
            return ideResizeEvent( static_cast<QGraphicsSceneResizeEvent*>(e) );
        case QEvent::GraphicsSceneDragEnter:
            return ideDragEnterEvent( static_cast<QGraphicsSceneDragDropEvent*>(e) );
        case QEvent::GraphicsSceneDragMove:
            return ideDragMoveEvent( static_cast<QGraphicsSceneDragDropEvent*>(e) );
        case QEvent::GraphicsSceneDragLeave:
            return ideDragLeaveEvent( static_cast<QGraphicsSceneDragDropEvent*>(e) );
        case QEvent::GraphicsSceneDrop:
            return ideDropEvent( static_cast<QGraphicsSceneDragDropEvent*>(e) );
        case QEvent::GraphicsSceneMousePress:
            return false; // forward to PHIGraphicsItem
        case QEvent::GraphicsSceneMouseMove:
            return false;
        case QEvent::GraphicsSceneMouseRelease:
            return false;
        default:;
        }
    }
    //qDebug( "Event %d", e->type() );
    return false;
}

bool PHIBaseItem::ideResizeEvent( QGraphicsSceneResizeEvent *e )
{
    qDebug( "ideResizeEvent %f %f", e->newSize().width(), e->newSize().height() );
    _width=e->newSize().width();
    _height=e->newSize().height();
    return true;
}

bool PHIBaseItem::ideDragEnterEvent( QGraphicsSceneDragDropEvent *e )
{
    qDebug( "ide drag enter" );
    //e->setDropAction( Qt::IgnoreAction );
    e->ignore();
    return true;
}

bool PHIBaseItem::ideDragMoveEvent( QGraphicsSceneDragDropEvent *e )
{
    //qDebug( "idedragmove" );
    return true;
}

bool PHIBaseItem::ideDragLeaveEvent( QGraphicsSceneDragDropEvent *e )
{
    qDebug( "ide drag leave" );
    return true;
}

bool PHIBaseItem::ideDropEvent( QGraphicsSceneDragDropEvent *e )
{
    qDebug( "ide drop" );
    return true;
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
