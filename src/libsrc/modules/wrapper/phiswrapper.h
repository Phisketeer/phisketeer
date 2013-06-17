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

class PHISWrapperModule : public PHISModule
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.phisketeer.phis.module.wrapper" FILE "phiswrapper.json")
    Q_CLASSINFO( "Author", "Marius Schumacher" )
    Q_CLASSINFO( "Url", "http://www.phisketeer.org" )
    Q_CLASSINFO( "Version", "1.0" )
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
    _it->setMoveBy( start, duration, x, y, w, h, ease );
    return self();
}

inline QScriptValue PHISScriptItem::moveTo( qint32 left, qint32 top, qint32 start, qint32 duration,
    const QString &ease ) {
    _it->setMoveTo( start, duration, left, top, ease );
    return self();
}

inline QScriptValue PHISScriptItem::fadeOut( qint32 start, qint32 duration, qreal minOpac,
    const QString &ease ) {
    _it->setFadeOut( start, duration, minOpac, ease );
    return self();
}

inline QScriptValue PHISScriptItem::fadeIn( qint32 start, qint32 duration, qreal maxOpac,
    const QString &ease ) {
    _it->setFadeIn( start, duration, maxOpac, ease );
    return self();
}

inline QScriptValue PHISScriptItem::selectOptions( const QString &v, const QString &d ) {
    if ( v.isNull() && d.isNull() ) return QScriptValue( _it->value() );
    if ( !d.isNull() ) _it->setDelimiter( d );
    _it->setValue( v );
    return self();
}

inline QScriptValue PHISScriptItem::rotateIn( quint8 axis, qint32 start, qint32 duration,
    const QString &ease )
{
    _it->setRotateIn( axis, start, duration, ease );
    return self();
}

inline QScriptValue PHISScriptItem::rotateOut( quint8 axis, qint32 start, qint32 duration,
    const QString &ease ) {
    _it->setRotateOut( axis, start, duration, ease );
    return self();
}

inline QScriptValue PHISScriptItem::left( const QScriptValue &l )
{
    if ( !l.isValid() ) return _it->x();
    _it->setX( l.toInt32() );
    return self();
}

inline QScriptValue PHISScriptItem::top( const QScriptValue &t )
{
    if ( !t.isValid() ) return _it->y();
    _it->setY( t.toInt32() );
    return self();
}

inline QScriptValue PHISScriptItem::width( const QScriptValue &w )
{
    if ( !w.isValid() ) return _it->width();
    _it->setWidth( w.toInt32() );
    return self();
}

inline QScriptValue PHISScriptItem::height( const QScriptValue &h )
{
    if ( !h.isValid() ) return _it->height();
    _it->setHeight( h.toInt32() );
    return self();
}

inline QScriptValue PHISScriptItem::zIndex( const QScriptValue &i )
{
    if ( !i.isValid() ) return _it->zValue();
    _it->setZValue( static_cast<qint16>(i.toInt32()) );
    return self();
}

inline QScriptValue PHISScriptItem::color( const QScriptValue &c )
{
    if ( !c.isValid() ) return _it->color().name();
    _it->setColor( QColor( c.toString() ) );
    return self();
}

inline QScriptValue PHISScriptItem::bgColor( const QScriptValue &c )
{
    if ( !c.isValid() ) return _it->outlineColor().name();
    _it->setOutlineColor( QColor( c.toString() ) );
    return self();
}

inline QScriptValue PHISScriptItem::dragEnabled( const QScriptValue &b )
{
    if ( !b.isValid() ) return _it->draggable();
    _it->setDragable( b.toBool() );
    return self();
}

inline QScriptValue PHISScriptItem::dropEnabled( const QScriptValue &b )
{
    if ( !b.isValid() ) return _it->droppable();
    _it->setDropable( b.toBool() );
    return self();
}

inline QScriptValue PHISScriptItem::progress( int p )
{
    if ( _it->wid()!=PHI::PROGRESSBAR ) return self();
    if ( p==-1 ) return _it->value().toInt();
    _it->setValue( QString::number( qBound( 0, p, 100 ) ) );
    return self();
}

inline QScriptValue PHISScriptItem::html( const QString &html )
{
    if ( _it->wid()!=PHI::RICH_TEXT && _it->wid()!=PHI::TEXT ) return self();
    if ( html.isNull() ) return _it->value();
    _it->setValue( html );
    return self();
}

inline QScriptValue PHISScriptItem::val( const QScriptValue &v )
{
    if ( !PHI::isInputItem( static_cast<PHI::Widget>(_it->wid()) ) ) return self();
    if ( _it->wid()==PHI::PROGRESSBAR ) return self();
    if ( !v.isValid() ) return _it->value();
    _it->setValue( v.toString() );
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
    if ( opac==-1. ) return _it->opacity();
    _it->setOpacity( qBound( 0., opac, 1. ) );
    return self();
}

inline QScriptValue PHISScriptItem::cursor( const QString &c )
{
    if ( c.isNull() ) return QString::fromLatin1( _it->cursor() );
    _it->setCursor( c.toLatin1() );
    return self();
}

inline QScriptValue PHISScriptItem::title( const QString &t )
{
    if ( t.isNull() ) return _it->toolTip();
    _it->setToolTip( t );
    return self();
}

inline QScriptValue PHISScriptItem::text( const QScriptValue &t )
{
    if ( !t.isValid() ) return _it->value();
    _it->setValue( t.toString() );
    return self();
}

inline QScriptValue PHISScriptItem::label( const QScriptValue &l )
{
    if ( !l.isValid() ) return _it->label();
    _it->setLabel( l.toString() );
    return self();
}

inline PHISScriptObj* PHISWrapperModule::create( const QString &key, const PHISInterface *interface ) const
{
    if ( key==QLatin1String( "wrapper" ) ) return new PHISWrapperObj( interface );
    return 0;
}

inline QStringList PHISWrapperModule::keys() const
{
    return QStringList() << QStringLiteral( "wrapper" );
}

#endif // PHISWRAPPER_H
