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
#include <QDateTime>
#include "phiobject.h"
#include "philogwriter.h"
#include "phierror.h"

const char *_phioutofmemory=QT_TRANSLATE_NOOP( "PHIObject", "Ensure to provide enough memory." );

PHIObject::PHIObject( QObject *parent )
    : QObject( parent )
{
    connect( this, SIGNAL( log( int, const char*, int, const QDateTime&,
        PHIRC, const QString& ) ), PHILogWriter::instance(),
        SLOT( log( int, const char*, int, const QDateTime&,
        PHIRC, const QString& ) ) );
}

PHIObject::~PHIObject()
{
}

void PHIObject::print( int type, const char *file, int line,
    const QDateTime &dt, PHIRC rc, const QString &todo )
{
    Q_UNUSED( type );
    Q_UNUSED( file );
    Q_UNUSED( line );
    Q_UNUSED( dt );
    PHIError::instance()->print( rc, todo );
}

QString PHIObject::formatObjectName( const QString &name ) const
{
    QString buffer=QLatin1Char( '\n' )+tr( "Object name: '%1'." )
        .arg( name.isEmpty() ? objectName() : name  );
    return buffer;
}
