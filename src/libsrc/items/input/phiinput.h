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
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PHIINPUT_H
#define PHIINPUT_H
#include "phiitemplugin.h"
#include "phiinputitems.h"
#include "phicheckitems.h"
#include "philistitems.h"
#include "phicalendaritems.h"
#include "phi.h"

class PHIInput : public PHIItemPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.phisketeer.phi.items.input" FILE "phiinput.json")
    Q_CLASSINFO( "Author", "Marius Schumacher" )
    Q_CLASSINFO( "Url", "http://www.phisketeer.org" )
    Q_CLASSINFO( "Version", "1.0" )
    Q_CLASSINFO( "License", "LGPL" )
    Q_CLASSINFO( "Copyright", "2013 Phisys AG, 2013 Phisketeer Team" )

public:
    virtual PHIBaseItem* create( PHIWID wid, const PHIBaseItemPrivate &p ) const;
    virtual QStringList keys() const;
    virtual PHIWID wid( const QString &key ) const;
    virtual QString category() const { return tr( "Input" ); }
};

inline PHIBaseItem* PHIInput::create( PHIWID wid , const PHIBaseItemPrivate &p ) const
{
    switch ( wid ) {
    case PHILineEditItem::LineEdit: return new PHILineEditItem( p );
    case PHICheckBoxItem::Checkbox: return new PHICheckBoxItem( p );
    case PHIRadioButtonItem::RadioButton: return new PHIRadioButtonItem( p );
    case PHITextAreaItem::TextArea: return new PHITextAreaItem( p );
    case PHISelectItem::Select: return new PHISelectItem( p );
    case PHIMultiSelectItem::MultiSelect: return new PHIMultiSelectItem( p );
    case PHISubmitButtonItem::SubmitButton: return new PHISubmitButtonItem( p );
    case PHIButtonItem::Button: return new PHIButtonItem( p );
    case PHIPasswordItem::Password: return new PHIPasswordItem( p );
    case PHIEmailItem::Email: return new PHIEmailItem( p );
    case PHINumberEditItem::NumberEdit: return new PHINumberEditItem( p );
    case PHIRealNumberEditItem::RealNumberEdit: return new PHIRealNumberEditItem( p );
    case PHIPhoneItem::Phone: return new PHIPhoneItem( p );
    case PHISelectCountryItem::CountrySelect: return new PHISelectCountryItem( p );
    case PHIResetButtonItem::ResetButton: return new PHIResetButtonItem( p );
    case PHIDateEditItem::DateEdit: return new PHIDateEditItem( p );
    case PHICalendarItem::Calendar: return new PHICalendarItem( p );
    }
    return 0;
}

inline QStringList PHIInput::keys() const
{
    return QStringList() << SL( "lineedit" ) << SL( "textarea" )
        << SL( "password" ) << SL( "checkbox" ) << SL( "radiobutton" )
        << SL( "button" ) << SL( "reset" ) << SL( "submit" )
        << SL( "number" ) << SL( "realnumber" ) << SL( "email" )
        << SL( "select" ) << SL( "country" ) << SL( "listbox" )
        << SL( "phone" ) << SL( "calendar" ) << SL( "dateedit" );
}

inline PHIWID PHIInput::wid( const QString &key ) const
{
    if ( key==L1( "lineedit" ) ) return PHILineEditItem::LineEdit;
    if ( key==L1( "textarea" ) ) return PHITextAreaItem::TextArea;
    if ( key==L1( "password" ) ) return PHIPasswordItem::Password;
    if ( key==L1( "checkbox" ) ) return PHICheckBoxItem::Checkbox;
    if ( key==L1( "radiobutton" ) ) return PHIRadioButtonItem::RadioButton;
    if ( key==L1( "select" ) ) return PHISelectItem::Select;
    if ( key==L1( "listbox" ) ) return PHIMultiSelectItem::MultiSelect;
    if ( key==L1( "button" ) ) return PHIButtonItem::Button;
    if ( key==L1( "submit" ) ) return PHISubmitButtonItem::SubmitButton;
    if ( key==L1( "email" ) ) return PHIEmailItem::Email;
    if ( key==L1( "number" ) ) return PHINumberEditItem::NumberEdit;
    if ( key==L1( "realnumber" ) ) return PHIRealNumberEditItem::RealNumberEdit;
    if ( key==L1( "phone" ) ) return PHIPhoneItem::Phone;
    if ( key==L1( "country" ) ) return PHISelectCountryItem::CountrySelect;
    if ( key==L1( "reset" ) ) return PHIResetButtonItem::ResetButton;
    if ( key==L1( "dateedit" ) ) return PHIDateEditItem::DateEdit;
    if ( key==L1( "calendar" ) ) return PHICalendarItem::Calendar;
    return 0;
}

#endif // PHIINPUT_H
