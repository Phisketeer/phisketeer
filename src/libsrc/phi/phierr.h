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
#ifndef PHIERR_H
#define PHIERR_H
/** @file phierr.h Defines all reason codes for PHI.
 * Include this file, if you want to check special RCs back or
 * want to print detailed error messages in C and C++ client programs.\n
 * All codes other then PHIRC_OK indicates an error @b or warning.
 * Some are used for tracing like PHIRC_MGR_START and will not be
 * returned from public functions. */

#include "phi.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup reason_codes PHIRC reason codes
 * @{ */

/** All successful completed operations return PHIRC_OK. */
const PHIRC PHIRC_OK=0;
/** Same as PHIRC_OK. */
const PHIRC PHIRC_SUCCESS=0;
/** If the reason can not be determined correctly, PHIRC_FAILED is returned. */
const PHIRC PHIRC_FAILED=1;
/** Same as PHIRC_FAILED. */
const PHIRC PHIRC_ERROR=1;
/** Declined code for Apache. */
const PHIRC PHIRC_DECLINED=2;
/** User log. */
const PHIRC PHIRC_USER=3;

/* file IO */
/** Files needed by PHI could not be created. */
const PHIRC PHIRC_IO_FILE_CREATION_ERROR=10;
/** File could not be accessed. */
const PHIRC PHIRC_IO_FILE_ACCESS_ERROR=11;
/** Stream could not be read correctly. */
const PHIRC PHIRC_IO_STREAM_ERROR=12;
/** IO format error. */
const PHIRC PHIRC_IO_FORMAT_ERROR=13;

/* qmanager */
/** Trace RC - does not indicate any error. */
const PHIRC PHIRC_MGR_START=20;
/** Trace RC - does not indicate any error. */
const PHIRC PHIRC_MGR_STOP=21;
/** Initialisation error - qmgr could not be started. */
const PHIRC PHIRC_MGR_INIT_ERROR=22;
/** Daemon is start with wrong name, or PHICNO is not set correctly. */
const PHIRC PHIRC_MGR_NAME_ERROR=23;
/** QMGR internal error occured. */
const PHIRC PHIRC_MGR_INTERNAL_ERROR=24;

/* objects */
/** Object with this name already exists. */
const PHIRC PHIRC_OBJ_EXISTS_ERROR=40;
/** Object with this name could not be found. */
const PHIRC PHIRC_OBJ_NOT_FOUND_ERROR=41;
/** A limit has been reached for this opject. */
const PHIRC PHIRC_OBJ_MAX_COUNT_ERROR=42;
/** Access violation for this object. */
const PHIRC PHIRC_OBJ_ACCESS_ERROR=43;
/** Object is already in use. */
const PHIRC PHIRC_OBJ_IN_USE_ERROR=44;
/** Handle for this object is or became invalid. */
const PHIRC PHIRC_OBJ_HANDLE_ERROR=45;
/** Type of object could not be determined. */
const PHIRC PHIRC_OBJ_TYPE_ERROR=46;
/** Object high event. */
const PHIRC PHIRC_OBJ_HIGH_EVENT=47;
/** Object low event. */
const PHIRC PHIRC_OBJ_LOW_EVENT=48;
/** Object full event. */
const PHIRC PHIRC_OBJ_FULL_EVENT=49;

/* connection */
/** TCP/IP address has incorrect format, or port is wrong. */
const PHIRC PHIRC_TCP_IP_ADDRESS_ERROR=60;
/** TCP/IP listener is started (trace only). */
const PHIRC PHIRC_TCP_LISTENER_START=61;
/** The connection was refused by the peer. */
const PHIRC PHIRC_TCP_CONNECTION_REFUSED=62;
/** Undefined TCP socket error. */
const PHIRC PHIRC_TCP_UNKNOWN_SOCKET_ERROR=63;
/** The remote host closed the connection unexpectedly. */
const PHIRC PHIRC_TCP_REMOTE_HOST_CLOSED=64;
/** The TCP socket timedout. */
const PHIRC PHIRC_TCP_SOCKET_TIMEOUT_ERROR=65;
/** A network error occured. */
const PHIRC PHIRC_TCP_NETWORK_ERROR=66;
/** The remote hostname could not be resolved. */
const PHIRC PHIRC_TCP_HOST_NOT_FOUND_ERROR=67;
/** Connection broken. */
const PHIRC PHIRC_TCP_CONNECTION_BROKEN=68;
/** SSL error. */
const PHIRC PHIRC_TCP_SSL_ERROR=69;

/* protocol */
/** Server or client received an unknown or unexpected command. */
const PHIRC PHIRC_PROT_UNKNOWN_CMD=30;
/** The command produced an error. */
const PHIRC PHIRC_PROT_CMD_ERROR=31;

/* modules */
/** Could not load a phis module. */
const PHIRC PHIRC_MODULE_LOAD_ERROR=35;
/** Log message from a module. */
const PHIRC PHIRC_MODULE_LOG=36;
/** Old static module style. */
const PHIRC PHIRC_MODULE_DEPRECATED=37;

/* message related */
/** Queue is empty or no message matches any criteria. */
const PHIRC PHIRC_MSG_NOT_FOUND=50;
/** Codec or MIB for the message could not be resolved. */
const PHIRC PHIRC_MSG_CODEC_ERROR=51;
/** The CRC16 checksum over the message missmatched. */
const PHIRC PHIRC_MSG_CRC16_ERROR=52;
/** A message was found but has expired. */
const PHIRC PHIRC_MSG_EXPIRED=53;

/* misc */
/** Operation failed due to memory shortage. */
const PHIRC PHIRC_OUT_OF_MEMORY=80;
/** The log writer could not be initalized. */
const PHIRC PHIRC_LOG_INIT_ERROR=81;
/** Arguments are missused. */
const PHIRC PHIRC_ARGUMENT_ERROR=82;
/** A buffer could not referenced or is too small. */
const PHIRC PHIRC_BUFFER_ERROR=83;
/** Locale setting error. */
const PHIRC PHIRC_LOCALE_ERROR=84;
/** Script parsing error. */
const PHIRC PHIRC_SCRIPT_PARSE_ERROR=85;
/** Requested function is not implemented yet. */
const PHIRC PHIRC_NOT_IMPL_ERROR=86;
/** User aborted request. */
const PHIRC PHIRC_USER_ABORT_ERROR=87;
/** Triggered process error. */
const PHIRC PHIRC_TRIGGER_ERROR=88;
/** Database error. */
const PHIRC PHIRC_DB_ERROR=89;
/** Transaction error. */
const PHIRC PHIRC_TRANSACTION_ERROR=90;
/** Query error. */
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

/** @} */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PHIERR_H */
