/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2013  Phisys AG, Switzerland
#    Copyright (C) 2012-2013  Phisketeer.org team
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QObject>
#ifdef Q_OS_MAC
#include "macservice.h"
#else
#include "phiservice.h"
#endif

int main( int argc, char **argv )
{
#ifdef Q_OS_MAC
    // Unfortunately Mac OS X sandboxing requires special handling
    MACService service( argc, argv, QStringLiteral( "phis" ) );
#else
    PHIService service( argc, argv, QStringLiteral( "phis" ) );
#endif
    return service.exec();
}
