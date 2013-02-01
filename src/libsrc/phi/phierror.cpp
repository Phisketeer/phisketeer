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
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QTextStream>
#include <QObject>
#include "phierror.h"

PHIError* PHIError::_instance=0;

PHIError::PHIError( QObject *parent )
    : QObject( parent )
{
    qDebug( "PHIError::PHIError()" );
    initShortDesc();
    initLongDesc();
}

PHIError::~PHIError()
{
    _instance=0;
    qDebug( "PHIError::~PHIError()" );
}

PHIError* PHIError::instance( QObject *parent )
{
    if ( _instance ) return _instance;
    _instance=new PHIError( parent );
    return _instance;
}

void PHIError::print( PHIRC rc, const QString &todo ) const
{
    // QTextStream uses default codec
    QTextStream t( stderr, QIODevice::WriteOnly );
    t << QStringLiteral( "RC=%1 (%2) " ).arg( rc )
       .arg( QObject::tr( shortDesc( rc ) ) );
    t << QObject::tr( longDesc( rc ).toLatin1().data() );
    t << endl << QObject::tr( todo.toLatin1().data() ) << endl;
}

void PHIError::printConsole( const QString &s )
{
    QTextStream t( stderr, QIODevice::WriteOnly );
    t << s << endl;
}
