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
#include <QSysInfo>
#include "phisysinfo.h"
#include "phi.h"

PHISysInfo::PHISysInfo()
{
    qDebug( "PHISysInfo::PHISysInfo()" );
}

QString PHISysInfo::systemString() {
	static QString ver;
	if ( !ver.isNull() ) return ver;
	switch ( QSysInfo::windowsVersion() ) {
    case QSysInfo::WV_NT: ver=QStringLiteral( "Windows NT/4.0" ); break;
    case QSysInfo::WV_2000: ver=QStringLiteral( "Windows 2000/5.0" ); break;
    case QSysInfo::WV_XP: ver=QStringLiteral( "Windows XP/5.1" ); break;
    case QSysInfo::WV_2003: ver=QStringLiteral( "Windows 2003/5.2" ); break;
    case QSysInfo::WV_VISTA: ver=QStringLiteral( "Windows Vista/6.0" ); break;
    case QSysInfo::WV_WINDOWS7: ver=QStringLiteral( "Windows 7/6.1" ); break;
    case QSysInfo::WV_WINDOWS8: ver=QStringLiteral( "Windows 8/6.2" ); break;
    default: ver=QStringLiteral( "Windows" );
	}
    ver+=QStringLiteral( "; I686" );
	return ver;
}

/* todo: get real ids */
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
