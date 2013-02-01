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
    sub.replace( QRegExp( QLatin1String( ".*name=\"" )+name+QLatin1String( "\">" ) ), QString() );
    sub.replace( QRegExp( QStringLiteral( "</product>.*" ) ), QString() );
    return sub;
}

QString PHIUpdateInfo::description( const QString &name, const QString &lang ) const
{
    QString sub=getProductInfo( name );
    if ( !sub.contains( QLatin1String( "lang=\"" )+lang+ QLatin1String( "\">" ) ) ) return QString();
    sub.replace( QRegExp( QLatin1String( ".*lang=\"" )+lang+QLatin1String( "\">" ) ), QString() );
    sub.replace( QRegExp( QStringLiteral( "</description>.*" ) ), QString() );
    return sub.simplified();
}

QString PHIUpdateInfo::version( const QString &name ) const
{
    return PHI::tag( QStringLiteral( "version" ), getProductInfo( name ).simplified() );
}

QString PHIUpdateInfo::downloadUrl( const QString &name ) const
{
    QString os;
#ifdef Q_OS_WIN
    os=QStringLiteral( "win32" );
#elif defined Q_OS_MAC
    os=QStringLiteral( "macos" );
#else
    os=QStringLiteral( "linux" );
#endif
    QString sub=getProductInfo( name );
    sub.replace( QRegExp( QLatin1String( ".*os=\"" )+os+QLatin1String( "\">" ) ), QString() );
    sub.replace( QRegExp( QStringLiteral( "</url>.*" ) ), QString() );
    return sub.simplified();
}

QDate PHIUpdateInfo::releaseDate( const QString &name ) const
{
    QString d=PHI::tag( QStringLiteral( "releasedate" ), getProductInfo( name ) ).simplified();
    return QDate::fromString( d, QStringLiteral( "yyyy-MM-dd" ) );
}
