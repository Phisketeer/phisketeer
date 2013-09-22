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
#include "phi.h"

PHILineEditItem::PHILineEditItem()
    : PHIAbstractInputItem()
{
    QLineEdit *edit=new QLineEdit();
    if ( isIdeItem() ) edit->setReadOnly( true );
    setWidget( edit );
    setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed, QSizePolicy::LineEdit ) );
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

PHITextAreaItem::PHITextAreaItem()
    : PHIAbstractInputItem()
{
    QPlainTextEdit *edit=new QPlainTextEdit();
    if ( isIdeItem() ) edit->setReadOnly( true );
#ifdef Q_OS_MAC
    edit->setFrameStyle( QFrame::Box );
#endif
    setWidget( edit );
}

void PHITextAreaItem::setWidgetText( const QString &t )
{
    QPlainTextEdit *edit=qobject_cast<QPlainTextEdit*>(widget());
    Q_ASSERT( edit );
    edit->setPlainText( t );
}

PHINumberEditItem::PHINumberEditItem()
    : PHIAbstractInputItem()
{
    setWidget( new QSpinBox() );
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
        s.setWidth( 80 ); // todo: adjust width to max decimals
        return s;
    }
    return PHIAbstractInputItem::sizeHint( which, constraint );
}

PHIRealNumberEditItem::PHIRealNumberEditItem()
    : PHIAbstractInputItem()
{
    setWidget( new QDoubleSpinBox() );
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
        s.setWidth( 80 ); // todo: adjust width to max decimals
        return s;
    }
    return PHIAbstractInputItem::sizeHint( which, constraint );
}

PHIPasswordItem::PHIPasswordItem()
    : PHILineEditItem()
{
    QLineEdit *pwd=new QLineEdit();
    pwd->setEchoMode( QLineEdit::PasswordEchoOnEdit );
    setWidget( pwd );
}

PHISubmitButtonItem::PHISubmitButtonItem()
    : PHIAbstractInputItem()
{
    setWidget( new QPushButton() );
    setColorRole( PHIPalette::ButtonText );
    setBackgroundColorRole( PHIPalette::Button );
    textData()->setText( tr( "Submit" ) );
    setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed, QSizePolicy::PushButton ) );
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

PHIResetButtonItem::PHIResetButtonItem()
    : PHISubmitButtonItem()
{
    textData()->setText( tr( "Reset" ) );
}

PHIButtonItem::PHIButtonItem()
    : PHISubmitButtonItem()
{
    textData()->setText( tr( "Button" ) );
}
