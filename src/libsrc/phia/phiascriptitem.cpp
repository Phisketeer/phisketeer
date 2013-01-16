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
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "phiascriptitem.h"

QScriptValue scriptItemToScriptValue( QScriptEngine *engine, PHIAScriptItem* const &obj )
{
    return engine->newQObject( obj, QScriptEngine::ScriptOwnership,
        QScriptEngine::PreferExistingWrapperObject | QScriptEngine::SkipMethodsInEnumeration |
        QScriptEngine::ExcludeSuperClassContents | QScriptEngine::ExcludeDeleteLater );
}

void scriptItemFromScriptValue( const QScriptValue &val, PHIAScriptItem* &obj )
{
    obj=qobject_cast<PHIAScriptItem*>(val.toQObject());
}

PHIAScriptItem::PHIAScriptItem( PHIAItem *it, QObject *parent )
    : QObject( parent ), _it( it )
{
    //qDebug( "PHIAScriptItem::PHIAScriptItem()" );
    setObjectName( _it->id() );
}

PHIAScriptItem::~PHIAScriptItem()
{
    //qDebug( "PHIAScriptItem::~PHIAScriptItem()" );
}

QScriptValue PHIAScriptItem::trigger( const QString &e, const QScriptValue &arg )
{
    QScriptValueList list;
    if ( arg.isArray() ) {
        quint32 i=0;
        while ( arg.property( i ).isValid() ) {
            list << arg.property( i );
            ++i;
        }
    } else list << arg;
    _it->triggerEvent( e, list );
    return self();
}

QScriptValue PHIAScriptItem::checked( const QScriptValue &v, const QScriptValue &c )
{
    qDebug() << "checked " << v.toString() << c.toString();
    if ( !v.isValid() && !c.isValid() ) return _it->checked();
    if ( v.isBool() ) { _it->setChecked( v.toBool() ); return self(); }
    if ( !v.isString() ) return self();
    if ( PHI::isLayoutContainer( _it->wid() ) ) { // we check the children of the layout
        QList <PHIBaseItem*> list=_it->view()->page()->childrenOf( _it->name() );
        PHIBaseItem *it;
        foreach ( it, list ) {
            if ( it->wid()!=PHI::RADIO_BUTTON && it->wid() !=PHI::CHECK_BOX ) continue;
            if ( it->value()==v.toString() ) {
                if ( !c.isBool() ) return it->checked();
                else {
                    qDebug() << "setting " << it->name() << " to " << c.toString();
                    it->setChecked( c.toBool() );
                    return self();
                }
            }
        }
    }
    if ( _it->wid()!=PHI::CHECK_LIST ) return self();
    if ( !c.isBool() ) return _it->valueChecked( v.toString() );
    _it->setValueChecked( v.toString(), c.toBool() );
    return self();
}

QScriptValue PHIAScriptItem::selected( const QString &v, const QScriptValue &s )
{
    qDebug() << "selected " << v << s.toString();
    if ( v.isNull() && !s.isValid() ) {
        QScriptValue arr=engine()->newArray();
        QString val;
        quint32 l=0;
        foreach ( val, _it->selectedItems() ) arr.setProperty( l++, val );
        return arr;
    }
    if ( !s.isBool() ) return _it->valueChecked( v );
    _it->setValueChecked( v, s.toBool() );
    return self();
}

QScriptValue PHIAScriptItem::progress( int p )
{
    if ( _it->wid()!=PHI::PROGRESSBAR ) return self();
    if ( p==-1 ) return _it->value().toInt();
    _it->setValue( QString::number( qBound( 0, p, 100 ) ) );
    return self();
}

QScriptValue PHIAScriptItem::html( const QString &html )
{
    if ( _it->wid()!=PHI::RICH_TEXT && _it->wid()!=PHI::TEXT ) return self();
    if ( html.isNull() ) return _it->value();
    _it->setValue( html );
    return self();
}

QScriptValue PHIAScriptItem::val( const QScriptValue &v )
{
    if ( !PHI::isInputItem( _it->wid() ) ) return self();
    if ( _it->wid()==PHI::PROGRESSBAR ) return self();
    if ( !v.isValid() ) return _it->value();
    _it->setValue( v.toString() ); return self();
}

QScriptValue PHIAScriptItem::visible( const QScriptValue &b )
{
    if ( b.isNull() ) return _it->visible();
    _it->setVisible( b.toBool() );
    return self();
}

QScriptValue PHIAScriptItem::opacity( qreal opac )
{
    if ( opac==-1. ) return _it->opacity();
    _it->setOpacity( qBound( 0., opac, 1. ) );
    return self();
}

QScriptValue PHIAScriptItem::cursor( const QString &c )
{
    if ( c.isNull() ) return QString::fromLatin1( _it->cursor() );
    _it->setCursor( c.toLatin1() );
    return self();
}

QScriptValue PHIAScriptItem::title( const QString &t )
{
    if ( t.isNull() ) return _it->toolTip();
    _it->setToolTip( t );
    return self();
}

QScriptValue PHIAScriptItem::pos( const QScriptValue &t, const QScriptValue &l )
{
    if ( t.isValid() ) {
        _it->setPos( QPointF( t.toInt32(), l.toInt32() ) );
        return self();
    }
    QScriptValue obj=engine()->newObject();
    obj.setProperty( "left", _it->x() );
    obj.setProperty( "top", _it->y() );
    return obj;
}

QScriptValue PHIAScriptItem::left( const QScriptValue &l )
{
    if ( !l.isValid() ) return _it->x();
    _it->setX( l.toInt32() ); return self();
}

QScriptValue PHIAScriptItem::top( const QScriptValue &t )
{
    if ( !t.isValid() ) return _it->y();
    _it->setY( t.toInt32() ); return self();
}

QScriptValue PHIAScriptItem::width( const QScriptValue &w )
{
    if ( !w.isValid() ) return _it->width();
    _it->setWidth( w.toInt32() ); return self();
}

QScriptValue PHIAScriptItem::height( const QScriptValue &h )
{
    if ( !h.isValid() ) return _it->height();
    _it->setHeight( h.toInt32() ); return self();
}

QScriptValue PHIAScriptItem::zIndex( const QScriptValue &i )
{
    if ( !i.isValid() ) return _it->zValue();
    _it->setZValue( static_cast<qint16>(i.toInt32()) ); return self();
}

QScriptValue PHIAScriptItem::color( const QScriptValue &c )
{
    if ( !c.isValid() ) return _it->color().name();
    _it->setColor( QColor( c.toString() ) ); return self();
}

QScriptValue PHIAScriptItem::bgColor( const QScriptValue &c )
{
    if ( !c.isValid() ) return _it->outlineColor().name();
    _it->setOutlineColor( QColor( c.toString() ) ); return self();
}

QScriptValue PHIAScriptItem::dragEnabled( const QScriptValue &b )
{
    if ( !b.isValid() ) return _it->draggable();
    _it->setDragable( b.toBool() ); return self();
}

QScriptValue PHIAScriptItem::dropEnabled( const QScriptValue &b )
{
    if ( !b.isValid() ) return _it->droppable();
    _it->setDropable( b.toBool() ); return self();
}
