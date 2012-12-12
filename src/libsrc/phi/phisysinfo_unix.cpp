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
#include <unistd.h>
#include <sys/utsname.h>
#include <grp.h>
#include <pwd.h>
#include "phisysinfo.h"
#include "phi.h"

#ifndef __STDC__
#error You need a standardized C/C++ compiler to compile Phi
#endif

PHISysInfo::PHISysInfo()
{
    qDebug( "PHISysInfo::PHISysInfo()" );
}

QString PHISysInfo::systemString()
{
    static QString _systemString;
    if ( !_systemString.isNull() ) return _systemString;

    _mutex.lock();
    struct utsname utsn;
    uname( &utsn );
#ifdef Q_OS_MAC
    _systemString="Macintosh; Intel Mac OS X "+QString::fromLocal8Bit( utsn.sysname );
    _systemString+=" "+QString::fromLocal8Bit( utsn.release );
#else
    _systemString=QString::fromLocal8Bit( utsn.sysname )+';';
    //_systemString+=QString::fromLocal8Bit( utsn.nodename )+';';
    _systemString+=QString::fromLocal8Bit( utsn.release )+';';
    _systemString+=QString::fromLocal8Bit( utsn.version );
    //_systemString+=QString::fromLocal8Bit( utsn.machine );
#endif
    _mutex.unlock();
    return _systemString;
}

QString PHISysInfo::realUserName()
{
    QString _realUserName;
    _mutex.lock();
    // getlogin is not threadsafe
    _realUserName=QString::fromLocal8Bit( getlogin() );
    if ( _realUserName.isEmpty() ){
        struct passwd *pw;
        pw=getpwuid( getuid() );
        _realUserName=QString::fromLocal8Bit( pw->pw_name );
    }
    _mutex.unlock();
    return _realUserName;
}

QString PHISysInfo::realGroupName()
{
    QString _realGroupName;
    _mutex.lock();
    struct group *grp;
    // getgrgid is not thread safe
    grp=getgrgid( getgid() );
    _realGroupName=QString::fromLocal8Bit( grp->gr_name );
    _mutex.unlock();
    return _realGroupName;
}

quint8 PHISysInfo::systemType() {
#ifdef Q_OS_MAC
    return static_cast<quint8>(PHI_SYS_TYPE_MACOSX);
#else
    return static_cast<quint8>(PHI_SYS_TYPE_LINUX);
#endif
}

quint32 PHISysInfo::realUserId() {
    return static_cast<quint32>(getuid());
}

quint32 PHISysInfo::effUserId() {
    return static_cast<quint32>(geteuid());
}

quint32 PHISysInfo::realGroupId() {
    return static_cast<quint32>(getgid());
}

quint32 PHISysInfo::effGroupId() {
    return static_cast<quint32>(getegid());
}

quint32 PHISysInfo::processId() {
    return static_cast<quint32>(getpid());
}

quint32 PHISysInfo::pProcessId() {
    return static_cast<quint32>(getppid());
}
