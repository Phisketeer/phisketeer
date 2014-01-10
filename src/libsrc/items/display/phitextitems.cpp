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
#include <QLabel>
#include <QPainter>
#include <QGraphicsTextItem>
#include <QStyleOptionGraphicsItem>
#include <QTextBrowser>
#include "phitextitems.h"
#include "phidatasources.h"
#include "phibasepage.h"
#include "phirequest.h"
#include "phidataparser.h"
#include "phi.h"

void PHILabelItem::initWidget()
{
    setWidget( new QLabel() );
    setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed ) );
}

void PHILabelItem::ideInit()
{
    textData()->setText( L1( "Label" ) );
    setColor( PHIPalette::WidgetBase, PHIPalette::Window, page()->phiPalette().color( PHIPalette::Window ) );
    setColor( PHIPalette::WidgetText, PHIPalette::WindowText, page()->phiPalette().color( PHIPalette::WindowText ) );
}

void PHILabelItem::setWidgetText( const QString &t )
{
    QLabel *l=qobject_cast<QLabel*>(widget());
    if ( !l ) return;
    l->setTextFormat( isPlainText() ? Qt::PlainText : Qt::RichText );
    l->setText( t );
}

void PHILabelItem::setWidgetAligment( Qt::Alignment align )
{
    QLabel *l=qobject_cast<QLabel*>(widget());
    Q_ASSERT( l );
    l->setAlignment( align );
}

void PHILabelItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    htmlInitItem( script, false );
    if ( isPlainText() ) script+=BL( ".text('" )+data( DText ).toByteArray()+BL( "')" );
    else script+=BL( ".html('" )+data( DText ).toByteArray()+BL( "')" );
    if ( colorRole( PHIPalette::WidgetText )!=PHIPalette::Text )
        script+=BL( ".color('" )+cssRgba( realColor() )+BL( "')" );
    if ( colorRole( PHIPalette::WidgetBase )!=PHIPalette::Window )
        script+=BL( ".bgColor('" )+cssRgba( realBackgroundColor() )+BL( "')" );
    script+=BL( ";\n" );
    out+=indent+BL( "<div" );
    htmlBase( req, out, script );
    out+=BL( "\"><table class=\"phi\"><tr><td id=\"" )+id()+BL( "_phit\" style=\"" );
    Qt::Alignment a=static_cast<Qt::Alignment>(realAlignment());
    if ( data( DFont ).isValid() ) {
        QFont f=data( DFont ).value<QFont>();
        if ( f.underline() ) {
            out+=BL( "text-decoration:underline;" );
        }
    }
    if ( a & Qt::AlignJustify ) out+=BL( "vertical-align:top;" );
    else out+=BL( "vertical-align:middle;" );
    if ( a & Qt::AlignHCenter ) out+=BL( "text-align:center;" );
    else if ( a & Qt::AlignRight ) out+=BL( "text-align:right;" );
    out+=BL( "\">" )+BL( "</td></tr></table></div>\n" );
}

void PHILabelItem::cssStatic( const PHIRequest *req, QByteArray &out ) const
{
    Q_UNUSED( req )
    out+='#'+id()+BL( "{overflow:hidden;white-space:nowrap;" );
    // fallback:
    if ( colorRole( PHIPalette::WidgetText )!=PHIPalette::WindowText ) out+=BL( "color:" )+realColor().name().toLatin1()+';';
    if ( colorRole( PHIPalette::WidgetBase )!=PHIPalette::Window ) out+=BL( "background-color:" )+realBackgroundColor().name().toLatin1()+';';
    out+=BL( "}\n" );
}

void PHILabelItem::cssGraphicEffect( const PHIRequest *req, QByteArray &out, QByteArray &script ) const
{
    if ( effect()->graphicsType()==PHIEffect::GTShadow && !(req->agentFeatures() & PHIRequest::IE678) ) {
        QColor c;
        qreal radius, xoff, yoff;
        effect()->shadow( c, xoff, yoff, radius );
        QByteArray col=cssRgba( c );
        if ( !(req->agentFeatures() & PHIRequest::RGBA) ) col=c.name().toLatin1();
        if ( colorRole( PHIPalette::WidgetBase )==PHIPalette::Window ) {
            out+=BL( "text-shadow:" )+QByteArray::number( qRound(xoff) )
                +BL( "px " )+QByteArray::number( qRound(yoff) )+BL( "px " )
                +QByteArray::number( qRound(radius) )+BL( "px " )+col+';';
        } else {
            QByteArray prefix=req->agentPrefix();
            if ( req->agentEngine()==PHIRequest::Gecko && req->engineMajorVersion()>1 ) prefix=QByteArray();
            else if ( req->agentEngine()==PHIRequest::WebKit && req->engineMajorVersion()>534 ) prefix=QByteArray();
            else if ( req->agentEngine()==PHIRequest::Presto ) prefix=QByteArray();
            out+=prefix+BL( "box-shadow:" )+QByteArray::number( qRound(xoff) )+"px ";
            out+=QByteArray::number( qRound(yoff) )+"px ";
            out+=QByteArray::number( qRound(radius) )+"px "+col+';';
        }
    } else PHIAbstractTextItem::cssGraphicEffect( req, out, script );
}

void PHILabelItem::setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col )
{
    if ( ir==PHIPalette::WidgetText ) {
        setData( DColor, col );
        setColorRole( cr );
    } else if ( ir==PHIPalette::WidgetBase ) {
        setData( DBackgroundColor, col );
        setBackgroundColorRole( cr );
    } else return;
    QWidget *w=widget();
    if ( !w ) return;
    QPalette::ColorRole role=QPalette::WindowText;
    if ( ir==PHIPalette::WidgetBase ) role=QPalette::Window;
    QPalette pal=w->palette();
    pal.setColor( role, col );
    w->setPalette( pal );
}

QScriptValue PHILabelItem::color(const QScriptValue &c )
{
    if ( !c.isValid() ) return PHI::colorToString( realColor() );
    setColor( PHIPalette::WidgetText, PHIPalette::Custom, PHI::colorFromString( c.toString() ) );
    return self();
}

QScriptValue PHILabelItem::bgColor( const QScriptValue &c )
{
    if ( !c.isValid() ) return PHI::colorToString( realBackgroundColor() );
    setColor( PHIPalette::WidgetBase, PHIPalette::Custom, PHI::colorFromString( c.toString() ) );
    return self();
}

QScriptValue PHILabelItem::text( const QScriptValue &t )
{
    if ( !t.isValid() ) return realText();
    setText( t.toString() );
    setPlainText( true );
    return self();
}

QScriptValue PHILabelItem::html( const QScriptValue &t )
{
    if ( !t.isValid() ) return realText();
    setPlainText( false );
    setText( t.toString() );
    return self();
}

void PHIGraphicRichTextItem::ideInit()
{
    textData()->setText( tr( "<b>Rich</b> <i>text<i>" ) );
    setColor( PHIPalette::WidgetBase, PHIPalette::Window, page()->phiPalette().color( PHIPalette::Window ) );
}

void PHIGraphicRichTextItem::initWidget()
{
    connect( this, &PHIBaseItem::sizeChanged, this, &PHIGraphicRichTextItem::slotSizeChanged );
}

void PHIGraphicRichTextItem::ideUpdateData()
{
    QString t;
    if ( textData()->isUnparsedTranslated() ) t=textData()->text( page()->currentLang() );
    else t=textData()->text();
    QSizeF s( 16., 16. );
    if ( textData()->isUnparsedStatic() ) resize( graphicSize( t ) );
    else if ( textData()->isUnparsedTranslated() ) {
        foreach( QByteArray l, textData()->langs() ) {
            QSizeF ls=graphicSize( textData()->text( l ) );
            if ( ls.width()>s.width() ) s.setWidth( ls.width() );
            if ( ls.height()>s.height() ) s.setHeight( ls.height() );
        }
        resize( s );
    }
    setData( DImage, graphicImage( t ) );
    update();
    PHIAbstractTextItem::ideUpdateData();
}

void PHIGraphicRichTextItem::setWidgetText( const QString &t )
{
    Q_UNUSED( t );
}

void PHIGraphicRichTextItem::slotSizeChanged( const QSizeF &s )
{
    Q_UNUSED( s )
    if ( !isIdeItem() || isChild() ) return;
    blockSignals( true );
    ideUpdateData();
    blockSignals( false );
}

void PHIGraphicRichTextItem::setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col )
{
    PHIAbstractTextItem::setColor( ir, cr, col );
    setData( DImage, graphicImage( realText() ) );
    update();
}

void PHIGraphicRichTextItem::squeeze()
{
    PHIAbstractTextItem::squeeze();
    removeData( DImage );
}

bool PHIGraphicRichTextItem::isHeightChangeable() const
{
    if ( textData()->isUnparsedStatic() || textData()->isUnparsedTranslated() ) return false;
    return true;
}

void PHIGraphicRichTextItem::phisCreateData( const PHIDataParser &parser )
{
    setData( DText, parser.text( textData() ) );
    if ( textData()->isUnparsedStatic() ) {
        setImagePath( parser.createImage( graphicImage( textData()->text() ) ) );
    } else if ( textData()->isUnparsedTranslated() ) {
        foreach ( QByteArray l, textData()->langs() ) {
            QByteArray path=parser.createImage( graphicImage( textData()->text( l ) ), l );
            qDebug() << id() << path << l;
        }
    } else setDirtyFlag( DFText );
}

void PHIGraphicRichTextItem::phisParseData( const PHIDataParser &parser )
{
    if ( Q_UNLIKELY( dirtyFlags() & DFText ) ) {
        setData( DText, parser.text( textData() ) );
        // need to create dynamic uncached image
        setImagePath( parser.createImage( graphicImage( realText() ), PHIData::c(), -1 ) );
    } else {
        if ( textData()->isUnparsedStatic() ) {
            setData( DText, textData()->variant() );
        } else {
            Q_ASSERT( textData()->isUnparsedTranslated() );
            setImagePath( parser.imagePath( page()->currentLang() ) );
        }
    }
}

void PHIGraphicRichTextItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    PHIBaseItem::htmlImg( req, out, script, indent );
}

QImage PHIGraphicRichTextItem::graphicImage( const QString &text ) const
{
    QStyleOptionGraphicsItem opt;
    opt.state=QStyle::State_None;
    opt.exposedRect=QRectF();
    QGraphicsTextItem it;
    QFont f=font();
    f.setPointSizeF( PHI::adjustedFontSize( f.pointSizeF() ) );
    it.setFont( f );
    it.setDefaultTextColor( realColor() );
    it.setTextWidth( realWidth() );
    it.setHtml( text );
    QImage img( qRound( realWidth() ), qRound( realHeight() ), QImage::Format_ARGB32_Premultiplied );
    img.fill( realBackgroundColor() );
    QPainter p( &img );
    it.paint( &p, &opt, 0 );
    p.end();
    return img;
}

QSizeF PHIGraphicRichTextItem::graphicSize( const QString &text ) const
{
    QGraphicsTextItem it;
    QFont f=font();
    f.setPointSizeF( PHI::adjustedFontSize( f.pointSizeF() ) );
    it.setFont( f );
    it.setDefaultTextColor( realColor() );
    it.setTextWidth( realWidth() );
    it.setHtml( text );
    return QSizeF( realWidth(), it.boundingRect().height() );
}

QSizeF PHIGraphicRichTextItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    if ( isChild() || !isIdeItem() ) return rect().size();
    if ( textData()->isUnparsedStatic() || textData()->isUnparsedTranslated() ) {
        if ( which==Qt::MinimumSize ) return QSizeF( 16., realHeight() );
        if ( which==Qt::PreferredSize ) return QSizeF( 200., realHeight() );
        if ( which==Qt::MaximumSize ) return QSizeF( 10000., realHeight() );
    }
    return PHIAbstractTextItem::sizeHint( which, constraint );
}

void PHIGraphicRichTextItem::paint( QPainter *painter, const QRectF &exposed )
{
    Q_UNUSED( exposed );
    painter->drawImage( 0, 0, image() );
}

QScriptValue PHIGraphicRichTextItem::html( const QScriptValue &t )
{
    if ( !t.isValid() ) return realText();
    setText( t.toString() );
    return self();
}

void PHIRichTextItem::ideInit()
{
    textData()->setText( L1( "<b>Rich</b> <i>text</i>" ) );
    setColor( PHIPalette::WidgetBase, PHIPalette::Window, page()->phiPalette().color( PHIPalette::Window ) );
    setColor( PHIPalette::WidgetText, PHIPalette::Text, page()->phiPalette().color( PHIPalette::Text ) );
}

void PHIRichTextItem::initWidget()
{
    QTextBrowser *tb=new QTextBrowser();
    tb->setReadOnly( true );
    tb->setUndoRedoEnabled( false );
    tb->setOpenLinks( false );
    tb->setOpenExternalLinks( false );
    setWidget( tb );
    if ( !isClientItem() ) return;
    connect( tb, &QTextBrowser::anchorClicked, this, &PHIRichTextItem::slotAnchorClicked );
    connect( tb, SIGNAL(highlighted( QUrl )), this, SLOT(slotAnchorHover( QUrl )) );
}

void PHIRichTextItem::setWidgetText( const QString &t )
{
    QTextBrowser *tb=qobject_cast<QTextBrowser*>(widget());
    if ( !tb ) return;
    tb->setHtml( t );
}

void PHIRichTextItem::setWidgetAligment( Qt::Alignment align )
{
    QTextBrowser *tb=qobject_cast<QTextBrowser*>(widget());
    if ( !tb ) return;
    tb->setAlignment( align );
}

QSizeF PHIRichTextItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    if ( isChild() ) return realSize();
    if ( which==Qt::MinimumSize ) return QSizeF( 16., 16. );
    if ( which==Qt::PreferredSize ) return QSizeF( 300., 200. );
    return PHIAbstractTextItem::sizeHint( which, constraint );
}

QScriptValue PHIRichTextItem::html( const QScriptValue &v )
{
    if ( !v.isValid() ) return realText();
    setText( v.toString() );
    return self();
}

void PHIRichTextItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    htmlInitItem( script, false );
    if ( colorRole( PHIPalette::WidgetText )!=PHIPalette::Text )
        script+=BL( ".color('" )+cssRgba( realColor() )+BL( "')" );
    if ( colorRole( PHIPalette::WidgetBase )!=PHIPalette::Window )
        script+=BL( ".bgColor('" )+cssRgba( realBackgroundColor() )+BL( "')" );
    script+=BL( ";\n" );
    out+=indent+BL( "<div" );
    htmlBase( req, out, script );
    out+=BL( "overflow:auto\">\n" );
    out+=data( DText ).toByteArray();
    out+='\n'+indent+BL( "</div>\n" );
}

void PHIRichTextItem::slotAnchorClicked( const QUrl &url )
{
    emit linkRequested( url.toString() );
}

void PHIRichTextItem::slotAnchorHover( const QUrl &url )
{
    emit linkHovered( url.toString() );
}
