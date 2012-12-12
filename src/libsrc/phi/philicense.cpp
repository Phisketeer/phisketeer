/*
#    Copyright (C) 2010-2012  Marius B. Schumacher
#    Copyright (C) 2011-2012  Phisys AG, Switzerland
#    Copyright (C) 2012  Phisketeer.org team
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
#include <QCryptographicHash>
#include <QUuid>
#include "philicense.h"

PHILicense::PHILicense()
{
    qDebug( "PHILicense::PHILicense()" );
    clear();
}

PHILicense::PHILicense( quint32 serial, const QString &registrar, const QString &domain,
    const QString &email, const QDate &expiry, quint32 procs )
{
    _version=1;
    _created=QDateTime::currentDateTime();
    _number=serial;
    _registrar=registrar.toUtf8();
    _domain=domain.toUtf8();
    _email=email.toUtf8();
    _procs=procs;
    _validDate=expiry;
    _id=QUuid::createUuid().toString().toAscii().mid( 1 );
    _id.chop( 1 );
}

PHILicense::~PHILicense()
{
    qDebug( "PHILicense::~PHILicense()" );
}

void PHILicense::clear()
{
    _validDate=QDate( 2010, 1, 1 );
    _created=QDateTime::currentDateTime();
    _number=0;
    _id=QUuid().toString().toAscii().mid( 1 );
    _id.chop( 1 );
    _email="unregistered";
    _domain="localhost";
    _registrar="unregistered";
    _procs=1;
    _version=1;
}

QDataStream& operator>> ( QDataStream &in, PHILicense &l )
{
    QByteArray magic;
    in >> magic;
    if ( magic!="PHILIC" ) {
        return in;
    }
    QByteArray hash, check;
    in >> l._version >> l._number >> l._id >> l._registrar >> l._email >> l._domain >> l._validDate
       >> l._procs >> l._created >> hash;
    check=QCryptographicHash::hash( QByteArray::number( l._number )+l._domain+l._email+l._registrar+
        l._validDate.toString( "yyyy-MM-dd" ).toAscii()+QByteArray::number( l._procs )+magic, QCryptographicHash::Md5 );
    if ( check!=hash ) {
        l._domain="localhost";
        l._procs=1;
        l._number=0;
        l._id=QUuid().toString().toAscii().mid( 1 );
        l._id.chop( 1 );
        l._validDate=QDate( 2010, 1, 1 );
    }
    return in;
}

QDataStream& operator<< ( QDataStream &out, const PHILicense &l )
{
    QByteArray magic="PHILIC";
    QByteArray hash=QCryptographicHash::hash( QByteArray::number( l._number )+l._domain
        +l._email+l._registrar+l._validDate.toString( "yyyy-MM-dd").toAscii()
        +QByteArray::number( l._procs )+magic, QCryptographicHash::Md5 );
    out << magic << l._version << l._number << l._id << l._registrar << l._email << l._domain
        << l._validDate << l._procs << l._created << hash;
    return out;
}
