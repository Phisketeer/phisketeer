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
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PHISESSION_H
#define PHISESSION_H

#include <QObject>
#include "phiresponserec.h"
#include "phierr.h"

class PHISession : public QObject
{
    Q_OBJECT

public:
    PHISession( PHIResponseRec *resp, const QString &dbDir, QObject *parent=0 );
    QString createSession( qint32 timeout, const QString &id=QString() );
    bool validateSession( const QString &uid, qint32 timeout );
    virtual ~PHISession();

private:
    PHIResponseRec *_resp;
    int _id;
};

#endif // PHISESSION_H
