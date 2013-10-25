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
#include <QNetworkRequest>
#include <QTimer>
#include <QTextCodec>
#include "phinetmanager.h"
#include "phinetrequest.h"
#include "phisysinfo.h"
#include "phierror.h"

PHINetRequestThread::PHINetRequestThread( PHIResponseRec *resp, QObject *parent )
    : QThread( parent ), _resp( resp )
{
    qDebug( "PHINetRequestThread::PHINetRequestThread()" );
    Q_ASSERT( _resp );
    _sem.release( 1 );
}

PHINetRequestThread::~PHINetRequestThread()
{
    qDebug( "PHINetRequestThread::~PHINetRequestThread()" );
}

QString PHINetRequestThread::getText( const QString &url )
{
    get( url, PHINetRequest::Text );
    if ( _arr.isEmpty() ) return QString();
    QTextCodec *codec=QTextCodec::codecForName( _codec );
    if ( !codec ) return QString::fromUtf8( _arr );
    return codec->toUnicode( _arr );
}

QImage PHINetRequestThread::getImage( const QString &url )
{
    get( url, PHINetRequest::Image );
    if ( _arr.isEmpty() ) return QImage();
    return QImage::fromData( _arr ).convertToFormat( QImage::Format_ARGB32_Premultiplied );
}

void PHINetRequestThread::get( const QString &url, PHINetRequest::Type type )
{
    _sem.acquire( 1 );
    _url=url;
    _type=type;
    start( QThread::HighPriority );
    _sem.acquire( 1 );
    quit();
    wait( 5000 );
}

void PHINetRequestThread::run()
{
    PHINetRequest request( _url, _type, &_sem );
    exec();
    _codec=request.codec();
    _arr=request.data();
    if ( request.rc()!=PHIRC_OK ) {
        _resp->log( PHILOGERR, request.rc(), request.error() );
    }
}

PHINetRequest::PHINetRequest( const QString &url, Type type, QSemaphore *sem, QObject *parent )
    : QObject( parent ), _rc( PHIRC_OK ), _sem( sem )
{
    qDebug( "PHINetRequest::PHINetRequest()" );
    Q_ASSERT( _sem );
    _codec="utf-8";
    _req.setUrl( QUrl( url ) );
    if ( type==Text ) _req.setRawHeader( "Accept", "text/*,*/*;q=0.5" );
    else _req.setRawHeader( "Accept", "image/png,image/*;q=0.7" ),
    _req.setRawHeader( "Accept-Charset", "utf8,ISO-8859-1,*;q=0.5" );
    QTimer::singleShot( 0, this, SLOT( start() ) );
}

PHINetRequest::~PHINetRequest()
{
    qDebug( "PHINetRequest::~PHINetRequest()" );
}

void PHINetRequest::start()
{
    qDebug( "PHINetRequest::start()" );
    if ( !_req.url().isValid() ) {
        _rc=PHIRC_ARGUMENT_ERROR;
        _error=QObject::tr( "Url '%1' is invalid.").arg( _req.url().toString() );
        _sem->release( 1 );
        return;
    }
    QNetworkAccessManager *manager=PHINetManager::instance()->networkAccessManager();
    connect( manager, &QNetworkAccessManager::finished, this, &PHINetRequest::requestFinished );
    manager->get( _req );
}

void PHINetRequest::requestFinished( QNetworkReply *reply )
{
    // @todo: check for moved content
    if ( reply->error()==QNetworkReply::NoError ) _arr=reply->readAll();
    else {
        qDebug( "error %s", qPrintable( reply->errorString() ) );
        _rc=PHIRC_OBJ_ACCESS_ERROR;
        _error=QObject::tr( "Could not get data from url '%1': %2" )
            .arg( reply->request().url().toString() )
            .arg( reply->errorString() );
    }
    if ( reply->hasRawHeader( "Charset" ) ) _codec=reply->rawHeader( "Charset" );
    else if ( reply->hasRawHeader( "Content-Type") ) {
        _codec=reply->rawHeader( "Content-Type" );
        int pos=_codec.indexOf( '=' );
        if ( pos!=-1 ) {
            _codec.replace( 0, pos+1, "" );
            qDebug( "_codec=%s", _codec.data() );
            if ( _codec.isEmpty() ) _codec="utf-8";
        } else _codec="utf-8";
    }
    reply->deleteLater();
    _sem->release( 1 );
}
