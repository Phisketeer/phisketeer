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
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PHIANETMANAGER_H
#define PHIANETMANAGER_H

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "phia.h"

class PHIAEXPORT PHIANetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT

public:
    explicit PHIANetworkAccessManager( QObject *parent=0 );
    virtual ~PHIANetworkAccessManager();
    inline QString userAgent() const { return QString::fromUtf8( _agent ); }

protected:
    virtual QNetworkReply* createRequest( Operation op, const QNetworkRequest&,
        QIODevice* outgoingData=0 );

protected:
    QByteArray _agent, _accept, _charset;
};

class PHIAEXPORT PHIANetworkAccessManagerNoPhiAccept : public PHIANetworkAccessManager
{
    Q_OBJECT

public:
    explicit PHIANetworkAccessManagerNoPhiAccept( QObject *parent=0 );
    virtual ~PHIANetworkAccessManagerNoPhiAccept();

protected:
    virtual QNetworkReply* createRequest( Operation op, const QNetworkRequest&,
        QIODevice* outgoingData=0 );
};

class PHIAEXPORT PHIANetManager : public QObject
{
    Q_OBJECT

public:
    static PHIANetManager* instance();
    inline PHIANetworkAccessManager* networkAccessManager() const { return _networkAccessManager; }
    inline QString userAgent() const { return networkAccessManager()->userAgent(); }
    inline PHIANetworkAccessManagerNoPhiAccept* networkAccessManagerNoPhiAccept() const
        { return _networkAccessManagerNoPhiAccept; }
    void updateProxy();

protected:
    PHIANetManager( QObject *parent=0 );
    virtual ~PHIANetManager();

private:
    PHIANetworkAccessManager *_networkAccessManager;
    PHIANetworkAccessManagerNoPhiAccept *_networkAccessManagerNoPhiAccept;
    static PHIANetManager *_instance;
};

#endif // PHIANETMANAGER_H
