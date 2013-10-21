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
#ifndef PHISPAGECACHE_H
#define PHISPAGECACHE_H

#include <QReadWriteLock>
#include <QHash>
#include <QSet>
#include <QMutex>
#include <QDateTime>
#include <QString>

class PHIRequest;
class PHIBasePage;

typedef QHash<QString, const PHIBasePage*> PageHash; // canonical file name -> page
typedef QHash<QString, QDateTime> PageModified; // page id -> date time

class PHISPageCache
{
    Q_DISABLE_COPY( PHISPageCache )

public:
    static PHIBasePage* page( const PHIRequest *req ); // returns a copy
    static PHIBasePage* insert( const PHIRequest *req, const PHIBasePage *p ); // returns a copy of p
    static QDateTime modified( const PHIRequest *req, const QString &pageId );
    static int getDbId();
    static void removeDbId( int );
    static void invalidate();

private:
    static QReadWriteLock _lock;
    static QMutex _dbLock;
    static QHash <QString, PageHash> _pages;
    static QHash <QString, PageModified> _modified;
    static QSet <int> _dbIds;
};

inline int PHISPageCache::getDbId()
{
    _dbLock.lock();
    int i=-1;
    while( 1 ) {
        if ( _dbIds.contains( ++i ) ) continue;
        break;
    }
    _dbIds.insert( i );
    _dbLock.unlock();
    return i;
}

inline void PHISPageCache::removeDbId( int id )
{
    _dbLock.lock();
    _dbIds.remove( id );
    _dbLock.unlock();
}

#endif // PHISPAGECACHE_H
