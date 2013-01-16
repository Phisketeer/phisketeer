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
#ifndef PHISITEMCACHE_H
#define PHISITEMCACHE_H

#include <QObject>
#include <QHash>
#include <QReadWriteLock>
#include "phisrequest.h"
#include "phisitem.h"
#include "phis.h"

/*
class PhisPosition
{
public:
    explicit PhisPosition( const QByteArray &d, const QRect &r ) : _d(d), _r(r) {};
    explicit PhisPosition() : _d(QByteArray()), _r(QRect()) {};
    inline QByteArray d() { return _d; }
    inline QRect r() { return _r; }

private:
    QByteArray _d;
    QRect _r;
};
*/

typedef QHash <QByteArray, PHISItem*> PhisItemHash;
typedef QHash <QByteArray, PhisItemHash> PhisItemPageHash;
typedef QHash <QByteArray, QString> PhisImageHash;
typedef QHash <QByteArray, PhisImageHash> PhisImagePageHash;
//typedef QHash <QByteArray, PhisPosition> PhisPositionHash;
typedef QHash <QByteArray, QByteArray> PhisPositionHash;
typedef QHash <QByteArray, PhisPositionHash> PhisPositionPageHash;

class PHISEXPORT PHISItemCache : public QObject
{
    Q_OBJECT

public:
    static PHISItemCache* instance( QObject *parent=0 );

public:
    void invalidate();
    void invalidate( const PHISRequest *req, const QByteArray &page );

    inline QReadWriteLock* readWriteLock() const { return &_lock; }

    /** Inserts item @a it. @warning This member is @b not thread safe. */
    void insertItem( const QString &domain, const QByteArray &page, const QByteArray &id, PHISItem *it );

    /** Returns item for ID @a id. @warning This member is @b not thread safe. */
    inline const PHISItem* item( const QString &domain, const QByteArray &page, const QByteArray &id ) const {
        return _items.value( domain ).value( page ).value( id, 0 );
    }
    /** Returns if list contains item @a id. @warning This member is @b not thread safe. */
    inline bool containsItem( const QString &domain, const QByteArray &page, const QByteArray &id ) const {
        return _items.value( domain ).value( page ).contains( id );
    }

    void insertImageId( const QString &domain, const QByteArray &page, const QByteArray &key, const QString &id,
        const QByteArray &lang=QByteArray( "C" ), int num=-1 );
    QString imageId( const QString &domain, const QByteArray &page, const QByteArray &key,
        const QByteArray &lang=QByteArray( "C" ), int num=-1 ) const;

/*
    void insertPosition( const QString &domain, const QByteArray &page, const QByteArray &id,
        const PhisPosition &pos );
    inline PhisPosition position( const QString &domain, const QByteArray &page, const QByteArray &id ) const {
        QReadLocker l( &_posLock ); return _positions.value( domain ).value( page ).value( id ); }
*/
    void insertPosition( const QString &domain, const QByteArray &page, const QByteArray &id,
        const QByteArray &pos );
    inline QByteArray position( const QString &domain, const QByteArray &page, const QByteArray &id ) const {
        QReadLocker l( &_posLock ); return _positions.value( domain ).value( page ).value( id ); }

    inline void insertText( const QString &key, const QString &t ) {
        QWriteLocker l( &_textLock );
        _textHash.insert( key, t );
    }
    inline QString text( const QString &key ) const {
        QReadLocker l( &_textLock );
        return _textHash.value( key, QString() );
    }

protected:
    PHISItemCache( QObject *parent=0 );
    virtual ~PHISItemCache();
    //void openDB( const PHISRequest *req );

private:
    static PHISItemCache *_instance;
    mutable QReadWriteLock _lock, _textLock, _imageIdLock, _posLock;
    QHash <QString, PhisItemPageHash> _items;
    QHash <QString, QString> _textHash;
    QHash <QString, PhisImagePageHash> _imageIds;
    QHash <QString, PhisPositionPageHash> _positions;
    //int _id;
};

#endif // PHISITEMCACHE_H
