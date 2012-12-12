/*
#    Copyright (C) 2010-2012  Marius Schumacher
#    Copyright (C) 2011-2012  Phisys AG, Switzerland
#    Copyright (C) 2012  Phisketeer.org team
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "apacherequest.h"
#include "phiparent.h"
#include <httpd.h>
#include <http_core.h>

static int addHeaderItem( void *data, const char *key, const char *value )
{
    QHash <QByteArray, QByteArray> *hash=static_cast<QHash <QByteArray, QByteArray> *>(data);
    //qDebug( "addTableItem %s %s", key, value );
    //hash->insert( QString::fromUtf8( QByteArray::fromRawData( key, qstrlen( key ) ) ),
    //    QString::fromUtf8( QByteArray::fromRawData( value, qstrlen( value ) ) ) );
    hash->insert( QByteArray::fromRawData( key, qstrlen( key ) ),
        QByteArray::fromRawData( value, qstrlen( value ) ) );
    return 1;
}

ApacheRequest::ApacheRequest( request_rec *r, PHIResponseRec *resp )
    : PHISRequest()
{
    //qDebug( "ApacheRequest::ApacheRequest()" );
    _resp=resp;
    _url=QUrl::fromEncoded( QByteArray::fromRawData( r->unparsed_uri,
        qstrlen( r->unparsed_uri ) ), QUrl::StrictMode );
    _documentRoot=QString::fromUtf8( ap_document_root( r ) );

    apr_table_do( addHeaderItem, &_headers, r->headers_in, NULL );

    // Content
    _keywords.insert( KContentType, _headers.value( "Content-Type" ) );
    _contentLength=_headers.value( "Content-Length", 0 ).toLongLong();

    _keywords.insert( KMethod, QByteArray::fromRawData( r->method, qstrlen( r->method ) ) );
    _url.setEncodedHost( QByteArray::fromRawData( r->hostname, qstrlen( r->hostname ) ));
    if ( _url.userName().isEmpty() )
        _url.setUserName( QString::fromUtf8( QByteArray::fromRawData( r->user, qstrlen( r->user ) ) ) );
    if ( _url.password().isEmpty() )
        _url.setPassword( QString() );

    QByteArray arr=QByteArray::fromRawData( r->protocol, qstrlen( r->protocol ) );
    _httpMajor=arr.mid( 5, 1 ).toInt();
    _httpMinor=arr.mid( 7, 1 ).toInt();
    _canonicalFilename=QString::fromUtf8( QByteArray::fromRawData( r->filename, qstrlen( r->filename ) ) );
    qDebug( "------------------->%s", qPrintable(_canonicalFilename) );

    server_rec *s=r->server;
    Q_ASSERT( s );
    _keywords.insert( KDefName, QByteArray::fromRawData( s->defn_name, qstrlen( s->defn_name ) ) );
    _keywords.insert( KServerDesc, QByteArray( ap_get_server_description(), qstrlen( ap_get_server_description() ) ) );
    _keywords.insert( KAdmin, QByteArray::fromRawData( s->server_admin, qstrlen( s->server_admin ) ) );
    _keywords.insert( KServerHostname, QByteArray::fromRawData( s->server_hostname, qstrlen( s->server_hostname ) ) );
    if ( s->port>0 ) _url.setPort( s->port );

    /** @todo implement SSL stuff properly */
    if ( s->port==443 ) _url.setScheme( "https" );
    else _url.setScheme( "http" );

    _keepAlive=static_cast<qint32>( s->keep_alive_timeout/1000 );
    _tmpDir=PHIParent::instance()->tempDir( _url.host() );
    _imgDir=_tmpDir+QDir::separator()+"img";
    //qDebug( "_tmpDir=%s", qPrintable( _tmpDir ) );

    conn_rec *c=r->connection;
    Q_ASSERT( c );
    _localIP=QHostAddress( c->local_ip );
    _remoteIP=QHostAddress( c->remote_ip );
}

ApacheRequest::~ApacheRequest()
{
    //qDebug( "ApacheRequest::~ApacheRequest()" );
}

void ApacheRequest::setPostEncodedUrl( const QByteArray &query )
{
    QList <QPair <QString, QString> > list=QUrl::fromEncoded( query ).queryItems();
    QPair <QString, QString> pair;
    foreach ( pair, list ) _postData.insertMulti( pair.first, pair.second );
}
