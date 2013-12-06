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
#include <QPainter>
#include "phimiscitems.h"
#include "phirequest.h"

void PHIHiddenItem::ideInit()
{
    textData()->setText( QString() );
}

QRectF PHIHiddenItem::boundingRect() const
{
    return rect();
}

QSizeF PHIHiddenItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    Q_UNUSED( which )
    Q_UNUSED( constraint )
    return QSizeF( 150., 16. );
}

void PHIHiddenItem::paint( QPainter *painter, const QRectF &exposed )
{
    Q_UNUSED( exposed )
    if ( !isIdeItem() ) return;
    painter->setRenderHint( QPainter::TextAntialiasing );
    painter->setPen( QPen( Qt::darkBlue, 1., Qt::SolidLine ) );
    painter->setOpacity( .5 );
    painter->fillRect( rect(), QBrush( QColor( Qt::blue ).lighter( 180 ) ) );
    painter->drawRect( rect() );
    QFont f=page()->font();
    f.setPointSizeF( PHI::adjustedFontSize( 10 ) );
    painter->setFont( f );
    QString t=name()+QLatin1Char( '=' )+realText();
    painter->drawText( rect(), t, QTextOption( Qt::AlignCenter ) );
}

void PHIHiddenItem::squeeze()
{
    setOpacity( 1. );
    removeData( DFont );
    setXRotation( 0 );
    setYRotation( 0 );
    setZRotation( 0 );
    setHSkew( 0 );
    setVSkew( 0 );
    PHIAbstractInputItem::squeeze();
}

void PHIHiddenItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    Q_UNUSED( req )
    Q_UNUSED( script )
    out+=indent+BL( "<input type=\"hidden\" id=\"" )+id()+BL( "\" value=\"" )
        +data( DText ).toByteArray()+BL( "\" name=\"" )+id()+BL( "\">\n" );
    htmlInitItem( script );
}

void PHIRolloverItem::ideInit()
{
    setLine( 0 ); // no border
    setPattern( 1 ); // solid pattern
    setColor( PHIPalette::Foreground, PHIPalette::Black, page()->phiPalette().color( PHIPalette::Black ) );
    setColor( PHIPalette::WidgetBase, PHIPalette::Window, page()->phiPalette().color( PHIPalette::Window ) );
    setColor( PHIPalette::Hover, PHIPalette::HighlightText, page()->phiPalette().color( PHIPalette::HighlightText ) );
    setColor( PHIPalette::HoverBackground, PHIPalette::Window, page()->phiPalette().color( PHIPalette::Window ) );
    // used for border:
    setColor( PHIPalette::Background, PHIPalette::Black, page()->phiPalette().color( PHIPalette::Black ) );
    _textData.setText( L1( "Rollover" ) );
}

// called form IDE only:
void PHIRolloverItem::ideSetText( const QString &t, const QByteArray &lang )
{
    if ( _textData.isTranslated() ) {
        _textData.setText( t, lang );
        setText( t );
    } else if ( _textData.isStatic() ) {
        _textData.setSource( PHIData::Translated );
        _textData.setText( t, lang );
        setText( t );
    } else setText( _textData.text() );
    if ( isChild() ) {
        PHIBaseItem *it=page()->findItem( parentId() );
        PHIAbstractLayoutItem *lit=qobject_cast<PHIAbstractLayoutItem*>(it);
        if ( lit ) lit->invalidateLayout();
    }
}

void PHIRolloverItem::ideUpdateData()
{
    if ( _textData.isTranslated() ) {
        setText( _textData.text( page()->currentLang() ) );
    } else {
        setText( _textData.text() );
    }
    if ( _imageBookData.isTranslated() ) {
        setImages( _imageBookData.imageBook( page()->currentLang() ) );
    } else {
        setImages( _imageBookData.imageBook() );
    }
    if ( isChild() ) {
        PHIBaseItem *it=page()->findItem( parentId() );
        PHIAbstractLayoutItem *lit=qobject_cast<PHIAbstractLayoutItem*>(it);
        if ( lit ) lit->invalidateLayout();
    }
}

QString PHIRolloverItem::ideText( const QByteArray &lang ) const
{
    if ( _textData.isTranslated() ) return _textData.text( lang );
    return _textData.text();
}

QColor PHIRolloverItem::color( PHIPalette::ItemRole role ) const
{
    if ( role==PHIPalette::Foreground ) return realColor();
    if ( role==PHIPalette::Background ) return realOutlineColor();
    if ( role==PHIPalette::WidgetBase ) return realBgColor();
    if ( role==PHIPalette::Hover ) return realHoverColor();
    if ( role==PHIPalette::HoverBackground ) return realHoverBgColor();
    return QColor();
}

PHIPalette::ColorRole PHIRolloverItem::colorRole( PHIPalette::ItemRole role ) const
{
    if ( role==PHIPalette::Foreground ) return PHIAbstractShapeItem::colorRole( role );
    if ( role==PHIPalette::Background ) return PHIAbstractShapeItem::colorRole( role );
    if ( role==PHIPalette::WidgetBase ) return _bgColorRole;
    if ( role==PHIPalette::Hover ) return _hoverColorRole;
    if ( role==PHIPalette::HoverBackground ) return _hoverBgColorRole;
    return PHIPalette::NoRole;
}

void PHIRolloverItem::setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col )
{
    if ( ir==PHIPalette::Foreground ) PHIAbstractShapeItem::setColor( ir, cr, col );
    if ( ir==PHIPalette::Background ) PHIAbstractShapeItem::setColor( ir, cr, col );
    if ( ir==PHIPalette::WidgetBase ) {
        _bgColorRole=cr;
        setData( DBgColor, col );
    }
    if ( ir==PHIPalette::Hover ) {
        _hoverColorRole=cr;
        setData( DHoverColor, col );
    }
    if ( ir==PHIPalette::HoverBackground ) {
        _hoverBgColorRole=cr;
        setData( DHoverBgColor, col );
    }
    update();
}

QSizeF PHIRolloverItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    if ( which==Qt::MinimumSize ) return QSizeF( 20., 20. );
    if ( which==Qt::PreferredSize ) {
        QFont f=font();
        f.setPointSizeF( PHI::adjustedFontSize( font().pointSizeF() ) );
        QFontMetricsF m( f );
        qreal minWidth=64.;
        if ( page() ) {
            foreach ( QString l, page()->languages() ) {
                l=ideText( l.toLatin1() );
                if ( m.width( l )>minWidth ) minWidth=qRound(m.width( l ));
            }
        }
        return QSizeF( minWidth, qMax( 22., qRound(m.height())+8. ) );
    }
    return PHIBaseItem::sizeHint( which, constraint );
}

void PHIRolloverItem::drawShape( QPainter *p, const QRectF &exposed )
{
    Q_UNUSED( exposed )
    p->setRenderHint( QPainter::TextAntialiasing );
    QFont f=font();
    f.setPointSizeF( PHI::adjustedFontSize( f.pointSizeF() ) );
    p->setFont( f );
    QBrush brush=p->brush();
    QImage img;
    if ( _hover ) {
        img=realImages().value( "1" );
        if ( realPattern()!=15 ) {
            brush.setColor( realHoverBgColor() );
            p->setBrush( brush );
        }
    } else {
        img=realImages().value( "0" );
        if ( realPattern()!=15 ) {
            brush.setColor( realBgColor() );
            p->setBrush( brush );
        }
    }
    p->drawRect( rect() ); // draw background (incl. border if any)
    if ( _hover ) p->setPen( QPen( realHoverColor() ) );
    else p->setPen( QPen( realColor() ) );
    if ( !img.isNull() ) p->drawImage( rect(), img );
    p->drawText( rect(), realText(), QTextOption( Qt::AlignCenter ) );
}

void PHIRolloverItem::squeeze()
{
    PHIAbstractShapeItem::squeeze();
    removeData( DText );
    removeData( DImages );
    if ( realHoverBgColor()==QColor( Qt::transparent ) ) removeData( DHoverBgColor );
    if ( realHoverColor()==QColor( Qt::black ) ) removeData( DHoverColor );
    if ( realBgColor()==QColor( Qt::transparent ) ) removeData( DBgColor );
}

void PHIRolloverItem::loadItemData( QDataStream &in, int version )
{
    PHIAbstractShapeItem::loadItemData( in, version );
    quint8 bgColor, hoverColor, hoverBgColor;
    in >> &_textData >> bgColor >> hoverColor >> hoverBgColor;
    _bgColorRole=static_cast<PHIPalette::ColorRole>(bgColor);
    _hoverColorRole=static_cast<PHIPalette::ColorRole>(hoverColor);
    _hoverBgColorRole=static_cast<PHIPalette::ColorRole>(hoverBgColor);
    QByteArray arr;
    in >> arr;
    arr=qUncompress( arr );
    QDataStream ds( &arr, QIODevice::ReadOnly );
    ds.setVersion( QDataStream::Qt_5_1 );
    ds >> &_imageBookData;
}

void PHIRolloverItem::saveItemData( QDataStream &out, int version )
{
    PHIAbstractShapeItem::saveItemData( out, version );
    out << &_textData << static_cast<quint8>(_bgColorRole)
        << static_cast<quint8>(_hoverColorRole) << static_cast<quint8>(_hoverBgColorRole);
    QByteArray arr;
    QDataStream ds( &arr, QIODevice::WriteOnly );
    ds.setVersion( QDataStream::Qt_5_1 );
    ds << &_imageBookData;
    out << qCompress( arr, 9 );
}

void PHIRolloverItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{

}
