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
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include "phiinputitems.h"
#include "phidatasources.h"
#include "phibasepage.h"
#include "phidataparser.h"
#include "phirequest.h"

void PHILineEditItem::initWidget()
{
    QLineEdit *edit=new QLineEdit();
    if ( isIdeItem() ) edit->setReadOnly( true );
    setWidget( edit );
    setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed, QSizePolicy::LineEdit ) );
}

void PHILineEditItem::initIDE()
{
    PHIAbstractInputItem::initIDE();
    textData()->setText( QString() );
}

void PHILineEditItem::setWidgetText( const QString &s )
{
    QLineEdit *edit=qobject_cast<QLineEdit*>(widget());
    Q_ASSERT( edit );
    edit->setText( s );
}

QSizeF PHILineEditItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    if ( which==Qt::PreferredSize ) {
        QSizeF s=PHIAbstractInputItem::sizeHint( which, constraint );
        if ( s.height()<24. ) s.setHeight( 24. );
        s.setWidth( 144 );
        return s;
    }
    return PHIAbstractInputItem::sizeHint( which, constraint );
}

void PHILineEditItem::setReadOnly( bool b )
{
    PHIBaseItem::setReadOnly( b );
    QLineEdit *edit=qobject_cast<QLineEdit*>(widget());
    if ( !edit ) return;
    edit->setReadOnly( b );
}

void PHILineEditItem::squeeze()
{
    PHIAbstractInputItem::squeeze();
    removeData( DPlaceholder );
}

void PHILineEditItem::saveItemData( QDataStream &out, int version )
{
    PHIAbstractInputItem::saveItemData( out, version );
    out << &_placeholderData;
}

void PHILineEditItem::loadItemData(QDataStream &in, int version)
{
    PHIAbstractInputItem::loadItemData( in, version );
    in >> &_placeholderData;
}

void PHILineEditItem::updateData()
{
    PHIAbstractInputItem::updateData();
    if ( _placeholderData.translated() ) setPlaceholder( _placeholderData.text( page()->currentLang() ) );
    else setPlaceholder( _placeholderData.text() );
}

void PHILineEditItem::createTmpData( const PHIDataParser &parser )
{
    PHIAbstractInputItem::createTmpData( parser );
    if ( _placeholderData.unparsedStatic() ) setPlaceholder( _placeholderData.text() );
    else setDirtyFlag( DFPlaceholder );
}

void PHILineEditItem::parseData( const PHIDataParser &parser )
{
    PHIAbstractInputItem::parseData( parser );
    if ( dirtyFlags() & DFPlaceholder ) setData( DPlaceholder, parser.text( &_placeholderData ) );
    switch ( parser.request()->agentEngine() ) {
    case PHIRequest::WebKit: setAdjustedRect( QRectF( 0, 0, realWidth()-6, realHeight()-6 ) ); break;
    //case PHIRequest::Trident: setAdjustedRect( QRectF( 0, 0, 0, -6 ) ); break;
    //case PHIRequest::Gecko: setAdjustedRect( QRectF( 0, 0, 0, 0 ) ); break;
    //case PHIRequest::Presto: setAdjustedRect( QRectF( 0, 0, 0, 0 ) ); break;
    //default: setAdjustedRect( QRectF( 0, 0, 0, 0 ) );
    }
}

void PHILineEditItem::html( const PHIRequest *req, QByteArray &out, QByteArray &jquery, const QByteArray &indent ) const
{
    genHtml( BL( "text" ), req, out, jquery, indent );
}

void PHILineEditItem::genHtml( const QByteArray &type, const PHIRequest *req, QByteArray &out, QByteArray &jquery, const QByteArray &indent ) const
{
    out+=indent+BL( "<input type=\"" )+type+BL( "\" name=\"" )+id()+'"';
    QByteArray arr=data( DText ).toByteArray();
    if ( !arr.isEmpty() ) out+=BL( " value=\"" )+arr+'"';
    if ( Q_LIKELY( req->agentFeatures() & PHIRequest::HTML5 ) ) {
        arr=data( DPlaceholder ).toByteArray();
        if ( !arr.isEmpty() ) out+=BL( " placeholder=\"" )+arr+'"';
    }
    startCSS( req, out, jquery );
    out+=BL( "\">\n" );
    genAdjustedSize( jquery );
}

void PHILineEditItem::setPlaceholder( const QString &t )
{
    setData( DPlaceholder, t.toUtf8() );
    QLineEdit *edit=qobject_cast<QLineEdit*>(widget());
    if ( !edit ) return;
    edit->setPlaceholderText( t );
}

void PHIPhoneItem::initWidget()
{
    QLineEdit *edit=qobject_cast<QLineEdit*>(widget());
    Q_ASSERT( edit );
    edit->setValidator( new QRegExpValidator( QRegExp( QString::fromLatin1( PHI::phoneNumberRegExp() ) ), edit ) );
}

void PHIPhoneItem::html( const PHIRequest *req, QByteArray &out, QByteArray &jquery, const QByteArray &indent ) const
{
    QByteArray arr=BL( "text" );
    if ( Q_LIKELY( req->agentFeatures() & PHIRequest::HTML5 ) ) arr=BL( "tel" );
    genHtml( arr, req, out, jquery, indent );
}

void PHIEmailItem::initWidget()
{
    QLineEdit *edit=qobject_cast<QLineEdit*>(widget());
    Q_ASSERT( edit );
    edit->setValidator( new QRegExpValidator( QRegExp( QString::fromLatin1( PHI::emailRegExp() ) ), edit ) );
}

void PHIEmailItem::html( const PHIRequest *req, QByteArray &out, QByteArray &jquery, const QByteArray &indent ) const
{
    QByteArray arr=BL( "text" );
    if ( Q_LIKELY( req->agentFeatures() & PHIRequest::HTML5 ) ) arr=BL( "email" );
    genHtml( arr, req, out, jquery, indent );
}

void PHITextAreaItem::initWidget()
{
    QPlainTextEdit *edit=new QPlainTextEdit();
#ifdef Q_OS_MAC
    edit->setFrameStyle( QFrame::Box );
#endif
    setWidget( edit );
}

void PHITextAreaItem::initIDE()
{
    PHIAbstractInputItem::initIDE();
    QPlainTextEdit *edit=qobject_cast<QPlainTextEdit*>(widget());
    Q_ASSERT( edit );
    edit->setReadOnly( true );
}

void PHITextAreaItem::setWidgetText( const QString &t )
{
    QPlainTextEdit *edit=qobject_cast<QPlainTextEdit*>(widget());
    Q_ASSERT( edit );
    edit->setPlainText( t );
}

void PHITextAreaItem::setReadOnly( bool b )
{
    PHIBaseItem::setReadOnly( b );
    QPlainTextEdit *edit=qobject_cast<QPlainTextEdit*>(widget());
    if ( !edit ) return;
    edit->setReadOnly( b );
}

void PHINumberEditItem::initWidget()
{
    setWidget( new QSpinBox() );
}

void PHINumberEditItem::initIDE()
{
    PHIAbstractInputItem::initIDE();
    textData()->setText( L1( "1:0:100:1" ) );
}

void PHINumberEditItem::setReadOnly( bool b )
{
    PHIBaseItem::setReadOnly( b );
    QSpinBox *spin=qobject_cast<QSpinBox*>(widget());
    if ( !spin ) return;
    spin->setReadOnly( b );
}

void PHINumberEditItem::setWidgetText( const QString &s )
{
    int val, min, max, step;
    PHI::extractNumbers( s.toLatin1(), val, min, max, step );
    QSpinBox *spin=qobject_cast<QSpinBox*>(widget());
    Q_ASSERT( spin );
    spin->setValue( val );
    spin->setMinimum( min );
    spin->setMaximum( max );
    spin->setSingleStep( step );
}

QSizeF PHINumberEditItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    if ( which==Qt::PreferredSize ) {
        QSizeF s=PHIAbstractInputItem::sizeHint( which, constraint );
        if ( s.height()<24. ) s.setHeight( 24. );
        s.setWidth( 80 ); // @todo: adjust width to max decimals
        return s;
    }
    return PHIAbstractInputItem::sizeHint( which, constraint );
}

void PHIRealNumberEditItem::initWidget()
{
    setWidget( new QDoubleSpinBox() );
}

void PHIRealNumberEditItem::initIDE()
{
    PHIAbstractInputItem::initIDE();
    textData()->setText( L1( "1.5:0:10.5:0.5:1" ) );
}

void PHIRealNumberEditItem::setWidgetText( const QString &s )
{
    qreal val, min, max, step;
    int dec;
    PHI::extractRealNumbers( s.toLatin1(), val, min, max, step, dec );
    QDoubleSpinBox *spin=qobject_cast<QDoubleSpinBox*>(widget());
    Q_ASSERT( spin );
    spin->setValue( val );
    spin->setMinimum( min );
    spin->setMaximum( max );
    spin->setSingleStep( step );
    spin->setDecimals( dec );
}

void PHIRealNumberEditItem::setReadOnly( bool b )
{
    PHIBaseItem::setReadOnly( b );
    QDoubleSpinBox *spin=qobject_cast<QDoubleSpinBox*>(widget());
    if( !spin ) return;
    spin->setReadOnly( b );
}

QSizeF PHIRealNumberEditItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    if ( which==Qt::PreferredSize ) {
        QSizeF s=PHIAbstractInputItem::sizeHint( which, constraint );
        if ( s.height()<24. ) s.setHeight( 24. );
        s.setWidth( 80 ); // @todo: adjust width to max decimals
        return s;
    }
    return PHIAbstractInputItem::sizeHint( which, constraint );
}

void PHIPasswordItem::initWidget()
{
    QLineEdit *pwd=qobject_cast<QLineEdit*>(widget());
    Q_ASSERT( pwd );
    pwd->setEchoMode( QLineEdit::PasswordEchoOnEdit );
}

void PHIPasswordItem::html( const PHIRequest *req, QByteArray &out, QByteArray &jquery, const QByteArray &indent ) const
{
    genHtml( BL( "password" ), req, out, jquery, indent );
}

void PHISubmitButtonItem::initWidget()
{
    setWidget( new QPushButton() );
    setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed, QSizePolicy::PushButton ) );
}

void PHISubmitButtonItem::initIDE()
{
    textData()->setText( tr( "Submit" ) );
    setColor( PHIPalette::WidgetBase, PHIPalette::Button, page()->phiPalette().color( PHIPalette::Button ) );
    setColor( PHIPalette::WidgetText, PHIPalette::ButtonText, page()->phiPalette().color( PHIPalette::ButtonText ) );
}

void PHISubmitButtonItem::setWidgetText( const QString &t )
{
    QPushButton *b=qobject_cast<QPushButton*>(widget());
    b->setText( t );
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

void PHIResetButtonItem::initIDE()
{
    PHISubmitButtonItem::initIDE();
    textData()->setText( tr( "Reset" ) );
}

void PHIButtonItem::initIDE()
{
    PHISubmitButtonItem::initIDE();
    textData()->setText( tr( "Button" ) );
}
