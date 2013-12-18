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
#ifndef PHIDOMEVENT_H
#define PHIDOMEVENT_H
#include <QObject>
#include <QScriptValue>
#include <QMultiHash>

typedef QMultiHash<QString, QScriptValue> PHIEventHash;
Q_DECLARE_METATYPE( PHIEventHash )

class PHIBaseItem;
class QGraphicsSceneMouseEvent;
class QGraphicsSceneHoverEvent;
class QGraphicsSceneDragDropEvent;
class QKeyEvent;
class QFocusEvent;

class PHIDomEvent : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( PHIDomEvent )
    Q_PROPERTY( qint64 timeStamp READ timeStamp )

public:
    enum Event { ENone=0, EClick=1, EDblClick, EMouseOver, EMouseMove, EMouseUp, EMouseDown,
        EMouseOut, EChange, EFocus, EBlur, EKeyPress, EKeyDown, EKeyUp, EDrop,
        ECustom, ESubmit, ESelect, ELoad };
    explicit PHIDomEvent( const QString &type, PHIBaseItem *item, bool cancelable=false );
    virtual ~PHIDomEvent();

    void setMouseEvent( QGraphicsSceneMouseEvent *e );
    void setHoverEvent( QGraphicsSceneHoverEvent *e );
    void setKeyEvent( QKeyEvent *e );
    void setFocusEvent( QFocusEvent *e );
    void setDropEvent( QGraphicsSceneDragDropEvent *e );
    QPoint mapFromScene( const QPointF &pos ) const;

    static QString eventTypeToString( Event e );
    static bool isMetaKey( int key );
    inline const QScriptValue& self() const { return _self; }

public slots:
    inline void stopPropagation() { _stopPropagation=true; }
    inline bool isPropagationStopped() const { return _stopPropagation; }
    inline void preventDefault() { if ( _cancelable ) _cancelDefault=true; }
    inline bool isDefaultPrevented() const { return _cancelDefault; }
    inline qint64 timeStamp() const { return _timestamp; }

private:
    const PHIBaseItem *_item;
    bool _cancelDefault, _stopPropagation;
    const bool _cancelable;
    qint64 _timestamp;
    QScriptValue _self;
};

Q_DECLARE_METATYPE( PHIDomEvent* )

QScriptValue eventToScriptValue( QScriptEngine*, PHIDomEvent* const &in );
void eventFromScriptValue( const QScriptValue&, PHIDomEvent* &out );

#endif // PHIDOMEVENT_H
