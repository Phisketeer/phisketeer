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
#ifndef PHIIMAGEITEMS_H
#define PHIIMAGEITEMS_H
#include "phiabstractitems.h"

class QTimer;
class QSvgRenderer;
class PHIContext2D;

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
    inline QString realText() const { return QString::fromLatin1( data( DSvgSource, QString() ).toByteArray() ); }

protected:
    virtual void paint( QPainter *painter, const QRectF &exposed );
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual void squeeze();
    virtual void loadItemData( QDataStream &in, int version );
    virtual void saveItemData( QDataStream &out, int version );
    virtual void ideUpdateData();
    virtual void phisCreateData( const PHIDataParser &parser );
    virtual void phisParseData( const PHIDataParser &parser );

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
    Q_PROPERTY( int fadeInterval READ fadeInterval WRITE setFadeInterval )
    Q_PROPERTY( int fadeTime READ fadeTime WRITE setFadeTime )
    Q_PROPERTY( int fadeIntervalMS READ fadeIntervalMS WRITE setFadeIntervalMS )
    Q_PROPERTY( int fadeTimeMS READ fadeTimeMS WRITE setFadeTimeMS )
    Q_PROPERTY( QStringList titles READ titles WRITE setTitles )

public:
    enum Wid { SlideShow=44 };
    enum ItemData { DInterval=1, DFadeTime=2, DCurrentOpacity=3, DCurrentStep=4, DCurrentImageNum=5, DTitles=6 };
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
    virtual PHIIntData* intData_1() { return &_intervalData; }
    virtual PHIIntData* intData_2() { return &_fadeTimeData; }

public slots:
    inline void setFadeInterval( int i ) { setData( DInterval, qMax( fadeTimeMS(), i*1000 ) ); updateImages(); }
    inline int fadeInterval() const { return data( DInterval, 4000 ).toInt()/1000; }
    inline void setFadeTime( int i ) { setData( DFadeTime, i*1000 ); updateImages(); }
    inline int fadeTime() const { return data( DFadeTime, 2000 ).toInt()/1000; }
    inline void setFadeIntervalMS( int i ) { setData( DInterval, qMax( fadeTimeMS(), i ) ); updateImages(); }
    inline int fadeIntervalMS() const { return data( DInterval, 4000 ).toInt(); }
    inline void setFadeTimeMS( int i ) { setData( DFadeTime, qMax( 50, i ) ); updateImages(); }
    inline int fadeTimeMS() const { return data( DFadeTime, 2000 ).toInt(); }
    inline void setTitles( const QStringList &l ) { setData( DTitles, l ); }
    inline QStringList titles() const { return data( DTitles ).toStringList(); }

protected:
    virtual void paint( QPainter *painter, const QRectF &exposed );
    virtual void updateImages();
    virtual void squeeze();
    virtual void loadItemData( QDataStream &in, int version );
    virtual void saveItemData( QDataStream &out, int version );
    virtual void ideInit();
    void initWidget();
    inline qreal step() const { return data( DCurrentStep, 50. ).toReal(); }
    inline void setStep( qreal s ) { setData( DCurrentStep, s ); }
    inline int currentImageNum() const { return data( DCurrentImageNum, 0 ).toInt(); }
    inline void setCurrentImageNum( int i ) { setData( DCurrentImageNum, i ); }
    inline qreal currentOpacity() const { return data( DCurrentOpacity, 1. ).toReal(); }
    inline void setCurrentOpacity( qreal o ) { setData( DCurrentOpacity, o ); }
    virtual void ideUpdateData();

protected slots:
    void pauseTimeout();
    void fadeTimeout();

private:
    PHIIntData _intervalData;
    PHIIntData _fadeTimeData;
    QTimer *_fadeTimer, *_pauseTimer;
};

class PHICanvasItem : public PHIBaseItem
{
    Q_OBJECT
    Q_PROPERTY( QString _text READ realText WRITE setText SCRIPTABLE false )

public:
    enum Wid { Canvas=61 };
    enum ItemData { DCanvasSource=100 };
    explicit PHICanvasItem( const PHIBaseItemPrivate &p ) : PHIBaseItem( p ), _ctx2D( 0 ) { if ( isGuiItem() ) initWidget(); }
    PHICanvasItem( const PHICanvasItem &it ) : PHIBaseItem( it ), _textData( it._textData ),
        _canvas( it._canvas ), _ctx2D( 0 ) { if ( isGuiItem() ) initWidget(); }
    virtual ~PHICanvasItem() {}

    virtual QString listName() const { return tr( "Canvas" ); }
    virtual QString description() const { return tr( "Creates a canvas 2D drawing space." ); }
    virtual PHIWID wid() const { return Canvas; }
    virtual QPixmap pixmap() const { return QPixmap( L1( ":/items/rect" ) ); }
    virtual void ideInit();
    virtual PHITextData* textData() { return &_textData; }
    virtual bool hasText() const { return true; }
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;

    inline void setText( const QString &s ) { setData( DCanvasSource, s.toLatin1() ); initWidget(); }
    inline QString realText() const { return QString::fromLatin1( data( DCanvasSource, QString() ).toByteArray() ); }

public slots:
    QScriptValue getContext( const QScriptValue &v );

protected:
    virtual void paint( QPainter *painter, const QRectF &exposed );
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual void squeeze();
    virtual void loadItemData( QDataStream &in, int version );
    virtual void saveItemData( QDataStream &out, int version );
    virtual void ideUpdateData();
    virtual void phisCreateData( const PHIDataParser &parser );
    virtual void phisParseData( const PHIDataParser &parser );

private slots:
    inline void imageChanged( const QImage &img ) { _canvas=img; qDebug()<< "image!!!"; }
    void slotSizeChanged( const QSizeF &s );

private:
    void initWidget();
    virtual void ideSetText( const QString &s, const QByteArray &lang );
    virtual QString ideText( const QByteArray &lang ) const;

    PHITextData _textData;
    QImage _canvas;
    PHIContext2D *_ctx2D;
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
    virtual QPixmap pixmap() const { return QPixmap(); }
    virtual bool isHeightChangeable() const { return false; }
    virtual bool isWidthChangeable() const { return false; }
    virtual bool isPrivateItem() const { return true; }
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;

protected:
    virtual void paint( QPainter *painter, const QRectF &exposed );
    virtual void squeeze();
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual void phisCreateData( const PHIDataParser &parser );

private:
    QImage _image;
};

#endif // PHIIMAGEITEMS_H
