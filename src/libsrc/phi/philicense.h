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
#ifndef PHILICENSE_H
#define PHILICENSE_H

#include <QDataStream>
#include <QDateTime>
#include "phi.h"

class PHIEXPORT PHILicense
{
    friend PHIEXPORT QDataStream& operator >> ( QDataStream &in, PHILicense &lic );
    friend PHIEXPORT QDataStream& operator << ( QDataStream &out, const PHILicense &lic );

public:
    PHILicense();
    PHILicense( quint32 number, const QString &registrar, const QString &domain,
        const QString &email, const QDate &expiry, quint32 procs );
    virtual ~PHILicense();
    void clear();

    inline quint8 version() const { return _version; }
    inline quint32 number() const { return _number; }
    inline QByteArray registrar() const { return _registrar; }
    inline QByteArray domain() const { return _domain; }
    inline QByteArray email() const { return _email; }
    inline QByteArray id() const { return _id; }
    inline QDate validDate() const { return _validDate; }
    inline quint32 processors() const { return _procs; }
    inline QDateTime created() const { return _created; }
    inline bool isValid() const { return _number>0; }

private:
    quint8 _version;
    quint32 _number, _procs;
    QByteArray _registrar, _domain, _email, _id;
    QDate _validDate;
    QDateTime _created;
};

PHIEXPORT QDataStream& operator >> ( QDataStream &in, PHILicense &lic );
PHIEXPORT QDataStream& operator << ( QDataStream &out, const PHILicense &lic );

#endif // PHILICENSE_H
