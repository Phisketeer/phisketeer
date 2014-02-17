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
    htmlInitItem( script, false );
    if ( Q_UNLIKELY( realReadOnly() ) ) script+=BL( ".readOnly(1)" );
    if ( Q_UNLIKELY( colorRole( PHIPalette::WidgetText )==PHIPalette::Custom ) )
        script+=BL( ".color('" )+cssColor( realColor() )+BL( "')" );
    if ( Q_UNLIKELY( colorRole( PHIPalette::WidgetBase )==PHIPalette::Custom ) )
        script+=BL( ".bgColor('" )+cssColor( realBackgroundColor() )+BL( "')" );
    script+=BL( ";\n" );

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
    setMaxLength( realMaxLength() );
    PHIAbstractInputItem::clientInitData();
}

QString PHILineEditItem::realValue() const
{
    QLineEdit *edit=qobject_cast<QLineEdit*>(widget());
    if ( !edit ) return QString::fromUtf8( data( DText ).toByteArray() );
    return edit->text();
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
    edit->setValidator( new QRegExpValidator( QRegExp( QString::fromLatin1( PHI::emailRegExp() ), Qt::CaseInsensitive ), edit ) );
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
    QPalette pal=edit->palette();
    pal.setColor( QPalette::WindowText, Qt::lightGray );
    edit->setPalette( pal );
#endif
    setWidget( edit );
    setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding, QSizePolicy::Frame ) );
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
    htmlInitItem( script, false );
    if ( Q_UNLIKELY( colorRole( PHIPalette::WidgetText )==PHIPalette::Custom ) )
        script+=BL( ".color('" )+cssColor( realColor() )+BL( "')" );
    if ( Q_UNLIKELY( colorRole( PHIPalette::WidgetBase )==PHIPalette::Custom ) )
        script+=BL( ".bgColor('" )+cssColor( realBackgroundColor() )+BL( "')" );
    script+=BL( ";\n" );
    out+=indent+BL( "<textarea name=\"" )+id()+'"';
    if ( realReadOnly() ) out+=BL( " readonly=\"readonly\"" );
    out+=BL( " maxlength=\"" )+QByteArray::number( realMaxLength() )+'"';
    htmlBase( req, out, script );
    out+=BL( "\">" )+data( DText ).toByteArray()+BL( "</textarea>\n" );
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
    QSpinBox *spin=new QSpinBox();
    setWidget( spin );
    QPalette pal=spin->palette();
    pal.setColor( QPalette::Window, Qt::transparent );
    spin->setPalette( pal );
    if ( !isClientItem() ) return;
    QLineEdit *edit=spin->findChild<QLineEdit*>();
    connect( edit, SIGNAL(editingFinished()), this, SLOT(slotChanged()) );
}

void PHINumberEditItem::slotChanged()
{
    if ( flags() & FHasChangeEventHandler ) trigger( L1( "change" ) );
}

void PHINumberEditItem::ideInit()
{
    PHIAbstractInputItem::ideInit();
    if ( wid()==NumberEdit ) textData()->setText( L1( "1:0:100:1" ) );
    else textData()->setText( L1( "1.5:0:10.5:0.5:1" ) );
}

void PHINumberEditItem::setReadOnly( bool b )
{
    PHIBaseItem::setReadOnly( b );
    if ( wid()==NumberEdit ) {
        QSpinBox *spin=qobject_cast<QSpinBox*>(widget());
        if ( !spin ) return;
        QLineEdit *edit=spin->findChild<QLineEdit*>();
        edit->setReadOnly( b );
    } else {
        QDoubleSpinBox *spin=qobject_cast<QDoubleSpinBox*>(widget());
        if ( !spin ) return;
        QLineEdit *edit=spin->findChild<QLineEdit*>();
        edit->setReadOnly( b );
    }
}

void PHINumberEditItem::cssStatic(const PHIRequest *req, QByteArray &out) const
{
    Q_UNUSED( req )
    out+='#'+id()+BL( " .ui-spinner-input{margin:0;margin-left:1px;" )+cssFont( font() )
        +BL( ";color:" )+realColor().name().toLatin1()+BL( ";background-color:" )+realBackgroundColor().name().toLatin1()+BL( "}\n" );
    out+='#'+id()+BL( " .ui-widget{border-radius:0}\n" );
    //out+='#'+id()+BL( " .ui-spinner{width:" )+QByteArray::number( qRound(realWidth()) )
    //    +BL( "px;height:" )+QByteArray::number( qRound(realHeight()) )+BL( "px}\n" );
}

PHIWID PHINumberEditItem::htmlHeaderExtension( const PHIRequest *req, QByteArray &header ) const
{
    Q_UNUSED( req )
    header+=BL( "<script type=\"text/javascript\" src=\"phi.phis?j=ui-spinner\"></script>\n" );
    return static_cast<PHIWID>(NumberEdit);
}

void PHINumberEditItem::setWidgetText( const QString &s )
{
    int val, min, max, step;
    PHI::extractNumbers( s.toLatin1(), val, min, max, step );
    QSpinBox *spin=qobject_cast<QSpinBox*>(widget());
    if ( !spin ) return;
    spin->setValue( val );
    spin->setMinimum( min );
    spin->setMaximum( max );
    spin->setSingleStep( step );
}

QScriptValue PHINumberEditItem::min( const QScriptValue &m )
{
    int val, min, max, step;
    PHI::extractNumbers( data( DText ).toByteArray(), val, min, max, step );
    if ( !m.isValid() ) return min;
    setText( QString( SL( "%1:%2:%3:%4" ) ).arg( val ).arg( m.toInt32() ).arg( max ).arg( step ) );
    QSpinBox *spin=qobject_cast<QSpinBox*>(widget());
    if ( spin ) spin->setMinimum( m.toInt32() );
    return self();
}

QScriptValue PHINumberEditItem::max( const QScriptValue &m )
{
    int val, min, max, step;
    PHI::extractNumbers( data( DText ).toByteArray(), val, min, max, step );
    if ( !m.isValid() ) return max;
    setText( QString( SL( "%1:%2:%3:%4" ) ).arg( val ).arg( min ).arg( m.toInt32() ).arg( step ) );
    QSpinBox *spin=qobject_cast<QSpinBox*>(widget());
    if ( spin ) spin->setMaximum( m.toInt32() );
    return self();
}

QScriptValue PHINumberEditItem::step( const QScriptValue &m )
{
    int val, min, max, step;
    PHI::extractNumbers( data( DText ).toByteArray(), val, min, max, step );
    if ( !m.isValid() ) return step;
    setText( QString( SL( "%1:%2:%3:%4" ) ).arg( val ).arg( min ).arg( max ).arg( m.toInt32() ) );
    QSpinBox *spin=qobject_cast<QSpinBox*>(widget());
    if ( spin ) spin->setSingleStep( m.toInt32() );
    return self();
}

void PHINumberEditItem::setValue( const QString &v )
{
    int val, min, max, step;
    PHI::extractNumbers( data( DText ).toByteArray(), val, min, max, step );
    setText( QString( SL( "%1:%2:%3:%4" ) ).arg( v.toInt() ).arg( min ).arg( max ).arg( step ) );
    QSpinBox *spin=qobject_cast<QSpinBox*>(widget());
    if ( !spin ) return;
    spin->setValue( v.toInt() );
}

QString PHINumberEditItem::realValue() const
{
    QSpinBox *spin=qobject_cast<QSpinBox*>(widget());
    if ( spin ) return spin->text();
    int val, min, max, step;
    PHI::extractNumbers( data( DText ).toByteArray(), val, min, max, step );
    return QString::number( val );
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

void PHINumberEditItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    setAdjustedRect( rect().adjusted( 0, 0, 0, -2 ) );
    htmlInitItem( script, false );
    if ( colorRole( PHIPalette::WidgetText )!=PHIPalette::Text ) script+=BL( ".color('" )+cssColor( realColor() )+BL( "')" );
    if ( colorRole( PHIPalette::WidgetBase )!=PHIPalette::Base ) script+=BL( ".bgColor('" )+cssColor( realBackgroundColor() )+BL( "')" );
    int val, min, max, step;
    PHI::extractNumbers( data( DText ).toByteArray(), val, min, max, step );
    script+=BL( ".val(" )+QByteArray::number( val )+BL( ").min(" )
        +QByteArray::number( min )+BL( ").max(" )+QByteArray::number( max )
        +BL( ").step(" )+QByteArray::number( step )+BL( ");\n" );
    out+=indent+BL( "<div" );
    htmlBase( req, out, script );
    out+=BL( "\"><input id=\"" )+id()+BL( "_phi\" name=\"" )+id()+BL( "\"></div>\n" );
}

void PHIRealNumberEditItem::initWidget()
{
    QDoubleSpinBox *spin=new QDoubleSpinBox();
    setWidget( spin );
    QPalette pal=spin->palette();
    pal.setColor( QPalette::Window, Qt::transparent );
    spin->setPalette( pal );
    if ( !isClientItem() ) return;
    QLineEdit *edit=spin->findChild<QLineEdit*>();
    connect( edit, SIGNAL(editingFinished()), this, SLOT(slotChanged()) );
}

void PHIRealNumberEditItem::slotChanged()
{
    if ( flags() & FHasChangeEventHandler ) trigger( L1( "change" ) );
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

QScriptValue PHIRealNumberEditItem::min( const QScriptValue &m )
{
    qreal val, min, max, step;
    int decimals;
    PHI::extractRealNumbers( data( DText ).toByteArray(), val, min, max, step, decimals );
    if ( !m.isValid() ) return min;
    setText( QString( SL( "%1:%2:%3:%4:%5" ) ).arg( val ).arg( m.toNumber() ).arg( max ).arg( step ).arg( decimals ) );
    QDoubleSpinBox *spin=qobject_cast<QDoubleSpinBox*>(widget());
    if ( spin ) spin->setMinimum( m.toNumber() );
    return self();
}

QScriptValue PHIRealNumberEditItem::max( const QScriptValue &m )
{
    qreal val, min, max, step;
    int decimals;
    PHI::extractRealNumbers( data( DText ).toByteArray(), val, min, max, step, decimals );
    if ( !m.isValid() ) return max;
    setText( QString( SL( "%1:%2:%3:%4:%5" ) ).arg( val ).arg( min ).arg( m.toNumber() ).arg( step ).arg( decimals ) );
    QDoubleSpinBox *spin=qobject_cast<QDoubleSpinBox*>(widget());
    if ( spin ) spin->setMaximum( m.toNumber() );
    return self();
}

QScriptValue PHIRealNumberEditItem::step( const QScriptValue &m )
{
    qreal val, min, max, step;
    int decimals;
    PHI::extractRealNumbers( data( DText ).toByteArray(), val, min, max, step, decimals );
    if ( !m.isValid() ) return step;
    setText( QString( SL( "%1:%2:%3:%4:%5" ) ).arg( val ).arg( min ).arg( max ).arg( m.toNumber() ).arg( decimals ) );
    QDoubleSpinBox *spin=qobject_cast<QDoubleSpinBox*>(widget());
    if ( spin ) spin->setSingleStep( m.toNumber() );
    return self();
}

void PHIRealNumberEditItem::setValue( const QString &v )
{
    qreal val, min, max, step;
    int decimals;
    PHI::extractRealNumbers( data( DText ).toByteArray(), val, min, max, step, decimals );
    setText( QString( SL( "%1:%2:%3:%4:%5" ) ).arg( v.toDouble() ).arg( min ).arg( max ).arg( step ).arg( decimals ) );
    QDoubleSpinBox *spin=qobject_cast<QDoubleSpinBox*>(widget());
    if ( !spin ) return;
    spin->setValue( v.toDouble() );
}

QString PHIRealNumberEditItem::realValue() const
{
    QDoubleSpinBox *spin=qobject_cast<QDoubleSpinBox*>(widget());
    if ( spin ) return spin->text();
    qreal val, min, max, step;
    int decimals;
    PHI::extractRealNumbers( data( DText ).toByteArray(), val, min, max, step, decimals );
    return QString::number( val, 'g', decimals );
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

void PHIRealNumberEditItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    setAdjustedRect( rect().adjusted( 0, 0, 0, -2 ) );
    htmlInitItem( script, false );
    if ( colorRole( PHIPalette::WidgetText )!=PHIPalette::Text ) script+=BL( ".color('" )+cssColor( realColor() )+BL( "')" );
    if ( colorRole( PHIPalette::WidgetBase )!=PHIPalette::Base ) script+=BL( ".bgColor('" )+cssColor( realBackgroundColor() )+BL( "')" );
    qreal val, min, max, step;
    int decimals;
    PHI::extractRealNumbers( data( DText ).toByteArray(), val, min, max, step, decimals );
    script+=BL( ".val(" )+QByteArray::number( val, 'g', decimals )+BL( ").min(" )
        +QByteArray::number( min )+BL( ").max(" )+QByteArray::number( max )
        +BL( ").step(" )+QByteArray::number( step )+BL( ");\n" );
    out+=indent+BL( "<div" );
    htmlBase( req, out, script );
    out+=BL( "\"><input id=\"" )+id()+BL( "_phi\" name=\"" )+id()+BL( "\"></div>\n" );
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
