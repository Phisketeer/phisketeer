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

static QScriptValue print( QScriptContext *ctx, QScriptEngine* )
{
    QScriptValue s=ctx->argument( 0 );
    PHIError::printConsole( s.toString() );
    return QScriptValue();
}

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

static QScriptValue loadModule( QScriptContext *ctx, QScriptEngine *engine, void *req )
{
    if ( !ctx->argument( 0 ).isString() ) return QScriptValue( false );
    const PHISRequest *_req=static_cast<const PHISRequest*>(req);
    Q_ASSERT(_req);
    QString m=ctx->argument( 0 ).toString();
    PHISModuleFactory *factory=PHISModuleFactory::instance();
    factory->lock(); //locking for read
    PHISModule *mod=factory->module( m );
    if ( !mod ) {
        factory->unlock();
        _req->responseRec()->log( PHILOGERR, PHIRC_OBJ_NOT_FOUND_ERROR,
            QObject::tr( "Could not find requested module '%1'." ).arg( m ) );
        return QScriptValue( false );
    }
    PHISScriptObj *obj=mod->create( m, new PHISInterface( _req, engine ) );
    if ( obj ) {
        obj->initObject( m );
        factory->unlock();
        return QScriptValue( true );
    } else {
        QString tmp=QObject::tr( "Could not create module '%1'." ).arg( m );
        _req->responseRec()->log( PHILOGERR, PHIRC_MODULE_LOAD_ERROR, tmp );
    }
    factory->unlock();
    return QScriptValue( false );
}

PHISGlobalScriptObj::PHISGlobalScriptObj( const PHISRequest *req, QScriptEngine *engine )
    : QObject( engine )
{
    qWarning( "PHISGlobalScriptObj::PHISGlobalScriptObj()" );
    QScriptValue go=engine->globalObject();
    go.setProperty( "$", engine->newFunction( getItemFunc, 1 ) );
    go.setProperty( "loadModule", engine->newFunction( loadModule, (void*)req ) );
    go.setProperty( "print", engine->newFunction( print, 1 ) );
}

PHISGlobalScriptObj::~PHISGlobalScriptObj()
{
    qWarning( "PHISGlobalScriptObj::~PHISGlobalScriptObj()" );
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
