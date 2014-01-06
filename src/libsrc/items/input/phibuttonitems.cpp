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

void PHISubmitButtonItem::initWidget()
{
    setWidget( new QPushButton() );
    setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed, QSizePolicy::PushButton ) );
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

void PHISubmitButtonItem::paint( QPainter *painter, const QRectF &exposed )
{
#ifdef Q_OS_MAC
    painter->translate( 0, -4 );
#endif
    PHIAbstractInputItem::paint( painter, exposed );
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
        setColorRole( cr );
    } else if ( ir==PHIPalette::WidgetBase ) {
        setData( DBackgroundColor, col );
        setBackgroundColorRole( cr );
    } else return;
    QWidget *w=widget();
    if ( !w ) return;
    QPalette::ColorRole role=QPalette::ButtonText;
    if ( ir==PHIPalette::WidgetBase ) role=QPalette::Button;
    QPalette pal=w->palette();
    pal.setColor( role, col );
    w->setPalette( pal );
}

void PHISubmitButtonItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    setAdjustedRect( PHIInputTools::adjustedButton( req, rect() ) );
    htmlInitItem( script );
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
    htmlInitItem( script );
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
    if ( !realUrl().isEmpty() ) {
        qDebug() << "url" << realUrl();
        QUrl url( realUrl() );
        script+=BL( ".click(function(){phi.href('" )+url.toEncoded()+BL( "')})" );
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
    if ( !realUrl().isEmpty() ) emit linkRequested( realUrl() );
}

QScriptValue PHIButtonItem::url( const QScriptValue &u )
{
    if ( !isServerItem() ) return QScriptValue( QScriptValue::UndefinedValue );
    if ( !u.isValid() ) return realUrl();
    setUrl( u.toString() );
    return self();
}

void PHIFileButtonItem::ideInit()
{
    textData()->setText( L1( "*.*" ) );
}

void PHIFileButtonItem::initWidget()
{
    _edit=new QLineEdit();
    _edit->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum, QSizePolicy::LineEdit ) );
    _edit->setReadOnly( true );
    _tool=new QToolButton();
    _tool->setArrowType( Qt::RightArrow );
    _tool->setMinimumHeight( 24 );
    _tool->setFixedWidth( 24 );
    _tool->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding, QSizePolicy::ToolButton ) );
    connect( _tool, &QToolButton::clicked, this, &PHIFileButtonItem::slotToolClicked );
    QWidget *w=new QWidget();
    QHBoxLayout *l=new QHBoxLayout();
    l->setContentsMargins( 0, 0, 0, 0 );
    l->setSpacing( 3 );
    l->addWidget( _edit );
    l->addWidget( _tool );
    w->setLayout( l );
    w->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed, QSizePolicy::LineEdit ) );
    setWidget( w );
    setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed, QSizePolicy::LineEdit ) );
}

QSizeF PHIFileButtonItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    if ( which==Qt::MinimumSize ) return QSizeF( 180., 24. );
    QSizeF s=PHIAbstractInputItem::sizeHint( which, constraint );
    if ( s.height()<24. ) s.setHeight( 24. );
    return s;
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
    part.setHeader( QNetworkRequest::ContentDispositionHeader, BL( "form-data; name=\"" )+id()+BL( "\"" ) );
    part.setBodyDevice( file );
    multiPart->append( part );
}

void PHIFileButtonItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    setAdjustedRect( PHIInputTools::adjustedFileButton( req, rect() ) );
    htmlInitItem( script );
    out+=indent+BL( "<input type=\"file\"" );
    htmlBase( req, out, script );
    out+=BL( "\">\n" );
}

void PHIFileButtonItem::slotToolClicked()
{
    QFileDialog dlg( graphicsWidget()->scene()->views().first(), Qt::Sheet );
    dlg.setAcceptMode( QFileDialog::AcceptOpen );
    dlg.setFileMode( QFileDialog::ExistingFile );
    dlg.setDirectory( QDir::home() );
    dlg.setNameFilter( realText() );
    dlg.setReadOnly( true );
    dlg.setViewMode( QFileDialog::Detail );
    dlg.exec();
    _edit->setText( dlg.selectedFiles().count() ? dlg.selectedFiles().first() : QString() );
}
