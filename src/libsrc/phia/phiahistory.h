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
#ifndef PHIAHISTORY_H
#define PHIAHISTORY_H

#include <QObject>
#include <QUrl>
#include <QVariant>
#include <QList>
#include <QIcon>
#include <QDateTime>
#include <QMutex>
#include <QMutexLocker>
#include "phia.h"

class PHIAEXPORT PHIAHistoryItem
{
    friend PHIAEXPORT QDataStream& operator<<( QDataStream&, const PHIAHistoryItem& );
    friend PHIAEXPORT QDataStream& operator>>( QDataStream&, PHIAHistoryItem& );

public:
    PHIAHistoryItem( const QUrl&, const QString&, const QIcon&, const QDateTime&,
        const QUrl &originalUrl=QUrl(), const QVariant &userData=QVariant() );
    PHIAHistoryItem();
    inline QUrl url() const { return _url; }
    inline QUrl originalUrl() const { return _originalUrl; }
    inline QString title() const { return _title; }
    inline QIcon icon() const { return _icon; }
    inline QDateTime lastVisited() const { return _lastVisited; }
    inline QVariant userData() const { return _userData; }
    inline void setUserData( const QVariant &v ) { _userData=v; }
    inline bool isValid() const { return _isValid; }
    inline void setIcon( const QIcon &icon ) { _icon=icon; }
    inline void setTitle( const QString &t ) { _title=t; }

private:
    QUrl _url, _originalUrl;
    QString _title;
    QIcon _icon;
    QDateTime _lastVisited;
    QVariant _userData;
    bool _isValid;
};

PHIAEXPORT QDataStream& operator<<( QDataStream&, const PHIAHistoryItem& );
PHIAEXPORT QDataStream& operator>>( QDataStream&, PHIAHistoryItem& );

class PHIAEXPORT PHIAHistory : public QObject
{
    Q_OBJECT

public:
    explicit PHIAHistory( QObject *parent=0 );
    virtual ~PHIAHistory();
    PHIAHistoryItem backItem() const;
    PHIAHistoryItem forwardItem() const;
    PHIAHistoryItem itemAt( int pos ) const;
    PHIAHistoryItem currentItem() const;
    QList <PHIAHistoryItem> forwardItems() const;
    QList <PHIAHistoryItem> backItems() const;
    bool canGoBack() const;
    bool canGoForward() const;
    void back();
    void forward();
    void goToItem( const PHIAHistoryItem& );
    void addHistoryItem( const PHIAHistoryItem& );
    void setMaximumItemCount( int );
    void removeItem( const QUrl& );

    inline int maximumItemCount() const { return _maxItemCount; }
    inline QList <PHIAHistoryItem> allItems() const { return _items; }
    inline int count() const { return _items.count(); }
    inline int currentItemIndex() const { return _currentPos; }

private:
    QList <PHIAHistoryItem> _items;
    int _maxItemCount;
    int _currentPos;
};

class PHIAEXPORT PHIAHistoryStorage : public QObject
{
    Q_OBJECT

public:
    static PHIAHistoryStorage* instance();
    inline QList <PHIAHistoryItem> allItems() const { QMutexLocker l( &_mutex ); return _items; }
    inline PHIAHistoryItem currentItem() const { QMutexLocker l( &_mutex ); return _currentItem; }
    void setMaximumItemCount( int );
    void addHistoryItem( const PHIAHistoryItem& );
    void save();
    void updateTitle( const QUrl&, const QString& );
    void updateIcon( const QUrl&, const QIcon& );

protected:
    explicit PHIAHistoryStorage( QObject *parent );
    virtual ~PHIAHistoryStorage();

signals:
    void historyItemsChanged();

private:
    static PHIAHistoryStorage *_instance;
    int _maxItemCount;
    QList <PHIAHistoryItem> _items;
    PHIAHistoryItem _currentItem;
    mutable QMutex _mutex;
};

#endif // PHIAHISTORY_H
