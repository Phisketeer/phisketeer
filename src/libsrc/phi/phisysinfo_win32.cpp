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
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QSysInfo>
#include "phisysinfo.h"
#include "phi.h"

PHISysInfo::PHISysInfo()
{
    qDebug( "PHISysInfo::PHISysInfo()" );
}

quint8 PHISysInfo::systemType() {
	return static_cast<quint8>(PHI_SYS_TYPE_WIN32);
}

QString PHISysInfo::systemString() {
	static QString ver;
	if ( !ver.isNull() ) return ver;
	switch ( QSysInfo::windowsVersion() ) {
	case QSysInfo::WV_NT: ver="Windows NT/4.0"; break;
	case QSysInfo::WV_2000: ver="Windows 2000/5.0"; break;
	case QSysInfo::WV_XP: ver="Windows XP/5.1"; break;
	case QSysInfo::WV_2003: ver="Windows 2003/5.2"; break;
	case QSysInfo::WV_VISTA: ver="Windows Vista/6.0"; break;
    case QSysInfo::WV_WINDOWS7: ver="Windows 7/6.1"; break;
    case QSysInfo::WV_WINDOWS8: ver="Windows 8/6.2"; break;
	default: ver="Windows";
	}
	ver+="; I386";
	return ver;
}

quint32 PHISysInfo::realUserId() {
    return static_cast<quint32>(0);
}

quint32 PHISysInfo::effUserId() {
    return static_cast<quint32>(0);
}

quint32 PHISysInfo::realGroupId() {
    return static_cast<quint32>(0);
}

quint32 PHISysInfo::effGroupId() {
    return static_cast<quint32>(0);
}

quint32 PHISysInfo::processId() {
    return static_cast<quint32>(0);
}

quint32 PHISysInfo::pProcessId() {
    return static_cast<quint32>(0);
}
