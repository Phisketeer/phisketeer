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
    virtual void updateData();
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

private:
    static QString svgDefaultSource();
    void initWidget();
    virtual void setText( const QString &s, const QByteArray &lang );
    virtual QString text( const QByteArray &lang ) const;

    PHITextData _textData;
    QSvgRenderer _renderer;
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
