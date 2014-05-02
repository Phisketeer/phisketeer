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
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include "phigraphictext.h"
#include "phicolorconfig.h"
#include "phibasepage.h"
#include "phidataparser.h"
#include "phiimagerequest.h"

void PHIGraphicTextItem::ideInit()
{
    PHIAbstractShapeItem::ideInit();
    _textData.setText( tr( "Graphical text" ) );
    QFont f=font();
    f.setPointSizeF( 28. );
    setFont( f );
}

void PHIGraphicTextItem::ideUpdateData()
{
    QString t=ideText( page()->currentLang() );
    setText( t );
    QSizeF s( 16., 16. );
    if ( _textData.isUnparsedStatic() ) resize( graphicSize( t ) );
    else if ( _textData.isUnparsedTranslated() ) {
        foreach( QByteArray l, _textData.langs() ) {
            QSizeF ls=graphicSize( _textData.text( l ) );
            if ( ls.width()>s.width() ) s.setWidth( ls.width() );
            if ( ls.height()>s.height() ) s.setHeight( ls.height() );
        }
        resize( s );
    }
    _image=graphicImage( t );
    update();
}

QSizeF PHIGraphicTextItem::graphicSize( const QString &t ) const
{
    QGraphicsSimpleTextItem it;
    QFont f=font();
    f.setPointSizeF( PHI::adjustedFontSize( f.pointSizeF() ) );
    it.setFont( f );
    QPen pen;
    Qt::PenStyle style=static_cast<Qt::PenStyle>(realLine());
    Qt::BrushStyle pat=static_cast<Qt::BrushStyle>(realPattern());
    if ( style!=Qt::NoPen ) {
        pen.setColor( realOutlineColor() );
        pen.setWidthF( realPenWidth() );
        pen.setJoinStyle( Qt::MiterJoin );
        pen.setCapStyle( Qt::FlatCap );
        pen.setStyle( style );
    } else pen.setStyle( Qt::NoPen );
    QBrush brush;
    if ( realPattern()<15 ) {
        brush.setStyle( pat );
        brush.setColor( realColor() );
    } else brush=QBrush( gradient() );
    it.setPen( pen );
    it.setBrush( brush );
    it.setText( t );
    QSizeF s=it.boundingRect().size();
    if ( realLine()>0 ) s+=QSizeF( realPenWidth(), realPenWidth() );
    return s;
}

QImage PHIGraphicTextItem::graphicImage( const QString &t ) const
{
    QGraphicsSimpleTextItem it;
    QStyleOptionGraphicsItem opt;
    opt.state=QStyle::State_None;
    opt.exposedRect=QRectF();
    QFont f=font();
    f.setPointSizeF( PHI::adjustedFontSize( f.pointSizeF() ) );
    it.setFont( f );
    QPen pen;
    Qt::PenStyle style=static_cast<Qt::PenStyle>(realLine());
    Qt::BrushStyle pat=static_cast<Qt::BrushStyle>(realPattern());
    if ( style!=Qt::NoPen ) {
        pen.setColor( realOutlineColor() );
        pen.setWidthF( realPenWidth() );
        pen.setJoinStyle( Qt::MiterJoin );
        pen.setCapStyle( Qt::FlatCap );
        pen.setStyle( style );
    } else pen.setStyle( Qt::NoPen );
    QBrush brush;
    if ( realPattern()<15 ) {
        brush.setStyle( pat );
        brush.setColor( realColor() );
    } else brush=QBrush( gradient() );
    it.setPen( pen );
    it.setBrush( brush );
    it.setText( t );
    QSizeF s=it.boundingRect().size();
    if ( realLine()>0 ) s+=QSizeF( realPenWidth(), realPenWidth() );
    QImage img( s.toSize(), QImage::Format_ARGB32_Premultiplied );
    img.fill( 0 );
    QPainter p( &img );
    p.translate( realPenWidth()/2., realPenWidth()/2. );
    p.setRenderHint( QPainter::TextAntialiasing, true );
    p.setRenderHint( QPainter::Antialiasing, true );
    it.paint( &p, &opt, 0 );
    p.end();
    if ( img.width()>realWidth() ) img=img.scaled( realSize().toSize(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
    else if ( img.height()>realHeight() ) img=img.scaledToHeight( realHeight(), Qt::SmoothTransformation );
    QImage dest( realSize().toSize(), QImage::Format_ARGB32_Premultiplied );
    dest.fill( 0 );
    qreal offx( 0 );
    Qt::Alignment align=static_cast<Qt::Alignment>(realAlignment());
    if ( align & Qt::AlignRight ) offx=realWidth()-img.width();
    else if ( align & Qt::AlignCenter ) offx=(realWidth()-img.width())/2.;
    p.begin( &dest );
    p.drawImage( QPointF( offx, 0 ), img );
    p.end();
    return dest;
}

QRectF PHIGraphicTextItem::boundingRect() const
{
    return rect();
}

void PHIGraphicTextItem::phisCreateData( const PHIDataParser &parser )
{
    setData( DText, parser.text( &_textData ) );
    if ( _textData.isUnparsedStatic() ) {
        setImagePath( parser.createImage( graphicImage( _textData.text() ) ) );
    } else if ( _textData.isUnparsedTranslated() ) {
        foreach ( QByteArray l, _textData.langs() ) {
            QByteArray path=parser.createImage( graphicImage( _textData.text( l ) ), l );
            qDebug() << id() << path << l;
        }
    } else setDirtyFlag( DFText );
}

void PHIGraphicTextItem::phisParseData( const PHIDataParser &parser )
{
    if ( Q_UNLIKELY( dirtyFlags() & DFText ) ) {
        setData( DText, parser.text( &_textData ) );
        // need to create dynamic uncached image
        setImagePath( parser.createImage( graphicImage( realText() ), PHIData::c(), -1 ) );
    } else {
        if ( _textData.isUnparsedStatic() ) {
            setData( DText, _textData.variant() );
        } else {
            Q_ASSERT( _textData.isUnparsedTranslated() );
            setImagePath( parser.imagePath( page()->currentLang() ) );
        }
    }
}

void PHIGraphicTextItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    PHIBaseItem::htmlImg( req, out, script, indent );
}

void PHIGraphicTextItem::clientPrepareData()
{
    setData( DTmpImgPath, imagePath() );
}

void PHIGraphicTextItem::clientInitData()
{
    setData( DIsImage, true );
    QUrl url=page()->baseUrl();
    if ( imagePath().startsWith( '/' ) ) {
        url.setPath( QString::fromUtf8( data( DTmpImgPath ).toByteArray() ) );
    } else {
        url.setPath( L1( "/phi.phis" ) );
        QUrlQuery query;
        query.addQueryItem( L1( "i" ), QString::fromUtf8( data( DTmpImgPath ).toByteArray() ) );
        query.addQueryItem( L1( "t" ) , L1( "1" ) );
        url.setQuery( query );
    }
    PHIImageRequest *req=new PHIImageRequest( this, url );
    connect( req, &PHIImageRequest::imageReady, this, &PHIGraphicTextItem::slotImageReady );
}

void PHIGraphicTextItem::slotImageReady( const QImage &img )
{
    _image=img;
    update();
}

void PHIGraphicTextItem::drawShape( QPainter *p, const QRectF &r )
{
    Q_UNUSED( r )
    qreal xoff=0, yoff=0;
    Qt::Alignment a=static_cast<Qt::Alignment>(realAlignment());
    if ( a & Qt::AlignLeft ) {
        yoff=(realHeight()-_image.size().height())/2.;
    } else if ( a & Qt::AlignHCenter ) {
        xoff=(realWidth()-_image.size().width())/2.;
        yoff=(realHeight()-_image.size().height())/2.;
    } else if ( a & Qt::AlignRight ) {
        xoff=(realWidth()-_image.size().width());
        yoff=(realHeight()-_image.size().height())/2.;
    }
    p->drawImage( xoff, yoff, _image );
}

void PHIGraphicTextItem::ideSetText( const QString &t, const QByteArray &lang )
{
    if ( _textData.isTranslated() ) {
        _textData.setText( t, lang );
    } else if ( _textData.isStatic() ) {
        _textData.setSource( PHIData::Translated );
        _textData.setText( t, lang );
    }
    if ( isChild() ) {
        PHIBaseItem *it=page()->findItem( parentId() );
        PHIAbstractLayoutItem *lit=qobject_cast<PHIAbstractLayoutItem*>(it);
        if ( lit ) lit->invalidateLayout();
    }
}

bool PHIGraphicTextItem::isWidthChangeable() const
{
    if ( _textData.isUnparsedStatic() || _textData.isUnparsedTranslated() ) return false;
    return true;
}

bool PHIGraphicTextItem::isHeightChangeable() const
{
    if ( _textData.isUnparsedStatic() || _textData.isUnparsedTranslated() ) return false;
    return true;
}

QString PHIGraphicTextItem::ideText( const QByteArray &lang ) const
{
    if ( _textData.isTranslated() ) return _textData.text( lang );
    return _textData.text();
}

void PHIGraphicTextItem::loadItemData( QDataStream &in, int version )
{
    PHIAbstractShapeItem::loadItemData( in, version );
    in >> &_textData;
}

void PHIGraphicTextItem::saveItemData( QDataStream &out, int version )
{
    PHIAbstractShapeItem::saveItemData( out, version );
    out << &_textData;
}

void PHIGraphicTextItem::squeeze()
{
    PHIAbstractShapeItem::squeeze();
    removeData( DText );
    if ( realAlignment()==static_cast<quint16>(Qt::AlignLeft|Qt::AlignVCenter) ) removeData( DAlignment );
}

QSizeF PHIGraphicTextItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    if ( isChild() || !isIdeItem() ) return rect().size();
    if ( _textData.isUnparsedStatic() || _textData.isUnparsedTranslated() ) return rect().size();
    return PHIAbstractShapeItem::sizeHint( which, constraint );
}

void PHIGraphicTextItem::ideDragMoveEvent( QGraphicsSceneDragDropEvent *event )
{
    PHIAbstractShapeItem::ideDragMoveEvent( event );
    _image=graphicImage( realText() );
    update();
}

void PHIGraphicTextItem::ideDragLeaveEvent( QGraphicsSceneDragDropEvent *event )
{
    PHIAbstractShapeItem::ideDragLeaveEvent( event );
    _image=graphicImage( realText() );
    update();
}

void PHIGraphicTextItem::ideDropEvent( QGraphicsSceneDragDropEvent *event )
{
    PHIAbstractShapeItem::ideDropEvent( event );
    _image=graphicImage( realText() );
    update();
}

PHIConfigWidget* PHIGraphicTextItem::ideConfigWidget()
{
    return new PHIColorConfig( this );
}
