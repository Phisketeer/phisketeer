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
#include "phimiscitems.h"
#include "phitableitems.h"
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
    virtual PHIBaseItem* copy( const PHIBaseItem *it ) const;
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
    case PHIHiddenItem::Hidden: return new PHIHiddenItem( p );
    case PHIDecoratedTableItem::DecoratedTable: return new PHIDecoratedTableItem( p );
    case PHIRolloverItem::Rollover: return new PHIRolloverItem( p );
    case PHIPasswordItem::Password: return new PHIPasswordItem( p );
    case PHIEmailItem::Email: return new PHIEmailItem( p );
    case PHINumberEditItem::NumberEdit: return new PHINumberEditItem( p );
    case PHIRealNumberEditItem::RealNumberEdit: return new PHIRealNumberEditItem( p );
    case PHIPhoneItem::Phone: return new PHIPhoneItem( p );
    case PHISearchItem::Search: return new PHISearchItem( p );
    case PHICheckListItem::CheckList: return new PHICheckListItem( p );
    case PHIDateEditItem::DateEdit: return new PHIDateEditItem( p );
    case PHICalendarItem::Calendar: return new PHICalendarItem( p );
    case PHISelectCountryItem::CountrySelect: return new PHISelectCountryItem( p );
    case PHIResetButtonItem::ResetButton: return new PHIResetButtonItem( p );
    }
    return 0;
}

inline PHIBaseItem* PHIInput::copy( const PHIBaseItem *it ) const
{
    switch ( it->wid() ) {
    case PHILineEditItem::LineEdit: return new PHILineEditItem( *qobject_cast<const PHILineEditItem*>(it) );
    case PHICheckBoxItem::Checkbox: return new PHICheckBoxItem( *qobject_cast<const PHICheckBoxItem*>(it) );
    case PHIRadioButtonItem::RadioButton: return new PHIRadioButtonItem( *qobject_cast<const PHIRadioButtonItem*>(it) );
    case PHITextAreaItem::TextArea: return new PHITextAreaItem( *qobject_cast<const PHITextAreaItem*>(it) );
    case PHISelectItem::Select: return new PHISelectItem( *qobject_cast<const PHISelectItem*>(it) );
    case PHIMultiSelectItem::MultiSelect: return new PHIMultiSelectItem( *qobject_cast<const PHIMultiSelectItem*>(it) );
    case PHISubmitButtonItem::SubmitButton: return new PHISubmitButtonItem( *qobject_cast<const PHISubmitButtonItem*>(it) );
    case PHIButtonItem::Button: return new PHIButtonItem( *qobject_cast<const PHIButtonItem*>(it) );
    case PHIHiddenItem::Hidden: return new PHIHiddenItem( *qobject_cast<const PHIHiddenItem*>(it) );
    case PHIDecoratedTableItem::DecoratedTable: return new PHIDecoratedTableItem( *qobject_cast<const PHIDecoratedTableItem*>(it) );
    case PHIRolloverItem::Rollover: return new PHIRolloverItem( *qobject_cast<const PHIRolloverItem*>(it) );
    case PHIPasswordItem::Password: return new PHIPasswordItem( *qobject_cast<const PHIPasswordItem*>(it) );
    case PHISearchItem::Search: return new PHISearchItem( *qobject_cast<const PHISearchItem*>(it) );
    case PHIDateEditItem::DateEdit: return new PHIDateEditItem( *qobject_cast<const PHIDateEditItem*>(it) );
    case PHICalendarItem::Calendar: return new PHICalendarItem( *qobject_cast<const PHICalendarItem*>(it) );
    case PHIEmailItem::Email: return new PHIEmailItem( *qobject_cast<const PHIEmailItem*>(it) );
    case PHINumberEditItem::NumberEdit: return new PHINumberEditItem( *qobject_cast<const PHINumberEditItem*>(it) );
    case PHIRealNumberEditItem::RealNumberEdit: return new PHIRealNumberEditItem( *qobject_cast<const PHIRealNumberEditItem*>(it) );
    case PHIPhoneItem::Phone: return new PHIPhoneItem( *qobject_cast<const PHIPhoneItem*>(it) );
    case PHICheckListItem::CheckList: return new PHICheckListItem( *qobject_cast<const PHICheckListItem*>(it) );
    case PHISelectCountryItem::CountrySelect: return new PHISelectCountryItem( *qobject_cast<const PHISelectCountryItem*>(it) );
    case PHIResetButtonItem::ResetButton: return new PHIResetButtonItem( *qobject_cast<const PHIResetButtonItem*>(it) );
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
        << SL( "phone" ) << SL( "calendar" ) << SL( "dateedit" )
        << SL( "hidden" ) << SL( "search" ) << SL( "dectable" )
        << SL( "checklist" ) << SL( "rollover" );
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
    if ( key==L1( "dectable" ) ) return PHIDecoratedTableItem::DecoratedTable;
    if ( key==L1( "rollover" ) ) return PHIRolloverItem::Rollover;
    if ( key==L1( "hidden" ) ) return PHIHiddenItem::Hidden;
    if ( key==L1( "submit" ) ) return PHISubmitButtonItem::SubmitButton;
    if ( key==L1( "email" ) ) return PHIEmailItem::Email;
    if ( key==L1( "number" ) ) return PHINumberEditItem::NumberEdit;
    if ( key==L1( "search" ) ) return PHISearchItem::Search;
    if ( key==L1( "dateedit" ) ) return PHIDateEditItem::DateEdit;
    if ( key==L1( "calendar" ) ) return PHICalendarItem::Calendar;
    if ( key==L1( "realnumber" ) ) return PHIRealNumberEditItem::RealNumberEdit;
    if ( key==L1( "phone" ) ) return PHIPhoneItem::Phone;
    if ( key==L1( "checklist" ) ) return PHICheckListItem::CheckList;
    if ( key==L1( "country" ) ) return PHISelectCountryItem::CountrySelect;
    if ( key==L1( "reset" ) ) return PHIResetButtonItem::ResetButton;
    return 0;
}

#endif // PHIINPUT_H
