/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2014  Phisys AG, Switzerland
#    Copyright (C) 2012-2014  Phisketeer.org team
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
#ifndef PHISSERVERREC_H
#define PHISSERVERREC_H

#include <QString>
#include <QHostAddress>

class PHISServerRec
{
public:
    QString _defName;
    QString _name;
    QString _admin;
    QString _scheme;
    quint16 _port;
    qint32 _keepAlive;
    QHostAddress _localAddr;
    QHostAddress _remoteAddr;
};

#endif // PHISSERVERREC_H
