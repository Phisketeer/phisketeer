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
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PHISYSINFO_H
#define PHISYSINFO_H
#include <QString>
#include <QMutex>
#include "phi.h"

class PHIEXPORT PHISysInfo
{
    friend class PHI;
    Q_DISABLE_COPY( PHISysInfo )

public:
    // inline QString application() const { return _application; }
    static QString systemString();
    static QString realUserName();
    static QString realGroupName();
    static quint32 realUserId();
    static quint32 realGroupId();
    static quint32 effUserId();
    static quint32 effGroupId();
    static quint32 pProcessId();
    static quint32 processId();

private:
    PHISysInfo();
    static QMutex _mutex;
};

#endif // PHISYSINFO_H
