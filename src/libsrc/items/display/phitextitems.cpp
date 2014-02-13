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
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QTextBrowser>
#include "phicolorconfig.h"
#include "phitextitems.h"
#include "phidatasources.h"
#include "phibasepage.h"
#include "phirequest.h"
#include "phidataparser.h"
#include "phi.h"

void PHILabelItem::initWidget()
{
    QLabel *l=new QLabel();
    l->setIndent( 1 );
    setWidget( l );
    setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum ) );
    QPalette pal=l->palette();
    pal.setColor( QPalette::Window, Qt::transparent );
    l->setPalette( pal );
    if ( !isClientItem() ) return;
    l->setTextInteractionFlags( Qt::TextBrowserInteraction );
    connect( l, &QLabel::linkActivated, this, &PHILabelItem::linkRequested );
    connect( l, &QLabel::linkHovered, this, &PHILabelItem::linkHovered );
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
    l->setTextFormat( isHtmlText() ? Qt::RichText : Qt::PlainText );
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
    if ( Q_UNLIKELY( colorRole( PHIPalette::WidgetText )==PHIPalette::Custom ) )
        script+=BL( ".color('" )+cssColor( realColor() )+BL( "')" );
    if ( Q_UNLIKELY( colorRole( PHIPalette::WidgetBase )==PHIPalette::Custom ) )
        script+=BL( ".bgColor('" )+cssColor( realBackgroundColor() )+BL( "')" );
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
    out+=BL( "\">" )+data( DText ).toByteArray()+BL( "</td></tr></table></div>\n" );
}

void PHILabelItem::cssStatic( const PHIRequest *req, QByteArray &out ) const
{
    Q_UNUSED( req )
    out+='#'+id()+BL( "{overflow:hidden;white-space:nowrap;}\n" );
    // fallback:
    //if ( colorRole( PHIPalette::WidgetText )!=PHIPalette::WindowText ) out+=BL( "color:" )+realColor().name().toLatin1()+';';
    //if ( colorRole( PHIPalette::WidgetBase )!=PHIPalette::Window ) out+=BL( "background-color:" )+realBackgroundColor().name().toLatin1()+';';
    //out+=BL( "}\n" );
}

void PHILabelItem::cssGraphicEffect( const PHIRequest *req, QByteArray &out, QByteArray &script ) const
{
    if ( effect()->graphicsType()==PHIEffect::GTShadow && !(req->agentFeatures() & PHIRequest::IE678) ) {
        QColor c;
        qreal radius, xoff, yoff;
        effect()->shadow( c, xoff, yoff, radius );
        QByteArray col=cssColor( c );
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
        if ( cr==PHIPalette::Custom ) setData( DColor, col );
        setColorRole( ir, cr );
    } else if ( ir==PHIPalette::WidgetBase ) {
        if ( cr==PHIPalette::Custom ) setData( DBackgroundColor, col );
        setColorRole( ir, cr );
    } else return;
    QWidget *w=widget();
    if ( !w ) return;
    QPalette::ColorRole role=QPalette::WindowText;
    if ( ir==PHIPalette::WidgetBase ) role=QPalette::Window;
    QPalette pal=w->palette();
    pal.setColor( role, col );
    w->setPalette( pal );
}

QScriptValue PHILabelItem::text( const QScriptValue &t )
{
    if ( !t.isValid() ) {
        QString s=realText();
        return s.replace( QRegExp( L1( "<[^>]*>" ) ), QString() );
    }
    setText( t.toString() );
    setHtmlText( false );
    return self();
}

QScriptValue PHILabelItem::html( const QScriptValue &t )
{
    if ( !t.isValid() ) return realText();
    setHtmlText( true );
    setText( t.toString() );
    return self();
}

void PHILabelItem::slotAnchorClicked( const QUrl &url )
{
    emit linkRequested( url.toString() );
}

void PHILabelItem::slotAnchorHover( const QUrl &url )
{
    emit linkHovered( url.toString() );
}

void PHILinkItem::ideInit()
{
    textData()->setText( L1( "Link item" ) );
    setColor( PHIPalette::WidgetText, PHIPalette::Link, page()->phiPalette().color( PHIPalette::Link ) );
    setColor( PHIPalette::WidgetBase, PHIPalette::Window, page()->phiPalette().color( PHIPalette::Window ) );
    setColor( PHIPalette::Hover, PHIPalette::HighlightText, page()->phiPalette().color( PHIPalette::HighlightText ) );
    setColor( PHIPalette::HoverBackground, PHIPalette::Window, page()->phiPalette().color( PHIPalette::Window ) );
}

PHIPalette::ColorRole PHILinkItem::colorRole( PHIPalette::ItemRole role ) const
{
    if ( role==PHIPalette::WidgetText || role==PHIPalette::WidgetBase ) return PHILabelItem::colorRole( role );
    if ( role==PHIPalette::Hover ) return _hoverColorRole;
    if ( role==PHIPalette::HoverBackground ) return _hoverBackgroundColorRole;
    return PHIPalette::NoRole;
}

QColor PHILinkItem::colorForRole(PHIPalette::ItemRole role) const
{
    if ( role==PHIPalette::WidgetText || role==PHIPalette::WidgetBase ) return PHILabelItem::colorForRole( role );
    if ( role==PHIPalette::Hover ) return realHoverColor();
    if ( role==PHIPalette::HoverBackground ) return realHoverBgColor();
    return QColor();
}

QColor PHILinkItem::realHoverColor() const
{
    if ( _hoverColorRole==PHIPalette::Custom ) return data( DHoverColor, QColor( Qt::black ) ).value<QColor>();
    if ( page() ) return page()->phiPalette().color( _hoverColorRole );
    return Qt::black;
}

QColor PHILinkItem::realHoverBgColor() const
{
    if ( _hoverBackgroundColorRole==PHIPalette::Custom ) return data( DHoverBgColor, QColor( Qt::transparent ) ).value<QColor>();
    if ( page() ) return page()->phiPalette().color( _hoverBackgroundColorRole );
    return Qt::transparent;
}

void PHILinkItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    PHILabelItem::html( req, out, script, indent );
    script+=BL( "$$link('" )+id()+BL( "','" )+data( DUrl ).toByteArray().replace( '\'', BL( "\\'" ) );
    script+=BL( "','" )+cssColor( realColor() )+BL( "','" );
    if ( realBackgroundColor()!=QColor( Qt::transparent ) ) script+=cssColor( realBackgroundColor() );
    script+=BL( "','" )+cssColor( realHoverColor() )+BL( "','" );
    if ( realHoverBgColor()!=QColor( Qt::transparent ) ) script+=cssColor( realHoverBgColor() );
    script+=BL( "');\n" );
}

QScriptValue PHILinkItem::url( const QScriptValue &v )
{
    if ( !isServerItem() ) return QScriptValue( QScriptValue::UndefinedValue );
    if ( !v.isValid() ) return realUrl();
    setUrl( v.toString() );
    return self();
}

void PHILinkItem::saveItemData( QDataStream &out, int version )
{
    PHILabelItem::saveItemData( out, version );
    out << static_cast<quint8>(_hoverColorRole) << static_cast<quint8>(_hoverBackgroundColorRole);
}

void PHILinkItem::loadItemData( QDataStream &in, int version )
{
    PHILabelItem::loadItemData( in, version );
    quint8 hcr, hbr;
    in >> hcr >> hbr;
    _hoverColorRole=static_cast<PHIPalette::ColorRole>(hcr);
    _hoverBackgroundColorRole=static_cast<PHIPalette::ColorRole>(hbr);
}

void PHILinkItem::setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col )
{
    if ( ir==PHIPalette::Hover ) {
        _hoverColorRole=cr;
        setData( DHoverColor, col );
    } else if ( ir==PHIPalette::HoverBackground ) {
        _hoverBackgroundColorRole=cr;
        setData( DHoverBgColor, col );
    }
    PHILabelItem::setColor( ir, cr, col );
}

void PHILinkItem::mouseover( const QGraphicsSceneHoverEvent *e )
{
    Q_UNUSED( e )
    QWidget *w=widget();
    if ( !w ) return;
    QColor txtCol=page()->phiPalette().color( _hoverColorRole );
    if ( _hoverColorRole==PHIPalette::Custom ) txtCol=realHoverColor();
    QColor bgCol=page()->phiPalette().color( _hoverBackgroundColorRole );
    if ( _hoverBackgroundColorRole==PHIPalette::Custom ) bgCol=realHoverBgColor();
    if ( bgCol==QColor( Qt::transparent ) ) bgCol=realBackgroundColor();
    QPalette pal=w->palette();
    pal.setColor( QPalette::WindowText, txtCol );
    pal.setColor( QPalette::Window, bgCol );
    w->setPalette( pal );
}

void PHILinkItem::mouseout( const QGraphicsSceneHoverEvent *e )
{
    Q_UNUSED( e )
    QWidget *w=widget();
    if ( !w ) return;
    QColor txtCol=page()->phiPalette().color( colorRole( PHIPalette::WidgetText ) );
    if ( colorRole( PHIPalette::WidgetText )==PHIPalette::Custom ) txtCol=realColor();
    QColor bgCol=page()->phiPalette().color( colorRole( PHIPalette::WidgetBase ) );
    if ( colorRole( PHIPalette::WidgetBase )==PHIPalette::Custom ) bgCol=realBackgroundColor();
    QPalette pal=w->palette();
    pal.setColor( QPalette::WindowText, txtCol );
    pal.setColor( QPalette::Window, bgCol );
    w->setPalette( pal );
}

void PHILinkItem::click( const QGraphicsSceneMouseEvent *e )
{
    Q_UNUSED( e )
    if ( !realUrl().isEmpty() ) emit linkRequested( realUrl() );
}

void PHILinkItem::clientPrepareData()
{
    PHILabelItem::clientPrepareData();
    setData( DTmpHoverRole, static_cast<quint8>(_hoverColorRole) );
    setData( DTmpHoverBgRole, static_cast<quint8>(_hoverBackgroundColorRole) );
}

void PHILinkItem::clientInitData()
{
    PHILabelItem::clientInitData();
    _hoverColorRole=static_cast<PHIPalette::ColorRole>(data( DTmpHoverRole ).value<quint8>());
    _hoverBackgroundColorRole=static_cast<PHIPalette::ColorRole>(data( DTmpHoverBgRole ).value<quint8>());
    //setCursor( Qt::PointingHandCursor );
}

PHIConfigWidget *PHILinkItem::ideConfigWidget()
{
    return new PHIHoverColorConfig( this );
}

void PHIGraphicRichTextItem::ideInit()
{
    textData()->setText( tr( "<b>Rich</b> <i>text<i>" ) );
    setColor( PHIPalette::WidgetBase, PHIPalette::Window, page()->phiPalette().color( PHIPalette::Window ) );
    setColor( PHIPalette::WidgetText, PHIPalette::WindowText, page()->phiPalette().color( PHIPalette::WindowText ) );
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

void PHIGraphicRichTextItem::setWidgetAligment( Qt::Alignment align )
{
    Q_UNUSED( align )
    setData( DImage, graphicImage( realText() ) );
    update();
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
    if ( Q_UNLIKELY( dirtyFlags() & DFCustom1 ) )
        setImagePath( PHIDataParser::createImage( req, this, graphicImage( realText() ), PHIData::c(), -1 ) );
    PHIBaseItem::htmlImg( req, out, script, indent );
}

QImage PHIGraphicRichTextItem::graphicImage( const QString &text ) const
{
    QFont f=font();
    f.setPointSizeF( PHI::adjustedFontSize( f.pointSizeF() ) );
    QTextDocument doc;
    QTextOption opt=doc.defaultTextOption();
    opt.setAlignment( static_cast<Qt::Alignment>(realAlignment()) );
    doc.setDefaultTextOption( opt );
    doc.setDefaultFont( f );
    doc.setTextWidth( realWidth() );
    doc.setDefaultStyleSheet( styleSheet() );
    doc.setHtml( text );
    QAbstractTextDocumentLayout::PaintContext ctx;
    ctx.palette.setColor( QPalette::Text, realColor() );
    ctx.clip=rect();
    //QTextCharFormat format;
    //format.setForeground( realColor() );
    //QTextCursor cursor( &doc );
    //cursor.select( QTextCursor::Document );
    //cursor.mergeCharFormat( format );
    QImage img( qRound( realWidth() ), qRound( realHeight() ), QImage::Format_ARGB32_Premultiplied );
    img.fill( realBackgroundColor() );
    QPainter p( &img );
    doc.documentLayout()->draw( &p, ctx );
    p.end();
    return img;
}

QSizeF PHIGraphicRichTextItem::graphicSize( const QString &text ) const
{
    QFont f=font();
    f.setPointSizeF( PHI::adjustedFontSize( f.pointSizeF() ) );
    QTextDocument doc;
    QTextOption opt=doc.defaultTextOption();
    opt.setAlignment( static_cast<Qt::Alignment>(realAlignment()) );
    doc.setDefaultTextOption( opt );
    doc.setDefaultFont( f );
    doc.setDefaultStyleSheet( styleSheet() );
    doc.setTextWidth( realWidth() );
    doc.setHtml( text );
    return QSizeF( realWidth(), doc.size().height() );
}

QSizeF PHIGraphicRichTextItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    if ( !isIdeItem() ) return rect().size();
    if ( textData()->isUnparsedStatic() || textData()->isUnparsedTranslated() ) {
        if ( which==Qt::MinimumSize ) return QSizeF( 16., realHeight() );
        if ( which==Qt::PreferredSize ) return QSizeF( 200., realHeight() );
        if ( which==Qt::MaximumSize ) return QSizeF( 10000., realHeight() );
    }
    return PHIAbstractTextItem::sizeHint( which, constraint );
}

bool PHIGraphicRichTextItem::paint( QPainter *painter, const QRectF &exposed )
{
    Q_UNUSED( exposed );
    painter->drawImage( 0, 0, image() );
    return true;
}

QScriptValue PHIGraphicRichTextItem::html( const QScriptValue &t )
{
    if ( !isServerItem() ) return scriptEngine()->undefinedValue();
    if ( !t.isValid() ) return realText();
    setDirtyFlag( DFCustom1 );
    setText( t.toString() );
    return self();
}

QScriptValue PHIGraphicRichTextItem::color( const QScriptValue &c )
{
    if ( !isServerItem() ) return scriptEngine()->undefinedValue();
    if ( !c.isValid() ) return PHIAbstractTextItem::color( c );
    setDirtyFlag( DFCustom1 );
    return PHIAbstractTextItem::color( c );
}

QScriptValue PHIGraphicRichTextItem::bgColor( const QScriptValue &c )
{
    if ( !isServerItem() ) return scriptEngine()->undefinedValue();
    if ( !c.isValid() ) return PHIAbstractTextItem::bgColor( c );
    setDirtyFlag( DFCustom1 );
    return PHIAbstractTextItem::bgColor( c );
}

void PHIRichTextItem::ideInit()
{
    textData()->setText( L1( "<b>Rich</b> <i>text</i>" ) );
    setColor( PHIPalette::WidgetBase, PHIPalette::Window, page()->phiPalette().color( PHIPalette::Window ) );
    setColor( PHIPalette::WidgetText, PHIPalette::WindowText, page()->phiPalette().color( PHIPalette::WindowText ) );
}

void PHIRichTextItem::initWidget()
{
    QTextBrowser *tb=new QTextBrowser();
    tb->setReadOnly( true );
    tb->setUndoRedoEnabled( false );
    tb->setOpenLinks( false );
    tb->setOpenExternalLinks( false );
    QPalette pal=tb->palette();
    pal.setColor( QPalette::Window, Qt::transparent );
    tb->setPalette( pal );
    setWidget( tb );
    setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    if ( !isClientItem() ) return;
    connect( tb, &QTextBrowser::anchorClicked, this, &PHIRichTextItem::slotAnchorClicked );
    connect( tb, SIGNAL(highlighted( QUrl )), this, SLOT(slotAnchorHover( QUrl )) );
}

void PHIRichTextItem::cssStatic( const PHIRequest *req, QByteArray &out ) const
{
    Q_UNUSED( req )
    out+='#'+id()+BL( "{padding:4px;overflow:auto;" );
    Qt::Alignment align=static_cast<Qt::Alignment>(realAlignment());
    if ( align & Qt::AlignHCenter ) out+=BL( "text-align:center" );
    else if ( align & Qt::AlignRight ) out+=BL( "text-align:right" );
    else if ( align & Qt::AlignJustify ) out+=BL( "text-align:justify" );
    out+=BL( "}\n" );
}

void PHIRichTextItem::setWidgetText( const QString &t )
{
    QTextBrowser *tb=qobject_cast<QTextBrowser*>(widget());
    if ( !tb ) return;
    QString css=L1( "a:link {text-decoration:" )+((page()->flags() & PHIBasePage::FNoUnderlinedLinks) ? L1( "none" )
        : L1( "underline" ))+L1( ";color:" )+QString::fromLatin1( cssRgba(
        page()->phiPalette().color( PHIPalette::Link) ) )+L1( "}" );
    tb->document()->setDefaultStyleSheet( css );
    tb->setHtml( t );
}

void PHIRichTextItem::setWidgetAligment( Qt::Alignment align )
{
    QTextBrowser *tb=qobject_cast<QTextBrowser*>(widget());
    if ( !tb ) return;
    QTextDocument *doc=tb->document();
    QTextOption opt=doc->defaultTextOption();
    opt.setAlignment( align );
    doc->setDefaultTextOption( opt );
}

QSizeF PHIRichTextItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
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
    setAdjustedRect( rect().adjusted( 0, 0, -8, -8 ));
    htmlInitItem( script, false );
    if ( colorRole( PHIPalette::WidgetText )!=PHIPalette::Text )
        script+=BL( ".color('" )+cssColor( realColor() )+BL( "')" );
    if ( colorRole( PHIPalette::WidgetBase )!=PHIPalette::Window )
        script+=BL( ".bgColor('" )+cssColor( realBackgroundColor() )+BL( "')" );
    script+=BL( ";\n" );
    out+=indent+BL( "<div" );
    htmlBase( req, out, script );
    out+=BL( "\">\n" );
    out+=data( DText ).toByteArray();
    out+='\n'+indent+BL( "</div>\n" );
}

void PHIRichTextItem::cssGraphicEffect( const PHIRequest *req, QByteArray &out, QByteArray &script ) const
{
    if ( effect()->graphicsType()==PHIEffect::GTShadow && !(req->agentFeatures() & PHIRequest::IE678) ) {
        QColor c;
        qreal radius, xoff, yoff;
        effect()->shadow( c, xoff, yoff, radius );
        QByteArray col=cssColor( c );
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

void PHIRichTextItem::slotAnchorClicked( const QUrl &url )
{
    emit linkRequested( url.toString() );
}

void PHIRichTextItem::slotAnchorHover( const QUrl &url )
{
    emit linkHovered( url.toString() );
}
