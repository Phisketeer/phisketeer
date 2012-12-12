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
#ifndef PHIASCRIPTEVENT_H
#define PHIASCRIPTEVENT_H

#include <QObject>
#include <QScriptEngine>
#include <QScriptValue>
#include "phiawebview.h"

class PHIBaseItem;
class QGraphicsSceneMouseEvent;
class QGraphicsSceneHoverEvent;
class QGraphicsSceneDragDropEvent;
class QKeyEvent;
class QFocusEvent;

#define PHIASCRIPTEVENTEXTENSION QScriptEngine::ScriptOwnership, QScriptEngine::SkipMethodsInEnumeration |\
    QScriptEngine::ExcludeSuperClassContents | QScriptEngine::ExcludeDeleteLater

class PHIAScriptEvent : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( PHIAScriptEvent )

    Q_PROPERTY( qint64 timeStamp READ timeStamp )

public:
    explicit PHIAScriptEvent( PHIAWebView*, const QString &type, const PHIBaseItem *item, bool cancelable=false );
    virtual ~PHIAScriptEvent();

    void setMouseEvent( QGraphicsSceneMouseEvent *e );
    void setHoverEvent( QGraphicsSceneHoverEvent *e );
    void setKeyEvent( QKeyEvent *e );
    void setFocusEvent( QFocusEvent *e );
    void setDropEvent( QGraphicsSceneDragDropEvent *e );
    inline bool defaultCanceled() const { return _cancelDefault; }
    inline QPoint mapFromScene( const QPointF &p ) const {
        return _view->scene()->views().first()->mapFromScene( p ); }

public slots:
    inline void stopPropagation() {;}
    inline void preventDefault() { if ( _cancelable ) _cancelDefault=true; }
    inline qint64 timeStamp() const { return _timestamp; }

private:
    PHIAWebView *_view;
    bool _cancelDefault, _cancelable;
    qint64 _timestamp;
};

Q_DECLARE_METATYPE( PHIAScriptEvent* )
QScriptValue eventToScriptValue( QScriptEngine*, PHIAScriptEvent* const &in );
void eventFromScriptValue( const QScriptValue&, PHIAScriptEvent* &out );

#endif // PHIASCRIPTEVENT_H
