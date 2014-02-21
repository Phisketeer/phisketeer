/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2014  Phisys AG, Switzerland
#    Copyright (C) 2012-2014  Phisketeer.org team
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
#ifndef PHIERROR_H
#define PHIERROR_H

#include <QHash>
#include <QByteArray>
#include <QDateTime>
#include <QString>
#include <QObject>

#ifdef PHILIB
#define PHIEXPORT Q_DECL_EXPORT
#else
#define PHIEXPORT Q_DECL_IMPORT
#endif

#define PHILOGTRACE 0x01,__FILE__,__LINE__,QDateTime::currentDateTime()
#define PHILOGERR   0x02,__FILE__,__LINE__,QDateTime::currentDateTime()
#define PHILOGWARN  0x04,__FILE__,__LINE__,QDateTime::currentDateTime()
#define PHILOGCRIT  0x08,__FILE__,__LINE__,QDateTime::currentDateTime()
#define PHILOGDBUG  0x10,__FILE__,__LINE__,QDateTime::currentDateTime()
#define PHILOGUSER  0x20,__FILE__,__LINE__,QDateTime::currentDateTime()

typedef quint16 PHIRC;

/* All successful completed operations return PHIRC_OK. */
const PHIRC PHIRC_OK=0;
const PHIRC PHIRC_SUCCESS=0;
const PHIRC PHIRC_FAILED=1;
const PHIRC PHIRC_ERROR=1;
const PHIRC PHIRC_DECLINED=2;
const PHIRC PHIRC_USER=3;

/* file IO */
const PHIRC PHIRC_IO_FILE_CREATION_ERROR=10;
const PHIRC PHIRC_IO_FILE_ACCESS_ERROR=11;
const PHIRC PHIRC_IO_STREAM_ERROR=12;
const PHIRC PHIRC_IO_FORMAT_ERROR=13;

/* phis manager */
const PHIRC PHIRC_MGR_START=20;
const PHIRC PHIRC_MGR_STOP=21;
const PHIRC PHIRC_MGR_INIT_ERROR=22;
const PHIRC PHIRC_MGR_NAME_ERROR=23;
const PHIRC PHIRC_MGR_INTERNAL_ERROR=24;

/* objects */
const PHIRC PHIRC_OBJ_EXISTS_ERROR=40;
const PHIRC PHIRC_OBJ_NOT_FOUND_ERROR=41;
const PHIRC PHIRC_OBJ_MAX_COUNT_ERROR=42;
const PHIRC PHIRC_OBJ_ACCESS_ERROR=43;
const PHIRC PHIRC_OBJ_IN_USE_ERROR=44;
const PHIRC PHIRC_OBJ_HANDLE_ERROR=45;
const PHIRC PHIRC_OBJ_TYPE_ERROR=46;
const PHIRC PHIRC_OBJ_HIGH_EVENT=47;
const PHIRC PHIRC_OBJ_LOW_EVENT=48;
const PHIRC PHIRC_OBJ_FULL_EVENT=49;

/* connection */
const PHIRC PHIRC_TCP_IP_ADDRESS_ERROR=60;
const PHIRC PHIRC_TCP_LISTENER_START=61;
const PHIRC PHIRC_TCP_CONNECTION_REFUSED=62;
const PHIRC PHIRC_TCP_UNKNOWN_SOCKET_ERROR=63;
const PHIRC PHIRC_TCP_REMOTE_HOST_CLOSED=64;
const PHIRC PHIRC_TCP_SOCKET_TIMEOUT_ERROR=65;
const PHIRC PHIRC_TCP_NETWORK_ERROR=66;
const PHIRC PHIRC_TCP_HOST_NOT_FOUND_ERROR=67;
const PHIRC PHIRC_TCP_CONNECTION_BROKEN=68;
const PHIRC PHIRC_TCP_SSL_ERROR=69;

/* protocol */
const PHIRC PHIRC_PROT_UNKNOWN_CMD=30;
const PHIRC PHIRC_PROT_CMD_ERROR=31;

/* modules */
const PHIRC PHIRC_MODULE_LOAD_ERROR=35;
const PHIRC PHIRC_MODULE_LOG=36;
const PHIRC PHIRC_MODULE_DEPRECATED=37;

const PHIRC PHIRC_MSG_NOT_FOUND=50;
const PHIRC PHIRC_MSG_CODEC_ERROR=51;
const PHIRC PHIRC_MSG_CRC16_ERROR=52;
const PHIRC PHIRC_MSG_EXPIRED=53;

/* misc */
const PHIRC PHIRC_OUT_OF_MEMORY=80;
const PHIRC PHIRC_LOG_INIT_ERROR=81;
const PHIRC PHIRC_ARGUMENT_ERROR=82;
const PHIRC PHIRC_BUFFER_ERROR=83;
const PHIRC PHIRC_LOCALE_ERROR=84;
const PHIRC PHIRC_SCRIPT_PARSE_ERROR=85;
const PHIRC PHIRC_NOT_IMPL_ERROR=86;
const PHIRC PHIRC_USER_ABORT_ERROR=87;
const PHIRC PHIRC_TRIGGER_ERROR=88;
const PHIRC PHIRC_DB_ERROR=89;
const PHIRC PHIRC_TRANSACTION_ERROR=90;
const PHIRC PHIRC_QUERY_ERROR=91;

/* HTTP */
const PHIRC PHIRC_HTTP_CONTINUE=100;
const PHIRC PHIRC_HTTP_SWITCH_PROTOCOL=101;

const PHIRC PHIRC_HTTP_OK=200;
const PHIRC PHIRC_HTTP_CREATED=201;
const PHIRC PHIRC_HTTP_ACCEPTED=202;
const PHIRC PHIRC_HTTP_NON_AUTHORITATIVE=203;
const PHIRC PHIRC_HTTP_NO_CONTENT=204;
const PHIRC PHIRC_HTTP_RESET_CONTENT=205;
const PHIRC PHIRC_HTTP_PARTIAL_CONTENT=206;

const PHIRC PHIRC_HTTP_MULTIPLE_CHOICES=300;
const PHIRC PHIRC_HTTP_MOVED_PERMANENTLY=301;
const PHIRC PHIRC_HTTP_FOUND=302;
const PHIRC PHIRC_HTTP_SEE_OTHER=303;
const PHIRC PHIRC_HTTP_NOT_MODIFIED=304;
const PHIRC PHIRC_HTTP_USE_PROXY=305;
const PHIRC PHIRC_HTTP_UNUSED=306;
const PHIRC PHIRC_HTTP_TMP_REDIRECT=307;

const PHIRC PHIRC_HTTP_BAD_REQUEST=400;
const PHIRC PHIRC_HTTP_UNAUTHORIZED=401;
const PHIRC PHIRC_HTTP_PAYMENT_REQUIRED=402;
const PHIRC PHIRC_HTTP_FORBIDDEN=403;
const PHIRC PHIRC_HTTP_NOT_FOUND=404;
const PHIRC PHIRC_HTTP_METHOD_NOT_ALLOWED=405;
const PHIRC PHIRC_HTTP_NOT_ACCEPTABLE=406;
const PHIRC PHIRC_HTTP_PROXY_AUTHENTICATION_RQUIRED=407;
const PHIRC PHIRC_HTTP_REQUEST_TIMEOUT=408;
const PHIRC PHIRC_HTTP_CONFLICT=409;
const PHIRC PHIRC_HTTP_GONE=410;
const PHIRC PHIRC_HTTP_LENGTH_REQUIRED=411;
const PHIRC PHIRC_HTTP_PRECONDITION_FAILED=412;
const PHIRC PHIRC_HTTP_REQUEST_ENTITY_TOO_LARGE=413;
const PHIRC PHIRC_HTTP_REQUEST_URI_TOO_LONG=414;
const PHIRC PHIRC_HTTP_UNSUPPORTED_MEDIA_TYPE=415;
const PHIRC PHIRC_HTTP_REQUESTED_RANGE_NOT_SATISFIABLE=416;
const PHIRC PHIRC_HTTP_EXPECTATION_FAILED=417;

const PHIRC PHIRC_HTTP_INTERNAL_SERVER_ERROR=500;
const PHIRC PHIRC_HTTP_NOT_IMPLEMENTED=501;
const PHIRC PHIRC_HTTP_BAD_GATEWAY=502;
const PHIRC PHIRC_HTTP_SERVICE_UNAVAILABLE=503;
const PHIRC PHIRC_HTTP_GATEWAY_TIMEOUT=504;
const PHIRC PHIRC_HTTP_VERSION_NOT_SUPPORTED=505;

class PHIEXPORT PHIError : public QObject
{
    Q_OBJECT

public:
    static PHIError* instance( QObject *parent=0 );
    void print( PHIRC rc, const QString &todo=QString() ) const;
    // No translations for short description:
    inline const char* shortDesc( PHIRC rc ) const {
        if ( _sl.contains( rc ) ) return _sl[rc].constData();
        return QT_TR_NOOP( "Unknown reason code." );
    }
    inline QString longDesc( PHIRC rc ) const { return _ll[rc]; }
    static void printConsole( const QString &s );

protected:
    explicit PHIError( QObject *parent );
    virtual ~PHIError();
    void initLongDesc();
    void initShortDesc();

private:
    static PHIError *_instance;
    QHash <int, QString> _ll;
    QHash <int, QByteArray> _sl;
};

#endif // PHIERROR_H
