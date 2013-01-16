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
#ifndef PHIASCRIPTITEM_H
#define PHIASCRIPTITEM_H

#include <QObject>
#include <QScriptEngine>
#include "phiaitem.h"
#include "phiawebview.h"

class PHIAScriptItem;

Q_DECLARE_METATYPE( PHIAScriptItem* )

QScriptValue scriptItemToScriptValue( QScriptEngine*, PHIAScriptItem* const &in );
void scriptItemFromScriptValue( const QScriptValue&, PHIAScriptItem* &out );

class PHIAScriptItem : public QObject
{
    Q_OBJECT

public:
    explicit PHIAScriptItem( PHIAItem *it, QObject *parent=0 );
    virtual ~PHIAScriptItem();

public slots:
    inline PHIAScriptItem* hide() { _it->setVisible( false ); return this; }
    inline PHIAScriptItem* show() { _it->setVisible( true ); return this; }
    inline PHIAScriptItem* toggle() { _it->setVisible( !_it->visible() ); return this; }

    inline PHIAScriptItem* moveBy( qint32 x, qint32 y, qint32 w=0, qint32 h=0, qint32 start=0,
        qint32 duration=1000, const QString &ease=PHI::defaultEasingCurve() ) {
        _it->setMoveBy( start, duration, x, y, w, h, ease ); return this;
    }
    inline PHIAScriptItem* moveTo( qint32 left, qint32 top, qint32 start=0, qint32 duration=1000,
        const QString &ease=PHI::defaultEasingCurve() ) {
        _it->setMoveTo( start, duration, left, top, ease ); return this;
    }
    inline PHIAScriptItem* fadeOut( qint32 start=0, qint32 duration=1000, qreal minOpac=0.,
        const QString &ease=PHI::defaultEasingCurve() ) {
        _it->setFadeOut( start, duration, minOpac, ease ); return this;
    }
    inline PHIAScriptItem* fadeIn( qint32 start=0, qint32 duration=1000, qreal maxOpac=1.,
        const QString &ease=PHI::defaultEasingCurve() ) {
        _it->setFadeIn( start, duration, maxOpac, ease ); return this;
    }
    inline PHIAScriptItem* on( const QString &e, const QScriptValue &fn ) {
        _it->setOnHandler( e, fn ); return this;
    }
    inline PHIAScriptItem* off( const QString &e, const QScriptValue &fn=QScriptValue() ) {
        _it->setOffHandler( e, fn ); return this;
    }
    inline PHIAScriptItem* click( const QScriptValue &fn=QScriptValue() ) {
        if ( fn.isValid() ) _it->setOnHandler( "click", fn );
        else _it->click(); return this;
    }
    inline PHIAScriptItem* dblclick( const QScriptValue &fn=QScriptValue() ) {
        if ( fn.isValid() ) _it->setOnHandler( "dblclick", fn );
        else _it->slotOnDblClick( 0 ); return this;
    }
    inline PHIAScriptItem* mouseover( const QScriptValue &fn=QScriptValue() ) {
        if ( fn.isValid() ) _it->setOnHandler( "mouseover", fn );
        else _it->slotOnMouseOver( 0 ); return this;
    }
    inline PHIAScriptItem* mouseout( const QScriptValue &fn=QScriptValue() ) {
        if ( fn.isValid() ) _it->setOnHandler( "mouseout", fn );
        else _it->slotOnMouseOut( 0 ); return this;
    }
    inline PHIAScriptItem* mousedown( const QScriptValue &fn=QScriptValue() ) {
        if ( fn.isValid() ) _it->setOnHandler( "mousedown", fn );
        else _it->slotOnMouseDown( 0 ); return this;
    }
    inline PHIAScriptItem* mouseup( const QScriptValue &fn=QScriptValue() ) {
        if ( fn.isValid() ) _it->setOnHandler( "mouseup", fn );
        else _it->slotOnMouseUp( 0 ); return this;
    }
    inline PHIAScriptItem* mousemove( const QScriptValue &fn=QScriptValue() ) {
        if ( fn.isValid() ) _it->setOnHandler( "mousemove", fn );
        else _it->slotOnMouseMove( 0 ); return this;
    }
    inline PHIAScriptItem* keypress( const QScriptValue &fn=QScriptValue() ) {
        if ( fn.isValid() ) _it->setOnHandler( "keypress", fn );
        else _it->slotOnKeyPress( 0 ); return this;
    }
    inline PHIAScriptItem* keyup( const QScriptValue &fn=QScriptValue() ) {
        if ( fn.isValid() ) _it->setOnHandler( "keyup", fn );
        else _it->slotOnKeyUp( 0 ); return this;
    }
    inline PHIAScriptItem* keydown( const QScriptValue &fn=QScriptValue() ) {
        if ( fn.isValid() ) _it->setOnHandler( "keydown", fn );
        else _it->slotOnKeyDown( 0 ); return this;
    }
    inline PHIAScriptItem* change( const QScriptValue &fn=QScriptValue() ) {
        if ( fn.isValid() ) _it->setOnHandler( "change", fn );
        else _it->slotOnChange(); return this;
    }
    inline PHIAScriptItem* blur( const QScriptValue &fn=QScriptValue() ) {
        if ( fn.isValid() ) _it->setOnHandler( "blur", fn );
        else _it->blur(); return this;
    }
    inline PHIAScriptItem* focus( const QScriptValue &fn=QScriptValue() ) {
        if ( fn.isValid() ) _it->setOnHandler( "focus", fn );
        else _it->focus(); return this;
    }
    inline PHIAScriptItem* drop( const QScriptValue &fn ) {
        if ( fn.isValid() ) _it->setOnHandler( "drop", fn );
        return this;
    }
    inline PHIAScriptItem* selectOptions( const QString &v, const QString &d=QString() ) {
        if ( _it->wid()==PHI::COMBO_BOX || _it->wid()==PHI::LIST ) {
            if ( !d.isNull() ) _it->setDelimiter( d );
            _it->setValue( v );
        }
        return this;
    }
    inline PHIAScriptItem* slide( const QString &dir, qint32 duration=400,
            const QString &ease=PHI::defaultEasingCurve() ) {
        if ( dir=="down" ) _it->setSlideDown( 0, duration, ease );
        else _it->setSlideUp( 0, duration, ease );
        return this;
    }
    inline PHIAScriptItem* rotateIn( quint8 axis=0x2, qint32 start=0, qint32 duration=1000,
        const QString &ease=PHI::defaultEasingCurve() ) {
        _it->setRotateIn( axis, start, duration, ease );
        return this;
    }
    inline PHIAScriptItem* rotateOut( quint8 axis=0x2, qint32 start=0, qint32 duration=1000,
        const QString &ease=PHI::defaultEasingCurve() ) {
        _it->setRotateOut( axis, start, duration, ease );
        return this;
    }
    inline PHIAScriptItem* stop() {
        _it->stopAnimations();
        return this;
    }

    QScriptValue val( const QScriptValue &v=QScriptValue() );
    QScriptValue progress( int=-1 );
    QScriptValue html( const QString &html=QString() );
    QScriptValue opacity( qreal opac=-1. );
    QScriptValue cursor( const QString &c=QString() );
    QScriptValue checked( const QScriptValue &v=QScriptValue(), const QScriptValue &c=QScriptValue() );
    QScriptValue title( const QString &t=QString() );
    QScriptValue trigger( const QString &e, const QScriptValue &arg=QScriptValue() );
    QScriptValue pos( const QScriptValue &l=QScriptValue(), const QScriptValue &t=QScriptValue() );
    QScriptValue left( const QScriptValue &l=QScriptValue() );
    QScriptValue top( const QScriptValue &t=QScriptValue() );
    QScriptValue width( const QScriptValue &w=QScriptValue() );
    QScriptValue height( const QScriptValue &h=QScriptValue() );
    QScriptValue selected( const QString &v=QString(), const QScriptValue &s=QScriptValue() );
    QScriptValue visible( const QScriptValue &b=QScriptValue() );
    QScriptValue zIndex( const QScriptValue &i=QScriptValue() );
    QScriptValue color( const QScriptValue &c=QScriptValue() );
    QScriptValue bgColor( const QScriptValue &c=QScriptValue() );
    QScriptValue dragEnabled( const QScriptValue &b=QScriptValue() );
    QScriptValue dropEnabled( const QScriptValue &b=QScriptValue() );

protected:
    inline QScriptEngine* engine() const { return _it->view()->scriptEngine(); }
    inline QScriptValue self() { return scriptItemToScriptValue( engine(), this ); }

private:
    PHIAItem *_it;
};

#endif // PHIASCRIPTITEM_H
