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
#include <QtGui>
#include <QScriptEngine>
#include <QScriptValue>
#include <QTimer>
#include <QMimeData>
#include <QDrag>
#include "phiaitem.h"
#include "phibaseitem.h"
#include "phiawebview.h"
#include "gitems.h"
#include "gwidgetitems.h"
#include "glayoutitems.h"
#include "phiascriptevent.h"

static bool phiismetakey( int k )
{
    switch( k ) {
    case Qt::Key_Shift:
    case Qt::Key_Tab:
    case Qt::Key_Alt:
    case Qt::Key_CapsLock:
    case Qt::Key_Control:
    case Qt::Key_Meta:
        return true;
    default:;
    }
    return false;
}

static QString phieventtostring( PHIAItem::Event e )
{
    QString type;
    switch( e ) {
    case PHIAItem::EMouseMove: type="mousemove"; break;
    case PHIAItem::EChange: type="change"; break;
    case PHIAItem::EClick: type="click"; break;
    case PHIAItem::EDblClick: type="dblclick"; break;
    case PHIAItem::EMouseOver: type="mouseover"; break;
    case PHIAItem::EMouseUp: type="mouseup"; break;
    case PHIAItem::EMouseDown: type="mousedown"; break;
    case PHIAItem::EMouseOut: type="mouseout"; break;
    case PHIAItem::EKeyPress: type="Keypress"; break;
    case PHIAItem::EKeyDown: type="keydown"; break;
    case PHIAItem::EKeyUp: type="keyup"; break;
    case PHIAItem::EDrop: type="drop"; break;
    case PHIAItem::EFocus: type="focus"; break;
    case PHIAItem::EBlur: type="blur"; break;
    case PHIAItem::ECustom: type="custom"; break;
    default: type="unknown";
    }
    return type;
}

PHIAScriptEvent* PHIAItem::getEvent( PHIAItem::Event e, bool cancelable ) const
{
    PHIAScriptEvent *se=_scriptEvents.value( e, 0 );
    if ( se ) delete se; // delete previous event
    se=new PHIAScriptEvent( view(), phieventtostring( e ), this, cancelable );
    _scriptEvents.insert( static_cast<quint8>(e), se );
    return se;
}

void PHIAItem::setOnHandler( const QString &e, const QScriptValue &fn )
{
    qDebug() << "setting on " << e << fn.toString();
    _eventFunctions.insert( e, fn );
}

void PHIAItem::setOffHandler( const QString &e, const QScriptValue &fn )
{
    QMultiHash <QString,QScriptValue>::iterator i=_eventFunctions.begin();
    while ( i!=_eventFunctions.end() ) {
        if ( i.key()!=e ) {
            i++; continue;
        }
        if ( !fn.isValid() ) {
            qDebug() << "setting off " << i.key() << i.value().toString();
            i=_eventFunctions.erase( i );
            continue;
        } else if ( fn.toString()==i.value().toString() ) {
            qDebug() << "setting off " << i.key() << i.value().toString();
            i=_eventFunctions.erase( i );
            continue;
        }
        i++;
    }
}

bool PHIAItem::hasOnEvents( PHIAItem::Event e ) const
{
    QScriptValueList functions=_eventFunctions.values( phieventtostring( e ) );
    if ( functions.count() ) return true;
    return false;
}

void PHIAItem::callOnEvents( PHIAItem::Event e, const QScriptValueList &args ) const
{
    // first parameter of args is always type event
    QScriptValue item=baseItemToScriptValue( view()->scriptEngine(), const_cast<PHIAItem*>(this) );
    QScriptValue fn, res;
    qDebug() << "callOnEvents " << _eventFunctions.count() << phieventtostring( e );
    QScriptValueList functions=_eventFunctions.values( phieventtostring( e ) );
    foreach ( fn, functions ) {
        if ( fn.isFunction() ) res=fn.call( item, args );
        else if ( fn.isString() ) res=view()->scriptEngine()->evaluate( fn.toString() );
        if ( res.isError() ) view()->throwJavaScriptError( res );
    }
}

void PHIAItem::triggerEvent( const QString &e, const QScriptValueList &args )
{
    if ( e=="drop" ) return;
    QScriptValue item=baseItemToScriptValue( view()->scriptEngine(), this );
    QScriptValue fn, res;
    qDebug() << "triggerEvent " << _eventFunctions.count() << e;
    QScriptValueList functions=_eventFunctions.values( e );
    if ( functions.count()==0 ) return;
    PHIAScriptEvent *se=new PHIAScriptEvent( view(), e, this, false );
    QScriptValue event=view()->scriptEngine()->toScriptValue( se );
    foreach ( fn, functions ) {
        if ( fn.isFunction() ) {
            res=fn.call( item, QScriptValueList() << event << args );
        } else if ( fn.isString() ) res=view()->scriptEngine()->evaluate( fn.toString() );
        if ( res.isError() ) view()->throwJavaScriptError( res );
    }
}

void PHIAItem::slotOnClick()
{
    qDebug( "Item onclick %s activated", id().data() );
    slotOnClick( 0 );
}

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

void PHIAItem::slotOnDblClick( QGraphicsSceneMouseEvent *e )
{
    qDebug( "Item ondblclick %s", id().data() );
    QScriptEngine *engine=view()->scriptEngine(); 
    if ( !engine ) return;
    QScriptValue item=baseItemToScriptValue( engine, this );
    QScriptValue ondblclick=item.property( "ondblclick" );
    if ( ondblclick.isFunction() || hasOnEvents( EDblClick ) ) {
        PHIAScriptEvent *se=getEvent( EDblClick, true );
        se->setMouseEvent( e );
        QScriptValue event=engine->toScriptValue( se );
        if ( hasOnEvents( EDblClick ) ) callOnEvents( EDblClick, QScriptValueList() << event );
        if ( ondblclick.isFunction() ) {
            QScriptValue res=ondblclick.call( item, QScriptValueList() << event );
            if ( res.isError() ) view()->throwJavaScriptError( res );
        }
    }
}

void PHIAItem::slotOnFocus( QFocusEvent *e )
{
    //qDebug( "Item onfocus %s", id().data() );
    QScriptEngine *engine=view()->scriptEngine();
    if ( !engine ) return;
    QScriptValue item=baseItemToScriptValue( engine, this );
    QScriptValue onfocus=item.property( "onfocus" );
    if ( onfocus.isFunction() || hasOnEvents( EFocus ) ) {
        PHIAScriptEvent *se=getEvent( EFocus );
        se->setFocusEvent( e );
        QScriptValue event=engine->toScriptValue( se );
        if ( hasOnEvents( EFocus ) ) callOnEvents( EFocus, QScriptValueList() << event );
        if ( onfocus.isFunction() ) {
            QScriptValue res=onfocus.call( item, QScriptValueList() << event );
            if ( res.isError() ) view()->throwJavaScriptError( res );
        }
    }
}

void PHIAItem::slotOnBlur( QFocusEvent *e )
{
    //qDebug( "Item onblur %s", id().data() );
    QScriptEngine *engine=view()->scriptEngine();
    if ( !engine ) return;
    QScriptValue item=baseItemToScriptValue( engine, this );
    QScriptValue onblur=item.property( "onblur" );
    if ( onblur.isFunction() || hasOnEvents( EBlur ) ) {
        PHIAScriptEvent *se=getEvent( EBlur );
        se->setFocusEvent( e );
        QScriptValue event=engine->toScriptValue( se );
        if ( hasOnEvents( EBlur ) ) callOnEvents( EBlur, QScriptValueList() << event );
        if ( onblur.isFunction() ) {
            QScriptValue res=onblur.call( item, QScriptValueList() << event );
            if ( res.isError() ) view()->throwJavaScriptError( res );
        }
    }
}

void PHIAItem::slotOnMouseDown( QGraphicsSceneMouseEvent *e )
{
    qDebug( "Item onmousedown %s", id().data() );
    QScriptEngine *engine=view()->scriptEngine();
    if ( !engine ) return;
    QScriptValue item=baseItemToScriptValue( engine, this );
    QScriptValue onmousedown=item.property( "onmousedown" );
    if ( onmousedown.isFunction() || hasOnEvents( EMouseDown ) ) {
        PHIAScriptEvent *se=getEvent( EMouseDown, true );
        se->setMouseEvent( e );
        QScriptValue event=engine->toScriptValue( se );
        if ( hasOnEvents( EMouseDown) ) callOnEvents( EMouseDown, QScriptValueList() << event );
        if ( onmousedown.isFunction() ) {
            QScriptValue res=onmousedown.call( item, QScriptValueList() << event );
            if ( res.isError() ) view()->throwJavaScriptError( res );
        }
    }
}

void PHIAItem::slotOnMouseUp( QGraphicsSceneMouseEvent *e )
{
    qDebug( "Item onmouseup %s", id().data() );
    QScriptEngine *engine=view()->scriptEngine();
    if ( !engine ) return;
    QScriptValue item=baseItemToScriptValue( engine, this );
    QScriptValue onmouseup=item.property( "onmouseup" );
    if ( onmouseup.isFunction() || hasOnEvents( EMouseUp ) ) {
        PHIAScriptEvent *se=getEvent( EMouseUp, true );
        se->setMouseEvent( e );
        QScriptValue event=engine->toScriptValue( se );
        if ( hasOnEvents( EMouseUp ) ) callOnEvents( EMouseUp, QScriptValueList() << event );
        if ( onmouseup.isFunction() ) {
            QScriptValue res=onmouseup.call( item, QScriptValueList() << event );
            if ( res.isError() ) view()->throwJavaScriptError( res );
        }
    }
}

void PHIAItem::slotOnMouseOver( QGraphicsSceneHoverEvent *e )
{
    qDebug( "Item onmouseover %s", id().data() );
    QScriptEngine *engine=view()->scriptEngine();
    if ( !engine ) return;
    QScriptValue item=baseItemToScriptValue( engine, this );
    QScriptValue onmouseover=item.property( "onmouseover" );
    if ( onmouseover.isFunction() || hasOnEvents( EMouseOver ) ) {
        PHIAScriptEvent *se=getEvent( EMouseOver, true );
        se->setHoverEvent( e );
        QScriptValue event=engine->toScriptValue( se );
        if ( hasOnEvents( EMouseOver ) ) callOnEvents( EMouseOver, QScriptValueList() << event );
        if ( onmouseover.isFunction() ) {
            QScriptValue res=onmouseover.call( item, QScriptValueList() << event );
            if ( res.isError() ) view()->throwJavaScriptError( res );
        }
    }
    if ( wid()==PHI::BUTTON || wid()==PHI::IMAGE_BUTTON || wid()==PHI::ROLLOVER_BUTTON
            || wid()==PHI::PHISYS_LINK || wid()==PHI::LINK ) {
        if ( !url().isEmpty() ) slotLinkHovered( url() );
    }
}

void PHIAItem::slotOnMouseMove( QGraphicsSceneHoverEvent *e )
{
    //qDebug( "Item onmousemove %s", id().data() );
    QScriptEngine *engine=view()->scriptEngine();
    if ( !engine ) return;
    QScriptValue item=baseItemToScriptValue( engine, this );
    QScriptValue onmousemove=item.property( "onmousemove" );
    if ( onmousemove.isFunction() || hasOnEvents( EMouseMove ) ) {
        PHIAScriptEvent *se=getEvent( EMouseMove );
        se->setHoverEvent( e );
        QScriptValue event=engine->toScriptValue( se );
        if ( hasOnEvents( EMouseMove ) ) callOnEvents( EMouseMove, QScriptValueList() << event );
        if ( onmousemove.isFunction() ) {
            QScriptValue res=onmousemove.call( item, QScriptValueList() << event );
            if ( res.isError() ) view()->throwJavaScriptError( res );
        }
    }
}

void PHIAItem::slotOnMouseOut( QGraphicsSceneHoverEvent *e )
{
    qDebug( "Item onmouseout %s", id().data() );
    QScriptEngine *engine=view()->scriptEngine();
    if ( !engine ) return;
    QScriptValue item=baseItemToScriptValue( engine, this );
    QScriptValue onmouseout=item.property( "onmouseout" );
    if ( onmouseout.isFunction() || hasOnEvents( EMouseOut ) ) {
        PHIAScriptEvent *se=getEvent( EMouseOut, true );
        se->setHoverEvent( e );
        QScriptValue event=engine->toScriptValue( se );
        if ( hasOnEvents( EMouseOut ) ) callOnEvents( EMouseOut, QScriptValueList() << event );
        if ( onmouseout.isFunction() ) {
            QScriptValue res=onmouseout.call( item, QScriptValueList() << event );
            if ( res.isError() ) view()->throwJavaScriptError( res );
        }
    }
    if ( wid()==PHI::BUTTON || wid()==PHI::IMAGE_BUTTON || wid()==PHI::ROLLOVER_BUTTON
        || wid()==PHI::PHISYS_LINK || wid()==PHI::LINK ) {
        if ( !url().isEmpty() ) slotLinkHovered( QString() );
    }
}

void PHIAItem::slotOnKeyPress( QKeyEvent *e )
{
    if ( phiismetakey( e->key() ) ) return;
    qDebug( "Item onkeypress %s", id().data() );
    QScriptEngine *engine=view()->scriptEngine();
    if ( !engine ) return;
    QScriptValue item=baseItemToScriptValue( engine, this );
    QScriptValue onkeypress=item.property( "onkeypress" );
    if ( onkeypress.isFunction() || hasOnEvents( EKeyPress ) ) {
        PHIAScriptEvent *se=getEvent( EKeyPress, true );
        se->setKeyEvent( e );
        QScriptValue event=engine->toScriptValue( se );
        if ( hasOnEvents( EKeyPress ) ) callOnEvents( EKeyPress, QScriptValueList() << event );
        if ( onkeypress.isFunction() ) {
            QScriptValue res=onkeypress.call( item, QScriptValueList() << event );
            if ( res.isError() ) view()->throwJavaScriptError( res );
        }
    }
}

void PHIAItem::slotOnKeyDown( QKeyEvent *e )
{
    if ( phiismetakey( e->key() ) ) return;
    qDebug( "Item onkeydown %s", id().data() );
    QScriptEngine *engine=view()->scriptEngine();
    if ( !engine ) return;
    QScriptValue item=baseItemToScriptValue( engine, this );
    QScriptValue onkeydown=item.property( "onkeydown" );
    if ( onkeydown.isFunction() || hasOnEvents( EKeyDown ) ) {
        PHIAScriptEvent *se=getEvent( EKeyDown, true );
        se->setKeyEvent( e );
        QScriptValue event=engine->toScriptValue( se );
        if ( hasOnEvents( EKeyDown ) ) callOnEvents( EKeyDown, QScriptValueList() << event );
        if ( onkeydown.isFunction() ) {
            QScriptValue res=onkeydown.call( item, QScriptValueList() << event );
            if ( res.isError() ) view()->throwJavaScriptError( res );
        }
    }
}

void PHIAItem::slotOnKeyUp( QKeyEvent *e )
{
    if ( phiismetakey( e->key() ) ) return;
    qDebug( "Item onkeyup %s", id().data() );
    QScriptEngine *engine=view()->scriptEngine();
    if ( !engine ) return;
    QScriptValue item=baseItemToScriptValue( engine, this );
    QScriptValue onkeyup=item.property( "onkeyup" );
    if ( onkeyup.isFunction() || hasOnEvents( EKeyUp ) ) {
        PHIAScriptEvent *se=getEvent( EKeyUp, true );
        se->setKeyEvent( e );
        QScriptValue event=engine->toScriptValue( se );
        if ( hasOnEvents( EKeyUp ) ) callOnEvents( EKeyUp, QScriptValueList() << event );
        if ( onkeyup.isFunction() ) {
            QScriptValue res=onkeyup.call( item, QScriptValueList() << event );
            if ( res.isError() ) view()->throwJavaScriptError( res );
        }
    }
}

void PHIAItem::slotOnChange()
{
    qDebug( "Item onchange %s", id().data() );
    if ( !PHI::isWidgetItem( wid() ) ) return;
    qDebug( "Item onchange handler activated %s", id().data() );
    QScriptEngine *engine=view()->scriptEngine();
    if ( !engine ) return;
    QScriptValue item=baseItemToScriptValue( engine, this );
    QScriptValue onchange=item.property( "onchange" );
    if ( onchange.isFunction() || hasOnEvents( EChange ) ) {
        PHIAScriptEvent *se=getEvent( EChange, true );
        QScriptValue event=engine->toScriptValue( se );
        if ( hasOnEvents( EChange ) ) callOnEvents( EChange, QScriptValueList() << event );
        if ( onchange.isFunction() ) {
            QScriptValue res=onchange.call( item, QScriptValueList() << event );
            if ( res.isError() ) view()->throwJavaScriptError( res );
        }
    }
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
    if ( proxy && !PHI::isLayoutContainer( this->wid() ) ) proxy->widget()->render( &p );
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
