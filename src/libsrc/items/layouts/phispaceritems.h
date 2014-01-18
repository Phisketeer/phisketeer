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
#ifndef PHISPACERITEMS_H
#define PHISPACERITEMS_H
#include "phibaseitem.h"

class PHIHSpacerItem : public PHIBaseItem
{
    Q_OBJECT

public:
    enum Wid { HorizontalSpacer=26 };
    explicit PHIHSpacerItem( const PHIBaseItemPrivate &p ) : PHIBaseItem( p ) { if ( isIdeItem() ) initWidget(); }
      PHIHSpacerItem( const PHIHSpacerItem &it ) : PHIBaseItem( it ) { if ( isIdeItem() ) initWidget(); }
    virtual ~PHIHSpacerItem() {}

    inline virtual PHIWID wid() const { return HorizontalSpacer; }
    inline virtual QString listName() const { return tr( "HSpacer" ); }
    inline virtual QPixmap pixmap() const { return QPixmap( L1( ":/items/hspacer" ) ); }
    inline virtual QString description() const { return tr( "Horizontal spacer item for layouts." ); }
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;

protected:
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual bool paint( QPainter *painter, const QRectF &exposed );

private:
    void initWidget();
};

class PHIVSpacerItem : public PHIBaseItem
{
    Q_OBJECT

public:
    enum Wid { VerticalSpacer=27 };
    explicit PHIVSpacerItem( const PHIBaseItemPrivate &p ) : PHIBaseItem( p ) { if ( isIdeItem() ) initWidget(); }
      PHIVSpacerItem( const PHIVSpacerItem &it ) : PHIBaseItem( it ) { if ( isIdeItem() ) initWidget(); }
    virtual ~PHIVSpacerItem() {}

    inline virtual PHIWID wid() const { return VerticalSpacer; }
    inline virtual QString listName() const { return tr( "VSpacer" ); }
    inline virtual QPixmap pixmap() const { return QPixmap( L1( ":/items/vspacer" ) ); }
    inline virtual QString description() const { return tr( "Vertical spacer item for layouts." ); }
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;

protected:
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual bool paint( QPainter *painter, const QRectF &exposed );

private:
    void initWidget();
};

#endif // PHISPACERITEMS_H
