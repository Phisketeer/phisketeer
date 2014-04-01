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
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QSizePolicy>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QFile>
#include <QHttpMultiPart>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "phibuttonitems.h"
#include "phiinputtools.h"
#include "phidataparser.h"
#include "phiimagerequest.h"

void PHISubmitButtonItem::initWidget()
{
    setWidget( new QPushButton() );
    setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed, QSizePolicy::PushButton ) );
    QPalette pal=widget()->palette();
    pal.setColor( QPalette::Window, Qt::transparent );
    widget()->setPalette( pal );
}

void PHISubmitButtonItem::ideInit()
{
    textData()->setText( tr( "Submit" ) );
    setColor( PHIPalette::WidgetBase, PHIPalette::Button, page()->phiPalette().color( PHIPalette::Button ) );
    setColor( PHIPalette::WidgetText, PHIPalette::ButtonText, page()->phiPalette().color( PHIPalette::ButtonText ) );
}

void PHISubmitButtonItem::setWidgetText( const QString &t )
{
    QPushButton *b=qobject_cast<QPushButton*>(widget());
    if ( !b ) return;
    b->setText( t );
}

bool PHISubmitButtonItem::paint( QPainter *painter, const QRectF &exposed )
{
    Q_UNUSED( exposed )
#ifdef Q_OS_MAC
    painter->translate( 0, -2 );
#else
    Q_UNUSED( painter )
#endif
    return false;
}

QSizeF PHISubmitButtonItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    if ( which==Qt::MinimumSize ) return QSizeF( 34, 21 );
    if ( which==Qt::PreferredSize ) {
        QSizeF s=PHIAbstractTextItem::sizeHint( which, constraint );
        if ( s.height()<23 ) s.setHeight( 23 );
        s.setWidth( s.width()+28 );
        return s;
    }
    return PHIAbstractTextItem::sizeHint( which, constraint );
}

void PHISubmitButtonItem::setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col )
{
    if ( ir==PHIPalette::WidgetText ) {
        setData( DColor, col );
        setColorRole( ir, cr );
    } else if ( ir==PHIPalette::WidgetBase ) {
        setData( DBackgroundColor, col );
        setColorRole( ir, cr );
        // @todo: set button background color with stylesheet?
        return;
    } else return;
    QWidget *w=widget();
    if ( !w ) return;
    QPalette::ColorRole role=QPalette::ButtonText;
    //if ( ir==PHIPalette::WidgetBase ) role=QPalette::Button;
    QPalette pal=w->palette();
    pal.setColor( role, col );
    w->setPalette( pal );
}

void PHISubmitButtonItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    setAdjustedRect( PHIInputTools::adjustedButton( req, rect() ) );
    htmlInitItem( script, false );
    if ( Q_UNLIKELY( colorRole( PHIPalette::WidgetText )==PHIPalette::Custom ) )
        script+=BL( ".color('" )+cssColor( realColor() )+BL( "')" );
    if ( Q_UNLIKELY( colorRole( PHIPalette::WidgetBase )==PHIPalette::Custom ) )
        script+=BL( ".bgColor('" )+cssColor( realBackgroundColor() )+BL( "')" );
    script+=BL( ";\n" );
    out+=indent+BL( "<input type=\"submit\" name=\"" )+id()+BL( "\" value=\"" )
        +data( DText ).toByteArray()+'"';
    htmlBase( req, out, script );
    if ( realHeight()>34 ) {
        if ( req->agentEngine()==PHIRequest::WebKit ) out+=BL( "-webkit-appearance:button" );
    }
    out+=BL( "\">\n" );
}

void PHISubmitButtonItem::click( const QGraphicsSceneMouseEvent *e )
{
    Q_UNUSED( e )
    emit submitClicked( name() );
}

void PHIResetButtonItem::ideInit()
{
    PHISubmitButtonItem::ideInit();
    textData()->setText( tr( "Reset" ) );
}

void PHIResetButtonItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    setAdjustedRect( PHIInputTools::adjustedButton( req, rect() ) );
    htmlInitItem( script, false );
    if ( Q_UNLIKELY( colorRole( PHIPalette::WidgetText )==PHIPalette::Custom ) )
        script+=BL( ".color('" )+cssColor( realColor() )+BL( "')" );
    if ( Q_UNLIKELY( colorRole( PHIPalette::WidgetBase )==PHIPalette::Custom ) )
        script+=BL( ".bgColor('" )+cssColor( realBackgroundColor() )+BL( "')" );
    script+=BL( ";\n" );
    out+=indent+BL( "<input type=\"reset\" value=\"" )+data( DText ).toByteArray()+'"';
    htmlBase( req, out, script );
    if ( realHeight()>34 ) {
        if ( req->agentEngine()==PHIRequest::WebKit ) out+=BL( "-webkit-appearance:button" );
    }
    out+=BL( "\">\n" );
}

void PHIResetButtonItem::click( const QGraphicsSceneMouseEvent *e )
{
    Q_UNUSED( e )
    emit resetClicked( name() );
}

void PHIButtonItem::ideInit()
{
    PHISubmitButtonItem::ideInit();
    textData()->setText( tr( "Button" ) );
}

void PHIButtonItem::squeeze()
{
    PHISubmitButtonItem::squeeze();
    if ( data( DUrl ).toByteArray().isEmpty() ) removeData( DUrl );
}

void PHIButtonItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    setAdjustedRect( PHIInputTools::adjustedButton( req, rect() ) );
    htmlInitItem( script, false );
    if ( Q_UNLIKELY( colorRole( PHIPalette::WidgetText )==PHIPalette::Custom ) )
        script+=BL( ".color('" )+cssColor( realColor() )+BL( "')" );
    if ( Q_UNLIKELY( colorRole( PHIPalette::WidgetBase )==PHIPalette::Custom ) )
        script+=BL( ".bgColor('" )+cssColor( realBackgroundColor() )+BL( "')" );
    if ( !data( DUrl ).toByteArray().isEmpty() ) {
        if ( !data( DUrl ).toByteArray().startsWith( "javascript:" ) )
            script+=BL( ".click(function(){phi.href('" )+data( DUrl ).toByteArray().replace( '\'', BL( "\\'" ) )+BL( "')})" );
        else script+=BL( ".click(function(){" )+data( DUrl ).toByteArray().remove( 0, 11 )+BL( "})" );
    }
    script+=BL( ";\n" );
    out+=indent+BL( "<input type=\"button\" value=\"" )+data( DText ).toByteArray()+'"';
    htmlBase( req, out, script );
    if ( realHeight()>34 ) {
        if ( req->agentEngine()==PHIRequest::WebKit ) out+=BL( "-webkit-appearance:button" );
    }
    out+=BL( "\">\n" );
}

void PHIButtonItem::click( const QGraphicsSceneMouseEvent *e )
{
    Q_UNUSED( e )
    if ( realUrl().isEmpty() ) return;
    if ( realUrl().startsWith( L1( "javascript:" ) ) ) {
        scriptEngine()->evaluate( realUrl().remove( 0, 11 ) );
        return;
    }
    emit linkRequested( realUrl() );
}

QScriptValue PHIButtonItem::url( const QScriptValue &u )
{
    if ( !isServerItem() ) return QScriptValue( QScriptValue::UndefinedValue );
    if ( !u.isValid() ) return realUrl();
    setUrl( u.toString() );
    return self();
}

void PHIImageButtonItem::ideInit()
{
    PHIButtonItem::ideInit();
    _imageData.setImage( QImage( L1( ":/file/phiappview" ) ) );
}

void PHIImageButtonItem::ideUpdateData()
{
    PHIButtonItem::ideUpdateData();
    QPushButton *b=qobject_cast<QPushButton*>(widget());
    if ( !b ) return;
    if ( _imageData.isUnparsedTranslated() ) {
        b->setIcon( QPixmap::fromImage( _imageData.image( page()->currentLang() ) ) );
    } else {
        b->setIcon( QPixmap::fromImage( _imageData.image() ) );
    }
    int s=qRound( realHeight() )-24;
    if ( s<16 ) s=16;
    b->setIconSize( QSize( s, s ) );
}

QSizeF PHIImageButtonItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    if ( which==Qt::MinimumSize ) return QSizeF( 60., 24. );
    if ( which==Qt::PreferredSize ) return QSizeF( 80., 35. );
    return PHIButtonItem::sizeHint( which, constraint );
}

bool PHIImageButtonItem::paint( QPainter *painter, const QRectF &exposed )
{
    Q_UNUSED( exposed )
#ifdef Q_OS_MAC
    painter->translate( 0, 1 );
#else
    Q_UNUSED( painter )
#endif
    return false;
}

void PHIImageButtonItem::squeeze()
{
    PHIButtonItem::squeeze();
    removeData( DImage );
}

void PHIImageButtonItem::loadItemData( QDataStream &in, int version )
{
    PHIButtonItem::loadItemData( in, version );
    QByteArray arr;
    in >> arr;
    arr=qUncompress( arr );
    QDataStream ds( &arr, QIODevice::ReadOnly );
    ds.setVersion( PHI_DSV2 );
    ds >> &_imageData;
}

void PHIImageButtonItem::saveItemData( QDataStream &out, int version )
{
    PHIButtonItem::saveItemData( out, version );
    QSize s=QSize( realSize().toSize().height(), realSize().toSize().height() );
    if ( _imageData.isTranslated() ) {
        foreach ( QByteArray lang, _imageData.langs() ) {
            QImage img=_imageData.image( lang );
            if ( img.isNull() ) _imageData.remove( lang );
            else {
                if ( img.size()!=s ) img=img.scaled( s, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
                _imageData.setImage( img, lang );
            }
        }
    } else {
        QImage img=_imageData.image();
        if ( img.isNull() ) _imageData.remove( _imageData.c() );
        else {
            foreach( QByteArray key, _imageData.langs() ) {
                _imageData.remove( key );
            }
            if ( img.size()!=s ) img=img.scaled( s, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
            _imageData.setImage( img );
        }
    }
    QByteArray arr;
    QDataStream ds( &arr, QIODevice::WriteOnly );
    ds.setVersion( PHI_DSV2 );
    ds << &_imageData;
    out << qCompress( arr, 9 );
}

void PHIImageButtonItem::phisCreateData( const PHIDataParser &parser )
{
    PHIButtonItem::phisCreateData( parser );
    parser.createImages( &_imageData );
}

void PHIImageButtonItem::phisParseData( const PHIDataParser &parser )
{
    PHIButtonItem::phisParseData( parser );
    setImagePath( parser.imagePath( &_imageData ) );
}

void PHIImageButtonItem::clientInitData()
{
    PHIButtonItem::clientInitData();
    QUrl url=page()->baseUrl();
    if ( imagePath().startsWith( '/' ) ) {
        url.setPath( QString::fromUtf8( imagePath() ) );
    } else {
        url.setPath( L1( "/phi.phis" ) );
        QUrlQuery query;
        query.addQueryItem( L1( "i" ), QString::fromUtf8( imagePath() ) );
        query.addQueryItem( L1( "t" ) , L1( "1" ) );
        url.setQuery( query );
    }
    PHIImageRequest *req=new PHIImageRequest( this, url );
    connect( req, &PHIImageRequest::imageReady, this, &PHIImageButtonItem::slotImageReady );
}

void PHIImageButtonItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    setAdjustedRect( PHIInputTools::adjustedImageButton( req, rect() ) );
    htmlInitItem( script, false );
    if ( Q_UNLIKELY( colorRole( PHIPalette::WidgetText )==PHIPalette::Custom ) )
        script+=BL( ".color('" )+cssColor( realColor() )+BL( "')" );
    if ( Q_UNLIKELY( colorRole( PHIPalette::WidgetBase )==PHIPalette::Custom ) )
        script+=BL( ".bgColor('" )+cssColor( realBackgroundColor() )+BL( "')" );
    if ( !data( DUrl ).toByteArray().isEmpty() ) {
        if ( !data( DUrl ).toByteArray().startsWith( "javascript:" ) )
            script+=BL( ".click(function(){phi.href('" )+data( DUrl ).toByteArray().replace( '\'', BL( "\\'" ) )+BL( "')})" );
        else script+=BL( ".click(function(){" )+data( DUrl ).toByteArray().remove( 0, 11 )+BL( "})" );
    }
    script+=BL( ";\n" );
    out+=indent+BL( "<button" );
    htmlBase( req, out, script );
    if ( Q_UNLIKELY( req->agentFeatures() & PHIRequest::IE678 ) ) {
        QByteArray s=QByteArray::number( qRound( realHeight()-24 < 16 ? 16 : realHeight()-24 ) );
        out+=BL( "\"><table width=\"100%\"><tr><td style=\"padding:0;width:" )+s+BL( "px\"><div style=\"width:" );
        out+=s+BL( "px;height:" )+s+BL( "px;filter:" )+cssImageIEFilter( imagePath(), false )+BL( "\"></div>" );
        out+=BL( "</td><td>" )+data( DText ).toByteArray()+BL( "</td></tr></table>" );
    } else {
        QByteArray s=QByteArray::number( qRound( realHeight()-24 < 16 ? 16 : realHeight()-24 ) );
        out+=BL( "\"><table width=\"100%\"><tr><td style=\"padding:0;padding-top:2px;width:" )+s+BL( "px\"><img alt=\"\"" );
        out+=BL( " width=\"" )+s+BL( "\" height=\"" )+s+BL( "\" src=\"" );
        if ( dirtyFlags() & DFUseFilePathInHTML ) out+=imagePath()+BL( "\">" );
        else out+=BL( "phi.phis?i=" )+imagePath()+BL( "&amp;t=1\">" );
        out+=BL( "</td><td>" )+data( DText ).toByteArray()+BL( "</td></tr></table>" );
    }
    out+=BL( "</button>\n" );
}

void PHIImageButtonItem::slotImageReady( const QImage &img )
{
    QPushButton *b=qobject_cast<QPushButton*>(widget());
    b->setIcon( QPixmap::fromImage( img ) );
    int s=qRound( realHeight() )-24;
    if ( s<16 ) s=16;
    b->setIconSize( QSize( s, s ) );
}

void PHIFileButtonItem::ideInit()
{
    PHIAbstractTextItem::ideInit();
    textData()->setText( L1( "*.*" ) );
}

void PHIFileButtonItem::initWidget()
{
    _edit=new QLineEdit();
    _edit->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed, QSizePolicy::LineEdit ) );
    _edit->setReadOnly( true );
    _tool=new QToolButton();
    _tool->setArrowType( Qt::RightArrow );
    _tool->setMinimumHeight( 22 );
    _tool->setFixedWidth( 22 );
    _tool->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding, QSizePolicy::ToolButton ) );
    QWidget *w=new QWidget();
    QHBoxLayout *l=new QHBoxLayout();
    l->setContentsMargins( 0, 0, 0, 0 );
    l->setSpacing( 3 );
    l->addWidget( _edit );
    l->addWidget( _tool );
    w->setLayout( l );
    w->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed, QSizePolicy::LineEdit ) );
    setWidget( w );
    setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed, QSizePolicy::LineEdit ) );
    QPalette pal=w->palette();
    pal.setColor( QPalette::Window, Qt::transparent );
    w->setPalette( pal );
    if ( !isClientItem() ) return;
    connect( _tool, &QToolButton::clicked, this, &PHIFileButtonItem::slotToolClicked );
}

QSizeF PHIFileButtonItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    if ( which==Qt::MinimumSize ) return QSizeF( 200., 22. );
    if ( which==Qt::PreferredSize ) return QSizeF( 200., 22.);
    return PHIAbstractInputItem::sizeHint( which, constraint );
}

void PHIFileButtonItem::clientPostData( QHttpMultiPart *multiPart ) const
{
    Q_ASSERT( _edit );
    QFile *file=new QFile( _edit->text(), multiPart );
    if ( !file->exists() ) return;
    if ( !file->open( QIODevice::ReadOnly ) ) return;
    QByteArray mime=PHI::mimeTypeForFile( QFileInfo( *file ) );
    QHttpPart part;
    part.setHeader( QNetworkRequest::ContentTypeHeader, mime );
    part.setHeader( QNetworkRequest::ContentDispositionHeader, BL( "form-data; name=\"" )+id()+BL( "\"; filename=\"" )
        +QFileInfo( file->fileName() ).fileName().toUtf8()+BL( "\"" ) );
    part.setBodyDevice( file );
    multiPart->append( part );
}

void PHIFileButtonItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    setAdjustedRect( PHIInputTools::adjustedFileButton( req, rect() ) );
    htmlInitItem( script );
    out+=indent+BL( "<input type=\"file\" name=\"" )+id()+'"';
    htmlBase( req, out, script );
    out+=BL( "\">\n" );
}

void PHIFileButtonItem::slotToolClicked()
{
    QFileDialog dlg( graphicsWidget()->scene()->views().first(), Qt::Sheet | Qt::WindowMaximizeButtonHint );
    dlg.setAcceptMode( QFileDialog::AcceptOpen );
    dlg.setFileMode( QFileDialog::ExistingFile );
    dlg.setDirectory( QDir::home() );
    dlg.setNameFilter( realText() );
    dlg.setReadOnly( true );
    dlg.setViewMode( QFileDialog::Detail );
    dlg.exec();
    _edit->setText( dlg.selectedFiles().count() ? dlg.selectedFiles().first() : QString() );
}
