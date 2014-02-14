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
class QGraphicsSceneDragDropEvent;
class QKeyEvent;
class PHIBasePage;
class PHIGraphicsItem;
class PHIRequest;
class PHISDataParser;
class PHIConfigWidget;
class PHIDataParser;
class PHIDomEvent;

class PHIEXPORT PHIBaseItemPrivate
{
public:
    enum Type { TUndefined=0, TIDEItem=1, TTemplateItem=2, TServerItem=3, TClientItem=4 };
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
    friend class PHIAbstractLayoutItem; // needs write access to _gw
    friend class PHIAGraphicsItem;
    friend class PHIAGraphicsScene;
    friend class ARTGraphicsScene;
    friend class ARTGraphicsItem;
    friend class ARTUndoDelLayout; // needs write access to _gw
    friend class ARTUndoEffect;
    friend class ARTItemSettings;
    friend class ARTDragDropWidget;
    friend class PHISProcessor;

    Q_OBJECT
    Q_PROPERTY( QFont _font READ font WRITE setFont SCRIPTABLE false )
    Q_PROPERTY( qreal _opacity READ realOpacity WRITE setOpacity SCRIPTABLE false )
    Q_PROPERTY( qreal _x READ realX WRITE setX SCRIPTABLE false )
    Q_PROPERTY( qreal _y READ realY WRITE setY SCRIPTABLE false )
    Q_PROPERTY( qreal _width READ realWidth WRITE setWidth SCRIPTABLE false )
    Q_PROPERTY( qreal _height READ realHeight WRITE setHeight SCRIPTABLE false )
    Q_PROPERTY( qreal _xRotation READ xRotation WRITE setXRotation SCRIPTABLE false )
    Q_PROPERTY( qreal _yRotation READ yRotation WRITE setYRotation SCRIPTABLE false )
    Q_PROPERTY( qreal _zRotation READ zRotation WRITE setZRotation SCRIPTABLE false )
    Q_PROPERTY( qreal _scale READ realScale WRITE setScale SCRIPTABLE false )

public:
    enum Wid { Unknown=0, User=1000 };
    enum DataType { DOpacity=-1, DTitle=-2, DFont=-3, DTabIndex=-4, DTransformPos=-5,
        DTransformOrigin=-6, DXRot=-7, DYRot=-8, DZRot=-9, DHSkew=-10, DVSkew=-11,
        DParentId=-12, DFlags=-13, DVisibility=-14, DStyleSheet=-15,
        DDragStartPos=-16, DDragOriginalPos=-17, DDragDistance=-18, DDragHotSpot=-19,
        DDropAcceptedIds=-20, DDragOpacity=-21, DDragHotSpotType=-22,
        DGradientType=-23, DGradientStartPoint=-24, DGradientStopPoints=-25,
        DGradientFinalStopPoint=-26, DGradientSpreadType=-27, DGradientAngle=-28,
        DGradientCenterPoint=-29, DGradientFocalPoint=-30, DGradientRadius=-31,
        DImagePath=-32, DImagePathes=-33, DIEFilter=-34, DAdjustedRect=-35, DEventFunctions=-36,
        DDragDropOptions=-37, DOneEventFunctions=-38, DAnimSizePolicy=-39,
        DAnimOrgGeometry=-40, DIsImage=-41, DAccessKey=-42 };
    enum Flag { FNone=0x0, FChild=0x1, FDoNotCache=0x2, FUseStyleSheet=0x4,
        FStoreTitleData=0x8, FStoreVisibleData=0x10, FChecked=0x20, FReadOnly=0x40,
        FDisabled=0x80, FStoreEffectData=0x100, FLayoutHeader=0x200,
        FStoreDisabledData=0x200, FHasMouseEventHandler=0x400, FHasFocusEventHandler=0x800,
        FHasHoverEventHandler=0x1000, FHasKeyEventHandler=0x2000, FHasChangeEventHandler=0x4000,
        FHasDropEventHandler=0x8000, FIsAnimating=0x10000, FIDEHidden=0x20000,
        FIDEFix=0x40000, FIDEHideChildRects=0x80000 }; //quint32
    enum DragDropOption { DDNone=0, DDMoveAction=0x1, DDRevertOnIgnore=0x2, DDRevertOnAccept=0x4,
        DDHighlightOnMouseOver=0x8, DDDragEnabled=0x10, DDDropEnabled=0x20 };
    enum DirtyFlag { DFClean=0x0, DFTitleData=0x1, DFVisibleData=0x2, DFStyleSheetData=0x4,
        DFText=0x8, DFHasFilePath=0x10, DFUseFilePathInHTML=0x20,
        DFInt1=0x100, DFInt2=0x200, DFReadOnlyData=0x400, DFDisabledData=0x800, DFHeader=0x1000,
        DFPlaceholder=DFHeader, DFDoNotCache=0x2000, DFColor=0x4000, DFBackgroundColor=0x10000,
        DFCustom1=0x100000, DFCustom2=0x200000, DFCustom3=0x400000, DFCustom4=0x800000,
        DFCustom5=0x1000000, DFCustom6=0x2000000, DFCustom7=0x4000000, DFCustom8=0x8000000 };
#ifdef PHIDEBUG
    Q_DECLARE_FLAGS( Flags, Flag )
    Q_DECLARE_FLAGS( DirtyFlags, DirtyFlag )
#else
    typedef quint32 Flags;
    typedef quint32 DirtyFlags;
#endif
    explicit PHIBaseItem( const PHIBaseItemPrivate &p );
    PHIBaseItem( const PHIBaseItem &it );
    virtual ~PHIBaseItem();

public: // not usable by script engine
    void setId( const QString &id );
    inline const QByteArray& id() const { return _id; }
    inline QString name() const { return QString::fromLatin1( _id ); }
    inline const QByteArray& parentId() const { return _parentId; }
    inline QString realParentName() const { return QString::fromLatin1( _parentId ); }
    inline void setParentName( const QString &n ) { _parentId=n.toLatin1(); }
    inline void setId( const QByteArray &id ) { setObjectName( QString::fromLatin1( id ) ); _id=id; }
    inline void setParentId( const QString &pid ) { _parentId=pid.toLatin1(); }
    inline void setParentId( const QByteArray &pid ) { _parentId=pid; }
    inline void setFocus() { if ( _gw ) { _gw->setFocus(); _gw->setSelected( true ); } }
    inline void update( const QRectF &r=QRectF() ) { if ( _gw ) _gw->update( r ); }
    inline const PHIEffect* effect() const { return _effect; }
    inline bool hasGraphicEffect() const { return _effect->effects() & PHIEffect::EGraphics; }
    void updateGraphicEffect();
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
    void prepareForAnimation();
    void restoreFromAnimation();
    void ideSetHidden( bool b );

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
    inline bool realVisible() const { return _variants.value( DVisibility, true ).toBool(); }

    inline void setX( qreal x ) { if ( _x==x ) return; _x=x; setPos( QPointF( _x, _y ) ); }
    inline void setY( qreal y ) { if ( _y==y ) return; _y=y; setPos( QPointF( _x, _y ) ); }
    inline void setOpacity( qreal o ) { o=qBound( 0., o, 1. ); _variants.insert( DOpacity, o ); if ( _gw ) _gw->setOpacity( o ); update(); }
    inline void setTitle( const QString &t ) { _variants.insert( DTitle, t.toUtf8() ); if ( _gw ) _gw->setToolTip( t ); }
    inline void setTabIndex( qint16 tab ) { if ( tab ) _variants.insert( DTabIndex, tab ); else _variants.remove( DTabIndex ); }
    inline PHIByteArrayList imagePathes() const { return _variants.value( DImagePathes ).value<PHIByteArrayList>(); }
    inline void setImagePathes( const PHIByteArrayList &list ) const {  QVariant v; v.setValue( list ); _variants.insert( DImagePathes, v ); }
    inline QByteArray imagePath() const { return _variants.value( DImagePath ).toByteArray(); }
    inline void setImagePath( const QByteArray &path ) const { _variants.insert( DImagePath, path ); }
    inline QString styleSheet() const { return QString::fromUtf8( _variants.value( DStyleSheet ).toByteArray() ); }
    virtual void setStyleSheet( const QString &s );
    inline QScriptEngine* scriptEngine() const { Q_ASSERT( page() ); return page()->scriptEngine(); }

    inline QRectF rect() const { return QRectF( QPointF(), realSize() ); }
    inline quint8 transformPos() const { return _variants.value( DTransformPos, 1 ).value<quint8>(); }
    inline void setTransformPos( PHI::Origin o ) { setTransformPos( static_cast<quint8>(o) ); }
    inline QPointF transformOrigin() const { return _transformOrigin; }
    inline void setXRotation( qreal x ) { _xRot=x; if ( _gw ) _gw->setTransform( computeTransformation() ); }
    inline void setYRotation( qreal y ) { _yRot=y; if ( _gw ) _gw->setTransform( computeTransformation() ); }
    inline void setZRotation( qreal z ) { _zRot=z; if ( _gw ) _gw->setTransform( computeTransformation() ); }
    inline void setHSkew( qreal h ) { _hSkew=h; if ( _gw ) _gw->setTransform( computeTransformation() ); }
    inline void setVSkew( qreal v ) { _vSkew=v; if ( _gw ) _gw->setTransform( computeTransformation() ); }
    inline void setScale( qreal s ) { if ( _gw ) _gw->setScale( s ); }
    inline qreal xRotation() const { return _xRot; }
    inline qreal yRotation() const { return _yRot; }
    inline qreal zRotation() const { return _zRot; }
    inline qreal hSkew() const { return _hSkew; }
    inline qreal vSkew() const { return _vSkew; }
    inline qreal realScale() const { if ( _gw ) return _gw->scale(); else return 1.; }
    inline bool hasTransformation() const { return _xRot+_yRot+_zRot+_hSkew+_vSkew ? true : false; }
    inline bool isIdeItem() const { return _type==PHIBaseItemPrivate::TIDEItem; }
    inline bool isTemplateItem() const { return _type==PHIBaseItemPrivate::TTemplateItem; }
    inline bool isClientItem() const { return _type==PHIBaseItemPrivate::TClientItem; }
    inline bool isServerItem() const { return _type==PHIBaseItemPrivate::TServerItem; }
    inline bool isGuiItem() const { return _gw ? true : false; }
    inline bool isChild() const { return _flags & FChild; }
    inline void setDirtyFlag( DirtyFlag flag, bool b=true ) const { if ( b ) _dirtyFlags |= flag; else _dirtyFlags &= ~flag; }
    inline DirtyFlags dirtyFlags() const { return _dirtyFlags; }
    inline quint32 dragDropOptions() const { return data( DDragDropOptions, 0 ).value<quint32>(); }
    inline void setDragDropOptions( quint32 opts ) { setData( DDragDropOptions, opts ); }

    QFont font() const;
    QGradient gradient() const;
    QLinearGradient linearGradient() const;
    QConicalGradient conicalGradient() const;
    QRadialGradient radialGradient() const;
    QTransform computeTransformation( bool translate=true ) const;
    const PHIBasePage* page() const;

    void phiPaletteChanged( const PHIPalette &pal );
    void setTransformPos( quint8 pos );
    void setTransformOrigin( const QPointF &pos );
    void setTransformation( qreal hs, qreal vs, qreal xRot, qreal yRot, qreal zRot );
    void setZIndex( qint16 idx );
    void load( const QByteArray &in, int version );
    void privateUpdateData();
    void privateStaticCSS( const PHIRequest *req, QByteArray &out ) const;
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;
    virtual PHIWID htmlHeaderExtension( const PHIRequest *req, QByteArray &headerOut ) const { Q_UNUSED( req ) Q_UNUSED( headerOut ) return 0; }
    virtual PHIWID htmlScriptExtension( const PHIRequest *req, QByteArray &globalScript ) const { Q_UNUSED(req ) Q_UNUSED( globalScript ) return 0; }
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint=QSizeF() ) const; // return invalid size to call basic implementation
    QByteArray save( int version );

    inline virtual bool hasText() const { return false; }
    inline virtual bool hasSingleImage() const { return false; }
    inline virtual bool hasImages() const { return false; }
    inline virtual bool hasUrl() const { return false; }
    inline virtual bool hasValue() const { return false; }
    inline virtual bool hasMaxLength() const { return false; }
    inline virtual bool hasDelimiter() const { return false; }
    inline virtual bool hasGradient() const { return false; }
    inline virtual bool hasHtmlExtension() const { return false; } // HTML header & script
    inline virtual bool isCheckable() const { return false; } // radio & checkboxes
    inline virtual bool isFocusable() const { return false; }
    inline virtual bool isInputItem() const { return false; } // has form data
    inline virtual bool isWidthChangeable() const { return true; }
    inline virtual bool isHeightChangeable() const { return true; }
    inline virtual bool isDraggable() const { return false; }
    inline virtual bool isDroppable() const { return false; }
    inline virtual QColor colorForRole( PHIPalette::ItemRole ) const { return QColor(); }
    inline virtual PHIPalette::ColorRole colorRole( PHIPalette::ItemRole ) const { return PHIPalette::NoRole; }
    virtual void setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &color );
    virtual void setFont( const QFont &font );
    virtual void setGradient( QLinearGradient g );
    virtual void setGradient( QConicalGradient g );
    virtual void setGradient( QRadialGradient g );
    virtual bool paint( QPainter *painter, const QRectF &exposed )=0;
    virtual void setDisabled( bool b );
    virtual void setChecked( bool b ) { b ? _flags|= FChecked : _flags&= ~FChecked; }
    virtual void setReadOnly( bool b ) { b ? _flags|= FReadOnly : _flags&= ~FReadOnly; }

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
    virtual void ideInit() {}

    inline const QGraphicsWidget* graphicsWidget() const { return _gw; }
    inline bool ideIsHidden() const { return _flags & FIDEHidden; }
    inline PHITextData* styleSheetData() { return &_styleSheetData; }
    inline PHITextData* titleData() { return &_titleData; }
    inline PHIBooleanData* visibleData() { return &_visibleData; }
    inline PHIBooleanData* disabledData() { return &_disabledData; }

    // static member
    static QByteArray cssRgba( const QColor &c );
    static QByteArray cssFont( const QFont &f );
    static PHIWID widFromMimeData( const QMimeData *md );
    static QImage imageFromMimeData( const QMimeData *md );
    static QString pathFromMimeData( const QMimeData *md );
    static QUrl urlFromMimeData( const QMimeData *md );
    static QColor colorFromMimeData( const QMimeData *md );

public slots: // usable by script engine
    inline QString $() const { return name(); }
    inline QScriptValue hide() { setVisible( false ); return self(); }
    inline QScriptValue show() { setVisible( true ); return self(); }
    inline QScriptValue toggle() { setVisible( !realVisible() ); return self(); }
    inline QScriptValue clearEffects() { _effect->clearAll(); updateGraphicEffect(); return self(); }
    inline bool isImage() const { return data( DIsImage, false ).toBool(); }

    virtual PHIWID wid() const=0;
    virtual QScriptValue width( const QScriptValue &v=QScriptValue() );
    virtual QScriptValue height( const QScriptValue &v=QScriptValue() );

    QScriptValue visible( const QScriptValue &b=QScriptValue() );
    QScriptValue disabled( const QScriptValue &d=QScriptValue() );
    QScriptValue title( const QScriptValue &t=QScriptValue() );
    QScriptValue opacity( const QScriptValue &o=QScriptValue() );
    QScriptValue fadeIn( qint32 start=0, qint32 duration=1000, qreal maxOpac=1., const QString &ease=PHI::defaultEasingCurve() );
    QScriptValue fadeOut( qint32 start=0, qint32 duration=1000, qreal minOpac=0, const QString &ease=PHI::defaultEasingCurve() );
    QScriptValue moveTo( qint32 left, qint32 top, qint32 start=0, qint32 duration=1000, const QString &ease=PHI::defaultEasingCurve() );
    QScriptValue moveBy( qint32 x, qint32 y, qint32 w=0, qint32 h=0, qint32 start=0, qint32 duration=1000, const QString &ease=PHI::defaultEasingCurve() );
    QScriptValue rotateIn( quint8 axis=0x2, qint32 start=0, qint32 duration=1000, const QString &ease=PHI::defaultEasingCurve() );
    QScriptValue rotateOut( quint8 axis=0x2, qint32 start=0, qint32 duration=1000, const QString &ease=PHI::defaultEasingCurve() );
    QScriptValue rotate( quint8 axis=0x4, qreal stepX=0, qreal stepY=0, qreal stepZ=1. );
    QScriptValue stop();
    QScriptValue css( const QScriptValue &sheet=QScriptValue() );
    QScriptValue on( const QString &name, const QScriptValue &v );
    QScriptValue off( const QString &name, const QScriptValue &v=QScriptValue() );
    QScriptValue one( const QString &name, const QScriptValue &v );
    QScriptValue trigger( const QString &name, const QScriptValue &args=QScriptValue(), PHIDomEvent *de=0 );
    QScriptValue x( const QScriptValue &v=QScriptValue() );
    QScriptValue y( const QScriptValue &v=QScriptValue() );
    QScriptValue top( const QScriptValue &v=QScriptValue() );
    QScriptValue left( const QScriptValue &v=QScriptValue() );
    QScriptValue pos( const QScriptValue &x=QScriptValue(), const QScriptValue &y=QScriptValue() );
    QScriptValue cursor( const QScriptValue &v=QScriptValue() );
    QScriptValue rotateX( const QScriptValue &v=QScriptValue() );
    QScriptValue rotateY( const QScriptValue &v=QScriptValue() );
    QScriptValue rotateZ( const QScriptValue &v=QScriptValue() );
    QScriptValue rotateToX( int angle, int duration=1000, const QString &ease=PHI::defaultEasingCurve() );
    QScriptValue rotateToY( int angle, int duration=1000, const QString &ease=PHI::defaultEasingCurve() );
    QScriptValue rotateToZ( int angle, int duration=1000, const QString &ease=PHI::defaultEasingCurve() );
    QScriptValue scale( const QScriptValue &v=QScriptValue() );
    QScriptValue scaleTo( double f, int duration=1000, const QString &ease=PHI::defaultEasingCurve() );
    QScriptValue slide( const QString &dir=SL( "up" ), int duration=400, const QString &ease=PHI::defaultEasingCurve() );

    QScriptValue click( const QScriptValue &v=QScriptValue() );
    QScriptValue dblclick( const QScriptValue &v=QScriptValue() );
    QScriptValue mouseover( const QScriptValue &v=QScriptValue() );
    QScriptValue mouseout( const QScriptValue &v=QScriptValue() );
    QScriptValue mousemove( const QScriptValue &v=QScriptValue() );
    QScriptValue mouseup( const QScriptValue &v=QScriptValue() );
    QScriptValue mousedown( const QScriptValue &v=QScriptValue() );
    QScriptValue keyup( const QScriptValue &v=QScriptValue() );
    QScriptValue keydown( const QScriptValue &v=QScriptValue() );
    QScriptValue keypress( const QScriptValue &v=QScriptValue() );
    QScriptValue change( const QScriptValue &v=QScriptValue() );
    QScriptValue focus( const QScriptValue &v=QScriptValue() );
    QScriptValue blur( const QScriptValue &v=QScriptValue() );
    QScriptValue drop( const QScriptValue &v=QScriptValue() );

protected:
    virtual void loadItemData( QDataStream &in, int version );
    virtual void saveItemData( QDataStream &out, int version );
    inline PHIEffect* effect() { return _effect; }
    inline void setSizePolicy( const QSizePolicy &policy ) { if ( _gw ) _gw->setSizePolicy( policy ); }
    inline void setData( quint8 t, const QVariant &v ) { _variants.insert( t, v ); }
    inline QVariant data( quint8 t, const QVariant &v=QVariant() ) const { return _variants.value( t, v ); }
    inline void removeData( quint8 t ) { _variants.remove( t ); }
    inline QString realAccessKey() const { return QString::fromUtf8( data( DAccessKey ).toByteArray() ); }
    void setAccessKey( const QString &s );
    void setCursor( Qt::CursorShape cur );
    void slideUp( int duration, const QString &ease );
    void slideDown( int duration, const QString &ease );
    void setWidget( QWidget* );
    QWidget* widget() const;
    QScriptValue self();

    QImage createImage();
    virtual QRectF boundingRect() const;
    virtual void clientInitData() {}
    virtual void updatePageFont( const QFont &font );
    virtual void paintHighlight( QPainter *painter );
    virtual void reset() {}
    virtual void checkForDragInMousePressEvent( QGraphicsSceneMouseEvent *e );
    virtual void checkForDragInMouseMoveEvent( QGraphicsSceneMouseEvent *e );
    virtual void checkDragEnterEvent( QGraphicsSceneDragDropEvent *e );
    virtual void checkDragLeaveEvent( QGraphicsSceneDragDropEvent *e );
    virtual void checkDragMoveEvent( QGraphicsSceneDragDropEvent *e );
    virtual void checkDropEvent( QGraphicsSceneDragDropEvent *e );
    inline bool dragMoveAction() const { return dragDropOptions() & DDMoveAction; }
    inline int dragDistance() const { return data( DDragDistance, PHI::defaultDragDistance() ).toInt(); }
    inline qreal dragOpacity() const { return data( DDragOpacity, 1. ).toReal(); }

    virtual void mousedown( const QGraphicsSceneMouseEvent *e ) { Q_UNUSED( e ) }
    virtual void mouseup( const QGraphicsSceneMouseEvent *e ) { Q_UNUSED( e ) }
    virtual void mouseover( const QGraphicsSceneHoverEvent *e ) { Q_UNUSED( e ) }
    virtual void mouseout( const QGraphicsSceneHoverEvent *e ) { Q_UNUSED( e ) }
    virtual void mousemove( const QGraphicsSceneHoverEvent *e ) { Q_UNUSED( e ) }
    virtual void click( const QGraphicsSceneMouseEvent *e ) { Q_UNUSED( e ) }
    virtual void dblclick( const QGraphicsSceneMouseEvent *e ) { Q_UNUSED( e ) }
    virtual void focus( const QFocusEvent *e ) { Q_UNUSED( e ) }
    virtual void blur( const QFocusEvent *e ) { Q_UNUSED( e ) }
    virtual void keyup( const QKeyEvent *e ) { Q_UNUSED( e ) }
    virtual void keydown( const QKeyEvent *e ) { Q_UNUSED( e ) }
    virtual void keypress( const QKeyEvent *e ) { Q_UNUSED( e ) }

    // HTML & server related members
    virtual void phisParseData( const PHIDataParser &parser ); // HTML
    virtual void phisCreateData( const PHIDataParser &parser ); // HTML
    virtual void clientPrepareData() {} // PHI
    virtual void cssStatic( const PHIRequest *req, QByteArray &out ) const;
    virtual void cssGraphicEffect( const PHIRequest *req, QByteArray &out, QByteArray &script ) const;
    bool cssGradientCreateable( const PHIRequest *req ) const;
    void cssLinearGradient( const PHIRequest *req, QByteArray &out ) const;
    QByteArray cssColor( const QColor &c ) const;
    QByteArray cssImageIEFilter( const QByteArray &imgId, bool alterFilters=true ) const;
    void htmlBase( const PHIRequest *req, QByteArray &out, QByteArray &script, bool project3D=true ) const; // includes id tag
    void htmlInitItem( QByteArray &script, bool close=true ) const;
    void htmlImg( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;
    void htmlImages( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;
    void htmlDragDropItem( QByteArray &script ) const;

    // IDE related members
    virtual void squeeze() {} // free unused data
    virtual void ideUpdateData() {}

    virtual void ideDragEnterEvent( QGraphicsSceneDragDropEvent *event );
    virtual void ideDragLeaveEvent( QGraphicsSceneDragDropEvent *event );
    virtual void ideDragMoveEvent( QGraphicsSceneDragDropEvent *event );
    virtual void ideDropEvent( QGraphicsSceneDragDropEvent *event );
    virtual void ideResizeEvent( QGraphicsSceneResizeEvent *event );
    virtual void ideKeyPressEvent( QKeyEvent *event );
    //virtual void ideHoverEnterEvent( QGraphicsSceneHoverEvent *event );
    //virtual void ideHoverMoveEvent( QGraphicsSceneHoverEvent *event );
    //virtual void ideHoverLeaveEvent( QGraphicsSceneHoverEvent *event );

private slots:
    inline void privateHide() { setVisible( false ); }
    inline void privateShow() { setVisible( true ); }
    inline void privateOpacityHide() { if ( realOpacity()<0.1 ) setVisible( false ); }
    void slideAnimationFinished();

private:
    PHIBaseItem& operator=( const PHIBaseItem& );
    void privateSqueeze();
    void privateClientInit();
    void phisPrivateParseData( const PHIDataParser &parser );
    void phisPrivateCreateData( const PHIDataParser &parser );
    void loadVersion1_x(const QByteArray &arr );
    void cssCustomStyleSheet( QByteArray &out ) const;
    void htmlEffects( QByteArray &script ) const;
    bool paint( QPainter *painter, const QStyleOptionGraphicsItem *options, QWidget *widget );
    bool sceneEvent( QEvent *event );
    inline QGraphicsWidget* gw() { return _gw; }
    QPointF adjustedPos() const;

    // IDE related members
    void loadEditorData1_x( const QByteArray &arr );
    void ideSetSelected( bool s );
    inline bool ideIsSelected() const { if ( _gw ) return _gw->isSelected(); return false; }
    virtual PHIConfigWidget* ideConfigWidget() { return 0; }
    virtual void ideSetText( const QString &t, const QByteArray &lang );
    virtual QString ideText( const QByteArray &lang ) const;

signals:
    void posChanged( const QPointF &pos );
    void sizeChanged( const QSizeF &size );
    void javaScriptError( const QScriptValue &err );
    void accessKeyTriggered();
    void linkRequested( const QString &url );
    void linkHovered( const QString &url );

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
    return engine->newQObject( it, QScriptEngine::QtOwnership, QScriptEngine::ExcludeSuperClassProperties |
        QScriptEngine::PreferExistingWrapperObject | QScriptEngine::SkipMethodsInEnumeration |
        QScriptEngine::ExcludeChildObjects | QScriptEngine::ExcludeDeleteLater );
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
    emit posChanged( p );
}

inline void PHIBaseItem::resize( qreal w, qreal h )
{
    if ( w==_width && h==_height ) return;
    resize( QSizeF( w, h ) );
}

inline void PHIBaseItem::resize( const QSizeF &s )
{
    _width=s.width();
    _height=s.height();
    if ( _gw ) _gw->resize( s );
    update();
    emit sizeChanged( s );
}

inline void PHIBaseItem::setZIndex( qint16 idx )
{
    _zIndex=qBound(static_cast<qint16>(-PHI::maxZIndex()), idx, PHI::maxZIndex());
    if ( _gw ) _gw->setZValue( static_cast<qreal>(idx) );
}

inline QTransform PHIBaseItem::computeTransformation( bool translate ) const
{
    QTransform shear, rotX, rotY, rotZ;
    shear.shear( _hSkew, _vSkew );
    rotZ.rotate( _zRot, Qt::ZAxis );
    rotY.rotate( _yRot, Qt::YAxis );
    rotX.rotate( _xRot, Qt::XAxis );
    if ( translate ) {
        QTransform trans;
        trans.translate( -_transformOrigin.x(), -_transformOrigin.y() );
        return trans*shear*rotZ*rotY*rotX*trans.inverted();
    } else return shear*rotZ*rotY*rotX;
    /*
    QMatrix4x4 m;
    m.translate( QVector3D( _transformOrigin ) );
    m.shear()
    m.rotate( _xRot, 1, 0, 0 );
    m.rotate( _yRot, 0, 1, 0 );
    m.rotate( _zRot, 0, 0, 1 );
    m.translate( QVector3D( -_transformOrigin ) );
    return t*m.toTransform();
    */
}

inline QWidget* PHIBaseItem::widget() const
{
    QGraphicsProxyWidget *proxy=qobject_cast<QGraphicsProxyWidget*>(_gw);
    if ( !proxy ) return 0;
    return proxy->widget();
}

inline QScriptValue PHIBaseItem::visible( const QScriptValue &b )
{
    if ( !b.isValid() ) return realVisible();
    setVisible( b.toBool() );
    return self();
}

inline QScriptValue PHIBaseItem::disabled( const QScriptValue &d )
{
    if ( !d.isValid() ) return realDisabled();
    setDisabled( d.toBool() );
    return self();
}

inline QScriptValue PHIBaseItem::title( const QScriptValue &t )
{
    if ( !t.isValid() ) return realTitle();
    setTitle( t.toString() );
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

inline QByteArray PHIBaseItem::cssRgba( const QColor &c )
{
    QByteArray arr;
    arr.reserve( 50 );
    arr+=BL( "rgba(" )+QByteArray::number( c.red() )+','+QByteArray::number( c.green() )+','
        +QByteArray::number( c.blue() )+','+(c.alphaF()==1. ? BL( "1" ) : QByteArray::number( c.alphaF(), 'f', 3 ))+')';
    return arr;
}

inline QByteArray PHIBaseItem::cssColor( const QColor &c ) const
{
    if ( Q_LIKELY( c.alphaF()==1. ) ) return c.name().toLatin1();
    return cssRgba( c );
}

inline QByteArray PHIBaseItem::cssImageIEFilter( const QByteArray &imgId, bool alterFilters ) const
{
    QByteArray tmp=BL( "phi.phis?i=" );
    if ( _dirtyFlags & DFUseFilePathInHTML ) tmp=QByteArray();
    tmp=BL( "progid:DXImageTransform.Microsoft.AlphaImageLoader(src='" )+tmp+imgId;
    if ( _dirtyFlags & DFUseFilePathInHTML ) tmp+BL( "',sizingMethod='scale')" );
    else tmp+=BL( "&t=1',sizingMethod='scale')" );
    if ( !alterFilters ) return tmp;
    if ( _variants.value( DIEFilter ).isValid() ) {
        _variants.insert( DIEFilter, tmp+_variants.value( DIEFilter ).toByteArray() );
    } else _variants.insert( DIEFilter, tmp );
    return tmp;
}

inline QByteArray PHIBaseItem::cssFont( const QFont &f )
{
    QByteArray out;
    out.reserve( 200 );
    out+=BL( "font-family:'" )+f.family().toUtf8();
    if ( !f.lastResortFamily().isEmpty() ) out+=BL( "','" )+f.lastResortFamily().toUtf8();
    out+=BL( "';font-size:" )+QByteArray::number( f.pointSize() )+BL( "pt;" );
    if ( f.bold() ) out+=BL( "font-weight:bold;" );
    if ( f.italic() ) out+=BL( "font-style:italic;" );
    if ( f.underline() ) out+=BL( "text-decoration:underline;" );
    out.chop(1);
    return out;
}

#endif // PHIBASEITEM_H
