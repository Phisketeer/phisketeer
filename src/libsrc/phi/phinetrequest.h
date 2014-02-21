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
#ifndef PHINETREQUEST_H
#define PHINETREQUEST_H

#include <QThread>
#include <QSemaphore>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QImage>
#include "phiresponserec.h"

class PHINetRequest : public QObject
{
    Q_OBJECT

public:
    enum Type { Text, Image };
    PHINetRequest( const QString &url, Type type, QSemaphore *sem, QObject *parent=0 );
    virtual ~PHINetRequest();
    inline QByteArray data() const { return _arr; }
    inline QByteArray codec() const { return _codec; }
    inline QString error() const { return _error; }
    inline PHIRC rc() const { return _rc; }

public slots:
    void start();
    void requestFinished( QNetworkReply *reply );

private:
    QNetworkRequest _req;
    QByteArray _arr, _codec;
    PHIRC _rc;
    QString _error;
    QSemaphore *_sem;
};

class PHIEXPORT PHINetRequestThread : public QThread
{
    Q_OBJECT

public:
    PHINetRequestThread( PHIResponseRec *resp, QObject *parent=0 );
    virtual ~PHINetRequestThread();
    QString getText( const QString &url );
    QImage getImage( const QString &url );

protected:
    void get( const QString &url, PHINetRequest::Type type );
    virtual void run();

private:
    PHIResponseRec *_resp;
    QSemaphore _sem;
    QByteArray _arr, _codec;
    QString _url;
    PHINetRequest::Type _type;
};

#endif // PHINETREQUEST_H
