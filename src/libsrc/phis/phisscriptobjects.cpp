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

#define PHISSCRIPTEXTENSION QScriptEngine::QtOwnership, QScriptEngine::PreferExistingWrapperObject |\
    QScriptEngine::ExcludeSuperClassContents | QScriptEngine::ExcludeDeleteLater

PHISServerObj::PHISServerObj( const PHISRequest *req, QScriptEngine *engine, PHIResponseRec *resp )
    : QObject( engine ), _req( req ), _resp( resp )
{
    qDebug( "PHISServerObj::PHISServerObj()" );
    setObjectName( "server" );
    QScriptValue server=engine->newQObject( this, PHISSCRIPTEXTENSION );
    engine->globalObject().setProperty( "server", server );
}

PHISServerObj::~PHISServerObj()
{
    qDebug( "PHISServerObj::~PHISServerObj()" );
}

void PHISServerObj::log( const QString &t )
{
    _resp->log( PHILOGUSER, PHIRC_USER, t );
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

PHISSystemObj::PHISSystemObj( QScriptEngine *engine, PHIResponseRec *resp )
    : QObject( engine ), _resp( resp ), _exitCode( 0 )
{
    qDebug( "PHISSystemObj::PHISSystemObj()" );
    setObjectName( "system" );
    QScriptValue system=engine->newQObject( this, PHISSCRIPTEXTENSION );
    engine->globalObject().setProperty( "system", system );
}

PHISSystemObj::~PHISSystemObj()
{
    qDebug( "PHISSystemObj::~PHISSystemObj()" );
}

void PHISSystemObj::log( const QString &t )
{
    _resp->log( PHILOGUSER, PHIRC_USER, t );
}

QString PHISSystemObj::toLocaleFormat( const QString &s )
{
    // translate de-de -> de_DE
    if ( s.size()==2 ) return s;
    if ( s.size()==5 ) {
        QString n=s.left( 3 );
        n.replace( 2, 1, '_' );
        n.append( s.right( 2 ).toUpper() );
        return n;
    }
    return s.left( 2 );
}

QString PHISSystemObj::exec( const QString &name, const QString &a, const QString &codec )
{
    QStringList args=a.split( QRegExp( "\\s" ), QString::SkipEmptyParts );
    return PHISSystemObj::exec( name, args, codec );
}

QString PHISSystemObj::exec(const QString &name, const QStringList &args, const QString &codecName )
{
    QProcess proc;
    _lastError="";
    proc.start( name, args, QIODevice::ReadOnly );
    if ( !proc.waitForStarted() ) {
        _resp->log( PHILOGERR, PHIRC_OBJ_ACCESS_ERROR,
            tr( "Could not start process '%1' with arguments '%2'." )
            .arg( name ).arg( args.join( " " ) )
            +PHI::errorText().arg( proc.errorString() ) );
        _lastError=proc.errorString();
        _exitCode=proc.exitCode();
        return QString();
    }
    if ( !proc.waitForFinished() ) {
        _resp->log( PHILOGERR, PHIRC_OBJ_ACCESS_ERROR,
            tr( "Could not finish process '%1' with arguments '%2'.")
            .arg( name ).arg( args.join( " " ) )
            +PHI::errorText().arg( proc.errorString() ) );
        _lastError=proc.errorString();
        _exitCode=proc.exitCode();
        return QString();
    }
    if ( proc.exitCode() ) {
        _resp->log( PHILOGWARN, PHIRC_OBJ_ACCESS_ERROR,
            tr( "Process '%1' exit code is '%2'.")
            .arg( name ).arg( proc.exitCode() )
            +PHI::errorText().arg( QString::fromUtf8( proc.readAllStandardError() ) ) );
        _lastError=proc.errorString();
        _exitCode=proc.exitCode();
    }
    QTextCodec *codec=QTextCodec::codecForName( codecName.toUtf8() );
    if ( !codec ) codec=QTextCodec::codecForLocale();
    return codec->toUnicode( proc.readAllStandardOutput() );
}

PHISFileObj::PHISFileObj( QScriptEngine *engine, PHIResponseRec *resp )
    : QObject( engine ), _resp( resp ), _handle( 0 )
{
    qDebug( "PHISFileObj::PHISFileObj()" );
    setObjectName( "file" );
    QScriptValue file=engine->newQObject( this, PHISSCRIPTEXTENSION );
    engine->globalObject().setProperty( "file", file );
}

PHISFileObj::~PHISFileObj()
{
    QFile *f;
    foreach( f, _files ) delete f;
    // _files.clear();
    qDebug( "PHISFileObj::~PHISFileObj()" );
}

int PHISFileObj::open( const QString &fn, int mode ) {
    QFile *f=new QFile( fn );
    f->setPermissions( QFile::ReadUser | QFile::WriteUser | QFile::ReadOwner
        | QFile::WriteOwner | QFile::ReadGroup );
    if ( !f->open( static_cast<QIODevice::OpenMode>(mode) ) ) {
        _error=f->errorString();
        _resp->log( PHILOGWARN, PHIRC_IO_FILE_ACCESS_ERROR, _error );
        delete f;
        return 0;
    }
    _files.insert( ++_handle, f );
    return _handle;
}

QString PHISFileObj::read( int handle, const QString &codecName ) const
{
    if ( !_files.contains( handle ) ) {
        _error=tr( "File handle '%1' not found." ).arg( handle );
        return QString();
    }
    QTextCodec *codec=QTextCodec::codecForName( codecName.toUtf8() );
    if ( !codec ) codec=QTextCodec::codecForLocale();
    try {
        QByteArray data=_files.value( handle )->readAll();
        QString tmp=codec->toUnicode( data );
        return tmp;
    } catch ( std::bad_alloc& ) {
        _error=tr( "Could not read data for handle '%1' - out of memory." ).arg( handle );
        _resp->log( PHILOGERR, PHIRC_OUT_OF_MEMORY, _error );
    }
    return QString();
}

bool PHISFileObj::write( int handle, const QString &data, const QString &codecName )
{
    if ( !_files.contains( handle ) ) {
        _error=tr( "File handle '%1' not found." ).arg( handle );
        return false;
    }
    QTextCodec *codec=QTextCodec::codecForName( codecName.toUtf8() );
    if ( !codec ) codec=QTextCodec::codecForLocale();
    try {
        QByteArray arr=codec->fromUnicode( data );
        if ( _files.value( handle )->write( arr )==-1 ) {
            _resp->log( PHILOGWARN, PHIRC_IO_FILE_ACCESS_ERROR, error( handle ) );
            return false;
        }
    } catch ( std::bad_alloc& ) {
        _resp->log( PHILOGERR, PHIRC_OUT_OF_MEMORY, _error );
        _error=tr( "Could not write data for handle '%1' - out of memory." ).arg( handle );
        return false;
    }
    return true;
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

PHISEmailObj::PHISEmailObj( const PHISRequest *req, QScriptEngine *engine, PHIResponseRec *resp )
    : QObject( engine ), _timeout( 20000 ), _port( 25 ), _resp( resp ),
    _req( req ), _socket( 0 )
{
    qDebug( "PHISEmailObj::PHISEmailObj()" );
    setObjectName( "email" );
    QScriptValue email=engine->newQObject( this, PHISSCRIPTEXTENSION );
    engine->globalObject().setProperty( "email", email );
    _from=_req->admin();
    _contentType="text/plain; charset=utf-8\n";
    _socket=new QTcpSocket( this );
}

PHISEmailObj::~PHISEmailObj()
{
    if ( _socket->state()==QTcpSocket::ConnectedState ) {
        QTextStream t( _socket );
        t.setCodec( "utf-8" );
        disconnectFromServer( 0, t );
    }
    qDebug( "PHISEmailObj::~PHISEmailObj()" );
}

qint16 PHISEmailObj::disconnectFromServer( qint16 err, QTextStream &t )
{
    if ( _socket->state()!=QTcpSocket::ConnectedState ) return 0;
    if ( err ) {
        QString string=tr( "Could not send email: <%1>." ).arg( _lastError );
        _resp->log( PHILOGERR, PHIRC_PROT_CMD_ERROR, string );
    }
    waitForDataWritten( QString( "QUIT" ), t );
    if ( !_socket->waitForDisconnected( _timeout ) ) {
        _lastError=tr( "Could not cleanly disconnect from SMTP server '%1': <%2>." ).arg( _server )
            .arg( _socket->errorString() );
        _resp->log( PHILOGERR, PHI::socketError( _socket->error() ), _lastError );
        if ( err==0 ) err=1;
    }
    _socket->close();
    return err;
}

qint16 PHISEmailObj::disconnect()
{
    QTextStream t( _socket );
    t.setCodec( "utf-8" );
    return disconnectFromServer( 0, t );
}

qint16 PHISEmailObj::connect()
{
    if ( _socket->isOpen() ) {
        _lastError=tr( "Connection to SMTP server '%1' already established." ).arg( _server );
        _resp->log( PHILOGWARN, PHIRC_ARGUMENT_ERROR, _lastError );
        return 0;
    }
    QHostInfo info=QHostInfo::fromName( _server );
    if ( info.addresses().isEmpty() ) {
        _lastError=tr( "DNS lookup error for SMTP server '%1': <%2>." )
            .arg( _server ).arg( info.errorString() );
        _resp->log( PHILOGERR, PHIRC_TCP_HOST_NOT_FOUND_ERROR, _lastError );
        return 1;
    }
    QHostAddress server=info.addresses().first();
    _socket->connectToHost( server, _port );
    if ( !_socket->waitForConnected( _timeout ) ) {
        _lastError=tr( "Could not connect to SMTP server '%1': <%2>." ).arg( _server )
            .arg( _socket->errorString() );
        _resp->log( PHILOGERR, PHI::socketError( _socket->error() ), _lastError );
        return 1;
    }
    QTextStream t( _socket );
    t.setCodec( "utf-8" );
    bool moreLines;
    qint16 res=waitForResponseLine();
    //qDebug( "EMAIL %d %s", res, qPrintable( _lastError ) );
    QString data="EHLO "+_req->hostname();
    if ( res!=220 ) return disconnectFromServer( res, t );
    res=waitForDataWritten( data, t );
    if ( res ) return disconnectFromServer( res, t );
    moreLines=true;
    while( moreLines ) {
        res=waitForResponseLine( &moreLines );
        if ( res!=250 ) return disconnectFromServer( res, t );
    }
    if ( !_user.isEmpty() && !_password.isEmpty() ) {
        res=waitForDataWritten( QString( "AUTH LOGIN" ), t );
        if ( res ) return disconnectFromServer( res, t );
        res=waitForResponseLine();
        if ( res!=334 ) return disconnectFromServer( res, t );
        data=QString::fromLatin1( _user.toLatin1().toBase64() );
        res=waitForDataWritten( data, t );
        if ( res ) return disconnectFromServer( res, t );
        res=waitForResponseLine();
        if ( res!=334 ) return disconnectFromServer( res, t );
        data=QString::fromLatin1( _password.toLatin1().toBase64() );
        res=waitForDataWritten( data, t );
        if ( res ) return disconnectFromServer( res, t );
        res=waitForResponseLine();
        if ( res!=235 ) return disconnectFromServer( res, t );
    }
    return 0;
}

qint16 PHISEmailObj::send()
{
    if ( _to.isEmpty() && _recipients.isEmpty() ) {
        _lastError=tr( "No email recipient(s) specified." );
        _resp->log( PHILOGERR, PHIRC_ARGUMENT_ERROR, _lastError );
        return 1;
    }
    qint16 res;
    res=connect();
    if ( res ) return res;
    QTextStream t( _socket );
    t.setCodec( "utf-8" );

    qDebug( "EMAIL %d %s", res, qPrintable( _lastError ) );
    QString data="MAIL FROM: <"+_from+'>';
    res=waitForDataWritten( data, t );
    if ( res ) return disconnectFromServer( res, t );
    res=waitForResponseLine();
    qDebug( "EMAIL %d %s", res, qPrintable( _lastError ) );
    if ( res!=250 ) return disconnectFromServer( res, t );
    if ( _recipients.count() ) {
        QString rcp;
        foreach ( rcp, _recipients ) {
            data="RCPT TO: <"+rcp+'>';
            res=waitForDataWritten( data, t );
            if ( res ) return disconnectFromServer( res, t );
            qDebug( "EMAIL %s %s", qPrintable( rcp ), qPrintable( _lastError ) );
            res=waitForResponseLine();
            if ( res!=250 ) {
                data=tr( "SMTP error for recipient '%1': <%2>." ).arg( rcp ).arg( _lastError );
                _resp->log( PHILOGWARN, PHIRC_PROT_CMD_ERROR, data );
            }
        }
    } else {
        data="RCPT TO: "+_to;
        res=waitForDataWritten( data, t );
        if ( res ) return disconnectFromServer( res, t );
        res=waitForResponseLine();
        qDebug( "EMAIL %d %s", res, qPrintable( _lastError ) );
        if ( res!=250 ) return disconnectFromServer( res, t );
    }
    res=waitForDataWritten( QString( "DATA" ), t );
    if ( res ) return disconnectFromServer( res, t );
    res=waitForResponseLine();
    qDebug( "EMAIL %d %s", res, qPrintable( _lastError ) );
    if ( res!=354 ) return disconnectFromServer( res, t );
    res=waitForMessageWritten( t );
    if ( res ) return disconnectFromServer( res, t );
    res=waitForResponseLine();
    qDebug( "EMAIL %d %s", res, qPrintable( _lastError ) );
    if ( res!=250 ) return disconnectFromServer( res, t );
    return disconnectFromServer( 0, t );
}

qint16 PHISEmailObj::sendTo( const QString &to, const QString &subject, const QString &body )
{
    if ( !_socket->isOpen() ) {
        _lastError=tr( "Could not sendTo(): not connected to SMTP server '%1'." ).arg( _server );
        _resp->log( PHILOGERR, PHIRC_TCP_NETWORK_ERROR, _lastError );
        return 1;
    }
    if ( to.isEmpty() ) {
        _lastError=tr( "No email recipient specified." );
        _resp->log( PHILOGERR, PHIRC_ARGUMENT_ERROR, _lastError );
        return 1;
    }
    QString fromEnvelope=_from;
    if ( fromEnvelope.isEmpty() ) {
        _lastError=tr( "No sender (FROM) specified." );
        _resp->log( PHILOGERR, PHIRC_ARGUMENT_ERROR, _lastError );
        return 1;
    }
    qint16 res;
    QTextStream t( _socket );
    t.setCodec( "utf-8" );

    QString data="MAIL FROM: <"+fromEnvelope+'>';
    res=waitForDataWritten( data, t );
    if ( res ) return res;
    res=waitForResponseLine();
    qDebug( "EMAIL %d %s", res, qPrintable( _lastError ) );
    if ( res!=250 ) return res;
    data="RCPT TO: "+to;
    res=waitForDataWritten( data, t );
    if ( res ) return res;
    res=waitForResponseLine();
    qDebug( "EMAIL %d %s", res, qPrintable( _lastError ) );
    if ( res!=250 ) return res;
    res=waitForDataWritten( QString( "DATA" ), t );
    if ( res ) return res;
    res=waitForResponseLine();
    qDebug( "EMAIL %d %s", res, qPrintable( _lastError ) );
    if ( res!=354 ) return res;
    res=waitForMessageWritten( t, to, subject, body );
    if ( res ) return res;
    res=waitForResponseLine();
    qDebug( "EMAIL %d %s", res, qPrintable( _lastError ) );
    if ( res!=250 ) return res;
    return 0;
}

qint16 PHISEmailObj::waitForResponseLine( bool *moreLines )
{
    Q_ASSERT( _socket );
    while ( 1 ) {
        if ( !_socket->canReadLine() && !_socket->waitForReadyRead( _timeout ) ) {
            _lastError=tr( "Could not read from SMTP server '%1': <%2>." )
                .arg( _server ).arg( _socket->errorString() );
            _resp->log( PHILOGERR, PHI::socketError( _socket->error() ), _lastError );
            if ( moreLines ) *moreLines=false;
            return 1;
        }
        if ( _socket->canReadLine() ) {
            _lastError=QString::fromLatin1( _socket->readLine( 1024 ) );
            _lastError.chop( 2 );
            //qDebug( "readLine %s", qPrintable( _lastError ) );
            if ( moreLines ) {
                if ( _lastError.at( 3 )=='-' ) *moreLines=true;
                else *moreLines=false;
            }
            return _lastError.left( 3 ).toInt();
        }
    }
    return 1;
}

qint16 PHISEmailObj::waitForDataWritten( const QString &data, QTextStream &t )
{
    Q_ASSERT( _socket );
    t << data << "\r\n";
    t.flush();
    if ( !_socket->waitForBytesWritten( _timeout ) ) {
        _lastError=tr( "Could not write data to SMTP server '%1': <%2>." )
            .arg( _server ).arg( _socket->errorString() );
        _resp->log( PHILOGERR, PHI::socketError( _socket->error() ), _lastError );
        return 1;
    }
    return 0;
}

qint16 PHISEmailObj::waitForMessageWritten( QTextStream &t, const QString &to,
    const QString &subject, const QString &data )
{
    Q_ASSERT( _socket );
    QString toHeader=to;
    if ( toHeader.isEmpty() ) {
        if ( _to.isEmpty() && _recipients.count() ) toHeader=_recipients.first();
        else toHeader=_to;
    }
    QString fromHeader=_from;
    QString subjectHeader=subject;
    if ( subjectHeader.isEmpty() ) subjectHeader=_subject;
    QString body=data;
    if ( body.isEmpty() ) body=_data;

    QString d;
    d.reserve( body.size()+200 );
    d.append( "To: "+toHeader+"\r\nFrom: "+fromHeader+"\r\n" );
    if ( !subjectHeader.isEmpty() ) d.append( "Subject: "+subjectHeader+"\r\n" );
    d.append( "MIME-Version: 1.0\r\n" );
    d.append( "Content-type: "+_contentType+"\r\n" );
    d.append( "\r\n"+body );
    //d.replace( QChar::fromLatin1( '\n' ), QString::fromLatin1( "\r\n" ) );
    //d.replace( QString::fromLatin1( "\r\n.\r\n" ), QString::fromLatin1( "\r\n..\r\n" ) );
    //d.replace( QChar::fromLatin1( '\n' ), QString::fromLatin1( "\r\n" ) );
    d.replace( QString::fromLatin1( "\r\n.\r\n" ), QString::fromLatin1( "\r\n..\r\n" ) );
    t << d << "\r\n.\r\n";
    t.flush();
    if ( !_socket->waitForBytesWritten( _timeout ) ) {
        _lastError=tr( "Could not write data to email server '%1': <%2>." )
            .arg( _server ).arg( _socket->errorString() );
        _resp->log( PHILOGERR, PHI::socketError( _socket->error() ), _lastError );
        return 1;
    }
    return 0;
}

#undef PHISSCRIPTEXTENSION
