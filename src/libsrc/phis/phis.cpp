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
#include "phis.h"
QByteArray PHIS::_xmlStartTag=QByteArray( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" );
QByteArray PHIS::_doctypeXhtml=QByteArray( "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\"\n\
 \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n" );
QByteArray PHIS::_doctypeHtml4=QByteArray( "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\"\n\
 \"http://www.w3.org/TR/html4/strict.dtd\">\n" );
QByteArray PHIS::_doctypeHtml5=QByteArray( "<!DOCTYPE HTML>\n" );
QByteArray PHIS::_htmlXmlStartTag=QByteArray( "<html xmlns=\"http://www.w3.org/1999/xhtml\"\
 xml:lang=\"en\" >\n" );
QByteArray PHIS::_htmlStartTag=QByteArray( "<html>\n" );

PHIS::PHIS()
{
    qDebug( "PHIS::PHIS()" );
}

PHIS::~PHIS()
{
    qDebug( "PHIS::~PHIS()" );
}

QString PHIS::libVersion()
{
    return QString( PHIVERSION );
}

QByteArray PHIS::textForHttpCode( PHIRC rc )
{
    switch ( rc ) {
    case 100: return QByteArray( "Continue" );
    case 101: return QByteArray( "Switching Protocols" );
    case 200: return QByteArray( "OK" );
    case 201: return QByteArray( "Created" );
    case 202: return QByteArray( "Accepted" );
    case 203: return QByteArray( "Non-Authoritative Information" );
    case 204: return QByteArray( "No Content" );
    case 205: return QByteArray( "Reset Content" );
    case 206: return QByteArray( "Partial Content" );
    case 300: return QByteArray( "Multiple Choices" );
    case 301: return QByteArray( "Moved Permanently" );
    case 302: return QByteArray( "Found" );
    case 303: return QByteArray( "See Other" );
    case 304: return QByteArray( "Not Modified" );
    case 305: return QByteArray( "Use Proxy" );
    case 307: return QByteArray( "Temporary Redirect" );
    case 400: return QByteArray( "Bad Request" );
    case 401: return QByteArray( "Unauthorized" );
    case 402: return QByteArray( "Payment Required" );
    case 403: return QByteArray( "Forbidden" );
    case 404: return QByteArray( "Not Found" );
    case 405: return QByteArray( "Method Not Allowed" );
    case 406: return QByteArray( "Not Acceptable" );
    case 407: return QByteArray( "Proxy Authentication Required" );
    case 408: return QByteArray( "Request Timeout" );
    case 409: return QByteArray( "Conflict" );
    case 410: return QByteArray( "Gone" );
    case 411: return QByteArray( "Length Required" );
    case 412: return QByteArray( "Precondition Failed" );
    case 413: return QByteArray( "Request Entity Too Large" );
    case 414: return QByteArray( "Request-URI Too Long" );
    case 415: return QByteArray( "Unsupported Media Type" );
    case 416: return QByteArray( "Requested Range Not Satisfiable" );
    case 417: return QByteArray( "Expectation Failed" );
    case 501: return QByteArray( "Not Implemented" );
    case 502: return QByteArray( "Bad Gateway" );
    case 503: return QByteArray( "Service Unavailable" );
    case 504: return QByteArray( "Gateway Timeout" );
    case 505: return QByteArray( "HTTP Version Not Supported" );
    default:;
    }
    return QByteArray( "Unknown error" )+" "+QByteArray::number( rc );
}
