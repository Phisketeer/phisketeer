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
#ifndef PHIGRAPHICTEXT_H
#define PHIGRAPHICTEXT_H
#include <QWidget>
#include <QImage>
#include "phiabstractitems.h"

class PHITextData;
class QGraphicsSimpleTextItem;

class PHIGraphicText : public PHIAbstractShapeItem
{
    Q_OBJECT
    Q_PROPERTY( QString text READ text WRITE setText )
    Q_PROPERTY( quint16 align READ alignment WRITE setAlignment )

public:
    enum Wid { GraphText=28 };
    enum ItemData { DText=1, DAlignment=2 };
    explicit PHIGraphicText( const PHIBaseItemPrivate &p );
    virtual ~PHIGraphicText();
    inline virtual PHIWID wid() const { return GraphText; }
    virtual QString listName() const { return tr( "Graphic text" ); }
    virtual QString description() const { return tr( "Draws colored graphical text." ); }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/graphtext" ) ); }
    virtual void initIDE();
    virtual void updateData();

public slots:
    inline void setText( const QString &t ) { setData( DText, t.toUtf8() ); }
    inline QString text() const { return QString::fromUtf8( data( DText ).toByteArray() ); }
    inline quint16 alignment() const { return data( DAlignment, static_cast<quint16>( Qt::AlignLeft | Qt::AlignVCenter ) ).value<quint16>(); }
    inline void setAlignment( quint16 align ) { setData( DAlignment, align ); }

protected:
    virtual bool hasText() const { return true; }
    virtual bool isWidthChangeable() const { return false; }
    virtual bool isHeightChangeable() const { return false; }
    virtual void setText( const QString &t, const QByteArray &lang );
    virtual QString text( const QByteArray &lang ) const;
    virtual PHITextData* textData() const { return _textData; }
    virtual PHIConfigWidget* configWidget();
    virtual void drawShape( QPainter *painter, const QRectF &r );
    virtual void saveItemData( QDataStream &out, int version );
    virtual void loadItemData( QDataStream &in, int version );
    virtual void squeeze();
    virtual QRectF boundingRect() const;
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;

private:
    PHITextData *_textData;
    QImage _image;
};

#endif // PHIGRAPHICTEXT_H
