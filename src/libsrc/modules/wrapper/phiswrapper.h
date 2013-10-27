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
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PHISWRAPPER_H
#define PHISWRAPPER_H
#include "phismodule.h"
#include "phibaseitem.h"
#include "phipalette.h"
#include "phi.h"

class PHISWrapperModule : public PHISModule
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.phisketeer.phis.module.wrapper" FILE "phiswrapper.json")
    Q_CLASSINFO( "Author", "Marius Schumacher" )
    Q_CLASSINFO( "Url", "http://www.phisketeer.org" )
    Q_CLASSINFO( "Version", "2.0" )
    Q_CLASSINFO( "License", "LGPL" )
    Q_CLASSINFO( "Copyright", "2013 Phisys AG, 2013 Phisketeer Team" )

public:
    virtual PHISScriptObj* create( const QString &key, const PHISInterface* ) const;
    virtual QStringList keys() const;
};

class PHISWrapperObj : public PHISScriptObj
{
    Q_OBJECT

public:
    explicit PHISWrapperObj( const PHISInterface *interf ) : PHISScriptObj( interf ) {}

protected:
    virtual QScriptValue initObject( QScriptEngine *engine, const QString &key );
};

class PHISScriptItem : public QObject
{
    Q_OBJECT

public:
    explicit PHISScriptItem( QScriptEngine *engine, PHIBaseItem *it ) : QObject( engine ), _it( it ) {}

// usable by script
public slots:
    QScriptValue hide();
    QScriptValue show();
    QScriptValue toggle();
    QScriptValue moveBy( qint32 x, qint32 y, qint32 w=0, qint32 h=0, qint32 start=0,
        qint32 duration=1000, const QString &ease=PHI::defaultEasingCurve() );
    QScriptValue moveTo( qint32 left, qint32 top, qint32 start=0, qint32 duration=1000,
        const QString &ease=PHI::defaultEasingCurve() );
    QScriptValue fadeOut( qint32 start=0, qint32 duration=1000, qreal minOpac=0.,
        const QString &ease=PHI::defaultEasingCurve() );
    QScriptValue fadeIn( qint32 start=0, qint32 duration=1000, qreal maxOpac=1.,
        const QString &ease=PHI::defaultEasingCurve() );
    QScriptValue selectOptions( const QString &v=QString(), const QString &d=QString() );
    QScriptValue rotateIn( quint8 axis=0x2, qint32 start=0, qint32 duration=1000,
        const QString &ease=PHI::defaultEasingCurve() );
    QScriptValue rotateOut( quint8 axis=0x2, qint32 start=0, qint32 duration=1000,
        const QString &ease=PHI::defaultEasingCurve() );
    QScriptValue val( const QScriptValue &v=QScriptValue() );
    QScriptValue progress( int=-1 );
    QScriptValue html( const QString &html=QString() );
    QScriptValue opacity( qreal opac=-1. );
    QScriptValue cursor( const QString &c=QString() );
    QScriptValue checked( const QScriptValue &v=QScriptValue(), const QScriptValue &c=QScriptValue() );
    QScriptValue title( const QString &t=QString() );
    QScriptValue pos( const QScriptValue &l=QScriptValue(), const QScriptValue &t=QScriptValue() );
    QScriptValue left( const QScriptValue &l=QScriptValue() );
    QScriptValue top( const QScriptValue &t=QScriptValue() );
    QScriptValue width( const QScriptValue &l=QScriptValue() );
    QScriptValue height( const QScriptValue &t=QScriptValue() );
    QScriptValue selected( const QString &v=QString(), const QScriptValue &s=QScriptValue() );
    QScriptValue visible( const QScriptValue &b=QScriptValue() );
    QScriptValue zIndex( const QScriptValue &i=QScriptValue() );
    QScriptValue color( const QScriptValue &c=QScriptValue() );
    QScriptValue bgColor( const QScriptValue &c=QScriptValue() );
    QScriptValue dragEnabled( const QScriptValue &b=QScriptValue() );
    QScriptValue dropEnabled( const QScriptValue &b=QScriptValue() );
    QScriptValue text( const QScriptValue &t=QScriptValue() );
    QScriptValue label( const QScriptValue &l=QScriptValue() );

protected:
    inline QScriptEngine* engine() const { return qobject_cast<QScriptEngine*>(parent()); }
    inline QScriptValue self() { return engine()->newQObject( this,
        QScriptEngine::ScriptOwnership, QScriptEngine::PreferExistingWrapperObject |
        QScriptEngine::ExcludeSuperClassContents | QScriptEngine::ExcludeDeleteLater ); }

private:
    PHIBaseItem *_it;
};

inline QScriptValue PHISScriptItem::hide()
{
    _it->setVisible( false );
    return self();
}

inline QScriptValue PHISScriptItem::show()
{
    _it->setVisible( true );
    return self();
}

inline QScriptValue PHISScriptItem::toggle()
{
    _it->setVisible( !_it->visible() );
    return self();
}

inline QScriptValue PHISScriptItem::moveBy( qint32 x, qint32 y, qint32 w, qint32 h, qint32 start,
    qint32 duration, const QString &ease )
{
    _it->effect()->setMoveBy( start, duration, x, y, w, h, PHI::toEasingCurveType( ease ) );
    return self();
}

inline QScriptValue PHISScriptItem::moveTo( qint32 left, qint32 top, qint32 start, qint32 duration,
    const QString &ease ) {
    _it->effect()->setMoveTo( start, duration, left, top, PHI::toEasingCurveType( ease ) );
    return self();
}

inline QScriptValue PHISScriptItem::fadeOut( qint32 start, qint32 duration, qreal minOpac,
    const QString &ease ) {
    _it->effect()->setFadeOut( start, duration, minOpac, PHI::toEasingCurveType( ease ) );
    return self();
}

inline QScriptValue PHISScriptItem::fadeIn( qint32 start, qint32 duration, qreal maxOpac,
    const QString &ease ) {
    _it->effect()->setFadeIn( start, duration, maxOpac, PHI::toEasingCurveType( ease ) );
    return self();
}

inline QScriptValue PHISScriptItem::selectOptions( const QString &v, const QString &d ) {
    if ( v.isNull() && d.isNull() ) return QScriptValue( _it->property( "value" ).toString() );
    if ( !d.isNull() ) _it->setProperty( "delimiter",  d );
    _it->setProperty( "value", v );
    return self();
}

inline QScriptValue PHISScriptItem::rotateIn( quint8 axis, qint32 start, qint32 duration,
    const QString &ease )
{
    // @todo: enable ease in rotateIn
    _it->effect()->setRotateIn( axis, start, duration );
    return self();
}

inline QScriptValue PHISScriptItem::rotateOut( quint8 axis, qint32 start, qint32 duration,
    const QString &ease ) {
    // @todo: enable ease in rotateOut
    _it->effect()->setRotateOut( axis, start, duration );
    return self();
}

inline QScriptValue PHISScriptItem::left( const QScriptValue &l )
{
    if ( !l.isValid() ) return _it->realX();
    _it->setX( l.toInt32() );
    return self();
}

inline QScriptValue PHISScriptItem::top( const QScriptValue &t )
{
    if ( !t.isValid() ) return _it->realY();
    _it->setY( t.toInt32() );
    return self();
}

inline QScriptValue PHISScriptItem::width( const QScriptValue &w )
{
    if ( !w.isValid() ) return _it->realWidth();
    _it->setWidth( w.toInt32() );
    return self();
}

inline QScriptValue PHISScriptItem::height( const QScriptValue &h )
{
    if ( !h.isValid() ) return _it->realHeight();
    _it->setHeight( h.toInt32() );
    return self();
}

inline QScriptValue PHISScriptItem::zIndex( const QScriptValue &i )
{
    if ( !i.isValid() ) return _it->realZIndex();
    _it->setZIndex( static_cast<qint16>(i.toInt32()) );
    return self();
}

inline QScriptValue PHISScriptItem::color( const QScriptValue &c )
{
    if ( !c.isValid() ) return _it->property( "color" ).value<QColor>().name();
    _it->setProperty( "color", QColor( c.toString() ) );
    return self();
}

inline QScriptValue PHISScriptItem::bgColor( const QScriptValue &c )
{
    if ( !c.isValid() ) return _it->property( "backgroundColor" ).value<QColor>().name();
    _it->setProperty( "backgroundColor", QColor( c.toString() ) );
    return self();
}

inline QScriptValue PHISScriptItem::dragEnabled( const QScriptValue &b )
{
    // @todo: implement drag/drop
    //if ( !b.isValid() ) return _it->draggable();
    //_it->setDragable( b.toBool() );
    return self();
}

inline QScriptValue PHISScriptItem::dropEnabled( const QScriptValue &b )
{
    // @todo: implement drag/drop
    //if ( !b.isValid() ) return _it->droppable();
    //_it->setDropable( b.toBool() );
    return self();
}

inline QScriptValue PHISScriptItem::progress( int p )
{
    // @todo: implement progress bar
    //if ( _it->wid()!=PHI::PROGRESSBAR ) return self();
    if ( p==-1 ) return _it->property( "value" ).toInt();
    _it->setProperty( "value", QString::number( qBound( 0, p, 100 ) ) );
    return self();
}

inline QScriptValue PHISScriptItem::html( const QString &html )
{
    // @todo: implement html for rich text
    //if ( _it->wid()!=PHI::RICH_TEXT && _it->wid()!=PHI::TEXT ) return self();
    if ( html.isNull() ) return _it->property( "value" ).toString();
    _it->setProperty( "value", html );
    return self();
}

inline QScriptValue PHISScriptItem::val( const QScriptValue &v )
{
    if ( !_it->isInputItem() ) return self();
    // if ( _it->wid()==PHI::PROGRESSBAR ) return self();
    if ( !v.isValid() ) return _it->property( "value" ).toString();
    _it->setProperty( "value", v.toString() );
    return self();
}

inline QScriptValue PHISScriptItem::visible( const QScriptValue &b )
{
    if ( b.isNull() ) return _it->visible();
    _it->setVisible( b.toBool() );
    return self();
}

inline QScriptValue PHISScriptItem::opacity( qreal opac )
{
    if ( opac==-1. ) return _it->realOpacity();
    _it->setOpacity( qBound( 0., opac, 1. ) );
    return self();
}

inline QScriptValue PHISScriptItem::cursor( const QString &c )
{
    // @todo: implement cursor
    //if ( c.isNull() ) return QString::fromLatin1( _it->cursor() );
    //_it->setCursor( c.toLatin1() );
    return self();
}

inline QScriptValue PHISScriptItem::title( const QString &t )
{
    if ( t.isNull() ) return _it->realTitle();
    _it->setTitle( t );
    return self();
}

inline QScriptValue PHISScriptItem::text( const QScriptValue &t )
{
    // @todo: check client script for property text
    if ( _it->isInputItem() ) {
        if ( !t.isValid() ) return _it->property( "value" ).toString();
        _it->setProperty( "value", t.toString() );
    } else {
        if ( !t.isValid() ) return _it->property( "text" ).toString();
        _it->setProperty( "text", t.toString() );
    }
    return self();
}

inline QScriptValue PHISScriptItem::label( const QScriptValue &l )
{
    if ( !l.isValid() ) return _it->property( "label" ).toString();
    _it->setProperty( "label", l.toString() );
    return self();
}

inline PHISScriptObj* PHISWrapperModule::create( const QString &key, const PHISInterface *interface ) const
{
    if ( key==L1( "wrapper" ) ) return new PHISWrapperObj( interface );
    return 0;
}

inline QStringList PHISWrapperModule::keys() const
{
    return QStringList() << SL( "wrapper" );
}

#endif // PHISWRAPPER_H
