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
#include "phidomimage.h"
#include "phismodule.h"
#include "phismodulefactory.h"

#define PHISSCRIPTEXTENSION QScriptEngine::ScriptOwnership, QScriptEngine::PreferExistingWrapperObject |\
    QScriptEngine::ExcludeSuperClassMethods | QScriptEngine::ExcludeDeleteLater

class PHISPrivateArgs
{
public:
    const PHISRequest *_req;
    QSqlDatabase _db;
};

static QScriptValue print( QScriptContext *ctx, QScriptEngine* )
{
    QScriptValue s=ctx->argument( 0 );
    qWarning( qPrintable( s.toString() ) );
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
    return engine->undefinedValue();
}

static QScriptValue newImage( QScriptContext*, QScriptEngine *engine )
{
    return engine->newQObject( new PHIDomImage(), QScriptEngine::ScriptOwnership,
        QScriptEngine::PreferExistingWrapperObject |
        QScriptEngine::ExcludeSuperClassMethods | QScriptEngine::ExcludeDeleteLater );
}

static QScriptValue loadModule( QScriptContext *ctx, QScriptEngine *engine, void *args )
{
    if ( !ctx->argument( 0 ).isString() ) return QScriptValue( false );
    QString m=ctx->argument( 0 ).toString();
    PHISPrivateArgs *pa=static_cast<PHISPrivateArgs*>(args);
    Q_ASSERT(pa);
    const PHISRequest *req=pa->_req;
    QSqlDatabase db=pa->_db;
    PHISModuleFactory *factory=PHISModuleFactory::instance();
    factory->lock(); //locking for read
    PHISModule *mod=factory->module( m );
    if ( !mod ) {
        factory->unlock();
        req->responseRec()->log( PHILOGERR, PHIRC_OBJ_NOT_FOUND_ERROR,
            QObject::tr( "Could not find requested module '%1'." ).arg( m ) );
        return QScriptValue( false );
    }
    PHISScriptObj *obj=mod->create( m, new PHISInterface( req, engine, db ) );
    if ( obj ) {
        obj->initObject( m );
        factory->unlock();
        return QScriptValue( true );
    } else {
        QString tmp=QObject::tr( "Could not create module '%1'." ).arg( m );
        req->responseRec()->log( PHILOGERR, PHIRC_MODULE_LOAD_ERROR, tmp );
    }
    factory->unlock();
    return QScriptValue( false );
}

PHISGlobalScriptObj::PHISGlobalScriptObj( const PHISRequest *req, QScriptEngine *engine, const QSqlDatabase &db )
    : QObject( engine )
{
    qDebug( "PHISGlobalScriptObj::PHISGlobalScriptObj()" );
    _args=new PHISPrivateArgs();
    _args->_req=req;
    _args->_db=db;
    QScriptValue go=engine->globalObject();
    go.setProperty( QStringLiteral( "$" ), engine->newFunction( getItemFunc, 1 ) );
    go.setProperty( QStringLiteral( "loadModule" ), engine->newFunction( loadModule, (void*)_args ) );
    go.setProperty( QStringLiteral( "print" ), engine->newFunction( print, 1 ) );
    go.setProperty( QStringLiteral( "Image" ), engine->newFunction( newImage, 0 ) );
}

PHISGlobalScriptObj::~PHISGlobalScriptObj()
{
    delete _args;
    qDebug( "PHISGlobalScriptObj::~PHISGlobalScriptObj()" );
}

#undef PHISSCRIPTEXTENSION
