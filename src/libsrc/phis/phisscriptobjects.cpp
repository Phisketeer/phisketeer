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
#include <QSqlDatabase>
#include <QScriptEngine>
#include <QScriptValue>
#include "phisscriptobjects.h"
#include "phirequest.h"
#include "phibasepage.h"
#include "phibaseitem.h"
#include "phidomimage.h"
#include "phismodule.h"
#include "phismodulefactory.h"
#include "phi.h"

#define PHISSCRIPTEXTENSION QScriptEngine::ScriptOwnership, QScriptEngine::PreferExistingWrapperObject |\
    QScriptEngine::ExcludeSuperClassMethods | QScriptEngine::ExcludeDeleteLater

static QScriptValue eval( QScriptContext *ctx, QScriptEngine *engine, void *args )
{
    Q_UNUSED( ctx )
    PHISInterface *phisif=static_cast<PHISInterface*>(args);
    Q_ASSERT( phisif );
    QString s=QObject::tr( "Security risk: 'eval()' in ServerScript is not permitted!" );
    phisif->log( PHISInterface::LTError, __FILE__, __LINE__, QDateTime::currentDateTime(), s );
    qWarning() << s;
    return engine->undefinedValue();
}

static QScriptValue print( QScriptContext *ctx, QScriptEngine *engine, void *args )
{
    QScriptValue s=ctx->argument( 0 );
    PHISInterface *phisif=static_cast<PHISInterface*>(args);
    Q_ASSERT( phisif );
    phisif->log( PHISInterface::LTUser, __FILE__, __LINE__, QDateTime::currentDateTime(), s.toString() );
    qWarning() << s.toString();
    return engine->undefinedValue();
}

static QScriptValue newImage( QScriptContext*, QScriptEngine *engine )
{
    return engine->newQObject( new PHIDomImage(), QScriptEngine::ScriptOwnership,
        QScriptEngine::PreferExistingWrapperObject |
        QScriptEngine::ExcludeSuperClassMethods |
        QScriptEngine::ExcludeDeleteLater );
}

static QScriptValue exitFunc( QScriptContext *ctx, QScriptEngine *engine )
{
    if ( ctx->argumentCount()>0 ) {
        QScriptValue code=ctx->argument( 0 );
        if ( code.toInt32()!=0 ) {
            QString errString=L1( "undefined" );
            if ( ctx->argumentCount()>1 ) errString=ctx->argument( 1 ).toString();
            QScriptValue err=ctx->throwError( errString );
            err.setProperty( L1( "lineNumber" ), -1 );
            engine->abortEvaluation( err );
            return err;
        }
    }
    engine->abortEvaluation();
    return engine->undefinedValue();
}

static QScriptValue getItemFunc( QScriptContext *ctx, QScriptEngine *engine )
{
    QScriptValue id=ctx->argument( 0 );
    if ( Q_LIKELY( id.isString() ) ) {
        const PHIBasePage *page=qobject_cast<const PHIBasePage*>(engine->parent());
        Q_ASSERT( page );
        PHIBaseItem *it=page->findItem( id.toString() );
        if ( it ) qDebug() << "$" << it->id();
        if ( Q_UNLIKELY( !it ) ) return engine->undefinedValue();
        return baseItemToScriptValue( engine, it );
    } else if ( id.isObject() ) return id;
    return engine->undefinedValue();
}

QScriptValue loadModule( QScriptContext *ctx, QScriptEngine *engine, void *args )
{
    if ( !ctx->argument( 0 ).isString() ) return QScriptValue( false );
    QString m=ctx->argument( 0 ).toString();
    if ( m==L1( "wrapper" ) ) return QScriptValue( true );
    PHISInterface *phisif=static_cast<PHISInterface*>(args);
    Q_ASSERT( phisif );
    PHISModuleFactory *factory=PHISModuleFactory::instance();
    factory->lock(); // locking for read
    PHISModule *mod=factory->module( m );
    if ( Q_UNLIKELY( !mod ) ) {
        factory->unlock();
        phisif->log( PHISInterface::LTError, __FILE__, __LINE__, QDateTime::currentDateTime(),
            QObject::tr( "Could not allocate module '%1'." ).arg( m ) );
        return QScriptValue( false );
    }
    PHISScriptObj *obj=mod->create( m, phisif );
    if ( Q_LIKELY( obj ) ) {
        qDebug( "loadModule( %s )", qPrintable( m ) );
        obj->initObject( engine, m );
        factory->unlock();
        return QScriptValue( true );
    } else {
        QString tmp=QObject::tr( "Could not instantiate module '%1'." ).arg( m );
        phisif->log( PHISInterface::LTError, __FILE__, __LINE__, QDateTime::currentDateTime(), tmp );
    }
    factory->unlock();
    return QScriptValue( false );
}

PHISGlobalScriptObj::PHISGlobalScriptObj( PHIBasePage *page, const PHIRequest *req,
    const QSqlDatabase &db, QScriptEngine *engine )
{
    PHISInterface *phisif=new PHISInterface( req, page, db ); // parent=page
    QScriptValue go=engine->globalObject();
    go.setProperty( SL( "eval" ), engine->newFunction( eval, static_cast<void*>(phisif) ) );
    go.setProperty( SL( "loadModule" ), engine->newFunction( loadModule, static_cast<void*>(phisif) ) );
    go.setProperty( SL( "print" ), engine->newFunction( print, static_cast<void*>(phisif) ) );
    go.setProperty( SL( "Image" ), engine->newFunction( newImage, 0 ) );
    go.setProperty( SL( "$" ), engine->newFunction( getItemFunc, 1 ) );
    go.setProperty( SL( "exit" ), engine->newFunction( exitFunc, 1 ) );
}

#undef PHISSCRIPTEXTENSION
