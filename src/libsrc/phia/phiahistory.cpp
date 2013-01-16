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
#include <QSettings>
#include <QFile>
#include <QDir>
#include <QApplication>
#include <QImage>
#include "phiahistory.h"
#include "phia.h"
#include "phi.h"

PHIAHistoryItem::PHIAHistoryItem( const QUrl &url, const QString &title, const QIcon &icon,
    const QDateTime &lastVisited, const QUrl &originalUrl, const QVariant &userData )
    : _url( url ), _originalUrl( originalUrl ), _title( title ), _icon( icon ),
    _lastVisited( lastVisited ), _userData( userData )
{
    _isValid=url.isValid();
}

PHIAHistoryItem::PHIAHistoryItem()
{
    _isValid=false;
}

QDataStream& operator<<( QDataStream &out, const PHIAHistoryItem &it )
{
    out << it._url << it._originalUrl << it._title << it._icon << it._lastVisited << it._userData;
    return out;
}

QDataStream& operator>>( QDataStream &in, PHIAHistoryItem &it )
{
    in >> it._url >> it._originalUrl >> it._title >> it._icon >> it._lastVisited >> it._userData;
    it._isValid=it.url().isValid();
    return in;
}

PHIAHistory::PHIAHistory( QObject *parent )
    : QObject( parent ), _currentPos( -1 )
{
    qDebug( "PHIAHistory::PHIAHistory()" );
    QSettings s;
    _maxItemCount=s.value( PHIA::configName( PHIA::MaxHistoryItems ), 30 ).toInt();
}

PHIAHistory::~PHIAHistory()
{
    qDebug( "PHIAHistory::~PHIAHistory()" );
}

void PHIAHistory::addHistoryItem( const PHIAHistoryItem &it )
{
    if ( !it.isValid() ) return;
    //qDebug( "PHIAHistory::addHistoryItem()" );
    PHIAHistoryStorage::instance()->addHistoryItem( it );
    for ( int i=0; i<_items.count(); i++ ) {
        if ( it.url()==_items.at( i ).url() ) {
            //qDebug( "Found history item at %d <%s>", i, qPrintable( it.url().toString() ) );
            _currentPos=i;
            return;
        }
    }
    //qDebug( "Adding history item at 0 <%s>", qPrintable( it.url().toString() ) );
    PHIAHistoryItem cur=currentItem();
    _items=backItems();
    if ( cur.isValid() ) _items.prepend( cur );
    _items.prepend( it );
    _currentPos=0;
    if ( _items.count() > _maxItemCount ) _items.removeLast();
}

PHIAHistoryItem PHIAHistory::currentItem() const
{
    //qDebug( "currPos %d", _currentPos );
    if ( _items.isEmpty() ) return PHIAHistoryItem();
    if ( _currentPos>-1 ) {
        if ( _items.count()>_currentPos ) return _items.at( _currentPos );
    }
    return PHIAHistoryItem();
}

void PHIAHistory::removeItem( const QUrl &url )
{
    for ( int i=0; i < _items.count(); i++ ) {
        if ( _items.at( i ).url()==url ) {
            _items.removeAt( i );
            return;
        }
    }
}

PHIAHistoryItem PHIAHistory::itemAt( int pos ) const
{
    if ( pos < 0 || pos >= _items.count() ) return PHIAHistoryItem();
    return _items.at( pos );
}

PHIAHistoryItem PHIAHistory::backItem() const
{
    return itemAt( _currentPos+1 );
}

PHIAHistoryItem PHIAHistory::forwardItem() const
{
    return itemAt( _currentPos-1 );
}

QList <PHIAHistoryItem> PHIAHistory::forwardItems() const
{
    QList <PHIAHistoryItem> list;
    for ( int i=0; i < _currentPos; i++ ) list.append( _items.at( i ) );
    return list;
}

QList <PHIAHistoryItem> PHIAHistory::backItems() const
{
    QList <PHIAHistoryItem> list;
    for ( int i=_currentPos+1; i<_items.count(); i++ ) list.append( _items.at( i ) );
    return list;
}

bool PHIAHistory::canGoBack() const
{
    return ((_currentPos+1) < _items.count());
}

bool PHIAHistory::canGoForward() const
{
    return (_currentPos > 0);
}

void PHIAHistory::back()
{
    if ( canGoBack() ) _currentPos++;
}

void PHIAHistory::forward()
{
    if ( canGoForward() ) _currentPos--;
}

void PHIAHistory::goToItem( const PHIAHistoryItem &it )
{
    for ( int i=0; i < _items.count(); i++ ) {
        if ( it.url()==_items.at( i ).url() ) {
            _currentPos=i;
            break;
        }
    }
}

void PHIAHistory::setMaximumItemCount( int max )
{
    QList <PHIAHistoryItem> list;
    for ( int i=0; i < qMin( max, _items.count() ); i++ ) list.append( _items.at( i ) );
    _items=list;
    _maxItemCount=max;
}

PHIAHistoryStorage* PHIAHistoryStorage::_instance=0;

PHIAHistoryStorage* PHIAHistoryStorage::instance()
{
    if ( _instance ) return _instance;
    _instance=new PHIAHistoryStorage( qApp );
    return _instance;
}

PHIAHistoryStorage::PHIAHistoryStorage( QObject *parent )
    : QObject( parent )
{
    qDebug( "PHIAHistoryStorage::PHIAHistoryStorage()" );
    QSettings s;
    QString dir=s.value( PHIA::configName( PHIA::CacheDirectory ) ).toString();
    qDebug( "CacheDirectory: %s", qPrintable( dir ) );
    QFile f( dir+QDir::separator()+"history.db" );
    if ( f.open( QIODevice::ReadOnly ) ) {
        QDataStream ds( &f );
        ds.setVersion( PHI_DSV );
        ds >> _items;
        f.close();
    }
    _maxItemCount=s.value( PHIA::configName( PHIA::MaxHistoryItems ), 30 ).toInt();
    if ( _items.count() > 0 ) _currentItem=_items.at( 0 );
}

PHIAHistoryStorage::~PHIAHistoryStorage()
{
    qDebug( "PHIAHistoryStorage::~PHIAHistoryStorage()" );
    _instance=0;
}

void PHIAHistoryStorage::save()
{
    QSettings s;
    QString dir=s.value( PHIA::configName( PHIA::CacheDirectory ) ).toString();
    qDebug( "CacheDirectory: %s", qPrintable( dir ) );
    QFile f( dir+QDir::separator()+"history.db" );
    if ( f.open( QIODevice::WriteOnly ) ) {
        QMutexLocker l( &_mutex );
        QDataStream ds( &f );
        ds.setVersion( PHI_DSV );
        ds << _items;
        f.close();
    }
    _instance=0;
}

void PHIAHistoryStorage::updateIcon( const QUrl &url, const QIcon &icon )
{
    if ( icon.isNull() ) return;
    PHIAHistoryItem it;
    _mutex.lock();
    foreach ( it, _items ) {
        if ( it.url()==url ) {
            it.setIcon( icon );
            _mutex.unlock();
            emit historyItemsChanged();
            return;
        }
    }
    _mutex.unlock();
}

void PHIAHistoryStorage::updateTitle( const QUrl &url, const QString &t )
{
    if ( t.isEmpty() ) return;
    PHIAHistoryItem it;
    _mutex.lock();
    foreach ( it, _items ) {
        if ( it.url()==url ) {
            it.setTitle( t );
            _mutex.unlock();
            emit historyItemsChanged();
            return;
        }
    }
    _mutex.unlock();
}

void PHIAHistoryStorage::addHistoryItem( const PHIAHistoryItem &it )
{
    if ( !it.isValid() ) return;
    _mutex.lock();
    if ( _items.count()>0 ) {
        if ( _items.at( 0 ).url()==it.url() ) {
            _mutex.unlock();
            return; // item is already at top
        }
    }
    for ( int i=0; i < _items.count(); i++ ) {
        if ( it.url()==_items.at( i ).url() ) {
            _items.removeAt( i );
            break;
        }
    }
    _items.prepend( it );
    _currentItem=it;
    if ( _items.count() > _maxItemCount ) _items.removeLast();
    _mutex.unlock();
    emit historyItemsChanged();
}

void PHIAHistoryStorage::setMaximumItemCount( int max )
{
    QMutexLocker l( &_mutex );
    QList <PHIAHistoryItem> list;
    for ( int i=0; i < qMin( max, _items.count() ); i++ ) list.append( _items.at( i ) );
    _items=list;
    _maxItemCount=max;
}
