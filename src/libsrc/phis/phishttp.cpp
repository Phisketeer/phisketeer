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
#include <QTcpSocket>
#include <QSslSocket>
#include <QFile>
#include <QFileInfo>
#include <QTimer>
#include <QTemporaryFile>
#include <QDir>
#include <QLocale>
#include <QCoreApplication>
#include <QSqlDatabase>
#include "phishttp.h"
#include "phisprocessor.h"
#include "phisysinfo.h"
#include "phispagecache.h"
#include "phisparent.h"
#include "phiapplication.h"
#include "phislogwriter.h"

PHISHttp::PHISHttp( QObject *parent )
    : QObject( parent ), _state( PHISHttp::Connected ), _req( 0 )
{
    qDebug( "PHISHttp::PHISHttp()" );
    connect( this, &PHISHttp::log, PHISLogWriter::instance(), &PHISLogWriter::log, Qt::QueuedConnection );
    _timer=new QTimer( this );
    connect( _timer, SIGNAL( timeout() ), this, SIGNAL( close() ) );
    _timer->setInterval( PHISParent::instance()->keepAlive() );
    _timer->start();
}

PHISHttp::~PHISHttp()
{
    if ( QSqlDatabase::contains( QString::number( _dbConnId ) ) ) {
        { // needed to delete db instance before removing
            QSqlDatabase db=QSqlDatabase::database( QString::number( _dbConnId ) );
            if ( db.isValid() && db.isOpen() ) {
                qDebug( "Removing DB connection Id %d", _dbConnId );
                db.close();
            }
        } // remove db instance
        QSqlDatabase::removeDatabase( QString::number( _dbConnId ) );
    }
    PHISPageCache::removeDbId( _dbConnId );
    qDebug( "PHISHttp::~PHISHttp()" );
}

PHIRC PHISHttp::init( qintptr socketDesc, bool usessl, QString &err )
{
    Q_UNUSED( err );
    _dbConnId=PHISPageCache::getDbId();
    if ( usessl ) {
        _socket=new QSslSocket( this );
        _socket->setSocketDescriptor( socketDesc );
        QSslSocket *s=qobject_cast<QSslSocket*>(_socket);
        Q_ASSERT( s );
        connect( s, SIGNAL( sslErrors( const QList<QSslError>& ) ), this,
            SLOT( slotSslErrors( const QList<QSslError>& ) ) );
        qDebug( "Starting SSL connection" );
        QString file=PHISParent::instance()->value( SL( "SSLCertificate" ) ).toString();
        if ( !QFile::exists( file ) ) {
            emit log( PHILOGERR, PHIRC_IO_FILE_ACCESS_ERROR,
                err=tr( "Could not open certificate file '%1'.").arg( file ) );
            return PHIRC_IO_FILE_ACCESS_ERROR;
        } else s->setLocalCertificate( file );
        file=PHISParent::instance()->value( SL( "SSLPrivateKey" ) ).toString();
        if ( !QFile::exists( file ) ) {
            emit log( PHILOGERR, PHIRC_IO_FILE_ACCESS_ERROR,
                err=tr( "Could not open private key file '%1'.").arg( file ) );
            return PHIRC_IO_FILE_ACCESS_ERROR;
        } else s->setPrivateKey( file );
        s->startServerEncryption();
        _server._scheme=SL( "https" );
    } else {
        _socket=new QTcpSocket( this );
        _socket->setSocketDescriptor( socketDesc );
        _server._scheme=SL( "http" );
    }
    //qDebug( "PHISHttp::init() after socket creation" );
    connect( _socket, SIGNAL( readyRead() ), this, SLOT( readyRead() ) );
    connect( _socket, SIGNAL( disconnected() ), this, SIGNAL( close() ) );
    _server._localAddr=_socket->localAddress();
    _server._remoteAddr=_socket->peerAddress();
    _server._name=SL( "phis/" )+L1( PHISVERSION );
    _server._keepAlive=PHISParent::instance()->keepAlive();
    _server._defName=PHISParent::instance()->name();
    _server._admin=PHISParent::instance()->admin();
    _server._port=_socket->localPort();
    _resp.setMinorHttpVer( 0 );
    try {
        _req=new PHISServerRequest( &_server, &_resp, this );
    } catch ( std::bad_alloc& ) {
        emit log( PHILOGCRIT, PHIRC_OUT_OF_MEMORY, err=tr( "Could not init request." ) );
        return PHIRC_OUT_OF_MEMORY;
    }
    qDebug( "PHISHttp::init() - end" );
    return PHIRC_OK;
}

void PHISHttp::slotSslErrors( const QList<QSslError> &errors )
{
    qDebug( "PHISHttp::slotSslErrors()" );
    QString tmp;
    QSslError err;
    foreach( err, errors ) tmp+=err.errorString()+QString::fromLatin1( PHI::nl() );
    emit log( PHILOGERR, PHIRC_TCP_UNKNOWN_SOCKET_ERROR, tmp );
    emit close();
}

void PHISHttp::sendError( PHIRC rc, const QString &reason )
{
    _resp.setBody( _resp.createErrorResponse( rc, reason ) );
    _resp.setContentLength( static_cast<qint64>(_resp.body().size()) );
    _resp.setContentType( QByteArray( "text/html;charset=utf-8" ) );
    QByteArray header=_resp.createErrorResponseHeader( rc, _server._name );
    _socket->write( header );
    _socket->write( "\r\n" );
    _socket->write( _resp.body() );
    _socket->close();
}

PHIRC PHISHttp::sendResponse( QString &err )
{
#ifdef PHIDEBUG
    qWarning( "sendResponse %s", qPrintable( _req->url().toString() ) );
    QTime t=QTime::currentTime();
#endif
    PHISProcessor proc( _req, _dbConnId );
    proc.run();

#ifdef PHIDEBUG
    qWarning( "Time elapsed: %d", t.msecsTo( QTime::currentTime() ) );
#endif

    PHIResponseRecLogEntry entry;
    foreach ( entry, _resp.logEntries() )
        emit log( entry._type, entry._file, entry._line, entry._dt, entry._rc, entry._desc );
    if ( _resp.rc()!=PHIRC_OK ) {
        if ( _resp.rc()==PHIRC_HTTP_NOT_MODIFIED ) {
            //qDebug( "NOT MODIFIED %s", qPrintable( _req->url().toString() ) );
            if ( _resp.options() & PHIResponseRec::SendFile ) {
                QFileInfo fi( _resp.fileName() );
                if ( _resp.contentType().startsWith( "text/html" ) ) {
                    QByteArray type=PHI::mimeTypeForFile( fi );
                    if ( type.isEmpty() ) {
                        err=tr( "Could not identify mime type of requested resource '%1'." )
                            .arg( fi.suffix() );
                        emit log( PHILOGERR, PHIRC_HTTP_UNSUPPORTED_MEDIA_TYPE, err );
                        return PHIRC_HTTP_UNSUPPORTED_MEDIA_TYPE;
                    }
                    _resp.setContentType( type );
                }
            }
            _socket->write( _resp.createResponseHeader( PHIRC_HTTP_NOT_MODIFIED, _server._name ) );
            _socket->write( "\r\n" );
            _socket->flush();
            return PHIRC_OK;
        }
        err=_resp.errorDesc();
        emit log( _resp.errorType(), _resp.errorFile(), _resp.errorLine(), _resp.errorDT(),
            _resp.rc(), _resp.errorDesc() );
        return _resp.rc();
    }
    if ( _resp.options() & PHIResponseRec::Redirect ) {
        if ( !_req->setRedirectedFile( _resp.fileName() ) ) {
            err=tr( "Redirected file '%1' not found." ).arg( _resp.fileName() );
            emit log( PHILOGERR, PHIRC_HTTP_NOT_FOUND, err );
            return PHIRC_HTTP_NOT_FOUND;
        }
        _resp.clear();
        //redirected=true;
        return sendResponse( err );
    }
    if ( _resp.options() & PHIResponseRec::SendFile ) {
        QFile f( _resp.fileName() );
        if ( !f.open( QIODevice::ReadOnly ) ) {
            err=tr( "Could not open file '%1' for reading." ).arg( _resp.fileName() );
            emit log( PHILOGERR, PHIRC_HTTP_FORBIDDEN, err );
            err=tr( "File not found." );
            return PHIRC_HTTP_FORBIDDEN;
        }
        QFileInfo fi( f );
        if ( _resp.contentType().startsWith( "text/html" ) ) {
            QByteArray type=PHI::mimeTypeForFile( fi );
            if ( type.isEmpty() ) {
                err=tr( "Could not identify mime type of requested resource '%1'." )
                    .arg( fi.suffix() );
                emit log( PHILOGERR, PHIRC_HTTP_UNSUPPORTED_MEDIA_TYPE, err );
                return PHIRC_HTTP_UNSUPPORTED_MEDIA_TYPE;
            }
            _resp.setContentType( type );
        }

        _socket->write( _resp.createResponseHeader( PHIRC_HTTP_OK, _server._name ) );
        _socket->write( "\r\n" );
        if ( _resp.options() & PHIResponseRec::HeaderOnly ) {
            f.close();
            _socket->flush();
            return PHIRC_OK;
        }
        while ( !f.atEnd() ) {
            _socket->write( f.read( 1024*32 ) );
            //QCoreApplication::instance()->processEvents();
        }
        f.close();
    } else {
        _socket->write( _resp.createResponseHeader( PHIRC_HTTP_OK, _server._name ) );
        _socket->write( "\r\n" );
        _socket->write( _resp.body() );
    }
    _socket->flush();
    return PHIRC_OK;
}

void PHISHttp::readyRead()
{
    if ( _state!=Reading ) {
        _timer->stop();
        _state=Reading;
        _readHeader=false;
        _headerStr=QString();
        _bytesDone=0;
        _contentEntity="";
        _content=Empty;
    }
    PHIRC rc;
    QString tmp;
    bool end=false;
    while ( !_readHeader ) {
        while ( !end && _socket->canReadLine() ) {
            tmp=QString::fromLatin1( _socket->readLine() );
            if ( tmp==L1( "\r\n" ) || tmp==L1( "\n" ) || tmp.isEmpty() ) end=true;
            else _headerStr+=tmp;
            // @todo: make header size configurable
            if ( _headerStr.size()>1024*10 ) {
                tmp=tr( "HTTP header is too large." );
                emit log( PHILOGERR, PHIRC_HTTP_BAD_REQUEST, tmp );
                sendError( PHIRC_HTTP_BAD_REQUEST, tmp );
                return;
            }
        }
        if ( !end ) {
            _timer->start();
            return;
        }
        _readHeader=true;
        emit log( PHILOGDBUG, PHIRC_OK, SL( "HTTP header ---<\n" )+_headerStr+SL( ">---" ) );
        rc=parseRequestHeader( _headerStr, tmp );
        if ( rc!=PHIRC_OK ) {
            sendError( rc, tmp );
            return;
        }
        end=false;
        _headerStr=QString();
        if ( _req->contentLength()>0 ) {
            if ( _req->contentType().startsWith( L1( "multipart/form-data" ) ) ) {
                if ( !_req->contentType().contains( SL( "boundary=" ) ) ) {
                    tmp=tr( "Missing boundary for media type (%1)." ).arg( SL( "multipart/form-data" ) );
                    emit log( PHILOGERR, PHIRC_HTTP_BAD_REQUEST, tmp );
                    sendError( PHIRC_HTTP_BAD_REQUEST, tmp );
                    return;
                }
                _content=MultipartEncoded;
                _boundary="--"+_req->contentType().mid( _req->contentType().indexOf( QLatin1Char( '=' ) )+1 ).toLatin1();
                _addPart=_readPart=false;
                _tmpFile=0;
            } else if ( _req->contentType().startsWith( L1( "application/x-www-form-urlencoded" ) ) ) {
                if ( _req->contentLength() > static_cast<qint64>(1024*1024) ) {
                    tmp=tr( "Content length too large (%1)." ).arg( _req->contentLength() );
                    emit log( PHILOGERR, PHIRC_HTTP_BAD_REQUEST, tmp );
                    sendError( PHIRC_HTTP_BAD_REQUEST, tmp );
                    return;
                }
                _content=UrlEncoded;
            } else if ( _req->contentType().startsWith( L1( "application/x-phi" ) ) ) _content=PhiEncoded;
            else {
                tmp=tr( "Unsupported content type in POST (%1)." ).arg( _req->contentType() );
                emit log( PHILOGERR, PHIRC_HTTP_UNSUPPORTED_MEDIA_TYPE, tmp );
                sendError( PHIRC_HTTP_UNSUPPORTED_MEDIA_TYPE, tmp );
                return;
            }
        }
        // qDebug( "ContentLength %lld", _requestInfo.contentLength() );
    }

    if ( _bytesDone<_req->contentLength() ) { //read data after HTTP header
        if ( _socket->bytesAvailable() ) {
            //qDebug( "Bytes available: %lld", _socket->bytesAvailable() );
            _timer->stop();
            if ( _content==UrlEncoded ) rc=readUrlEncoded( tmp );
            else if ( _content==MultipartEncoded ) rc=readMultipartEncoded( tmp );
            else if ( _content==PhiEncoded ) rc=readPhiEncoded( tmp );
            if ( rc!=PHIRC_OK ) {
                sendError( rc, tmp );
                return;
            }
        }
        if ( _bytesDone<_req->contentLength() ) {
            _timer->start();
            return;
        }
    }
    _req->init();

    rc=sendResponse( tmp );
    if ( rc!=PHIRC_OK ) sendError( rc, tmp );

    _req->clear();
    _state=Connected;
    _timer->start();
}

PHIRC PHISHttp::readUrlEncoded( QString &err )
{
    Q_UNUSED( err );
    QByteArray chunk=_socket->read( _req->contentLength()-_bytesDone );
    _bytesDone+=chunk.length();
    _contentEntity+=chunk;
    if ( _bytesDone<_req->contentLength() ) return PHIRC_OK;
    _req->setPostEncodedUrl( _contentEntity );
    _contentEntity="";
    /*
    if ( !_req._postUrl.isValid() ) {
        err=tr( "Invalid URL encoded POST data (%1)." )
            .arg( QString::fromUtf8( _req._postUrl.encodedQuery() ) );
        emit log( PHILOGERR, PHIRC_HTTP_BAD_REQUEST, err );
        return PHIRC_HTTP_BAD_REQUEST;
    }
    */
    return PHIRC_OK;
}

PHIRC PHISHttp::readMultipartEncoded( QString &err )
{
    // qDebug( "readMultipartEncoded %lld\n<%s>", _requestInfo.contentLength(), _boundary.constData() );
    QByteArray arr;
    _headerStr=QString();

    while ( _socket->canReadLine() ) {
        arr=_socket->readLine();
        _bytesDone+=arr.length();
        //qDebug( "_bytesDone %lld %lld (%s)", _bytesDone, _socket->bytesAvailable(), qPrintable( _req->url().toString() ) );
        if ( arr.startsWith( _boundary ) ) {
            _newPart=true;
            _readPart=false;
            if ( _addPart ) {
                _addPart=false;
                if ( _tmpFile ) {
                    _tmpFile->resize( _tmpFile->pos()-2 ); // chop newlines
                    _req->insertTmpFile( _name, _tmpFile );
                    _tmpFile=0;
                } else {
                    if ( _contentEntity.endsWith( "\r\n" ) ) _contentEntity.chop( 2 );
                    else if ( _contentEntity.endsWith( '\n' ) ) _contentEntity.chop( 1 );
                    _req->addPostQueryItem( QString::fromUtf8( _name ), QString::fromUtf8( _contentEntity ) );
                }
            }
            if ( _bytesDone==_req->contentLength() ) return PHIRC_OK;
            continue;
        }
        if ( _newPart ) {
            if ( arr.startsWith( "Content-Disposition:" ) ) {
                arr=arr.mid( arr.indexOf( '"' )+1 );
                _name=arr.left( arr.indexOf( '"' ) );
                _contentEntity="";
                if ( arr.contains( "filename=" ) ) {
                    arr=arr.mid( arr.indexOf( "filename=" )+10 );
                    QByteArray filename=arr.left( arr.indexOf( '"' ) );
                    if ( !filename.isEmpty() ) {
                        qDebug( "filename=<%s>", filename.constData() );
                        _tmpFile=new QTemporaryFile( phiApp->tmpPath()+L1( "/phiXXXXXX" ), this );
                        // _tmpFile->setAutoRemove( false );
                        _tmpFile->setObjectName( QString::fromUtf8( filename ) );
                        if ( !_tmpFile->open() ) {
                            delete _tmpFile;
                            err=tr( "Could not create temporary file." );
                            emit log( PHILOGCRIT, PHIRC_IO_FILE_CREATION_ERROR, err );
                            return PHIRC_HTTP_INTERNAL_SERVER_ERROR;
                        }
                    }
                }
                // qDebug( "name=%s", _name.constData() );
                continue;
            }
            if ( arr=="\r\n" || arr=="\n" || arr.isEmpty() ) {
                _readPart=true;
                _newPart=false;
                _addPart=true;
                continue;
            }
        }
        if ( _readPart ) {
            if ( _tmpFile ) _tmpFile->write( arr );
            else _contentEntity+=arr;
        }
    }
    return PHIRC_OK;
}

PHIRC PHISHttp::readPhiEncoded( QString &err )
{
    err=tr( "This is currently not implemented." );
    emit log( PHILOGERR, PHIRC_NOT_IMPL_ERROR, err );
    return PHIRC_HTTP_INTERNAL_SERVER_ERROR;
}

PHIRC PHISHttp::parseRequestHeader( const QString &headerStr, QString &err )
{
    PHIRC rc=_req->setupHttpHeader( headerStr, err );
    if ( rc!=PHIRC_OK ) {
        emit log( PHILOGERR, rc, err );
        return rc;
    }
    _resp.setMinorHttpVer( _req->httpServerMinorVersion() );
    return rc;
}
