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
#include <QNetworkRequest>
#include <QNetworkReply>
#include "phiimagerequest.h"
#include "phibaseitem.h"
#include "phinetmanager.h"

QNetworkAccessManager* PHIImageRequest::_nam=0;

PHIImageRequest::PHIImageRequest( QObject *parent, const QUrl &url, int num )
    : QObject( parent ), _reply( 0 ), _num( num )
{
    QNetworkAccessManager *nam=networkAccessManager();
    QNetworkRequest req( url );
    _reply=nam->get( req );
    connect( _reply, &QNetworkReply::finished, this, &PHIImageRequest::slotFinished );
}

PHIImageRequest::~PHIImageRequest()
{
    if ( _reply ) _reply->deleteLater();
}

QNetworkAccessManager* PHIImageRequest::networkAccessManager()
{
    if ( _nam==0 ) return PHINetManager::instance()->networkAccessManager();
    return _nam;
}

void PHIImageRequest::slotFinished()
{
    if ( _reply->error()!=QNetworkReply::NoError ) {
        emit error( _reply->errorString() );
    }
    QImage img=QImage::fromData( _reply->readAll() );
    emit imageReady( img, _num );
    deleteLater();
}
