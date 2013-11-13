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
#include <QScriptValue>
#include <QScriptEngine>
#include <QDataStream>
#include <QGraphicsProxyWidget>
#include <QPixmap>
#include "phi.h"
#include "phipalette.h"
#include "phieffect.h"
#include "phidatasources.h"
#include "phibasepage.h"

class QGraphicsSceneEvent;
class QKeyEvent;
class PHIBasePage;
class PHIGraphicsItem;
class PHIRequest;
class PHISDataParser;
class PHIConfigWidget;
class PHIDataParser;

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
    friend class PHIAbstractLayoutItem; // need access to _gw
    friend class PHIAGraphicsItem;
    friend class PHIAGraphicsScene;
    friend class ARTGraphicsScene;
    friend class ARTGraphicsItem;
    friend class ARTUndoDelLayout; // need access to _gw
    friend class ARTItemSettings;
    friend class PHISProcessor;

    Q_OBJECT
    //Q_PROPERTY( qreal x READ x WRITE setX )
    //Q_PROPERTY( qreal y READ y WRITE setY )
    //Q_PROPERTY( qreal width READ width WRITE setWidth )
    //Q_PROPERTY( qreal height READ height WRITE setHeight )
    //Q_PROPERTY( qint16 tabIndex READ tabIndex WRITE setTabIndex NOTIFY tabIndexChanged )
    //Q_PROPERTY( QString title READ title WRITE setTitle NOTIFY titleChanged )
    //Q_PROPERTY( qint16 zIndex READ zIndex WRITE setZIndex NOTIFY zIndexChanged )
    //Q_PROPERTY( QPointF pos READ pos WRITE setPos )
    //Q_PROPERTY( quint8 transformPos READ transformPos WRITE setTransformPos )
    //Q_PROPERTY( QPointF transformOrigin READ transformOrigin WRITE setTransformOrigin )
    Q_PROPERTY( QFont _font READ font WRITE setFont SCRIPTABLE false )
    Q_PROPERTY( qreal _opacity READ realOpacity WRITE setOpacity SCRIPTABLE false )

public:
    enum Wid { Unknown=0, User=1000 };
    enum DataType { DOpacity=-1, DTitle=-2, DFont=-3, DTabIndex=-4, DTransformPos=-5,
        DTransformOrigin=-6, DXRot=-7, DYRot=-8, DZRot=-9, DHSkew=-10, DVSkew=-11,
        DParentId=-12, DFlags=-13, DVisibility=-14, DStyleSheet=-15,
        DGradientType=-23, DGradientStartPoint=-24, DGradientStopPoints=-25,
        DGradientFinalStopPoint=-26, DGradientSpreadType=-27, DGradientAngle=-28,
        DGradientCenterPoint=-29, DGradientFocalPoint=-30, DGradientRadius=-31,
        DImagePath=-32, DImagePathes=-33, DIEFilter=-34, DAdjustedRect=-35 };
    enum Flag { FNone=0x0, FChild=0x1, FDoNotCache=0x2, FUseStyleSheet=0x4,
        FStoreTitleData=0x8, FStoreVisibleData=0x10, FChecked=0x20, FReadOnly=0x40,
        FDisabled=0x80, FStoreEffectData=0x100, FLayoutHeader=0x200,
        FStoreDisabledData=0x200, FUseFilePath=0x400 }; //quint32
    enum DirtyFlag { DFClean=0x0, DFTitleData=0x1, DFVisibleData=0x2, DFStyleSheetData=0x4,
        DFEffect=0x8, DFPos=0x10, DFSize=0x20, DFText=0x40, DFTransform=0x80,
        DFInt1=0x100, DFInt2=0x200, DFReadOnlyData=0x400, DFDisabledData=0x800,
        DFHeader=0x1000, DFPlaceholder=DFHeader, DFCustom1=0x10000, DFCustom2=0x20000,
        DFCustom3=0x40000, DFCustom4=0x80000 };
#ifdef PHIDEBUG
    Q_DECLARE_FLAGS( Flags, Flag )
    Q_DECLARE_FLAGS( DirtyFlags, DirtyFlag )
#else
    typedef quint32 Flags;
    typedef quint32 DirtyFlags;
#endif
    /*
    enum Widget {
        FILE_BUTTON=9, HIDDEN=11, IMAGE_BUTTON=12,
        LINE=15
        LINK=23, TAB=24, TEXT=29,
        LANG_SELECTOR=33, LAYOUT_DELIVERY=35,
        ROLLOVER_BUTTON=37,
        RICH_TEXT=41, CHECK_LIST=43, IMAGE_BOOK=45, TABLE=46
        HTML_DOC=49, SEARCH=50, FACEBOOK_LIKE=55, GOOGLE_STATIC_MAP=56,
        GOOGLE_PLUS=57, TWITTER=58, PROGRESSBAR=59, YOUTUBE=60, CANVAS=61, GOOGLE_CALENDAR=62, GOOGLE_MAPS=63
    */
    explicit PHIBaseItem( const PHIBaseItemPrivate &p );
    PHIBaseItem( const PHIBaseItem &it );
    virtual ~PHIBaseItem();

public: // not usable by script engine
    void setId( const QString &id );
    inline const QByteArray& id() const { return _id; }
    inline const QByteArray& parentId() const { return _parentId; }
    inline QString realParentName() const { return QString::fromLatin1( _parentId ); }
    inline void setParentName( const QString &n ) { _parentId=n.toLatin1(); }
    inline void setId( const QByteArray &id ) { setObjectName( QString::fromLatin1( id ) ); _id=id; }
    inline void setParentId( const QString &pid ) { _parentId=pid.toLatin1(); }
    inline void setParentId( const QByteArray &pid ) { _parentId=pid; }
    inline void setFocus() { if ( _gw ) { _gw->setFocus(); _gw->setSelected( true ); } }
    inline void update( const QRectF &r=QRectF() ) { if ( _gw ) _gw->update( r ); }
    inline PHIEffect* effect() { return _effect; }
    inline const PHIEffect* effect() const { return _effect; }
    inline bool hasGraphicEffect() const { return _effect->effects() & PHIEffect::EGraphics; }
    void updateEffect();
    inline void storeFlags() { _variants.insert( DFlags, static_cast<quint32>(_flags) ); }
    inline void setFlag( Flag f, bool b=true ) { b ? _flags |= f : _flags &= ~f; }
    inline Flags flags() const { return _flags; }
    inline PHIKeyHash data() { storeFlags(); return _variants; }
    inline void setData( const PHIKeyHash &data ) { _variants=data; _flags=static_cast<Flags>(_variants.value( DFlags, 0 ).value<quint32>()); }
    inline void setPos( qreal x, qreal y ) { setPos( QPointF( x, y ) ); }
    inline QRectF adjustedRect() const { return _variants.value( DAdjustedRect, boundingRect() ).toRectF(); }
    inline void setAdjustedRect( const QRectF &r ) const { _variants.insert( DAdjustedRect, r ); }
    void setWidth( qreal w );
    void setHeight( qreal h );
    void setPos( const QPointF &p );
    void resize( qreal w, qreal h );
    void resize( const QSizeF &s );
    void setVisible( bool b );

    // prefixing with 'real' for internal using:
    inline qreal realX() const { return _x; }
    inline qreal realY() const { return _y; }
    inline qreal realWidth() const { return _width; }
    inline qreal realHeight() const { return _height; }
    inline QPointF realPos() const { return QPointF( _x, _y ); }
    inline QSizeF realSize() const { return QSizeF( _width, _height ); }
    inline qint16 realTabIndex() const { return _variants.value( DTabIndex, 0 ).value<qint16>(); }
    inline qreal realOpacity() const { return _variants.value( DOpacity, 1. ).toReal(); }
    inline QString realTitle() const { return QString::fromUtf8( _variants.value( DTitle ).toByteArray() ); }
    inline qint16 realZIndex() const { return _zIndex; }
    inline bool realDisabled() const { return _flags & FDisabled; }
    inline bool realChecked() const { return _flags & FChecked; }
    inline bool realReadOnly() const { return _flags & FReadOnly; }

    inline void setX( qreal x ) { if ( _x==x ) return; _dirtyFlags|=DFPos; _x=x; if ( _gw ) _gw->setX( x ); }
    inline void setY( qreal y ) { if ( _y==y ) return; _dirtyFlags|=DFPos; _y=y; if ( _gw ) _gw->setY( y ); }
    inline void setOpacity( qreal o ) { o=qBound( 0., o, 1. ); _variants.insert( DOpacity, o ); update(); }
    inline void setTitle( const QString &t ) { _variants.insert( DTitle, t.toUtf8() ); if ( _gw ) _gw->setToolTip( t ); }
    inline void setTabIndex( qint16 tab ) { if ( tab ) _variants.insert( DTabIndex, tab ); else _variants.remove( DTabIndex ); }
    inline PHIByteArrayList imagePathes() const { return _variants.value( DImagePathes ).value<PHIByteArrayList>(); }
    inline void setImagePathes( const PHIByteArrayList &list ) const {  QVariant v; v.setValue( list ); _variants.insert( DImagePathes, v ); }
    inline QByteArray imagePath() const { return _variants.value( DImagePath ).toByteArray(); }
    inline void setImagePath( const QByteArray &path ) const { _variants.insert( DImagePath, path ); }
    inline QString styleSheet() const { return QString::fromUtf8( _variants.value( DStyleSheet ).toByteArray() ); }
    virtual void setStyleSheet( const QString &s );

    inline QRectF rect() const { return QRectF( QPointF(), realSize() ); }
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
    inline bool hasTransformation() const { return _xRot+_yRot+_zRot+_hSkew+_vSkew ? true : false; }
    inline bool isIdeItem() const { return _type==PHIBaseItemPrivate::TIDEItem; }
    inline bool isTemplateItem() const { return _type==PHIBaseItemPrivate::TTemplateItem; }
    inline bool isClientItem() const { return _type==PHIBaseItemPrivate::TClientItem; }
    inline bool isServerItem() const { return _type==PHIBaseItemPrivate::TServerItem; }
    inline bool isGuiItem() const { return _gw ? true : false; }
    inline bool isChild() const { return _flags & FChild; }
    QFont font() const;
    QGradient gradient() const;
    QLinearGradient linearGradient() const;
    QConicalGradient conicalGradient() const;
    QRadialGradient radialGradient() const;
    QTransform computeTransformation() const;

    void phiPaletteChanged( const PHIPalette &pal );
    void setTransformPos( quint8 pos );
    void setTransformOrigin( const QPointF &pos );
    void setTransformation( qreal hs, qreal vs, qreal xRot, qreal yRot, qreal zRot );
    void setZIndex( qint16 idx );
    void load( const QByteArray &in, int version );
    void privateUpdateData();
    void privateStaticCSS( const PHIRequest *req, QByteArray &out ) const;
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &jquery, const QByteArray &indent ) const;
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
    inline virtual bool isCheckable() const { return false; } // radio & checkboxes
    inline virtual bool isFocusable() const { return false; }
    inline virtual bool isInputItem() const { return false; } // has form data
    inline virtual bool isLayoutItem() const { return false; }
    inline virtual bool isWidthChangeable() const { return true; }
    inline virtual bool isHeightChangeable() const { return true; }
    inline virtual bool isDraggable() const { return false; }
    inline virtual bool isDroppable() const { return false; }
    inline virtual QColor color( PHIPalette::ItemRole ) const { return QColor(); }
    inline virtual PHIPalette::ColorRole colorRole( PHIPalette::ItemRole ) const { return PHIPalette::NoRole; }
    inline virtual QByteArray extension() const { return QByteArray(); }
    virtual void setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &color );
    virtual void setFont( const QFont &font );
    virtual void setGradient( QLinearGradient g );
    virtual void setGradient( QConicalGradient g );
    virtual void setGradient( QRadialGradient g );
    virtual void paint( QPainter *painter, const QRectF &exposed );
    virtual void setDisabled( bool b );
    virtual void setChecked( bool b ) { b ? _flags|= FChecked : _flags&= ~FChecked; }
    virtual void setReadOnly( bool b ) { b ? _flags|= FReadOnly : _flags&= ~FReadOnly; }
    virtual PHIWID wid() const=0;

    // IDE related members
    virtual QPixmap pixmap() const=0;
    virtual QString listName() const=0;
    virtual QString description() const=0;
    virtual bool isPrivateItem() const { return false; }
    virtual PHITextData* textData() { return 0; }
    virtual PHIImageData* imageData() { return 0; }
    virtual PHIImageBookData* imageBookData() { return 0; }
    virtual PHITextData* placeholderData() { return 0; }
    virtual PHIBooleanData* checkedData() { return 0; }
    virtual PHIBooleanData* readOnlyData() { return 0; }
    virtual PHIIntData* intData_1() { return 0; }
    virtual PHIIntData* intData_2() { return 0; }
    virtual void initIDE() {}
    inline const QGraphicsWidget* gw() const { return _gw; }
    inline PHITextData* styleSheetData() { return &_styleSheetData; }
    inline PHITextData* titleData() { return &_titleData; }
    inline PHIBooleanData* visibleData() { return &_visibleData; }
    inline PHIBooleanData* disabledData() { return &_disabledData; }

    static QByteArray rgba( const QColor &c );
    static PHIWID widFromMimeData( const QMimeData *md );
    static QImage imageFromMimeData( const QMimeData *md );
    static QString pathFromMimeData( const QMimeData *md );
    static QUrl urlFromMimeData( const QMimeData *md );
    static QColor colorFromMimeData( const QMimeData *md );

public slots: // usable by script engine
    inline QString name() const { return QString::fromLatin1( _id ); }
    QScriptValue visible( const QScriptValue &b=QScriptValue() );
    QScriptValue opacity( const QScriptValue &o=QScriptValue() );
    inline QScriptValue hide() { setVisible( false ); return self(); }
    inline QScriptValue show() { setVisible( true ); return self(); }
    inline QScriptValue clearEffects() { _effect->clearAll(); return self(); }
    inline QScriptValue fadeIn( qint32 start, qint32 duration, qreal maxOpac, const QString &ease ) {
        _effect->setFadeIn( start, duration, maxOpac, PHI::toEasingCurveType( ease ) ); return self(); }
    inline QScriptValue fadeOut( qint32 start, qint32 duration, qreal minOpac, const QString &ease ) {
        _effect->setFadeOut( start, duration, minOpac, PHI::toEasingCurveType( ease ) ); return self(); }
    //inline void shadowEffect( const QColor &c, qreal xOff, qreal yOff, qreal radius ) { _effect->setShadow( c, xOff, yOff, radius ); }
    //inline void reflectionEffect( qreal yOff, qreal size ) { _effect->setSurface( yOff, size ); }
    //inline void blurEffect( qreal radius ) { _effect->setBlur( radius ); }
    //inline void colorizeEffect( const QColor &c, qreal strength ) { _effect->setColorize( c, strength ); }
    inline QScriptValue moveTo( qint32 start, qint32 duration, qint32 left, qint32 top, const QString &ease ) {
        _effect->setMoveTo( start, duration, left, top, PHI::toEasingCurveType( ease ) ); return self(); }
    inline QScriptValue moveBy( qint32 start, qint32 duration, qint32 x, qint32 y, qint32 w, qint32 h, const QString &ease ) {
        _effect->setMoveBy( start, duration, x, y, w, h, PHI::toEasingCurveType( ease ) ); return self(); }
    inline QScriptValue rotateIn( quint8 axis, qint32 start, qint32 duration, const QString &ease ) {
        _effect->setRotateIn( axis, start, duration, PHI::toEasingCurveType( ease ) ); return self(); }
    inline QScriptValue rotateOut( quint8 axis, qint32 start, qint32 duration, const QString &ease ) {
        _effect->setRotateOut( axis, start, duration, PHI::toEasingCurveType( ease ) ); return self(); }
    inline QScriptValue rotate( quint8 axis, qreal stepX, qreal stepY, qreal stepZ ) {
        _effect->setRotate( axis, stepX, stepY, stepZ ); return self(); }
    QScriptValue css( const QScriptValue &sheet=QScriptValue() );

    //inline QString label() const { return QString::fromUtf8( _variants.value( DLabel ).toByteArray() ); }
    //inline virtual void setLabel( const QString &s ) { _variants.insert( DLabel, s.toUtf8() ); }
    //inline QString value() const { return QString::fromUtf8( _variants.value( DValue ).toByteArray() ); }
    //inline virtual void setValue( const QString &s ) { _variants.insert( DValue, s.toUtf8() ); }
    //inline quint16 maxLength() const { return _variants.value( DMaxLength, 100 ).value<quint16>(); }
    //inline virtual void setMaxLength( quint16 max ) { _variants.insert( DMaxLength, max ); }

protected:
    virtual void loadItemData( QDataStream &in, int version );
    virtual void saveItemData( QDataStream &out, int version );
    virtual void squeeze() {} // free unused data
    virtual void parseData( const PHIDataParser &parser ) { Q_UNUSED( parser ) }
    virtual void createTmpData( const PHIDataParser &parser ) { Q_UNUSED( parser ) }
    virtual void updateData() {}
    void setWidget( QWidget* );
    QWidget* widget() const;
    const PHIBasePage* page() const;
    QScriptValue self();
    QImage createImage();
    virtual void paintHighlight( QPainter *painter );
    virtual QRectF boundingRect() const;
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint=QSizeF() ) const; // return invalid size to call basic implementation
    virtual bool sceneEvent( QEvent *event ); // return false to call basic implementation
    inline void setSizePolicy( const QSizePolicy &policy ) { if ( _gw ) _gw->setSizePolicy( policy ); }
    inline void setDirtyFlag( DirtyFlag flag ) { _dirtyFlags |= flag; }
    inline DirtyFlags dirtyFlags() const { return _dirtyFlags; }

    // HTML related members
    virtual void staticCSS( const PHIRequest *req, QByteArray &out ) const;
    void startCSS( const PHIRequest *req, QByteArray &out, QByteArray &jquery, bool project3D=true ) const; // includes id tag
    virtual void graphicEffectCSS( const PHIRequest *req, QByteArray &out, QByteArray &jquery ) const;
    void imageIEFilterCSS( const QByteArray &imgId ) const;
    void genAdjustedPos( QByteArray &jquery ) const;
    void genAdjustedSize( QByteArray &jquery ) const;
    void genLinearGradient( const PHIRequest *req, QByteArray &out ) const;
    void genHtmlImg( const PHIRequest *req, QByteArray &out, QByteArray &jquery, const QByteArray &indent ) const;

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
    void privateParseData( const PHIDataParser &parser );
    void privateCreateTmpData( const PHIDataParser &parser );
    void loadVersion1_x( const QByteArray &arr );
    void genCustomStyleSheet( QByteArray &out ) const;

    void paint( QPainter *painter, const QStyleOptionGraphicsItem *options, QWidget *widget );
    inline QGraphicsWidget* gw() { return _gw; }

    // IDE related members
    void loadEditorData1_x( const QByteArray &arr );
    inline void setSelected( bool s ) { if ( _gw ) _gw->setSelected( s ); }
    inline bool isSelected() const { if ( _gw ) return _gw->isSelected(); return false; }
    virtual PHIConfigWidget* configWidget() { return 0; }
    virtual void setText( const QString &t, const QByteArray &lang );
    virtual QString text( const QByteArray &lang ) const;

signals:
    // IDE related signals
    void pushUndoStack( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &newColor );
    void pushUndoStack( const char* property, const QVariant &newVariant );
    void pushUndoStack( const QSizeF &oldSize );
    void pushUndoStack( const QImage &image, int i=-1 );
    void beginUndoStackMacro( const QString &text );
    void endUndoStackMacro();

private:
    PHIBaseItemPrivate::Type _type;
    QGraphicsWidget *_gw;
    QByteArray _id, _parentId;
    qreal _x, _y, _width, _height, _xRot, _yRot, _zRot, _hSkew, _vSkew;
    qint16 _zIndex;
    QPointF _transformOrigin;
    Flags _flags;
    PHIBooleanData _visibleData, _disabledData;
    PHITextData _titleData, _styleSheetData;
    PHIEffect *_effect;
    mutable DirtyFlags _dirtyFlags;
    mutable PHIKeyHash _variants;
};

#ifdef PHIDEBUG
Q_DECLARE_OPERATORS_FOR_FLAGS( PHIBaseItem::Flags )
Q_DECLARE_OPERATORS_FOR_FLAGS( PHIBaseItem::DirtyFlags )
#endif
Q_DECLARE_METATYPE( PHIBaseItem* )
Q_DECLARE_METATYPE( const PHIBaseItem* )

PHIEXPORT QScriptValue baseItemToScriptValue( QScriptEngine*, PHIBaseItem* const &in );
PHIEXPORT void baseItemFromScriptValue( const QScriptValue&, PHIBaseItem* &out );

inline QScriptValue baseItemToScriptValue( QScriptEngine *engine, PHIBaseItem* const &it )
{
    return engine->newQObject( it, QScriptEngine::AutoOwnership,
        QScriptEngine::PreferExistingWrapperObject | QScriptEngine::ExcludeDeleteLater );
}

inline void baseItemFromScriptValue( const QScriptValue &obj, PHIBaseItem* &it )
{
    it=qobject_cast<PHIBaseItem*>(obj.toQObject());
}

inline const PHIBasePage* PHIBaseItem::page() const
{
    return qobject_cast<const PHIBasePage*>(parent());
}

inline QScriptValue PHIBaseItem::self()
{
    Q_ASSERT( page()->scriptEngine() );
    return baseItemToScriptValue( page()->scriptEngine(), this );
}

inline void PHIBaseItem::setWidth( qreal w )
{
    if ( w==_width ) return;
    _dirtyFlags |= DFSize;
    resize( QSizeF( w, _height ) );
}

inline void PHIBaseItem::setHeight( qreal h )
{
    if ( h==_height ) return;
    _dirtyFlags |= DFSize;
    resize( QSizeF( _width, h ) );
}

inline void PHIBaseItem::setPos( const QPointF &p )
{
    _x=p.x();
    _y=p.y();
    _dirtyFlags |= DFPos;
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
    _dirtyFlags |= DFSize;
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

inline QScriptValue PHIBaseItem::visible( const QScriptValue &b )
{
    if ( !b.isValid() ) return _variants.value( DVisibility, true ).toBool();
    setVisible( b.toBool() );
    return self();
}

inline QScriptValue PHIBaseItem::css( const QScriptValue &sheet )
{
    if ( !sheet.isValid() ) return styleSheet();
    setStyleSheet( sheet.toString() );
    return self();
}

inline QScriptValue PHIBaseItem::opacity( const QScriptValue &o )
{
    if ( !o.isValid() ) return realOpacity();
    setOpacity( o.toNumber() );
    return self();
}

inline QByteArray PHIBaseItem::rgba( const QColor &c )
{
    QByteArray arr;
    arr.reserve( 50 );
    arr+=BL( "rgba(" );
    arr+=QByteArray::number( c.red() )+','+QByteArray::number( c.green() )+','
        +QByteArray::number( c.blue() )+','+QByteArray::number( c.alphaF(), 'f', 3 )+')';
    return arr;
}

inline void PHIBaseItem::genAdjustedPos( QByteArray &jquery ) const
{
    QRectF r=adjustedRect();
    if ( QPointF()==r.topLeft() ) return;
    jquery+=BL( "$('" )+id()+BL( "').pos(" )+QByteArray::number( qRound(_x+r.x()) )
        +','+QByteArray::number( qRound(_y+r.y()) )+BL( ");\n" );
}

inline void PHIBaseItem::genAdjustedSize( QByteArray &jquery ) const
{
    QRectF r=adjustedRect();
    if ( realSize()==r.size() ) return;
    jquery+=BL( "$('" )+id()+BL( "').width(" )+QByteArray::number( qRound(r.width()) )
        +BL( ").height(" )+QByteArray::number( qRound(r.height()) )+BL( ");\n" );
}

inline void PHIBaseItem::imageIEFilterCSS( const QByteArray &imgId ) const
{
    QByteArray tmp=BL( "&t=1" );
    if ( _flags & FUseFilePath ) tmp=QByteArray();
    tmp=BL( "progid:DXImageTransform.Microsoft.AlphaImageLoader(src='phi.phis?i=" )+imgId+tmp+BL( "')" );
    if ( _variants.value( DIEFilter ).isValid() ) tmp=tmp+_variants.value( DIEFilter ).toByteArray();
    _variants.insert( DIEFilter, tmp );
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
*/

#endif // PHIBASEITEM_H
