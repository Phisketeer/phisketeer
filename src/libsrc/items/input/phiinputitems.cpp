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
#include "phi.h"

PHILineEditItem::PHILineEditItem( const PHIBaseItemPrivate &p )
    : PHIAbstractInputItem( p )
{
    setWidget( new QLineEdit() );
    setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed, QSizePolicy::LineEdit ) );
}

void PHILineEditItem::initIDE()
{
    PHIAbstractInputItem::initIDE();
    QLineEdit *edit=qobject_cast<QLineEdit*>(widget());
    Q_ASSERT( edit );
    edit->setReadOnly( true );
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

PHIPhoneItem::PHIPhoneItem( const PHIBaseItemPrivate &p )
    : PHILineEditItem( p )
{
    QLineEdit *edit=qobject_cast<QLineEdit*>(widget());
    if ( edit ) edit->setValidator( new QRegExpValidator( QRegExp( QString::fromLatin1( PHI::phoneNumberRegExp() ) ), edit ) );
}

PHIEmailItem::PHIEmailItem( const PHIBaseItemPrivate &p )
    : PHILineEditItem( p )
{
    QLineEdit *edit=qobject_cast<QLineEdit*>(widget());
    if ( edit ) edit->setValidator( new QRegExpValidator( QRegExp( QString::fromLatin1( PHI::emailRegExp() ) ), edit ) );
}

PHITextAreaItem::PHITextAreaItem( const PHIBaseItemPrivate &p )
    : PHIAbstractInputItem( p )
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

PHINumberEditItem::PHINumberEditItem( const PHIBaseItemPrivate &p )
    : PHIAbstractInputItem( p )
{
    setWidget( new QSpinBox() );
}

void PHINumberEditItem::initIDE()
{
    PHIAbstractInputItem::initIDE();
    textData()->setText( L1( "1:0:100:1" ) );
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

PHIRealNumberEditItem::PHIRealNumberEditItem( const PHIBaseItemPrivate &p )
    : PHIAbstractInputItem( p )
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

PHIPasswordItem::PHIPasswordItem( const PHIBaseItemPrivate &p )
    : PHILineEditItem( p )
{
    QLineEdit *pwd=qobject_cast<QLineEdit*>(widget());
    if ( pwd ) pwd->setEchoMode( QLineEdit::PasswordEchoOnEdit );
}

PHISubmitButtonItem::PHISubmitButtonItem( const PHIBaseItemPrivate &p )
    : PHIAbstractInputItem( p )
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

PHIResetButtonItem::PHIResetButtonItem( const PHIBaseItemPrivate &p )
    : PHISubmitButtonItem( p )
{
}

void PHIResetButtonItem::initIDE()
{
    PHISubmitButtonItem::initIDE();
    textData()->setText( tr( "Reset" ) );
}

PHIButtonItem::PHIButtonItem( const PHIBaseItemPrivate &p )
    : PHISubmitButtonItem( p )
{
}

void PHIButtonItem::initIDE()
{
    PHISubmitButtonItem::initIDE();
    textData()->setText( tr( "Button" ) );
}
