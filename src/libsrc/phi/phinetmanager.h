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
#include "phi.h"

class QNetworkReply;
class QNetworkRequest;

class PHIEXPORT PHINetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT

public:
    explicit PHINetworkAccessManager( QObject *parent=0 );
    virtual ~PHINetworkAccessManager();
    inline QString userAgent() const { return QString::fromUtf8( _agent ); }

protected:
    virtual QNetworkReply* createRequest( Operation op, const QNetworkRequest&, QIODevice* outgoingData=0 );

protected:
    QByteArray _agent, _accept, _charset, _acceptedLangs;
};

class PHIEXPORT PHINetManager : public QObject
{
    Q_OBJECT

public:
    static PHINetManager* instance();
    inline PHINetworkAccessManager* networkAccessManager() const { return _networkAccessManager; }
    inline QString userAgent() const { return networkAccessManager()->userAgent(); }
    void updateProxy();

protected:
    PHINetManager( QObject *parent=0 );
    virtual ~PHINetManager();

private:
    PHINetworkAccessManager *_networkAccessManager;
    static PHINetManager *_instance;
};

#endif // PHINETMANAGER_H
