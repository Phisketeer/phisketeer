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
#include <QDateTime>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QKeyEvent>
#include "phiascriptevent.h"
#include "phibaseitem.h"

struct PHIAFakeDomEvent
{
     enum KeyCodes  {
     DOM_VK_UNDEFINED            = 0x0,
     DOM_VK_RIGHT_ALT            = 0x12,
     DOM_VK_LEFT_ALT             = 0x12,
     DOM_VK_LEFT_CONTROL         = 0x11,
     DOM_VK_RIGHT_CONTROL        = 0x11,
     DOM_VK_LEFT_SHIFT           = 0x10,
     DOM_VK_RIGHT_SHIFT          = 0x10,
     DOM_VK_META                 = 0x9D,
     DOM_VK_BACK_SPACE           = 0x08,
     DOM_VK_CAPS_LOCK            = 0x14,
     DOM_VK_INSERT               = 0x2D,
     DOM_VK_DELETE               = 0x2E,
     DOM_VK_END                  = 0x23,
     DOM_VK_ENTER                = 0x0D,
     DOM_VK_ESCAPE               = 0x1B,
     DOM_VK_HOME                 = 0x24,
     DOM_VK_NUM_LOCK             = 0x90,
     DOM_VK_PAUSE                = 0x13,
     DOM_VK_PRINTSCREEN          = 0x9A,
     DOM_VK_SCROLL_LOCK          = 0x91,
     DOM_VK_SPACE                = 0x20,
     DOM_VK_TAB                  = 0x09,
     DOM_VK_LEFT                 = 0x25,
     DOM_VK_RIGHT                = 0x27,
     DOM_VK_UP                   = 0x26,
     DOM_VK_DOWN                 = 0x28,
     DOM_VK_PAGE_DOWN            = 0x22,
     DOM_VK_PAGE_UP              = 0x21,
     DOM_VK_F1                   = 0x70,
     DOM_VK_F2                   = 0x71,
     DOM_VK_F3                   = 0x72,
     DOM_VK_F4                   = 0x73,
     DOM_VK_F5                   = 0x74,
     DOM_VK_F6                   = 0x75,
     DOM_VK_F7                   = 0x76,
     DOM_VK_F8                   = 0x77,
     DOM_VK_F9                   = 0x78,
     DOM_VK_F10                  = 0x79,
     DOM_VK_F11                  = 0x7A,
     DOM_VK_F12                  = 0x7B,
     DOM_VK_F13                  = 0xF000,
     DOM_VK_F14                  = 0xF001,
     DOM_VK_F15                  = 0xF002,
     DOM_VK_F16                  = 0xF003,
     DOM_VK_F17                  = 0xF004,
     DOM_VK_F18                  = 0xF005,
     DOM_VK_F19                  = 0xF006,
     DOM_VK_F20                  = 0xF007,
     DOM_VK_F21                  = 0xF008,
     DOM_VK_F22                  = 0xF009,
     DOM_VK_F23                  = 0xF00A,
     DOM_VK_F24                  = 0xF00B
     };
     static int qtToDomKey( int keyCode );
};

QScriptValue eventToScriptValue( QScriptEngine *engine, PHIAScriptEvent* const &obj )
{
    return engine->newQObject( obj, QScriptEngine::QtOwnership,
        QScriptEngine::PreferExistingWrapperObject | QScriptEngine::SkipMethodsInEnumeration |
        QScriptEngine::ExcludeSuperClassContents | QScriptEngine::ExcludeDeleteLater );
}

void eventFromScriptValue( const QScriptValue &val, PHIAScriptEvent* &obj )
{
    obj=qobject_cast<PHIAScriptEvent*>(val.toQObject());
}

PHIAScriptEvent::PHIAScriptEvent( PHIAWebView *view, const QString &type, const PHIBaseItem *item, bool cancelable )
    : QObject( view->scriptEngine() ), _view( view ), _cancelDefault( false ), _cancelable( cancelable )
{
    qDebug( "PHIAScriptEvent::PHIAScriptEvent()" );
    Q_ASSERT( view );
    Q_ASSERT( item );
    _timestamp=QDateTime::currentMSecsSinceEpoch();
    QScriptValue target=baseItemToScriptValue( view->scriptEngine(), const_cast<PHIBaseItem*>(item) );
    QScriptValue self=eventToScriptValue( view->scriptEngine(), this );
    //QScriptValue target=_view->scriptEngine()->toScriptValue( item );
    //QScriptValue self=_view->scriptEngine()->toScriptValue( this );
    self.setProperty( "target", target, QScriptValue::ReadOnly );
    self.setProperty( "currentTarget", target, QScriptValue::ReadOnly );
    self.setProperty( "type", type, QScriptValue::ReadOnly );
    self.setProperty( "bubbles", true, QScriptValue::ReadOnly );
    self.setProperty( "cancelable", cancelable, QScriptValue::ReadOnly );
    self.setProperty( "eventPhase", 3, QScriptValue::ReadOnly );
    self.setProperty( "view", _view->scriptEngine()->globalObject(), QScriptValue::ReadOnly );
    self.setProperty( "detail", 0, QScriptValue::ReadOnly );
}

PHIAScriptEvent::~PHIAScriptEvent()
{
    qDebug( "PHIAScriptEvent::~PHIAScriptEvent()" );
}

void PHIAScriptEvent::setMouseEvent( QGraphicsSceneMouseEvent *e )
{
    if ( !e ) return;
    QScriptValue self=_view->scriptEngine()->toScriptValue( this );
    QPoint v=mapFromScene( e->scenePos() );
    QPoint doc=e->scenePos().toPoint();
    self.setProperty( "pageX", doc.x(), QScriptValue::ReadOnly );
    self.setProperty( "pageY", doc.y(), QScriptValue::ReadOnly );
    self.setProperty( "clientX", v.x(), QScriptValue::ReadOnly );
    self.setProperty( "clientY", v.y(), QScriptValue::ReadOnly );
    self.setProperty( "screenX", e->screenPos().x(), QScriptValue::ReadOnly );
    self.setProperty( "screenY", e->screenPos().y(), QScriptValue::ReadOnly );

    int button=0, which=0;
    if ( e->button()==Qt::LeftButton ) which=1;
    if ( e->button()==Qt::MidButton ) { button=1; which=2; }
    if ( e->button()==Qt::RightButton ) { button=2; which=3; }
    self.setProperty( "which", which, QScriptValue::ReadOnly );
    self.setProperty( "button", button, QScriptValue::ReadOnly );

    self.setProperty( "altKey", (e->modifiers() & Qt::AltModifier ? true : false), QScriptValue::ReadOnly );
    self.setProperty( "ctrlKey", (e->modifiers() & Qt::ControlModifier ? true : false), QScriptValue::ReadOnly );
    self.setProperty( "shiftKey", (e->modifiers() & Qt::ShiftModifier ? true : false), QScriptValue::ReadOnly );
    self.setProperty( "metaKey", (e->modifiers() & Qt::MetaModifier ? true : false), QScriptValue::ReadOnly );
}

void PHIAScriptEvent::setHoverEvent( QGraphicsSceneHoverEvent *e )
{
    if ( !e ) return;
    QScriptValue self=_view->scriptEngine()->toScriptValue( this );
    QPoint v=mapFromScene( e->scenePos() );
    QPoint doc=e->scenePos().toPoint();
    self.setProperty( "pageX", doc.x(), QScriptValue::ReadOnly );
    self.setProperty( "pageY", doc.y(), QScriptValue::ReadOnly );
    self.setProperty( "clientX", v.x(), QScriptValue::ReadOnly );
    self.setProperty( "clientY", v.y(), QScriptValue::ReadOnly );
    self.setProperty( "screenX", e->screenPos().x(), QScriptValue::ReadOnly );
    self.setProperty( "screenY", e->screenPos().y(), QScriptValue::ReadOnly );

    self.setProperty( "which", 0, QScriptValue::ReadOnly );

    self.setProperty( "altKey", (e->modifiers() & Qt::AltModifier ? true : false), QScriptValue::ReadOnly );
    self.setProperty( "ctrlKey", (e->modifiers() & Qt::ControlModifier ? true : false), QScriptValue::ReadOnly );
    self.setProperty( "shiftKey", (e->modifiers() & Qt::ShiftModifier ? true : false), QScriptValue::ReadOnly );
    self.setProperty( "metaKey", (e->modifiers() & Qt::MetaModifier ? true : false), QScriptValue::ReadOnly );
}

void PHIAScriptEvent::setKeyEvent( QKeyEvent *e )
{
    if ( !e ) return;
    QScriptValue self=_view->scriptEngine()->toScriptValue( this );
    self.setProperty( "altKey", (e->modifiers() & Qt::AltModifier ? true : false), QScriptValue::ReadOnly );
    self.setProperty( "ctrlKey", (e->modifiers() & Qt::ControlModifier ? true : false), QScriptValue::ReadOnly );
    self.setProperty( "shiftKey", (e->modifiers() & Qt::ShiftModifier ? true : false), QScriptValue::ReadOnly );
    self.setProperty( "metaKey", (e->modifiers() & Qt::MetaModifier ? true : false), QScriptValue::ReadOnly );

    int code=PHIAFakeDomEvent::qtToDomKey( e->key() );
    if ( !e->text().isEmpty() ) {
        QChar c;
        c=e->text().at( 0 );
        code=static_cast<int>(c.unicode());
    }
    self.setProperty( "isChar", !e->text().isEmpty() );
    self.setProperty( "charCode", e->text().isEmpty()?0:e->text() );
    self.setProperty( "keyCode", code );
    self.setProperty( "which", code );
}

void PHIAScriptEvent::setDropEvent( QGraphicsSceneDragDropEvent *e )
{
    if ( !e ) return;
    QScriptValue self=_view->scriptEngine()->toScriptValue( this );
    /* DOM does not specify mouse positions in a drop event. :-(
    QPoint v=mapFromScene( e->scenePos() );
    QPoint doc=e->scenePos().toPoint();
    self.setProperty( "pageX", doc.x(), QScriptValue::ReadOnly );
    self.setProperty( "pageY", doc.y(), QScriptValue::ReadOnly );
    self.setProperty( "clientX", v.x(), QScriptValue::ReadOnly );
    self.setProperty( "clientY", v.y(), QScriptValue::ReadOnly );
    self.setProperty( "screenX", e->screenPos().x(), QScriptValue::ReadOnly );
    self.setProperty( "screenY", e->screenPos().y(), QScriptValue::ReadOnly );
    */

    int button=0, which=0;
    if ( e->buttons() & Qt::LeftButton ) which=1;
    else if ( e->buttons() & Qt::RightButton ) { button=2; which=3; }
    else if ( e->buttons() & Qt::MidButton ) { button=1; which=2; }
    self.setProperty( "which", which, QScriptValue::ReadOnly );
    self.setProperty( "button", button, QScriptValue::ReadOnly );

    self.setProperty( "altKey", (e->modifiers() & Qt::AltModifier ? true : false), QScriptValue::ReadOnly );
    self.setProperty( "ctrlKey", (e->modifiers() & Qt::ControlModifier ? true : false), QScriptValue::ReadOnly );
    self.setProperty( "shiftKey", (e->modifiers() & Qt::ShiftModifier ? true : false), QScriptValue::ReadOnly );
    self.setProperty( "metaKey", (e->modifiers() & Qt::MetaModifier ? true : false), QScriptValue::ReadOnly );
}

void PHIAScriptEvent::setFocusEvent( QFocusEvent *e )
{
    Q_UNUSED( e );
}

int PHIAFakeDomEvent::qtToDomKey( int keyCode )
{
     switch (keyCode) {
     case Qt::Key_Backspace:
         return  DOM_VK_BACK_SPACE;
     case Qt::Key_Enter:
         return  DOM_VK_ENTER;
     case Qt::Key_Return:
         return  DOM_VK_ENTER;
     case Qt::Key_NumLock:
         return  DOM_VK_NUM_LOCK;
     case Qt::Key_Alt:
         return  DOM_VK_RIGHT_ALT;
     case Qt::Key_Control:
         return  DOM_VK_LEFT_CONTROL;
     case Qt::Key_Shift:
         return  DOM_VK_LEFT_SHIFT;
     case Qt::Key_Meta:
         return  DOM_VK_META;
     case Qt::Key_CapsLock:
         return  DOM_VK_CAPS_LOCK;
     case Qt::Key_Delete:
         return  DOM_VK_DELETE;
     case Qt::Key_Insert:
         return DOM_VK_INSERT;
     case Qt::Key_End:
         return  DOM_VK_END;
     case Qt::Key_Escape:
         return  DOM_VK_ESCAPE;
     case Qt::Key_Home:
         return  DOM_VK_HOME;
     case Qt::Key_Pause:
         return  DOM_VK_PAUSE;
     case Qt::Key_Print:
         return  DOM_VK_PRINTSCREEN;
     case Qt::Key_ScrollLock:
         return  DOM_VK_SCROLL_LOCK;
     case Qt::Key_Left:
         return  DOM_VK_LEFT;
     case Qt::Key_Right:
         return  DOM_VK_RIGHT;
     case Qt::Key_Up:
         return  DOM_VK_UP;
     case Qt::Key_Down:
         return  DOM_VK_DOWN;
     case Qt::Key_PageDown:
         return  DOM_VK_PAGE_DOWN;
     case Qt::Key_PageUp:
         return  DOM_VK_PAGE_UP;
     case Qt::Key_F1:
         return  DOM_VK_F1;
     case Qt::Key_F2:
         return  DOM_VK_F2;
     case Qt::Key_F3:
         return  DOM_VK_F3;
     case Qt::Key_F4:
         return  DOM_VK_F4;
     case Qt::Key_F5:
         return  DOM_VK_F5;
     case Qt::Key_F6:
         return  DOM_VK_F6;
     case Qt::Key_F7:
         return  DOM_VK_F7;
     case Qt::Key_F8:
         return  DOM_VK_F8;
     case Qt::Key_F9:
         return  DOM_VK_F9;
     case Qt::Key_F10:
         return  DOM_VK_F10;
     case Qt::Key_F11:
         return  DOM_VK_F11;
     case Qt::Key_F12:
         return  DOM_VK_F12;
     case Qt::Key_F13:
         return  DOM_VK_F13;
     case Qt::Key_F14:
         return  DOM_VK_F14;
     case Qt::Key_F15:
         return  DOM_VK_F15;
     case Qt::Key_F16:
         return  DOM_VK_F16;
     case Qt::Key_F17:
         return  DOM_VK_F17;
     case Qt::Key_F18:
         return  DOM_VK_F18;
     case Qt::Key_F19:
         return  DOM_VK_F19;
     case Qt::Key_F20:
         return  DOM_VK_F20;
     case Qt::Key_F21:
         return  DOM_VK_F21;
     case Qt::Key_F22:
         return  DOM_VK_F22;
     case Qt::Key_F23:
         return  DOM_VK_F23;
     case Qt::Key_F24:
        return  DOM_VK_F24;
    }
    return keyCode;
}
