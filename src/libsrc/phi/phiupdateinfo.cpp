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
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QRegExp>
#include "phiupdateinfo.h"
#include "phisysinfo.h"
#include "phi.h"

PHIUpdateInfo::PHIUpdateInfo( const QString &xml )
    : _xml( xml )
{
    qDebug( "PHIUpdateInfo::PHIUpdateInfo()" );
}

PHIUpdateInfo::~PHIUpdateInfo()
{
    qDebug( "PHIUpdateInfo::~PHIUpdateInfo()" );
}

QString PHIUpdateInfo::getProductInfo( const QString &name ) const
{
    QString sub=_xml;
    sub.replace( QRegExp( ".*name=\""+name+"\">" ), "" );
    sub.replace( QRegExp( "</product>.*" ), "" );
    return sub;
}

QString PHIUpdateInfo::description( const QString &name, const QString &lang ) const
{
    QString sub=getProductInfo( name );
    if ( !sub.contains( "lang=\""+lang+"\">" ) ) return QString();
    sub.replace( QRegExp( ".*lang=\""+lang+"\">" ), "" );
    sub.replace( QRegExp( "</description>.*" ), "" );
    return sub.simplified();
}

QString PHIUpdateInfo::version( const QString &name ) const
{
    return PHI::tag( "version", getProductInfo( name ).simplified() );
}

QString PHIUpdateInfo::downloadUrl( const QString &name ) const
{
    QString os;
    if ( PHISysInfo::systemType()==PHI_SYS_TYPE_LINUX ) os="linux";
    else if ( PHISysInfo::systemType()==PHI_SYS_TYPE_WIN32 ) os="win32";
    else if ( PHISysInfo::systemType()==PHI_SYS_TYPE_MACOSX ) os="macos";
    QString sub=getProductInfo( name );
    sub.replace( QRegExp( ".*os=\""+os+"\">" ), "" );
    sub.replace( QRegExp( "</url>.*" ), "" );
    return sub.simplified();
}

QDate PHIUpdateInfo::releaseDate( const QString &name ) const
{
    QString d=PHI::tag( "releasedate", getProductInfo( name ) ).simplified();
    return QDate::fromString( d, "yyyy-MM-dd" );
}
