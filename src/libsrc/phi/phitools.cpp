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
#include "phitools.h"

PHIIcon::PHIIcon( const char *name, const QColor &col, const QColor &dis )
    : QIcon( PHI::dropShadowedPixmap( ( col.isValid() ?
    PHI::colorizedPixmap( QPixmap( L1( name ) ), col )
    : QPixmap( L1( name ) ) ), QPointF( 1., 2. ) ) )
{
    QPixmap pix( PHI::colorizedPixmap( QPixmap( L1( name ) ), dis ) );
    addPixmap( pix, QIcon::Disabled, QIcon::Off );
    addPixmap( pix, QIcon::Disabled, QIcon::On );
}

PHIIcon::PHIIcon( const QPixmap &src, const QColor &col, const QColor &dis )
    : QIcon( PHI::dropShadowedPixmap( ( col.isValid() ?
    PHI::colorizedPixmap( src ) : src ), QPointF( 1., 2. ) ) )
{
    QPixmap pix( PHI::colorizedPixmap( src, dis ) );
    addPixmap( pix, QIcon::Disabled, QIcon::Off );
    addPixmap( pix, QIcon::Disabled, QIcon::On );
}
