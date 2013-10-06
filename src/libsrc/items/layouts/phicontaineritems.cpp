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
#include "phicontaineritems.h"
#include "phiitemfactory.h"
#include "phiabstractitems.h"
#include "phibasepage.h"
#include "phidatasources.h"

PHILoginContainerItem::PHILoginContainerItem( const PHIBaseItemPrivate &p )
    : PHIFormLayoutItem( p )
{
}

void PHILoginContainerItem::initIDE()
{
    PHIBaseItem *it;
    PHIRectHash rects;
    PHIItemFactory* f=PHIItemFactory::instance();

    it=f->item( 30, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "loginlabel" ) ) );
    it->initIDE();
    it->setProperty( "align", static_cast<quint16>(Qt::AlignCenter) );
    it->textData()->setSource( PHIData::Translated );
    it->textData()->setText( QT_TR_NOOP( L1( "Login" ) ) );
    it->textData()->setText( L1( "Login" ), "en" );
    it->textData()->setText( L1( "Login" ), "de" );
    insertBaseItem( it, 0, 0, 1, 2 );
    rects.insert( it->id(), QRect( 0, 0, 2, 1 ) );

    it=f->item( 30, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "emaillabel" ) ) );
    it->initIDE();
    it->textData()->setSource( PHIData::Translated );
    it->textData()->setText( QT_TR_NOOP( L1( "Email" ) ) );
    it->textData()->setText( L1( "Email" ), "en" );
    it->textData()->setText( L1( "E-Mail" ), "de" );
    insertBaseItem( it, 1, 0 );
    rects.insert( it->id(), QRect( 0, 1, 1, 1 ) );

    it=f->item( 51, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "phiemail" ) ) );
    it->initIDE();
    it->textData()->setOption( PHIData::Parse );
    it->textData()->setText( L1( "$PHIREQUEST[$PHIID]" ) );
    insertBaseItem( it, 1, 1 );
    rects.insert( it->id(), QRect( 1, 1, 1, 1 ) );

    it=f->item( 30, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "passwordlabel" ) ) );
    it->initIDE();
    it->textData()->setSource( PHIData::Translated );
    it->textData()->setText( QT_TR_NOOP( L1( "Password" ) ) );
    it->textData()->setText( L1( "Password" ), "en" );
    it->textData()->setText( L1( "Passwort" ), "de" );
    insertBaseItem( it, 2, 0 );
    rects.insert( it->id(), QRect( 0, 2, 1, 1 ) );

    it=f->item( 3, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "phipasswd" ) ) );
    it->initIDE();
    insertBaseItem( it, 2, 1 );
    rects.insert( it->id(), QRect( 1, 2, 1, 1 ) );

    setChildRects( rects );
    foreach ( it, childItems() ) it->updateData();
    resize( layout()->preferredSize() );
}

PHIRegisterUserContainerItem::PHIRegisterUserContainerItem( const PHIBaseItemPrivate &p )
    : PHILoginContainerItem( p )
{
}

void PHIRegisterUserContainerItem::initIDE()
{
    PHIBaseItem *it;
    PHIRectHash rects;
    PHIItemFactory* f=PHIItemFactory::instance();

    it=f->item( 30, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "registerlabel" ) ) );
    it->initIDE();
    it->setProperty( "align", static_cast<quint16>(Qt::AlignCenter) );
    it->textData()->setSource( PHIData::Translated );
    it->textData()->setText( QT_TR_NOOP( L1( "Register user" ) ) );
    it->textData()->setText( L1( "Register user" ), "en" );
    it->textData()->setText( L1( "Benutzerregistrierung" ), "de" );
    insertBaseItem( it, 0, 0, 1, 2 );
    rects.insert( it->id(), QRect( 0, 0, 2, 1 ) );

    it=f->item( 30, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "namelabel" ) ) );
    it->initIDE();
    it->textData()->setSource( PHIData::Translated );
    it->textData()->setText( QT_TR_NOOP( L1( "Name" ) ) );
    it->textData()->setText( L1( "Name" ), "en" );
    it->textData()->setText( L1( "Name" ), "de" );
    insertBaseItem( it, 1, 0 );
    rects.insert( it->id(), QRect( 0, 1, 1, 1 ) );

    it=f->item( 1, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "phiname" ) ) );
    it->initIDE();
    it->textData()->setOption( PHIData::Parse );
    it->textData()->setText( L1( "$PHIREQUEST[$PHIID]" ) );
    insertBaseItem( it, 1, 1 );
    rects.insert( it->id(), QRect( 1, 1, 1, 1 ) );

    it=f->item( 30, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "emaillabel" ) ) );
    it->initIDE();
    it->textData()->setSource( PHIData::Translated );
    it->textData()->setText( QT_TR_NOOP( L1( "Email" ) ) );
    it->textData()->setText( L1( "Email" ), "en" );
    it->textData()->setText( L1( "E-Mail" ), "de" );
    insertBaseItem( it, 2, 0 );
    rects.insert( it->id(), QRect( 0, 2, 1, 1 ) );

    it=f->item( 51, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "phiemail" ) ) );
    it->initIDE();
    it->textData()->setOption( PHIData::Parse );
    it->textData()->setText( L1( "$PHIREQUEST[$PHIID]" ) );
    insertBaseItem( it, 2, 1 );
    rects.insert( it->id(), QRect( 1, 2, 1, 1 ) );

    it=f->item( 30, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "passwordlabel" ) ) );
    it->initIDE();
    it->textData()->setSource( PHIData::Translated );
    it->textData()->setText( QT_TR_NOOP( L1( "Password" ) ) );
    it->textData()->setText( L1( "Password" ), "en" );
    it->textData()->setText( L1( "Passwort" ), "de" );
    insertBaseItem( it, 3, 0 );
    rects.insert( it->id(), QRect( 0, 3, 1, 1 ) );

    it=f->item( 3, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "phipasswd" ) ) );
    it->initIDE();
    insertBaseItem( it, 3, 1 );
    rects.insert( it->id(), QRect( 1, 3, 1, 1 ) );

    it=f->item( 30, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "verifylabel" ) ) );
    it->initIDE();
    it->textData()->setSource( PHIData::Translated );
    it->textData()->setText( QT_TR_NOOP( L1( "Verify password" ) ) );
    it->textData()->setText( L1( "Verify password" ), "en" );
    it->textData()->setText( L1( "Verifizierung" ), "de" );
    insertBaseItem( it, 4, 0 );
    rects.insert( it->id(), QRect( 0, 4, 1, 1 ) );

    it=f->item( 3, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "phiverify" ) ) );
    it->initIDE();
    insertBaseItem( it, 4, 1 );
    rects.insert( it->id(), QRect( 1, 4, 1, 1 ) );

    setChildRects( rects );
    foreach ( it, childItems() ) it->updateData();
    resize( layout()->preferredSize() );
}

PHIAddressContainerItem::PHIAddressContainerItem( const PHIBaseItemPrivate &p )
    : PHILoginContainerItem( p )
{
}

void PHIAddressContainerItem::initIDE()
{
    PHIBaseItem *it;
    PHIRectHash rects;
    PHIItemFactory* f=PHIItemFactory::instance();

    it=f->item( 30, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "addresslabel" ) ) );
    it->initIDE();
    it->setProperty( "align", static_cast<quint16>(Qt::AlignCenter) );
    it->textData()->setSource( PHIData::Translated );
    it->textData()->setText( QT_TR_NOOP( L1( "Delivery address" ) ) );
    it->textData()->setText( L1( "Delivery address" ), "en" );
    it->textData()->setText( L1( "Lieferadresse" ), "de" );
    insertBaseItem( it, 0, 0, 1, 2 );
    rects.insert( it->id(), QRect( 0, 0, 2, 1 ) );

    it=f->item( 30, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "titlelabel" ) ) );
    it->initIDE();
    it->textData()->setSource( PHIData::Translated );
    it->textData()->setText( QT_TR_NOOP( L1( "Title" ) ) );
    it->textData()->setText( L1( "Title" ), "en" );
    it->textData()->setText( L1( "Titel" ), "de" );
    insertBaseItem( it, 1, 0 );
    rects.insert( it->id(), QRect( 0, 1, 1, 1 ) );

    it=f->item( 1, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "phititle" ) ) );
    it->initIDE();
    it->textData()->setOption( PHIData::Parse );
    it->textData()->setText( L1( "$PHIREQUEST[$PHIID]" ) );
    insertBaseItem( it, 1, 1 );
    rects.insert( it->id(), QRect( 1, 1, 1, 1 ) );

    it=f->item( 30, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "firstnamelabel" ) ) );
    it->initIDE();
    it->textData()->setSource( PHIData::Translated );
    it->textData()->setText( QT_TR_NOOP( L1( "First name" ) ) );
    it->textData()->setText( L1( "First name" ), "en" );
    it->textData()->setText( L1( "Vorname" ), "de" );
    insertBaseItem( it, 2, 0 );
    rects.insert( it->id(), QRect( 0, 2, 1, 1 ) );

    it=f->item( 1, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "phifirstname" ) ) );
    it->initIDE();
    it->textData()->setOption( PHIData::Parse );
    it->textData()->setText( L1( "$PHIREQUEST[$PHIID]" ) );
    insertBaseItem( it, 2, 1 );
    rects.insert( it->id(), QRect( 1, 2, 1, 1 ) );

    it=f->item( 30, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "lastnamelabel" ) ) );
    it->initIDE();
    it->textData()->setSource( PHIData::Translated );
    it->textData()->setText( QT_TR_NOOP( L1( "Last name" ) ) );
    it->textData()->setText( L1( "Last name" ), "en" );
    it->textData()->setText( L1( "Nachname" ), "de" );
    insertBaseItem( it, 3, 0 );
    rects.insert( it->id(), QRect( 0, 3, 1, 1 ) );

    it=f->item( 1, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "philastname" ) ) );
    it->initIDE();
    it->textData()->setOption( PHIData::Parse );
    it->textData()->setText( L1( "$PHIREQUEST[$PHIID]" ) );
    insertBaseItem( it, 3, 1 );
    rects.insert( it->id(), QRect( 1, 3, 1, 1 ) );

    it=f->item( 30, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "organisationlabel" ) ) );
    it->initIDE();
    it->textData()->setSource( PHIData::Translated );
    it->textData()->setText( QT_TR_NOOP( L1( "Organisation" ) ) );
    it->textData()->setText( L1( "Organisation" ), "en" );
    it->textData()->setText( L1( "Organisation" ), "de" );
    insertBaseItem( it, 4, 0 );
    rects.insert( it->id(), QRect( 0, 4, 1, 1 ) );

    it=f->item( 1, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "phiorg" ) ) );
    it->initIDE();
    it->textData()->setOption( PHIData::Parse );
    it->textData()->setText( L1( "$PHIREQUEST[$PHIID]" ) );
    insertBaseItem( it, 4, 1 );
    rects.insert( it->id(), QRect( 1, 4, 1, 1 ) );

    it=f->item( 30, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "unitlabel" ) ) );
    it->initIDE();
    it->textData()->setSource( PHIData::Translated );
    it->textData()->setText( QT_TR_NOOP( L1( "Unit" ) ) );
    it->textData()->setText( L1( "Unit" ), "en" );
    it->textData()->setText( L1( "Abteilung" ), "de" );
    insertBaseItem( it, 5, 0 );
    rects.insert( it->id(), QRect( 0, 5, 1, 1 ) );

    it=f->item( 1, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "phiunit" ) ) );
    it->initIDE();
    it->textData()->setOption( PHIData::Parse );
    it->textData()->setText( L1( "$PHIREQUEST[$PHIID]" ) );
    insertBaseItem( it, 5, 1 );
    rects.insert( it->id(), QRect( 1, 5, 1, 1 ) );

    it=f->item( 30, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "streetlabel" ) ) );
    it->initIDE();
    it->textData()->setSource( PHIData::Translated );
    it->textData()->setText( QT_TR_NOOP( L1( "Street" ) ) );
    it->textData()->setText( L1( "Street" ), "en" );
    it->textData()->setText( L1( "Strasse" ), "de" );
    insertBaseItem( it, 6, 0 );
    rects.insert( it->id(), QRect( 0, 6, 1, 1 ) );

    it=f->item( 1, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "phistreet" ) ) );
    it->initIDE();
    it->textData()->setOption( PHIData::Parse );
    it->textData()->setText( L1( "$PHIREQUEST[$PHIID]" ) );
    insertBaseItem( it, 6, 1 );
    rects.insert( it->id(), QRect( 1, 6, 1, 1 ) );

    it=f->item( 30, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "statelabel" ) ) );
    it->initIDE();
    it->textData()->setSource( PHIData::Translated );
    it->textData()->setText( QT_TR_NOOP( L1( "State" ) ) );
    it->textData()->setText( L1( "State" ), "en" );
    it->textData()->setText( L1( "Bundesland" ), "de" );
    insertBaseItem( it, 7, 0 );
    rects.insert( it->id(), QRect( 0, 7, 1, 1 ) );

    it=f->item( 1, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "phistate" ) ) );
    it->initIDE();
    it->textData()->setOption( PHIData::Parse );
    it->textData()->setText( L1( "$PHIREQUEST[$PHIID]" ) );
    insertBaseItem( it, 7, 1 );
    rects.insert( it->id(), QRect( 1, 7, 1, 1 ) );

    it=f->item( 30, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "citylabel" ) ) );
    it->initIDE();
    it->textData()->setSource( PHIData::Translated );
    it->textData()->setText( QT_TR_NOOP( L1( "City" ) ) );
    it->textData()->setText( L1( "City" ), "en" );
    it->textData()->setText( L1( "Stadt" ), "de" );
    insertBaseItem( it, 9, 0 );
    rects.insert( it->id(), QRect( 0, 9, 1, 1 ) );

    it=f->item( 1, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "phicity" ) ) );
    it->initIDE();
    it->textData()->setOption( PHIData::Parse );
    it->textData()->setText( L1( "$PHIREQUEST[$PHIID]" ) );
    insertBaseItem( it, 9, 1 );
    rects.insert( it->id(), QRect( 1, 9, 1, 1 ) );

    it=f->item( 30, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "ziplabel" ) ) );
    it->initIDE();
    it->textData()->setSource( PHIData::Translated );
    it->textData()->setText( QT_TR_NOOP( L1( "Zip code" ) ) );
    it->textData()->setText( L1( "Zip code" ), "en" );
    it->textData()->setText( L1( "PLZ" ), "de" );
    insertBaseItem( it, 10, 0 );
    rects.insert( it->id(), QRect( 0, 10, 1, 1 ) );

    it=f->item( 1, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "phizip" ) ) );
    it->initIDE();
    it->textData()->setOption( PHIData::Parse );
    it->textData()->setText( L1( "$PHIREQUEST[$PHIID]" ) );
    insertBaseItem( it, 10, 1 );
    rects.insert( it->id(), QRect( 1, 10, 1, 1 ) );

    it=f->item( 30, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "countrylabel" ) ) );
    it->initIDE();
    it->textData()->setSource( PHIData::Translated );
    it->textData()->setText( QT_TR_NOOP( L1( "Country" ) ) );
    it->textData()->setText( L1( "Country" ), "en" );
    it->textData()->setText( L1( "Land" ), "de" );
    insertBaseItem( it, 11, 0 );
    rects.insert( it->id(), QRect( 0, 11, 1, 1 ) );

    it=f->item( 47, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "phicountry" ) ) );
    it->initIDE();
    it->textData()->setOption( PHIData::Parse );
    it->textData()->setText( L1( "$PHIREQUEST[$PHIID]" ) );
    insertBaseItem( it, 11, 1 );
    rects.insert( it->id(), QRect( 1, 11, 1, 1 ) );

    setChildRects( rects );
    foreach ( it, childItems() ) it->updateData();
    resize( layout()->preferredSize() );
}

PHIContactContainerItem::PHIContactContainerItem( const PHIBaseItemPrivate &p )
    : PHILoginContainerItem( p )
{
}

void PHIContactContainerItem::initIDE()
{
    PHIBaseItem *it;
    PHIRectHash rects;
    PHIItemFactory* f=PHIItemFactory::instance();

    it=f->item( 30, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "contactlabel" ) ) );
    it->initIDE();
    it->setProperty( "align", static_cast<quint16>(Qt::AlignCenter) );
    it->textData()->setSource( PHIData::Translated );
    it->textData()->setText( QT_TR_NOOP( L1( "Contact information" ) ) );
    it->textData()->setText( L1( "Contact information" ), "en" );
    it->textData()->setText( L1( "Kontaktinformation" ), "de" );
    insertBaseItem( it, 0, 0, 1, 2 );
    rects.insert( it->id(), QRect( 0, 0, 2, 1 ) );

    it=f->item( 30, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "namelabel" ) ) );
    it->initIDE();
    it->textData()->setSource( PHIData::Translated );
    it->textData()->setText( QT_TR_NOOP( L1( "Name" ) ) );
    it->textData()->setText( L1( "Name" ), "en" );
    it->textData()->setText( L1( "Name" ), "de" );
    insertBaseItem( it, 1, 0 );
    rects.insert( it->id(), QRect( 0, 1, 1, 1 ) );

    it=f->item( 1, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "phiname" ) ) );
    it->initIDE();
    it->textData()->setOption( PHIData::Parse );
    it->textData()->setText( L1( "$PHIREQUEST[$PHIID]" ) );
    insertBaseItem( it, 1, 1 );
    rects.insert( it->id(), QRect( 1, 1, 1, 1 ) );

    it=f->item( 30, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "emaillabel" ) ) );
    it->initIDE();
    it->textData()->setSource( PHIData::Translated );
    it->textData()->setText( QT_TR_NOOP( L1( "Email" ) ) );
    it->textData()->setText( L1( "Email" ), "en" );
    it->textData()->setText( L1( "E-Mail" ), "de" );
    insertBaseItem( it, 2, 0 );
    rects.insert( it->id(), QRect( 0, 2, 1, 1 ) );

    it=f->item( 51, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "phiemail" ) ) );
    it->initIDE();
    it->textData()->setOption( PHIData::Parse );
    it->textData()->setText( L1( "$PHIREQUEST[$PHIID]" ) );
    insertBaseItem( it, 2, 1 );
    rects.insert( it->id(), QRect( 1, 2, 1, 1 ) );

    it=f->item( 30, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "mobilelabel" ) ) );
    it->initIDE();
    it->textData()->setSource( PHIData::Translated );
    it->textData()->setText( QT_TR_NOOP( L1( "Mobile number" ) ) );
    it->textData()->setText( L1( "Mobile number" ), "en" );
    it->textData()->setText( L1( "Mobilnummer" ), "de" );
    insertBaseItem( it, 3, 0 );
    rects.insert( it->id(), QRect( 0, 3, 1, 1 ) );

    it=f->item( 54, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "phimobile" ) ) );
    it->initIDE();
    it->textData()->setOption( PHIData::Parse );
    it->textData()->setText( L1( "$PHIREQUEST[$PHIID]" ) );
    insertBaseItem( it, 3, 1 );
    rects.insert( it->id(), QRect( 1, 3, 1, 1 ) );

    it=f->item( 30, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "phonelabel" ) ) );
    it->initIDE();
    it->textData()->setSource( PHIData::Translated );
    it->textData()->setText( QT_TR_NOOP( L1( "Phone number" ) ) );
    it->textData()->setText( L1( "Phone number" ), "en" );
    it->textData()->setText( L1( "Telefonnummer" ), "de" );
    insertBaseItem( it, 4, 0 );
    rects.insert( it->id(), QRect( 0, 4, 1, 1 ) );

    it=f->item( 54, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "phiphone" ) ) );
    it->initIDE();
    it->textData()->setOption( PHIData::Parse );
    it->textData()->setText( L1( "$PHIREQUEST[$PHIID]" ) );
    insertBaseItem( it, 4, 1 );
    rects.insert( it->id(), QRect( 1, 4, 1, 1 ) );

    it=f->item( 30, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "faxlabel" ) ) );
    it->initIDE();
    it->textData()->setSource( PHIData::Translated );
    it->textData()->setText( QT_TR_NOOP( L1( "Fax" ) ) );
    it->textData()->setText( L1( "Fax" ), "en" );
    it->textData()->setText( L1( "Fax" ), "de" );
    insertBaseItem( it, 5, 0 );
    rects.insert( it->id(), QRect( 0, 5, 1, 1 ) );

    it=f->item( 54, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "phifax" ) ) );
    it->initIDE();
    it->textData()->setOption( PHIData::Parse );
    it->textData()->setText( L1( "$PHIREQUEST[$PHIID]" ) );
    insertBaseItem( it, 5, 1 );
    rects.insert( it->id(), QRect( 1, 5, 1, 1 ) );

    it=f->item( 30, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "urllabel" ) ) );
    it->initIDE();
    it->textData()->setSource( PHIData::Translated );
    it->textData()->setText( QT_TR_NOOP( L1( "URL" ) ) );
    it->textData()->setText( L1( "URL" ), "en" );
    it->textData()->setText( L1( "URL" ), "de" );
    insertBaseItem( it, 6, 0 );
    rects.insert( it->id(), QRect( 0, 6, 1, 1 ) );

    it=f->item( 1, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "phiurl" ) ) );
    it->initIDE();
    it->textData()->setOption( PHIData::Parse );
    it->textData()->setText( L1( "$PHIREQUEST[$PHIID]" ) );
    insertBaseItem( it, 6, 1 );
    rects.insert( it->id(), QRect( 1, 6, 1, 1 ) );

    setChildRects( rects );
    foreach ( it, childItems() ) it->updateData();
    resize( layout()->preferredSize() );
}

PHICCContainerItem::PHICCContainerItem( const PHIBaseItemPrivate &p )
    : PHIGridLayoutItem( p )
{
}

void PHICCContainerItem::initIDE()
{
    PHIBaseItem *it;
    PHIRectHash rects;
    PHIItemFactory* f=PHIItemFactory::instance();

    it=f->item( 30, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "cardlabel" ) ) );
    it->initIDE();
    it->setProperty( "align", static_cast<quint16>(Qt::AlignCenter) );
    it->textData()->setSource( PHIData::Translated );
    it->textData()->setText( QT_TR_NOOP( L1( "Credit card" ) ) );
    it->textData()->setText( L1( "Credit card" ), "en" );
    it->textData()->setText( L1( "Kreditkarte" ), "de" );
    insertBaseItem( it, 0, 0, 1, 3 );
    rects.insert( it->id(), QRect( 0, 0, 3, 1 ) );

    it=f->item( 30, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "cardtypelabel" ) ) );
    it->initIDE();
    it->textData()->setSource( PHIData::Translated );
    it->textData()->setText( QT_TR_NOOP( L1( "Type" ) ) );
    it->textData()->setText( L1( "Type" ), "en" );
    it->textData()->setText( L1( "Typ" ), "de" );
    insertBaseItem( it, 1, 0 );
    rects.insert( it->id(), QRect( 0, 1, 1, 1 ) );

    it=f->item( 10, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "phicardtype" ) ) );
    it->initIDE();
    it->textData()->setOption( PHIData::Parse );
    it->textData()->setText( L1( "Mastercard[M]\nVISA[V]\nAmerican Express[A]\nDiners Club[D]" ) );
    insertBaseItem( it, 1, 1 );
    rects.insert( it->id(), QRect( 1, 1, 1, 1 ) );

    it=f->item( 30, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "cardnamelabel" ) ) );
    it->initIDE();
    it->textData()->setSource( PHIData::Translated );
    it->textData()->setText( QT_TR_NOOP( L1( "Name" ) ) );
    it->textData()->setText( L1( "Name" ), "en" );
    it->textData()->setText( L1( "Name" ), "de" );
    insertBaseItem( it, 2, 0 );
    rects.insert( it->id(), QRect( 0, 2, 1, 1 ) );

    it=f->item( 1, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "phicardname" ) ) );
    it->initIDE();
    it->textData()->setOption( PHIData::Parse );
    it->textData()->setText( L1( "$PHIREQUEST[$PHIID]" ) );
    insertBaseItem( it, 2, 1, 1, 2 );
    rects.insert( it->id(), QRect( 1, 2, 2, 1 ) );

    it=f->item( 30, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "cardnumberlabel" ) ) );
    it->initIDE();
    it->textData()->setSource( PHIData::Translated );
    it->textData()->setText( QT_TR_NOOP( L1( "Number" ) ) );
    it->textData()->setText( L1( "Number" ), "en" );
    it->textData()->setText( L1( "Nummer" ), "de" );
    insertBaseItem( it, 3, 0 );
    rects.insert( it->id(), QRect( 0, 3, 1, 1 ) );

    it=f->item( 1, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "phicardnumber" ) ) );
    it->initIDE();
    it->textData()->setOption( PHIData::Parse );
    it->textData()->setText( L1( "$PHIREQUEST[$PHIID]" ) );
    insertBaseItem( it, 3, 1, 1, 2 );
    rects.insert( it->id(), QRect( 1, 3, 2, 1 ) );

    it=f->item( 30, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "cardvalidlabel" ) ) );
    it->initIDE();
    it->textData()->setSource( PHIData::Translated );
    it->textData()->setText( QT_TR_NOOP( L1( "Valid thru" ) ) );
    it->textData()->setText( L1( "Valid thru" ), "en" );
    it->textData()->setText( QString::fromUtf8( "Gültig bis" ), "de" );
    insertBaseItem( it, 4, 0 );
    rects.insert( it->id(), QRect( 0, 4, 1, 1 ) );

    it=f->item( 10, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "phicardmonth" ) ) );
    it->initIDE();
    it->textData()->setOption( PHIData::Parse );
    it->textData()->setText( L1( "$PHISERVER[monthlist]" ) );
    insertBaseItem( it, 4, 1 );
    rects.insert( it->id(), QRect( 1, 4, 1, 1 ) );

    it=f->item( 10, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "phicardyear" ) ) );
    it->initIDE();
    it->textData()->setOption( PHIData::Parse );
    it->textData()->setText( L1( "$PHISERVER[yearlist][20]" ) );
    insertBaseItem( it, 4, 2 );
    rects.insert( it->id(), QRect( 2, 4, 1, 1 ) );

    it=f->item( 30, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "cardcivlabel" ) ) );
    it->initIDE();
    it->textData()->setSource( PHIData::Translated );
    it->textData()->setText( QT_TR_NOOP( L1( "CIV" ) ) );
    it->textData()->setText( L1( "CIV" ), "en" );
    it->textData()->setText( L1( "CIV" ), "de" );
    insertBaseItem( it, 5, 0 );
    rects.insert( it->id(), QRect( 0, 5, 1, 1 ) );

    it=f->item( 1, PHIBaseItemPrivate( page() ) );
    it->setId( page()->nextFreeId( L1( "phicardciv" ) ) );
    it->initIDE();
    it->textData()->setOption( PHIData::Parse );
    it->textData()->setText( L1( "$PHIREQUEST[$PHIID]" ) );
    insertBaseItem( it, 5, 1 );
    rects.insert( it->id(), QRect( 1, 5, 1, 1 ) );

    setChildRects( rects );
    foreach ( it, childItems() ) it->updateData();
    resize( layout()->preferredSize() );
}
