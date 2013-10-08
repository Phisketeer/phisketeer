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
#ifndef PHIIMAGEITEMS_H
#define PHIIMAGEITEMS_H
#include "phiabstractitems.h"

class PHIPhisysItem : public PHIBaseItem
{
    Q_OBJECT

public:
    enum Wid { Phisys=48 };
    explicit PHIPhisysItem( const PHIBaseItemPrivate &p ) : PHIBaseItem( p ), _image( L1( ":/sponsor" ) ) {}
    PHIPhisysItem( const PHIPhisysItem &it ) : PHIBaseItem( it ), _image( it._image ) {}
    virtual ~PHIPhisysItem() {}

    virtual QString listName() const { return tr( "Phisys" ); }
    virtual QString description() const { return tr( "Displays the 'powered by' logo." ); }
    virtual PHIWID wid() const { return Phisys; }
    virtual QPixmap pixmap() const { return QPixmap(); }
    inline virtual bool isHeightChangeable() const { return false; }
    inline virtual bool isWidthChangeable() const { return false; }
    inline virtual bool isPrivateItem() const { return true; }

protected:
    virtual void paint( QPainter *painter, const QRectF &exposed );
    virtual void squeeze();
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;

private:
    QImage _image;
};

#endif // PHIIMAGEITEMS_H
