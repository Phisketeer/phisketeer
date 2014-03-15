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
#ifndef PHITEXTITEMS_H
#define PHITEXTITEMS_H
#include "phiabstractitems.h"

class PHILabelItem : public PHIAbstractTextItem
{
    Q_OBJECT

public:
    enum Wid { Label=30 };
    enum ItemData { DHtmlText=1 };
    explicit PHILabelItem( const PHIBaseItemPrivate &p ) : PHIAbstractTextItem( p ) { if ( isGuiItem() ) initWidget(); }
    PHILabelItem( const PHILabelItem &it ) : PHIAbstractTextItem( it ) { if ( isGuiItem() ) initWidget(); }
    virtual ~PHILabelItem() {}

    virtual QString listName() const { return tr( "Label" ); }
    virtual QString description() const { return tr( "Displays a label." ); }
    virtual PHIWID wid() const { return Label; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/label" ) ); }
    virtual void setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col );
    virtual void ideInit();
    virtual bool useTextEditor() const { return true; }
    bool isHtmlText() const { return data( DHtmlText, false ).toBool(); }
    void setHtmlText( bool b ) { if ( !b ) removeData( DHtmlText ); else setData( DHtmlText, b ); }

protected slots:
    QScriptValue text( const QScriptValue &t=QScriptValue() );
    QScriptValue html( const QScriptValue &h=QScriptValue() );

protected:
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;
    virtual void setWidgetText( const QString &t );
    virtual void setWidgetAligment( Qt::Alignment align );
    virtual void cssStatic( const PHIRequest *req, QByteArray &out ) const;
    virtual void cssGraphicEffect( const PHIRequest *req, QByteArray &out, QByteArray &script ) const;

private slots:
    void slotAnchorClicked( const QUrl &url );
    void slotAnchorHover( const QUrl &url );

private:
    void initWidget();
};

class PHILinkItem : public PHILabelItem
{
    Q_OBJECT
    Q_PROPERTY( QString _url READ realUrl WRITE setUrl SCRIPTABLE false )
    Q_PROPERTY( QString _accessKey WRITE setAccessKey READ realAccessKey SCRIPTABLE false )

public:
    enum Wid { Link=23 };
    enum ItemData { DHoverColor=1, DHoverBgColor=2, DTmpHoverRole=3, DTmpHoverBgRole=4, DUrl=5 };
    explicit PHILinkItem( const PHIBaseItemPrivate &p ) : PHILabelItem( p ) {}
    PHILinkItem( const PHILinkItem &it ) : PHILabelItem( it ), _hoverColorRole( it._hoverColorRole ),
        _hoverBackgroundColorRole( it._hoverBackgroundColorRole ) {}
    virtual ~PHILinkItem() {}

    virtual QString listName() const { return tr( "Link" ); }
    virtual QString description() const { return tr( "Creates a link item with hover effect." ); }
    virtual PHIWID wid() const { return Link; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/button" ) ); }
    virtual bool hasUrl() const { return true; }
    virtual bool isFocusable() const { return false; } // @todo: enable later with accessKey()
    virtual PHIPalette::ColorRole colorRole( PHIPalette::ItemRole role ) const;
    virtual QColor colorForRole( PHIPalette::ItemRole role ) const;

    QColor realHoverColor() const;
    QColor realHoverBgColor() const;
    QString realUrl() const { return QString::fromUtf8( data( DUrl, QString() ).toByteArray() ); }
    void setUrl( const QString &url ) { setData( DUrl, url.toUtf8() ); }
    void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;

public slots:
    QScriptValue url( const QScriptValue &v=QScriptValue() );

protected:
    virtual void saveItemData( QDataStream &out, int version );
    virtual void loadItemData( QDataStream &in, int version );
    virtual void ideInit();
    virtual void setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col );
    virtual void mouseover( const QGraphicsSceneHoverEvent *e );
    virtual void mouseout( const QGraphicsSceneHoverEvent *e );
    virtual void click( const QGraphicsSceneMouseEvent *e );
    virtual void clientPrepareData();
    virtual void clientInitData();

private:
    virtual PHIConfigWidget* ideConfigWidget();
    PHIPalette::ColorRole _hoverColorRole, _hoverBackgroundColorRole;
};

class PHIGraphicRichTextItem : public PHIAbstractTextItem
{
    Q_OBJECT

public:
    enum Wid { GraphicRichText=29 };
    enum ItemData { DImage=1 };
    explicit PHIGraphicRichTextItem( const PHIBaseItemPrivate &p ) : PHIAbstractTextItem( p ) { if ( isGuiItem() ) initWidget(); }
    PHIGraphicRichTextItem( const PHILabelItem &it ) : PHIAbstractTextItem( it ) { if ( isGuiItem() ) initWidget(); }
    virtual ~PHIGraphicRichTextItem() {}

    virtual QString listName() const { return tr( "Graphic rich text" ); }
    virtual QString description() const { return tr( "Displays rich text rendered into an image." ); }
    virtual PHIWID wid() const { return GraphicRichText; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/richtext" ) ); }
    virtual void ideInit();
    virtual bool isHeightChangeable() const;
    virtual void setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col );

    inline QImage image() const { return data( DImage ).value<QImage>(); }
    inline void setImage( const QImage &img ) { setData( DImage, img ); update(); }

public slots:
    QScriptValue html( const QScriptValue &h=QScriptValue() );
    virtual QScriptValue color( const QScriptValue &c=QScriptValue() );
    virtual QScriptValue bgColor( const QScriptValue &c=QScriptValue() );

protected:
    virtual void phisCreateData( const PHIDataParser &parser );
    virtual void phisParseData( const PHIDataParser &parser );
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;
    virtual void setWidgetText( const QString &t );
    virtual void setWidgetAligment( Qt::Alignment align );
    virtual bool paint( QPainter *painter, const QRectF &exposed );
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual void squeeze();
    virtual void ideUpdateData();

private slots:
    void slotSizeChanged( const QSizeF &s );

private:
    void initWidget();

private:
    QImage graphicImage( const QString &text ) const;
    QSizeF graphicSize( const QString &text ) const;
};

class PHIRichTextItem : public PHIAbstractTextItem
{
    Q_OBJECT

public:
    enum Wid { RichText=41 };
    explicit PHIRichTextItem( const PHIBaseItemPrivate &p ) : PHIAbstractTextItem( p ) { if ( isGuiItem() ) initWidget(); }
    PHIRichTextItem( const PHILabelItem &it ) : PHIAbstractTextItem( it ) { if ( isGuiItem() ) initWidget(); }
    virtual ~PHIRichTextItem() {}

    virtual QString listName() const { return tr( "Rich text" ); }
    virtual QString description() const { return tr( "Displays rich text (HTML subset)." ); }
    virtual PHIWID wid() const { return RichText; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/richtext" ) ); }
    virtual bool isFocusable() const { return true; }
    virtual void ideInit();

public slots:
    QScriptValue html( const QScriptValue &h=QScriptValue() );

protected:
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;
    virtual void setWidgetText( const QString &t );
    virtual void setWidgetAligment( Qt::Alignment align );
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual void initWidget();
    virtual void cssStatic( const PHIRequest *req, QByteArray &out ) const;
    virtual void cssGraphicEffect( const PHIRequest *req, QByteArray &out, QByteArray &script ) const;

private slots:
    void slotAnchorClicked( const QUrl &url );
    void slotAnchorHover( const QUrl &url );
};

#endif // PHITEXTITEMS_H
