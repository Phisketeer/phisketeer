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
#ifndef PHIITEMPLUGIN_H
#define PHIITEMPLUGIN_H
#include "phibaseitem.h"

class PHIEXPORT PHIItemPluginIF
{
public:
    virtual PHIBaseItem* create( PHIWID, const PHIBaseItemPrivate& ) const=0;
    virtual PHIBaseItem* copy( const PHIBaseItem* ) const=0;
    virtual QStringList keys() const=0;
    virtual PHIWID wid( const QString &key ) const=0;
    virtual QString category() const=0;
    virtual QPixmap pixmap() const=0;
};

Q_DECLARE_INTERFACE( PHIItemPluginIF, "org.phisketeer.phi.items/1.0" )

class PHIEXPORT PHIItemPlugin : public QObject, public PHIItemPluginIF
{
    Q_OBJECT
    Q_INTERFACES( PHIItemPluginIF )

public:
    virtual PHIBaseItem* create( PHIWID wid, const PHIBaseItemPrivate &p ) const=0;
    virtual PHIBaseItem* copy( const PHIBaseItem *it ) const=0;
    virtual QStringList keys() const=0;
    virtual PHIWID wid( const QString &key ) const=0;
    virtual QString category() const=0;
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/navigation/star" ) ); }
};

#endif // PHIITEMPLUGIN_H
