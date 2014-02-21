/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2014  Phisys AG, Switzerland
#    Copyright (C) 2012-2014  Phisketeer.org team
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
#include <QSqlDatabase>
#include "phierror.h"
#include "phi.h"

class PHIRequest;

class PHIEXPORT PHISession : public QObject
{
    Q_DISABLE_COPY( PHISession )
    Q_OBJECT

public:
    static PHISession* instance();
    PHIRC init( QString &error, QObject *parent=0 );
    QString createSession( const PHIRequest *req, qint32 timeout, const QString &sid=QString() ) const;
    bool validateSession( const PHIRequest *req, qint32 timeout, const QString &sid ) const;
    virtual ~PHISession();

protected:
    PHISession() : QObject( 0 ) {}

private:
    static PHISession *_instance;
    QSqlDatabase _db;
    QString _name;
};

inline PHISession* PHISession::instance()
{
    if ( Q_LIKELY( _instance ) ) return _instance;
    _instance=new PHISession();
    return _instance;
}

#endif // PHISESSION_H
