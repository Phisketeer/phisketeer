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
#ifndef PHIUPDATEINFO_H
#define PHIUPDATEINFO_H

#include <QString>
#include <QDate>
#include "phi.h"

class PHIEXPORT PHIUpdateInfo
{
public:
    explicit PHIUpdateInfo( const QString& updatexml );
    QString version( const QString &name ) const;
    QString downloadUrl( const QString &name ) const;
    QString description( const QString &name, const QString &lang ) const;
    QDate releaseDate( const QString &name ) const;
    virtual ~PHIUpdateInfo();

protected:
    QString getProductInfo( const QString &name ) const;

private:
    const QString _xml;
};

#endif // PHIUPDATEINFO_H
