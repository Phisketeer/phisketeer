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
#ifndef PHIHTMLITEMS_H
#define PHIHTMLITEMS_H
#include "phiabstractitems.h"

class PHIHtmlDocItem : public PHIAbstractExternalItem
{
    Q_OBJECT

public:
    enum Wid { HtmlDoc=49 };
    explicit PHIHtmlDocItem( const PHIBaseItemPrivate &p ) : PHIAbstractExternalItem( p ) {}
    PHIHtmlDocItem( const PHIHtmlDocItem &it ) : PHIAbstractExternalItem( it ) {}
    virtual ~PHIHtmlDocItem() {}

    virtual QString listName() const { return tr( "Html" ); }
    virtual QString description() const { return tr( "Embeds an external HTML page." ); }
    virtual PHIWID wid() const { return HtmlDoc; }
    virtual QPixmap pixmap() const { return QPixmap( L1( ":/items/richtext" ) ); }

protected:
    virtual void ideInit();
    virtual QByteArray source() const { return QByteArray(); }
};

class PHITwitterItem : public PHIAbstractExternalItem
{
    Q_OBJECT

public:
    enum Wid { Twitter=58 };
    enum ItemData { DTmpText=1 };
    explicit PHITwitterItem( const PHIBaseItemPrivate &p ) : PHIAbstractExternalItem( p ) {}
    PHITwitterItem( const PHITwitterItem &it ) : PHIAbstractExternalItem( it ) {}
    virtual ~PHITwitterItem() {}

    virtual QString listName() const { return tr( "Twitter" ); }
    virtual QString description() const { return tr( "Embeds a Twitter button." ); }
    virtual PHIWID wid() const { return Twitter; }
    virtual QPixmap pixmap() const { return QPixmap( L1( ":/items/button" ) ); }

    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;
    virtual bool hasHtmlExtension() const { return true; }
    virtual PHIWID htmlHeaderExtension( const PHIRequest *req, QByteArray &headerOut ) const;

protected:
    virtual void ideInit();
    virtual void squeeze();
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual QByteArray source() const { return QByteArray(); }
    virtual void setWidgetText( const QString &t );
};

class PHIFacebookItem : public PHIAbstractExternalItem
{
    Q_OBJECT

public:
    enum Wid { Facebook=55 };
    enum ItemData { DTmpText=1 };
    explicit PHIFacebookItem( const PHIBaseItemPrivate &p ) : PHIAbstractExternalItem( p ) {}
    PHIFacebookItem( const PHIFacebookItem &it ) : PHIAbstractExternalItem( it ) {}
    virtual ~PHIFacebookItem() {}

    virtual QString listName() const { return tr( "Facebook" ); }
    virtual QString description() const { return tr( "Embeds a Facebook 'like' button." ); }
    virtual PHIWID wid() const { return Facebook; }
    virtual QPixmap pixmap() const { return QPixmap( L1( ":/items/checkbox" ) ); }

protected:
    virtual void ideInit();
    virtual void squeeze();
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual QByteArray source() const;
    virtual void setWidgetText( const QString &t );
    virtual void phisCreateData( const PHIDataParser &parser );
    virtual void phisParseData( const PHIDataParser &parser );
    virtual QByteArray mapLanguage( const QByteArray &lang ) const;
};

#endif // PHIHTMLITEMS_H
