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
#include <QDateTime>
#include <QLocale>
#include "phiresponserec.h"
#include "phierror.h"

static QByteArray _phiHtmlContentType="text/html; charset=utf-8";

PHIResponseRec::PHIResponseRec() : _contentType( _phiHtmlContentType ), _contentLength( 0 )
{
    qDebug( "PHIResponseRec::PHIResponseRec()" );
}

PHIResponseRec::~PHIResponseRec()
{
    qDebug( "PHIResponseRec::~PHIResponseRec()" );
}

void PHIResponseRec::clear()
{
    _contentType=_phiHtmlContentType;
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
        QLocale::ShortFormat ).toLatin1()+dt.toUTC().toString( QStringLiteral( ", dd " ) ).toLatin1()
        +locale.monthName( dt.toUTC().date().month(),
        QLocale::ShortFormat ).toLatin1()+dt.toUTC().toString( QStringLiteral( " yyyy HH:mm:ss" ) )
        .toLatin1()+" GMT";
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
    if ( _minorHttpVer>0 ) tmp=PHIS::xmlStartTag()+PHIS::doctypeXhtml()+PHIS::htmlXmlStartTag();
    else tmp=PHIS::doctypeHtml4()+PHIS::htmlStartTag();
    return tmp;
}

// Not used in Apache module
QByteArray PHIResponseRec::createResponseHeader( PHIRC rc, const QString &serverString ) const
{
    QLocale locale( QLocale::C );
    QDateTime dt=QDateTime::currentDateTime();
    QByteArray tmp="HTTP/1."+QByteArray::number( _minorHttpVer )+' '+QByteArray::number( static_cast<int>(rc) )
        +' '+PHIS::textForHttpCode( rc )+"\r\n";
    tmp+="Date: "+locale.dayName( dt.toUTC().date().dayOfWeek(), QLocale::ShortFormat ).toLatin1()
        +dt.toUTC().toString( ", dd " ).toLatin1()+locale.monthName( dt.toUTC().date().month(),
        QLocale::ShortFormat ).toLatin1()+dt.toUTC().toString( " yyyy HH:mm:ss" ).toLatin1()+" GMT\r\n";
    tmp+="Server: "+serverString.toUtf8()+"\r\n";
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
        +PHIS::textForHttpCode( rc )+"</title>\n<meta http-equiv=\"content-type\" ";
    //if ( _minorHttpVer>0 ) content+="content=\"application/xhtml+xml;";
    content+="content=\"text/html;";
    content+=" charset=UTF-8\" />\n</head>\n<body>\n<h1>"+QByteArray::number( static_cast<int>(rc) );
    content+=' '+PHIS::textForHttpCode( rc )+"</h1>\n<p>"+PHIError::instance()->longDesc( rc ).toUtf8()+"</p>\n";
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
