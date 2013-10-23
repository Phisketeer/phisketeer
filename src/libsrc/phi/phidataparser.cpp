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
#include <QLocale>
#include <QDir>
#include <QProcess>
#include <QSqlError>
#include <QCryptographicHash>
#include "phidataparser.h"
#include "phibasepage.h"
#include "phibaseitem.h"
#include "phirequest.h"
#include "phiresponserec.h"
#include "phinetrequest.h"

static QByteArray _matchingLanguage( const PHIRequest *req, const PHIData *data )
{
    if ( data->keys().contains( req->currentLangByteArray() ) ) return req->currentLangByteArray();
    QString lang;
    foreach ( lang, req->acceptedLanguages() )
        if ( data->keys().contains( lang.toLatin1() ) ) return lang.toLatin1();
    foreach ( lang, req->acceptedLanguages() )
        if ( data->keys().contains( lang.left( 2 ).toLatin1() ) ) return lang.left( 2 ).toLatin1();
    if ( data->keys().contains( req->defaultLang() ) ) return req->defaultLang();
    return PHIData::c();
}

QVariant PHIDataParser::text( PHIData *data ) const
{
    bool parse=data->options() & PHIData::Parse;
    switch ( data->source() ) {
    case PHIData::Static:
        if ( Q_LIKELY( !parse ) ) return data->variant();
        else return parseVariables( data->variant().toString() );
    case PHIData::Translated:
        if ( Q_LIKELY( !parse ) ) return data->variant( _matchingLanguage( _req, data ) );
        else return parseVariables( data->variant( _matchingLanguage( _req, data ) ).toString() );
    case PHIData::Database: {
        QString t=loadTextFromDB( data->sqlStatement(), data->templateText() );
        if ( Q_LIKELY( !parse ) ) return t;
        return parseVariables( t ); }
    case PHIData::File: {
        // @todo: implement caching
        QString t=loadTextFromFile( data->fileName(), data->textCodec() );
        if ( Q_UNLIKELY( parse) ) t=parseVariables( t );
        t.squeeze();
        return t; }
    case PHIData::Url: {
        QString t=loadTextFromUrl( data->url() );
        if ( Q_UNLIKELY( parse ) ) t=parseVariables( t );
        t.squeeze();
        if ( data->options() & PHIData::NoCache ) return t;
        cacheText( data, t );
        return t; }
    case PHIData::Process: {
        // @todo: make codec configurable
        QString t=QString::fromUtf8( loadFromProcess( data->processName(), data->attributes() ) );
        if ( Q_UNLIKELY( parse ) ) t=parseVariables( t );
        t.squeeze();
        if ( data->options() & PHIData::NoCache ) return t;
        cacheText( data, t );
        return t; }
    default:;
    }
    return QString();
}

void PHIDataParser::cacheText( PHIData *data, const QString &t, const QByteArray &lang ) const
{
    // @todo: implement translation using 'lang'
    switch ( data->type() ) {
    case PHIData::Text: {
        PHITextData d;
        d.setText( t, lang );
        *dynamic_cast<PHITextData*>(data)=d;
        return; }
    case PHIData::Integer: {
        PHIIntData d;
        d.setText( t, lang );
        *dynamic_cast<PHIIntData*>(data)=d;
        return; }
    case PHIData::Boolean: {
        PHIBooleanData d;
        d.setText( t, lang );
        *dynamic_cast<PHIBooleanData*>(data)=d;
        return; }
    default:;
    }
}

QString PHIDataParser::parseVariables( const QString &s ) const
{
    QString t=s, type, key, sub, replace;
    int pos=0, start, end;
    pos=t.indexOf( L1( "$PHI" ), pos );
    while ( pos!=-1 ) {
        start=t.indexOf( QLatin1Char( '[' ), pos );
        end=t.indexOf( QLatin1Char( ']' ), start );
        if ( start==-1 || end==-1 ) break;
        type=t.mid( pos+4, start-pos-4 );
        key=t.mid( start+1, end-start-1 );
        start=t.indexOf( QLatin1Char( '[' ), ++start );
        if ( start==end+1 ) {
            end=t.indexOf( QLatin1Char( ']' ), start );
            if ( end==-1 ) break;
            sub=t.mid( start+1, end-start-1 );
        } else sub=SL( "0" );
        replace=QString();
        if ( key==L1( "$PHIID" ) ) key=_currentItem ? _currentItem->name() : _pageId;
        if ( type==L1( "SERVER" ) ) replace=findValue( key, sub.toInt(), Server );
        else if ( type==L1( "SQL" ) ) replace=sqlValue( key.toInt() );
        else if ( type==L1( "POST" ) ) replace=findValue( key, sub.toInt(), Post );
        else if ( type==L1( "GET" ) ) replace=findValue( key, sub.toInt(), Get );
        else if ( type==L1( "REQUEST" ) ) replace=findValue( key, sub.toInt(), Request );
        else if ( type==L1( "HEADER" ) ) replace=findValue( key, sub.toInt(), Header );
        else if ( type==L1( "FILE" ) ) {
            if ( sub==L1( "size" ) ) replace=_req->fileSize( key );
            else if ( sub==L1( "filename" ) ) replace=_req->fileName( key );
            else if ( sub==L1( "tmpfile" ) ) replace=_req->tmpFile( key );
        } else if ( type==L1( "COOKIE" ) ) replace=_req->cookieValue( key );
        else if ( type==L1( "ALL" ) ) replace=findValue( key, sub.toInt(), All );
        else if ( type==L1( "PHIID") ) replace=_currentItem ? _currentItem->name() : _pageId;
        t.replace( pos, end-pos+1, replace );
        pos=t.indexOf( L1( "$PHI" ), ++pos );
    }
    return t;
}

QString PHIDataParser::findValue( const QString &key, int index, Type type ) const
{
    QStringList list;
    switch ( type ) {
    case Server:
        if ( key==L1( "today" ) ) {
            return QDate::currentDate().addDays( index ).toString( QString::fromLatin1( PHI::isoDateFormat() ) );
        } else if ( key==L1( "monthlist" ) ) {
            QLocale locale( _req->currentLang() );
            QString monthnames;
            for( int i=1; i<13; i++ ) {
                monthnames+=locale.monthName( i, QLocale::LongFormat )
                    +QLatin1Char( '[' )+QString::number(i)+L1( "]\n" );
            }
            return monthnames;
        } else if ( key==L1( "month" ) ) {
            QLocale locale( _req->currentLang() );
            if ( index>12 || index<0 ) index=0;
            if ( index==0 ) return locale.monthName( QDate::currentDate().month(), QLocale::LongFormat );
            return locale.monthName( index, QLocale::LongFormat );
        } else if ( key==L1( "year" ) ) {
            int year=QDate::currentDate().year();
            return QString::number( year+index );
        } else if ( key==L1( "yearlist" ) ) {
            if ( index>1000 || index<0 ) index=0;
            if ( index==0 ) return QDate::currentDate().toString( QStringLiteral( "yyyy" ) );
            int year=QDate::currentDate().year();
            QString years=QString::number( year );
            for ( int i=1; i<index; i++ ) {
                years+=QLatin1Char( '\n' )+QString::number( year+i );
            }
            return years;
        } else if ( key==L1( "version" ) ) return _req->version();
        else if ( key==L1( "userengine" ) ) return _req->userEngine();
        else if ( key==L1( "useragent" ) ) return _req->userAgent();
        else if ( key==L1( "useros" ) ) return _req->userOS();
        return _req->serverValue( key );
    case Header:
        return _req->header( key ); break;
    case Post:
        list=_req->postValues( key ); break;
    case Get:
        list=_req->getValues( key ); break;
    case Cookie:
        list=_req->cookieValues( key ); break;
    case Request:
        list=_req->requestValues( key ); break;
    default:
        list=_req->requestValues( key );
        if ( !_req->serverValue( key ).isEmpty() ) list << _req->serverValue( key );
        if ( !_req->header( key ).isEmpty() ) list << _req->header( key );
    }
    if ( list.count()>0 ) {
        if ( index>-1 && index<list.count() ) return list.at( index );
        return list.first();
    }
    return QString();
}

QString PHIDataParser::loadTextFromDB( const QString &statement, const QString &templateText ) const
{
    if ( !_query.exec( parseVariables( statement ) ) ) {
        _req->responseRec()->log( PHILOGERR, PHIRC_QUERY_ERROR,
            tr( "Could not execute query '%1' for page '%2': %3" )
            .arg( parseVariables( statement ) )
            .arg( _pageId  ).arg( _query.lastError().text() ) );
        return templateText;
    }
    QString tmp;
    if ( _query.size() < 1 ) { // fallback and try C locale
        tmp=statement;
        tmp.replace( SL( "$PHISERVER[lang]" ), SL( "C" ) );
        _query.exec( parseVariables( tmp ) );
        tmp.clear();
    }
    while ( _query.next() ) tmp.append( parseVariables( templateText ) );
    return tmp;
}

QImage PHIDataParser::loadImageFromDB( const QString &statement ) const
{
    if ( Q_UNLIKELY( !_query.exec( parseVariables( statement ) ) ) ) {
        _req->responseRec()->log( PHILOGERR, PHIRC_QUERY_ERROR,
            tr( "Could not execute query '%1' for page '%2': %3" )
            .arg( parseVariables( statement ) )
            .arg( _pageId  ).arg( _query.lastError().text() ) );
        return QImage();
    }
    if ( Q_UNLIKELY( !_query.next() ) ) {
        _req->responseRec()->log( PHILOGERR, PHIRC_QUERY_ERROR,
            tr( "Could not execute query '%1' for page '%2': %3" )
            .arg( parseVariables( statement ) )
            .arg( _pageId  ).arg( _query.lastError().text() ) );
        return QImage();
    }
    return QImage::fromData( _query.value( 0 ).toByteArray() );
}

QString PHIDataParser::loadTextFromFile( const QString &fileName, const QString &codec ) const
{
    QString fn=QDir::fromNativeSeparators( fileName );
    if ( fn.startsWith( QLatin1Char( '/' ) ) ) {
        fn=_req->documentRoot()+fn;
    } else {
        fn=QFileInfo( _req->canonicalFilename() ).absolutePath()+QDir::separator()+fn;
    }
    fn=replaceDefaultLangC( fn );
    QFile f( fn );
    if ( !f.open( QIODevice::ReadOnly ) ) {
        _req->responseRec()->log( PHILOGERR, PHIRC_IO_FILE_ACCESS_ERROR,
            tr( "Could not open file '%1' for reading in page '%2': %3" )
            .arg( fn ).arg( _pageId ).arg( f.errorString() ) );
        return QString();
    }
    QTextStream t( &f );
    if ( !codec.isEmpty() ) t.setCodec( codec.toUtf8().constData() );
    return t.readAll();
}

QString PHIDataParser::loadTextFromUrl( const QString &url ) const
{
    PHINetRequestThread request( _req->responseRec() );
    return request.getText( parseVariables( url ) );
}

QImage PHIDataParser::loadImageFromUrl( const QString &url ) const
{
    PHINetRequestThread request( _req->responseRec() );
    return request.getImage( parseVariables( url ) );
}

QByteArray PHIDataParser::loadFromProcess( const QString &procName, const QString &a ) const
{
    //QString name=replaceDefaultLangC( procName, true );
    QString name=parseVariables( procName );
    QProcess proc;
    QString arg=parseVariables( a );
    QStringList args=arg.split( QRegExp( SL( "\\s" ) ), QString::SkipEmptyParts );
    proc.start( name, args, QIODevice::ReadOnly );
    if ( !proc.waitForStarted() ) {
        _req->responseRec()->log( PHILOGERR, PHIRC_OBJ_ACCESS_ERROR,
            tr( "Could not start process '%1' with arguments '%2' for page '%3': %4" )
            .arg( name ).arg( args.join( SL( " " ) ) ).arg( _pageId  ).arg( proc.errorString() ) );
        return QByteArray();
    }
    if ( !proc.waitForFinished() ) {
        _req->responseRec()->log( PHILOGERR, PHIRC_OBJ_ACCESS_ERROR,
            tr( "Could not finish process '%1' with arguments '%2' for page '%3': %4" )
            .arg( name ).arg( args.join( SL( " " ) ) ).arg( _pageId ).arg( proc.errorString() ) );
        return QByteArray();
    }
    if ( proc.exitCode() ) {
        _req->responseRec()->log( PHILOGWARN, PHIRC_OBJ_ACCESS_ERROR,
            tr( "Process '%1' exit code is '%2' for page '%3': %4")
            .arg( name ).arg( proc.exitCode() ).arg( _pageId )
            .arg( QString::fromUtf8( proc.readAllStandardError() ) ) );
    }
    return proc.readAllStandardOutput();
}

QString PHIDataParser::replaceDefaultLangC( const QString &fileName ) const
{
    QString fn=QDir::fromNativeSeparators( fileName );
    static const QString cmatch=L1( "/C/" );
    if ( fileName.contains( cmatch ) ) {
        QString testfn( fn );
        testfn.replace( cmatch, QLatin1Char( '/' )+_req->currentLang()+QLatin1Char( '/' ) );
        if ( QFile::exists( testfn ) ) return testfn;
        QString lang;
        foreach ( lang, _req->acceptedLanguages() ) {
            testfn=fn;
            testfn.replace( cmatch, QLatin1Char( '/' )+lang+QLatin1Char( '/' ) );
            if ( QFile::exists( testfn ) ) return testfn;
        }
        foreach ( lang, _req->acceptedLanguages() ) {
            testfn=fn;
            testfn.replace( cmatch, QLatin1Char( '/' )+lang.left( 2 )+QLatin1Char( '/' ) );
            if ( QFile::exists( testfn ) ) return testfn;
        }
        testfn=fn;
        testfn.replace( cmatch, QLatin1Char( '/' )+QString::fromLatin1( _req->defaultLang() )
            +QLatin1Char( '/' ) );
        if ( QFile::exists( testfn ) ) return testfn;
    }
    return fn;
}

void PHIDataParser::createTmpImages( PHIImageData *data ) const
{
    Q_ASSERT( data );
    if ( data->options() & PHIData::NoCache ) return;
    const QByteArray name( _currentItem ? _currentItem->id() : _pageId.toUtf8() );
    QByteArray id=createImageId( name );
    switch ( data->source() ) {
    case PHIData::Static:
        saveImage( data->image(), id );
        break;
    case PHIData::Translated:
    {
        PHIImageData trans;
        trans.setSource( PHIData::Translated );
        foreach ( QByteArray lang, data->data().keys() ) {
            if ( lang.startsWith( '#' ) ) continue; // meta data
            id=createImageId( name, lang );
            saveImage( data->image( lang ), id );
            trans.setImageId( id, lang );
        }
        *data=trans;
        return;
    }
    case PHIData::Database:
        if ( data->options() & PHIData::FileName ) {
            saveImage( loadTextFromDB( data->sqlStatement(), data->templateText() ), id );
        } else {
            saveImage( loadImageFromDB( data->sqlStatement() ), id );
        }
        break;
    case PHIData::File:
        // @todo: implement caching
        return;
    case PHIData::Url:
        if ( data->options() & PHIData::FileName ) {
            saveImage( loadTextFromUrl( data->url() ), id );
        } else {
            saveImage( loadImageFromUrl( data->url() ), id );
        }
        break;
    case PHIData::Process:
        if ( data->options() & PHIData::FileName ) {
            saveImage( QString::fromUtf8( loadFromProcess( data->processName(), data->attributes() ) ), id );
        } else {
            QImage img=QImage::fromData( loadFromProcess( data->processName(), data->attributes() ) );
            saveImage( img, id );
        }
        break;
    default:;
    }
    PHIImageData res;
    res.setImageId( id );
    *data=res; // implicits freeing image memory
}

QByteArray PHIDataParser::createImageId( const QByteArray &name, const QByteArray &lang, int num ) const
{
    QByteArray arr=name+QByteArray::number( num )+lang+_req->canonicalFilename().toUtf8();
    arr=QCryptographicHash::hash( arr, QCryptographicHash::Md5 );
    arr.squeeze();
    return arr;
}

void PHIDataParser::saveImage( const QImage &img, const QByteArray &id ) const
{
    if ( img.isNull() ) return;
    img.convertToFormat( QImage::Format_ARGB32_Premultiplied )
        .save( _req->imgDir()+QDir::separator()+QString::fromLatin1( id ), "PNG" );
}

void PHIDataParser::saveImage( const QString &fn, const QByteArray &id ) const
{
}

/*
QImage PHIDataParser::parseImage( PHIImageData *data )
{
    return QImage();
}

PHIImageHash PHIDataParser::parseImageBook( PHIImageBookData *data )
{
    return PHIImageHash();
}
*/
