/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2013  Phisys AG, Switzerland
#    Copyright (C) 2012-2013  Phisketeer.org team
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

**    # example for apache2.conf
**    LoadModule phi_module modules/mod_phi.so
**    <IfModule phi_module>
**      AddHandler phi .phis
**      DirectoryIndex index.phis
**      AddType application/x-phi phi
**    </IfModule>
**
**    Then restart Apache:
**
**    # apachectl restart
*/

#define BUFLEN 8192
#define MAX_SIZE 1024*1024

#include <QByteArray>
#include <QDateTime>
#include <QHash>
#include <QHostAddress>
#include <QFileInfo>
#include <QString>
#include <QTemporaryFile>
#include <QSqlDatabase>
#include <QDir>
#include <QObject>

#include "phiprocessor.h"
#include "phiresponserec.h"
#include "phierr.h"
#include "phiparent.h"
#include "phierror.h"
#include "phispagecache.h"
#include "apacherequest.h"

#include "httpd.h"
//#include "apr_pools.h"
//#include "ap_config.h"
//#include "apr.h"
//#include "apr_hooks.h"
//#include "apr_optional_hooks.h"
#include "http_core.h"
#include "http_config.h"
#include "http_protocol.h"
#include "http_log.h"

#ifdef __cplusplus
extern "C" {
#endif

static int cleanupResponseRec( void *ptr ) { delete static_cast<PHIResponseRec*>(ptr); return 0; }
static int cleanupPHIParent( void *ptr ) { delete static_cast<PHIParent*>(ptr); return 0; }

static int translateReturnCode( PHIRC rc )
{
    if ( rc==PHIRC_OK ) return OK;
    if ( rc==PHIRC_DECLINED ) return DECLINED;
    if ( rc>99 && rc<510 ) return static_cast<int>(rc);
    return HTTP_INTERNAL_SERVER_ERROR;
}

static int parseMultipartPostData( request_rec *r, ApacheRequest *req )
{
    qDebug( "parsing POST data" );
    if ( !req->keyword( PHISRequest::KContentType ).contains( "boundary=" ) ) {
        ap_log_rerror( APLOG_MARK, APLOG_WARNING, 0, r, "%s",
            QObject::tr( "Missing boundary for media type (multipart/form-data)" ).toUtf8().constData() );
        return HTTP_BAD_REQUEST;
    }
    QByteArray boundary="--"+req->keyword( PHISRequest::KContentType )
        .mid( req->keyword( PHISRequest::KContentType ).indexOf( '=' )+1 );
    QByteArray name, contentEntity, arr;
    QTemporaryFile *tmpFile=0;

    apr_status_t rv;
    bool eos=false, readPart=false, newPart=false, addPart=false, chunk=false;
    apr_size_t bytes;
    const char *buf;
    apr_bucket *b;
    apr_bucket_brigade *bb;

    bb=apr_brigade_create( r->pool, r->connection->bucket_alloc );
    do {
        rv=ap_get_brigade( r->input_filters, bb, AP_MODE_GETLINE, APR_BLOCK_READ, BUFLEN );
        if ( rv!=APR_SUCCESS ) {
            ap_log_rerror( APLOG_MARK, APLOG_ERR, 0, r, "%s",
                QObject::tr( "Could not read POST input data" ).toUtf8().constData() );
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        for ( b=APR_BRIGADE_FIRST(bb); b!=APR_BRIGADE_SENTINEL(bb); b=APR_BUCKET_NEXT(b) ) {
            if ( APR_BUCKET_IS_EOS(b) ) {
                eos=true;
                break;
            }
            if ( APR_BUCKET_IS_METADATA(b) ) continue;
            rv=apr_bucket_read( b, &buf, &bytes, APR_BLOCK_READ );
            if ( rv!=APR_SUCCESS ) {
                ap_log_rerror( APLOG_MARK, APLOG_ERR, 0, r, "%s",
                    QObject::tr( "Could not read POST input data from bucket" ).toUtf8().constData() );
                return HTTP_INTERNAL_SERVER_ERROR;
            }

	    QByteArray line=QByteArray( buf, bytes );
            //arr=QByteArray( buf, bytes );
	    if ( !line.endsWith( '\n' ) ) {
		if ( !tmpFile ) {
		    if ( chunk ) arr+=line;
		    else arr=line;
		    //qWarning( "chunk arr=%s<---", arr.data() );
		    chunk=true;
		    continue;
		} else arr=line;
	    } else if ( chunk ) {
		arr+=line;
		chunk=false;
	    } else arr=line;
	    //qWarning( "arr=%s", arr.data() );

            if ( arr.startsWith( boundary ) ) {
                newPart=true;
                readPart=false;
                if ( addPart ) {
                    addPart=false;
                    if ( tmpFile ) {
                        tmpFile->resize( tmpFile->pos()-2 ); // chop newlines
                        tmpFile->close();
                        req->insertTmpFile( name, tmpFile );
                        tmpFile=0;
                    } else {
                        if ( contentEntity.endsWith( "\r\n" ) ) contentEntity.chop( 2 );
                        else if ( contentEntity.endsWith( '\n' ) ) contentEntity.chop( 1 );
                        req->addPostQueryItem( QString::fromUtf8( name ), QString::fromUtf8( contentEntity ) );
                        //qWarning( "Adding <%s> <%s>", name.data(), contentEntity.data() );
                    }
                }
                continue;
            }
            if ( newPart ) {
                if ( arr.startsWith( "Content-Disposition: form-data; " ) ) {
                    arr=arr.mid( arr.indexOf( '"' )+1 );
                    name=arr.left( arr.indexOf( '"' ) );
                    contentEntity="";
                    if ( arr.contains( "filename=" ) ) {
                        arr=arr.mid( arr.indexOf( "filename=" )+10 );
                        QByteArray filename=arr.left( arr.indexOf( '"' ) );
                        if ( !filename.isEmpty() ) {
                            // qDebug( "filename=<%s>", filename.data() );
                            tmpFile=new QTemporaryFile( req->tmpDir()+QDir::separator()+"phiXXXXXX" );
                            // tmpFile->setAutoRemove( false );
                            tmpFile->setObjectName( filename );
                            if ( !tmpFile->open() ) {
                                ap_log_rerror( APLOG_MARK, APLOG_ERR, 0, r, "%s",
                                    QObject::tr( "Could not open tmp file %1" ).arg( tmpFile->fileName() ).toUtf8().data() );
                                delete tmpFile;
                                return HTTP_INTERNAL_SERVER_ERROR;
                            }
                        }
                    }
                    // qDebug( "name=%s", name.data() );
                    continue;
                }
                if ( arr=="\r\n" || arr=="\n" || arr.isEmpty() ) {
                    readPart=true;
                    newPart=false;
                    addPart=true;
                    continue;
                }
            }
            if ( readPart ) {
                if ( tmpFile ) tmpFile->write( arr );
                else contentEntity+=arr;
            }
        }
        apr_brigade_cleanup( bb );
    } while ( !eos );
    return OK;
}

static int parseUrlEncodedPostData( request_rec *r, ApacheRequest *req )
{
    if ( req->contentLength() > static_cast<qint64>(MAX_SIZE) ) {
        ap_log_rerror( APLOG_MARK, APLOG_ERR, 0, r, "%s",
            QObject::tr( "POST url-encoded data is limited to %d bytes" ).arg( MAX_SIZE ).toUtf8().constData() );
        return HTTP_REQUEST_ENTITY_TOO_LARGE;
    }
    apr_status_t rv;
    bool eos=false;
    apr_size_t bytes;
    const char *buf;
    apr_bucket *b;
    apr_bucket_brigade *bb;
    QByteArray arr;

    bb=apr_brigade_create( r->pool, r->connection->bucket_alloc );
    do {
        rv=ap_get_brigade( r->input_filters, bb, AP_MODE_READBYTES, APR_BLOCK_READ, BUFLEN );
        if ( rv!=APR_SUCCESS ) {
            ap_log_rerror( APLOG_MARK, APLOG_ERR, 0, r, "%s",
                QObject::tr( "Could not read POST input data" ).toUtf8().constData() );
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        for ( b=APR_BRIGADE_FIRST(bb); b!=APR_BRIGADE_SENTINEL(bb); b=APR_BUCKET_NEXT(b) ) {
            if ( APR_BUCKET_IS_EOS(b) ) {
                eos=true;
                break;
            }
            if ( APR_BUCKET_IS_METADATA(b) ) continue;
            bytes=BUFLEN;
            rv=apr_bucket_read( b, &buf, &bytes, APR_BLOCK_READ );
            if ( rv!=APR_SUCCESS ) {
                ap_log_rerror( APLOG_MARK, APLOG_ERR, 0, r, "%s",
                    QObject::tr( "Could not read POST input data from bucket" ).toUtf8().constData() );
                return HTTP_INTERNAL_SERVER_ERROR;
            }
            arr+=QByteArray( buf, bytes );
        }
        apr_brigade_cleanup( bb );
    } while ( !eos );
    ap_log_rerror( APLOG_MARK, APLOG_ERR, 0, r, "Adding <%s>", arr.constData() );
    req->setPostEncodedUrl( arr );
    /*
    if ( !req->postUrlIsValid() ) {
        ap_log_rerror( APLOG_MARK, APLOG_WARNING, 0, r, "%s",
            QObject::tr( "Invalid URL encoded POST data (%1)" ).arg( QString::fromUtf8( arr ) ).toUtf8().data() );
        return HTTP_BAD_REQUEST;
    }
    */
    return OK;
}

static int phi_handler( request_rec *r )
{
    if ( !r->handler || qstrcmp( r->handler, "phi" ) ) return DECLINED;
    PHIResponseRec *resp=new PHIResponseRec();
    apr_pool_cleanup_register( r->pool, static_cast<void*>(resp), cleanupResponseRec, apr_pool_cleanup_null );

    ApacheRequest areq( r, resp );
    if ( areq.keyword( PHISRequest::KMethod ) != "GET" && areq.keyword( PHISRequest::KMethod ) != "POST"
            && areq.keyword( PHISRequest::KMethod ) != "HEAD" ) return HTTP_METHOD_NOT_ALLOWED;

    //PHIParent::instance()->readLicenseFile( areq.documentRoot(), areq.hostname() );

    if ( areq.keyword( PHISRequest::KMethod ) == "POST" ) {
        int status=OK;
        if ( areq.keyword( PHISRequest::KContentType ).startsWith( "application/x-www-form-urlencoded" )
                && areq.contentLength() > 0 )
            status=parseUrlEncodedPostData( r, &areq );
        else if ( areq.keyword( PHISRequest::KContentType ).startsWith( "multipart/form-data" )
                && areq.contentLength() > 0 )
            status=parseMultipartPostData( r, &areq );
        if ( status!=OK ) return status;
    }
    areq.init();
#ifdef PHIDEBUG
    QTime t=QTime::currentTime();
#endif

    int dbId=PHISPageCache::getDbId();
    {
    PHIProcessor phiproc( &areq, dbId ); // process request (PHI engine)
    phiproc.run();
    // Print out any occured log entries
    PHIResponseRecLogEntry entry;
    foreach ( entry, resp->logEntries() ) {
        ap_log_rerror( APLOG_MARK, APLOG_WARNING, 0, r, "EC=%d <%s:%d> %s", static_cast<int>(entry._rc),
            entry._file, entry._line, entry._desc.toUtf8().constData() );
        ap_log_rerror( APLOG_MARK, APLOG_INFO, 0, r, "EC=%d [%s]: %s", static_cast<int>(entry._rc),
            PHIError::instance()->shortDesc( entry._rc ), PHIError::instance()->longDesc( entry._rc )
            .toUtf8().constData() );
    }
    if ( resp->options() & PHIResponseRec::Redirect ) {
        if ( !areq.setRedirectedFile( resp->fileName() ) ) {
            QString err=QObject::tr( "Redirected file '%1' not found." ).arg( resp->fileName() );
            resp->error( PHILOGERR, PHIRC_HTTP_NOT_FOUND, err );
        } else {
            PHIProcessor phiproc2( &areq, dbId );
            phiproc2.run();
            // Print out any occured log entries
            foreach ( entry, resp->logEntries() ) {
                ap_log_rerror( APLOG_MARK, APLOG_WARNING, 0, r, "EC=%d <%s:%d> %s", static_cast<int>(entry._rc),
                    entry._file, entry._line, entry._desc.toUtf8().constData() );
                ap_log_rerror( APLOG_MARK, APLOG_INFO, 0, r, "EC=%d [%s]: %s", static_cast<int>(entry._rc),
                    PHIError::instance()->shortDesc( entry._rc ), PHIError::instance()->longDesc( entry._rc )
                    .toUtf8().constData() );
            }
        }
    }
    }
    if ( QSqlDatabase::contains( QString::number( dbId ) ) ) {
        { // needed to delete db instance before removing
            QSqlDatabase db=QSqlDatabase::database( QString::number( dbId ) );
            if ( db.isValid() && db.isOpen() ) {
                db.close();
            }
        } // remove db instance
        qDebug( "Removing DB connection Id %d", dbId );
        QSqlDatabase::removeDatabase( QString::number( dbId ) );
    }
    PHISPageCache::removeDbId( dbId );


#ifdef PHIDEBUG
    qWarning( "Time elapsed: %d", t.msecsTo( QTime::currentTime() ) );
    areq.dump();
#endif

    QByteArray arr;
    foreach ( arr, resp->headersOut().keys() ) {
        if ( arr.startsWith( "Set-Cookie" ) ) continue;
        apr_table_set( r->headers_out, arr.constData(), resp->headersOut().value( arr ).constData() );
    }
    QList <QByteArray> setcookies=resp->headersOut().values( "Set-Cookie" );
    if ( setcookies.count()>0 ) {
        QByteArray cookies;
        foreach ( arr, setcookies ) cookies+=arr+", ";
        cookies.chop( 2 ); // remove trailing comma
        apr_table_add( r->headers_out, "Set-Cookie", cookies.constData() );
    }
    ap_set_content_type( r, resp->contentType().constData() );
    ap_set_content_length( r, resp->contentLength() );

    if ( resp->rc()!=PHIRC_OK ) {
        int rc=translateReturnCode( resp->rc() );
        if ( rc==DECLINED ) return DECLINED;
        qDebug( "rc=%d", rc );
        if ( rc==HTTP_NOT_MODIFIED ) {
            if ( resp->options() & PHIResponseRec::SendFile ) {
                QFileInfo fi( resp->fileName() );
                QByteArray type=PHI::mimeType( fi.suffix().toUtf8() );
                if ( type.isEmpty() ) {
                    PHIRC rcm=PHIRC_HTTP_UNSUPPORTED_MEDIA_TYPE;
                    ap_log_rerror( APLOG_MARK, APLOG_ERR, 0, r, "RC=%d <%s:%d> Unknown media type '%s'.",
                        static_cast<int>(rcm), __FILE__, __LINE__, fi.suffix().toUtf8().constData() );
                    ap_log_rerror( APLOG_MARK, APLOG_INFO, 0, r, "RC=%d [%s]: %s", static_cast<int>(rcm),
                        PHIError::instance()->shortDesc( rcm ), PHIError::instance()->longDesc( rcm )
                        .toUtf8().constData() );
                    return translateReturnCode( rcm );
                }
                ap_set_content_type( r, type );
            }
            return HTTP_NOT_MODIFIED;
        }
        ap_log_rerror( APLOG_MARK, APLOG_ERR, 0, r, "RC=%d <%s:%d> %s",
            static_cast<int>(resp->rc()), resp->errorFile(), resp->errorLine(), resp->errorDesc().toUtf8().constData() );
        ap_log_rerror( APLOG_MARK, APLOG_INFO, 0, r, "RC=%d [%s]: %s", static_cast<int>(resp->rc()),
            PHIError::instance()->shortDesc( resp->rc() ), PHIError::instance()->longDesc( resp->rc() )
            .toUtf8().constData() );
        return rc;
    }
    if ( resp->options() & PHIResponseRec::HeaderOnly ) return OK;
    if ( resp->options() & PHIResponseRec::SendFile ) {
        QFileInfo fi( resp->fileName() );
        QByteArray type=PHI::mimeType( fi.suffix().toUtf8() );
        if ( type.isEmpty() ) {
            PHIRC rcm=PHIRC_HTTP_UNSUPPORTED_MEDIA_TYPE;
            ap_log_rerror( APLOG_MARK, APLOG_ERR, 0, r, "RC=%d <%s:%d> Unknown media type '%s'.",
                static_cast<int>(rcm), __FILE__, __LINE__, fi.suffix().toUtf8().constData() );
            ap_log_rerror( APLOG_MARK, APLOG_INFO, 0, r, "RC=%d [%s]: %s",
                static_cast<int>(rcm), PHIError::instance()->shortDesc( rcm ),
                PHIError::instance()->longDesc( rcm ).toUtf8().constData() );
            return translateReturnCode(rcm);
        }
        ap_set_content_type( r, type.constData() );
        apr_file_t *fd;
        apr_status_t rv;
        rv=apr_file_open( &fd, resp->fileName().toUtf8().constData(),
            APR_READ | APR_SHARELOCK | APR_SENDFILE_ENABLED, APR_OS_DEFAULT, r->pool );
        if ( rv!=APR_SUCCESS ) {
            PHIRC rcm=PHIRC_HTTP_NOT_FOUND;
            ap_log_rerror( APLOG_MARK, APLOG_ERR, 0, r, "RC=%d <%s:%d> Can not open '%s'.",
                static_cast<int>(rcm), __FILE__, __LINE__, resp->fileName().toUtf8().constData() );
            ap_log_rerror( APLOG_MARK, APLOG_INFO, 0, r, "RC=%d [%s]: %s", static_cast<int>(rcm),
                PHIError::instance()->shortDesc( rcm ),
                PHIError::instance()->longDesc( rcm ).toUtf8().constData() );
            return translateReturnCode(rcm);
        }
        apr_size_t size=static_cast<apr_size_t>(resp->contentLength());
        apr_size_t offset(0), bytes_written(0);
        forever {
            rv=ap_send_fd( fd, r, offset, size, &bytes_written );
            offset+=bytes_written;
            if ( size==bytes_written ) break;
            if ( rv!=APR_SUCCESS ) {
                PHIRC rcm=PHIRC_HTTP_INTERNAL_SERVER_ERROR;
                ap_log_rerror( APLOG_MARK, APLOG_ERR, 0, r, "RC=%d <%s:%d> Could not send '%s'.",
                    static_cast<int>(rcm), __FILE__, __LINE__, resp->fileName().toUtf8().constData() );
                ap_log_rerror( APLOG_MARK, APLOG_INFO, 0, r, "RC=%d [%s]: %s", static_cast<int>(rcm),
                    PHIError::instance()->shortDesc( rcm ),
                    PHIError::instance()->longDesc( rcm ).toUtf8().constData() );
                apr_file_close( fd );
                return translateReturnCode( rcm );
            }
        }
        apr_file_close( fd );
    } else {
        QByteArray body=resp->body();
        int size=body.size();
        int bytes_written=0;
        while ( size ) {
            bytes_written=ap_rwrite( body.constData(), size, r );
            if ( bytes_written==-1 ) {
                PHIRC rcm=PHIRC_HTTP_INTERNAL_SERVER_ERROR;
                ap_log_rerror( APLOG_MARK, APLOG_ERR, 0, r, "RC=%d <%s:%d> Could not send data to the client.",
                    static_cast<int>(rcm), __FILE__, __LINE__);
                ap_log_rerror( APLOG_MARK, APLOG_INFO, 0, r, "RC=%d [%s]: %s", static_cast<int>(rcm),
                    PHIError::instance()->shortDesc( rcm ),
                    PHIError::instance()->longDesc( rcm ).toUtf8().constData() );
                ap_rflush( r );
                return HTTP_RESET_CONTENT;
            }
            if ( size==bytes_written ) break;
            size-=bytes_written;
            body.remove( 0, bytes_written );
        }
        ap_rflush( r );
    }
    return OK;
}

/*  // Used for ap_hook_child_init
static void global_init( apr_pool_t *p, server_rec* )
{
    //qWarning( "--- phi --- global_init" );
    PHIParent *parent=PHIParent::instance();
    QString fileName="dummy";
    PHI::createMimeTypes( fileName );
    apr_pool_cleanup_register( p, static_cast<void*>(parent), cleanupPHIParent, apr_pool_cleanup_null );
}
*/

static int global_init( apr_pool_t *p, apr_pool_t *plog, apr_pool_t *ptemp, server_rec *s )
{
    Q_UNUSED( plog );
    Q_UNUSED( ptemp );
    Q_UNUSED( s );
    //qWarning( "--- phi --- global_init" );

    PHIParent *parent=PHIParent::instance();
    /** @todo Make mime type filename configurable. */
    PHI::createMimeTypes( QStringLiteral( "dummy" ) );
    apr_pool_cleanup_register( p, static_cast<void*>(parent), cleanupPHIParent, apr_pool_cleanup_null );
    QString err;
    foreach ( err, parent->moduleLoadErrors() ) {
        err.replace( '\n', ' ' );
    }

    return 0;
}

static void phi_register_hooks( apr_pool_t* )
{
    //ap_hook_child_init( global_init, NULL, NULL, APR_HOOK_MIDDLE );
    ap_hook_post_config( global_init, NULL, NULL, APR_HOOK_MIDDLE );
    ap_hook_handler( phi_handler, NULL, NULL, APR_HOOK_MIDDLE );
}

/* Dispatch list for API hooks */
module AP_MODULE_DECLARE_DATA phi_module = {
    STANDARD20_MODULE_STUFF,
    NULL,                  /* create per-dir    config structures */
    NULL,                  /* merge  per-dir    config structures */
    NULL,                  /* create per-server config structures */
    NULL,                  /* merge  per-server config structures */
    NULL,                  /* table of config file commands       */
    phi_register_hooks     /* register hooks                      */
};

#ifdef __cplusplus
}
#endif
