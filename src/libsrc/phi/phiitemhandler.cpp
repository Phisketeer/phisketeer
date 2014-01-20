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
#include <QScriptEngine>
#include <QScriptValue>
#include <QTimer>
#include <QMimeData>
#include <QDrag>
#include <QCursor>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QPainter>
#include <QImage>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsView>
#include <QBuffer>
#include "phibaseitem.h"
#include "phidomevent.h"
#include "phianimations.h"
#include "phigraphicsscene.h"
#include "qpixmapfilter_p.h"

#define PHIBOUND(x) qBound( -10000, x, 10000 )

QPointF PHIBaseItem::adjustedPos() const
{
    QRectF br=boundingRect();
    if ( hasGraphicEffect() ) {
        switch ( _effect->graphicsType() ) {
        case PHIEffect::GTShadow: {
            QColor c;
            qreal offx, offy, radius;
            _effect->shadow( c, offx, offy, radius );
            QPixmapDropShadowFilter f;
            f.setOffset( offx, offy );
            f.setBlurRadius( radius );
            QRectF r=f.boundingRectFor( br );
            if ( r.x()<0 ) br.setX( br.x()+r.x() );
            if ( r.y()<0 ) br.setY( br.y()+r.y() );
            break;
        }
        case PHIEffect::GTColorize: {
           break;
        }
        case PHIEffect::GTBlur: {
            qreal radius;
            _effect->blur( radius );
            QPixmapBlurFilter f;
            f.setRadius( radius );
            f.setBlurHints( QGraphicsBlurEffect::AnimationHint );
            QRectF r=f.boundingRectFor( br );
            br.setX( br.x()+r.x()+1 );
            br.setY( br.y()+r.y()+1 );
            break;
        }
        case PHIEffect::GTReflection: {
            break;
        }
        default:;
        }
    }
    QTransform t=computeTransformation( false );
    return t.map( br.topLeft() );
}

QScriptValue PHIBaseItem::on( const QString &name, const QScriptValue &v )
{
    if ( !isClientItem() ) return self();
    PHIEventHash hash=data( DEventFunctions ).value<PHIEventHash>();
    hash.insert( name, v );
    qDebug() << "on" << name << v.toString();
    switch ( PHIDomEvent::stringToEventType( name ) ) {
    case PHIDomEvent::EMouseMove:
    case PHIDomEvent::EMouseOut:
    case PHIDomEvent::EMouseOver: _flags |= FHasHoverEventHandler; break;
    case PHIDomEvent::EClick:
    case PHIDomEvent::EDblClick:
    case PHIDomEvent::EMouseDown:
    case PHIDomEvent::EMouseUp: _flags |= FHasMouseEventHandler; break;
    case PHIDomEvent::EKeyDown:
    case PHIDomEvent::EKeyPress:
    case PHIDomEvent::EKeyUp: _flags |= FHasKeyEventHandler; break;
    case PHIDomEvent::EBlur:
    case PHIDomEvent::EFocus: _flags |= FHasFocusEventHandler; break;
    case PHIDomEvent::EChange: _flags |= FHasChangeEventHandler; break;
    case PHIDomEvent::EDrop: _flags |= FHasDropEventHandler; break;
    default:;
    }
    setData( DEventFunctions, qVariantFromValue( hash ) );
    return self();
}

QScriptValue PHIBaseItem::one( const QString &name, const QScriptValue &v )
{
    if ( !isClientItem() ) return self();
    PHIEventHash hash=data( DOneEventFunctions ).value<PHIEventHash>();
    hash.insert( name, v );
    qDebug() << "one" << name << v.toString();
    switch ( PHIDomEvent::stringToEventType( name ) ) {
    case PHIDomEvent::EMouseMove:
    case PHIDomEvent::EMouseOut:
    case PHIDomEvent::EMouseOver: _flags |= FHasHoverEventHandler; break;
    case PHIDomEvent::EClick:
    case PHIDomEvent::EDblClick:
    case PHIDomEvent::EMouseDown:
    case PHIDomEvent::EMouseUp: _flags |= FHasMouseEventHandler; break;
    case PHIDomEvent::EKeyDown:
    case PHIDomEvent::EKeyPress:
    case PHIDomEvent::EKeyUp: _flags |= FHasKeyEventHandler; break;
    case PHIDomEvent::EBlur:
    case PHIDomEvent::EFocus: _flags |= FHasFocusEventHandler; break;
    case PHIDomEvent::EChange: _flags |= FHasChangeEventHandler; break;
    case PHIDomEvent::EDrop: _flags |= FHasDropEventHandler; break;
    default:;
    }
    setData( DOneEventFunctions, qVariantFromValue( hash ) );
    return self();
}

QScriptValue PHIBaseItem::off( const QString &name, const QScriptValue &fn )
{
    if ( !isClientItem() ) return self();
    PHIEventHash hash=data( DEventFunctions ).value<PHIEventHash>();
    PHIEventHash::iterator i=hash.begin();
    while ( i!=hash.end() ) {
        if ( i.key()!=name ) {
            i++;
            continue;
        }
        if ( !fn.isValid() ) {
            qDebug() << "off " << i.key() << i.value().toString();
            i=hash.erase( i );
            continue;
        } else if ( fn.toString()==i.value().toString() ) {
            qDebug() << "off " << i.key() << i.value().toString();
            i=hash.erase( i );
            continue;
        }
        i++;
    }
    setData( DEventFunctions, qVariantFromValue( hash ) );
    return self();
}

QScriptValue PHIBaseItem::trigger( const QString &name, const QScriptValue &args, PHIDomEvent *de )
{
    if ( !isClientItem() ) return self();
    if ( name==SL( "drop" ) && !de ) return self(); // drop can not be triggered manually
    PHIEventHash hash=data( DEventFunctions ).value<PHIEventHash>();
    QScriptValueList functions=hash.values( name );
    PHIDomEvent fake( name, this );
    QScriptValue event, fn, res ;
    if ( de ) event=de->self();
    else event=scriptEngine()->toScriptValue( &fake );
    foreach ( fn, functions ) {
        if ( fn.isFunction() ) {
            res=fn.call( self(), QScriptValueList() << event << args );
        } else if ( fn.isString() ) res=scriptEngine()->evaluate( fn.toString() );
        if ( res.isError() ) {
            emit javaScriptError( res );
            qDebug() << res.toString();
        } else if ( de && res.isBool() ) if ( res.toBool()==false ) de->preventDefault();
    }
    hash=data( DOneEventFunctions ).value<PHIEventHash>();
    functions=hash.values( name );
    foreach( fn, functions ) {
        if ( fn.isFunction() ) {
            res=fn.call( self(), QScriptValueList() << event << args );
        } else if ( fn.isString() ) res=scriptEngine()->evaluate( fn.toString() );
        if ( res.isError() ) {
            emit javaScriptError( res );
            qDebug() << res.toString();
        } else if ( de && res.isBool() ) if ( res.toBool()==false ) de->preventDefault();
    }
    hash.remove( name );
    setData( DOneEventFunctions, qVariantFromValue( hash ) );
    return self();
}

QScriptValue PHIBaseItem::x( const QScriptValue &v )
{
    if ( !v.isValid() ) return qRound( realX() );
    setX( v.toNumber() );
    return self();
}

QScriptValue PHIBaseItem::y( const QScriptValue &v )
{
    if ( !v.isValid() ) return qRound( realY() );
    setY( v.toNumber() );
    return self();
}

QScriptValue PHIBaseItem::top( const QScriptValue &v )
{
    if ( !v.isValid() ) return qRound( realY()+adjustedPos().y() );
    setY( v.toNumber()-adjustedPos().y() );
    return self();
}

QScriptValue PHIBaseItem::left( const QScriptValue &v )
{
    if ( !v.isValid() ) return qRound( realX()+adjustedPos().x() );
    setX( v.toNumber()-adjustedPos().x() );
    return self();
}

QScriptValue PHIBaseItem::width( const QScriptValue &v )
{
    if ( !v.isValid() ) return qRound( realWidth() );
    setWidth( v.toNumber() );
    return self();
}

QScriptValue PHIBaseItem::height( const QScriptValue &v )
{
    if ( !v.isValid() ) return qRound( realHeight() );
    setHeight( v.toNumber() );
    return self();
}

QScriptValue PHIBaseItem::pos( const QScriptValue &x, const QScriptValue &y )
{
    if ( !x.isValid() ) {
        QPointF p=adjustedPos();
        QScriptValue v=scriptEngine()->newObject();
        v.setProperty( L1( "left" ), qRound( realX()+p.x() ) );
        v.setProperty( L1( "top" ), qRound( realY()+p.y() ) );
        v.setProperty( L1( "x" ), qRound( realX() ) );
        v.setProperty( L1( "y" ), qRound( realY() ) );
        return v;
    }
    setPos( x.toNumber(), y.toNumber() );
    return self();
}

QScriptValue PHIBaseItem::fadeIn( qint32 start, qint32 duration, qreal maxOpac, const QString &ease )
{
    _effect->setFadeIn( start, duration, maxOpac, PHI::toEasingCurveType( ease ) );
    if ( !isClientItem() ) return self();
    show();
    setOpacity( 0 );
    QPropertyAnimation *anim=new QPropertyAnimation( this, BL( "_opacity" ), this );
    anim->setEndValue( qBound( 0., maxOpac, 1. ) );
    anim->setDuration( duration );
    anim->setEasingCurve( PHI::toEasingCurveType( ease ) );
    connect( anim, &QAbstractAnimation::finished, anim, &QAbstractAnimation::deleteLater );
    QTimer::singleShot( start, anim, SLOT( start() ) );
    return self();
}

QScriptValue PHIBaseItem::fadeOut( qint32 start, qint32 duration, qreal minOpac, const QString &ease )
{
    _effect->setFadeOut( start, duration, minOpac, PHI::toEasingCurveType( ease ) );
    if ( !isClientItem() ) return self();
    show();
    setOpacity( 1. );
    QPropertyAnimation *anim=new QPropertyAnimation( this, BL( "_opacity" ), this );
    anim->setEndValue( qBound( 0., minOpac, 1. ) );
    anim->setDuration( duration );
    anim->setEasingCurve( PHI::toEasingCurveType( ease ) );
    connect( anim, &QAbstractAnimation::finished, this, &PHIBaseItem::privateOpacityHide );
    connect( anim, &QAbstractAnimation::finished, anim, &QAbstractAnimation::deleteLater );
    QTimer::singleShot( start, anim, SLOT( start() ) );
    return self();
}

QScriptValue PHIBaseItem::moveTo( qint32 left, qint32 top, qint32 start, qint32 duration, const QString &ease )
{
    _effect->setMoveTo( start, duration, left, top, PHI::toEasingCurveType( ease ) );
    if ( !isClientItem() ) return self();
    QParallelAnimationGroup *group=new QParallelAnimationGroup( this );
    QPropertyAnimation *l=new QPropertyAnimation( this, BL( "_x" ), group );
    l->setEndValue( PHIBOUND(left) );
    l->setDuration( duration );
    l->setEasingCurve( PHI::toEasingCurveType( ease ) );
    QPropertyAnimation *t=new QPropertyAnimation( this, BL( "_y" ), group );
    t->setEndValue( PHIBOUND(top) );
    t->setDuration( duration );
    t->setEasingCurve( PHI::toEasingCurveType( ease ) );
    group->addAnimation( l );
    group->addAnimation( t );
    connect( group, &QAbstractAnimation::finished, group, &QAbstractAnimation::deleteLater );
    QTimer::singleShot( start, group, SLOT( start() ) );
    return self();
}

QScriptValue PHIBaseItem::moveBy( qint32 left, qint32 top, qint32 width, qint32 height, qint32 start, qint32 duration, const QString &ease )
{
    QEasingCurve::Type curve=PHI::toEasingCurveType( ease );
    _effect->setMoveBy( start, duration, left, top, width, height, curve );
    if ( !isClientItem() ) return self();
    QParallelAnimationGroup *group=new QParallelAnimationGroup( this );
    QPropertyAnimation *l=new QPropertyAnimation( this, BL( "_x" ), group );
    l->setEndValue( PHIBOUND(left)+qRound( realX() ) );
    l->setDuration( duration );
    l->setEasingCurve( curve );
    QPropertyAnimation *t=new QPropertyAnimation( this, BL( "_y" ), group );
    t->setEndValue( PHIBOUND(top)+qRound( realY() ) );
    t->setDuration( duration );
    t->setEasingCurve( curve );
    QPropertyAnimation *w=new QPropertyAnimation( this, BL( "_width" ), group );
    w->setEndValue( PHIBOUND(width)+qRound( realWidth() ) );
    w->setDuration( duration );
    w->setEasingCurve( curve );
    QPropertyAnimation *h=new QPropertyAnimation( this, BL( "_height" ), group );
    h->setEndValue( PHIBOUND(height)+qRound( realHeight() ) );
    h->setDuration( duration );
    h->setEasingCurve( curve );
    group->addAnimation( l );
    group->addAnimation( t );
    group->addAnimation( w );
    group->addAnimation( h );
    connect( group, &QAbstractAnimation::finished, group, &QAbstractAnimation::deleteLater );
    QTimer::singleShot( start, group, SLOT( start() ) );
    return self();
}

QScriptValue PHIBaseItem::rotateIn( quint8 axis, qint32 start, qint32 duration, const QString &ease )
{
    QEasingCurve::Type curve=PHI::toEasingCurveType( ease );
    _effect->setRotateIn( axis, start, duration, curve );
    if ( !isClientItem() ) return self();
    show();
    QParallelAnimationGroup *group=new QParallelAnimationGroup( this );
    if ( axis & 0x1 ) {
        setXRotation( 90. );
        QPropertyAnimation *xrot=new QPropertyAnimation( this, BL( "_xRotation" ), group );
        xrot->setEndValue( 0 );
        xrot->setDuration( duration );
        xrot->setEasingCurve( curve );
        group->addAnimation( xrot );
    }
    if ( axis & 0x2 ) {
        setYRotation( 90. );
        QPropertyAnimation *yrot=new QPropertyAnimation( this, BL( "_yRotation" ), group );
        yrot->setEndValue( 0 );
        yrot->setDuration( duration );
        yrot->setEasingCurve( curve );
        group->addAnimation( yrot );
    }
    connect( group, &QAbstractAnimation::finished, group, &QAbstractAnimation::deleteLater );
    QTimer::singleShot( start, group, SLOT( start() ) );
    return self();
}

QScriptValue PHIBaseItem::rotateOut( quint8 axis, qint32 start, qint32 duration, const QString &ease )
{
    QEasingCurve::Type curve=PHI::toEasingCurveType( ease );
    _effect->setRotateOut( axis, start, duration, curve );
    if ( !isClientItem() ) return self();
    QParallelAnimationGroup *group=new QParallelAnimationGroup( this );
    if ( axis & 0x1 ) {
        setXRotation( 0 );
        QPropertyAnimation *xrot=new QPropertyAnimation( this, BL( "_xRotation" ), group );
        xrot->setEndValue( 90. );
        xrot->setDuration( duration );
        xrot->setEasingCurve( curve );
        group->addAnimation( xrot );
    }
    if ( axis & 0x2 ) {
        setYRotation( 0 );
        QPropertyAnimation *yrot=new QPropertyAnimation( this, BL( "_yRotation" ), group );
        yrot->setEndValue( 90. );
        yrot->setDuration( duration );
        yrot->setEasingCurve( curve );
        group->addAnimation( yrot );
    }
    connect( group, &QAbstractAnimation::finished, this, &PHIBaseItem::privateHide );
    connect( group, &QAbstractAnimation::finished, group, &QAbstractAnimation::deleteLater );
    QTimer::singleShot( start, group, SLOT( start() ) );
    return self();
}

QScriptValue PHIBaseItem::rotate( quint8 axis, qreal stepX, qreal stepY, qreal stepZ )
{
    _effect->setRotate( axis, stepX, stepY, stepZ );
    if ( !isClientItem() ) return self();
    QParallelAnimationGroup *group=new QParallelAnimationGroup( this );
    if ( axis & 0x1 && stepX!=0 ) {
        QPropertyAnimation *xrot=new QPropertyAnimation( this, BL( "_xRotation" ), group );
        xrot->setStartValue( 0. );
        xrot->setEndValue( stepX>0 ? 360. : -360. );
        xrot->setDuration( 9000/qAbs(stepX) );
        xrot->setLoopCount( -1 );
        group->addAnimation( xrot );
    }
    if ( axis & 0x2 && stepY!=0 ) {
        QPropertyAnimation *yrot=new QPropertyAnimation( this, BL( "_yRotation" ), group );
        yrot->setStartValue( 0. );
        yrot->setEndValue( stepY>0 ? 360. : -360. );
        yrot->setDuration( 9000/qAbs(stepY) );
        yrot->setLoopCount( -1 );
        group->addAnimation( yrot );
    }
    if ( axis & 0x4 && stepZ!=0 ) {
        QPropertyAnimation *zrot=new QPropertyAnimation( this, BL( "_zRotation" ), group );
        zrot->setStartValue( 0. );
        zrot->setEndValue( stepZ>0 ? 360. : -360. );
        zrot->setDuration( 9000/qAbs(stepZ) );
        zrot->setLoopCount( -1 );
        group->addAnimation( zrot );
    }
    group->start();
    return self();
}

QScriptValue PHIBaseItem::stop()
{
    if ( !isClientItem() ) return self();
    QList <QParallelAnimationGroup*> grouplist=findChildren<QParallelAnimationGroup*>(QString(), Qt::FindDirectChildrenOnly);
    foreach ( QParallelAnimationGroup *gan, grouplist ) {
        gan->stop();
        gan->deleteLater();
    }
    QList <QPropertyAnimation*> proplist=findChildren<QPropertyAnimation*>(QString(), Qt::FindDirectChildrenOnly);
    foreach ( QPropertyAnimation *pan, proplist ) {
        pan->stop();
        pan->deleteLater();
    }
    return self();
}

QScriptValue PHIBaseItem::cursor( const QScriptValue &v )
{
    if ( !isClientItem() ) return self();
    if ( !v.isValid() ) return QString::fromLatin1( PHI::toCursorString( _gw->cursor().shape() ) );
    _gw->setCursor( QCursor( PHI::toCursorShape( v.toString().toLatin1() ) ) );
    if ( widget() ) widget()->setCursor( _gw->cursor() );
    return self();
}

QScriptValue PHIBaseItem::rotateX( const QScriptValue &v )
{
    if ( !v.isValid() ) return xRotation();
    setXRotation( v.toNumber() );
    return self();
}

QScriptValue PHIBaseItem::rotateY( const QScriptValue &v )
{
    if ( !v.isValid() ) return yRotation();
    setYRotation( v.toNumber() );
    return self();
}

QScriptValue PHIBaseItem::rotateZ( const QScriptValue &v )
{
    if ( !v.isValid() ) return zRotation();
    setZRotation( v.toNumber() );
    return self();
}

QScriptValue PHIBaseItem::rotateToX( int angle, int duration, const QString &ease )
{
    if ( !isClientItem() ) return scriptEngine()->undefinedValue();
    QPropertyAnimation *xrot=new QPropertyAnimation( this, BL( "_xRotation" ) );
    xrot->setEndValue( angle );
    xrot->setDuration( duration );
    xrot->setEasingCurve( PHI::toEasingCurveType( ease ) );
    xrot->start( QAbstractAnimation::DeleteWhenStopped );
    return self();
}

QScriptValue PHIBaseItem::rotateToY( int angle, int duration, const QString &ease )
{
    if ( !isClientItem() ) return scriptEngine()->undefinedValue();
    QPropertyAnimation *yrot=new QPropertyAnimation( this, BL( "_yRotation" ) );
    yrot->setEndValue( angle );
    yrot->setDuration( duration );
    yrot->setEasingCurve( PHI::toEasingCurveType( ease ) );
    yrot->start( QAbstractAnimation::DeleteWhenStopped );
    return self();
}

QScriptValue PHIBaseItem::rotateToZ( int angle, int duration, const QString &ease )
{
    if ( !isClientItem() ) return scriptEngine()->undefinedValue();
    QPropertyAnimation *zrot=new QPropertyAnimation( this, BL( "_zRotation" ) );
    zrot->setEndValue( angle );
    zrot->setDuration( duration );
    zrot->setEasingCurve( PHI::toEasingCurveType( ease ) );
    zrot->start( QAbstractAnimation::DeleteWhenStopped );
    return self();
}

QScriptValue PHIBaseItem::slide( const QString &dir, int duration, const QString &ease )
{
    if ( !isClientItem() ) return scriptEngine()->undefinedValue();
    if ( dir==L1( "down" ) ) slideDown( duration, ease );
    else slideUp( duration, ease );
    return self();
}

void PHIBaseItem::slideUp( int duration, const QString &ease )
{
    Q_ASSERT( isClientItem() );
    QSequentialAnimationGroup *group=findChild<QSequentialAnimationGroup*>( L1( "slide" ), Qt::FindDirectChildrenOnly );
    if ( !group ) {
        group=new QSequentialAnimationGroup( this );
        connect( group, &QSequentialAnimationGroup::finished, this, &PHIBaseItem::slideAnimationFinished );
        group->setObjectName( L1( "slide" ) );
        prepareForAnimation();
    }
    QSizeF size=data( DAnimOrgGeometry ).toRectF().size();
    PHISlideAnimation *anim=new PHISlideAnimation( this, size.height(), true );
    anim->setDuration( duration );
    anim->setEasingCurve( PHI::toEasingCurveType( ease ) );
    group->addAnimation( anim );
    if ( !(group->state()==QAbstractAnimation::Running) ) group->start( QAbstractAnimation::DeleteWhenStopped );
}

void PHIBaseItem::slideDown( int duration, const QString &ease )
{
    Q_ASSERT( isClientItem() );
    QSequentialAnimationGroup *group=findChild<QSequentialAnimationGroup*>( L1( "slide" ), Qt::FindDirectChildrenOnly );
    if ( !group ) {
        group=new QSequentialAnimationGroup( this );
        connect( group, &QSequentialAnimationGroup::finished, this, &PHIBaseItem::slideAnimationFinished );
        group->setObjectName( L1( "slide" ) );
        prepareForAnimation();
    }
    QSizeF size=data( DAnimOrgGeometry ).toRectF().size();
    PHISlideAnimation *anim=new PHISlideAnimation( this, size.height(), false );
    anim->setDuration( duration );
    anim->setEasingCurve( PHI::toEasingCurveType( ease ) );
    group->addAnimation( anim );
    if ( !(group->state()==QAbstractAnimation::Running) ) group->start( QAbstractAnimation::DeleteWhenStopped );
}

QScriptValue PHIBaseItem::click( const QScriptValue &v )
{
    if ( v.isValid() ) return on( SL( "click" ), v );
    return trigger( SL( "click" ) );
}

QScriptValue PHIBaseItem::dblclick( const QScriptValue &v )
{
    if ( v.isValid() ) return on( SL( "dblclick" ), v );
    return trigger( SL( "dblclick" ) );
}

QScriptValue PHIBaseItem::mousedown( const QScriptValue &v )
{
    if ( v.isValid() ) return on( SL( "mousedown" ), v );
    return trigger( SL( "mousedown" ) );
}

QScriptValue PHIBaseItem::mouseup( const QScriptValue &v )
{
    if ( v.isValid() ) return on( SL( "mouseup" ), v );
    return trigger( SL( "mouseup" ) );
}

QScriptValue PHIBaseItem::mousemove( const QScriptValue &v )
{
    if ( v.isValid() ) return on( SL( "mousemove" ), v );
    return trigger( SL( "mousemove" ) );
}

QScriptValue PHIBaseItem::mouseover( const QScriptValue &v )
{
    if ( v.isValid() ) return on( SL( "mouseover" ), v );
    return trigger( SL( "mouseover" ) );
}

QScriptValue PHIBaseItem::mouseout( const QScriptValue &v )
{
    if ( v.isValid() ) return on( SL( "mouseout" ), v );
    return trigger( SL( "mouseout" ) );
}

QScriptValue PHIBaseItem::keydown( const QScriptValue &v )
{
    if ( v.isValid() ) return on( SL( "keydown" ), v );
    return trigger( SL( "keydown" ) );
}

QScriptValue PHIBaseItem::keypress( const QScriptValue &v )
{
    if ( v.isValid() ) return on( SL( "keypress" ), v );
    return trigger( SL( "keypress" ) );
}

QScriptValue PHIBaseItem::keyup( const QScriptValue &v )
{
    if ( v.isValid() ) return on( SL( "keyup" ), v );
    return trigger( SL( "keyup" ) );
}

QScriptValue PHIBaseItem::focus( const QScriptValue &v )
{
    if ( v.isValid() ) return on( SL( "focus" ), v );
    return trigger( SL( "focus" ) );
}

QScriptValue PHIBaseItem::blur( const QScriptValue &v )
{
    if ( v.isValid() ) return on( SL( "blur" ), v );
    return trigger( SL( "blur" ) );
}

QScriptValue PHIBaseItem::change( const QScriptValue &v )
{
    if ( v.isValid() ) return on( SL( "change" ), v );
    return trigger( SL( "change" ) );
}

QScriptValue PHIBaseItem::drop( const QScriptValue &v )
{
    if ( v.isValid() ) return on( SL( "drop" ), v );
    return self();
}

void PHIBaseItem::checkForDragInMousePressEvent( QGraphicsSceneMouseEvent *e )
{
    qDebug() << "checkForDragInMousePressEvent()" << _id;
    if ( !(e->buttons() & Qt::LeftButton) ) return;
    if ( !(dragDropOptions() & DDDragEnabled) ) return;
    setData( DDragStartPos, e->scenePos() );
    setData( DDragOriginalPos, realPos() );
}

void PHIBaseItem::checkForDragInMouseMoveEvent( QGraphicsSceneMouseEvent *e )
{
    qDebug( "checkForDragInMouseMoveEvent()" );
    Q_ASSERT( _gw );
    if ( !(e->buttons() & Qt::LeftButton) ) return;
    if ( !(dragDropOptions() & DDDragEnabled) ) return;
    if ( (e->pos() - data( DDragStartPos ).toPointF()).manhattanLength() < dragDistance() ) return;
    if ( dragMoveAction() ) hide();

    QImage img( qRound(realWidth()), qRound(realHeight()), QImage::Format_ARGB32_Premultiplied );
    img.fill( Qt::transparent );
    QPainter p( &img );
    p.setRenderHint( QPainter::Antialiasing );
    p.setRenderHint( QPainter::SmoothPixmapTransform );
    p.setOpacity( dragOpacity() );
    QStyleOptionGraphicsItem opt;
    if ( widget() ) widget()->render( &p );
    else paint( &p, &opt, 0 );
    p.end();

    QPixmap pix=QPixmap::fromImage( img, Qt::ColorOnly );
    QDrag *drag=new QDrag( _gw->scene()->views().first() );
    drag->setPixmap( pix );
    switch ( data( DDragHotSpotType ).toInt() ) {
    case 0: drag->setHotSpot( data( DDragHotSpot ).toPoint() ); break;
    default: drag->setHotSpot( QPoint( realWidth()/2, realHeight()/2 ) );
    }
    QMimeData *mimeData=new QMimeData();
    mimeData->setImageData( img );

    QByteArray output;
    QBuffer outputBuffer( &output );
    outputBuffer.open( QIODevice::WriteOnly );
    QDataStream ds( &outputBuffer );
    ds.setVersion( PHI_DSV2 );
    ds << static_cast<quint8>(1) << name() << data( DDragOriginalPos ).toPointF()
       << data( DDragDropOptions, 0 ).value<quint32>() << drag->hotSpot();
    mimeData->setData( L1( "application/x-phi-dd" ), output );
    drag->setMimeData( mimeData );
    Qt::DropAction action=drag->exec( dragMoveAction() ? Qt::MoveAction : Qt::CopyAction );

    if ( action==Qt::CopyAction ) return;
    if ( !dragMoveAction() ) return;

    // QPointF startPos=data( DDragStartPos ).toPointF();
    QPointF origPos=data( DDragOriginalPos ).toPointF();
    QPointF dragMovePos=qobject_cast<PHIGraphicsScene*>(_gw->scene())->dragMovePos();
    qDebug() << "drop (move action)" << _id << origPos << dragMovePos;
    setVisible( true );
    if ( action==Qt::IgnoreAction ) {
        if ( dragDropOptions() & DDRevertOnIgnore ) {
            // done by Qt:
            // moveTo( origPos.x(), origPos.y(), 0, 500, PHI::defaultEasingCurve() );
        } else {
            setPos( dragMovePos-drag->hotSpot() );
        }
    } else {
        if ( dragDropOptions() & DDRevertOnAccept ) {
            qreal curOpac=realOpacity();
            setOpacity( 0 );
            fadeIn( 0, 1000, curOpac, PHI::defaultEasingCurve() );
        } else {
            setPos( dragMovePos-drag->hotSpot() );
        }
    }
}

void PHIBaseItem::checkDragEnterEvent( QGraphicsSceneDragDropEvent *e )
{
    if ( !(dragDropOptions() & DDDropEnabled) ) return e->ignore();
    if ( !e->mimeData()->hasFormat( L1( "application/x-phi-dd" ) ) ) return e->ignore();

    QByteArray input=e->mimeData()->data( L1( "application/x-phi-dd" ) );
    QBuffer inputBuffer( &input );
    inputBuffer.open( QIODevice::ReadOnly );
    QDataStream ds( &inputBuffer );
    ds.setVersion( PHI_DSV2 );
    quint8 version;
    QString id;
    PHIByteArrayList accepted=data( DDropAcceptedIds ).value<PHIByteArrayList>();
    ds >> version >> id;
    qDebug() << "checkDragEnterEvent" << _id << id << accepted;
    if ( accepted.isEmpty() || accepted.contains( id.toLatin1() ) ) return e->accept();
    e->ignore();
}

void PHIBaseItem::checkDragMoveEvent( QGraphicsSceneDragDropEvent *e )
{
    e->acceptProposedAction();
}

void PHIBaseItem::checkDragLeaveEvent( QGraphicsSceneDragDropEvent *e )
{
    Q_UNUSED( e )
}

void PHIBaseItem::checkDropEvent( QGraphicsSceneDragDropEvent *e )
{
    if ( !(_flags & PHIBaseItem::FHasDropEventHandler) ) return;
    if ( !e->mimeData()->hasFormat( L1( "application/x-phi-dd" ) ) ) return;

    QByteArray input=e->mimeData()->data( L1( "application/x-phi-dd" ) );
    QBuffer inputBuffer( &input );
    inputBuffer.open( QIODevice::ReadOnly );
    QDataStream ds( &inputBuffer );
    ds.setVersion( PHI_DSV2 );
    quint8 version;
    QString sourceId;
    QPointF orgPos;
    quint32 opts;
    QPoint hotspot;
    ds >> version >> sourceId >> orgPos >> opts >> hotspot;
    qDebug() << "checkDropEvent" << _id << sourceId << hotspot << e->scenePos();

    PHIDomEvent dropevent( L1( "drop" ), this, false );
    dropevent.setDropEvent( e );
    QScriptValue ui=scriptEngine()->newObject();
    QScriptValue offset=scriptEngine()->newObject();
    QScriptValue position=scriptEngine()->newObject();
    QPoint off=dropevent.mapFromScene( e->scenePos() )-hotspot;
    QPoint pos=e->scenePos().toPoint()-hotspot;
    offset.setProperty( L1( "left" ), off.x() );
    offset.setProperty( L1( "top" ), off.y() );
    position.setProperty( L1( "left" ), pos.x() );
    position.setProperty( L1( "top" ), pos.y() );
    ui.setProperty( L1( "source" ), sourceId );
    ui.setProperty( L1( "target" ), name() );
    ui.setProperty( L1( "position" ), position );
    ui.setProperty( L1( "offset" ), offset );
    trigger( L1( "drop" ), ui, &dropevent );
}
