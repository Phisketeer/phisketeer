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
#ifndef PHIBASEITEM_H
#define PHIBASEITEM_H

#include <QObject>
#include <QMetaType>
#include <QScriptEngine>
#include <QScriptValue>
#include <QFont>
#include <QColor>
#include <QPalette>
#include <QVariant>
#include <QHash>
#include <QGraphicsEffect>
#include <QStringList>
#include "phiitem.h"
#include "phieffect.h"
#include "phi.h"

class PHIEXPORT PHIBaseItem : public QObject, public PHIItem
{
    Q_OBJECT
    Q_DISABLE_COPY( PHIBaseItem )

    Q_PROPERTY( quint16 type READ type ) // note: wid is internaly stored as quint8
    Q_PROPERTY( QString name READ name )
    Q_PROPERTY( QString id READ name )
    Q_PROPERTY( QString parentName READ parentName )
    Q_PROPERTY( QStringList properties READ properties )
    Q_PROPERTY( QString value WRITE setValue READ value )
    Q_PROPERTY( QString styleSheet WRITE setStyleSheet READ styleSheet )
    Q_PROPERTY( QString title WRITE setToolTip READ toolTip )
    Q_PROPERTY( QString imageId WRITE setImageId READ imageId )
    Q_PROPERTY( QString label WRITE setLabel READ label )
    Q_PROPERTY( QString delimiter WRITE setDelimiter READ delimiter )
    Q_PROPERTY( QString url WRITE setUrl READ url )
    Q_PROPERTY( QString accessKey WRITE setAccessKey READ accessKey )
    Q_PROPERTY( QStringList pictureBookIds WRITE setPictureBookIds READ pictureBookIds )
    Q_PROPERTY( bool disabled WRITE setDisabled READ disabled )
    Q_PROPERTY( bool checked WRITE setChecked READ checked )
    Q_PROPERTY( bool readOnly WRITE setReadOnly READ readOnly )
    Q_PROPERTY( qint32 fontSize WRITE setFontSize READ fontSize )
    Q_PROPERTY( quint8 align WRITE setAlignment READ alignment )
    Q_PROPERTY( quint16 maxLength WRITE setMaxLength READ maxLength )
    Q_PROPERTY( quint16 tabIndex WRITE setTabIndex READ tabIndex )
    Q_PROPERTY( quint32 fadeTime WRITE setFadeTime READ fadeTime )
    Q_PROPERTY( quint32 fadeInterval WRITE setFadeInterval READ fadeInterval )

    //friend class PHIBaseStyle;
    friend PHIEXPORT QDataStream& operator<<( QDataStream&, const PHIBaseItem* );
    friend PHIEXPORT QDataStream& operator>>( QDataStream&, PHIBaseItem* );

public: //not useable by script engine
    PHIBaseItem( QObject *parent=0 );
    PHIBaseItem( const PHIItem&, QObject *parent=0 );
    PHIBaseItem( PHI::Widget wid, const QByteArray &id, QObject *parent=0 );
    virtual ~PHIBaseItem();
    virtual QGradient gradient() const;

    inline PHIRectHash gridLayoutInfo() const { return _variants.value( DGridLayoutInfo ).value<PHIRectHash>(); }
    inline PHIByteArrayList childIds() const { return _variants.value( DChildIds ).value<PHIByteArrayList>(); }
    inline bool isChild() const { return _attributes & AChild; }
    inline bool isTemplateItem() const { return _attributes & ATemplateItem; }
    inline PHIEffect::Effects effects() const { return _effect->effects(); }
    inline const PHIEffect* effect() const { return _effect; }
    inline QByteArray pageId() const { return _pageId; }
    inline void setPageId( const QByteArray &id ) { _pageId=id; }
    inline bool hasGraphicEffect() const { return _effect->effects() & PHIEffect::EGraphics; }
    inline quint8 transformPosition() const { return _transformPos; }
    inline QByteArray valueData() const { return _variants.value( DText ).toByteArray(); }
    inline QByteArray imageIdData() const { return _variants.value( DImage ).toByteArray(); }
    inline QByteArray urlData() const { return _variants.value( DUrl ).toByteArray(); }
    inline QByteArray labelData() const { return _variants.value( DLabel ).toByteArray(); }
    inline QByteArray toolTipData() const { return _variants.value( DToolTip ).toByteArray(); }
    inline quint8 dragHotSpotType() const { return _variants.value( DDragHotSpotType, 0 ).value<quint8>(); }
    inline QPoint dragHotSpot() const { return _variants.value( DDragHotSpot, PHI::defaultHotSpot() ).toPoint(); }

    virtual void setPenWidth( qreal w );
    virtual void setColor( const QColor &c );
    virtual void setOutlineColor( const QColor &c );
    virtual void setRolloverTextColor( const QColor &c );
    virtual void setRolloverBackgroundColor( const QColor &c );
    virtual void setPattern( quint8 p );
    virtual void setLine( quint8 l );
    virtual void setStartAngle( qint16 a );
    virtual void setSpanAngle( qint16 a );
    virtual void setAlignment( quint8 a );

    //set by PHIEffects
    inline virtual void clearEffects() { _effect->clearAll(); }
    inline virtual void setFadeIn( qint32 start, qint32 duration, qreal maxOpac, const QString &ease ) {
        _effect->setFadeIn( start, duration, maxOpac, PHI::toEasingCurveType( ease ) ); }
    inline virtual void fadeIn( qint32 &start, qint32 &duration, qreal &maxOpac, quint8 &ease ) const {
        _effect->fadeIn( start, duration, maxOpac, ease ); }
    inline virtual void setFadeOut( qint32 start, qint32 duration, qreal minOpac, const QString &ease ) {
        _effect->setFadeOut( start, duration, minOpac, PHI::toEasingCurveType( ease ) ); }
    inline virtual void fadeOut( qint32 &start, qint32 &duration, qreal &minOpac, quint8 &ease ) const {
        _effect->fadeOut( start, duration, minOpac, ease ); }
    inline virtual void setShadow( const QColor &c, qreal xOff, qreal yOff, qreal radius ) {
        _effect->setShadow( c, xOff, yOff, radius ); }
    inline virtual void setSurface( qreal yOff, qreal size ) {
        _effect->setSurface( yOff, size ); }
    inline virtual void setBlur( qreal radius ) {
        _effect->setBlur( radius ); }
    inline virtual void setColorize( const QColor &c, qreal strength ) {
        _effect->setColorize( c, strength ); }
    inline virtual void setMoveTo( qint32 start, qint32 duration, qint32 left, qint32 top, const QString &ease ) {
        _effect->setMoveTo( start, duration, left, top, PHI::toEasingCurveType( ease ) ); }
    inline virtual void setMoveBy( qint32 start, qint32 duration, qint32 x, qint32 y,
            qint32 w, qint32 h, const QString &ease ) {
        _effect->setMoveBy( start, duration, x, y, w, h, PHI::toEasingCurveType( ease ) ); }

    inline virtual void setRotateIn( quint8 axis, qint32 start, qint32 duration, const QString& ) {
        _effect->setRotateIn( axis, start, duration); }
    inline virtual void setRotateOut( quint8 axis, qint32 start, qint32 duration, const QString& ) {
        _effect->setRotateOut( axis, start, duration ); }
    inline virtual void setRotate( quint8 axis, qreal stepX, qreal stepY, qreal stepZ, const QString& ) {
        _effect->setRotate( axis, stepX, stepY, stepZ ); }
    inline virtual void stopAnimations() {;}

    //misc
    inline virtual void resetItem() {;}
    inline virtual qint32 fontSize() const { return font().pointSize(); }
    inline virtual void setFontSize( qint32 ps ) { QFont f=font(); f.setPointSize( ps ); setFont( f ); }
    inline virtual QFont font( const QFont &f=PHI::invalidFont() ) const { return _variants.value( DFont, f ).value<QFont>(); }
    inline virtual void setFont( const QFont &f ) { _properties|=PHIItem::PFont; _variants.insert( DFont, f ); }
    //inline virtual QPalette palette( const QPalette &p=QPalette() ) const { return _variants.value( DPalette, p ).value<QPalette>(); }
    //inline virtual void setPalette( const QPalette &p ) { _properties|=PHIItem::PPalette; _variants.insert( DPalette, p ); }

    // functions offered by style property
    inline virtual qreal penWidth() const { return static_cast<qreal>(_variants.value( DPenWidth, 1. ).toDouble()); }
    inline virtual QColor color() const {
        return _variants.value( DColor, QColor() ).value<QColor>(); }
    inline virtual QColor outlineColor() const {
        return _variants.value( DOutlineColor, QColor() ).value<QColor>(); }
    inline virtual QColor rolloverTextColor() const {
        return _variants.value( DRolloverTextColor, QColor() ).value<QColor>(); }
    inline virtual QColor rolloverBackgroundColor() const {
        return _variants.value( DRolloverBackgroundColor, QColor() ).value<QColor>(); }
    inline virtual quint8 pattern() const { return _variants.value( DPattern, 1 ).value<quint8>(); }
    inline virtual quint8 line() const { return _variants.value( DLine, 0 ).value<quint8>(); }
    inline virtual qint16 startAngle() const { return _variants.value( DStartAngle, 0 ).value<qint16>(); }
    inline virtual qint16 spanAngle() const { return _variants.value( DSpanAngle, 5760 ).value<qint16>(); }
    inline virtual bool visible() const { return _attributes & AVisible; }
    inline virtual void setVisible( bool b ) { b ? _attributes|= AVisible : _attributes&= ~AVisible; }
    inline virtual quint8 alignment() const { return _variants.value( DAlignment, 0 ).value<quint8>(); }

    inline qreal gradientAngle() const { return _variants.value( PHIItem::DAngle ).value<qreal>(); }
    inline qreal gradientRadius() const { return _variants.value( PHIItem::DRadius ).value<qreal>(); }
    inline virtual qint16 borderRadius() const { return _variants.value( DBorderRadius, 15 ).value<qint16>(); }
    inline virtual void setBorderRadius( qint16 r ) { _variants.insert( DBorderRadius, r ); }
    inline QByteArray cursor() const { return _variants.value( DCursor, QByteArray( "auto" ) ).toByteArray(); }
    inline virtual void setCursor( const QByteArray &c ) { _variants.insert( DCursor, c ); }

public slots: //useable by script engine
    // inline qint32 attributes() const { return static_cast<qint32>(_attributes); }
    inline QString name() const { return QString::fromUtf8( _id ); }
    inline QString parentName() const { return QString::fromUtf8( _parent ); }
    inline void setParentName( const QString &pid ) { _parent=pid.toUtf8(); }
    inline quint16 type() const { return static_cast<quint16>(_wid); }
    inline virtual QStringList properties() const { return PHI::properties( this ); }

    inline QString imageId() const {
        return QString::fromUtf8( _variants.value( DImage ).toByteArray() ); }
    inline virtual void setImageId( const QString &id ) { _variants.insert( DImage, id.toUtf8() ); }
    inline QStringList pictureBookIds() const { return _variants.value( DImageBook ).toStringList(); }
    inline virtual void setPictureBookIds( const QStringList &l ) { _variants.insert( DImageBook, l ); }

    inline bool disabled() const { return _attributes & ADisabled; }
    inline virtual void setDisabled( bool b ) { b ? _attributes|= ADisabled : _attributes&= ~ADisabled; }
    inline bool checked() const { return _attributes & AChecked; }
    inline virtual void setChecked( bool b ) { b ? _attributes|= AChecked : _attributes&= ~AChecked; }
    inline bool readOnly() const { return _attributes & AReadOnly; }
    inline virtual void setReadOnly( bool b ) { b ? _attributes|= AReadOnly : _attributes&= ~AReadOnly; }
    inline QString value() const { return QString::fromUtf8( _variants.value( DText ).toByteArray() ); }
    inline virtual void setValue( const QString &s ) { _variants.insert( DText, s.toUtf8() ); }
    inline QString toolTip() const { return QString::fromUtf8( _variants.value( DToolTip ).toByteArray() ); }
    inline virtual void setToolTip( const QString &s ) { _variants.insert( DToolTip, s.toUtf8() ); }
    inline QString styleSheet() const { return QString::fromUtf8( _variants.value( DStyleSheet ).toByteArray() ); }
    inline virtual void setStyleSheet( const QString &s ) { _variants.insert( DStyleSheet, s.toUtf8() ); }
    inline QString label() const { return QString::fromUtf8( _variants.value( DLabel ).toByteArray() ); }
    inline virtual void setLabel( const QString &s ) { _variants.insert( DLabel, s.toUtf8() ); }
    inline QString url() const { return QString::fromUtf8( _variants.value( DUrl ).toByteArray() ); }
    inline virtual void setUrl( const QString &s ) { _variants.insert( DUrl, s.toUtf8() ); }
    inline QString accessKey() const { return QString::fromUtf8( _variants.value( DShortCut ).toByteArray() ); }
    inline virtual void setAccessKey( const QString &s ) { _variants.insert( DShortCut, s.left(1).toUtf8() ); }
    inline quint16 maxLength() const { return _variants.value( DMaxSize, PHI::maxLength() ).value<quint16>(); }
    inline virtual void setMaxLength( quint16 max ) { _variants.insert( DMaxSize, max ); }
    inline QString delimiter() const { return _variants.value( DDelimiter, QString( "\n" ) ).toString(); }
    inline virtual void setDelimiter( const QString &d ) { _variants.insert( DDelimiter, d ); }
    inline quint16 tabIndex() const { return _variants.value( DTabOrder, 0 ).value<quint16>(); }
    inline virtual void setTabIndex( quint16 tab ) { _variants.insert( DTabOrder, tab ); }
    inline qint16 fadeTime() const { return _variants.value( DFadeTime, 2 ).value<qint16>(); }
    inline virtual void setFadeTime( qint16 ft ) { _variants.insert( DFadeTime, ft ); }
    inline virtual qint16 fadeInterval() const { return _variants.value( DFadeInterval, 4 ).value<qint16>(); }
    inline virtual void setFadeInterval( qint16 fp ) { _variants.insert( DFadeInterval, fp ); }

    inline bool droppable() const { return _attributes & ADroppable; }
    inline virtual void setDropable( bool b ) { b ? _attributes |= ADroppable : _attributes &= ~ADroppable; }
    inline bool draggable() const { return _attributes & ADraggable; }
    inline virtual void setDragable( bool b ) { b ? _attributes |= ADraggable : _attributes &= ~ADraggable; }
    inline qint16 dragHotSpotX() const { return _variants.value( DDragHotSpot, PHI::defaultHotSpot() ).toPoint().x(); }
    inline qint16 dragHotSpotY() const { return _variants.value( DDragHotSpot, PHI::defaultHotSpot() ).toPoint().y(); }
    inline virtual void setDragHotSpot( int x, int y ) { _variants.remove( DDragHotSpotType );
        _variants.insert( DDragHotSpot, QPoint( x, y ) ); }
    inline bool dragMoveAction() const {
        return _variants.value( DDragDropOptions, 0 ).value<qint32>() & DDMoveAction; }
    inline virtual void setDragMoveAction( bool b ) {
        qint32 o=_variants.value( DDragDropOptions, 0 ).value<qint32>();
        b ? o |= DDMoveAction : o &= ~DDMoveAction; _variants.insert( DDragDropOptions, o ); }
    inline bool dragRevertOnIgnore() const {
        return _variants.value( DDragDropOptions, 0 ).value<qint32>() & DDRevertOnIgnore; }
    inline virtual void setDragRevertOnIgnore( bool b ) {
        qint32 o=_variants.value( DDragDropOptions, 0 ).value<qint32>();
        b ? o |= DDRevertOnIgnore : o &= ~DDRevertOnIgnore; _variants.insert( DDragDropOptions, o ); }
    inline bool dragRevertOnAccept() const {
        return _variants.value( DDragDropOptions, 0 ).value<qint32>() & DDRevertOnAccept; }
    inline virtual void setDragRevertOnAccept( bool b ) {
        qint32 o=_variants.value( DDragDropOptions, 0 ).value<qint32>();
        b ? o |= DDRevertOnAccept : o &= ~DDRevertOnAccept; _variants.insert( DDragDropOptions, o ); }
    inline qreal dragDistance() const {
        return _variants.value( DDragDistance, PHI::defaultDragDistance() ).toReal(); }
    inline virtual void setDragDistance( qreal d ) { _variants.insert( DDragDistance, d ); }
    inline QStringList dropAcceptedIds() const { return _variants.value( DDropAcceptedIds ).toStringList(); }
    inline virtual void setDropAcceptedIds( const QStringList &list ) { _variants.insert( DDropAcceptedIds, list ); }
    inline void setDragOpacity( qreal o ) { _variants.insert( DDragOpacity, qBound( 0., o, 1. ) ); }
    inline qreal dragOpacity() const { return _variants.value( DDragOpacity, 1. ).toReal(); }
    inline bool dropHighlightItem() const {
        return _variants.value( DDragDropOptions, 0 ).value<qint32>() & DDHighlightOnMouseOver; }
    inline virtual void setDropHighlightItem( bool b ) {
        qint32 o=_variants.value( DDragDropOptions, 0 ).value<qint32>();
        b ? o |= DDHighlightOnMouseOver : o &= ~DDHighlightOnMouseOver;
        _variants.insert( DDragDropOptions, o ); }

protected:
    PHIEffect *_effect;
    QByteArray _pageId;
};

Q_DECLARE_METATYPE( PHIBaseItem* );
Q_DECLARE_METATYPE( const PHIBaseItem* );

PHIEXPORT QScriptValue baseItemToScriptValue( QScriptEngine*, PHIBaseItem* const &in );
PHIEXPORT void baseItemFromScriptValue( const QScriptValue&, PHIBaseItem* &out );

PHIEXPORT QDataStream& operator<<( QDataStream&, const PHIBaseItem* );
PHIEXPORT QDataStream& operator>>( QDataStream&, PHIBaseItem* );

class PHIBaseStyle : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( PHIBaseStyle )

    Q_PROPERTY( QString top WRITE setTop READ top )
    Q_PROPERTY( QString left WRITE setLeft READ left )
    Q_PROPERTY( QString width WRITE setWidth READ width )
    Q_PROPERTY( QString height WRITE setHeight READ height )
    Q_PROPERTY( qreal opacity WRITE setOpacity READ opacity )
    Q_PROPERTY( qint16 zIndex WRITE setZIndex READ zIndex )
    Q_PROPERTY( QString borderRadius WRITE setBorderRadius READ borderRadius )
    Q_PROPERTY( quint8 line WRITE setLine READ line )
    Q_PROPERTY( quint8 pattern WRITE setPattern READ pattern )
    Q_PROPERTY( QStringList properties READ properties )
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
    PHIBaseStyle( PHIBaseItem* );
    virtual ~PHIBaseStyle();

public slots:
    // offered by CSS 2.1 / 3.0
    inline QString left() const { return QString::number( _it->x() )+"px"; }
    inline void setLeft( const QString &x ) { QString xx=x; _it->setX( xx.replace( "px", "" ).toDouble() ); }
    inline QString top() const { return QString::number( _it->y() )+"px"; }
    inline void setTop( const QString &y ) { QString yy=y; _it->setY( yy.replace( "px", "" ).toDouble() ); }
    inline qint16 zIndex() const { return _it->zValue(); }
    inline void setZIndex( qint16 z ) { if ( z <= PHI::maxZValue() ) _it->setZValue( z ); }
    inline QString height() const { return QString::number( _it->height() )+"px"; }
    inline void setHeight( const QString &h ) { QString hh=h; _it->setHeight( hh.replace( "px", "" ).toDouble() ); }
    inline QString width() const { return QString::number( _it->width() )+"px"; }
    inline void setWidth( const QString &w ) { QString ww=w; _it->setWidth( ww.replace( "px", "" ).toDouble() ); }
    inline void setColor( const QString &c ) { _it->setColor( c ); }
    inline QString color() const { return _it->color().name(); }
    inline void setBackgroundColor( const QString &c ) { _it->setOutlineColor( c ); }
    inline QString backgroundColor() const { return _it->outlineColor().name(); }
    QString visibility() const;
    void setVisibility( const QString& );
    inline void setBorderRadius( const QString &r ) {
        QString rr=r; _it->setBorderRadius( rr.replace( "px", "" ).toShort() ); }
    inline QString borderRadius() const { return QString::number( _it->borderRadius() )+"px"; }
    inline QString cursor() const { return QString::fromLatin1( _it->cursor() ); }
    inline void setCursor( const QString &c ) { _it->setCursor( c.toLatin1() ); }

    // PHI extensions
    inline qreal opacity() const { return _it->opacity(); }
    inline void setOpacity( qreal o ) { _it->setOpacity( o ); }
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

    inline QStringList properties() const { return PHI::properties( this ); }

protected:
    PHIBaseItem *_it;
};

class PHIBaseEffect : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( PHIBaseEffect )

    Q_PROPERTY( quint8 xAxis READ xAxis )
    Q_PROPERTY( quint8 yAxis READ yAxis )
    Q_PROPERTY( quint8 zAxis READ zAxis )
    Q_PROPERTY( QStringList properties READ properties )

public:
    PHIBaseEffect( PHIBaseItem* );
    virtual ~PHIBaseEffect();

    // Available by script engine
public slots:
    inline quint8 xAxis() const { return 0x1; }
    inline quint8 yAxis() const { return 0x2; }
    inline quint8 zAxis() const { return 0x4; }
    inline QStringList properties() const { return PHI::properties( this ); }
    inline void fadeIn( qint32 start=0, qint32 duration=1000, qreal maxOpac=1.,
        const QString &ease=PHI::defaultEasingCurve() )
        { _it->setFadeIn( start, duration, maxOpac, ease ); }
    inline void fadeOut( qint32 start=0, qint32 duration=1000, qreal minOpac=0.,
        const QString &ease=PHI::defaultEasingCurve() )
        { _it->setFadeOut( start, duration, minOpac, ease ); }
    void shadow( const QString &color=QString( "#3F3F3F" ), qreal opac=.7, qreal xOff=8.,
        qreal yOff=8., qreal radius=1. );
    inline void surface( qreal yOff=0., qreal size=30. ) { _it->setSurface( yOff, size ); }
    inline void blur( qreal radius=5. ) { _it->setBlur( radius ); }
    inline void colorize( const QString &c=QString( "#0000C0" ), qreal strength=1. ) {
        _it->setColorize( QColor( c ), strength ); }
    inline void moveTo( qint32 left, qint32 top, qint32 start=0, qint32 duration=1000,
        const QString &ease=PHI::defaultEasingCurve() )
        { _it->setMoveTo( start, duration, left, top, ease ); }
    inline void rotateIn( quint8 axis=0x2, qint32 start=0, qint32 duration=1000,
        const QString &ease=PHI::defaultEasingCurve() )
        { _it->setRotateIn( axis, start, duration, ease ); }
    inline void rotateOut( quint8 axis=0x2, qint32 start=0, qint32 duration=1000,
        const QString &ease=PHI::defaultEasingCurve() )
        { _it->setRotateOut( axis, start, duration, ease ); }
    void rotate( quint8 axis=0x4, qreal step=1., const QString &ease=PHI::defaultEasingCurve() );
    inline void clear() { _it->clearEffects(); }
    inline void stopAnimations() { _it->stopAnimations(); }

protected:
    PHIBaseItem *_it;
};

#endif // PHIBASEITEM_H
