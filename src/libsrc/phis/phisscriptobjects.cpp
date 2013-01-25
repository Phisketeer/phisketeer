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
#include <QMetaObject>
#include <QMetaProperty>
#include <QSqlRecord>
#include <QProcess>
#include <QTextCodec>
#include <QHostAddress>
#include <QHostInfo>
#include "phisscriptobjects.h"
#include "phi.h"
#include "phierror.h"
#include "phibaseitem.h"
#include "phibasepage.h"
#include "phismodule.h"
#include "phismodulefactory.h"

#define PHISSCRIPTEXTENSION QScriptEngine::ScriptOwnership, QScriptEngine::PreferExistingWrapperObject |\
    QScriptEngine::ExcludeSuperClassMethods | QScriptEngine::ExcludeDeleteLater

static QScriptValue getItemFunc( QScriptContext *ctx, QScriptEngine *engine )
{
    QScriptValue id=ctx->argument( 0 );
    if ( id.isString() ) {
        PHIBasePage *page=qobject_cast<PHIBasePage*>(engine->parent());
        Q_ASSERT( page );
        PHIBaseItem *it=page->getElementById( id.toString() );
        if ( !it ) return QScriptValue( QScriptValue::UndefinedValue );
        return baseItemToScriptValue( engine, it );
    } else if ( id.isObject() ) return id;
    return QScriptValue( QScriptValue::UndefinedValue );
}

PHISGlobalScriptObj::PHISGlobalScriptObj( const PHISRequest *req, QScriptEngine *engine )
    : QObject( engine ), _req( req )
{
    qWarning( "PHISGlobalScriptObj::PHISGlobalScriptObj()" );
    QScriptValue sv=engine->newQObject( this, PHISSCRIPTEXTENSION );
    engine->setGlobalObject( sv );
    sv=engine->newFunction( getItemFunc );
    engine->globalObject().setProperty( "$", sv );
}

PHISGlobalScriptObj::~PHISGlobalScriptObj()
{
    qWarning( "PHISGlobalScriptObj::~PHISGlobalScriptObj()" );
}

void PHISGlobalScriptObj::loadModule( const QString &m )
{
    QScriptEngine *engine=qobject_cast<QScriptEngine*>(parent());
    Q_ASSERT( engine );
    PHISModuleFactory *factory=PHISModuleFactory::instance();
    factory->lock(); //locking for read
    PHISModule *mod=factory->module( m );
    if ( !mod ) {
        factory->unlock();
        _req->responseRec()->log( PHILOGERR, PHIRC_OBJ_NOT_FOUND_ERROR,
            tr( "Could not find requested module '%1'." ).arg( m ) );
        return;
    }
    QObject *obj=mod->create( m, new PHISInterface( _req, engine ) );
    if ( obj ) {
        QScriptValue sv=engine->newQObject( obj, PHISSCRIPTEXTENSION );
        engine->globalObject().setProperty( m, sv );
    } else {
        QString tmp=QObject::tr( "Could not create module '%1'." ).arg( m );
        _req->responseRec()->log( PHILOGERR, PHIRC_MODULE_LOAD_ERROR, tmp );
    }
    factory->unlock();
}

PHISRequestObj::PHISRequestObj( const PHISRequest *req, QScriptEngine *engine, PHIResponseRec *resp )
    : QObject( engine ), _req( req ), _resp( resp )
{
    qDebug( "PHISRequestObj::PHISRequestObj()" );
    setObjectName( "request" );
    QScriptValue request=engine->newQObject( this, PHISSCRIPTEXTENSION );
    engine->globalObject().setProperty( "request", request );

    QScriptValue arr=engine->newArray();
    QStringList values, keys;
    QString value, key;
    int i=0, l;

    QScriptValue post=engine->newObject();
    QScriptValue subarr;
    request.setProperty( "post", post );
    keys=_req->postKeys();
    foreach( key, keys ) {
        values=req->postValues( key );
        subarr=engine->newArray();
        l=0;
        /*
        foreach ( value, values ) subarr.setProperty( l++, value, QScriptValue::ReadOnly );
        post.setProperty( QString::fromUtf8( key ), subarr, QScriptValue::ReadOnly );
        arr.setProperty( i++, QString::fromUtf8( key ), QScriptValue::ReadOnly );
        */
        foreach ( value, values ) subarr.setProperty( l++, value );
        post.setProperty( key, subarr );
        arr.setProperty( i++, key );
    }
    //post.setProperty( "properties", arr, QScriptValue::ReadOnly );
    post.setProperty( "properties", arr );

    QScriptValue get=engine->newObject();
    request.setProperty( "get", get );
    arr=engine->newArray();
    i=0;
    keys=_req->getKeys();
    foreach( key, keys ) {
        values=_req->getValues( key );
        subarr=engine->newArray();
        l=0;
        /*
        foreach ( value, values ) subarr.setProperty( l++, value, QScriptValue::ReadOnly );
        get.setProperty( QString::fromUtf8( key ), subarr, QScriptValue::ReadOnly );
        arr.setProperty( i++, QString::fromUtf8( key ), QScriptValue::ReadOnly );
        */
        foreach ( value, values ) subarr.setProperty( l++, value );
        get.setProperty( key, subarr );
        arr.setProperty( i++, key );
    }
    //get.setProperty( "properties", arr, QScriptValue::ReadOnly );
    get.setProperty( "properties", arr );

    QScriptValue cookie=engine->newObject();
    request.setProperty( "cookie", cookie );
    arr=engine->newArray();
    i=0;
    keys=_req->cookieKeys();
    foreach( key, keys ) {
        value=_req->cookieValue( key );
        //cookie.setProperty( QString::fromUtf8( key ), value, QScriptValue::ReadOnly );
        //arr.setProperty( i++, QString::fromUtf8( key ), QScriptValue::ReadOnly );
        cookie.setProperty( key, value );
        arr.setProperty( i++, key );
    }
    //cookie.setProperty( "properties", arr, QScriptValue::ReadOnly );
    cookie.setProperty( "properties", arr );

    QScriptValue file=engine->newObject();
    request.setProperty( "file", file );
    arr=engine->newArray();
    i=0;
    keys=_req->fileKeys();
    foreach( key, keys ) {
        const QString fileName=_req->fileName( key );
        const QString tmpFile=_req->tmpFile( key );
        int size=_req->fileSize( key ).toInt();
        QScriptValue id=engine->newObject();
        file.setProperty( key, id );
        /*
        id.setProperty( "filename", fileName, QScriptValue::ReadOnly );
        id.setProperty( "tmpfile", tmpFile, QScriptValue::ReadOnly );
        id.setProperty( "size", size, QScriptValue::ReadOnly );
        arr.setProperty( i++, QString::fromUtf8( key ), QScriptValue::ReadOnly );
        */
        id.setProperty( "filename", fileName );
        id.setProperty( "tmpfile", tmpFile );
        id.setProperty( "size", size );
        arr.setProperty( i++, key );
    }
    //file.setProperty( "properties", arr, QScriptValue::ReadOnly );
    file.setProperty( "properties", arr );

    QScriptValue header=engine->newObject();
    request.setProperty( "header", header );
    arr=engine->newArray();
    i=0;
    keys=_req->headerKeys();
    foreach( key, keys ) {
        value=_req->headerValue( key );
        //header.setProperty( QString::fromUtf8( key ), value, QScriptValue::ReadOnly );
        //arr.setProperty( i++, QString::fromUtf8( key ), QScriptValue::ReadOnly );
        header.setProperty( key, value );
        arr.setProperty( i++, key );
    }
    //header.setProperty( "properties", arr, QScriptValue::ReadOnly );
    header.setProperty( "properties", arr );
}

PHISRequestObj::~PHISRequestObj()
{
    qDebug( "PHISRequestObj::~PHISRequestObj()" );
}

PHISReplyObj::PHISReplyObj( QScriptEngine *engine, PHIResponseRec *resp )
    : QObject( engine ), _ownContent( false ), _charset( "utf-8" ), _resp( resp )
{
    qDebug( "PHISReplyObj::PHISReplyObj()" );
    _contentType=QString::fromLatin1( _resp->contentType() );
    setObjectName( "reply" );
    QScriptValue reply=engine->newQObject( this, PHISSCRIPTEXTENSION );
    engine->globalObject().setProperty( "reply", reply );
}

PHISReplyObj::~PHISReplyObj()
{
    qDebug( "PHISReplyObj::~PHISReplyObj()" );
}

void PHISReplyObj::setContent( const QString &c )
{
    if ( c.isEmpty() ) {
        _ownContent=false;
        _resp->setContentLength( 0 );
        _resp->setBody( QByteArray() );
        return;
    }
    QTextCodec *codec=QTextCodec::codecForName( _charset.toUtf8() );
    if ( !codec ) {
        _resp->log( PHILOGWARN, PHIRC_LOCALE_ERROR,
            tr( "Unknown codec '%1' - using standard locale." ).arg( _charset ) );
        codec=QTextCodec::codecForLocale();
    }
    _resp->setBody( codec->fromUnicode( c ) );
    _resp->setContentLength( _resp->body().length() );
    _ownContent=true;
    _content=c;
    _resp->setFileName( QString() );
}

void PHISReplyObj::setContentType( const QString &t )
{
    _resp->setContentType( t.toLatin1() );
    _contentType=t;
}

void PHISReplyObj::setCharset( const QString &charset )
{
    QTextCodec *codec=QTextCodec::codecForName( charset.toUtf8() );
    if ( !codec ) {
        _resp->log( PHILOGWARN, PHIRC_LOCALE_ERROR,
            tr( "Unknown codec '%1' - using standard locale." ).arg( charset ) );
        codec=QTextCodec::codecForLocale();
    }
    _resp->setBody( codec->fromUnicode( _content ) );
    _resp->setContentLength( _resp->body().length() );
    _charset=charset;
}

void PHISReplyObj::setFileName( const QString &f )
{
    _resp->setFileName( f );
    _fileName=f;
}

void PHISReplyObj::setCookie( const QString &name, const QString &value, const QDateTime &expires,
    const QString &path, const QString &domain, bool secure, bool discard )
{
    _resp->setCookie( name, value, expires, path, domain, secure, discard );
}

void PHISReplyObj::setCookie( const QString &name, const QString &value, int maxage,
    const QString &path, const QString &domain, bool secure, bool discard )
{
    _resp->setCookie( name, value, maxage, path, domain, secure, discard );
}

PHISSqlObj::PHISSqlObj( const QSqlDatabase &db, QScriptEngine *engine, PHIResponseRec *resp,
    bool useSql ) : QObject( engine ), _query( db ), _resp( resp )
{
    qDebug( "PHISSqlObj::PHISSqlObj()" );
    setObjectName( "sql" );
    _db=db;
    _query.setForwardOnly( true );
    if ( useSql ) {
        if ( !db.isOpen() ) {
            _resp->log( PHILOGWARN, PHIRC_DB_ERROR, tr( "A page is marked for database access, "
                "but the server is not connected. The script 'sql' property is not initialized. "
                "DB error: [%1] %2" ).arg( db.lastError().number() ).arg( db.lastError().text() ) );
            return;
        }
        QScriptValue sql=engine->newQObject( this, PHISSCRIPTEXTENSION );
        engine->globalObject().setProperty( "sql", sql );
    }
}

PHISSqlObj::PHISSqlObj( const QSqlDatabase &db, QScriptEngine *engine, PHIResponseRec *resp )
    : QObject( engine ), _query( db ), _resp( resp )
{
    setObjectName( "sqlclone" );
    _db=db;
    _query.setForwardOnly( true );
}

PHISSqlObj::~PHISSqlObj()
{
    qDebug( "PHISSqlObj::~PHISSqlObj()" );
}

QScriptValue PHISSqlObj::clone() const
{
    QScriptEngine *engine=qobject_cast<QScriptEngine*>(parent());
    Q_ASSERT( engine );
    PHISSqlObj *clone=new PHISSqlObj( _db, engine, _resp );
    return engine->newQObject( clone, PHISSCRIPTEXTENSION );
}

QList<QVariant> PHISSqlObj::values() const
{
    if ( !_query.isValid() ) return QList<QVariant>();
    QSqlRecord rec=_query.record();
    QList<QVariant> list;
    for ( int i=0; i < rec.count(); i++ ) list.append( rec.value( i ) );
    return list;
}

bool PHISSqlObj::query( const QString &q ) const
{
    bool res=_query.exec( q );
    if ( !res ) _resp->log( PHILOGWARN, PHIRC_QUERY_ERROR,
        tr( "SQL query '%1' failed. DB error: [%2] %3" ).arg( q )
        .arg( _query.lastError().number() ).arg( _query.lastError().text() ) );
    return res;
}

#undef PHISSCRIPTEXTENSION
