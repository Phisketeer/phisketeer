/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2014  Phisys AG, Switzerland
#    Copyright (C) 2012-2014  Phisketeer.org team
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
#include "phis.h"

class PHIRequest;
class PHIBasePage;

typedef QHash<QString, const PHIBasePage*> PageHash; // canonical file name -> page
typedef QHash<QString, QDateTime> PageModified; // page id -> date time

class PHISEXPORT PHISPageCache
{
    Q_DISABLE_COPY( PHISPageCache )

public:
    static PHISPageCache* instance();
    ~PHISPageCache() { _instance=0; }

    PHIBasePage* page( const PHIRequest *req, const QString &filename ); // returns a copy
    PHIBasePage* insert( const PHIRequest *req, const PHIBasePage *page, const QString &filename ); // returns a copy of p
    QDateTime modified( const PHIRequest *req, const QString &pageId );
    int getDbId();
    void removeDbId( int );
    void invalidate();

protected:
    PHISPageCache() {}

private:
    static PHISPageCache* _instance;
    QReadWriteLock _lock;
    QMutex _dbLock;
    QHash <QString, PageHash> _pages;
    QHash <QString, PageModified> _modified;
    QSet <int> _dbIds;
};

inline PHISPageCache* PHISPageCache::instance()
{
    if ( Q_LIKELY( _instance ) ) return _instance;
    _instance=new PHISPageCache();
    return _instance;
}

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
