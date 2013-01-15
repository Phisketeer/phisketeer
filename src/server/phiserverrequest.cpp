/*
#    Copyright (C) 2010-2012  Marius B. Schumacher
#    Copyright (C) 2011-2012  Phisys AG, Switzerland
#    Copyright (C) 2012  Phisketeer.org team
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
#include "phiserverrequest.h"
#include "phiconfig.h"
#include "phiparent.h"

PHIServerRequest::PHIServerRequest( const PHIServerRec *server, PHIResponseRec *resp, QObject *parent )
    : QObject( parent ), PHISRequest(), _server( server )
{
    //qDebug( "PHIServerRequest::PHIServerRequest()" );
    Q_ASSERT( resp );
    _resp=resp;
    _keywords.insert( KAdmin, server->_admin.toUtf8() );
    _keywords.insert( KDefName, server->_defName.toUtf8() );
    _keywords.insert( KServerDesc, server->_name.toUtf8() );
    //_url.setPort( server->_port );
    //_url.setScheme( server->_scheme );
    _remoteIP=server->_remoteAddr;
    _localIP=server->_localAddr;
    _keepAlive=server->_keepAlive;
}

PHIServerRequest::~PHIServerRequest()
{
    //qDebug( "PHIServerRequest::~PHIServerRequest()" );
}

void PHIServerRequest::setPostEncodedUrl( const QByteArray &query )
{
    QList <QPair <QString, QString> > list=QUrlQuery( QUrl::fromEncoded( query ) ).queryItems();
    QPair <QString, QString> pair;
    foreach ( pair, list ) _postData.insertMulti( pair.first, pair.second );
}

void PHIServerRequest::clear()
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

PHIRC PHIServerRequest::setupHttpHeader( const QString &headerStr, QString &err )
{
    QStringList lines=headerStr.split( '\n', QString::SkipEmptyParts );
    if ( lines.count()==0 ) {
        err=tr( "Empty HTTP header." );
        return PHIRC_HTTP_BAD_REQUEST;
    }
    QString line, key, value=" GET POST HEAD PUT CREATE ";
    if ( !lines.at(0).contains( "HTTP/" ) ) {
        err=tr( "Missing HTTP header." );
        return PHIRC_HTTP_BAD_REQUEST;
    }
    int start;
    _keywords.insert( KMethod, headerStr.left( start=headerStr.indexOf( ' ' ) ).toUpper().toLatin1() );
    if ( !value.contains( ' '+method()+' ' ) ) {
        err=tr( "Unknown method." );
        return PHIRC_HTTP_METHOD_NOT_ALLOWED;
    }
    key=headerStr.mid( lines.at(0).indexOf( "HTTP/" )+5, 3 );
    _httpMajor=key.left(1).toUShort();
    _httpMinor=key.right(1).toUShort();
    if ( _httpMajor!=1 || _httpMinor>2 ) {
        err=tr( "Unknown HTTP protocol version (%1.%2)." ).arg( _httpMajor ).arg( _httpMinor );
        return PHIRC_HTTP_VERSION_NOT_SUPPORTED;
    }
    _url=QUrl::fromEncoded( lines.at(0).mid( start+1,
        lines.at(0).indexOf( ' ', start+1 )-start-1 ).toUtf8(), QUrl::StrictMode );
    _url.setScheme( _server->_scheme );
    _url.setPort( _server->_port );
    lines.removeFirst();
    foreach ( line, lines ) {
        key=value=line.trimmed();
        if ( !key.contains( ':' ) ) {
            err=tr( "Bad header entry (%1)." ).arg( key );
            return PHIRC_HTTP_BAD_REQUEST;
        }
        key=key.left( start=key.indexOf( ':' ) );
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
    if ( key.contains( ':' ) ) key=key.left( key.indexOf( ':' ) ); // remove port if any
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
    value=PHIConfig::instance()->documentRoot( key );
    if ( value.isEmpty() ) {
        err=tr( "Virtual host name '%1' not found." ).arg( key );
        return PHIRC_HTTP_FORBIDDEN;
    }
    _documentRoot=value;
    //PHIParent::instance()->readLicenseFile( value, key );
    _tmpDir=PHIParent::instance()->tempDir( key );
    _imgDir=_tmpDir+QDir::separator()+"img";
    _canonicalFilename=_documentRoot+_url.path();
    //qDebug( "FILE %s %s", qPrintable( _canonicalFilename ), qPrintable( _url.toString() ) );

    if ( _canonicalFilename.endsWith( "/phi.phis" ) ) {
        _modified=QDateTime::currentDateTime();
    } else {
        QFileInfo fi( _canonicalFilename );
        if ( fi.isDir() ) {
            if ( _canonicalFilename.endsWith( '/' ) || _canonicalFilename.endsWith( '\\' ) )
                _canonicalFilename.chop( 1 );
            QString index=PHIConfig::instance()->index();
            index.replace( QRegExp( "\\s" ), "" );
            QStringList indexList=index.split( ',', QString::SkipEmptyParts );
            foreach ( index, indexList ) {
                fi.setFile( _canonicalFilename+QDir::separator()+index );
                if ( fi.exists() ) break;
            }
        }
        if ( !QFile::exists( fi.canonicalFilePath() ) ) {
            qDebug( "value=%s", qPrintable( value ) );
            err=tr( "File '%1' not found." ).arg( _url.path() );
            return PHIRC_HTTP_NOT_FOUND;
        }
        _modified=fi.lastModified();
        _canonicalFilename=fi.canonicalFilePath();
    }
    return PHIRC_OK;
}
