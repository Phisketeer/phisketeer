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
#ifndef PHIAADDRESSDLG_H
#define PHIAADDRESSDLG_H

#include <QDialog>
#include "phiaconfig.h"
#include "ui_addressdlg.h"
#include "ui_contactdlg.h"
#include "ui_creditcarddlg.h"

class PHIAAddressDlg : public QDialog, private Ui::AddressDlg
{
    Q_OBJECT

public:
    PHIAAddressDlg( QWidget *parent );
    virtual ~PHIAAddressDlg();

    void setAddress( const PHIAAddress& );
    PHIAAddress address() const;
};

class PHIACreditCardDlg : public QDialog, private Ui::CreditCardDlg
{
    Q_OBJECT

public:
    PHIACreditCardDlg( QWidget *parent );
    virtual ~PHIACreditCardDlg();

    void setCreditCard( const PHIACreditCard& );
    PHIACreditCard creditCard() const;
};

class PHIAContactDlg : public QDialog, private Ui::ContactDlg
{
    Q_OBJECT

public:
    PHIAContactDlg( QWidget *parent );
    virtual ~PHIAContactDlg();

    void setContact( const PHIAContact& );
    PHIAContact contact() const;
};

#endif // PHIAADDRESSDLG_H
