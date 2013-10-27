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
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include "phigraphictext.h"
#include "phicolorconfig.h"
#include "phibasepage.h"

void PHIGraphicTextItem::initIDE()
{
    PHIAbstractShapeItem::initIDE();
    _textData.setText( tr( "Graphical text" ) );
    QFont f=font();
    f.setPointSizeF( 28. );
    setFont( f );
    updateGeometry();
}

void PHIGraphicTextItem::updateData()
{
    updateGeometry();
}

void PHIGraphicTextItem::updateGeometry()
{
    QGraphicsSimpleTextItem it;
    QFont f=font();
    f.setPointSizeF( PHI::adjustedFontSize( f.pointSizeF() ) );
    it.setFont( f );
    QPen pen;
    Qt::PenStyle style=static_cast<Qt::PenStyle>(line());
    Qt::BrushStyle pat=static_cast<Qt::BrushStyle>(pattern());
    if ( style!=Qt::NoPen ) {
        pen.setColor( outlineColor() );
        pen.setWidthF( penWidth() );
        pen.setJoinStyle( Qt::MiterJoin );
        pen.setCapStyle( Qt::FlatCap );
        pen.setStyle( style );
    } else pen.setStyle( Qt::NoPen );
    QBrush brush;
    if ( pattern()<15 ) { // no gradient style
        brush.setStyle( pat );
        brush.setColor( color() );
    } else brush=QBrush( gradient() );
    it.setPen( pen );
    it.setBrush( brush );
    QSizeF s=QSizeF( 64., 22. );
    if ( isIdeItem() ) {
        foreach ( QString lang, page()->languages() ) {
            it.setText( text( lang.toLatin1() ) );
            s.setWidth( qMax( it.boundingRect().size().width(), s.width() ) );
            s.setHeight( qMax( it.boundingRect().size().height(), s.height() ) );
        }
    } else {
        it.setText( text() );
        s.setWidth( qMax( it.boundingRect().size().width(), s.width() ) );
        s.setHeight( qMax( it.boundingRect().size().height(), s.height() ) );
    }
    // @todo: respect old position and transform origin
    s+=QSizeF( penWidth(), penWidth() );
    resize( s );
    updateContent();
}

void PHIGraphicTextItem::updateContent()
{
    QGraphicsSimpleTextItem it;
    QStyleOptionGraphicsItem opt;
    opt.state=QStyle::State_None;
    opt.exposedRect=QRectF();
    QFont f=font();
    f.setPointSizeF( PHI::adjustedFontSize( f.pointSizeF() ) );
    it.setFont( f );
    QPen pen;
    Qt::PenStyle style=static_cast<Qt::PenStyle>(line());
    Qt::BrushStyle pat=static_cast<Qt::BrushStyle>(pattern());
    if ( style!=Qt::NoPen ) {
        pen.setColor( outlineColor() );
        pen.setWidthF( penWidth() );
        pen.setJoinStyle( Qt::MiterJoin );
        pen.setCapStyle( Qt::FlatCap );
        pen.setStyle( style );
    } else pen.setStyle( Qt::NoPen );
    QBrush brush;
    if ( pattern()<15 ) { // no gradient style
        brush.setStyle( pat );
        brush.setColor( color() );
    } else brush=QBrush( gradient() );
    it.setPen( pen );
    it.setBrush( brush );
    if ( isIdeItem() ) it.setText( text( page()->currentLang() ) );
    else it.setText( text() );
    QSizeF s=realSize();
    QImage img( s.toSize(), QImage::Format_ARGB32_Premultiplied );
    QSizeF penSize=QSizeF( penWidth(), penWidth() );
    s-=it.boundingRect().size()+penSize;
    img.fill( 0 );
    QPainter p( &img );
    p.translate( penSize.width()/2., penSize.height()/2. );
    p.translate( 0, s.height()/2. );
    if ( alignment() & Qt::AlignRight ) p.translate( s.width(), 0 );
    else if ( alignment() & Qt::AlignHCenter ) p.translate( s.width()/2., 0 );
    p.setCompositionMode( QPainter::CompositionMode_Source );
    p.setRenderHint( QPainter::TextAntialiasing, true );
    p.setRenderHint( QPainter::Antialiasing, true );
    it.paint( &p, &opt, 0 );
    p.end();
    _image=img;
    update();
}

QRectF PHIGraphicTextItem::boundingRect() const
{
    return rect();
}

void PHIGraphicTextItem::drawShape( QPainter *p, const QRectF &r )
{
    Q_UNUSED( r )
    p->drawImage( 0, 0, _image );
}

void PHIGraphicTextItem::setText( const QString &t, const QByteArray &lang )
{
    if ( _textData.translated() ) {
        _textData.setText( t, lang );
    } else if ( _textData.unparsedStatic() ) {
        _textData.setSource( PHIData::Translated );
        _textData.setText( t, lang );
    }
    updateGeometry();
    if ( isChild() ) {
        PHIBaseItem *it=page()->findItem( parentId() );
        PHIAbstractLayoutItem *lit=qobject_cast<PHIAbstractLayoutItem*>(it);
        if ( lit ) lit->invalidateLayout();
    }
}

QString PHIGraphicTextItem::text( const QByteArray &lang ) const
{
    if ( _textData.translated() ) return _textData.text( lang );
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
    if ( alignment()==static_cast<quint16>(Qt::AlignLeft|Qt::AlignVCenter) ) removeData( DAlignment );
}

QSizeF PHIGraphicTextItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    Q_UNUSED( which )
    Q_UNUSED( constraint )
    return rect().size();
}

PHIConfigWidget* PHIGraphicTextItem::configWidget()
{
    return new PHIColorConfig( this );
}
