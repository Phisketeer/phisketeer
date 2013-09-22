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
#ifndef PHIABSTRACTINPUTITEM_H
#define PHIABSTRACTINPUTITEM_H
#include "phiabstractitems.h"

class PHIAbstractInputItem : public PHIAbstractTextItem
{
    Q_OBJECT
    Q_PROPERTY( QString accessKey WRITE setAccessKey READ accessKey )

public:
    enum ItemData { DAccessKey=-99 };
    explicit PHIAbstractInputItem();
    virtual bool isFocusable() const { return true; }

protected:
    virtual void squeeze();
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual void updateData();

public slots:
    inline QString accessKey() const { return QString::fromUtf8( data( DAccessKey ).toByteArray() ); }
    inline virtual void setAccessKey( const QString &s ) { setData( DAccessKey, s.left(1).toUtf8() ); updateData(); }
};

#endif // PHIABSTRACTINPUTITEM_H
