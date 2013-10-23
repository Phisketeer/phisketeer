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
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PHISIMAGECACHE_H
#define PHISIMAGECACHE_H
#include <QObject>
#include <QSqlDatabase>
#include "phierror.h"

class PHIRequest;

class PHISImageCache : public QObject
{
    Q_DISABLE_COPY( PHISImageCache )
    Q_OBJECT

public:
    static PHISImageCache* instance();
    virtual ~PHISImageCache();
    QString createId( const PHIRequest *req );
    const QString& lastError() const { return _lastError; }
    PHIRC lastRC() const { return _rc; }
    void cleanCache( const PHIRequest *req ) const;

protected:
    PHISImageCache( QObject *parent );

private:
    static PHISImageCache *_instance;
    QSqlDatabase _db;
    PHIRC _rc;
    QString _lastError, _name;
};

#endif // PHISIMAGECACHE_H
