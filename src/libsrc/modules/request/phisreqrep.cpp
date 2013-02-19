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
#include <QTextCodec>
#include "phisreqrep.h"

QString PHISReplyObj::_slash=QStringLiteral( "/" );

QScriptValue PHISRequestObj::initObject( QScriptEngine *engine, const QString &mod )
{
    QScriptValue request=PHISScriptObj::initObject( engine, mod );
    QScriptValue arr=engine->newArray();
    QStringList values, keys;
    QString value, key, props=QStringLiteral( "properties" );
    int i=0, l;

    QScriptValue sv=engine->newObject();
    QScriptValue subarr;
    request.setProperty( QStringLiteral( "post" ), sv );
    keys=PHIS_IF()->postKeys();
    foreach( key, keys ) {
        values=PHIS_IF()->postValues( key );
        subarr=engine->newArray();
        l=0;
        foreach ( value, values ) subarr.setProperty( l++, value );
        sv.setProperty( key, subarr );
        arr.setProperty( i++, key );
    }
    sv.setProperty( props, arr );

    sv=engine->newObject();
    request.setProperty( QStringLiteral( "get" ), sv );
    arr=engine->newArray();
    i=0;
    keys=PHIS_IF()->getKeys();
    foreach( key, keys ) {
        values=PHIS_IF()->getValues( key );
        subarr=engine->newArray();
        l=0;
        foreach ( value, values ) subarr.setProperty( l++, value );
        sv.setProperty( key, subarr );
        arr.setProperty( i++, key );
    }
    sv.setProperty( props, arr );

    QScriptValue cookie=engine->newObject();
    request.setProperty( QStringLiteral( "cookie" ), cookie );
    arr=engine->newArray();
    i=0;
    keys=PHIS_IF()->cookieKeys();
    foreach( key, keys ) {
        value=PHIS_IF()->cookieValue( key );
        cookie.setProperty( key, value );
        arr.setProperty( i++, key );
    }
    cookie.setProperty( props, arr );

    QScriptValue file=engine->newObject();
    request.setProperty( QStringLiteral( "file" ), file );
    arr=engine->newArray();
    i=0;
    keys=PHIS_IF()->uploadFileKeys();
    foreach( key, keys ) {
        const QString fileName=PHIS_IF()->uploadFileName( key );
        const QString tmpFile=PHIS_IF()->uploadTmpFile( key );
        int size=PHIS_IF()->uploadFileSize( key );
        QScriptValue id=engine->newObject();
        file.setProperty( key, id );
        id.setProperty( QStringLiteral( "filename" ), fileName );
        id.setProperty( QStringLiteral( "tmpfile" ), tmpFile );
        id.setProperty( QStringLiteral( "size" ), size );
        arr.setProperty( i++, key );
    }
    file.setProperty( props, arr );

    sv=engine->newObject();
    request.setProperty( QStringLiteral( "header" ), sv );
    arr=engine->newArray();
    i=0;
    keys=PHIS_IF()->headerKeys();
    foreach( key, keys ) {
        value=PHIS_IF()->headerValue( key );
        sv.setProperty( key, value );
        arr.setProperty( i++, key );
    }
    sv.setProperty( props, arr );
    return request;
}

PHISReplyObj::PHISReplyObj( const PHISInterface *interf )
    : PHISScriptObj( interf ), _charset( QStringLiteral( "utf-8" ) )
{
    qDebug( "PHISReplyObj::PHISReplyObj()" );
    _contentType=QStringLiteral( "text/html; charset=utf-8" );
}

void PHISReplyObj::setContent( const QString &c )
{
    if ( c.isEmpty() ) {
        PHIS_IF()->setContent( QByteArray() );
        return;
    }
    QTextCodec *codec=QTextCodec::codecForName( _charset.toUtf8() );
    if ( !codec ) {
        PHIS_ERROR( tr( "Unknown codec '%1' - using standard locale." ).arg( _charset ) );
        codec=QTextCodec::codecForLocale();
    }
    PHIS_IF()->setContent( codec->fromUnicode( c ) );
    _content=c;
    PHIS_IF()->setFileName( QString() );
}

void PHISReplyObj::setCharset( const QString &charset )
{
    QTextCodec *codec=QTextCodec::codecForName( charset.toUtf8() );
    if ( !codec ) {
        PHIS_ERROR( tr( "Unknown codec '%1' - using standard locale." ).arg( charset ) );
        codec=QTextCodec::codecForLocale();
    }
    PHIS_IF()->setContent( codec->fromUnicode( _content ) );
    _charset=charset;
}
