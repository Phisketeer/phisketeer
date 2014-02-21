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
#include <QObject>
#include "phierror.h"

void PHIError::initShortDesc()
{
    _sl[PHIRC_OK]="OK";
    _sl[PHIRC_FAILED]="FAILED";
    _sl[PHIRC_DECLINED]="DECLINED";
    _sl[PHIRC_USER]="USER";

    _sl[PHIRC_IO_FILE_CREATION_ERROR]="IO_FILE_CREATION_ERROR";
    _sl[PHIRC_IO_FILE_ACCESS_ERROR]="IO_FILE_ACCESS_ERROR";
    _sl[PHIRC_IO_STREAM_ERROR]="IO_STREAM_ERROR";
    _sl[PHIRC_IO_FORMAT_ERROR]="IO_FORMAT_ERROR";

    _sl[PHIRC_MGR_START]="MGR_START";
    _sl[PHIRC_MGR_STOP]="MGR_STOP";
    _sl[PHIRC_MGR_INIT_ERROR]="MGR_INIT_ERROR";
    _sl[PHIRC_MGR_NAME_ERROR]="MGR_NAME_ERROR";
    _sl[PHIRC_MGR_INTERNAL_ERROR]="MGR_INTERNAL_ERROR";

    _sl[PHIRC_OBJ_EXISTS_ERROR]="OBJ_EXISTS_ERROR";
    _sl[PHIRC_OBJ_NOT_FOUND_ERROR]="OBJ_NOT_FOUND_ERROR";
    _sl[PHIRC_OBJ_MAX_COUNT_ERROR]="OBJ_MAX_COUNT_ERROR";
    _sl[PHIRC_OBJ_ACCESS_ERROR]="OBJ_ACCESS_ERROR";
    _sl[PHIRC_OBJ_IN_USE_ERROR]="OBJ_IN_USE_ERROR";
    _sl[PHIRC_OBJ_HANDLE_ERROR]="OBJ_HANDLE_ERROR";
    _sl[PHIRC_OBJ_TYPE_ERROR]="OBJ_TYPE_ERROR";
    _sl[PHIRC_OBJ_HIGH_EVENT]="OBJ_HIGH_EVENT";
    _sl[PHIRC_OBJ_LOW_EVENT]="OBJ_LOW_EVENT";
    _sl[PHIRC_OBJ_FULL_EVENT]="OBJ_FULL_EVENT";

    _sl[PHIRC_TCP_IP_ADDRESS_ERROR]="TCP_IP_ADDRESS_ERROR";
    _sl[PHIRC_TCP_LISTENER_START]="TCP_LISTENER_START";
    _sl[PHIRC_TCP_CONNECTION_REFUSED]="TCP_CONNECTION_REFUSED";
    _sl[PHIRC_TCP_UNKNOWN_SOCKET_ERROR]="TCP_UNKNOWN_SOCKET_ERROR";
    _sl[PHIRC_TCP_REMOTE_HOST_CLOSED]="TCP_REMOTE_HOST_CLOSED";
    _sl[PHIRC_TCP_SOCKET_TIMEOUT_ERROR]="TCP_SOCKET_TIMEOUT_ERROR";
    _sl[PHIRC_TCP_NETWORK_ERROR]="TCP_NETWORK_ERROR";
    _sl[PHIRC_TCP_HOST_NOT_FOUND_ERROR]="TCP_HOST_NOT_FOUND_ERROR";
    _sl[PHIRC_TCP_CONNECTION_BROKEN]="TCP_CONNECTION_BROKEN";
    _sl[PHIRC_TCP_SSL_ERROR]="TCP_SSL_ERROR";

    _sl[PHIRC_PROT_UNKNOWN_CMD]="PROT_UNKNOWN_CMD";
    _sl[PHIRC_PROT_CMD_ERROR]="PROT_CMD_ERROR";

    _sl[PHIRC_MSG_NOT_FOUND]="MSG_NOT_FOUND";
    _sl[PHIRC_MSG_CODEC_ERROR]="MSG_CODEC_ERROR";
    _sl[PHIRC_MSG_CRC16_ERROR]="MSG_CRC16_ERROR";
    _sl[PHIRC_MSG_EXPIRED]="MSG_EXPIRED";

    _sl[PHIRC_OUT_OF_MEMORY]="OUT_OF_MEMORY";
    _sl[PHIRC_LOG_INIT_ERROR]="LOG_INIT_ERROR";
    _sl[PHIRC_ARGUMENT_ERROR]="ARGUMENT_ERROR";
    _sl[PHIRC_BUFFER_ERROR]="BUFFER_ERROR";
    _sl[PHIRC_LOCALE_ERROR]="LOCALE_ERROR";
    _sl[PHIRC_SCRIPT_PARSE_ERROR]="SCRIPT_PARSE_ERROR";
    _sl[PHIRC_NOT_IMPL_ERROR]="NOT_IMPL_ERROR";
    _sl[PHIRC_USER_ABORT_ERROR]="USER_ABORT_ERROR";
    _sl[PHIRC_TRIGGER_ERROR]="TRIGGER_ERROR";
    _sl[PHIRC_DB_ERROR]="DB_ERROR";
    _sl[PHIRC_TRANSACTION_ERROR]="TRANSACTION_ERROR";
    _sl[PHIRC_QUERY_ERROR]="QUERY_ERROR";

    _sl[PHIRC_MODULE_LOAD_ERROR]="MODULE_LOAD_ERROR";
    _sl[PHIRC_MODULE_LOG]="MODULE_LOG";
    _sl[PHIRC_MODULE_DEPRECATED]="MODULE_DEPRECATED";
    _sl[PHIRC_HTTP_CONTINUE]="HTTP_CONTINUE";
    _sl[PHIRC_HTTP_SWITCH_PROTOCOL]="HTTP_SWITCH_PROTOCO";

    _sl[PHIRC_HTTP_OK]="HTTP_OK";
    _sl[PHIRC_HTTP_CREATED]="HTTP_CREATED";
    _sl[PHIRC_HTTP_ACCEPTED]="HTTP_ACCEPTED";
    _sl[PHIRC_HTTP_NON_AUTHORITATIVE]="HTTP_NON_AUTHORITATIVE";
    _sl[PHIRC_HTTP_NO_CONTENT]="HTTP_NO_CONTENT";
    _sl[PHIRC_HTTP_RESET_CONTENT]="RESET_CONTENT";
    _sl[PHIRC_HTTP_PARTIAL_CONTENT]="PARTIAL_CONTENT";

    _sl[PHIRC_HTTP_MULTIPLE_CHOICES]="HTTP_MULTIPLE_CHOICES";
    _sl[PHIRC_HTTP_MOVED_PERMANENTLY]="HTTP_MOVED_PERMANENTLY";
    _sl[PHIRC_HTTP_FOUND]="HTTP_FOUND";
    _sl[PHIRC_HTTP_SEE_OTHER]="HTTP_SEE_OTHER";
    _sl[PHIRC_HTTP_NOT_MODIFIED]="HTTP_NOT_MODIFIED";
    _sl[PHIRC_HTTP_USE_PROXY]="HTTP_USE_PROXY";
    _sl[PHIRC_HTTP_UNUSED]="HTTP_UNUSED";
    _sl[PHIRC_HTTP_TMP_REDIRECT]="HTTP_TMP_REDIRECT";

    _sl[PHIRC_HTTP_BAD_REQUEST]="HTTP_BAD_REQUEST";
    _sl[PHIRC_HTTP_UNAUTHORIZED]="HTTP_UNAUTHORIZED";
    _sl[PHIRC_HTTP_PAYMENT_REQUIRED]="HTTP_PAYMENT_REQUIRED";
    _sl[PHIRC_HTTP_FORBIDDEN]="HTTP_FORBIDDEN";
    _sl[PHIRC_HTTP_NOT_FOUND]="HTTP_NOT_FOUND";
    _sl[PHIRC_HTTP_METHOD_NOT_ALLOWED]="HTTP_METHOD_NOT_ALLOWED";
    _sl[PHIRC_HTTP_NOT_ACCEPTABLE]="HTTP_NOT_ACCEPTABLE";
    _sl[PHIRC_HTTP_PROXY_AUTHENTICATION_RQUIRED]="HTTP_PROXY_AUTHENTICATION_RQUIRED";
    _sl[PHIRC_HTTP_REQUEST_TIMEOUT]="HTTP_REQUEST_TIMEOUT";
    _sl[PHIRC_HTTP_CONFLICT]="HTTP_CONFLICT";
    _sl[PHIRC_HTTP_GONE]="HTTP_GONE";
    _sl[PHIRC_HTTP_LENGTH_REQUIRED]="HTTP_LENGTH_REQUIRED";
    _sl[PHIRC_HTTP_PRECONDITION_FAILED]="HTTP_PRECONDITION_FAILED";
    _sl[PHIRC_HTTP_REQUEST_ENTITY_TOO_LARGE]="HTTP_REQUEST_ENTITY_TOO_LARGE";
    _sl[PHIRC_HTTP_REQUEST_URI_TOO_LONG]="HTTP_REQUEST_URI_TOO_LONG";
    _sl[PHIRC_HTTP_UNSUPPORTED_MEDIA_TYPE]="HTTP_UNSUPPORTED_MEDIA_TYPE";
    _sl[PHIRC_HTTP_REQUESTED_RANGE_NOT_SATISFIABLE]="HTTP_REQUESTED_RANGE_NOT_SATISFIABLE";
    _sl[PHIRC_HTTP_EXPECTATION_FAILED]="HTTP_EXPECTATION_FAILED";

    _sl[PHIRC_HTTP_INTERNAL_SERVER_ERROR]="HTTP_INTERNAL_SERVER_ERROR";
    _sl[PHIRC_HTTP_NOT_IMPLEMENTED]="HTTP_NOT_IMPLEMENTED";
    _sl[PHIRC_HTTP_BAD_GATEWAY]="HTTP_BAD_GATEWAY";
    _sl[PHIRC_HTTP_SERVICE_UNAVAILABLE]="HTTP_SERVICE_UNAVAILABLE";
    _sl[PHIRC_HTTP_GATEWAY_TIMEOUT]="HTTP_GATEWAY_TIMEOUT";
    _sl[PHIRC_HTTP_VERSION_NOT_SUPPORTED]="HTTP_VERSION_NOT_SUPPORTED";
}

void PHIError::initLongDesc()
{
    _ll[PHIRC_OK]=QStringLiteral( QT_TR_NOOP( "Success." ) );
    _ll[PHIRC_FAILED]=QStringLiteral( QT_TR_NOOP( "Failure." ) );
    _ll[PHIRC_DECLINED]=QStringLiteral( QT_TR_NOOP( "Declined." ) );
    _ll[PHIRC_USER]=QStringLiteral( QT_TR_NOOP( "User log." ) );

    _ll[PHIRC_IO_FILE_CREATION_ERROR]=QStringLiteral( QT_TR_NOOP( "Could not create a file or directory." ) );
    _ll[PHIRC_IO_FILE_ACCESS_ERROR]=QStringLiteral( QT_TR_NOOP( "Could not access a file. Permission denied, or file not found." ) );
    _ll[PHIRC_IO_STREAM_ERROR]=QStringLiteral( QT_TR_NOOP( "Could not read stream correctly." ) );
    _ll[PHIRC_IO_FORMAT_ERROR]=QStringLiteral( QT_TR_NOOP( "File format error or wrong format version." ) );

    _ll[PHIRC_MGR_START]=QStringLiteral( QT_TR_NOOP( "Server start." ) );
    _ll[PHIRC_MGR_STOP]=QStringLiteral( QT_TR_NOOP( "Server stop." ) );
    _ll[PHIRC_MGR_INIT_ERROR]=QStringLiteral( QT_TR_NOOP( "Server initalisation failure." ) );
    _ll[PHIRC_MGR_NAME_ERROR]=QStringLiteral( QT_TR_NOOP( "The server name could not be resolved." ) );
    _ll[PHIRC_MGR_INTERNAL_ERROR]=QStringLiteral( QT_TR_NOOP( "An internal error occured." ) );

    _ll[PHIRC_OBJ_EXISTS_ERROR]=QStringLiteral( QT_TR_NOOP( "Object already exists." ) );
    _ll[PHIRC_OBJ_NOT_FOUND_ERROR]=QStringLiteral( QT_TR_NOOP( "Object not found." ) );
    _ll[PHIRC_OBJ_MAX_COUNT_ERROR]=QStringLiteral( QT_TR_NOOP( "Objects reached max. allowed count." ) );
    _ll[PHIRC_OBJ_ACCESS_ERROR]=QStringLiteral( QT_TR_NOOP( "Operation on object was rejected." ) );
    _ll[PHIRC_OBJ_IN_USE_ERROR]=QStringLiteral( QT_TR_NOOP( "Operation on object denied. Object is in use." ) );
    _ll[PHIRC_OBJ_HANDLE_ERROR]=QStringLiteral( QT_TR_NOOP( "A handle is not valid or is not found." ) );
    _ll[PHIRC_OBJ_TYPE_ERROR]=QStringLiteral( QT_TR_NOOP( "The object type is incorrectly used or type does not exist." ) );
    _ll[PHIRC_OBJ_HIGH_EVENT]=QStringLiteral( QT_TR_NOOP( "The queue depth reached the high event mark." ) );
    _ll[PHIRC_OBJ_LOW_EVENT]=QStringLiteral( QT_TR_NOOP( "The queue depth reached the low event mark." ) );
    _ll[PHIRC_OBJ_FULL_EVENT]=QStringLiteral( QT_TR_NOOP( "The queue is full." ) );

    _ll[PHIRC_TCP_IP_ADDRESS_ERROR]=QStringLiteral( QT_TR_NOOP( "Wrong configured IP address." ) );
    _ll[PHIRC_TCP_LISTENER_START]=QStringLiteral( QT_TR_NOOP( "TCP listener started." ) );
    _ll[PHIRC_TCP_CONNECTION_REFUSED]=QStringLiteral( QT_TR_NOOP( "Connection was refused by peer." ) );
    _ll[PHIRC_TCP_UNKNOWN_SOCKET_ERROR]=QStringLiteral( QT_TR_NOOP( "Unknown error occured on a tcp socket." ) );
    _ll[PHIRC_TCP_REMOTE_HOST_CLOSED]=QStringLiteral( QT_TR_NOOP( "The remote peer closed the connection." ) );
    _ll[PHIRC_TCP_SOCKET_TIMEOUT_ERROR]=QStringLiteral( QT_TR_NOOP( "A tcp socket timedout." ) );
    _ll[PHIRC_TCP_NETWORK_ERROR]=QStringLiteral( QT_TR_NOOP( "A network error occured." ) );
    _ll[PHIRC_TCP_HOST_NOT_FOUND_ERROR]=QStringLiteral( QT_TR_NOOP( "The remote host could not be resolved." ) );
    _ll[PHIRC_TCP_CONNECTION_BROKEN]=QStringLiteral( QT_TR_NOOP( "Lost connection to the remote peer." ) );
    _ll[PHIRC_TCP_SSL_ERROR]=QStringLiteral( QT_TR_NOOP( "An SSL (secure socket layer) error occured." ) );

    _ll[PHIRC_PROT_UNKNOWN_CMD]=QStringLiteral( QT_TR_NOOP( "Unknown command received. Closing channel." ) );
    _ll[PHIRC_PROT_CMD_ERROR]=QStringLiteral( QT_TR_NOOP( "Received an error in the protocol stack." ) );

    _ll[PHIRC_MSG_NOT_FOUND]=QStringLiteral( QT_TR_NOOP( "Queue is empty or no message matches any criteria." ) );
    _ll[PHIRC_MSG_CODEC_ERROR]=QStringLiteral( QT_TR_NOOP( "Codec or MIB for the message could not be resolved." ) );
    _ll[PHIRC_MSG_CRC16_ERROR]=QStringLiteral( QT_TR_NOOP( "The CRC16 checksum over the message missmatched." ) );
    _ll[PHIRC_MSG_EXPIRED]=QStringLiteral( QT_TR_NOOP( "A message was found but has expired." ) );

    _ll[PHIRC_OUT_OF_MEMORY]=QStringLiteral( QT_TR_NOOP( "Could not allocate enough memory." ) );
    _ll[PHIRC_LOG_INIT_ERROR]=QStringLiteral( QT_TR_NOOP( "Log writer initalisation failure." ) );
    _ll[PHIRC_ARGUMENT_ERROR]=QStringLiteral( QT_TR_NOOP( "Wrong argument or argument count." ) );
    _ll[PHIRC_BUFFER_ERROR]=QStringLiteral( QT_TR_NOOP( "A buffer could not be referenced or the buffer is too small." ) );
    _ll[PHIRC_LOCALE_ERROR]=QStringLiteral( QT_TR_NOOP( "Error with locale settings for this machine." ) );
    _ll[PHIRC_SCRIPT_PARSE_ERROR]=QStringLiteral( QT_TR_NOOP( "The script could not be parsed correctly." ) );
    _ll[PHIRC_NOT_IMPL_ERROR]=QStringLiteral( QT_TR_NOOP( "The requested function is not implemented yet." ) );
    _ll[PHIRC_USER_ABORT_ERROR]=QStringLiteral( QT_TR_NOOP( "The user aborted an operation." ) );
    _ll[PHIRC_TRIGGER_ERROR]=QStringLiteral( QT_TR_NOOP( "An error occured on a triggered program." ) );
    _ll[PHIRC_DB_ERROR]=QStringLiteral( QT_TR_NOOP( "The underlying DBMS returned an error." ) );
    _ll[PHIRC_TRANSACTION_ERROR]=QStringLiteral( QT_TR_NOOP( "A transaction could not be started or finished." ) );
    _ll[PHIRC_QUERY_ERROR]=QStringLiteral( QT_TR_NOOP( "A database query returned an error." ) );

    _ll[PHIRC_MODULE_LOAD_ERROR]=QStringLiteral( QT_TR_NOOP( "A server module could not be loaded." ) );
    _ll[PHIRC_MODULE_LOG]=QStringLiteral( QT_TR_NOOP( "Module log message." ) );
    _ll[PHIRC_MODULE_DEPRECATED]=QStringLiteral( QT_TR_NOOP( "A module is deprecated or an old interface is used." ) );

    _ll[PHIRC_HTTP_CONTINUE]=QStringLiteral( QT_TR_NOOP( "Continue with request." ) );
    _ll[PHIRC_HTTP_SWITCH_PROTOCOL]=QStringLiteral( QT_TR_NOOP( "Switch to another protocol." ) );

    _ll[PHIRC_HTTP_OK]=QStringLiteral( QT_TR_NOOP( "Request succeeded." ) );
    _ll[PHIRC_HTTP_CREATED]=QStringLiteral( QT_TR_NOOP( "Document created successfully." ) );
    _ll[PHIRC_HTTP_ACCEPTED]=QStringLiteral( QT_TR_NOOP( "Request has been accepted." ) );
    _ll[PHIRC_HTTP_NON_AUTHORITATIVE]=QStringLiteral( QT_TR_NOOP( "Non authoritative answer." ) );
    _ll[PHIRC_HTTP_NO_CONTENT]=QStringLiteral( QT_TR_NOOP( "No content available for this request." ) );
    _ll[PHIRC_HTTP_RESET_CONTENT]=QStringLiteral( QT_TR_NOOP( "Reset document content." ) );
    _ll[PHIRC_HTTP_PARTIAL_CONTENT]=QStringLiteral( QT_TR_NOOP( "Response contains partial content." ) );

    _ll[PHIRC_HTTP_MULTIPLE_CHOICES]=QStringLiteral( QT_TR_NOOP( "Multiple choices are available." ) );
    _ll[PHIRC_HTTP_MOVED_PERMANENTLY]=QStringLiteral( QT_TR_NOOP( "Document moved permanently." ) );
    _ll[PHIRC_HTTP_FOUND]=QStringLiteral( QT_TR_NOOP( "Document found but at a different location." ) );
    _ll[PHIRC_HTTP_SEE_OTHER]=QStringLiteral( QT_TR_NOOP( "See at other location." ) );
    _ll[PHIRC_HTTP_NOT_MODIFIED]=QStringLiteral( QT_TR_NOOP( "Document is not modified." ) );
    _ll[PHIRC_HTTP_USE_PROXY]=QStringLiteral( QT_TR_NOOP( "Use a specific proxy." ) );
    _ll[PHIRC_HTTP_UNUSED]=QStringLiteral( QT_TR_NOOP( "Preserved return code." ) );
    _ll[PHIRC_HTTP_TMP_REDIRECT]=QStringLiteral( QT_TR_NOOP( "Document is temporarily at a different URI." ) );

    _ll[PHIRC_HTTP_BAD_REQUEST]=QStringLiteral( QT_TR_NOOP( "Client sent a bad request." ) );
    _ll[PHIRC_HTTP_UNAUTHORIZED]=QStringLiteral( QT_TR_NOOP( "Client is not authorized." ) );
    _ll[PHIRC_HTTP_PAYMENT_REQUIRED]=QStringLiteral( QT_TR_NOOP( "Payment is required." ) );
    _ll[PHIRC_HTTP_FORBIDDEN]=QStringLiteral( QT_TR_NOOP( "Server is refusing the request." ) );
    _ll[PHIRC_HTTP_NOT_FOUND]=QStringLiteral( QT_TR_NOOP( "Document not found." ) );
    _ll[PHIRC_HTTP_METHOD_NOT_ALLOWED]=QStringLiteral( QT_TR_NOOP( "Method is not allowed." ) );
    _ll[PHIRC_HTTP_NOT_ACCEPTABLE]=QStringLiteral( QT_TR_NOOP( "Resource is not accepted." ) );
    _ll[PHIRC_HTTP_PROXY_AUTHENTICATION_RQUIRED]=QStringLiteral( QT_TR_NOOP( "Proxy authentication is required." ) );
    _ll[PHIRC_HTTP_REQUEST_TIMEOUT]=QStringLiteral( QT_TR_NOOP( "Request timedout." ) );
    _ll[PHIRC_HTTP_CONFLICT]=QStringLiteral( QT_TR_NOOP( "A resource conflict occured." ) );
    _ll[PHIRC_HTTP_GONE]=QStringLiteral( QT_TR_NOOP( "The document is gone to an unknown location." ) );
    _ll[PHIRC_HTTP_LENGTH_REQUIRED]=QStringLiteral( QT_TR_NOOP( "Length of the entity is required." ) );
    _ll[PHIRC_HTTP_PRECONDITION_FAILED]=QStringLiteral( QT_TR_NOOP( "A precondition failed." ) );
    _ll[PHIRC_HTTP_REQUEST_ENTITY_TOO_LARGE]=QStringLiteral( QT_TR_NOOP( "Entity is too large." ) );
    _ll[PHIRC_HTTP_REQUEST_URI_TOO_LONG]=QStringLiteral( QT_TR_NOOP( "Request URI is too long." ) );
    _ll[PHIRC_HTTP_UNSUPPORTED_MEDIA_TYPE]=QStringLiteral( QT_TR_NOOP( "Provided media type is not supported." ) );
    _ll[PHIRC_HTTP_REQUESTED_RANGE_NOT_SATISFIABLE]=QStringLiteral( QT_TR_NOOP( "Range is not satisfiable." ) );
    _ll[PHIRC_HTTP_EXPECTATION_FAILED]=QStringLiteral( QT_TR_NOOP( "Expectation could not be fullfilled." ) );

    _ll[PHIRC_HTTP_INTERNAL_SERVER_ERROR]=QStringLiteral( QT_TR_NOOP( "An internal server error occured." ) );
    _ll[PHIRC_HTTP_NOT_IMPLEMENTED]=QStringLiteral( QT_TR_NOOP( "The server does not support the requested functionality." ) );
    _ll[PHIRC_HTTP_BAD_GATEWAY]=QStringLiteral( QT_TR_NOOP( "Invalid response received from gateway." ) );
    _ll[PHIRC_HTTP_SERVICE_UNAVAILABLE]=QStringLiteral( QT_TR_NOOP( "Service is currently not available." ) );
    _ll[PHIRC_HTTP_GATEWAY_TIMEOUT]=QStringLiteral( QT_TR_NOOP( "Response from the gateway timedout." ) );
    _ll[PHIRC_HTTP_VERSION_NOT_SUPPORTED]=QStringLiteral( QT_TR_NOOP( "The HTTP version is not supported." ) );
}
