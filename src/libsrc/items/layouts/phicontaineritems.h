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
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PHICONTAINERITEMS_H
#define PHICONTAINERITEMS_H
#include "philayoutitems.h"

class PHILoginContainerItem : public PHIGridLayoutItem
{
    Q_OBJECT

public:
    enum Wid { LoginContainer=17 };
    explicit PHILoginContainerItem( const PHIBaseItemPrivate &p ) : PHIGridLayoutItem( p ) {}
    PHILoginContainerItem( const PHILoginContainerItem &it ) : PHIGridLayoutItem( it ) {}
    virtual ~PHILoginContainerItem() {}

    inline virtual PHIWID wid() const { return LoginContainer; }
    inline virtual QString listName() const { return tr( "Login" ); }
    inline virtual QPixmap pixmap() const { return QPixmap( L1( ":/items/login" ) ); }
    inline virtual QString description() const { return tr( "Login layout container." ); }
    inline virtual bool isPrivateItem() const { return false; }
    virtual void ideInit();
};

class PHIRegisterUserContainerItem : public PHIGridLayoutItem
{
    Q_OBJECT

public:
    enum Wid { RegisterUserContainer=40 };
    explicit PHIRegisterUserContainerItem( const PHIBaseItemPrivate &p ) : PHIGridLayoutItem( p ) {}
    PHIRegisterUserContainerItem( const PHIRegisterUserContainerItem &it ) : PHIGridLayoutItem( it ) {}
    virtual ~PHIRegisterUserContainerItem() {}

    inline virtual PHIWID wid() const { return RegisterUserContainer; }
    inline virtual QString listName() const { return tr( "Register user" ); }
    inline virtual QPixmap pixmap() const { return QPixmap( L1( ":/items/user" ) ); }
    inline virtual QString description() const { return tr( "Regsister user layout container." ); }
    inline virtual bool isPrivateItem() const { return false; }
    virtual void ideInit();
};

class PHIAddressContainerItem : public PHIGridLayoutItem
{
    Q_OBJECT

public:
    enum Wid { AddressContainer=34 };
    explicit PHIAddressContainerItem( const PHIBaseItemPrivate &p ) : PHIGridLayoutItem( p ) {}
    PHIAddressContainerItem( const PHIAddressContainerItem &it ) : PHIGridLayoutItem( it ) {}
    virtual ~PHIAddressContainerItem() {}

    inline virtual PHIWID wid() const { return AddressContainer; }
    inline virtual QString listName() const { return tr( "Address" ); }
    inline virtual QPixmap pixmap() const { return QPixmap( L1( ":/items/address" ) ); }
    inline virtual QString description() const { return tr( "Address layout container." ); }
    inline virtual bool isPrivateItem() const { return false; }
    virtual void ideInit();
};

class PHIContactContainerItem : public PHIGridLayoutItem
{
    Q_OBJECT

public:
    enum Wid { ContactContainer=38 };
    explicit PHIContactContainerItem( const PHIBaseItemPrivate &p ) : PHIGridLayoutItem( p ) {}
    PHIContactContainerItem( const PHIContactContainerItem &it ) : PHIGridLayoutItem( it ) {}
    virtual ~PHIContactContainerItem() {}

    inline virtual PHIWID wid() const { return ContactContainer; }
    inline virtual QString listName() const { return tr( "Contact" ); }
    inline virtual QPixmap pixmap() const { return QPixmap( L1( ":/items/contact" ) ); }
    inline virtual QString description() const { return tr( "Contact layout container." ); }
    inline virtual bool isPrivateItem() const { return false; }
    virtual void ideInit();
};

class PHIPeriodContainerItem : public PHIGridLayoutItem
{
    Q_OBJECT

public:
    enum Wid { PeriodContainer=39 };
    explicit PHIPeriodContainerItem( const PHIBaseItemPrivate &p ) : PHIGridLayoutItem( p ) {}
    PHIPeriodContainerItem( const PHIPeriodContainerItem &it ) : PHIGridLayoutItem( it ) {}
    virtual ~PHIPeriodContainerItem() {}

    inline virtual PHIWID wid() const { return PeriodContainer; }
    inline virtual QString listName() const { return tr( "Period" ); }
    inline virtual QPixmap pixmap() const { return QPixmap( L1( ":/items/period" ) ); }
    inline virtual QString description() const { return tr( "Date period layout container." ); }
    inline virtual bool isPrivateItem() const { return false; }
    virtual void ideInit();
};

class PHICCContainerItem : public PHIGridLayoutItem
{
    Q_OBJECT

public:
    enum Wid { CreditCardContainer=36 };
    explicit PHICCContainerItem( const PHIBaseItemPrivate &p ) : PHIGridLayoutItem( p ) {}
    PHICCContainerItem( const PHICCContainerItem & it ) : PHIGridLayoutItem( it ) {}
    virtual ~PHICCContainerItem() {}

    inline virtual PHIWID wid() const { return CreditCardContainer; }
    inline virtual QString listName() const { return tr( "Credit card" ); }
    inline virtual QPixmap pixmap() const { return QPixmap( L1( ":/items/creditcard" ) ); }
    inline virtual QString description() const { return tr( "Credit card layout container." ); }
    inline virtual bool isPrivateItem() const { return false; }
    virtual void ideInit();
};

#endif // PHICONTAINERITEMS_H
