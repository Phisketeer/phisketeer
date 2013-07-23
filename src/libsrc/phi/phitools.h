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
#ifndef PHITOOLS_H
#define PHITOOLS_H
#include <QIcon>
#include <QColor>
#include "phi.h"

class PHIEXPORT PHIIcon : public QIcon
{
public:
    explicit PHIIcon( const char *name, const QColor &disabled=QColor( Qt::black ) );
    explicit PHIIcon( const QPixmap &pix, const QColor &disabled=QColor( Qt::black ) );
};

#endif // PHITOOLS_H
