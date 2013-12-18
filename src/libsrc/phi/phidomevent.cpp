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
#include <QDateTime>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QKeyEvent>
#include <QScriptEngine>
#include "phidomevent.h"
#include "phibaseitem.h"
#include "phidomitem.h"

#define PHIDOMSCRIPTOPTIONS QScriptEngine::PreferExistingWrapperObject | QScriptEngine::SkipMethodsInEnumeration |\
    QScriptEngine::ExcludeSuperClassContents | QScriptEngine::ExcludeDeleteLater

struct PHIFakeDomEvent
{
     enum KeyCodes  {
     DOM_UNDEFINED            = 0x0,
     DOM_RIGHT_ALT            = 0x12,
     DOM_LEFT_ALT             = 0x12,
     DOM_LEFT_CONTROL         = 0x11,
     DOM_RIGHT_CONTROL        = 0x11,
     DOM_LEFT_SHIFT           = 0x10,
     DOM_RIGHT_SHIFT          = 0x10,
     DOM_META                 = 0x9D,
     DOM_BACK_SPACE           = 0x08,
     DOM_CAPS_LOCK            = 0x14,
     DOM_INSERT               = 0x2D,
     DOM_DELETE               = 0x2E,
     DOM_END                  = 0x23,
     DOM_ENTER                = 0x0D,
     DOM_ESCAPE               = 0x1B,
     DOM_HOME                 = 0x24,
     DOM_NUM_LOCK             = 0x90,
     DOM_PAUSE                = 0x13,
     DOM_PRINTSCREEN          = 0x9A,
     DOM_SCROLL_LOCK          = 0x91,
     DOM_SPACE                = 0x20,
     DOM_TAB                  = 0x09,
     DOM_LEFT                 = 0x25,
     DOM_RIGHT                = 0x27,
     DOM_UP                   = 0x26,
     DOM_DOWN                 = 0x28,
     DOM_PAGE_DOWN            = 0x22,
     DOM_PAGE_UP              = 0x21,
     DOM_F1                   = 0x70,
     DOM_F2                   = 0x71,
     DOM_F3                   = 0x72,
     DOM_F4                   = 0x73,
     DOM_F5                   = 0x74,
     DOM_F6                   = 0x75,
     DOM_F7                   = 0x76,
     DOM_F8                   = 0x77,
     DOM_F9                   = 0x78,
     DOM_F10                  = 0x79,
     DOM_F11                  = 0x7A,
     DOM_F12                  = 0x7B,
     DOM_F13                  = 0xF000,
     DOM_F14                  = 0xF001,
     DOM_F15                  = 0xF002,
     DOM_F16                  = 0xF003,
     DOM_F17                  = 0xF004,
     DOM_F18                  = 0xF005,
     DOM_F19                  = 0xF006,
     DOM_F20                  = 0xF007,
     DOM_F21                  = 0xF008,
     DOM_F22                  = 0xF009,
     DOM_F23                  = 0xF00A,
     DOM_F24                  = 0xF00B
     };
     static int qtToDomKey( int keyCode );
};

QScriptValue eventToScriptValue( QScriptEngine *engine, PHIDomEvent* const &obj )
{
    return engine->newQObject( obj, QScriptEngine::QtOwnership, PHIDOMSCRIPTOPTIONS );
}

void eventFromScriptValue( const QScriptValue &val, PHIDomEvent* &obj )
{
    obj=qobject_cast<PHIDomEvent*>(val.toQObject());
}

PHIDomEvent::PHIDomEvent( const QString &type, PHIBaseItem *item, bool cancelable )
    : QObject( item->scriptEngine() ), _item( item ), _cancelDefault( false ),
    _stopPropagation( false ), _cancelable( cancelable )
{
    qDebug( "PHIDomEvent::PHIDomEvent()" );
    _timestamp=QDateTime::currentMSecsSinceEpoch();
    PHIDomItem *domItem=new PHIDomItem( item, this );
    QScriptValue target=item->scriptEngine()->newQObject( domItem, QScriptEngine::QtOwnership, PHIDOMSCRIPTOPTIONS );
    _self=eventToScriptValue( _item->scriptEngine(), this );
    _self.setProperty( L1( "target" ), target, QScriptValue::ReadOnly );
    _self.setProperty( L1( "currentTarget" ), target, QScriptValue::ReadOnly );
    _self.setProperty( L1( "type" ), type, QScriptValue::ReadOnly );
    _self.setProperty( L1( "bubbles" ), true, QScriptValue::ReadOnly );
    _self.setProperty( L1( "cancelable" ), cancelable, QScriptValue::ReadOnly );
    _self.setProperty( L1( "eventPhase" ), 1, QScriptValue::ReadOnly );
    _self.setProperty( L1( "view" ), item->scriptEngine()->globalObject(), QScriptValue::ReadOnly );
    _self.setProperty( L1( "detail" ), 0, QScriptValue::ReadOnly );
}

PHIDomEvent::~PHIDomEvent()
{
    qDebug( "PHIDomEvent::~PHIDomEvent()" );
}

bool PHIDomEvent::isMetaKey( int k )
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

QString PHIDomEvent::eventTypeToString( Event e )
{
    QString type;
    switch( e ) {
    case EMouseMove: type=SL( "mousemove" ); break;
    case EChange: type=SL( "change" ); break;
    case EClick: type=SL( "click" ); break;
    case EDblClick: type=SL( "dblclick" ); break;
    case EMouseOver: type=SL( "mouseover" ); break;
    case EMouseUp: type=SL( "mouseup" ); break;
    case EMouseDown: type=SL( "mousedown" ); break;
    case EMouseOut: type=SL( "mouseout" ); break;
    case EKeyPress: type=SL( "keypress" ); break;
    case EKeyDown: type=SL( "keydown" ); break;
    case EKeyUp: type=SL( "keyup" ); break;
    case EDrop: type=SL( "drop" ); break;
    case EFocus: type=SL( "focus" ); break;
    case EBlur: type=SL( "blur" ); break;
    case ECustom: type=SL( "custom" ); break;
    default: type=SL( "unknown" );
    }
    return type;
}

QPoint PHIDomEvent::mapFromScene( const QPointF &p ) const
{
    const QGraphicsWidget *gw=_item->graphicsWidget();
    Q_ASSERT( gw->scene()->views().first() );
    return gw->scene()->views().first()->mapFromScene( p );
}

void PHIDomEvent::setMouseEvent( QGraphicsSceneMouseEvent *e )
{
    Q_ASSERT( e );
    QPoint v=mapFromScene( e->scenePos() );
    QPoint doc=e->scenePos().toPoint();
    _self.setProperty( L1( "pageX" ), doc.x(), QScriptValue::ReadOnly );
    _self.setProperty( L1( "pageY" ), doc.y(), QScriptValue::ReadOnly );
    _self.setProperty( L1( "clientX" ), v.x(), QScriptValue::ReadOnly );
    _self.setProperty( L1( "clientY" ), v.y(), QScriptValue::ReadOnly );
    _self.setProperty( L1( "screenX" ), e->screenPos().x(), QScriptValue::ReadOnly );
    _self.setProperty( L1( "screenY" ), e->screenPos().y(), QScriptValue::ReadOnly );

    int button=0, which=0;
    if ( e->button()==Qt::LeftButton ) which=1;
    if ( e->button()==Qt::MidButton ) { button=1; which=2; }
    if ( e->button()==Qt::RightButton ) { button=2; which=3; }
    _self.setProperty( L1( "which" ), which, QScriptValue::ReadOnly );
    _self.setProperty( L1( "button" ), button, QScriptValue::ReadOnly );

    _self.setProperty( L1( "altKey" ), (e->modifiers() & Qt::AltModifier ? true : false), QScriptValue::ReadOnly );
    _self.setProperty( L1( "ctrlKey" ), (e->modifiers() & Qt::ControlModifier ? true : false), QScriptValue::ReadOnly );
    _self.setProperty( L1( "shiftKey" ), (e->modifiers() & Qt::ShiftModifier ? true : false), QScriptValue::ReadOnly );
    _self.setProperty( L1( "metaKey" ), (e->modifiers() & Qt::MetaModifier ? true : false), QScriptValue::ReadOnly );
}

void PHIDomEvent::setHoverEvent( QGraphicsSceneHoverEvent *e )
{
    Q_ASSERT( e );
    QPoint v=mapFromScene( e->scenePos() );
    QPoint doc=e->scenePos().toPoint();
    _self.setProperty( L1( "pageX" ), doc.x(), QScriptValue::ReadOnly );
    _self.setProperty( L1( "pageY" ), doc.y(), QScriptValue::ReadOnly );
    _self.setProperty( L1( "clientX" ), v.x(), QScriptValue::ReadOnly );
    _self.setProperty( L1( "clientY" ), v.y(), QScriptValue::ReadOnly );
    _self.setProperty( L1( "screenX" ), e->screenPos().x(), QScriptValue::ReadOnly );
    _self.setProperty( L1( "screenY" ), e->screenPos().y(), QScriptValue::ReadOnly );

    _self.setProperty( L1( "which" ), 0, QScriptValue::ReadOnly );

    _self.setProperty( L1( "altKey" ), (e->modifiers() & Qt::AltModifier ? true : false), QScriptValue::ReadOnly );
    _self.setProperty( L1( "ctrlKey" ), (e->modifiers() & Qt::ControlModifier ? true : false), QScriptValue::ReadOnly );
    _self.setProperty( L1( "shiftKey" ), (e->modifiers() & Qt::ShiftModifier ? true : false), QScriptValue::ReadOnly );
    _self.setProperty( L1( "metaKey" ), (e->modifiers() & Qt::MetaModifier ? true : false), QScriptValue::ReadOnly );
}

void PHIDomEvent::setKeyEvent( QKeyEvent *e )
{
    Q_ASSERT( e );
    _self.setProperty( L1( "altKey" ), (e->modifiers() & Qt::AltModifier ? true : false), QScriptValue::ReadOnly );
    _self.setProperty( L1( "ctrlKey" ), (e->modifiers() & Qt::ControlModifier ? true : false), QScriptValue::ReadOnly );
    _self.setProperty( L1( "shiftKey" ), (e->modifiers() & Qt::ShiftModifier ? true : false), QScriptValue::ReadOnly );
    _self.setProperty( L1( "metaKey" ), (e->modifiers() & Qt::MetaModifier ? true : false), QScriptValue::ReadOnly );

    int code=PHIFakeDomEvent::qtToDomKey( e->key() );
    if ( !e->text().isEmpty() ) {
        QChar c;
        c=e->text().at( 0 );
        code=static_cast<int>(c.unicode());
    }
    _self.setProperty( L1( "isChar" ), !e->text().isEmpty() );
    _self.setProperty( L1( "charCode" ), e->text().isEmpty() ? L1( "0" ) : e->text() );
    _self.setProperty( L1( "keyCode" ), code );
    _self.setProperty( L1( "which" ), code );
}

void PHIDomEvent::setDropEvent( QGraphicsSceneDragDropEvent *e )
{
    Q_ASSERT( e );
    /* DOM does not specify mouse positions in a drop event. :-(
    QPoint v=mapFromScene( e->scenePos() );
    QPoint doc=e->scenePos().toPoint();
    self.setProperty( "pageX" ), doc.x(), QScriptValue::ReadOnly );
    self.setProperty( "pageY" ), doc.y(), QScriptValue::ReadOnly );
    self.setProperty( "clientX" ), v.x(), QScriptValue::ReadOnly );
    self.setProperty( "clientY" ), v.y(), QScriptValue::ReadOnly );
    self.setProperty( "screenX" ), e->screenPos().x(), QScriptValue::ReadOnly );
    self.setProperty( "screenY" ), e->screenPos().y(), QScriptValue::ReadOnly );
    */

    int button=0, which=0;
    if ( e->buttons() & Qt::LeftButton ) which=1;
    else if ( e->buttons() & Qt::RightButton ) { button=2; which=3; }
    else if ( e->buttons() & Qt::MidButton ) { button=1; which=2; }
    _self.setProperty( L1( "which" ), which, QScriptValue::ReadOnly );
    _self.setProperty( L1( "button" ), button, QScriptValue::ReadOnly );

    _self.setProperty( L1( "altKey" ), (e->modifiers() & Qt::AltModifier ? true : false), QScriptValue::ReadOnly );
    _self.setProperty( L1( "ctrlKey" ), (e->modifiers() & Qt::ControlModifier ? true : false), QScriptValue::ReadOnly );
    _self.setProperty( L1( "shiftKey" ), (e->modifiers() & Qt::ShiftModifier ? true : false), QScriptValue::ReadOnly );
    _self.setProperty( L1( "metaKey" ), (e->modifiers() & Qt::MetaModifier ? true : false), QScriptValue::ReadOnly );
}

void PHIDomEvent::setFocusEvent( QFocusEvent *e )
{
    Q_UNUSED( e );
}

int PHIFakeDomEvent::qtToDomKey( int keyCode )
{
     switch (keyCode) {
     case Qt::Key_Backspace:
         return  DOM_BACK_SPACE;
     case Qt::Key_Enter:
         return  DOM_ENTER;
     case Qt::Key_Return:
         return  DOM_ENTER;
     case Qt::Key_NumLock:
         return  DOM_NUM_LOCK;
     case Qt::Key_Alt:
         return  DOM_RIGHT_ALT;
     case Qt::Key_Control:
         return  DOM_LEFT_CONTROL;
     case Qt::Key_Shift:
         return  DOM_LEFT_SHIFT;
     case Qt::Key_Meta:
         return  DOM_META;
     case Qt::Key_CapsLock:
         return  DOM_CAPS_LOCK;
     case Qt::Key_Delete:
         return  DOM_DELETE;
     case Qt::Key_Insert:
         return DOM_INSERT;
     case Qt::Key_End:
         return  DOM_END;
     case Qt::Key_Escape:
         return  DOM_ESCAPE;
     case Qt::Key_Home:
         return  DOM_HOME;
     case Qt::Key_Pause:
         return  DOM_PAUSE;
     case Qt::Key_Print:
         return  DOM_PRINTSCREEN;
     case Qt::Key_ScrollLock:
         return  DOM_SCROLL_LOCK;
     case Qt::Key_Left:
         return  DOM_LEFT;
     case Qt::Key_Right:
         return  DOM_RIGHT;
     case Qt::Key_Up:
         return  DOM_UP;
     case Qt::Key_Down:
         return  DOM_DOWN;
     case Qt::Key_PageDown:
         return  DOM_PAGE_DOWN;
     case Qt::Key_PageUp:
         return  DOM_PAGE_UP;
     case Qt::Key_F1:
         return  DOM_F1;
     case Qt::Key_F2:
         return  DOM_F2;
     case Qt::Key_F3:
         return  DOM_F3;
     case Qt::Key_F4:
         return  DOM_F4;
     case Qt::Key_F5:
         return  DOM_F5;
     case Qt::Key_F6:
         return  DOM_F6;
     case Qt::Key_F7:
         return  DOM_F7;
     case Qt::Key_F8:
         return  DOM_F8;
     case Qt::Key_F9:
         return  DOM_F9;
     case Qt::Key_F10:
         return  DOM_F10;
     case Qt::Key_F11:
         return  DOM_F11;
     case Qt::Key_F12:
         return  DOM_F12;
     case Qt::Key_F13:
         return  DOM_F13;
     case Qt::Key_F14:
         return  DOM_F14;
     case Qt::Key_F15:
         return  DOM_F15;
     case Qt::Key_F16:
         return  DOM_F16;
     case Qt::Key_F17:
         return  DOM_F17;
     case Qt::Key_F18:
         return  DOM_F18;
     case Qt::Key_F19:
         return  DOM_F19;
     case Qt::Key_F20:
         return  DOM_F20;
     case Qt::Key_F21:
         return  DOM_F21;
     case Qt::Key_F22:
         return  DOM_F22;
     case Qt::Key_F23:
         return  DOM_F23;
     case Qt::Key_F24:
        return  DOM_F24;
    }
    return keyCode;
}
