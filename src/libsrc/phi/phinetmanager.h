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
#ifndef PHINETMANAGER_H
#define PHINETMANAGER_H
#include <QNetworkAccessManager>
#include <QNetworkCookieJar>
#include <QNetworkCookie>
#include <QNetworkProxy>
#include <QMutexLocker>
#include "phi.h"

class QNetworkReply;
class QNetworkRequest;

class PHIEXPORT PHINetworkCookies : public QNetworkCookieJar
{
    Q_OBJECT

public:
    static PHINetworkCookies* instance();
    virtual bool setCookiesFromUrl( const QList<QNetworkCookie> &cookieList, const QUrl &url );
    virtual QList<QNetworkCookie> cookiesForUrl( const QUrl &url ) const;
    bool removeCookie( const QString &domain, const QString &name );
    inline QList<QNetworkCookie> cookies() const { QMutexLocker l( &_mutex ); return allCookies(); }
    inline void clear() { QMutexLocker l( &_mutex ); setAllCookies( QList<QNetworkCookie>() ); }

protected:
    PHINetworkCookies( QObject *parent=0 );
    virtual ~PHINetworkCookies();

private:
    static PHINetworkCookies* _instance;
    QString _dbPath;
    mutable QMutex _mutex;
};

class PHIEXPORT PHINetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT

public:
    explicit PHINetworkAccessManager( QObject *parent=0, bool serverMode=false );
    virtual ~PHINetworkAccessManager() {}
    static inline QString userAgent() { return QString::fromUtf8( _agent ); }
    static inline QString accept() { return QString::fromLatin1( _accept ); }
    static inline QString charset() { return QString::fromLatin1( _charset ); }
    static inline QString acceptedLangs() { return QString::fromLatin1( _acceptedLangs ); }
    static inline void setUserAgent( const QString &a ) { _agent=a.toUtf8(); }
    static inline void setAccept( const QString &a ) { _accept=a.toLatin1(); }
    static inline void setCharset( const QString &c ) { _charset=c.toLatin1(); }
    static inline void setAcceptedLangs( const QString &a ) { _acceptedLangs=a.toLatin1(); }

protected:
    virtual QNetworkReply* createRequest( Operation op, const QNetworkRequest&, QIODevice* outgoingData=0 );

protected:
    static QByteArray _agent, _accept, _charset, _acceptedLangs;
};

class PHIEXPORT PHINetManager : public QObject
{
    Q_OBJECT

public:
    static PHINetManager* instance();
    virtual ~PHINetManager();
    PHINetworkAccessManager* networkAccessManager( QObject *parent );
    inline PHINetworkAccessManager* defaultNetworkAccessManager() const { return _defaultNam; }
    const QNetworkProxy& proxy() const { return _proxy; }
    void updateProxy();

protected:
    PHINetManager( QObject *parent=0 );

private:
    static PHINetManager *_instance;
    QNetworkProxy _proxy;
    PHINetworkAccessManager *_defaultNam;
};

#endif // PHINETMANAGER_H
