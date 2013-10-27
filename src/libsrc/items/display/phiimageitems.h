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
#include <QSvgRenderer>
#include "phiabstractitems.h"

class QTimer;

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
    Q_PROPERTY( QString text READ text WRITE setText )

public:
    enum Wid { Svg=42 };
    enum ItemData { DSvgSource=100 };
    explicit PHISvgItem( const PHIBaseItemPrivate &p ) : PHIBaseItem( p ) { if ( isGuiItem() ) initWidget(); }
    PHISvgItem( const PHISvgItem &it ) : PHIBaseItem( it ), _textData( it._textData ) { if ( isGuiItem() ) initWidget(); }
    virtual ~PHISvgItem() {}

    virtual QString listName() const { return tr( "Svg" ); }
    virtual QString description() const { return tr( "Displays scaleable vector graphics." ); }
    virtual PHIWID wid() const { return Svg; }
    virtual QPixmap pixmap() const { return QPixmap( L1( ":/items/roundedrect" ) ); }
    virtual void initIDE();
    inline virtual bool isHeightChangeable() const { return false; }
    inline virtual bool isWidthChangeable() const { return false; }
    inline PHITextData* textData() { return &_textData; }
    inline virtual bool hasText() const { return true; }

public slots:
    inline void setText( const QString &s ) { setData( DSvgSource, s.toLatin1() ); initWidget(); }
    inline QString text() const { return QString::fromLatin1( data( DSvgSource, QString() ).toByteArray() ); }

protected:
    virtual void paint( QPainter *painter, const QRectF &exposed );
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual void squeeze();
    virtual void loadItemData( QDataStream &in, int version );
    virtual void saveItemData( QDataStream &out, int version );
    virtual void updateData();

private:
    static QString svgDefaultSource();
    void initWidget();
    virtual void setText( const QString &s, const QByteArray &lang );
    virtual QString text( const QByteArray &lang ) const;

    PHITextData _textData;
    QSvgRenderer _renderer;
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
    virtual void initIDE();
    void initWidget();
    inline qreal step() const { return data( DCurrentStep, 50. ).toReal(); }
    inline void setStep( qreal s ) { setData( DCurrentStep, s ); }
    inline int currentImageNum() const { return data( DCurrentImageNum, 0 ).toInt(); }
    inline void setCurrentImageNum( int i ) { setData( DCurrentImageNum, i ); }
    inline qreal currentOpacity() const { return data( DCurrentOpacity, 1. ).toReal(); }
    inline void setCurrentOpacity( qreal o ) { setData( DCurrentOpacity, o ); }
    virtual void updateData();

protected slots:
    void pauseTimeout();
    void fadeTimeout();

private:
    PHIIntData _intervalData;
    PHIIntData _fadeTimeData;
    QTimer *_fadeTimer, *_pauseTimer;
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
    inline virtual bool isHeightChangeable() const { return false; }
    inline virtual bool isWidthChangeable() const { return false; }
    inline virtual bool isPrivateItem() const { return true; }

protected:
    virtual void paint( QPainter *painter, const QRectF &exposed );
    virtual void squeeze();
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;

private:
    QImage _image;
};

#endif // PHIIMAGEITEMS_H
