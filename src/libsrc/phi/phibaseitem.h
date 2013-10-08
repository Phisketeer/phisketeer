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
#ifndef PHIBASEITEM_H
#define PHIBASEITEM_H

#include <QObject>
/*
#include "phieffect.h"
*/
#include <QScriptValue>
#include <QScriptEngine>
#include <QDataStream>
#include <QGraphicsProxyWidget>
#include <QPixmap>
#include "phi.h"
#include "phipalette.h"
#include "phieffect.h"
#include "phidatasources.h"

class QGraphicsSceneEvent;
class QKeyEvent;
class PHIBasePage;
class PHIGraphicsItem;
class PHISRequest;
class PHISDataParser;
class PHIConfigWidget;

class PHIEXPORT PHIBaseItemPrivate
{
public:
    enum Type { TUndefined=0, TIDEItem=1, TTemplateItem=2, TServerItem=3,
        TServerParserItem=4, TClientItem=5 };
    explicit PHIBaseItemPrivate( Type type, PHIBasePage *page, PHIGraphicsItem *gw )
        : _type( type ), _page( page ), _gw( gw ) {}
    explicit PHIBaseItemPrivate( const PHIBasePage *page=0 ); // ctor for IDE only

    inline PHIBasePage* page() const { return _page; }
    inline PHIGraphicsItem* gw() const { return _gw; }
    inline Type type() const { return _type; }

private:
    Type _type;
    PHIBasePage *_page;
    PHIGraphicsItem *_gw;
};

class PHIEXPORT PHIBaseItem : public QObject
{
    friend class PHIGraphicsItem;
    friend class PHIGraphicsScene;
    friend class PHIAbstractLayoutItem;
    friend class PHIAGraphicsItem;
    friend class PHIAGraphicsScene;
    friend class ARTGraphicsScene;
    friend class ARTGraphicsItem;
    friend class ARTUndoData;
    friend class ARTUndoEffect;
    friend class ARTUndoAddLayout;
    friend class ARTUndoDelLayout;
    friend class ARTItemSettings;
    friend class ARTItemProperties;
    friend class ARTItemCSS;

    Q_OBJECT
    Q_PROPERTY( QString id READ name )
    Q_PROPERTY( QString name READ name )
    Q_PROPERTY( quint16 type READ type ) // type==WID (to keep old code working)
    Q_PROPERTY( QStringList properties READ properties )
    Q_PROPERTY( QString parentName READ parentName WRITE setParentName )
    Q_PROPERTY( qreal x READ x WRITE setX )
    Q_PROPERTY( qreal y READ y WRITE setY )
    Q_PROPERTY( qreal width READ width WRITE setWidth )
    Q_PROPERTY( qreal height READ height WRITE setHeight )
    Q_PROPERTY( qint16 tabIndex READ tabIndex WRITE setTabIndex NOTIFY tabIndexChanged )
    Q_PROPERTY( qreal opacity READ opacity WRITE setOpacity NOTIFY opacityChanged )
    Q_PROPERTY( QString title READ title WRITE setTitle NOTIFY titleChanged )
    Q_PROPERTY( qint16 zIndex READ zIndex WRITE setZIndex NOTIFY zIndexChanged )
    Q_PROPERTY( quint8 transformPos READ transformPos WRITE setTransformPos )
    Q_PROPERTY( QPointF transformOrigin READ transformOrigin WRITE setTransformOrigin )
    Q_PROPERTY( QPointF pos READ pos WRITE setPos )
    Q_PROPERTY( QFont font READ font WRITE setFont SCRIPTABLE false )
    Q_PROPERTY( QString styleSheet READ styleSheet WRITE setStyleSheet )

public:
    enum Wid { Unknown=0, User=1000 };
    enum DataType { DOpacity=-1, DTitle=-2, DFont=-3, DTabIndex=-4, DTransformPos=-5,
        DTransformOrigin=-6, DXRot=-7, DYRot=-8, DZRot=-9, DHSkew=-10, DVSkew=-11,
        DParentId=-12, DFlags=-13, DVisibility=-14, DStyleSheet=-15,
        DGradientType=-23, DGradientStartPoint=-24, DGradientStopPoints=-25,
        DGradientFinalStopPoint=-26, DGradientSpreadType=-27, DGradientAngle=-28,
        DGradientCenterPoint=-29, DGradientFocalPoint=-30, DGradientRadius=-31 };
    enum Flag { FNone=0x0, FChild=0x1, FDoNotCache=0x2, FUseStyleSheet=0x4,
        FStoreTitleData=0x8, FStoreVisibleData=0x10, FChecked=0x20, FReadOnly=0x40,
        FDisabled=0x80, FStoreEffectData=0x100, FLayoutHeader=0x200 }; //quint32
#ifdef PHIDEBUG
    Q_DECLARE_FLAGS( Flags, Flag )
#else
    typedef quint32 Flags;
#endif
    /*
    enum Widget {
        FILE_BUTTON=9, HIDDEN=11, IMAGE_BUTTON=12,
        LINE=15, IMAGE=16
        LINK=23, TAB=24, HSPACE=26, VSPACE=27, TEXT=29,
        LANG_SELECTOR=33, LAYOUT_DELIVERY=35,
        ROLLOVER_BUTTON=37,
        RICH_TEXT=41, SVG=42, CHECK_LIST=43, DIA_SHOW=44, IMAGE_BOOK=45, TABLE=46
        HTML_DOC=49, SEARCH=50, FACEBOOK_LIKE=55, GOOGLE_STATIC_MAP=56,
        GOOGLE_PLUS=57, TWITTER=58, PROGRESSBAR=59, YOUTUBE=60, CANVAS=61, GOOGLE_CALENDAR=62, GOOGLE_MAPS=63
    */
    explicit PHIBaseItem( const PHIBaseItemPrivate &p );
    PHIBaseItem( const PHIBaseItem &it );
    virtual ~PHIBaseItem();

signals:
    void zIndexChanged( qint16 );
    void tabIndexChanged( qint16 );
    void titleChanged( QString );
    void opacityChanged( qreal );

public: // not usable by script engine
    void setId( const QString &id );
    inline QByteArray id() const { return _id; }
    inline QByteArray parentId() const { return _parentId; }
    inline void setId( const QByteArray &id ) { setObjectName( QString::fromLatin1( id ) ); _id=id; }
    inline void setParentId( const QString &pid ) { _parentId=pid.toLatin1(); }
    inline void setParentId( const QByteArray &pid ) { _parentId=pid; }
    inline void setFocus() { if ( _gw ) { _gw->setFocus(); _gw->setSelected( true ); } }
    inline const PHIEffect* effect() const { return _effect; }
    inline bool hasGraphicEffect() const { return _effect->effects() & PHIEffect::EGraphics; }

    inline qint16 zIndex() const { return _zIndex; }
    inline QRectF rect() const { return QRectF( QPointF(), size() ); }
    inline quint8 transformPos() const { return _variants.value( DTransformPos, 1 ).value<quint8>(); }
    inline void setTransformPos( PHI::Origin o ) { setTransformPos( static_cast<quint8>(o) ); }
    inline QPointF transformOrigin() const { return _transformOrigin; }
    inline void setXRotation( qreal x ) { _xRot=x; if ( _gw ) _gw->setTransform( computeTransformation() ); }
    inline void setYRotation( qreal y ) { _yRot=y; if ( _gw ) _gw->setTransform( computeTransformation() ); }
    inline void setZRotation( qreal z ) { _zRot=z; if ( _gw ) _gw->setTransform( computeTransformation() ); }
    inline void setHSkew( qreal h ) { _hSkew=h; if ( _gw ) _gw->setTransform( computeTransformation() ); }
    inline void setVSkew( qreal v ) { _vSkew=v; if ( _gw ) _gw->setTransform( computeTransformation() ); }
    inline qreal xRotation() const { return _xRot; }
    inline qreal yRotation() const { return _yRot; }
    inline qreal zRotation() const { return _zRot; }
    inline qreal hSkew() const { return _hSkew; }
    inline qreal vSkew() const { return _vSkew; }
    inline bool hasTransformation() const { return _xRot+_yRot+_zRot+_hSkew+_vSkew? true : false; }
    inline bool isIdeItem() const { return _type==PHIBaseItemPrivate::TIDEItem; }
    inline bool isTemplateItem() const { return _type==PHIBaseItemPrivate::TTemplateItem; }
    inline bool isClientItem() const { return _type==PHIBaseItemPrivate::TClientItem; }
    inline bool isServerItem() const { return _type==PHIBaseItemPrivate::TServerItem; }
    inline bool isGuiItem() const { return _gw ? true : false; }
    inline bool isChild() const { return _flags & FChild; }
    inline Flags flags() const { return _flags; }
    inline PHIKeyHash data() { storeFlags(); return _variants; }
    inline void setData( const PHIKeyHash &data ) { _variants=data; _flags=static_cast<Flags>(_variants.value( DFlags, 0 ).value<quint32>()); }
    QFont font() const;
    QGradient gradient() const;
    QLinearGradient linearGradient() const;
    QConicalGradient conicalGradient() const;
    QRadialGradient radialGradient() const;

    void setTransformPos( quint8 pos );
    void setTransformOrigin( const QPointF &pos );
    void setTransformation( qreal hs, qreal vs, qreal xRot, qreal yRot, qreal zRot );
    void setZIndex( qint16 idx );
    void load( const QByteArray &in, int version );
    QByteArray save( int version );

    //virtual functions
    inline virtual bool hasText() const { return false; }
    inline virtual bool hasSingleImage() const { return false; }
    inline virtual bool hasImages() const { return false; }
    inline virtual bool hasUrl() const { return false; }
    inline virtual bool hasValue() const { return false; }
    inline virtual bool hasMaxLength() const { return false; }
    inline virtual bool hasDelimiter() const { return false; }
    inline virtual bool hasGradient() const { return false; }
    inline virtual bool isCheckable() const { return false; } // like checkboxes
    inline virtual bool isFocusable() const { return false; }
    inline virtual bool isDisable() const { return false; }
    inline virtual bool isReadOnlyChangeable() const { return false; }
    inline virtual bool isInputItem() const { return false; } // has form data
    inline virtual bool isLayoutItem() const { return false; }
    inline virtual bool isWidthChangeable() const { return true; }
    inline virtual bool isHeightChangeable() const { return true; }
    inline virtual bool isDraggable() const { return false; }
    inline virtual bool isDroppable() const { return false; }
    inline virtual QColor color( PHIPalette::ItemRole ) const { return QColor(); }
    inline virtual PHIPalette::ColorRole colorRole( PHIPalette::ItemRole ) const { return PHIPalette::NoRole; }
    virtual void setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &color );
    virtual void setFont( const QFont &font );
    virtual void setGradient( QLinearGradient g );
    virtual void setGradient( QConicalGradient g );
    virtual void setGradient( QRadialGradient g );
    virtual void paint( QPainter *painter, const QRectF &exposed );
    virtual PHIWID wid() const=0;

    // IDE related members
    virtual QPixmap pixmap() const=0;
    virtual QString listName() const=0;
    virtual QString description() const=0;
    inline virtual bool isPrivateItem() const { return false; }
    inline virtual PHITextData* textData() { return 0; }
    inline virtual void updateData() {}
    inline virtual void initIDE() {}
    inline const QGraphicsWidget* gw() const { return _gw; }

public slots: // usable by script engine
    inline qreal x() const { return _x; }
    inline qreal y() const { return _y; }
    inline qreal width() const { return _width; }
    inline qreal height() const { return _height; }
    inline QPointF pos() const { return QPointF( _x, _y ); }
    inline QSizeF size() const { return QSizeF( _width, _height ); }
    inline qint16 tabIndex() const { return _variants.value( DTabIndex, 0 ).value<qint16>(); }
    inline qreal opacity() const { return _variants.value( DOpacity, 1. ).toReal(); }
    inline QString title() const { return QString::fromUtf8( _variants.value( DTitle ).toByteArray() ); }
    inline QString name() const { return QString::fromLatin1( _id ); }
    inline QString parentName() const { return QString::fromLatin1( _parentId ); }
    inline PHIWID type() const { return wid(); }
    inline bool visible() const { return _variants.value( DVisibility, true ).toBool(); }

    inline void setX( qreal x ) { if ( _x==x ) return; _x=x; if ( _gw ) _gw->setX( x ); }
    inline void setY( qreal y ) { if ( _y==y ) return; _y=y; if ( _gw ) _gw->setY( y ); }
    inline void setOpacity( qreal o ) { o=qBound( 0., o, 1. ); _variants.insert( DOpacity, o ); update(); }
    inline void setTitle( const QString &t ) { _variants.insert( DTitle, t.toUtf8() ); if ( _gw ) _gw->setToolTip( t ); }
    inline void setParentName( const QString &n ) { _parentId=n.toLatin1(); }
    inline void setTabIndex( qint16 tab ) { if ( tab ) _variants.insert( DTabIndex, tab ); else _variants.remove( DTabIndex ); }
    inline void setPos( qreal x, qreal y ) { setPos( QPointF( x, y ) ); }
    inline void hide() { setVisible( false ); }
    inline void show() { setVisible( true ); }

    void setWidth( qreal w );
    void setHeight( qreal h );
    void setPos( const QPointF &p );
    void resize( qreal w, qreal h );
    void resize( const QSizeF &s );
    void setVisible( bool b );
    QStringList properties() const;

    inline QString styleSheet() const { return QString::fromUtf8( _variants.value( DStyleSheet ).toByteArray() ); }
    inline virtual void setStyleSheet( const QString &s ) { _variants.insert( DStyleSheet, s.toUtf8() ); }
    //inline QString label() const { return QString::fromUtf8( _variants.value( DLabel ).toByteArray() ); }
    //inline virtual void setLabel( const QString &s ) { _variants.insert( DLabel, s.toUtf8() ); }
    //inline QString value() const { return QString::fromUtf8( _variants.value( DValue ).toByteArray() ); }
    //inline virtual void setValue( const QString &s ) { _variants.insert( DValue, s.toUtf8() ); }
    //inline QString delimiter() const { return _variants.value( DDelimiter, L1( "\n" ) ).toString(); }
    //inline virtual void setDelimiter( const QString &s ) { _variants.insert( DDelimiter, s ); }
    //inline quint16 maxLength() const { return _variants.value( DMaxLength, 100 ).value<quint16>(); }
    //inline virtual void setMaxLength( quint16 max ) { _variants.insert( DMaxLength, max ); }
    inline bool disabled() const { return _flags & FDisabled; }
    inline virtual void setDisabled( bool b ) { b ? _flags|= FDisabled : _flags&= ~FDisabled; }
    inline bool checked() const { return _flags & FChecked; }
    inline virtual void setChecked( bool b ) { b ? _flags|= FChecked : _flags&= ~FChecked; }
    inline bool readOnly() const { return _flags & FReadOnly; }
    inline virtual void setReadOnly( bool b ) { b ? _flags|= FReadOnly : _flags&= ~FReadOnly; }

protected:
    virtual void loadItemData( QDataStream &in, int version );
    virtual void saveItemData( QDataStream &out, int version );
    virtual void squeeze() {} // free unused data
    void setWidget( QWidget* );
    QWidget* widget() const;
    //inline const QGraphicsWidget* graphicsWidget() const { return _gw; }
    const PHIBasePage* page() const;
    virtual void paintHighlight( QPainter *painter );
    virtual QRectF boundingRect() const;
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint=QSizeF() ) const; // return invalid size to call basic implementation
    virtual bool sceneEvent( QEvent *event ); // return false to call basic implementation
    inline void setSizePolicy( const QSizePolicy &policy ) { if ( _gw ) _gw->setSizePolicy( policy ); }
    inline void update( const QRectF &r=QRectF() ) { if ( _gw ) _gw->update( r ); }

    inline virtual PHIBooleanData* checkedData() { return 0; }
    inline virtual PHIBooleanData* readOnlyData() { return 0; }
    inline virtual PHIBooleanData* disabledData() { return 0; }
    inline virtual PHIImageData* imageData() { return 0; }
    inline virtual PHIImageBookData* imageBookData() { return 0; }

    // Phis server related members
    // create all cached language dependend images and transformed images:
    virtual void createCachedItems() {}
    // create a srtict HTML 4 for old browser versions:
    virtual void strictHtml( const PHISRequest* const req, QByteArray &out, const QByteArray &indent );
    // create new HTML5 content:
    virtual void html5( const PHISRequest* const req, QByteArray &out, const QByteArray &indent );
    // create jQuery related stuff, like onclick and other handlers:
    virtual void jQuery( const PHISRequest* const req, QByteArray &out );
    // create all none dynamic CSS styles:
    virtual void css( const PHISRequest* const req, QByteArray &out );

    // IDE related members
    inline void setData( quint8 t, const QVariant &v ) { _variants.insert( t, v ); }
    inline QVariant data( quint8 t, const QVariant &v=QVariant() ) const { return _variants.value( t, v ); }
    inline void removeData( quint8 t ) { _variants.remove( t ); }

    virtual void ideDragEnterEvent( QGraphicsSceneDragDropEvent *event );
    virtual void ideDragLeaveEvent( QGraphicsSceneDragDropEvent *event );
    virtual void ideDragMoveEvent( QGraphicsSceneDragDropEvent *event );
    virtual void ideDropEvent( QGraphicsSceneDragDropEvent *event );
    virtual void ideResizeEvent( QGraphicsSceneResizeEvent *event );
    virtual void ideKeyPressEvent( QKeyEvent *event );
    //virtual void ideHoverEnterEvent( QGraphicsSceneHoverEvent *event );
    //virtual void ideHoverMoveEvent( QGraphicsSceneHoverEvent *event );
    //virtual void ideHoverLeaveEvent( QGraphicsSceneHoverEvent *event );
    virtual void updatePageFont( const QFont &font );

private:
    PHIBaseItem& operator=( const PHIBaseItem& );
    void privateSqueeze();
    void updateEffect();
    void loadVersion1_x( const QByteArray &arr );
    inline void storeFlags() { _variants.insert( DFlags, static_cast<quint32>(_flags) ); }
    inline void setFlag( Flag f, bool b=true ) { b ? _flags |= f : _flags &= ~f; }

    QTransform computeTransformation() const;
    void paint( QPainter *painter, const QStyleOptionGraphicsItem *options, QWidget *widget );
    void phiPaletteChanged( const PHIPalette &pal );
    inline PHITextData* styleSheetData() { return &_styleSheetData; }
    inline PHITextData* titleData() { return &_titleData; }
    inline PHIBooleanData* visibleData() { return &_visibleData; }
    inline PHIEffect* effect() { return _effect; }
    inline QGraphicsWidget* gw() { return _gw; }

    // IDE related members
    void loadEditorData1_x( const QByteArray &arr );
    inline void setSelected( bool s ) { if ( _gw ) _gw->setSelected( s ); }
    inline bool isSelected() const { if ( _gw ) return _gw->isSelected(); return false; }
    virtual PHIConfigWidget* configWidget() { return 0; }
    virtual PHIIntData* intData_1() { return 0; }
    virtual PHIIntData* intData_2() { return 0; }
    virtual void setText( const QString &t, const QByteArray &lang );
    virtual QString text( const QByteArray &lang ) const;

signals:
    // IDE related signals
    void pushUndoStack( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &newColor );
    void pushUndoStack( const char* property, const QVariant &newVariant );
    void pushUndoStack( const QSizeF &oldSize );
    void beginUndoStackMacro( const QString &text );
    void endUndoStackMacro();

private:
    PHIBaseItemPrivate::Type _type;
    QGraphicsWidget *_gw;
    QByteArray _id, _parentId;
    qreal _x, _y, _width, _height, _xRot, _yRot, _zRot, _hSkew, _vSkew;
    qint16 _zIndex;
    QPointF _transformOrigin;
    PHIKeyHash _variants;
    Flags _flags;
    PHIBooleanData _visibleData;
    PHITextData _titleData, _styleSheetData;
    PHIEffect *_effect;
};

#ifdef PHIDEBUG
Q_DECLARE_OPERATORS_FOR_FLAGS( PHIBaseItem::Flags )
#endif
Q_DECLARE_METATYPE( PHIBaseItem* )
Q_DECLARE_METATYPE( const PHIBaseItem* )

PHIEXPORT QScriptValue baseItemToScriptValue( QScriptEngine*, PHIBaseItem* const &in );
PHIEXPORT void baseItemFromScriptValue( const QScriptValue&, PHIBaseItem* &out );

inline void baseItemFromScriptValue( const QScriptValue &obj, PHIBaseItem* &it )
{
    it=qobject_cast<PHIBaseItem*>(obj.toQObject());
}

inline void PHIBaseItem::setWidth( qreal w )
{
    if ( w==_width ) return;
    resize( QSizeF( w, _height ) );
}

inline void PHIBaseItem::setHeight( qreal h )
{
    if ( h==_height ) return;
    resize( QSizeF( _width, h ) );
}

inline void PHIBaseItem::setPos( const QPointF &p )
{
    _x=p.x();
    _y=p.y();
    if ( _gw ) _gw->setPos( p );
}

inline void PHIBaseItem::resize( qreal w, qreal h )
{
    resize( QSizeF( w, h ) );
}

inline void PHIBaseItem::resize( const QSizeF &s )
{
    _width=s.width();
    _height=s.height();
    if ( _gw ) _gw->resize( s );
}

inline void PHIBaseItem::setZIndex( qint16 idx )
{
    _zIndex=qBound(static_cast<qint16>(-PHI::maxZIndex()), idx, PHI::maxZIndex());
    if ( _gw ) _gw->setZValue( static_cast<qreal>(idx) );
}

inline QWidget* PHIBaseItem::widget() const
{
    QGraphicsProxyWidget *proxy=qobject_cast<QGraphicsProxyWidget*>(_gw);
    if ( !proxy ) return 0;
    return proxy->widget();
}

/*
class PHIEXPORT PHIBaseItem : public QObject, public PHIItem
{
    Q_OBJECT
    Q_DISABLE_COPY( PHIBaseItem )

    Q_PROPERTY( QString value WRITE setValue READ value )
    Q_PROPERTY( QString imageId WRITE setImageId READ imageId )
    Q_PROPERTY( QString label WRITE setLabel READ label )
    Q_PROPERTY( QStringList pictureBookIds WRITE setPictureBookIds READ pictureBookIds )
    Q_PROPERTY( qint32 fontSize WRITE setFontSize READ fontSize )
    Q_PROPERTY( quint8 align WRITE setAlignment READ alignment )
    Q_PROPERTY( quint32 fadeTime WRITE setFadeTime READ fadeTime )
    Q_PROPERTY( quint32 fadeInterval WRITE setFadeInterval READ fadeInterval )

    //friend class PHIBaseStyle;
    friend PHIEXPORT QDataStream& operator<<( QDataStream&, const PHIBaseItem* );
    friend PHIEXPORT QDataStream& operator>>( QDataStream&, PHIBaseItem* );

public: //not useable by script engine
    virtual QGradient gradient() const;

    inline PHIRectHash gridLayoutInfo() const { return _variants.value( DGridLayoutInfo ).value<PHIRectHash>(); }
    inline quint8 dragHotSpotType() const { return _variants.value( DDragHotSpotType, 0 ).value<quint8>(); }
    inline QPoint dragHotSpot() const { return _variants.value( DDragHotSpot, PHI::defaultHotSpot() ).toPoint(); }

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

    // functions offered by style property
    inline virtual QColor rolloverTextColor() const {
        return _variants.value( DRolloverTextColor, QColor() ).value<QColor>(); }
    inline virtual QColor rolloverBackgroundColor() const {
        return _variants.value( DRolloverBackgroundColor, QColor() ).value<QColor>(); }
    inline virtual quint8 alignment() const { return _variants.value( DAlignment, 0 ).value<quint8>(); }

    inline QByteArray cursor() const { return _variants.value( DCursor, QByteArray( "auto" ) ).toByteArray(); }
    inline virtual void setCursor( const QByteArray &c ) { _variants.insert( DCursor, c ); }

public slots: //useable by script engine
    inline QString imageId() const {
        return QString::fromUtf8( _variants.value( DImage ).toByteArray() ); }
    inline virtual void setImageId( const QString &id ) { _variants.insert( DImage, id.toUtf8() ); }
    inline QStringList pictureBookIds() const { return _variants.value( DImageBook ).toStringList(); }
    inline virtual void setPictureBookIds( const QStringList &l ) { _variants.insert( DImageBook, l ); }

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

PHIEXPORT QDataStream& operator<<( QDataStream&, const PHIBaseItem* );
PHIEXPORT QDataStream& operator>>( QDataStream&, PHIBaseItem* );

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
    void shadow( const QString &color=QString::fromLatin1( "#3F3F3F" ), qreal opac=.7, qreal xOff=8.,
        qreal yOff=8., qreal radius=1. );
    inline void surface( qreal yOff=0., qreal size=30. ) { _it->setSurface( yOff, size ); }
    inline void blur( qreal radius=5. ) { _it->setBlur( radius ); }
    inline void colorize( const QString &c=QString::fromLatin1( "#0000C0" ), qreal strength=1. ) {
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
*/
#endif // PHIBASEITEM_H
