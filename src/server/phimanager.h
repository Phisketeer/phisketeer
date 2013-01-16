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
#ifndef PHIMANAGER_H
#define PHIMANAGER_H

#include <QObject>

class PHIManager : public QObject
{
    Q_OBJECT

public:
    explicit PHIManager( QObject *parent, const QString& );
    virtual ~PHIManager();

public:
    void start();
    void updateConfig();
    void stop();

signals:
    void processServiceCommand( int code ); // needed for Mac OS X

private:
    bool _logWriterStart, _sslListenerStart;
};

#endif // PHIMANAGER_H
