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
#ifndef PHISLISTENER_H
#define PHISLISTENER_H

#include <QPair>
#include <QTcpServer>
#include "phi.h"

class QDateTime;

class PHISListener : public QTcpServer
{
    Q_OBJECT

public:
    static PHISListener* instance();
    virtual ~PHISListener();
    PHIRC init( QObject*, const QString &name=PHI::defaultString() );

protected:
    explicit PHISListener( QObject *parent=0 );
    virtual void incomingConnection( qintptr );

private:
    static PHISListener *_instance;

signals:
    void log( int, const char*, int, const QDateTime&, PHIRC, const QString& );
};

class PHISslListener : public QTcpServer
{
    Q_OBJECT

public:
    static PHISslListener* instance();
    virtual ~PHISslListener();
    PHIRC init( QObject*, const QString &name=PHI::defaultString() );

protected:
    PHISslListener( QObject *parent=0 );
    virtual void incomingConnection( qintptr );

private:
    static PHISslListener *_instance;

signals:
    void log( int, const char*, int, const QDateTime&, PHIRC, const QString& );
};

#endif // PHISLISTENER_H
