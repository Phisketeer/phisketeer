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
#ifndef MACSERVICE_H
#define MACSERVICE_H
#include <QObject>
class PHIManager;
class QGuiApplication;

class MACService : public QObject
{
    Q_OBJECT

public:
    explicit MACService( int argc, char **argv, const QString &name );
    virtual ~MACService();
    void stop();
    int exec();

protected slots:
    void processServiceCommand( int code );
    void start();

protected:
    QString _name;
    QGuiApplication *_app;
    PHIManager *_manager;
};

#endif // MACSERVICE_H
