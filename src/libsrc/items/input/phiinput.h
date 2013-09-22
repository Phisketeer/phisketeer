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
    virtual PHIBaseItem* create( PHIWID wid ) const;
    virtual QStringList keys() const;
    virtual PHIWID wid( const QString &key ) const;
    virtual QString category() const { return tr( "Input" ); }
};

inline PHIBaseItem* PHIInput::create( PHIWID wid ) const
{
    switch ( wid ) {
    case PHILineEditItem::LineEdit: return new PHILineEditItem();
    case PHICheckBoxItem::Checkbox: return new PHICheckBoxItem();
    case PHIRadioButtonItem::RadioButton: return new PHIRadioButtonItem();
    case PHITextAreaItem::TextArea: return new PHITextAreaItem();
    case PHISubmitButtonItem::SubmitButton: return new PHISubmitButtonItem();
    case PHIButtonItem::Button: return new PHIButtonItem();
    case PHIPasswordItem::Password: return new PHIPasswordItem();
    case PHINumberEditItem::NumberEdit: return new PHINumberEditItem();
    case PHIRealNumberEditItem::RealNumberEdit: return new PHIRealNumberEditItem();
    case PHIResetButtonItem::ResetButton: return new PHIResetButtonItem();
    }
    return 0;
}

inline QStringList PHIInput::keys() const
{
    return QStringList() << QStringLiteral( "lineedit" ) << QStringLiteral( "textarea" )
        << QStringLiteral( "password" ) << QStringLiteral( "checkbox" ) << QStringLiteral( "radiobutton" )
        << QStringLiteral( "button" ) << QStringLiteral( "reset" ) << QStringLiteral( "submit" )
        << QStringLiteral( "number" ) << QStringLiteral( "realnumber" );
}

inline PHIWID PHIInput::wid( const QString &key ) const
{
    if ( key==QLatin1String( "lineedit" ) ) return PHILineEditItem::LineEdit;
    if ( key==QLatin1String( "textarea" ) ) return PHITextAreaItem::TextArea;
    if ( key==QLatin1String( "password" ) ) return PHIPasswordItem::Password;
    if ( key==QLatin1String( "checkbox" ) ) return PHICheckBoxItem::Checkbox;
    if ( key==QLatin1String( "radiobutton" ) ) return PHIRadioButtonItem::RadioButton;
    if ( key==QLatin1String( "button" ) ) return PHIButtonItem::Button;
    if ( key==QLatin1String( "submit" ) ) return PHISubmitButtonItem::SubmitButton;
    if ( key==QLatin1String( "number" ) ) return PHINumberEditItem::NumberEdit;
    if ( key==QLatin1String( "realnumber" ) ) return PHIRealNumberEditItem::RealNumberEdit;
    if ( key==QLatin1String( "reset" ) ) return PHIResetButtonItem::ResetButton;
    return 0;
}

#endif // PHIINPUT_H
