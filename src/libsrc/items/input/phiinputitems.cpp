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
#include <QPainter>
#include "phiinputitems.h"
#include "phidatasources.h"
#include "phibasepage.h"
#include "phidataparser.h"
#include "phirequest.h"
#include "phiinputtools.h"

void PHILineEditItem::initWidget()
{
    QLineEdit *edit=new QLineEdit();
    if ( isIdeItem() ) edit->setReadOnly( true );
    setWidget( edit );
    setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed, QSizePolicy::LineEdit ) );
    if( !isClientItem() ) return;
    connect( edit, &QLineEdit::editingFinished, this, &PHILineEditItem::slotChanged );
}

void PHILineEditItem::ideInit()
{
    PHIAbstractInputItem::ideInit();
    textData()->setText( QString() );
}

void PHILineEditItem::setWidgetText( const QString &s )
{
    QLineEdit *edit=qobject_cast<QLineEdit*>(widget());
    if ( !edit ) return;
    edit->setText( s );
}

void PHILineEditItem::slotChanged()
{
    QLineEdit *edit=qobject_cast<QLineEdit*>(widget());
    Q_ASSERT( edit );
    setData( DText, edit->text().toUtf8() );
    if ( flags() & FHasChangeEventHandler ) trigger( L1( "change" ) );
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
    qDebug() << "setReadOnly" << b;
    PHIBaseItem::setReadOnly( b );
    QLineEdit *edit=qobject_cast<QLineEdit*>(widget());
    if ( !edit ) return;
    edit->setReadOnly( b );
}

void PHILineEditItem::squeeze()
{
    PHIAbstractInputItem::squeeze();
    removeData( DPlaceholder );
    if ( realMaxLength()==100 ) removeData( DMaxLength );
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

void PHILineEditItem::ideUpdateData()
{
    PHIAbstractInputItem::ideUpdateData();
    if ( _placeholderData.isTranslated() ) setPlaceholder( _placeholderData.text( page()->currentLang() ) );
    else setPlaceholder( _placeholderData.text() );
}

void PHILineEditItem::phisCreateData( const PHIDataParser &parser )
{
    PHIAbstractInputItem::phisCreateData( parser );
    setData( DPlaceholder, parser.text( &_placeholderData ) );
    if ( !_placeholderData.isUnparsedStatic() ) setDirtyFlag( DFPlaceholder );
}

void PHILineEditItem::phisParseData( const PHIDataParser &parser )
{
    PHIAbstractInputItem::phisParseData( parser );
    if ( dirtyFlags() & DFPlaceholder ) setData( DPlaceholder, parser.text( &_placeholderData ) );
}

void PHILineEditItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    genHtml( BL( "text" ), req, out, script, indent );
}

void PHILineEditItem::genHtml( const QByteArray &type, const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    setAdjustedRect( PHIInputTools::adjustedLineEdit( req, rect() ) );
    out+=indent+BL( "<input type=\"" )+type+BL( "\" name=\"" )+id()+'"';
    QByteArray arr=data( DText ).toByteArray();
    if ( !arr.isEmpty() ) out+=BL( " value=\"" )+arr+'"';
    out+=BL( " maxlength=\"" )+QByteArray::number( realMaxLength() )+'"';
    if ( Q_LIKELY( req->agentFeatures() & PHIRequest::HTML5 ) ) {
        arr=data( DPlaceholder ).toByteArray();
        if ( !arr.isEmpty() ) out+=BL( " placeholder=\"" )+arr+'"';
    }
    htmlBase( req, out, script );
    out+=BL( "\">\n" );
    htmlInitItem( script );
}

void PHILineEditItem::setPlaceholder( const QString &t )
{
    setData( DPlaceholder, t.toUtf8() );
    QLineEdit *edit=qobject_cast<QLineEdit*>(widget());
    if ( !edit ) return;
    edit->setPlaceholderText( t );
}

QScriptValue PHILineEditItem::placeholder( const QScriptValue &v )
{
    if ( !v.isValid() ) return realPlaceholder();
    setPlaceholder( v.toString() );
    return self();
}

void PHILineEditItem::setMaxLength( int l )
{
    setData( DMaxLength, l );
    QLineEdit *edit=qobject_cast<QLineEdit*>(widget());
    if ( !edit ) return;
    edit->setMaxLength( l );
}

QScriptValue PHILineEditItem::maxLength( const QScriptValue &l )
{
    if ( !l.isValid() ) return realMaxLength();
    setMaxLength( l.toInt32() );
    return self();
}

void PHILineEditItem::clientInitData()
{
    setPlaceholder( realPlaceholder() );
    PHIAbstractInputItem::clientInitData();
}

void PHIPhoneItem::initWidget()
{
    QLineEdit *edit=qobject_cast<QLineEdit*>(widget());
    Q_ASSERT( edit );
    edit->setValidator( new QRegExpValidator( QRegExp( QString::fromLatin1( PHI::phoneNumberRegExp() ) ), edit ) );
}

void PHIPhoneItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    QByteArray arr=BL( "text" );
    if ( Q_LIKELY( req->agentFeatures() & PHIRequest::HTML5 ) ) arr=BL( "tel" );
    genHtml( arr, req, out, script, indent );
}

void PHIEmailItem::initWidget()
{
    QLineEdit *edit=qobject_cast<QLineEdit*>(widget());
    Q_ASSERT( edit );
    edit->setValidator( new QRegExpValidator( QRegExp( QString::fromLatin1( PHI::emailRegExp() ) ), edit ) );
}

void PHIEmailItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    QByteArray arr=BL( "text" );
    if ( Q_LIKELY( req->agentFeatures() & PHIRequest::HTML5 ) ) arr=BL( "email" );
    genHtml( arr, req, out, script, indent );
}

void PHISearchItem::initWidget()
{
    QLineEdit *edit=qobject_cast<QLineEdit*>(widget());
    Q_ASSERT( edit );
    // @todo: implement clear button
}

void PHISearchItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    QByteArray arr=BL( "text" );
    if ( Q_LIKELY( req->agentFeatures() & PHIRequest::HTML5 ) ) arr=BL( "search" );
    genHtml( arr, req, out, script, indent );
}

void PHITextAreaItem::initWidget()
{
    QPlainTextEdit *edit=new QPlainTextEdit();
#ifdef Q_OS_MAC
    edit->setFrameStyle( QFrame::Box );
#endif
    setWidget( edit );
    if ( !isClientItem() ) return;
    connect( edit, &QPlainTextEdit::textChanged, this, &PHITextAreaItem::slotChanged );
}

void PHITextAreaItem::ideInit()
{
    PHIAbstractInputItem::ideInit();
    QPlainTextEdit *edit=qobject_cast<QPlainTextEdit*>(widget());
    Q_ASSERT( edit );
    edit->setReadOnly( true );
}

void PHITextAreaItem::setWidgetText( const QString &t )
{
    QPlainTextEdit *edit=qobject_cast<QPlainTextEdit*>(widget());
    if ( !edit ) return;
    edit->setPlainText( t );
}

void PHITextAreaItem::setReadOnly( bool b )
{
    PHIBaseItem::setReadOnly( b );
    QPlainTextEdit *edit=qobject_cast<QPlainTextEdit*>(widget());
    if ( !edit ) return;
    edit->setReadOnly( b );
}

void PHITextAreaItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    setAdjustedRect( PHIInputTools::adjustedTextArea( req, rect() ) );
    out+=indent+BL( "<textarea name=\"" )+id()+'"';
    htmlBase( req, out, script );
    out+=BL( "\">" )+data( DText ).toByteArray()+BL( "</textarea>\n" );
    htmlInitItem( script );
}

void PHITextAreaItem::squeeze()
{
    PHIAbstractInputItem::squeeze();
    if ( realMaxLength()==5000 ) removeData( DMaxLength );
}

void PHITextAreaItem::setMaxLength( int l )
{
    setData( DMaxLength, l );
}

void PHITextAreaItem::slotChanged()
{
    QPlainTextEdit *edit=qobject_cast<QPlainTextEdit*>(widget());
    if ( !edit ) return;
    QString tmp=edit->toPlainText();
    if ( tmp.length()>realMaxLength() ) {
        tmp.truncate( realMaxLength() );
        edit->blockSignals( true );
        edit->setPlainText( tmp );
        edit->blockSignals( false );
    }
    setData( DText, tmp.toUtf8() );
    if ( flags() & FHasChangeEventHandler ) trigger( L1( "change" ) );
}

QScriptValue PHITextAreaItem::maxLength( const QScriptValue &l )
{
    if ( !l.isValid() ) return realMaxLength();
    setMaxLength( l.toInt32() );
    return self();
}

void PHINumberEditItem::initWidget()
{
    setWidget( new QSpinBox() );
}

void PHINumberEditItem::ideInit()
{
    PHIAbstractInputItem::ideInit();
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

void PHIRealNumberEditItem::ideInit()
{
    PHIAbstractInputItem::ideInit();
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

void PHIPasswordItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    genHtml( BL( "password" ), req, out, script, indent );
}

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
    painter->translate( 0, -2 );
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
    out+=indent+BL( "<input type=\"submit\" name=\"" )+id()+BL( "\" value=\"" )
        +data( DText ).toByteArray()+'"';
    htmlBase( req, out, script );
    if ( realHeight()>34 ) {
        if ( req->agentEngine()==PHIRequest::WebKit ) out+=BL( "-webkit-appearance:button" );
    }
    out+=BL( "\">\n" );
    htmlInitItem( script );
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
    out+=indent+BL( "<input type=\"reset\" name=\"" )+id()+BL( "\" value=\"" )
        +data( DText ).toByteArray()+'"';
    htmlBase( req, out, script );
    if ( realHeight()>34 ) {
        if ( req->agentEngine()==PHIRequest::WebKit ) out+=BL( "-webkit-appearance:button" );
    }
    out+=BL( "\">\n" );
    htmlInitItem( script );
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
    out+=indent+BL( "<input type=\"button\" name=\"" )+id()+BL( "\" value=\"" )
        +data( DText ).toByteArray()+'"';
    htmlBase( req, out, script );
    if ( realHeight()>34 ) {
        if ( req->agentEngine()==PHIRequest::WebKit ) out+=BL( "-webkit-appearance:button" );
    }
    out+=BL( "\">\n" );
    htmlInitItem( script, false );
    if ( !realUrl().isEmpty() ) {
        qDebug() << "url" <<realUrl();
        QUrl url( realUrl() );
        script+=BL( ".click(function(){phi.href('" )+url.toEncoded()+BL( "')})" );
    }
    script+=BL( ";\n" );
}

void PHIButtonItem::click( const QGraphicsSceneMouseEvent *e )
{
    Q_UNUSED( e )
    if ( !realUrl().isEmpty() ) emit linkRequested( realUrl() );
}
