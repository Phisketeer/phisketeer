/*
#    Copyright (C) 2010-2012  Marius B. Schumacher
#    Copyright (C) 2011-2012  Phisys AG, Switzerland
#    Copyright (C) 2012  Phisketeer.org team
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
#ifndef PHILISTENER_H
#define PHILISTENER_H

#include <QPair>
#include <QTcpServer>
#include <QDateTime>
#include "phi.h"

class PHIListener : public QTcpServer
{
    Q_OBJECT

    friend class PHIManager;

public:
    static PHIListener* instance();
    PHIRC init( QObject* );

protected:
    explicit PHIListener( QObject *parent=0 );
    virtual ~PHIListener();
    virtual void incomingConnection( int );

private:
    static PHIListener *_instance;

signals:
    void log( int, const char*, int, const QDateTime&,
              PHIRC, const QString& );
    void processServiceCommand( int code ); // needed for Mac OS X
};

class PHISslListener : public QTcpServer
{
    Q_OBJECT

    friend class PHIManager;

public:
    static PHISslListener* instance();
    PHIRC init( QObject* );

protected:
    PHISslListener( QObject *parent=0 );
    virtual ~PHISslListener();
    virtual void incomingConnection( int );

private:
    static PHISslListener *_instance;

signals:
    void log( int, const char*, int, const QDateTime&,
              PHIRC, const QString& );
};

#endif // PHILISTENER_H
