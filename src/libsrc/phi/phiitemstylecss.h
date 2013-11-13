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
#ifndef PHIITEMSTYLECSS_H
#define PHIITEMSTYLECSS_H

#include <QObject>
#include <QRegularExpression>
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
    virtual ~PHIDomEffect() { qDebug( "PHIDomEffect()::~PHIDomEffect()" ); }

public slots:
    inline quint8 xAxis() const { return 0x1; }
    inline quint8 yAxis() const { return 0x2; }
    inline quint8 zAxis() const { return 0x4; }
    inline QStringList properties() const { return PHI::properties( this ); }
    inline void fadeIn( qint32 start=0, qint32 duration=1000, qreal maxOpac=1.,
        const QString &ease=PHI::defaultEasingCurve() )
        { _it->effect()->setFadeIn( start, duration, maxOpac, PHI::toEasingCurveType( ease ) ); }
    inline void fadeOut( qint32 start=0, qint32 duration=1000, qreal minOpac=0.,
        const QString &ease=PHI::defaultEasingCurve() )
        { _it->effect()->setFadeOut( start, duration, minOpac, PHI::toEasingCurveType( ease ) ); }
    inline void shadow( const QString &color=QString::fromLatin1( "#3F3F3F" ), qreal opac=.7, qreal xOff=8.,
        qreal yOff=8., qreal radius=1. ) { QColor c( color ); c.setAlphaF( qBound( 0., opac, 1. ) );
        _it->effect()->setShadow( QColor( color ), xOff, yOff, radius ); }
    // deprecated:
    inline void surface( qreal yOff=0., qreal size=30. ) { _it->effect()->setReflection( yOff, size ); }
    inline void reflection( qreal yOff=0., qreal size=30. ) { _it->effect()->setReflection( yOff, size ); }
    inline void blur( qreal radius=5. ) { _it->effect()->setBlur( radius ); }
    inline void colorize( const QString &c=QString::fromLatin1( "#0000C0" ), qreal strength=1. ) {
        _it->effect()->setColorize( QColor( c ), strength ); }
    inline void moveTo( qint32 left, qint32 top, qint32 start=0, qint32 duration=1000,
        const QString &ease=PHI::defaultEasingCurve() )
        { _it->moveTo( start, duration, left, top, ease ); }
    inline void rotateIn( quint8 axis=0x2, qint32 start=0, qint32 duration=1000,
        const QString &ease=PHI::defaultEasingCurve() )
        { _it->rotateIn( axis, start, duration, ease ); }
    inline void rotateOut( quint8 axis=0x2, qint32 start=0, qint32 duration=1000,
        const QString &ease=PHI::defaultEasingCurve() )
        { _it->effect()->setRotateOut( axis, start, duration, PHI::toEasingCurveType( ease ) ); }
    inline void rotate( quint8 axis=0x4, qreal stepx=0, qreal stepy=0, qreal stepz=1. )
        { _it->rotate( axis, stepx, stepy, stepz ); }
    inline void clear() { _it->clearEffects(); }
    inline void stopAnimations() { /* @todo: stop anim */ }

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
    /*
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
    */

public:
    explicit PHIItemStyleCSS( PHIDomItem* );
    virtual ~PHIItemStyleCSS(){ qDebug( "PHIItemStyleCSS::~PHIItemStyleCSS()" ); }

public slots:
    // offered by CSS 2.1 / 3.0
    inline QString left() const { return QString::number( _it->realX() )+QLatin1String( "px" ); }
    inline void setLeft( const QString &x ) { _it->setX( toReal( x ) ); }
    inline QString top() const { return QString::number( _it->realY() )+QLatin1String( "px" ); }
    inline void setTop( const QString &y ) { _it->setY( toReal( y ) ); }
    inline qint16 zIndex() const { return _it->realZIndex(); }
    inline void setZIndex( qint16 z ) { _it->setZIndex( z ); }
    inline QString height() const { return QString::number( _it->realHeight() )+QLatin1String( "px" ); }
    inline void setHeight( const QString &h ) { _it->setHeight( toReal( h ) ); }
    inline QString width() const { return QString::number( _it->realWidth() )+QLatin1String( "px" ); }
    inline void setWidth( const QString &w ) { _it->setWidth( toReal( w ) ); }
    /*
    inline void setColor( const QString &c ) { _it->setColor( c ); }
    inline QString color() const { return _it->color().name(); }
    inline void setBackgroundColor( const QString &c ) { _it->setOutlineColor( c ); }
    inline QString backgroundColor() const { return _it->outlineColor().name(); }
    QString visibility() const;
    void setVisibility( const QString& );
    inline void setBorderRadius( const QString &r ) {
        QString rr=r; _it->setBorderRadius( rr.replace( QStringLiteral( "px" ), QString() ).toShort() ); }
    inline QString borderRadius() const { return QString::number( _it->borderRadius() )+QLatin1String( "px" ); }
    inline QString cursor() const { return QString::fromLatin1( _it->cursor() ); }
    inline void setCursor( const QString &c ) { _it->setCursor( c.toLatin1() ); }
*/
    // PHI extensions
    inline qreal opacity() const { return _it->realOpacity(); }
    inline void setOpacity( qreal o ) { _it->setOpacity( o ); }
/*
    inline void setPattern( quint8 p ) { _it->setPattern( p ); }
    inline quint8 pattern() const { return _it->pattern(); }
    inline void setLine( quint8 l ) { _it->setLine( l ); }
    inline quint8 line() const { return _it->line(); }
    inline void setRolloverTextColor( const QString &c ) { _it->setRolloverTextColor( c ); }
    inline QString rolloverTextColor() const { return _it->rolloverTextColor().name(); }
    inline void setRolloverBackgroundColor( const QString &c ) { _it->setRolloverBackgroundColor( c ); }
    inline QString rolloverBackgroundColor() const { return _it->rolloverBackgroundColor().name(); }
    inline void setOutlineColor( const QString &c ) { _it->setOutlineColor( c ); }
    inline QString outlineColor() const { return _it->outlineColor().name(); }
    inline void setPenWidth( qreal w ) { _it->setPenWidth( w ); }
    inline qreal penWidth() const { return _it->penWidth(); }
    inline void setSpanAngle( qint16 a ) { _it->setSpanAngle( a ); }
    inline qint16 spanAngle() const { return _it->spanAngle(); }
    inline void setStartAngle( qint16 a ) { _it->setStartAngle( a ); }
    inline qint16 startAngle() const { return _it->startAngle(); }
    inline void setXRotation( qreal xr ) { _it->setXRotation( xr ); }
    inline qreal xRotation() const { return _it->xRotation(); }
    inline void setYRotation( qreal yr ) { _it->setYRotation( yr ); }
    inline qreal yRotation() const { return _it->yRotation(); }
    inline void setZRotation( qreal zr ) { _it->setZRotation( zr ); }
    inline qreal zRotation() const { return _it->zRotation(); }
    inline void setHShear( qreal hs ) { _it->setHShear( hs ); }
    inline qreal hShear() const { return _it->hShear(); }
    inline void setVShear( qreal vs ) { _it->setVShear( vs ); }
    inline qreal vShear() const { return _it->vShear(); }
    inline void setXTranslation( qreal xt ) { _it->setXTranslation( xt ); }
    inline qreal xTranslation() const { return _it->xTranslation(); }
    inline void setYTranslation( qreal yt ) { _it->setYTranslation( yt ); }
    inline qreal yTranslation() const { return _it->yTranslation(); }
    inline void setXSkew( qreal xs ) { _it->setHShear( xs ); }
    inline qreal xSkew() const { return _it->hShear(); }
    inline void setYSkew( qreal ys ) { _it->setVShear( ys ); }
    inline qreal ySkew() const { return _it->vShear(); }
    */
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
