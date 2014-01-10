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
#ifndef PHIABSTRACTITEMS_H
#define PHIABSTRACTITEMS_H
#include "phibaseitem.h"
#include "phipalette.h"
#include "phidatasources.h"
#include "phi.h"

class QGraphicsGridLayout;
class QHttpMultiPart;
class PHIDataParser;
class PHIBasePage;
class PHIWebPage;

class PHIEXPORT PHIAbstractTextItem : public PHIBaseItem
{
    Q_OBJECT
    Q_PROPERTY( QColor _textColor READ realColor WRITE setColor SCRIPTABLE false )
    Q_PROPERTY( QColor _backgroundColor READ realBackgroundColor WRITE setBackgroundColor SCRIPTABLE false )
    Q_PROPERTY( QString _text READ realText WRITE setText SCRIPTABLE false )
    Q_PROPERTY( quint16 _align READ realAlignment WRITE setAlignment SCRIPTABLE false )

public:
    enum ItemData { DColor=-100, DBackgroundColor=-101, DText=-102, DTmpColor=-103, DTmpBackgroundColor=-104, DAlignment=-105 };
    explicit PHIAbstractTextItem( const PHIBaseItemPrivate &p ) : PHIBaseItem( p ) {}
    PHIAbstractTextItem( const PHIAbstractTextItem &it ) : PHIBaseItem( it ), _colorRole( it._colorRole ),
        _backgroundColorRole( it._backgroundColorRole ), _textData( it._textData ) {}
    virtual ~PHIAbstractTextItem() {}

    inline void setColor( const QColor &col ) { setColor( PHIPalette::WidgetText, _colorRole, col ); }
    inline void setBackgroundColor( const QColor &col ) { setColor( PHIPalette::WidgetBase, _backgroundColorRole, col ); }
    inline QColor realColor() const { return data( DColor, QColor( Qt::black ) ).value<QColor>(); }
    inline QColor realBackgroundColor() const { return data( DBackgroundColor, QColor( Qt::white ) ).value<QColor>(); }
    inline QString realText() const { return QString::fromUtf8( data( DText ).toByteArray() ); }
    inline void setText( const QString &s ) { setData( DText, s.toUtf8() ); if ( !isServerItem() ) setWidgetText( s ); }
    inline quint16 realAlignment() const { return data( DAlignment, static_cast<quint16>( Qt::AlignLeft | Qt::AlignVCenter ) ).value<quint16>(); }
    inline void setAlignment( quint16 align ) { setData( DAlignment, align ); if ( !isServerItem() ) setWidgetAligment( static_cast<Qt::Alignment>(align) ); }
    inline const PHITextData* textData() const { return &_textData; }

    virtual void setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col );
    virtual QColor colorForRole( PHIPalette::ItemRole role ) const;
    virtual PHIPalette::ColorRole colorRole( PHIPalette::ItemRole role ) const;
    virtual void ideInit();
    virtual bool isDraggable() const { return true; }

public slots:
    QScriptValue textAlign( const QScriptValue &v=QScriptValue() );

protected:
    virtual bool isSingleLine() const { return true; }
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual void ideUpdateData();
    virtual void setWidgetText( const QString &t )=0;
    virtual void setWidgetAligment( Qt::Alignment )=0;
    virtual void ideDragEnterEvent( QGraphicsSceneDragDropEvent *event );
    virtual void ideDragMoveEvent( QGraphicsSceneDragDropEvent *event );
    virtual void ideDragLeaveEvent( QGraphicsSceneDragDropEvent *event );
    virtual void ideDropEvent( QGraphicsSceneDragDropEvent *event );
    virtual bool hasText() const { return true; }
    virtual PHITextData* textData() { return &_textData; }
    virtual void saveItemData( QDataStream &out, int version );
    virtual void loadItemData( QDataStream &in, int version );
    virtual void squeeze();
    virtual PHIConfigWidget* ideConfigWidget();
    virtual void phisCreateData( const PHIDataParser &parser );
    virtual void phisParseData( const PHIDataParser &parser );
    virtual void clientPrepareData();
    virtual void clientInitData();

    inline void setColorRole( PHIPalette::ColorRole cr ) { _colorRole=cr; }
    inline PHIPalette::ColorRole colorRole() const { return _colorRole; }
    inline void setBackgroundColorRole( PHIPalette::ColorRole cr ) { _backgroundColorRole=cr; }
    inline PHIPalette::ColorRole backgroundColorRole() const { return _backgroundColorRole; }

private:
    virtual void ideSetText( const QString &t, const QByteArray &lang );
    virtual QString ideText( const QByteArray &lang ) const;

    PHIPalette::ColorRole _colorRole, _backgroundColorRole;
    static qreal _dropRegion;
    PHITextData _textData;
};

class PHIEXPORT PHIAbstractShapeItem : public PHIBaseItem
{
    Q_OBJECT
    Q_PROPERTY( quint8 _line READ realLine WRITE setLine SCRIPTABLE false )
    Q_PROPERTY( quint8 _pattern READ realPattern WRITE setPattern SCRIPTABLE false )
    Q_PROPERTY( QColor _color READ realColor WRITE setColor SCRIPTABLE false )
    Q_PROPERTY( QColor _outlineColor READ realOutlineColor WRITE setOutlineColor SCRIPTABLE false )
    Q_PROPERTY( qreal _penWidth READ realPenWidth WRITE setPenWidth SCRIPTABLE false )

public:
    enum ItemData { DLineStyle=-100, DPatternStyle=-101, DGradientType=-102,
        DColor=-103, DOutlineColor=-104, DPenWidth=-105, DTmpColor=-106,
        DTmpOutlineColor=-107, DTmpPatternStyle=-108, DTmpLineStyle=-109,
        DTmpPenWidth=-110 };
    explicit PHIAbstractShapeItem( const PHIBaseItemPrivate &p ) : PHIBaseItem( p ) {}
    PHIAbstractShapeItem( const PHIAbstractShapeItem &it ) : PHIBaseItem( it ),
        _colorRole( it._colorRole ), _outlineColorRole( it._outlineColorRole ) {}
    virtual ~PHIAbstractShapeItem() {}

    virtual void setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col );
    virtual QColor color( PHIPalette::ItemRole role ) const;
    virtual PHIPalette::ColorRole colorRole( PHIPalette::ItemRole role ) const;
    virtual bool hasGradient() const { return true; }
    virtual bool isDraggable() const { return true; }
    virtual bool isDroppable() const { return true; }
    virtual void ideInit();
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;

    void setLine( quint8 l );
    void setPattern( quint8 p );
    void setPenWidth( qreal w );
    inline void setColor( const QColor &col ) { setColor( PHIPalette::Foreground, _colorRole, col ); }
    inline void setOutlineColor( const QColor &col ) { setColor( PHIPalette::Background, _outlineColorRole, col ); }
    inline quint8 realLine() const { return data( DLineStyle, 0 ).value<quint8>(); }
    inline quint8 realPattern() const { return data( DPatternStyle, 1 ).value<quint8>(); }
    inline QColor realColor() const { return data( DColor, QColor( Qt::black ) ).value<QColor>(); }
    inline QColor realOutlineColor() const { return data( DOutlineColor, QColor( Qt::black ) ).value<QColor>(); }
    inline qreal realPenWidth() const { return data( DPenWidth, 1. ).toReal(); }

protected:
    virtual QRectF boundingRect() const;
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual void paint( QPainter *p, const QRectF &exposed );
    virtual void ideDragEnterEvent( QGraphicsSceneDragDropEvent *event );
    virtual void ideDragMoveEvent( QGraphicsSceneDragDropEvent *event );
    virtual void ideDragLeaveEvent( QGraphicsSceneDragDropEvent *event );
    virtual void ideDropEvent( QGraphicsSceneDragDropEvent *event );
    virtual void drawShape( QPainter *p, const QRectF &exposed )=0;
    virtual void saveItemData( QDataStream &out, int version );
    virtual void loadItemData( QDataStream &in, int version );
    virtual void phisCreateData( const PHIDataParser &parser );
    virtual void clientPrepareData();
    virtual void clientInitData();
    virtual void cssGraphicEffect( const PHIRequest *req, QByteArray &out, QByteArray &script ) const;
    virtual void squeeze();
    virtual PHIConfigWidget* ideConfigWidget();

    inline void setColorRole( PHIPalette::ColorRole cr ) { _colorRole=cr; }
    inline PHIPalette::ColorRole colorRole() const { return _colorRole; }
    inline void setOutlineColorRole( PHIPalette::ColorRole cr ) { _outlineColorRole=cr; }
    inline PHIPalette::ColorRole outlineColorRole() const { return _outlineColorRole; }

private:
    PHIPalette::ColorRole _colorRole, _outlineColorRole;
    static qreal _dropRegion;
};

class PHIEXPORT PHIAbstractImageItem : public PHIBaseItem
{
    Q_OBJECT

public:
    enum ItemData { DImage=-99, DTmpImage=-98 };
    explicit PHIAbstractImageItem( const PHIBaseItemPrivate &p ) : PHIBaseItem( p ) {}
    PHIAbstractImageItem( const PHIAbstractImageItem &it ) : PHIBaseItem( it ), _imageData( it._imageData ) {}
    virtual ~PHIAbstractImageItem() {}
    virtual bool hasImage() const { return true; }
    virtual bool isDraggable() const { return true; }
    virtual bool isDroppable() const { return true; }
    virtual PHIImageData* imageData() { return &_imageData; }
    QImage realImage() const { return data( DImage, QImage() ).value<QImage>(); }
    void setImage( const QImage &img ) { setData( DImage, img ); updateImage(); }
    void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;

public slots:
    QScriptValue src( const QScriptValue &v=QScriptValue() );

protected:
    virtual void ideUpdateData();
    virtual void squeeze();
    virtual void saveItemData( QDataStream &out, int version );
    virtual void loadItemData( QDataStream &in, int version );
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual void paint( QPainter *p, const QRectF &exposed );
    virtual void ideDragEnterEvent( QGraphicsSceneDragDropEvent *event );
    virtual void ideDragLeaveEvent( QGraphicsSceneDragDropEvent *event );
    virtual void ideDropEvent( QGraphicsSceneDragDropEvent *event );
    virtual void updateImage()=0;
    virtual void clientInitData();
    virtual void phisCreateData( const PHIDataParser &parser );
    virtual void phisParseData( const PHIDataParser &parser );
    virtual void cssGraphicEffect( const PHIRequest *req, QByteArray &out, QByteArray &script ) const;

private slots:
    void slotImageReady( const QImage &image );

private:
    PHIImageData _imageData;
};

class PHIEXPORT PHIAbstractImageBookItem : public PHIBaseItem
{
    Q_OBJECT
    Q_PROPERTY( PHIImageHash _images READ realImages WRITE setImages SCRIPTABLE false )

public:
    enum ItemData { DImages=-99, DTmpImages=-98 };
    explicit PHIAbstractImageBookItem( const PHIBaseItemPrivate &p ) : PHIBaseItem( p ) {}
    PHIAbstractImageBookItem( const PHIAbstractImageBookItem &it ) : PHIBaseItem( it ), _imageBookData( it._imageBookData ) {}
    virtual ~PHIAbstractImageBookItem() {}
    virtual bool hasImages() const { return true; }
    virtual bool isDroppable() const { return true; }
    virtual PHIImageBookData* imageBookData() { return &_imageBookData; }
    PHIImageHash realImages() const { return data( DImages ).value<PHIImageHash>(); }
    void setImages( const PHIImageHash &imgs ) { setData( DImages, qVariantFromValue( imgs ) ); updateImages(); }

protected:
    virtual void squeeze();
    virtual void ideUpdateData();
    virtual void saveItemData( QDataStream &out, int version );
    virtual void loadItemData( QDataStream &in, int version );
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual void ideDragEnterEvent( QGraphicsSceneDragDropEvent *event );
    virtual void ideDragLeaveEvent( QGraphicsSceneDragDropEvent *event );
    virtual void ideDropEvent( QGraphicsSceneDragDropEvent *event );
    virtual void updateImages()=0;
    virtual void paint( QPainter *painter, const QRectF &exposed );
    virtual void clientInitData();
    virtual void phisCreateData( const PHIDataParser &parser );
    virtual void phisParseData( const PHIDataParser &parser );
    virtual void cssGraphicEffect( const PHIRequest *req, QByteArray &out, QByteArray &script ) const;
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;

private slots:
    void slotImageReady( const QImage &image, int num );

private:
    PHIImageBookData _imageBookData;
};

class PHIEXPORT PHIAbstractLayoutItem : public PHIAbstractShapeItem
{
    Q_OBJECT
    Q_PROPERTY( quint16 _align READ realAlignment WRITE setAlignment SCRIPTABLE false )
    //Q_PROPERTY( qreal topLeftRadius READ topLeftRadius WRITE setTopLeftRadius )
    //Q_PROPERTY( qreal topRightRadius READ topRightRadius WRITE setTopRightRadius )
    //Q_PROPERTY( qreal bottomLeftRadius READ bottomLeftRadius WRITE setBottomLeftRadius )
    //Q_PROPERTY( qreal bottomRightRadius READ bottomRightRadius WRITE setBottomRightRadius )
    //Q_PROPERTY( qreal leftMargin READ leftMargin WRITE setLeftMargin )
    //Q_PROPERTY( qreal topMargin READ topMargin WRITE setTopMargin )
    //Q_PROPERTY( qreal rightMargin READ rightMargin WRITE setRightMargin )
    //Q_PROPERTY( qreal bottomMargin READ bottomMargin WRITE setBottomMargin )
    //Q_PROPERTY( qreal horizontalSpacing READ horizontalSpacing WRITE setHorizontalSpacing )
    //Q_PROPERTY( qreal verticalSpacing READ verticalSpacing WRITE setVerticalSpacing )
    //Q_PROPERTY( bool enableHeader READ enableHeader WRITE setEnableHeader )
    Q_PROPERTY( QString _header READ realHeader WRITE setHeader )

public:
    enum ItemData { DRadiusTopLeft=-99, DRadiusTopRight=-98, DRadiusBottomLeft=-97,
        DRadiusBottomRight=-96, DVerticalSpacing=-95, DHorizontalSpacing=-94,
        DPaddingLeft=-93, DPaddingTop=-93, DPaddingRight=-92, DPaddingBottom=-91,
        DBorderWidthLeft=-90, DBorderWidthTop=-89, DBorderWidthRight=-88,
        DBorderWidthBottom=-87, DMarginLeft=-86, DMarginTop=-85, DMarginRight=-84,
        DMarginBottom=-83, DAlignment=-82, DHeader=-81, DChildIds=-80, DChildRects=-79 };
    explicit PHIAbstractLayoutItem( const PHIBaseItemPrivate &p ) : PHIAbstractShapeItem( p ), _l( 0 ) { if ( isGuiItem() ) initLayout(); }
    PHIAbstractLayoutItem( const PHIAbstractLayoutItem &it ) : PHIAbstractShapeItem( it ), _l( 0 ),
        _textData( it._textData ) { if ( isGuiItem() ) initLayout(); } // _children must not be copied!
    virtual ~PHIAbstractLayoutItem() {}

    virtual bool isDroppable() const { return true; }
    virtual void addBaseItems( const QList <PHIBaseItem*> &list )=0;
    virtual void activateLayout()=0; // called once after page loading
    virtual void updateChildId( const QString &oldId, const QString &newId )=0;
    virtual void ideInit();
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;
    inline const QList<PHIBaseItem*>& childItems() const { return _children; }
    inline void setChildItems( const QList<PHIBaseItem*> &list ) { _children=list; }
    inline bool hasBorderRadius() const { return topLeftRadius()+topRightRadius()+bottomLeftRadius()+bottomRightRadius() > 0 ? true : false; }
    void breakLayout();
    void invalidateLayout();

    inline quint16 realAlignment() const { return data( DAlignment, static_cast<quint16>( Qt::AlignLeft | Qt::AlignVCenter ) ).value<quint16>(); }
    inline void setAlignment( quint16 align ) { setData( DAlignment, align ); update(); }
    inline qreal topLeftRadius() const { return data( DRadiusTopLeft, 0 ).toReal(); }
    inline void setTopLeftRadius( qreal r ) { setData( DRadiusTopLeft, r ); update(); }
    inline qreal topRightRadius() const { return data( DRadiusTopRight, 0 ).toReal(); }
    inline void setTopRightRadius( qreal r ) { setData( DRadiusTopRight, r ); update(); }
    inline qreal bottomRightRadius() const { return data( DRadiusBottomRight, 0 ).toReal(); }
    inline void setBottomRightRadius( qreal r ) { setData( DRadiusBottomRight, r ); update(); }
    inline qreal bottomLeftRadius() const { return data( DRadiusBottomLeft, 0 ).toReal(); }
    inline void setBottomLeftRadius( qreal r ) { setData( DRadiusBottomLeft, r ); update(); }
    inline qreal leftMargin() const { return data( DMarginLeft, 6 ).toReal(); }
    inline void setLeftMargin( qreal m ) { setData( DMarginLeft, m ); invalidateLayout(); }
    inline qreal topMargin() const { return data( DMarginTop, 6 ).toReal(); }
    inline void setTopMargin( qreal m ) { setData( DMarginTop, m ); invalidateLayout(); }
    inline qreal rightMargin() const { return data( DMarginRight, 6 ).toReal(); }
    inline void setRightMargin( qreal m ) { setData( DMarginRight, m ); invalidateLayout(); }
    inline qreal bottomMargin() const { return data( DMarginBottom, 6 ).toReal(); }
    inline void setBottomMargin( qreal m ) { setData( DMarginBottom, m ); invalidateLayout(); }
    inline qreal horizontalSpacing() const { return data( DHorizontalSpacing, 6 ).toReal(); }
    inline void setHorizontalSpacing( qreal h ) { setData( DHorizontalSpacing, h ); invalidateLayout(); }
    inline qreal verticalSpacing() const { return data( DVerticalSpacing, 6 ).toReal(); }
    inline void setVerticalSpacing( qreal v ) { setData( DVerticalSpacing, v ); invalidateLayout(); }
    inline bool enableHeader() const { return flags() & PHIBaseItem::FLayoutHeader; }
    inline void setEnableHeader( bool b ) { setFlag( PHIBaseItem::FLayoutHeader, b ); invalidateLayout(); }
    inline void setHeader( const QString &h ) { setData( DHeader, h ); update(); }
    inline QString realHeader() const { return data( DHeader ).toString(); }

public slots:
    QScriptValue textAlign( const QScriptValue &a=QScriptValue() );

protected:
    virtual QRectF boundingRect() const;
    virtual void squeeze();
    virtual void saveItemData( QDataStream &out, int version );
    virtual void loadItemData( QDataStream &in, int version );
    virtual bool hasText() const { return true; }
    virtual PHITextData* textData() { return &_textData; }
    virtual void paint( QPainter *p, const QRectF &exposed );
    virtual void drawShape( QPainter *p, const QRectF &exposed );
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual PHIConfigWidget* ideConfigWidget();
    virtual void phisParseData( const PHIDataParser &parser );
    virtual void phisCreateData( const PHIDataParser &parser );
    virtual void ideUpdateData();
    virtual void clientInitData();

    inline const QGraphicsGridLayout* layout() const { return _l; }
    void insertBaseItem( PHIBaseItem *it, int row, int column=0, int rowSpan=1, int columnSpan=1 );

private slots:
    void updateLayoutGeometry();

signals:
    void layoutChanged();

private:
    virtual void ideSetText( const QString &t, const QByteArray &lang );
    virtual QString ideText( const QByteArray &lang ) const;
    void setChildItem( PHIBaseItem *it );
    void releaseItem( PHIBaseItem *it );
    void initLayout();

    QGraphicsGridLayout *_l;
    QList <PHIBaseItem*> _children;
    PHITextData _textData;
};

class PHIEXPORT PHIAbstractInputItem : public PHIAbstractTextItem
{
    Q_OBJECT
    Q_PROPERTY( QString _accessKey WRITE setAccessKey READ realAccessKey SCRIPTABLE false )
    Q_PROPERTY( QString _value WRITE setValue READ realValue SCRIPTABLE false )

public:
    enum ItemData { DAccessKey=-99, DValue=-98, DMaxLength=-97 };
    explicit PHIAbstractInputItem( const PHIBaseItemPrivate &p ) : PHIAbstractTextItem( p ) {}
    PHIAbstractInputItem( const PHIAbstractInputItem &it ) : PHIAbstractTextItem( it ) {}
    virtual ~PHIAbstractInputItem() {}
    virtual bool isFocusable() const { return true; }
    virtual bool isDroppable() const { return true; }
    virtual bool isDraggable() const { return false; }
    virtual void clientPostData( QHttpMultiPart *multiPart ) const;
    virtual void reset();
    virtual void setValue( const QString &v ) { setText( v ); }
    virtual QString realValue() const { return realText(); }

public slots:
    virtual QScriptValue val( const QScriptValue &v=QScriptValue() );
    virtual QScriptValue readOnly( const QScriptValue &r=QScriptValue() );
    virtual QScriptValue accessKey( const QScriptValue &a=QScriptValue() );
    virtual QScriptValue color( const QScriptValue &c=QScriptValue() );
    virtual QScriptValue bgColor( const QScriptValue &c=QScriptValue() );

protected:
    virtual PHIBooleanData* readOnlyData() { return &_readOnlyData; }
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual void loadItemData( QDataStream &in, int version );
    virtual void saveItemData( QDataStream &out, int version );
    virtual void ideUpdateData();
    virtual void squeeze();
    virtual void phisParseData( const PHIDataParser &parser );
    virtual void phisCreateData( const PHIDataParser &parser );
    virtual void setWidgetAligment( Qt::Alignment align ) { Q_UNUSED( align ) }
    virtual void clientInitData();

signals:
    void submitClicked( const QString &id );
    void resetClicked( const QString &id );
    void langChangeRequested( const QString &lang );

private:
    PHIBooleanData _readOnlyData;
};

class PHIEXPORT PHIAbstractExternalItem : public PHIAbstractTextItem
{
    Q_OBJECT
    Q_PROPERTY( QString _accessKey WRITE setAccessKey READ realAccessKey SCRIPTABLE false )

public:
    enum ItemData { DAccessKey=-99 };
    explicit PHIAbstractExternalItem( const PHIBaseItemPrivate &p ) : PHIAbstractTextItem( p ), _webPage( 0 ) { if ( isGuiItem() ) initWidget(); }
    PHIAbstractExternalItem( const PHIAbstractExternalItem &it ) : PHIAbstractTextItem( it ), _webPage( 0 ) { if ( isGuiItem() ) initWidget(); }
    virtual ~PHIAbstractExternalItem() {}
    bool isFocusable() const { return true; }
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;

public slots:
    QScriptValue accessKey( const QScriptValue &v );

protected slots:
    void slotSizeChanged( const QSizeF &size );

protected:
    virtual void phisCreateData( const PHIDataParser &parser );
    virtual void phisParseData( const PHIDataParser &parser );
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;

    inline PHIWebPage* webPage() const { return _webPage; }
    QByteArray escapeChars( const QByteArray &a ) const;
    virtual QByteArray mapLanguage( const QByteArray &lang ) const;
    virtual QByteArray source() const=0;
    virtual void setWidgetAligment( Qt::Alignment a ) { Q_UNUSED( a ) }
    virtual void setWidgetText( const QString &t );
    virtual void ideInit();

private:
    void initWidget();
    PHIWebPage *_webPage;
};

#endif // PHIABSTRACTITEMS_H
