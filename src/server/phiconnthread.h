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
#ifndef PHICONNTHREAD_H
#define PHICONNTHREAD_H

#include <QThread>

class PHIConnThread : public QThread
{
    Q_OBJECT

public:
    explicit PHIConnThread( QObject *parent, int sockdesc, bool usessl );
    virtual ~PHIConnThread();

protected slots:
    virtual void run();

signals:
    void processServiceCommand( int code ); // needed for Mac OS X

private:
    int _socketDesc;
    bool _usessl;
};

#endif // PHICONNTHREAD_H
