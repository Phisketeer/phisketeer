/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2014  Phisys AG, Switzerland
#    Copyright (C) 2012-2014  Phisketeer.org team
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
#ifndef PHIGOOGLEITEMS_H
#define PHIGOOGLEITEMS_H
#include "phiabstractitems.h"

class PHIGoogleStaticMapItem : public PHIAbstractExternalItem
{
    Q_OBJECT

public:
    enum Wid { GoogleStaticMap=56 };
    explicit PHIGoogleStaticMapItem( const PHIBaseItemPrivate &p ) : PHIAbstractExternalItem( p ) {}
    PHIGoogleStaticMapItem( const PHIGoogleStaticMapItem &it ) : PHIAbstractExternalItem( it ) {}
    virtual ~PHIGoogleStaticMapItem() {}

    virtual QString listName() const { return tr( "Static map" ); }
    virtual QString description() const { return tr( "Displays a Google static map image." ); }
    virtual PHIWID wid() const { return GoogleStaticMap; }
    virtual QPixmap pixmap() const { return QPixmap( L1( ":/items/reset" ) ); }
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;

protected:
    virtual void ideInit();
    virtual void setWidgetText( const QString &t );
    virtual void phisCreateData( const PHIDataParser &parser );
    virtual void phisParseData( const PHIDataParser &parser );
    virtual QByteArray source() const;
};

class PHIGooglePlusItem : public PHIAbstractExternalItem
{
    Q_OBJECT

public:
    enum Wid { GooglePlus=57 };
    enum ItemData { DTmpUrl=1 };
    explicit PHIGooglePlusItem( const PHIBaseItemPrivate &p ) : PHIAbstractExternalItem( p ) {}
    PHIGooglePlusItem( const PHIGooglePlusItem &it ) : PHIAbstractExternalItem( it ) {}
    virtual ~PHIGooglePlusItem() {}

    virtual QString listName() const { return tr( "Google plus" ); }
    virtual QString description() const { return tr( "Displays a Google+ button." ); }
    virtual PHIWID wid() const { return GooglePlus; }
    virtual QPixmap pixmap() const { return QPixmap( L1( ":/items/radiobutton" ) ); }
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;
    virtual bool hasHtmlExtension() const { return true; }
    virtual PHIWID htmlHeaderExtension( const PHIRequest *req, QByteArray &headerOut ) const;

protected:
    virtual void ideInit();
    virtual void phisCreateData( const PHIDataParser &parser );
    virtual void phisParseData( const PHIDataParser &parser );
    virtual QByteArray source() const { return QByteArray(); }
    virtual void setWidgetText( const QString &t );
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual void squeeze();
};

class PHIYouTubeItem : public PHIAbstractExternalItem
{
    Q_OBJECT

public:
    enum Wid { YouTube=60 };
    explicit PHIYouTubeItem( const PHIBaseItemPrivate &p ) : PHIAbstractExternalItem( p ) {}
    PHIYouTubeItem( const PHIYouTubeItem &it ) : PHIAbstractExternalItem( it ) {}
    virtual ~PHIYouTubeItem() {}

    virtual QString listName() const { return tr( "YouTube" ); }
    virtual QString description() const { return tr( "Displays a YouTube video container." ); }
    virtual PHIWID wid() const { return YouTube; }
    virtual QPixmap pixmap() const { return QPixmap( L1( ":/items/image" ) ); }

protected:
    virtual void ideInit();
    virtual void phisCreateData( const PHIDataParser &parser );
    virtual void phisParseData( const PHIDataParser &parser );
    virtual QByteArray source() const;

private:
    QByteArray parse( const QByteArray &arr );
};

class PHIGoogleMapsItem : public PHIAbstractExternalItem
{
    Q_OBJECT

public:
    enum Wid { GoogleMaps=63 };
    enum ItemData { DTmpText=1 };
    explicit PHIGoogleMapsItem( const PHIBaseItemPrivate &p ) : PHIAbstractExternalItem( p ) {}
    PHIGoogleMapsItem( const PHIGoogleMapsItem &it ) : PHIAbstractExternalItem( it ) {}
    virtual ~PHIGoogleMapsItem() {}

    virtual QString listName() const { return tr( "Google Maps" ); }
    virtual QString description() const { return tr( "Embeds a dynamic Google Map." ); }
    virtual PHIWID wid() const { return GoogleMaps; }
    virtual QPixmap pixmap() const { return QPixmap( L1( ":/items/rollover" ) ); }

protected:
    virtual void ideInit();
    virtual void squeeze();
    virtual void setWidgetText( const QString &t );
    virtual void phisCreateData( const PHIDataParser &parser );
    virtual void phisParseData( const PHIDataParser &parser );
    virtual QByteArray source() const;
};

class PHIGoogleCalendarItem : public PHIAbstractExternalItem
{
    Q_OBJECT

public:
    enum Wid { GoogleCalendar=62 };
    enum ItemData { DTmpText=1 };
    explicit PHIGoogleCalendarItem( const PHIBaseItemPrivate &p ) : PHIAbstractExternalItem( p ) {}
    PHIGoogleCalendarItem( const PHIGoogleCalendarItem &it ) : PHIAbstractExternalItem( it ) {}
    virtual ~PHIGoogleCalendarItem() {}

    virtual QString listName() const { return tr( "Google Cal" ); }
    virtual QString description() const { return tr( "Embeds a Google calendar." ); }
    virtual PHIWID wid() const { return GoogleCalendar; }
    virtual QPixmap pixmap() const { return QPixmap( L1( ":/items/calendar" ) ); }
    //virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;

protected:
    virtual void ideInit();
    virtual void squeeze();
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual void setWidgetText( const QString &t );
    //virtual void phisCreateData( const PHIDataParser &parser );
    //virtual void phisParseData( const PHIDataParser &parser );
    virtual QByteArray source() const;
};

#endif // PHIGOOGLEITEMS_H
