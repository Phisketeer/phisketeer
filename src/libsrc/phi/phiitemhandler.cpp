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
#include "phibaseitem.h"
#include "phidomevent.h"
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
    qDebug() << "on" << name;
    if ( !isClientItem() ) return self();
    PHIEventHash hash=data( DEventFunctions ).value<PHIEventHash>();
    hash.insert( name, v );
    qDebug() << "on" << name << v.toString();
    setData( DEventFunctions, qVariantFromValue( hash ) );
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
    if ( name==SL( "drop" ) || !isClientItem() ) return self();
    PHIEventHash hash=data( DEventFunctions ).value<PHIEventHash>();
    QScriptValueList functions=hash.values( name );
    if ( functions.count()==0 ) return self();
    qDebug() << "trigger" << name;
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

QScriptValue PHIBaseItem::pos( const QScriptValue &left, const QScriptValue &top )
{
    if ( !left.isValid() ) {
        QPointF p=adjustedPos();
        QScriptValue v=scriptEngine()->newObject();
        v.setProperty( L1( "left" ), qRound( realX()+p.x() ) );
        v.setProperty( L1( "top" ), qRound( realY()+p.y() ) );
        v.setProperty( L1( "x" ), qRound( realX() ) );
        v.setProperty( L1( "y" ), qRound( realY() ) );
        return v;
    }
    setPos( left.toNumber(), top.toNumber() );
    return self();
}

QScriptValue PHIBaseItem::fadeIn( qint32 start, qint32 duration, qreal maxOpac, const QString &ease )
{
    _effect->setFadeIn( start, duration, maxOpac, PHI::toEasingCurveType( ease ) );
    if ( !isClientItem() ) return self();
    show();
    setOpacity( 0 );
    QPropertyAnimation *anim=new QPropertyAnimation( this, BL( "_opacity" ), scriptEngine() );
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
    QPropertyAnimation *anim=new QPropertyAnimation( this, BL( "_opacity" ), scriptEngine() );
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
    QPropertyAnimation *l=new QPropertyAnimation( this, BL( "_x" ), scriptEngine() );
    l->setEndValue( PHIBOUND(left) );
    l->setDuration( duration );
    l->setEasingCurve( PHI::toEasingCurveType( ease ) );
    QPropertyAnimation *t=new QPropertyAnimation( this, BL( "_y" ), scriptEngine() );
    t->setEndValue( PHIBOUND(top) );
    t->setDuration( duration );
    t->setEasingCurve( PHI::toEasingCurveType( ease ) );
    QParallelAnimationGroup *group=new QParallelAnimationGroup( scriptEngine() );
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
    QPropertyAnimation *l=new QPropertyAnimation( this, BL( "_x" ), scriptEngine() );
    l->setEndValue( PHIBOUND(left)+qRound( realX() ) );
    l->setDuration( duration );
    l->setEasingCurve( curve );
    QPropertyAnimation *t=new QPropertyAnimation( this, BL( "_y" ), scriptEngine() );
    t->setEndValue( PHIBOUND(top)+qRound( realY() ) );
    t->setDuration( duration );
    t->setEasingCurve( curve );
    QPropertyAnimation *w=new QPropertyAnimation( this, BL( "_width" ), scriptEngine() );
    w->setEndValue( PHIBOUND(width)+qRound( realWidth() ) );
    w->setDuration( duration );
    w->setEasingCurve( curve );
    QPropertyAnimation *h=new QPropertyAnimation( this, BL( "_height" ), scriptEngine() );
    h->setEndValue( PHIBOUND(height)+qRound( realHeight() ) );
    h->setDuration( duration );
    h->setEasingCurve( curve );
    QParallelAnimationGroup *group=new QParallelAnimationGroup( scriptEngine() );
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
    QParallelAnimationGroup *group=new QParallelAnimationGroup( scriptEngine() );
    if ( axis & 0x1 ) {
        setXRotation( 90. );
        QPropertyAnimation *xrot=new QPropertyAnimation( this, BL( "_xRotation" ), scriptEngine() );
        xrot->setEndValue( 0 );
        xrot->setDuration( duration );
        xrot->setEasingCurve( curve );
        group->addAnimation( xrot );
    }
    if ( axis & 0x2 ) {
        setYRotation( 90. );
        QPropertyAnimation *yrot=new QPropertyAnimation( this, BL( "_yRotation" ), scriptEngine() );
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
    QParallelAnimationGroup *group=new QParallelAnimationGroup( scriptEngine() );
    if ( axis & 0x1 ) {
        setXRotation( 0 );
        QPropertyAnimation *xrot=new QPropertyAnimation( this, BL( "_xRotation" ), scriptEngine() );
        xrot->setEndValue( 90. );
        xrot->setDuration( duration );
        xrot->setEasingCurve( curve );
        group->addAnimation( xrot );
    }
    if ( axis & 0x2 ) {
        setYRotation( 0 );
        QPropertyAnimation *yrot=new QPropertyAnimation( this, BL( "_yRotation" ), scriptEngine() );
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
    qDebug() << "rotate" << axis << stepZ;
    _effect->setRotate( axis, stepX, stepY, stepZ );
    if ( !isClientItem() ) return self();
    QParallelAnimationGroup *group=new QParallelAnimationGroup( scriptEngine() );
    if ( axis & 0x1 && stepX!=0 ) {
        QPropertyAnimation *xrot=new QPropertyAnimation( this, BL( "_xRotation" ), scriptEngine() );
        xrot->setStartValue( 0. );
        xrot->setEndValue( stepX>0 ? 360. : -360. );
        xrot->setDuration( 9000/qAbs(stepX) );
        xrot->setLoopCount( -1 );
        group->addAnimation( xrot );
    }
    if ( axis & 0x2 && stepY!=0 ) {
        QPropertyAnimation *yrot=new QPropertyAnimation( this, BL( "_yRotation" ), scriptEngine() );
        yrot->setStartValue( 0. );
        yrot->setEndValue( stepY>0 ? 360. : -360. );
        yrot->setDuration( 9000/qAbs(stepY) );
        yrot->setLoopCount( -1 );
        group->addAnimation( yrot );
    }
    if ( axis & 0x4 && stepZ!=0 ) {
        QPropertyAnimation *zrot=new QPropertyAnimation( this, BL( "_zRotation" ), scriptEngine() );
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
    QList <QAbstractAnimation*> list=findChildren<QAbstractAnimation*>(QString(), Qt::FindDirectChildrenOnly);
    foreach ( QAbstractAnimation *anim, list ) {
        anim->stop();
        anim->deleteLater();
    }
    return self();
}

QScriptValue PHIBaseItem::cursor( const QScriptValue &v )
{
    if ( !isClientItem() ) return self();
    if ( !v.isValid() ) return QString::fromLatin1( PHI::toCursorString( _gw->cursor().shape() ) );
    _gw->setCursor( QCursor( PHI::toCursorShape( v.toString().toLatin1() ) ) );
    return self();
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

/*

void PHIAItem::slotOnClick( QGraphicsSceneMouseEvent *e )
{
    qDebug( "Item onclick %s", id().data() );
    QScriptEngine *engine=view()->scriptEngine();
    if ( !engine ) return;
    bool isSubmit( wid()==PHI::SUBMIT_BUTTON );
    bool isReset( wid()==PHI::RESET_BUTTON );
    bool isButton( wid()==PHI::BUTTON || wid()==PHI::IMAGE_BUTTON );
    bool isRollover( wid()==PHI::ROLLOVER_BUTTON || wid()==PHI::PHISYS_LINK || wid()==PHI::LINK );
    QScriptValue item=baseItemToScriptValue( engine, this );
    QScriptValue onclick=item.property( "onclick" );
    if ( onclick.isFunction() || hasOnEvents( EClick ) ) {
        PHIAScriptEvent *se=getEvent( EClick, true );
        se->setMouseEvent( e );
        QScriptValue event=engine->toScriptValue( se );
        if ( hasOnEvents( EClick ) ) callOnEvents( EClick, QScriptValueList() << event );
        if ( onclick.isFunction() ) {
            QScriptValue res=onclick.call( item, QScriptValueList() << event );
            if ( res.isError() ) view()->throwJavaScriptError( res );
            //if ( isSubmit && res.toBool()  ) return emit submitForm(); // return val checked later
            if ( se->defaultCanceled() ) return;
            if ( isSubmit ) emit submitForm( this->name() ); //submit button name
            if ( res.isValid() && !res.toBool() ) return;
            if ( isReset ) emit resetForm();
            if ( isButton && !url().isEmpty() ) slotLinkActivated( url() );
            if ( isRollover && !url().isEmpty() ) slotLinkActivated( url() );
            return;
        }
    }
    if ( isSubmit ) emit submitForm( this->name() );
    if ( isReset ) emit resetForm();
    if ( isButton && !url().isEmpty() ) slotLinkActivated( url() );
    if ( isRollover && !url().isEmpty() ) slotLinkActivated( url() );
}

void PHIAItem::checkForDragInMousePressEvent( QGraphicsSceneMouseEvent *e ) const
{
    qDebug( "PHIAItem::checkForDragInMousePressEvent();" );
    if ( !( e->buttons() & Qt::LeftButton ) ) return e->ignore();
    if ( !draggable() ) return e->ignore();
    PHIAItem *that=const_cast<PHIAItem*>(this); //not a nice hack but _variants are safe to use
    that->setData( DDragStartPos, e->scenePos() );
    that->setData( DDragOriginalPos, _git->pos() );
    e->accept();
}

void PHIAItem::checkForDragInMouseMoveEvent( QGraphicsSceneMouseEvent *e ) const
{
    qDebug( "PHIAItem::checkForDragInMouseMoveEvent();" );
    if ( !( e->buttons() & Qt::LeftButton ) ) return e->ignore();
    if ( !draggable() ) return e->ignore();
    if ( ( e->pos() - data( DDragStartPos ).toPointF() ).manhattanLength() < dragDistance() ) return e->ignore();
    if ( dragMoveAction() ) _git->hide();

    QImage img( width(), height(), QImage::Format_ARGB32_Premultiplied );
    img.fill( Qt::transparent );
    QPainter p( &img );
    p.setRenderHint( QPainter::Antialiasing );
    p.setRenderHint( QPainter::SmoothPixmapTransform );
    p.setOpacity( dragOpacity() );
    QStyleOptionGraphicsItem opt;
    QGraphicsProxyWidget *proxy=qgraphicsitem_cast<QGraphicsProxyWidget*>(_git);
    _git->paint( &p, &opt, 0 );
    if ( proxy && !PHI::isLayoutContainer( static_cast<PHI::Widget>(this->wid()) ) ) proxy->widget()->render( &p );
    p.end();

    QPixmap pix=QPixmap::fromImage( img, Qt::ColorOnly );
    QDrag *drag=new QDrag( view() );
    drag->setPixmap( pix );
    switch ( dragHotSpotType() ) {
    case 0: drag->setHotSpot( dragHotSpot() ); break;
    default: drag->setHotSpot( QPoint( width()/2, height()/2 ) );
    }
    QMimeData *mimeData=new QMimeData();
    mimeData->setImageData( img );
    //mimeData->setText( name() );

    QByteArray output;
    QBuffer outputBuffer( &output );
    outputBuffer.open( QIODevice::WriteOnly );
    QDataStream ds( &outputBuffer );
    ds.setVersion( PHI_DSV );
    ds << static_cast<quint8>(1) << name() << data( DDragOriginalPos ).toPointF()
       << data( DDragDropOptions, 0 ).value<qint32>();
    mimeData->setData( "application/x-phi-dd", output );
    e->accept();
    drag->setMimeData( mimeData );
    Qt::DropAction action=drag->exec( dragMoveAction() ? Qt::MoveAction : Qt::CopyAction );
    qDebug( "Drop action: %d", static_cast<int>(action) );
    if ( action==Qt::CopyAction ) return;
    if ( !dragMoveAction() ) return;
    QPointF startPos=data( DDragStartPos ).toPointF();
    QPointF origPos=data( DDragOriginalPos ).toPointF();
    startPos-=drag->hotSpot();
    qDebug() << startPos;
    PHIAItem *that=const_cast<PHIAItem*>(this);
    that->setVisible( true );
    if ( action==Qt::IgnoreAction ) {
        if ( dragRevertOnIgnore() ) {
            that->setMoveTo( 0, 500, origPos.x(), origPos.y(), PHI::defaultEasingCurve() );
            that->setPos( startPos );
        }
    } else {
        if ( dragRevertOnAccept() ) {
            qreal curOpac=opacity();
            that->setOpacity( 0 );
            that->setFadeIn( 0, 1000, curOpac, PHI::defaultEasingCurve() );
        } else {
            that->setPos( startPos );
        }
    }
}

void PHIAItem::checkDragEnterEvent( QGraphicsSceneDragDropEvent *e ) const
{
    if ( !droppable() ) return e->ignore();
    if ( !e->mimeData()->hasFormat( "application/x-phi-dd" ) ) return e->ignore();

    QByteArray input=e->mimeData()->data( "application/x-phi-dd" );
    QBuffer inputBuffer( &input );
    inputBuffer.open( QIODevice::ReadOnly );
    QDataStream ds( &inputBuffer );
    ds.setVersion( PHI_DSV );
    quint8 version;
    QString id;
    QStringList accepted=dropAcceptedIds();
    ds >> version >> id;
    if ( accepted.isEmpty() || accepted.contains( id ) ) return e->accept();
    e->ignore();
}

void PHIAItem::checkDragMoveEvent( QGraphicsSceneDragDropEvent *e ) const
{
    e->acceptProposedAction();
}

void PHIAItem::checkDropEvent( QGraphicsSceneDragDropEvent *e ) const
{
    qDebug( "Item droped %s", id().data() );
    if ( !droppable() ) return;
    if ( !e->mimeData()->hasFormat( "application/x-phi-dd" ) ) return;
    QScriptEngine *engine=view()->scriptEngine();
    if ( !engine ) return;
    QByteArray input=e->mimeData()->data( "application/x-phi-dd" );
    QBuffer inputBuffer( &input );
    inputBuffer.open( QIODevice::ReadOnly );
    QDataStream ds( &inputBuffer );
    ds.setVersion( PHI_DSV );
    quint8 version;
    QString id;
    ds >> version >> id;
    PHIAItem *that=const_cast<PHIAItem*>(this);
    QScriptValue item=baseItemToScriptValue( engine, that );
    QScriptValue ondrop=item.property( "ondrop" );
    if ( ondrop.isFunction() || hasOnEvents( EDrop ) ) {
        PHIAScriptEvent *se=getEvent( EDrop, true );
        se->setDropEvent( e );
        QScriptValue event=eventToScriptValue( engine, se );
        QScriptValue ui=engine->newObject();
        QScriptValue offset=engine->newObject();
        QScriptValue position=engine->newObject();
        QPoint off=se->mapFromScene( e->scenePos() );
        QPoint pos=e->scenePos().toPoint();
        offset.setProperty( "left", off.x() );
        offset.setProperty( "top", off.y() );
        position.setProperty( "left", pos.x() );
        position.setProperty( "top", pos.y() );
        ui.setProperty( "source", id );
        ui.setProperty( "target", name() );
        ui.setProperty( "position", position );
        ui.setProperty( "offset", offset );
        if ( hasOnEvents( EDrop ) ) callOnEvents( EDrop, QScriptValueList() << event << ui );
        if ( ondrop.isFunction() ) {
            QScriptValue res=ondrop.call( item, QScriptValueList() << event << ui );
            if ( res.isError() ) view()->throwJavaScriptError( res );
        }
    }
}
*/
