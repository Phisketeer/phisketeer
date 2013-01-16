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
#include "phiaaddressdlg.h"

PHIAAddressDlg::PHIAAddressDlg( QWidget *parent )
    : QDialog( parent )
{
    qDebug( "PHIAAddressDlg::PHIAAddressDlg()" );
    setupUi( this );
    _country->setValidator( new QRegExpValidator( QRegExp( "[a-z][a-z]" ), _country ) );
}

PHIAAddressDlg::~PHIAAddressDlg()
{
    qDebug( "PHIAAddressDlg::~PHIAAddressDlg()" );
}

void PHIAAddressDlg::setAddress( const PHIAAddress &a )
{
    if ( a.options & PHIAAddress::Business ) {
        _business->setChecked( true );
        _private->setChecked( false );
    } else {
        _business->setChecked( false );
        _private->setChecked( true );
    }
    _title->setText( a.title );
    _firstname->setText( a.firstname );
    _lastname->setText( a.lastname );
    _organisation->setText( a.organisation );
    _unit->setText( a.unit );
    _street->setText( a.street );
    _state->setText( a.state );
    _city->setText( a.city );
    _zip->setText( a.zip );
    _country->setText( a.country );
}

PHIAAddress PHIAAddressDlg::address() const
{
    PHIAAddress a;
    if ( _business->isChecked() ) {
        a.options |= PHIAAddress::Business;
        a.options &= ~PHIAAddress::Private;
    } else {
        a.options &= ~PHIAAddress::Business;
        a.options |= PHIAAddress::Private;
    }
    a.title=_title->text();
    a.firstname=_firstname->text();
    a.lastname=_lastname->text();
    a.organisation=_organisation->text();
    a.unit=_unit->text();
    a.street=_street->text();
    a.state=_state->text();
    a.city=_city->text();
    a.zip=_zip->text();
    a.country=_country->text();
    return a;
}

PHIACreditCardDlg::PHIACreditCardDlg( QWidget *parent )
    : QDialog( parent )
{
    qDebug( "PHIACreditCardDlg::PHIACreditCardDlg()" );
    setupUi( this );
    _typeCombo->addItem( "Amex" );
    _typeCombo->addItem( "Diners club" );
    _typeCombo->addItem( "Mastercard" );
    _typeCombo->addItem( "Visa" );
    QLocale locale;
    for ( int i=0; i<12; i++ ) {
        _monthCombo->addItem( locale.monthName( i+1, QLocale::LongFormat ) );
    }
    int year=QDate::currentDate().year();
    for ( int i=0; i<20; i++ ) {
        _yearCombo->addItem( QString::number( year+i ) );
    }
}

PHIACreditCardDlg::~PHIACreditCardDlg()
{
    qDebug( "PHIACreditCardDlg::~PHIACreditCardDlg()" );
}

void PHIACreditCardDlg::setCreditCard( const PHIACreditCard &c )
{
    if ( c.options & PHIACreditCard::Business ) {
        _business->setChecked( true );
        _private->setChecked( false );
    } else {
        _business->setChecked( false );
        _private->setChecked( true );
    }
    _name->setText( c.name );
    _civ->setText( c.civ );
    _number->setText( c.number );
    if ( c.type=="A" ) _typeCombo->setCurrentIndex( 0 );
    if ( c.type=="D" ) _typeCombo->setCurrentIndex( 1 );
    if ( c.type=="M" ) _typeCombo->setCurrentIndex( 2 );
    if ( c.type=="V" ) _typeCombo->setCurrentIndex( 3 );
    _monthCombo->setCurrentIndex( c.valid.month()-1 );
    int row=_yearCombo->findText( QString::number( c.valid.year() ) );
    if ( row!=-1 ) _yearCombo->setCurrentIndex( row );
}

PHIACreditCard PHIACreditCardDlg::creditCard() const
{
    PHIACreditCard c;
    if ( _business->isChecked() ) {
        c.options |= PHIACreditCard::Business;
        c.options &= ~PHIACreditCard::Private;
    } else {
        c.options &= ~PHIACreditCard::Business;
        c.options |= PHIACreditCard::Private;
    }
    c.type=_typeCombo->currentText().left( 1 );
    c.name=_name->text();
    c.number=_number->text();
    c.civ=_civ->text();
    c.valid.setDate( _yearCombo->currentText().toInt(), _monthCombo->currentIndex()+1, 1 );
    return c;
}

PHIAContactDlg::PHIAContactDlg( QWidget *parent )
    : QDialog( parent )
{
    qDebug( "PHIAContactDlg::PHIAContactDlg()" );
    setupUi( this );
}

PHIAContactDlg::~PHIAContactDlg()
{
    qDebug( "PHIAContactDlg::~PHIAContactDlg()" );
}

void PHIAContactDlg::setContact( const PHIAContact &c )
{
    if ( c.options & PHIAContact::Business ) {
        _business->setChecked( true );
        _private->setChecked( false );
    } else {
        _business->setChecked( false );
        _private->setChecked( true );
    }
    _name->setText( c.name );
    _email->setText( c.email );
    _mobile->setText( c.mobile );
    _phone->setText( c.phone );
    _fax->setText( c.fax );
    _url->setText( c.url );
}

PHIAContact PHIAContactDlg::contact() const
{
    PHIAContact c;
    if ( _business->isChecked() ) {
        c.options |= PHIAContact::Business;
        c.options &= ~PHIAContact::Private;
    } else {
        c.options &= ~PHIAContact::Business;
        c.options |= PHIAContact::Private;
    }
    c.name=_name->text();
    c.email=_email->text();
    c.mobile=_mobile->text();
    c.phone=_phone->text();
    c.fax=_fax->text();
    c.url=_url->text();
    return c;
}
