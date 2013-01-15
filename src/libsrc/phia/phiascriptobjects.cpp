/*
#    Copyright (C) 2010-2012  Marius B. Schumacher
#    Copyright (C) 2011-2012  Phisys AG, Switzerland
#    Copyright (C) 2012  Phisketeer.org team
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
#include <QScriptValue>
#include <QMessageBox>
#include <QInputDialog>
#include <QTimer>
#include <QLocale>
#include <QDesktopServices>
#include "phiascriptobjects.h"
#include "phiawebview.h"
#include "phia.h"
#include "phianetmanager.h"
#include "phisysinfo.h"
#include "phiascriptevent.h"
#include "phiascriptitem.h"

static QScriptValue getItemFunc( QScriptContext *ctx, QScriptEngine *engine )
{
    QScriptValue id=ctx->argument( 0 );
    if ( !id.isObject() && !id.isString() ) return QScriptValue( QScriptValue::UndefinedValue );

    PHIAWebView *view=qobject_cast<PHIAWebView*>(engine->parent());
    Q_ASSERT( view );
    QScriptValue item;
    if ( id.isObject() ) {
        PHIBaseItem *obj;
        baseItemFromScriptValue( id, obj );
        if ( !obj ) return QScriptValue( QScriptValue::UndefinedValue );
        item=view->scriptValue( obj->name() );
        if ( item.isValid() ) return item; // cached
        item=scriptItemToScriptValue( engine, new PHIAScriptItem( qobject_cast<PHIAItem*>(obj) ) );
        view->insertScriptValue( obj->name(), item );
    } else { // id is string
        item=view->scriptValue( id.toString() );
        if ( item.isValid() ) return item; // cached
        PHIBaseItem *it=view->page()->getElementById( id.toString() );
        if ( !it ) return QScriptValue( QScriptValue::UndefinedValue );
        item=scriptItemToScriptValue( engine, new PHIAScriptItem( qobject_cast<PHIAItem*>(it) ) );
        view->insertScriptValue( id.toString(), item );
    }
    return item;
}

PHIAScriptWindowObj::PHIAScriptWindowObj( PHIAWebView *view )
    : QObject( view->scriptEngine() ), _engine( view->scriptEngine() ), _view( view )
{
    qDebug( "PHIAScriptWindowObj::PHIAScriptWindowObj()" );
    setObjectName( "window" );
    Q_ASSERT( _engine );

    qScriptRegisterMetaType( _engine, baseItemToScriptValue, baseItemFromScriptValue );
    qScriptRegisterMetaType( _engine, eventToScriptValue, eventFromScriptValue );
    qScriptRegisterMetaType( _engine, scriptItemToScriptValue, scriptItemFromScriptValue );

    QScriptValue win=_engine->newQObject( this, PHIASCRIPTEXTENSION );
    _engine->setGlobalObject( win );
    _engine->globalObject().setProperty( "window", win );
    QScriptValue doc=_engine->newQObject( _view->page(), PHIASCRIPTEXTENSION );
    win.setProperty( "document", doc );
    QScriptValue form=_engine->newQObject( new PHIAScriptFormsObj( _view ), PHIASCRIPTEXTENSION );
    doc.setProperty( "phiform", form );

    QScriptValue loc=_engine->newQObject( new PHIAScriptLocationObj( _view ), PHIASCRIPTEXTENSION );
    win.setProperty( "location", loc );

    QScriptValue ifn=_engine->newFunction( getItemFunc );
    _engine->globalObject().setProperty( "$", ifn );

    connect( this, SIGNAL( homeRequested() ), _view,
        SIGNAL( homeRequested() ), Qt::QueuedConnection );
    connect( this, SIGNAL( windowCloseRequested() ), _view,
        SIGNAL( windowCloseRequested() ), Qt::QueuedConnection );
    connect( this, SIGNAL( changeGeometry( const QRect& ) ), _view,
        SIGNAL( geometryChangeRequested( const QRect& ) ), Qt::QueuedConnection );
    connect( this, SIGNAL( createWindow( const QString&, const QString&, const QString& ) ),
        _view, SLOT( slotCreateWindow( const QString&, const QString&, const QString& ) ),
        Qt::QueuedConnection );
    connect( this, SIGNAL( requestPrint() ), _view, SLOT( slotRequestPrint() ),
        Qt::QueuedConnection );
}

PHIAScriptWindowObj::~PHIAScriptWindowObj()
{
    qDebug( "PHIAScriptWindowObj::~PHIAScriptWindowObj()" );
}

void PHIAScriptWindowObj::open( const QString &url, const QString &target, const QString &options )
{
    emit createWindow( url, target, options );
}

void PHIAScriptWindowObj::print()
{
    emit requestPrint();
}

void PHIAScriptWindowObj::resizeTo( qint32 w, qint32 h )
{
    emit changeGeometry( QRect( 0, 0, w, h ) );
}

void PHIAScriptWindowObj::resizeBy( qint32 dx, qint32 dy )
{
    emit changeGeometry( QRect( -1, -1, dx, dy ) );
}

qint32 PHIAScriptWindowObj::setTimeout( const QScriptValue &func, qint32 timeout )
{
    if ( !func.isFunction() && !func.isString() ) return 0;
    qint32 id=static_cast<qint32>(startTimer( static_cast<int>(timeout) ));
    if ( id==0 ) return id;
    _timeoutFunctions.insert( id, func );
    return id;
}

void PHIAScriptWindowObj::clearTimeout( qint32 id )
{
    if ( !_timeoutFunctions.contains( id ) ) return;
    killTimer( static_cast<int>(id) );
    _timeoutFunctions.remove( id );
}

qint32 PHIAScriptWindowObj::setInterval( const QScriptValue &func, qint32 timeout )
{
    if ( !func.isFunction() && !func.isString() ) return 0;
    qint32 id=static_cast<qint32>(startTimer( static_cast<int>(timeout) ));
    if ( id==0 ) return id;
    _intervalFunctions.insert( id, func );
    return id;
}

void PHIAScriptWindowObj::clearInterval( qint32 id )
{
    if ( !_intervalFunctions.contains( id ) ) return;
    killTimer( static_cast<int>(id) );
    _intervalFunctions.remove( id );
}

void PHIAScriptWindowObj::timerEvent( QTimerEvent *e )
{
    QScriptValue func, res;
    qint32 id=static_cast<qint32>(e->timerId());
    if ( _timeoutFunctions.contains( id ) ) {
        killTimer( static_cast<int>(id) );
        func=_timeoutFunctions.take( id );
        if ( func.isFunction() ) res=func.call();
        else res=_engine->evaluate( func.toString() );
    } else if ( _intervalFunctions.contains( id ) ) {
        func=_intervalFunctions.value( id );
        if ( func.isFunction() ) func.call();
        else res=_engine->evaluate( func.toString() );
        if ( res.isError() ) clearInterval( id );
    }
    if ( res.isError() ) _view->throwJavaScriptError( res );
    return QObject::timerEvent( e );
}

void PHIAScriptWindowObj::alert( const QString &s ) const
{
    QMessageBox::warning( _view, PHIA::browserName(), s, QMessageBox::Ok );
}

QString PHIAScriptWindowObj::prompt( const QString &text, const QString &value ) const
{
    return QInputDialog::getText( _view, PHIA::browserName(), text, QLineEdit::Normal, value );
}

bool PHIAScriptWindowObj::confirm( const QString &text )
{
    QMessageBox::Button res=QMessageBox::information( _view, PHIA::browserName(), text,
        QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel );
    if ( res==QMessageBox::Cancel ) return true;
    return false;
}

void PHIAScriptWindowObj::close()
{
    emit windowCloseRequested();
}

void PHIAScriptWindowObj::back()
{
    QTimer::singleShot( 0, _view, SLOT( slotBack() ) );
}

void PHIAScriptWindowObj::forward()
{
    QTimer::singleShot( 0, _view, SLOT( slotForward() ) );
}

void PHIAScriptWindowObj::home()
{
    emit homeRequested();
}

void PHIAScriptWindowObj::stop()
{
    QTimer::singleShot( 0, _view, SLOT( slotStop() ) );
}

PHIAScriptAjaxObj::PHIAScriptAjaxObj( PHIAWebView *view )
    : QObject( view->scriptEngine() ), _engine( view->scriptEngine() ), _view( view ), _reply( 0 )
{
    qDebug( "PHIAScriptAjaxObj::PHIAScriptAjaxObj()" );
    Q_ASSERT( _engine );
    _readyState=static_cast<quint8>(UNSENT);
    _codec=QTextCodec::codecForName( "utf-8" );
    _hasError=false;
    _running=false;
    _status=0;
    _statusText="";
}

PHIAScriptAjaxObj::~PHIAScriptAjaxObj()
{
    if ( _reply ) {
        _reply->abort();
        _reply->deleteLater();
    }
    qDebug( "PHIAScriptAjaxObj::~PHIAScriptAjaxObj()" );
}

void PHIAScriptAjaxObj::throwError( const QString &err )
{
    QScriptContext *ctx=_engine->currentContext();
    //QScriptContext *par=ctx->parentContext();
    QScriptValue v=ctx->throwError( QScriptContext::UnknownError, QString( "AJAX: " )+err );
    _view->throwJavaScriptError( v );
}

void PHIAScriptAjaxObj::open( const QString &method, const QString &url, bool async,
    const QString &user, const QString &passwd )
{
    if ( _reply ) { // stopping another request in progress
        _reply->abort();
        _reply->deleteLater();
        _reply=0;
    }
    QString methods=" GET POST DELETE PUT CREATE TRACE CONNECT TRACK ";
    if ( !methods.contains( method.toUpper() ) )
        return throwError( tr( "Unknown method '%1'." ).arg( method ) );
    _method=method.toUpper();
    if ( _method!="POST" && _method!="GET" && _method!="HEAD" )
        return throwError( tr( "Allowed methods are GET, POST and HEAD." ) );
    _async=async;
    _url=QUrl( url );
    if ( !_url.isValid() ) return throwError( tr( "URL is invalid '%1'." ).arg( _url.toString() ) );
    _url=PHI::createUrlForLink( _view->url(), url );
    if ( _view->url().host()!=_url.host() )
        return throwError( tr( "Security violation using a different host name." ) );
    _url.setUserName( user );
    _url.setPassword( passwd );
    _running=_hasError=false;
    _statusText="";
    _status=0;
    _requestHeaders.clear();
    _responseHeaders.clear();
    _responseText="";
    _responseXML="";
    _codec=QTextCodec::codecForName( "utf-8" );
    QScriptValue ajax=ajaxObjToScriptValue( _engine, this );
    if ( !ajax.isValid() ) return throwError( tr( "AJAX: invalid object." ) );

    _readyState=static_cast<quint8>(OPENED);
    QScriptValue func=ajax.property( "onreadystatechange" );
    if ( func.isFunction() ) {
        QScriptValue err=func.call( ajax );
        if ( err.isError() ) _view->throwJavaScriptError( err );
    }
}

void PHIAScriptAjaxObj::setRequestHeader( const QString &key, const QString &value )
{
    if ( _readyState!=static_cast<quint8>(OPENED) )
        return throwError( tr( "Could not set a header, wrong readyState.") );
    QStringList known;
    known << "accept-charset" << "accept-encoding" << "connection" << "content-length"
        << "cookie" << "cookie2" << "content-transfer-encoding" << "date" << "expect"
        << "host" << "keep-alive" << "referer" << "te" << "trailer" << "transfer-encoding"
        << "upgrade" << "user-agent" << "via";
    if ( known.contains( key.toLower(), Qt::CaseSensitive ) )
        return throwError( tr( "Header '%1' can not be overwritten." ).arg( key ) );
    if ( key.toLower().startsWith( "sec") || key.toLower().startsWith( "proxy" ) )
        return throwError( tr( "Header '%1' is not allowed to be set." ).arg( key ) );
    if ( _requestHeaders.contains( key ) ) {
        QString v=_requestHeaders.value( key );
        if ( !v.isEmpty() ) v=v+", "+value;
        _requestHeaders.insert( key, v );
    } else _requestHeaders.insert( key, value );
}

void PHIAScriptAjaxObj::send( const QString &body )
{
    if ( _readyState!=static_cast<quint8>(OPENED) )
        return throwError( tr( "Could not send, wrong readyState.") );
    if ( _running )
        return throwError( tr( "A request is already in progress.") );
    if ( !_async ) {
        /** @todo implement sync ajax. */
        return throwError( tr( "Phi does not yet support syncronus requests." ) );
    }

    QNetworkRequest req;
    QString key;
    foreach ( key, _requestHeaders.keys() ) {
        req.setRawHeader( key.toUtf8(), _requestHeaders.value( key ).toUtf8() );
    }
    req.setUrl( _url );

    if ( _method=="GET" ) _reply=_view->networkAccessManager()->get( req );
    else if ( _method=="HEAD" ) _reply=_view->networkAccessManager()->head( req );
    else if ( _method=="POST" ) {
        _reply=_view->networkAccessManager()->post( req, body.toUtf8() );
    }
    Q_ASSERT( _reply );
    _running=true;
    connect( _reply, SIGNAL( finished() ), this, SLOT( slotFinished() ) );
    connect( _reply, SIGNAL( metaDataChanged() ), this, SLOT( slotMetaDataChanged() ) );
    connect( _reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this,
        SLOT( slotNetworkError( QNetworkReply::NetworkError ) ) );
    connect( _reply, SIGNAL( readyRead() ), this, SLOT( slotNewBytesAvailable() ) );

    // historic call for onreadystatechange without a change (see specification)
    QScriptValue ajax=ajaxObjToScriptValue( _engine, this );
    QScriptValue func=ajax.property( "onreadystatechange" );
    if ( func.isFunction() ) {
        QScriptValue err=func.call( ajax );
        if ( err.isError() ) _view->throwJavaScriptError( err );
    }
}

void PHIAScriptAjaxObj::slotMetaDataChanged()
{
    qDebug( "slotMetaDataChanged();" );
    if ( !_reply ) return;
    QList <QByteArray> list=_reply->rawHeaderList();
    QByteArray header;
    _responseHeaders.clear();
    foreach ( header, list ) {
        if ( header.toLower().startsWith( "set-cookie" ) ) continue;
        _responseHeaders.insertMulti( QString::fromUtf8( header ),
            QString::fromUtf8( _reply->rawHeader( header ) ) );
    }
    //if ( list.contains( "Content-Length" ) ) qDebug( "Yipiee" );
    if ( _responseHeaders.contains( "Charset" ) ) {
        _codec=QTextCodec::codecForName( _responseHeaders.value( "Charset" ).toUtf8() );
        if ( !_codec ) _codec=QTextCodec::codecForName( "utf-8" );
    } else if ( _responseHeaders.contains( "Content-Type") ) {
        QString charset=_responseHeaders.value( "Content-Type" );
        charset.replace( QRegExp( ".*charset=" ), "" );
        if ( !charset.isEmpty() ) {
            _codec=QTextCodec::codecForName( charset.toUtf8() );
            if ( !_codec ) _codec=QTextCodec::codecForName( "utf-8" );
        }
    }
    _readyState=static_cast<quint8>(HEADERS_RECEIVED);
    QScriptValue ajax=ajaxObjToScriptValue( _engine, this );
    QScriptValue func=ajax.property( "onreadystatechange" );
    if ( func.isFunction() ) {
        QScriptValue err=func.call( ajax);
        if ( err.isError() ) _view->throwJavaScriptError( err );
    }
}

void PHIAScriptAjaxObj::slotNewBytesAvailable()
{
    qDebug( "slotNewBytesAvailable();" );
    if ( !_reply ) return;
    QString translated=_codec->toUnicode( _reply->readAll() );
    _responseText=_responseText+translated;
    _readyState=static_cast<quint8>(LOADING);
    QScriptValue ajax=ajaxObjToScriptValue( _engine, this );
    QScriptValue func=ajax.property( "onreadystatechange" );
    if ( func.isFunction() ) {
        QScriptValue err=func.call( ajax );
        if ( err.isError() ) _view->throwJavaScriptError( err );
    }
}

QString PHIAScriptAjaxObj::getAllResponseHeaders() const
{
    if ( _hasError || _readyState==static_cast<quint8>(UNSENT)
        || _readyState==static_cast<quint8>(OPENED) ) return QString();
    QString key, string;
    foreach ( key, _responseHeaders.keys() ) {
        string=string+key+": "+_responseHeaders.value( key )+"\r\n";
    }
    string.chop( 2 );
    return string;
}

QString PHIAScriptAjaxObj::getResponseHeader( const QString &key ) const
{
    if ( _hasError || _readyState==static_cast<quint8>(UNSENT)
        || _readyState==static_cast<quint8>(OPENED) ) return QString();
    return _responseHeaders.value( key, QString() );
}

void PHIAScriptAjaxObj::abort()
{
    if ( _reply ) {
        _reply->abort();
        _reply->deleteLater();
        _reply=0;
    }
    _hasError=true;
    if ( _readyState==static_cast<quint8>(UNSENT) ) return;
    if ( _readyState==static_cast<quint8>(OPENED) && !_running ) {
        _readyState=static_cast<quint8>(UNSENT);
        return;
    }
    if ( _readyState==static_cast<quint8>(DONE) ) {
        _readyState=static_cast<quint8>(UNSENT);
        return;
    }
    _running=false;
    _readyState=static_cast<quint8>(DONE);
    QScriptValue ajax=ajaxObjToScriptValue( _engine, this );
    QScriptValue func=ajax.property( "onreadystatechange" );
    if ( func.isFunction() ) {
        QScriptValue err=func.call( ajax );
        if ( err.isError() ) _view->throwJavaScriptError( err );
    }
    _readyState=static_cast<quint8>(UNSENT);
}

void PHIAScriptAjaxObj::slotFinished()
{
    qDebug( "slotFinished();" );
    if ( _reply ) {
        _status=_reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
        _statusText=QString::fromLatin1(
            _reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toByteArray() );
        QString translated=_codec->toUnicode( _reply->readAll() );
        _responseText=_responseText+translated;
        _reply->deleteLater();
        _reply=0;
    }
    _running=false;
    _readyState=static_cast<quint8>(DONE);
    QScriptValue ajax=ajaxObjToScriptValue( _engine, this );
    QScriptValue func=ajax.property( "onreadystatechange" );
    if ( func.isFunction() ) {
        QScriptValue err=func.call( ajax );
        if ( err.isError() ) _view->throwJavaScriptError( err );
    }
}

void PHIAScriptAjaxObj::slotNetworkError( QNetworkReply::NetworkError )
{
    qDebug( "slotNetworkError();" );
    _hasError=true;
    //if ( _reply ) {
    //    throwError( _reply->errorString() );
    //}
}

QScriptValue ajaxObjToScriptValue( QScriptEngine *engine, PHIAScriptAjaxObj* const &obj )
{
    return engine->newQObject( obj, QScriptEngine::QtOwnership,
        QScriptEngine::PreferExistingWrapperObject |
        QScriptEngine::ExcludeSuperClassContents | QScriptEngine::ExcludeDeleteLater );
}

void ajaxObjFromScriptValue( const QScriptValue &val, PHIAScriptAjaxObj* &obj )
{
    obj=qobject_cast<PHIAScriptAjaxObj*>(val.toQObject());
}

PHIAScriptFormsObj::PHIAScriptFormsObj( PHIAWebView *view )
    : QObject( view->scriptEngine() ), _view( view )
{
    qDebug( "PHIAScriptFormsObj::PHIAScriptFormsObj()" );
    connect( this, SIGNAL( submitRequest( const QString& ) ), _view,
        SLOT( slotSubmitForm( const QString&) ), Qt::QueuedConnection );
    connect( this, SIGNAL( resetRequest() ), _view, SLOT( slotResetForm() ), Qt::QueuedConnection );
    connect( _view, SIGNAL( checkSubmit( const QString& ) ), this, SLOT( submit( const QString& ) ) );
}

PHIAScriptFormsObj::~PHIAScriptFormsObj()
{
    qDebug( "PHIAScriptFormsObj::~PHIAScriptFormsObj()" );
}

void PHIAScriptFormsObj::submit( const QString &buttonid )
{
    qDebug( "PHIAScriptFormsObj::submit()" );
    QScriptValue phi=_view->scriptEngine()->globalObject().property( "phi" );
    if ( phi.isValid() ) {
        QScriptValue func=phi.property( "onsubmit" );
        QScriptValue res;
        if ( func.isFunction() ) res=func.call( phi );
        else emit submitRequest( buttonid );
        if ( res.isValid() && res.toBool() ) emit submitRequest( buttonid );
        else if ( res.isError() ) {
            _view->throwJavaScriptError( res );
        }
    } else submitRequest( buttonid );
}

void PHIAScriptFormsObj::reset()
{
    emit resetRequest();
}

PHIAScriptLocationObj::PHIAScriptLocationObj( PHIAWebView *view )
    : QObject( view->scriptEngine() )
{
    qDebug( "PHIAScriptLocationObj::PHIAScriptLocationObj()" );
    _url=view->url();
    connect( this, SIGNAL( hrefRequested( const QUrl& ) ), view,
        SLOT( slotLinkRequested( const QUrl& ) ), Qt::QueuedConnection );
}

PHIAScriptLocationObj::~PHIAScriptLocationObj()
{
    qDebug( "PHIAScriptLocationObj::~PHIAScriptLocationObj()" );
}

void PHIAScriptLocationObj::setHref( const QString &h )
{
    QUrl url=PHI::createUrlForLink( _url, h );
    emit hrefRequested( url );
}

void PHIAScriptLocationObj::reload()
{
    emit hrefRequested( _url );
}

PHIAScriptNavigatorObj::PHIAScriptNavigatorObj( PHIAWebView *view )
    : QObject( view->scriptEngine() ), _view( view )
{
    qDebug( "PHIAScriptNavigatorObj::PHIAScriptNavigatorObj()" );
    QScriptEngine *engine=view->scriptEngine();
    Q_ASSERT( engine );

    QScriptValue navigator=engine->newQObject( this, PHIASCRIPTEXTENSION );
    engine->globalObject().setProperty( "navigator", navigator );
}

PHIAScriptNavigatorObj::~PHIAScriptNavigatorObj()
{
    qDebug( "PHIAScriptNavigatorObj::PHIAScriptNavigatorObj()" );
}

QString PHIAScriptNavigatorObj::appCodeName() const
{
    return PHIA::browserName();
}

QString PHIAScriptNavigatorObj::appName() const
{
    return PHIA::browserName();
}

QString PHIAScriptNavigatorObj::appVersion() const
{
    return PHIA::libVersion();
}

QString PHIAScriptNavigatorObj::language() const
{
    return QLocale::system().name();
}

QString PHIAScriptNavigatorObj::platform() const
{
    return PHISysInfo::systemString();
}

QString PHIAScriptNavigatorObj::userAgent() const
{
    return PHIANetManager::instance()->userAgent();
}

PHIAScriptPhiObj::PHIAScriptPhiObj( PHIAWebView *view )
    : QObject( view->scriptEngine() ), _view( view )
{
    qDebug( "PHIAScriptPhiObj::PHIAScriptPhiObj()" );
    QScriptEngine *engine=view->scriptEngine();
    Q_ASSERT( engine );

    //qScriptRegisterMetaType( engine, baseItemToScriptValue, baseItemFromScriptValue );
    qScriptRegisterMetaType( engine, ajaxObjToScriptValue, ajaxObjFromScriptValue );
    QScriptValue phi=engine->newQObject( this, PHIASCRIPTEXTENSION );
    engine->globalObject().setProperty( "phi", phi );

    QScriptValue ajax=engine->newQObject( new PHIAScriptAjaxObj( _view ), PHIASCRIPTEXTENSION );
    phi.setProperty( "ajax", ajax );

    connect( this, SIGNAL( hrefRequested( const QUrl& ) ), _view,
        SLOT( slotLinkRequested( const QUrl& ) ), Qt::QueuedConnection );
}

PHIAScriptPhiObj::~PHIAScriptPhiObj()
{
    qDebug( "PHIAScriptPhiObj::~PHIAScriptPhiObj()" );
}

PHIBaseItem* PHIAScriptPhiObj::getElementById( const QString &id )
{
    return _view->page()->getElementById( id );
}

void PHIAScriptPhiObj::setVisible( const QString &id, bool visible )
{
    //qWarning( "setVisible( %s, %d )", qPrintable( id ), visible );
    PHIBaseItem *it=_view->page()->getElementById( id );
    if ( !it ) return;
    it->setVisible( visible );
}

/*
void PHIAScriptPhiObj::toggle( const QString &id )
{
    PHIBaseItem *it=_view->page()->getElementById( id );
    if ( !it ) return;
    if ( it->visible() ) it->setVisible( false );
    else it->setVisible( true );
}
*/

void PHIAScriptPhiObj::setOpacity( const QString &id, qreal opac )
{
    PHIBaseItem *it=_view->page()->getElementById( id );
    if ( !it ) return;
    it->setOpacity( opac );
}

void PHIAScriptPhiObj::href( const QString &l )
{
    QString link=l;
    if ( link.startsWith( "//" ) || link.startsWith( "\\\\" ) ) link.remove( 0, 1 );
    if ( link.startsWith( "mailto:" ) ) {
        //QString mail=link;
        //mail.remove( 0, 7 );
        QDesktopServices::openUrl( link );
        return;
    }
    qDebug( "----------------------- %s %s", qPrintable( link ), qPrintable( _view->url().toString( ) ) );
    QUrl url=PHI::createUrlForLink( _view->url(), link );
    QUrlQuery query( url );
    //if ( _view->url().host()==url.host() ) { // same host so add session and lang
        QString sid=_view->page()->session();
        QString philang=_view->page()->lang();
        if ( !sid.isEmpty() )
            if ( !query.hasQueryItem( "phisid" ) ) query.addQueryItem( "phisid", sid );
        if ( !philang.isEmpty() )
            if ( !query.hasQueryItem( "philang" ) ) query.addQueryItem( "philang", philang );
    //}
    url.setQuery( query );
    emit hrefRequested( url );
}

void PHIAScriptPhiObj::fadeIn( const QString &id, qint32 start, qint32 duration,
    qreal maxOpac, const QString &ease )
{
    PHIBaseItem *it=_view->page()->getElementById( id );
    if ( !it ) return;
    it->setFadeIn( start, duration, maxOpac, ease );
}

void PHIAScriptPhiObj::fadeOut( const QString &id, qint32 start, qint32 duration,
    qreal minOpac, const QString &ease )
{
    PHIBaseItem *it=_view->page()->getElementById( id );
    if ( !it ) return;
    it->setFadeOut( start, duration, minOpac, ease );
}

void PHIAScriptPhiObj::moveTo( const QString &id, qint32 left, qint32 top, qint32 start,
    qint32 duration, const QString &ease )
{
    PHIBaseItem *it=_view->page()->getElementById( id );
    if ( !it ) return;
    it->setMoveTo( start, duration, left, top, ease );
}

/*
void PHIAScriptPhiObj::moveBy( const QString &id, qint32 x, qint32 y, qint32 w, qint32 h,
    qint32 start, qint32 duration, const QString &ease )
{
    PHIBaseItem *it=_view->page()->getElementById( id );
    if ( !it ) return;
    it->setMoveBy( start, duration, x, y, w, h, ease );
}
*/

void PHIAScriptPhiObj::rotateIn( const QString &id, quint8 axis, qint32 start,
    qint32 duration, const QString &ease )
{
    PHIBaseItem *it=_view->page()->getElementById( id );
    if ( !it ) return;
    it->setRotateIn( axis, start, duration, ease );
}

void PHIAScriptPhiObj::rotateOut( const QString &id, quint8 axis, qint32 start,
    qint32 duration, const QString &ease )
{
    PHIBaseItem *it=_view->page()->getElementById( id );
    if ( !it ) return;
    it->setRotateOut( axis, start, duration, ease );
}

void PHIAScriptPhiObj::rotate( const QString &id, quint8 axis, qreal step, const QString &ease )
{
    PHIBaseItem *it=_view->page()->getElementById( id );
    if ( !it ) return;
    qreal xstep=0, ystep=0, zstep=0;
    if ( axis & 0x1 ) xstep=step;
    if ( axis & 0x2 ) ystep=step;
    if ( axis & 0x4 ) zstep=step;
    it->setRotate( axis, xstep, ystep, zstep, ease );
}

/*
void PHIAScriptPhiObj::stopAnimations( const QString &id )
{
    PHIBaseItem *it=_view->page()->getElementById( id );
    if ( !it ) return;
    it->stopAnimations();
}

void PHIAScriptPhiObj::setCursor( const QString &id, const QString &shape )
{
    PHIBaseItem *it=_view->page()->getElementById( id );
    if ( !it ) return;
    it->setCursor( shape.toLatin1() );
}

void PHIAScriptPhiObj::setHtml( const QString &id, const QString &html )
{
    PHIBaseItem *it=_view->page()->getElementById( id );
    if ( !it ) return;
    if ( it->wid()!=PHI::RICH_TEXT ) return;
    it->setValue( html );
}

void PHIAScriptPhiObj::setProgress( const QString &id, const QString &value )
{
    PHIBaseItem *it=_view->page()->getElementById( id );
    if ( !it ) return;
    if ( it->wid()!=PHI::PROGRESSBAR ) return;
    it->setValue( value );
}
*/

void PHIAScriptPhiObj::alert( const QString &s ) const
{
    QMessageBox::warning( _view, PHIA::browserName(), s, QMessageBox::Ok );
}

void PHIAScriptPhiObj::setData( const QString &id, const QString &data, const QString &delimiter )
{
    PHIBaseItem *it=_view->page()->getElementById( id );
    if ( !it ) return;
    switch ( it->wid() ) {
    case PHI::LIST:
    case PHI::COUNTRY:
    case PHI::COMBO_BOX:
    case PHI::LANG_SELECTOR: break;
    default: return;
    }

    it->setDelimiter( delimiter );
    it->setValue( data );
}

PHIAScriptMenuObj::PHIAScriptMenuObj( PHIAWebView *view, QMenuBar *menubar )
    : QObject( view->scriptEngine() ), _menubar( menubar ), _engine( view->scriptEngine() ),
    _view( view )
{
    qDebug( "PHIAScriptMenuObj::PHIAScriptMenuObj()" );
    Q_ASSERT( _view );
    Q_ASSERT( _engine );
    if ( !_menubar ) return;

    QScriptValue phi=_engine->globalObject().property( "phi" );
    QScriptValue m=_engine->newQObject( this, PHIASCRIPTEXTENSION );
    phi.setProperty( "menu", m );

    QList <PHIPageMenuEntry> menuEntries=_view->page()->menuEntries();
    QMenu *menu;
    PHIPageMenuEntry entry;

    foreach ( entry, menuEntries ) {
        qDebug( "menu text %s", qPrintable( entry.text() ) );
        qDebug( "menu parent %s", qPrintable( entry.parent() ) );
        if ( entry.parent().isEmpty() ) { // top level (menu header)
            if ( entry.text()=="#-" ) _menubar->addSeparator();
            else {
                menu=_menubar->addMenu( entry.text() );
                connect( menu, SIGNAL( triggered( QAction* ) ), this,
                    SLOT( slotMenuTriggered( QAction* ) ) );
                if ( entry.options() & PHIPageMenuEntry::Disabled ) menu->setEnabled( false );
                _menus.insert( entry.id(), menu );
            }
        } else {
            menu=_menus.value( entry.parent() );
            Q_ASSERT( menu );
            if ( entry.text()=="#-" ) menu->addSeparator();
            else {
                QString text=entry.text();
                QString shortcut=entry.text();
                text.replace( QRegExp( "\\[.*" ), "" );
                shortcut.replace( QRegExp( ".*\\[" ), "" );
                shortcut.replace( QRegExp( "\\].*" ), "" );
                if ( entry.options() & PHIPageMenuEntry::SubMenu ) {
                    menu=menu->addMenu( QIcon( entry.pixmap() ), text );
                    if ( entry.options() & PHIPageMenuEntry::Disabled ) menu->setEnabled( false );
                    _menus.insert( entry.id(), menu );
                } else {
                    QAction *act=menu->addAction( QIcon( entry.pixmap() ), text );
                    act->setShortcut( QKeySequence( shortcut ) );
                    if ( entry.options() & PHIPageMenuEntry::Checkable ) {
                        act->setCheckable( true );
                        if ( entry.options() & PHIPageMenuEntry::Checked ) act->setChecked( true );
                    }
                    if ( entry.options() & PHIPageMenuEntry::Disabled ) act->setEnabled( false );
                    act->setData( entry.id() );
                    _actions.insert( entry.id(), act );
                }
            }
        }
    }
}

PHIAScriptMenuObj::~PHIAScriptMenuObj()
{
    qDebug( "PHIAScriptMenuObj::~PHIAScriptMenuObj()" );
}

void PHIAScriptMenuObj::throwError( const QString &err )
{
    QScriptContext *ctx=_engine->currentContext();
    //QScriptContext *par=ctx->parentContext();
    QScriptValue v=ctx->throwError( QScriptContext::ReferenceError, err );
    _view->throwJavaScriptError( v );
}

void PHIAScriptMenuObj::slotMenuTriggered( QAction *act )
{
    //qDebug( "Action tiggerd %s", qPrintable( act->data().toString() ) );
    QScriptValue phi=_engine->globalObject().property( "phi" );
    QScriptValue menu=phi.property( "menu" );
    if ( !menu.isValid() ) return;

    QScriptValue func=menu.property( "activated" );
    if ( func.isFunction() ) {
        QScriptValue err=func.call( QScriptValue(), QScriptValueList()
        << act->data().toString() << act->isChecked() );
        if ( err.isError() ) _view->throwJavaScriptError( err );
    }
}

bool PHIAScriptMenuObj::isCheckable( const QString &id )
{
    QAction *act=_actions.value( id );
    if ( !act ) return false;
    return act->isCheckable();
}

void PHIAScriptMenuObj::setEnabled( const QString &id, bool enable )
{
    QMenu *m=_menus.value( id );
    if ( m ) return m->setEnabled( enable );
    QAction *act=_actions.value( id );
    if ( act ) return act->setEnabled( enable );
    throwError( tr( "Menu item with ID '%1' not found." ).arg( id ) );
}

bool PHIAScriptMenuObj::isEnabled( const QString &id )
{
    QAction *act=_actions.value( id );
    if ( act ) return act->isEnabled();
    QMenu *m=_menus.value( id );
    if ( m ) return m->isEnabled();
    throwError( tr( "Menu or menu item with ID '%1' not found." ).arg( id ) );
    return false;
}

void PHIAScriptMenuObj::setChecked( const QString &id, bool check )
{
    QAction *act=_actions.value( id );
    if ( act ) {
        if ( act->isCheckable() ) return act->setChecked( check );
        throwError( tr( "Menu item with ID '%1' is not checkable." ).arg( id ) );
    }
    throwError( tr( "Menu item with ID '%1' not found." ).arg( id ) );
}

bool PHIAScriptMenuObj::isChecked( const QString &id )
{
    QAction *act=_actions.value( id );
    if ( act ) return act->isChecked();
    throwError( tr( "Menu item with ID '%1' not found." ).arg( id ) );
    return false;
}

void PHIAScriptMenuObj::setText( const QString &id, const QString &text )
{
    QMenu *m=_menus.value( id );
    if ( m ) return m->setTitle( text );
    QAction *act=_actions.value( id );
    if ( act ) return act->setText( text );
    throwError( tr( "Menu or menu item with ID '%1' not found." ).arg( id ) );
}

QString PHIAScriptMenuObj::text( const QString &id )
{
    QMenu *m=_menus.value( id );
    if ( m ) return m->title();
    QAction *act=_actions.value( id );
    if ( act ) return act->text();
    throwError( tr( "Menu or menu item with ID '%1' not found." ).arg( id ) );
    return QString();
}

void PHIAScriptMenuObj::setShortcut( const QString &id, const QString &sc )
{
    QAction *act=_actions.value( id );
    if ( act ) return act->setShortcut( QKeySequence( sc ) );
    throwError( tr( "Menu item with ID '%1' not found." ).arg( id ) );
}

QString PHIAScriptMenuObj::shortcut( const QString &id )
{
    QAction *act=_actions.value( id );
    if ( act ) return act->shortcut().toString();
    throwError( tr( "Menu item with ID '%1' not found." ).arg( id ) );
    return QString();
}

void PHIAScriptMenuObj::addMenuItem( const QString &parId, const QString &id,
    const QString &text, const QString &sc )
{
    QMenu *m=_menus.value( parId );
    if ( !m ) {
        throwError( tr( "Parent menu with ID '%1' not found." ).arg( parId ) );
        return;
    }
    QAction *act=_actions.value( id );
    if ( act ) {
        throwError( tr( "Menu item with ID '%1' already exists." ).arg( id ) );
        return;
    }
    act=m->addAction( text );
    act->setShortcut( QKeySequence( sc ) );
    act->setData( id );
    _actions.insert( id, act );
}

void PHIAScriptMenuObj::addCheckableMenuItem( const QString &parId, const QString &id,
    const QString &text, bool isChecked, const QString &sc )
{
    QMenu *m=_menus.value( parId );
    if ( !m ) {
        throwError( tr( "Parent menu with ID '%1' not found." ).arg( parId ) );
        return;
    }
    QAction *act=_actions.value( id );
    if ( act ) {
        throwError( tr( "Menu item with ID '%1' already exists." ).arg( id ) );
        return;
    }
    act=m->addAction( text );
    act->setCheckable( true );
    act->setChecked( isChecked );
    act->setShortcut( QKeySequence( sc ) );
    act->setData( id );
    _actions.insert( id, act );
}

void PHIAScriptMenuObj::addMenu( const QString &id, const QString &text,
    const QString &parId )
{
    QMenu *m=_menus.value( id );
    QAction *act=_actions.value( id );
    if ( m || act ) {
        throwError( tr( "Menu or menu item with ID '%1' already exists." ).arg( id ) );
        return;
    }
    if ( parId.isEmpty() ) {
        m=_menubar->addMenu( text );
        connect( m, SIGNAL( triggered( QAction* ) ), this,
            SLOT( slotMenuTriggered( QAction* ) ) );
        _menus.insert( id, m );
        return;
    }
    m=_menus.value( parId );
    if ( m ) {
        m=m->addMenu( text );
        _menus.insert( id, m );
    } else {
        throwError( tr( "Menu item with ID '%1' not found." ).arg( parId ) );
    }
}

void PHIAScriptMenuObj::removeMenuItem( const QString &id )
{
    QAction *act=_actions.value( id );
    if ( !act ) return throwError( tr( "Menu item with ID '%1' not found." ).arg( id ) );
    delete _actions.take( id );
}

/*
void PHIAScriptMenuObj::removeMenu( const QString &id )
{
    QMenu *m=_menus.value( id );
    if ( !m ) return throwError( tr( "Menu with ID '%1' not found." ).arg( id ) );
    delete _menus.take( id );
}
*/

#undef PHIASCRIPTEXTENTION
