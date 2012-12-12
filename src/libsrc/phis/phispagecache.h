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
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PHISPAGECACHE_H
#define PHISPAGECACHE_H

#include <QReadWriteLock>
#include <QHash>
#include <QSet>
#include <QMutex>
#include <QDateTime>
#include <QString>
#include "phisrequest.h"
#include "phispage.h"
#include "phis.h"

typedef QHash<QString, PHISPage*> PageHash;
typedef QHash<QByteArray, QDateTime> PageModified;

class PHISEXPORT PHISPageCache
{
public:
    static void invalidate();

    /** Returns page @a p. @warning This member is @b not thread safe. */
    static PHISPage* page( const PHISRequest *req, const QString &path, const QDateTime &dt );
    static PHISPage* page( const PHISRequest *req ) {
        return page( req, req->canonicalFilename(), req->lastModified() );
    }

    /** Inserts page @a p. @warning This member is @b not thread safe. */
    static PHISPage* insert( const PHISRequest *req, PHISPage *p );
    static PHISPage* insert( const PHISRequest *req, const QString &path, const QDateTime &dt, PHISPage *p );

    static QDateTime modified( const PHISRequest *req, const QByteArray &pageId );

    static inline QReadWriteLock* readWriteLock() { return &_lock; }
    static int getDbId();
    static void removeDbId( int );

private:
    static QReadWriteLock _lock;
    static QMutex _dbLock;
    static QHash <QString, PageHash> _pages;
    static QHash <QString, PageModified> _modified;
    static QSet <int> _dbIds;
};

#endif // PHISPAGECACHE_H
