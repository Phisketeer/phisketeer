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
#ifndef PHIIMAGEITEMS_H
#define PHIIMAGEITEMS_H
#include "phiabstractitems.h"
#include "phiconfigwidget.h"

class QTimer;
class QSvgRenderer;
class QLabel;
class QSpinBox;
class QToolButton;
class PHIContext2D;

class PHISlideShowConfig : public PHIConfigWidget
{
    Q_OBJECT

public:
    explicit PHISlideShowConfig( PHIBaseItem *it, QWidget *parent=0 );
    virtual ~PHISlideShowConfig();
    virtual bool storeData();
    virtual PHIConfigData originalData() const;

protected slots:
    void intervalTimeChanged( int );
    void fadeTimeChanged( int );
    void intervalToolClicked();
    void fadeToolClicked();

private:
    QLabel *_intervalLabel, *_fadeLabel;
    QSpinBox *_intervalSpin, *_fadeSpin;
    QToolButton *_intervalTool, *_fadeTool;
    PHIIntData *_orgIntervalData, *_orgFadeData;
};

class PHIImageItem : public PHIAbstractImageItem
{
    Q_OBJECT

public:
    enum Wid { Image=16 };
    explicit PHIImageItem( const PHIBaseItemPrivate &p ) : PHIAbstractImageItem( p ) {}
    PHIImageItem( const PHIImageItem &it ) : PHIAbstractImageItem( it ) {}
    virtual ~PHIImageItem() {}

    virtual QString listName() const { return tr( "Image" ); }
    virtual QString description() const { return tr( "Displays an image." ); }
    virtual PHIWID wid() const { return Image; }
    virtual QPixmap pixmap() const { return QPixmap( L1( ":/items/image" ) ); }

protected:
    virtual void updateImage();
};

class PHISvgItem : public PHIBaseItem
{
    Q_OBJECT
    Q_PROPERTY( QString _text READ realText WRITE setText SCRIPTABLE false )

public:
    enum Wid { Svg=42 };
    enum ItemData { DSvgSource=100 };
    explicit PHISvgItem( const PHIBaseItemPrivate &p ) : PHIBaseItem( p ), _renderer( 0 ) { if ( isGuiItem() ) initWidget(); }
    PHISvgItem( const PHISvgItem &it ) : PHIBaseItem( it ), _textData( it._textData ), _renderer( 0 ) { if ( isGuiItem() ) initWidget(); }
    virtual ~PHISvgItem() {}

    virtual QString listName() const { return tr( "Svg" ); }
    virtual QString description() const { return tr( "Displays scaleable vector graphics." ); }
    virtual PHIWID wid() const { return Svg; }
    virtual QPixmap pixmap() const { return QPixmap( L1( ":/items/roundedrect" ) ); }
    virtual void ideInit();
    virtual bool isHeightChangeable() const { return false; }
    virtual bool isWidthChangeable() const { return false; }
    virtual PHITextData* textData() { return &_textData; }
    virtual bool hasText() const { return true; }
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;

    inline void setText( const QString &s ) { setData( DSvgSource, s.toLatin1() ); initWidget(); }
    inline QString realText() const { return QString::fromLatin1( PHIBaseItem::data( DSvgSource, QString() ).toByteArray() ); }

public slots:
    QScriptValue data( const QScriptValue &v=QScriptValue() );

protected:
    virtual bool paint( QPainter *painter, const QRectF &exposed );
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual void squeeze();
    virtual void loadItemData( QDataStream &in, int version );
    virtual void saveItemData( QDataStream &out, int version );
    virtual void ideUpdateData();
    virtual void phisCreateData( const PHIDataParser &parser );
    virtual void phisParseData( const PHIDataParser &parser );
    virtual void clientInitData();

private:
    static QString svgDefaultSource();
    void initWidget();
    virtual void ideSetText( const QString &s, const QByteArray &lang );
    virtual QString ideText( const QByteArray &lang ) const;
    QImage graphicImage( const QByteArray &source ) const;

    PHITextData _textData;
    QSvgRenderer *_renderer;
};

class PHISlideShowItem : public PHIAbstractImageBookItem
{
    Q_OBJECT

public:
    enum Wid { SlideShow=44 };
    enum ItemData { DInterval=1, DFadeTime=2, DCurrentOpacity=3, DCurrentStep=4, DCurrentImageNum=5,
        DTitles=6, DTmpTitle=7 };
    explicit PHISlideShowItem( const PHIBaseItemPrivate &p ) : PHIAbstractImageBookItem( p ), _fadeTimer( 0 ),
        _pauseTimer( 0 ) { if ( isGuiItem() ) initWidget(); }
    PHISlideShowItem( const PHISlideShowItem &it ) : PHIAbstractImageBookItem( it ),
        _intervalData( it._intervalData ), _fadeTimeData( it._fadeTimeData ), _fadeTimer( 0 ),
        _pauseTimer( 0 ) { if ( isGuiItem() ) initWidget(); }
    virtual ~PHISlideShowItem() {}

    virtual QString listName() const { return tr( "Slide show" ); }
    virtual QString description() const { return tr( "Displays a slide show with changing images." ); }
    virtual PHIWID wid() const { return SlideShow; }
    virtual QPixmap pixmap() const { return QPixmap( L1( ":/items/imagebutton" ) ); }
    inline void setTitleList( const QString &toolTip ) { setData( DTitles, toolTip.split( QLatin1Char( ':' ), QString::KeepEmptyParts ) ); }
    inline QString titleList() const { return data( DTitles ).toStringList().join( L1( ":" ) ); }
    virtual PHIIntData* intData_1() { return &_fadeTimeData; }
    virtual PHIIntData* intData_2() { return &_intervalData; }
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;

    inline void setFadeInterval( int i ) { setData( DInterval, qMax( realFadeTimeMS(), i*1000 ) ); updateImages(); }
    inline int realFadeInterval() const { return data( DInterval, 4000 ).toInt()/1000; }
    inline void setFadeTime( int i ) { setData( DFadeTime, i*1000 ); updateImages(); }
    inline int realFadeTime() const { return data( DFadeTime, 2000 ).toInt()/1000; }
    inline void setFadeIntervalMS( int i ) { setData( DInterval, qMax( realFadeTimeMS(), i ) ); updateImages(); }
    inline int realFadeIntervalMS() const { return data( DInterval, 4000 ).toInt(); }
    inline void setFadeTimeMS( int i ) { setData( DFadeTime, qMax( 50, i ) ); updateImages(); }
    inline int realFadeTimeMS() const { return data( DFadeTime, 2000 ).toInt(); }
    // @todo: replace QStringList with QByteArrayList:
    inline void setTitles( const QStringList &l ) { setData( DTitles, l ); }
    inline QStringList realTitles() const { return data( DTitles ).toStringList(); }

public slots:
    QScriptValue fadeInterval( const QScriptValue &fi );
    QScriptValue fadeIntervalMS( const QScriptValue &fi );
    QScriptValue fadeTime( const QScriptValue &fi );
    QScriptValue fadeTimeMS( const QScriptValue &fi );
    QScriptValue titles( const QScriptValue &t );
    QScriptValue start();
    QScriptValue stop();
    QScriptValue display( const QScriptValue &c );
    QScriptValue count() const { return realImages().count(); }

protected:
    virtual bool paint( QPainter *painter, const QRectF &exposed );
    virtual void updateImages();
    virtual void squeeze();
    virtual void loadItemData( QDataStream &in, int version );
    virtual void saveItemData( QDataStream &out, int version );
    virtual void ideInit();
    inline qreal step() const { return data( DCurrentStep, 50. ).toReal(); }
    inline void setStep( qreal s ) { setData( DCurrentStep, s ); }
    inline int currentImageNum() const { return data( DCurrentImageNum, 0 ).toInt(); }
    inline void setCurrentImageNum( int i ) { setData( DCurrentImageNum, i ); }
    inline qreal currentOpacity() const { return data( DCurrentOpacity, 1. ).toReal(); }
    inline void setCurrentOpacity( qreal o ) { setData( DCurrentOpacity, o ); }
    virtual void ideUpdateData();
    virtual void phisCreateData( const PHIDataParser &parser );
    virtual void phisParseData( const PHIDataParser &parser );

protected slots:
    void pauseTimeout();
    void fadeTimeout();

private:
    virtual PHIConfigWidget* ideConfigWidget();
    void initWidget();
    PHIIntData _intervalData;
    PHIIntData _fadeTimeData;
    QTimer *_fadeTimer, *_pauseTimer;
};

class PHICanvasItem : public PHIBaseItem
{
    Q_OBJECT
    Q_PROPERTY( QString _text READ realText WRITE setText SCRIPTABLE false )
    Q_PROPERTY( QImage _image READ realImage WRITE setImage SCRIPTABLE false )

public:
    enum Wid { Canvas=61 };
    enum ItemData { DCanvasSource=100, DCanvasImage=101 };
    explicit PHICanvasItem( const PHIBaseItemPrivate &p ) : PHIBaseItem( p ), _ctx2D( 0 ) { if ( isGuiItem() ) initWidget(); }
    PHICanvasItem( const PHICanvasItem &it ) : PHIBaseItem( it ), _textData( it._textData ), _ctx2D( 0 ) { if ( isGuiItem() ) initWidget(); }
    virtual ~PHICanvasItem() {}

    virtual QString listName() const { return tr( "Canvas" ); }
    virtual QString description() const { return tr( "Creates a canvas 2D drawing space." ); }
    virtual PHIWID wid() const { return Canvas; }
    virtual QPixmap pixmap() const { return QPixmap( L1( ":/items/rect" ) ); }
    virtual PHITextData* textData() { return &_textData; }
    virtual bool hasText() const { return true; }
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;

    inline void setText( const QString &s ) { setData( DCanvasSource, s.toLatin1() ); initWidget(); }
    inline QString realText() const { return QString::fromLatin1( data( DCanvasSource, QString() ).toByteArray() ); }
    inline QImage realImage() const { return data( DCanvasImage ).value<QImage>(); }

public slots:
    QScriptValue getContext( const QScriptValue &v );

protected slots:
    inline void setImage( const QImage &img ) { setData( DCanvasImage, img ); update(); }
    void slotSizeChanged( const QSizeF &s );

protected:
    virtual bool paint( QPainter *painter, const QRectF &exposed );
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual void squeeze();
    virtual void loadItemData( QDataStream &in, int version );
    virtual void saveItemData( QDataStream &out, int version );
    virtual void ideUpdateData();
    virtual void phisCreateData( const PHIDataParser &parser );
    virtual void phisParseData( const PHIDataParser &parser );
    virtual void clientInitData();

private:
    void initWidget();
    virtual void ideSetText( const QString &s, const QByteArray &lang );
    virtual QString ideText( const QByteArray &lang ) const;

    PHITextData _textData;
    PHIContext2D *_ctx2D;
};

class PHIRolloverItem : public PHIAbstractShapeItem
{
    Q_OBJECT
    Q_PROPERTY( QString _text READ realText WRITE setText SCRIPTABLE false )
    Q_PROPERTY( PHIImageHash _images READ realImages WRITE setImages SCRIPTABLE false )
    Q_PROPERTY( QString _url READ realUrl WRITE setUrl SCRIPTABLE false )
    Q_PROPERTY( QString _accessKey READ realAccessKey WRITE setAccessKey SCRIPTABLE false )

public:
    enum Wid { Rollover=37 };
    enum ItemData { DText=1, DImages=2, DUrl=3, DHoverColor=4, DHoverBgColor=5, DTmpHoverRole=6, DTmpHoverBgRole=7 };
    explicit PHIRolloverItem( const PHIBaseItemPrivate &p ) : PHIAbstractShapeItem( p ), _hover( false ) {}
    PHIRolloverItem( const PHIRolloverItem &it ) : PHIAbstractShapeItem( it ),
        _textData( it._textData ), _imageBookData( it._imageBookData ),
        _hoverColorRole( it._hoverColorRole ), _hoverBgColorRole( it._hoverBgColorRole ), _hover( false ) {}
    virtual ~PHIRolloverItem() {}

    virtual QString listName() const { return tr( "Rollover" ); }
    virtual QString description() const { return tr( "Creates a button with rollover effect." ); }
    virtual PHIWID wid() const { return Rollover; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/rollover" ) ); }
    virtual bool hasUrl() const { return true; }
    virtual bool isFocusable() const { return true; }
    virtual QColor colorForRole( PHIPalette::ItemRole role ) const;
    virtual PHIPalette::ColorRole colorRole( PHIPalette::ItemRole role ) const;
    QColor realHoverColor() const;
    QColor realHoverBgColor() const;

    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;
    virtual bool hasText() const { return true; }
    virtual PHITextData* textData() { return &_textData; }
    virtual bool hasImages() const { return true; }
    virtual PHIImageBookData* imageBookData() { return &_imageBookData; }
    inline PHIImageHash realImages() const { return data( DImages ).value<PHIImageHash>(); }
    inline void setImages( const PHIImageHash &imgs ) { setData( DImages, qVariantFromValue( imgs ) ); update(); }
    inline QString realText() const { return QString::fromUtf8( data( DText ).toByteArray() ); }
    inline void setText( const QString &s ) { setData( DText, s.toUtf8() ); update(); }
    inline QString realUrl() const { return QString::fromUtf8( data( DUrl ).toByteArray() ); }
    inline void setUrl( const QString &url ) { setData( DUrl, url.toUtf8() ); }
    inline PHIVariantHash imageLangPathes() const { return data( DImages ).value<PHIVariantHash>(); }
    inline void setImageLangPathes( const PHIVariantHash &v ) { setData( DImages, qVariantFromValue( v ) ); }

public slots:
    QScriptValue url( const QScriptValue &v=QScriptValue() );
    QScriptValue text( const QScriptValue &v=QScriptValue() );

protected:
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual void drawShape( QPainter *p, const QRectF &exposed );
    virtual void setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col );
    virtual void ideInit();
    virtual void squeeze();
    virtual void saveItemData( QDataStream &out, int version );
    virtual void loadItemData( QDataStream &in, int version );
    virtual void ideUpdateData();
    virtual void clientPrepareData();
    virtual void clientInitData();
    virtual void phisCreateData( const PHIDataParser &parser );
    virtual void phisParseData( const PHIDataParser &parser );
    virtual void click( const QGraphicsSceneMouseEvent *e );
    virtual void mouseover( const QGraphicsSceneHoverEvent *e );
    virtual void mouseout( const QGraphicsSceneHoverEvent *e );

private slots:
    void slotImageReady( const QImage &img, int i );

private:
    virtual void ideSetText( const QString &t, const QByteArray &lang );
    virtual QString ideText( const QByteArray &lang ) const;
    virtual PHIConfigWidget* ideConfigWidget();
    QImage createImage( const QString &t, const QImage &img, const QColor &col, const QColor &bgCol ) const;
    QString resolvePath( const PHIRequest *req, const QString &path, QByteArray &lang );

    PHITextData _textData;
    PHIImageBookData _imageBookData;
    PHIPalette::ColorRole _hoverColorRole, _hoverBgColorRole;
    bool _hover;
};

class PHISponsorItem : public PHIBaseItem
{
    Q_OBJECT

public:
    enum Wid { Sponsor=48 };
    explicit PHISponsorItem( const PHIBaseItemPrivate &p ) : PHIBaseItem( p ), _image( L1( ":/sponsor" ) ) {}
    PHISponsorItem( const PHISponsorItem &it ) : PHIBaseItem( it ), _image( it._image ) {}
    virtual ~PHISponsorItem() {}

    virtual QString listName() const { return tr( "Sponsor" ); }
    virtual QString description() const { return tr( "Displays the 'powered by' logo." ); }
    virtual PHIWID wid() const { return Sponsor; }
    virtual QPixmap pixmap() const { return QPixmap( L1( ":/items/imagebutton" ) ); }
    virtual bool isHeightChangeable() const { return false; }
    virtual bool isWidthChangeable() const { return false; }
    virtual bool isPrivateItem() const { return true; }
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;

protected:
    virtual bool paint( QPainter *painter, const QRectF &exposed );
    virtual void squeeze();
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual void phisCreateData( const PHIDataParser &parser );

private:
    QImage _image;
};

#endif // PHIIMAGEITEMS_H
