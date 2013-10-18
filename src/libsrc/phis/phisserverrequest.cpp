/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2013  Phisys AG, Switzerland
#    Copyright (C) 2012-2013  Phisketeer.org team
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QDir>
#include "phisserverrequest.h"
#include "phiapplication.h"
#include "phisparent.h"

PHISServerRequest::PHISServerRequest( const PHISServerRec *server, PHIResponseRec *resp, QObject *parent )
    : QObject( parent ), PHIRequest(), _server( server )
{
    Q_ASSERT( resp );
    _resp=resp;
    _keywords.insert( KAdmin, server->_admin.toUtf8() );
    _keywords.insert( KDefName, server->_defName.toUtf8() );
    _keywords.insert( KServerDesc, server->_name.toUtf8() );
    _remoteIP=server->_remoteAddr;
    _localIP=server->_localAddr;
    _keepAlive=server->_keepAlive;
}

void PHISServerRequest::setPostEncodedUrl( const QByteArray &query )
{
    QList <QPair <QString, QString> > list=QUrlQuery( QUrl::fromEncoded( query ) ).queryItems();
    QPair <QString, QString> pair;
    foreach ( pair, list ) _postData.insertMulti( pair.first, pair.second );
}

void PHISServerRequest::clear()
{
    _url.clear();
    _contentLength=0;
    _headers.clear(); // could be set from a recent request
    _cookies.clear();
    _postData.clear();
    _acceptedLangs.clear();
    //_keywords.clear(); // Do NOT clear keywords - they are not setup again in a second request!
    _resp->clear();
    QTemporaryFile *tmpFile;
    foreach ( tmpFile, _tmpFiles ) {
        qDebug( "Closing %s", qPrintable( tmpFile->fileName() ) );
        tmpFile->close();
        delete tmpFile;
    }
    _tmpFiles.clear();
}

PHIRC PHISServerRequest::setupHttpHeader( const QString &headerStr, QString &err )
{
    QStringList lines=headerStr.split( QLatin1Char( '\n' ), QString::SkipEmptyParts );
    if ( lines.count()==0 ) {
        err=tr( "Empty HTTP header." );
        return PHIRC_HTTP_BAD_REQUEST;
    }
    QString line, key, value=SL( " GET POST HEAD PUT CREATE " );
    if ( !lines.at(0).contains( SL( "HTTP/" ) ) ) {
        err=tr( "Missing HTTP header." );
        return PHIRC_HTTP_BAD_REQUEST;
    }
    int start;
    _keywords.insert( KMethod, headerStr.left( start=headerStr.indexOf( QLatin1Char( ' ' ) ) ).toUpper().toLatin1() );
    if ( !value.contains( QLatin1Char( ' ' )+method()+QLatin1Char( ' ' ) ) ) {
        err=tr( "Unknown method." );
        return PHIRC_HTTP_METHOD_NOT_ALLOWED;
    }
    key=headerStr.mid( lines.at(0).indexOf( L1( "HTTP/" ) )+5, 3 );
    _httpMajor=key.left(1).toUShort();
    _httpMinor=key.right(1).toUShort();
    if ( _httpMajor!=1 || _httpMinor>2 ) {
        err=tr( "Unknown HTTP protocol version (%1.%2)." ).arg( _httpMajor ).arg( _httpMinor );
        return PHIRC_HTTP_VERSION_NOT_SUPPORTED;
    }
    _url=QUrl::fromEncoded( lines.at(0).mid( start+1,
        lines.at(0).indexOf( QLatin1Char( ' ' ), start+1 )-start-1 ).toUtf8(), QUrl::StrictMode );
    _url.setScheme( _server->_scheme );
    _url.setPort( _server->_port );
    lines.removeFirst();
    foreach ( line, lines ) {
        key=value=line.trimmed();
        if ( !key.contains( QLatin1Char( ':' ) ) ) {
            err=tr( "Bad header entry (%1)." ).arg( key );
            return PHIRC_HTTP_BAD_REQUEST;
        }
        key=key.left( start=key.indexOf( QLatin1Char( ':' ) ) );
        if ( key.size()>200 ) {
            err=tr( "Too large header entry (%1)" ).arg( key );
            return PHIRC_HTTP_BAD_REQUEST;
        }
        value=value.mid( start+2 );
        if ( value.size()>1024*100 ) {
            err=tr( "Too large header entry (%1)" ).arg( key );
            return PHIRC_HTTP_BAD_REQUEST;
        }
        _headers.insert( key.toUtf8(), value.toUtf8() );
    }
    _keywords.insert( KContentType, _headers.value( "Content-Type" ) );
    _contentLength=_headers.value( "Content-Length", 0 ).toLongLong();

    key=QString::fromUtf8( _headers.value( "Host" ) );
    if ( key.contains( QLatin1Char( ':' ) ) ) key=key.left( key.indexOf( QLatin1Char( ':' ) ) ); // remove port if any
    if ( _httpMinor>0 ) {
        if ( key.isEmpty() ) {
            err=tr( "Missing Host header field." );
            return PHIRC_HTTP_BAD_REQUEST;
        }
        _url.setHost( key );
    } else {
        if ( key.isEmpty() ) key=_url.host();
        if ( key.isEmpty() ) {
            err=tr( "Missing host name in URL." );
            return PHIRC_HTTP_BAD_REQUEST;
        }
    }
    if ( !_url.isValid() ) {
        err=tr( "Invalid URL (%1)." ).arg( _url.toString() );
        return PHIRC_HTTP_NOT_FOUND;
    }
    _keywords.insert( KServerHostname, key.toUtf8() );
    value=PHISParent::instance()->documentRoot( key );
    QString index=PHISParent::instance()->index();
    if ( value.isEmpty() ) {
        err=tr( "Document root for virtual host name '%1' not found." ).arg( key );
        return PHIRC_HTTP_FORBIDDEN;
    }
    _documentRoot=value;
    _tmpDir=PHISParent::instance()->tempDir( key );
    _imgDir=_tmpDir+L1( "/img" );
    _canonicalFilename=_documentRoot+_url.path();
    //qDebug( "FILE %s %s", qPrintable( _canonicalFilename ), qPrintable( _url.toString() ) );

    if ( _canonicalFilename.endsWith( L1( "/phi.phis" ) ) ) {
        _modified=QDateTime::currentDateTime();
    } else {
        QFileInfo fi( _canonicalFilename );
        if ( fi.isDir() ) {
            if ( _canonicalFilename.endsWith( QLatin1Char( '/' ) )
                || _canonicalFilename.endsWith( QLatin1Char( '\\' ) ) )
                _canonicalFilename.chop( 1 );
            index.replace( QRegExp( SL( "\\s" ) ), QString() );
            QStringList indexList=index.split( QLatin1Char( ',' ), QString::SkipEmptyParts );
            foreach ( index, indexList ) {
                fi.setFile( _canonicalFilename+QDir::separator()+index );
                if ( fi.exists() ) break;
            }
        }
        _canonicalFilename=fi.canonicalFilePath();
        if ( !QFile::exists( _canonicalFilename ) ) {
            qDebug( "value=%s", qPrintable( value ) );
            err=tr( "File '%1' not found." ).arg( _url.path() );
            return PHIRC_HTTP_NOT_FOUND;
        }
        _modified=fi.lastModified();
    }
    return PHIRC_OK;
}
