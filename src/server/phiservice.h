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
#ifndef PHISERVICE_H
#define PHISERVICE_H

#include <QtServiceBase>
#include <QGuiApplication>
#include "phimanager.h"

class PHIService : public QtServiceBase
{
public:
    explicit PHIService( int argc, char **argv, const QString &name );
    virtual ~PHIService();

    virtual void createApplication( int &argc, char **argv );
    virtual int executeApplication();
    virtual void pause();
    virtual void processCommand( int code );
    virtual void resume();
    virtual void start();
    virtual void stop();

protected:
    QGuiApplication *_app;
    PHIManager *_manager;
};

#endif // PHISERVICE_H
