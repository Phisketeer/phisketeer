/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2014  Phisys AG, Switzerland
#    Copyright (C) 2012-2014  Phisketeer.org team
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
#ifndef PHIITEMSTYLECSS_H
#define PHIITEMSTYLECSS_H

#include <QObject>
#include <QRegularExpression>
#include <QCursor>
#include "phibaseitem.h"
#include "phieffect.h"
#include "phi.h"

class PHIDomItem;

class PHIDomEffect : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( PHIDomEffect )

    Q_PROPERTY( QStringList properties READ properties )
    Q_PROPERTY( quint8 xAxis READ xAxis )
    Q_PROPERTY( quint8 yAxis READ yAxis )
    Q_PROPERTY( quint8 zAxis READ zAxis )

public:
    explicit PHIDomEffect( PHIDomItem *it );
    virtual ~PHIDomEffect() {}

public slots:
    inline quint8 xAxis() const { return 0x1; }
    inline quint8 yAxis() const { return 0x2; }
    inline quint8 zAxis() const { return 0x4; }
    inline QStringList properties() const { return PHI::properties( this ); }
    inline void fadeIn( qint32 start=0, qint32 duration=1000, qreal maxOpac=1.,
        const QString &ease=PHI::defaultEasingCurve() )
        { _it->fadeIn( start, duration, maxOpac, ease ); }
    inline void fadeOut( qint32 start=0, qint32 duration=1000, qreal minOpac=0.,
        const QString &ease=PHI::defaultEasingCurve() )
        { _it->fadeOut( start, duration, minOpac, ease ); }
    inline void moveTo( qint32 left, qint32 top, qint32 start=0, qint32 duration=1000,
        const QString &ease=PHI::defaultEasingCurve() )
        { _it->moveTo( start, duration, left, top, ease ); }
    inline void rotateIn( quint8 axis=0x2, qint32 start=0, qint32 duration=1000,
        const QString &ease=PHI::defaultEasingCurve() )
        { _it->rotateIn( axis, start, duration, ease ); }
    inline void rotateOut( quint8 axis=0x2, qint32 start=0, qint32 duration=1000,
        const QString &ease=PHI::defaultEasingCurve() )
        { _it->rotateOut( axis, start, duration, ease ); }
    inline void rotate( quint8 axis=0x4, qreal stepx=0, qreal stepy=0, qreal stepz=1. )
        { _it->rotate( axis, stepx, stepy, stepz ); }
    inline void clear() { _it->clearEffects(); }
    inline void stopAnimations() { _it->stop(); }

private:
    PHIBaseItem *_it;
};

// this class provides the CSS style property (used for scripting only)
class PHIItemStyleCSS : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( PHIItemStyleCSS )

    Q_PROPERTY( QString top WRITE setTop READ top )
    Q_PROPERTY( QString left WRITE setLeft READ left )
    Q_PROPERTY( QString width WRITE setWidth READ width )
    Q_PROPERTY( QString height WRITE setHeight READ height )
    Q_PROPERTY( qreal opacity WRITE setOpacity READ opacity )
    Q_PROPERTY( qint16 zIndex WRITE setZIndex READ zIndex )
    Q_PROPERTY( QStringList properties READ properties )

    Q_PROPERTY( QString borderRadius WRITE setBorderRadius READ borderRadius )
    Q_PROPERTY( quint8 line WRITE setLine READ line )
    Q_PROPERTY( quint8 pattern WRITE setPattern READ pattern )
    Q_PROPERTY( QString color WRITE setColor READ color )
    Q_PROPERTY( QString outlineColor WRITE setOutlineColor READ outlineColor )
    Q_PROPERTY( QString backgroundColor WRITE setBackgroundColor READ backgroundColor )
    Q_PROPERTY( QString rolloverTextColor WRITE setRolloverTextColor READ rolloverTextColor )
    Q_PROPERTY( QString rolloverBackgroundColor WRITE setRolloverBackgroundColor READ rolloverBackgroundColor )

    Q_PROPERTY( qreal penWidth WRITE setPenWidth READ penWidth )
    Q_PROPERTY( qint16 startAngle WRITE setStartAngle READ startAngle )
    Q_PROPERTY( qint16 spanAngle WRITE setSpanAngle READ spanAngle )
    Q_PROPERTY( QString visibility WRITE setVisibility READ visibility )
    Q_PROPERTY( qreal rotateX WRITE setXRotation READ xRotation )
    Q_PROPERTY( qreal rotateY WRITE setYRotation READ yRotation )
    Q_PROPERTY( qreal rotateZ WRITE setZRotation READ zRotation )
    Q_PROPERTY( qreal shearH WRITE setHShear READ hShear )
    Q_PROPERTY( qreal shearV WRITE setVShear READ vShear )
    Q_PROPERTY( qreal translateX WRITE setXTranslation READ xTranslation )
    Q_PROPERTY( qreal translateY WRITE setYTranslation READ yTranslation )
    Q_PROPERTY( qreal skewX WRITE setXSkew READ xSkew )
    Q_PROPERTY( qreal skewY WRITE setYSkew READ ySkew )
    Q_PROPERTY( QString cursor WRITE setCursor READ cursor )

public:
    explicit PHIItemStyleCSS( PHIDomItem* );
    virtual ~PHIItemStyleCSS(){}

public slots:
    // CSS:
    inline QString left() const { return QString::number( _it->realX() )+L1( "px" ); }
    inline void setLeft( const QString &x ) { _it->setX( toReal( x ) ); }
    inline QString top() const { return QString::number( _it->realY() )+L1( "px" ); }
    inline void setTop( const QString &y ) { _it->setY( toReal( y ) ); }
    inline qint16 zIndex() const { return _it->realZIndex(); }
    inline void setZIndex( qint16 z ) { _it->setZIndex( z ); }
    inline QString height() const { return QString::number( _it->realHeight() )+L1( "px" ); }
    inline void setHeight( const QString &h ) { _it->setHeight( toReal( h ) ); }
    inline QString width() const { return QString::number( _it->realWidth() )+L1( "px" ); }
    inline void setWidth( const QString &w ) { _it->setWidth( toReal( w ) ); }
    inline void setColor( const QString &c ) {
        _it->setColor( PHIPalette::Foreground, PHIPalette::Custom, PHI::colorFromString( c ) );
        _it->setColor( PHIPalette::WidgetText, PHIPalette::Custom, PHI::colorFromString( c ) );
    }
    inline QString color() const {
        if ( _it->colorForRole( PHIPalette::Foreground ).isValid() ) return _it->colorForRole( PHIPalette::Foreground ).name();
        if ( _it->colorForRole( PHIPalette::WidgetText ).isValid() ) return _it->colorForRole( PHIPalette::WidgetText ).name();
        return QString( L1( "#000000" ) );
    }
    inline void setBackgroundColor( const QString &c ) {
        _it->setColor( PHIPalette::Background, PHIPalette::Custom, PHI::colorFromString( c ) );
        _it->setColor( PHIPalette::WidgetBase, PHIPalette::Custom, PHI::colorFromString( c ) );
    }
    inline QString backgroundColor() const {
        if ( _it->colorForRole( PHIPalette::Background ).isValid() ) return _it->colorForRole( PHIPalette::Background ).name();
        if ( _it->colorForRole( PHIPalette::WidgetBase ).isValid() ) return _it->colorForRole( PHIPalette::WidgetBase ).name();
        return QString( L1( "#000000" ) );
    }
    inline QString visibility() const { if ( _it->realVisible() ) return QString( L1( "visible" ) ); return QString( L1( "hidden" ) ); }
    inline void setVisibility( const QString &s ) {
        if ( s==L1( "visible" ) ) _it->setVisible( true );
        else if ( s==L1( "hidden" ) ) _it->setVisible( false );
    }
    inline void setBorderRadius( const QString &r ) { _it->setProperty( "_borderRadius", toNumber( r ) ); }
    inline QString borderRadius() const { return QString::number( _it->property( "_borderRadius" ).toInt() )+L1( "px" ); }
    inline QString cursor() const { return QString::fromLatin1( PHI::toCursorString( _it->graphicsWidget()->cursor().shape() ) ); }
    inline void setCursor( const QString &c ) { _it->setCursor( PHI::toCursorShape( c.toLatin1() ) ); }

    // PHI extensions
    inline qreal opacity() const { return _it->realOpacity(); }
    inline void setOpacity( qreal o ) { _it->setOpacity( o ); }

    // deprecated
    inline void setPattern( quint8 p ) { _it->setProperty( "_pattern", p ); }
    inline quint8 pattern() const { return _it->property( "_pattern" ).value<quint8>(); }
    inline void setLine( quint8 l ) { _it->setProperty( "_line", l ); }
    inline quint8 line() const { return _it->property( "_line" ).value<quint8>(); }
    inline void setRolloverTextColor( const QString &c ) { _it->setColor( PHIPalette::Hover, PHIPalette::Custom, PHI::colorFromString( c ) ); }
    inline QString rolloverTextColor() const { return _it->colorForRole( PHIPalette::Hover ).name(); }
    inline void setRolloverBackgroundColor( const QString &c ) { _it->setColor( PHIPalette::HoverBackground, PHIPalette::Custom, PHI::colorFromString( c ) ); }
    inline QString rolloverBackgroundColor() const { return _it->colorForRole( PHIPalette::HoverBackground ).name(); }
    inline void setOutlineColor( const QString &c ) { setBackgroundColor( c ); }
    inline QString outlineColor() const { return backgroundColor(); }
    inline void setPenWidth( qreal w ) { _it->setProperty( "_penWidth", w ); }
    inline qreal penWidth() const { return _it->property( "_penWidth" ).toReal(); }
    inline void setSpanAngle( qint16 a ) { _it->setProperty( "_spanAngle", a ); }
    inline qint16 spanAngle() const { return _it->property( "_spanAngle" ).value<qint16>(); }
    inline void setStartAngle( qint16 a ) { _it->setProperty( "_startAngle", a ); }
    inline qint16 startAngle() const { return _it->property( "_startAngle" ).value<qint16>(); }
    inline void setXRotation( qreal xr ) { _it->setXRotation( xr ); }
    inline qreal xRotation() const { return _it->xRotation(); }
    inline void setYRotation( qreal yr ) { _it->setYRotation( yr ); }
    inline qreal yRotation() const { return _it->yRotation(); }
    inline void setZRotation( qreal zr ) { _it->setZRotation( zr ); }
    inline qreal zRotation() const { return _it->zRotation(); }
    inline void setHShear( qreal hs ) { _it->setHSkew( hs ); }
    inline qreal hShear() const { return _it->hSkew(); }
    inline void setVShear( qreal vs ) { _it->setVSkew( vs ); }
    inline qreal vShear() const { return _it->vSkew(); }
    inline void setXTranslation( qreal xt ) { _it->setTransformOrigin( QPointF( xt, _it->transformOrigin().y() ) ); }
    inline qreal xTranslation() const { return _it->transformOrigin().x(); }
    inline void setYTranslation( qreal yt ) { _it->setTransformOrigin( QPointF( _it->transformOrigin().x(), yt ) ); }
    inline qreal yTranslation() const { return _it->transformOrigin().y(); }
    inline void setXSkew( qreal xs ) { _it->setHSkew( xs ); }
    inline qreal xSkew() const { return _it->hSkew(); }
    inline void setYSkew( qreal ys ) { _it->setVSkew( ys ); }
    inline qreal ySkew() const { return _it->vSkew(); }

    inline QStringList properties() const { return PHI::properties( this ); }

protected:
    static int toNumber( const QString &s );
    static qreal toReal( const QString &s );

private:
    PHIBaseItem *_it;
};

inline int PHIItemStyleCSS::toNumber( const QString &s )
{
    QString tmp=s;
    tmp.remove( QRegularExpression( QLatin1String( "\\D" ) ) );
    return tmp.toInt();
}

inline qreal PHIItemStyleCSS::toReal( const QString &s )
{
    QString tmp=s;
    tmp.remove( QRegularExpression( QLatin1String( "\\D" ) ) );
    return tmp.toDouble();
}

#endif // PHIITEMSTYLECSS_H
