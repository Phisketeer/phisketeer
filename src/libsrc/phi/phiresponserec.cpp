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
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QDateTime>
#include <QLocale>
#include "phiresponserec.h"

static const QByteArray _htmlContentType="text/html; charset=utf-8";
static const QByteArray _xmlStartTag=QByteArray( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" );
static const QByteArray _doctypeXhtml=QByteArray( "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\"\n\
 \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n" );
static const QByteArray _doctypeHtml4=QByteArray( "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\"\n\
 \"http://www.w3.org/TR/html4/strict.dtd\">\n" );
static const QByteArray _htmlXmlStartTag=QByteArray( "<html xmlns=\"http://www.w3.org/1999/xhtml\"\
 xml:lang=\"en\" >\n" );
static const QByteArray _htmlStartTag=QByteArray( "<html>\n" );

PHIResponseRec::PHIResponseRec() : _contentType( _htmlContentType ), _contentLength( 0 )
{
}

void PHIResponseRec::clear()
{
    _contentType=_htmlContentType;
    _contentLength=0;
    _body.clear();
    _error._desc.clear();
    _error._rc=PHIRC_OK;
    _options=None;
    _headersOut.clear();
    _logs.clear();
}

void PHIResponseRec::log( int type, const char* file, int line, const QDateTime &dt, PHIRC rc, const QString &desc )
{
    PHIResponseRecLogEntry e;
    e._type=type;
    e._file=file;
    e._line=line;
    e._dt=dt;
    e._rc=rc;
    e._desc=desc;
    _logs.append( e );
}

QByteArray PHIResponseRec::timeEncoded( const QDateTime &dt ) const
{
    QLocale locale( QLocale::C );
    QByteArray arr=locale.dayName( dt.toUTC().date().dayOfWeek(),
        QLocale::ShortFormat ).toUtf8()+dt.toUTC().toString( SL( ", dd " ) ).toUtf8()
        +locale.monthName( dt.toUTC().date().month(),
        QLocale::ShortFormat ).toUtf8()+dt.toUTC().toString( SL( " yyyy HH:mm:ss" ) )
        .toUtf8()+" GMT";
    return arr;
}

void PHIResponseRec::setCookie( const QString &name, const QString &value, int maxage, const QString &path,
    const QString &domain, bool secure, bool discard )
{
    QByteArray arr;
    if ( _minorHttpVer==0 ) {
        QDateTime dt=QDateTime::currentDateTime().addSecs( maxage );
        arr=name.toUtf8()+"="+value.toUtf8()+"; Expires="+timeEncoded( dt )+"; Path="+path.toUtf8()+"; ";
        if ( !domain.isEmpty() ) arr+="Domain="+domain.toUtf8()+"; ";
    } else {
        arr=name.toUtf8()+"=\""+value.toUtf8()+"\"; Max-Age=\""+QByteArray::number( maxage )+
            "\"; Path=\""+path.toUtf8()+"\"; ";
        if ( !domain.isEmpty() ) arr+="Domain=\""+domain.toUtf8()+"\"; ";
    }
    if ( secure ) arr+="Secure; ";
    if ( discard ) arr+="Discard; ";
    if ( _minorHttpVer==0 ) arr+="Version=1";
    else arr+="Version=\"1\"";
    _headersOut.insertMulti( "Set-Cookie", arr );
    qDebug( "Set-Cookie: %s", arr.data() );
}

QByteArray PHIResponseRec::createDocumentHeader() const
{
    QByteArray tmp;
    if ( _minorHttpVer>0 ) tmp=_xmlStartTag+_doctypeXhtml+_htmlXmlStartTag;
    else tmp=_doctypeHtml4+_htmlStartTag;
    return tmp;
}

// Not used in Apache module
QByteArray PHIResponseRec::createResponseHeader( PHIRC rc, const QString &serverString ) const
{
    QLocale locale( QLocale::C );
    QDateTime dt=QDateTime::currentDateTime();
    QByteArray tmp="HTTP/1."+QByteArray::number( _minorHttpVer )+' '+QByteArray::number( static_cast<int>(rc) )
        +' '+textForHttpCode( rc )+"\r\n";
    tmp+="Date: "+locale.dayName( dt.toUTC().date().dayOfWeek(), QLocale::ShortFormat ).toUtf8()
        +dt.toUTC().toString( SL( ", dd " ) ).toUtf8()+locale.monthName( dt.toUTC().date().month(),
        QLocale::ShortFormat ).toUtf8()+dt.toUTC().toString( SL( " yyyy HH:mm:ss" ) ).toUtf8()+" GMT\r\n";
    tmp+="Server: "+serverString.toLatin1()+"\r\n";
    qDebug( "PHIResponseRec::createResponseHeader(rc=%d)", rc );
    tmp+="Content-Type: "+_contentType+"\r\n";
    if ( rc==PHIRC_HTTP_NOT_MODIFIED ) return tmp;
    tmp+="Content-Length: "+QByteArray::number( _contentLength )+"\r\n";
    if ( rc!=PHIRC_HTTP_OK ) return tmp;
    QByteArray key;
    foreach ( key, _headersOut.keys() ) {
        if ( !key.startsWith( "Set-Cookie" ) ) tmp+=key+": "+_headersOut.value( key )+"\r\n";
    }
    QList <QByteArray> cookies=_headersOut.values( "Set-Cookie" );
    if ( cookies.count()>0 ) {
        //tmp+=QByteArray( "Set-Cookie: " ); // some browsers have problems to merge cookies into one header
        foreach ( key, cookies ) tmp+=QByteArray( "Set-Cookie: " )+key+"\r\n";
        //tmp.chop( 2 ); // remove trailing comma
        //tmp+="\r\n";
    }
    return tmp;
}

QByteArray PHIResponseRec::createErrorResponse( PHIRC rc, const QString &err ) const
{
    quint8 ver=_minorHttpVer;
    _minorHttpVer=0; // create "old" document header
    QByteArray content=createDocumentHeader();
    _minorHttpVer=ver;
    content+="<head>\n<title>"+QByteArray::number( static_cast<int>(rc) )+' '
        +textForHttpCode( rc )+"</title>\n<meta http-equiv=\"content-type\" ";
    //if ( _minorHttpVer>0 ) content+="content=\"application/xhtml+xml;";
    content+="content=\"text/html;";
    content+=" charset=UTF-8\" />\n</head>\n<body>\n<h1>"+QByteArray::number( static_cast<int>(rc) );
    content+=' '+textForHttpCode( rc )+"</h1>\n<p>"+PHIError::instance()->longDesc( rc ).toUtf8()+"</p>\n";
    content+="<p>"+err.toUtf8()+"</p>\n</body>\n</html>\n";
    return content;
}

// Not used in Apache module
QByteArray PHIResponseRec::createErrorResponseHeader( PHIRC rc, const QString &serverString ) const
{
    QByteArray header=createResponseHeader( rc, serverString );
    if ( _minorHttpVer>0 ) header+="Cache-Control:";
    else header+="Pragma:";
    header+=" no-cache\r\n";
    if ( _minorHttpVer>0 ) header+="Connection: close\r\n";
    return header;
}

QByteArray PHIResponseRec::textForHttpCode( PHIRC rc )
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
    case 500: return QByteArray( "Internal Server Error" );
    case 501: return QByteArray( "Not Implemented" );
    case 502: return QByteArray( "Bad Gateway" );
    case 503: return QByteArray( "Service Unavailable" );
    case 504: return QByteArray( "Gateway Timeout" );
    case 505: return QByteArray( "HTTP Version Not Supported" );
    default:;
    }
    return QByteArray( "Unknown error" )+" "+QByteArray::number( rc );
}
