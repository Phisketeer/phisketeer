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
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PHIIMAGEREQUEST_H
#define PHIIMAGEREQUEST_H
#include <QObject>
#include "phi.h"
class QNetworkReply;
class QNetworkAccessManager;

class PHIEXPORT PHIImageRequest : public QObject
{
    Q_OBJECT

public:
    explicit PHIImageRequest( QObject *parent, const QUrl &url, int num=0 );
    virtual ~PHIImageRequest();
    static void setNetworkAccessManager( QNetworkAccessManager *nam ) { _nam=nam; }
    static QNetworkAccessManager* networkAccessManager();

signals:
    void imageReady( const QImage &img, int num=0 );
    void error( const QString &err );

private slots:
    void slotFinished();

private:
    static QNetworkAccessManager *_nam;
    QNetworkReply *_reply;
    int _num;
};

#endif // PHIIMAGEREQUEST_H
