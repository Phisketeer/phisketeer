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
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PHIGRAPHICTEXT_H
#define PHIGRAPHICTEXT_H
#include <QWidget>
#include <QImage>
#include "phiabstractitems.h"
#include "phidatasources.h"

class QGraphicsSimpleTextItem;

class PHIGraphicTextItem : public PHIAbstractShapeItem
{
    Q_OBJECT
    Q_PROPERTY( QString _text READ realText WRITE setText SCRIPTABLE false )
    Q_PROPERTY( quint16 _align READ realAlignment WRITE setAlignment SCRIPTABLE false )

public:
    enum Wid { GraphText=28 };
    enum ItemData { DText=1, DAlignment=2, DTmpImgPath=3 };
    explicit PHIGraphicTextItem( const PHIBaseItemPrivate &p ) : PHIAbstractShapeItem( p ) {}
    PHIGraphicTextItem( const PHIGraphicTextItem &it ) : PHIAbstractShapeItem( it ), _textData( it._textData ) {}
    virtual ~PHIGraphicTextItem() {}

    virtual PHIWID wid() const { return GraphText; }
    virtual QString listName() const { return tr( "Graphic text" ); }
    virtual QString description() const { return tr( "Draws colored graphical text." ); }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/graphtext" ) ); }
    virtual void ideInit();
    virtual PHITextData* textData() { return &_textData; }
    virtual bool useTextEditor() const { return true; }

    inline void setText( const QString &t ) { setData( DText, t.toUtf8() ); resize( graphicSize( t ) );
        if ( !isServerItem() ) _image=graphicImage( t ); update(); }
    inline QString realText() const { return QString::fromUtf8( data( DText ).toByteArray() ); }
    inline quint16 realAlignment() const { return data( DAlignment, static_cast<quint16>( Qt::AlignLeft | Qt::AlignVCenter ) ).value<quint16>(); }
    inline void setAlignment( quint16 align ) { setData( DAlignment, align );
        if ( !isServerItem() ) _image=graphicImage( realText() ); update(); }

public slots:

protected:
    QSizeF graphicSize( const QString &text ) const;
    QImage graphicImage( const QString &text ) const;

    virtual bool isWidthChangeable() const;
    virtual bool isHeightChangeable() const;
    virtual void drawShape( QPainter *painter, const QRectF &r );
    virtual void saveItemData( QDataStream &out, int version );
    virtual void loadItemData( QDataStream &in, int version );
    virtual void squeeze();
    virtual QRectF boundingRect() const;
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual void ideDragMoveEvent( QGraphicsSceneDragDropEvent *event );
    virtual void ideDragLeaveEvent( QGraphicsSceneDragDropEvent *event );
    virtual void ideDropEvent( QGraphicsSceneDragDropEvent *event );
    virtual void ideUpdateData();
    virtual void phisParseData( const PHIDataParser &parser );
    virtual void phisCreateData( const PHIDataParser &parser );
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;
    virtual void clientPrepareData();
    virtual void clientInitData();

private slots:
    void slotImageReady( const QImage &img );

private:
    virtual void ideSetText( const QString &t, const QByteArray &lang );
    virtual QString ideText( const QByteArray &lang ) const;
    virtual PHIConfigWidget* ideConfigWidget();

    PHITextData _textData;
    QImage _image;
};

#endif // PHIGRAPHICTEXT_H
