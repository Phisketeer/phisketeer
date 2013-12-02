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
#include <QProcess>
#include <QSqlError>
#include <QTransform>
#include <QPainter>
#include "phidataparser.h"
#include "phibasepage.h"
#include "phinetrequest.h"

QHash <QByteArray, QRectF> PHIDataParser::_imageTransformedRects;
QHash <QByteArray, QRectF> PHIDataParser::_imageGraphicsRects;
QReadWriteLock PHIDataParser::_lock;

static QByteArray _matchingLanguage( const PHIRequest *req, const PHIData *data )
{
    if ( data->keys().contains( req->currentLangByteArray() ) ) return req->currentLangByteArray();
    QByteArray lang;
    foreach ( lang, req->acceptedLanguages() )
        if ( data->keys().contains( lang ) ) return lang;
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
        if ( _currentItem ) _currentItem->setDirtyFlag( PHIBaseItem::DFDoNotCache );
        if ( Q_LIKELY( !parse ) ) return t;
        return parseVariables( t );
    }
    case PHIData::File: {
        // @todo: implement caching
        QByteArray matchedLang;
        QString t=loadTextFromFile( data->fileName(), data->textCodec(), matchedLang );
        if ( _currentItem ) _currentItem->setDirtyFlag( PHIBaseItem::DFDoNotCache );
        if ( Q_UNLIKELY( parse ) ) t=parseVariables( t );
        t.squeeze();
        return t;
    }
    case PHIData::Url: {
        QString t=loadTextFromUrl( data->url() );
        if ( Q_UNLIKELY( parse ) ) t=parseVariables( t );
        t.squeeze();
        if ( data->options() & PHIData::NoCache ) {
            if ( _currentItem ) _currentItem->setDirtyFlag( PHIBaseItem::DFDoNotCache );
            return t;
        }
        cacheText( data, t );
        return t;
    }
    case PHIData::Process: {
        // @todo: make codec configurable
        QString t=QString::fromUtf8( loadFromProcess( data->processName(), data->attributes() ) );
        if ( Q_UNLIKELY( parse ) ) t=parseVariables( t );
        t.squeeze();
        if ( data->options() & PHIData::NoCache ) {
            if ( _currentItem ) _currentItem->setDirtyFlag( PHIBaseItem::DFDoNotCache );
            return t;
        }
        cacheText( data, t );
        return t;
    }
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
        return;
    }
    case PHIData::Integer: {
        PHIIntData d;
        d.setText( t, lang );
        *dynamic_cast<PHIIntData*>(data)=d;
        return;
    }
    case PHIData::Boolean: {
        PHIBooleanData d;
        d.setText( t, lang );
        *dynamic_cast<PHIBooleanData*>(data)=d;
        return;
    }
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
            tr( "Could not execute query '%1' for page '%2' and id '%3': %4" )
            .arg( parseVariables( statement ) ).arg( _req->canonicalFilename()  )
            .arg( _currentItem ? _currentItem->name() : _pageId )
            .arg( _query.lastError().text() ) );
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
            tr( "Could not execute query '%1' for page '%2' and id '%3': %4" )
            .arg( parseVariables( statement ) ).arg( _req->canonicalFilename()  )
            .arg( _currentItem ? _currentItem->name() : _pageId )
            .arg( _query.lastError().text() ) );
        return QImage();
    }
    if ( Q_UNLIKELY( !_query.next() ) ) {
        _req->responseRec()->log( PHILOGERR, PHIRC_QUERY_ERROR,
            tr( "Could not execute query '%1' for page '%2': %3" )
            .arg( parseVariables( statement ) )
            .arg( _req->canonicalFilename() ).arg( _query.lastError().text() ) );
        return QImage();
    }
    return QImage::fromData( _query.value( 0 ).toByteArray() );
}

QList <QImage> PHIDataParser::loadImagesFromDB( const QString &statement ) const
{
    QList <QImage> list;
    if ( Q_UNLIKELY( !_query.exec( parseVariables( statement ) ) ) ) {
        _req->responseRec()->log( PHILOGERR, PHIRC_QUERY_ERROR,
            tr( "Could not execute query '%1' for page '%2' and id '%3': %4" )
            .arg( parseVariables( statement ) ).arg( _req->canonicalFilename()  )
            .arg( _currentItem ? _currentItem->name() : _pageId )
            .arg( _query.lastError().text() ) );
        return list;
    }
    while( _query.next() ) {
        list.append( QImage::fromData( _query.value( 0 ).toByteArray() ) );
    }
    return list;
}

QImage PHIDataParser::loadImageFromFile( const QString &fileName, QByteArray &matchedLang ) const
{
    QImage img( replaceDefaultLangC( fileName, matchedLang ) ); // resolves path
    if ( Q_UNLIKELY( img.isNull() ) ) {
        _req->responseRec()->log( PHILOGERR, PHIRC_IO_FILE_ACCESS_ERROR,
            tr( "Could not open image file '%1' for reading in page '%2' and id '%3'." )
            .arg( fileName ).arg( _pageId ).arg( _currentItem ? _currentItem->name() :_pageId ) );
        return QImage();
    }
    return img;
}

QString PHIDataParser::loadTextFromFile( const QString &fileName, const QString &codec, QByteArray &matchedLang ) const
{
    QFile f( replaceDefaultLangC( fileName, matchedLang ) );
    if ( Q_UNLIKELY( !f.open( QIODevice::ReadOnly ) ) ) {
        _req->responseRec()->log( PHILOGERR, PHIRC_IO_FILE_ACCESS_ERROR,
            tr( "Could not open file '%1' for reading in page '%2': %3" )
            .arg( fileName ).arg( _pageId ).arg( f.errorString() ) );
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
    QString name=parseVariables( procName );
    QProcess proc;
    QString arg=parseVariables( a );
    QStringList args=arg.split( QRegExp( SL( "\\s" ) ), QString::SkipEmptyParts );
    proc.start( name, args, QIODevice::ReadOnly );
    if ( !proc.waitForStarted() ) {
        _req->responseRec()->log( PHILOGERR, PHIRC_OBJ_ACCESS_ERROR,
            tr( "Could not finish process '%1' with arguments '%2' for page '%3'"
              "and id '%4': %5" ).arg( name ).arg( args.join( SL( " " ) ) )
            .arg( _req->canonicalFilename() )
            .arg( _currentItem ? _currentItem->name() : _pageId )
            .arg( proc.errorString() ) );
        return QByteArray();
    }
    if ( !proc.waitForFinished() ) {
        _req->responseRec()->log( PHILOGERR, PHIRC_OBJ_ACCESS_ERROR,
            tr( "Could not finish process '%1' with arguments '%2' for page '%3'"
                "and id '%4': %5" ).arg( name ).arg( args.join( SL( " " ) ) )
            .arg( _req->canonicalFilename() )
            .arg( _currentItem ? _currentItem->name() : _pageId )
            .arg( proc.errorString() ) );
        return QByteArray();
    }
    if ( proc.exitCode() ) {
        _req->responseRec()->log( PHILOGWARN, PHIRC_OBJ_ACCESS_ERROR,
            tr( "Process '%1' exit code is '%2' for page '%3' and id '%4': %5" )
            .arg( name ).arg( proc.exitCode() ).arg( _req->canonicalFilename() )
            .arg( _currentItem ? _currentItem->name() : _pageId )
            .arg( QString::fromUtf8( proc.readAllStandardError() ) ) );
    }
    return proc.readAllStandardOutput();
}

QString PHIDataParser::replaceDefaultLangC( const QString &fileName, QByteArray &matchedLang ) const
{
    QString fn=resolvePath( fileName );
    matchedLang=matchedLangForResolvedFilename( fn );
    if ( matchedLang==PHIData::c() ) return fn;
    fn.replace( L1( "/C/" ), QString::fromLatin1( matchedLang ) );
    return fn;
}

QByteArray PHIDataParser::matchedLangForResolvedFilename( const QString &filename ) const
{
    static const QString cmatch=L1( "/C/" );
    if ( !filename.contains( cmatch ) ) return PHIData::c();
    QString testfn( filename );
    testfn.replace( cmatch, QLatin1Char( '/' )+_req->currentLang()+QLatin1Char( '/' ) );
    if ( QFile::exists( testfn ) ) return _req->currentLangByteArray();
    QByteArray lang;
    foreach ( lang, _req->acceptedLanguages() ) {
        testfn=filename;
        testfn.replace( cmatch, QLatin1Char( '/' )+QString::fromLatin1( lang )+QLatin1Char( '/' ) );
        if ( QFile::exists( testfn ) ) return lang;
    }
    testfn=filename;
    testfn.replace( cmatch, QLatin1Char( '/' )+QString::fromLatin1( _req->defaultLang() )+QLatin1Char( '/' ) );
    if ( QFile::exists( testfn ) ) return _req->defaultLang();
    return PHIData::c();
}

void PHIDataParser::cacheImageFile( PHIImageData *data, const QString &filename ) const
{
    PHIImageData res;
    res.setOption( PHIData::UseFilePath );
    res.setSource( PHIData::File );
    res.setFileName( filename );
    *data=res;
}

void PHIDataParser::cacheImageFiles( PHIImageBookData *data, const QStringList &filenames ) const
{
    PHIImageBookData res;
    res.setOption( PHIData::UseFilePath );
    res.setSource( PHIData::File );
    res.setFileNames( filenames );
    *data=res;
}

void PHIDataParser::createImages( PHIImageData *data ) const
{
    Q_ASSERT( data );
    if ( data->options() & PHIData::NoCache ) return;
    const QByteArray name( _currentItem ? _currentItem->id() : _pageId.toUtf8() );
    QByteArray id=createImageId( _req, name );
    switch ( data->source() ) {
    case PHIData::Static:
        saveImage( data->image(), id );
        break;
    case PHIData::Translated:
    {
        PHIImageData trans;
        trans.setSource( PHIData::Translated );
        foreach ( QByteArray lang, data->keys() ) {
            if ( lang.startsWith( '#' ) ) continue; // meta data
            id=createImageId( _req, name, lang );
            saveImage( data->image( lang ), id );
            trans.setImageId( id, lang );
        }
        *data=trans;
        return;
    }
    case PHIData::Database:
        if ( Q_UNLIKELY( data->options() & PHIData::FileName ) ) {
            return cacheImageFile( data, loadTextFromDB( data->sqlStatement(), data->templateText() ) );
        } else {
            saveImage( loadImageFromDB( data->sqlStatement() ), id );
        }
        break;
    case PHIData::File:
        qDebug() << "file:" << data->fileName();
        return; // resolve at processing time
    case PHIData::Url:
        if ( Q_UNLIKELY( data->options() & PHIData::FileName ) ) {
            return cacheImageFile( data, loadTextFromUrl( data->url() ) );
        } else {
            saveImage( loadImageFromUrl( data->url() ), id );
        }
        break;
    case PHIData::Process:
        if ( data->options() & PHIData::FileName ) {
            return cacheImageFile( data, QString::fromUtf8( loadFromProcess( data->processName(), data->attributes() ) ) );
        } else {
            QImage img=QImage::fromData( loadFromProcess( data->processName(), data->attributes() ) );
            saveImage( img, id );
        }
        break;
    default:;
    }
    PHIImageData res; // source = Static
    res.setImageId( id ); // option |= TmpObjCreated
    *data=res; // implicitly frees image memory if any
}

void PHIDataParser::createImages( PHIImageBookData *data ) const
{
    Q_ASSERT( data );
    if ( data->options() & PHIData::NoCache ) return;
    const QByteArray name( _currentItem ? _currentItem->id() : _pageId.toUtf8() );
    QByteArray id;
    PHIByteArrayList ids;
    switch ( data->source() ) {
    case PHIData::Static:
    {
        PHIImageBookData res; // source = Static
        PHIImageHash images=data->imageBook();
        for ( int i=0; i<images.count(); i++ ) {
            id=createImageId( _req, name, PHIData::c(), i );
            saveImage( images.value( QByteArray::number( i ) ), id );
            ids.append( id );
        }
        res.setImageIds( ids ); // options |= TmpObjCreated
        *data=res; // implicitly frees image memory if any
        return;
    }
    case PHIData::Translated:
    {
        PHIImageBookData trans;
        trans.setSource( PHIData::Translated );
        foreach ( QByteArray lang, data->keys() ) {
            if ( lang.startsWith( '#' ) ) continue; // meta data
            PHIImageHash images=data->imageBook( lang );
            for ( int i=0; i<images.count(); i++ ) {
                id=createImageId( _req, name, lang, i );
                saveImage( images.value( QByteArray::number( i ) ), id );
                ids.append( id ); // options |= TmpObjCreated
            }
            trans.setImageIds( ids, lang );
        }
        *data=trans;
        return;
    }
    case PHIData::Database:
        if ( data->options() & PHIData::FileName ) {
            QStringList list=loadTextFromDB( data->sqlStatement(), data->templateText() )
                .split( QRegExp( SL( "\\s" ) ), QString::SkipEmptyParts );
            return cacheImageFiles( data, list );
        } else {
            PHIImageBookData res; // source = Static
            QList <QImage> list=loadImagesFromDB( data->sqlStatement() );
            for ( int i=0; i<list.count(); i++ ) {
                id=createImageId( _req, name, PHIData::c(), i );
                saveImage( list.at( i ), id );
                ids.append( id );
            }
            res.setImageIds( ids ); // options |= TmpObjCreated
            *data=res;
        }
        return;
    case PHIData::File:
        return; // resolve at processing time
    case PHIData::Url:
    {
        Q_ASSERT( data->options() & PHIData::FileName );
        QStringList list=loadTextFromUrl( data->url() )
            .split( QRegExp( SL( "\\s" ) ), QString::SkipEmptyParts );
        return cacheImageFiles( data, list );
    }
    case PHIData::Process:
    {
        Q_ASSERT( data->options() & PHIData::FileName );
        QStringList list=QString::fromUtf8( loadFromProcess( data->processName(), data->attributes() ) )
            .split( QRegExp( SL( "\\s" ) ), QString::SkipEmptyParts );
        return cacheImageFiles( data, list );
    }
    default:;
    }
    Q_ASSERT( 0 );
}

QByteArray PHIDataParser::imagePath( PHIImageData *data ) const
{
    Q_ASSERT( data );
    if ( data->options() & PHIData::TmpObjCreated ) {
        qDebug() << "from cache" << (_currentItem ? _currentItem->name() : _pageId);
        if ( data->source()==PHIData::Static ) return data->imageId();
        Q_ASSERT( data->source()==PHIData::Translated );
        return data->imageId( _matchingLanguage( _req, data ) );
    }
    QByteArray id;
    switch ( data->source() ) {
    case PHIData::Static:
        Q_ASSERT( data->options() & PHIData::NoCache );
        id=PHIImageCache::instance()->createUid( _req );
        saveImage( data->image(), id );
        if ( _currentItem ) _currentItem->setDirtyFlag( PHIBaseItem::DFDoNotCache );
        return id;
    case PHIData::Translated:
        Q_ASSERT( data->options() & PHIData::NoCache );
        id=PHIImageCache::instance()->createUid( _req );
        saveImage( data->image( _matchingLanguage( _req, data ) ), id );
        if ( _currentItem ) _currentItem->setDirtyFlag( PHIBaseItem::DFDoNotCache );
        return id;
    case PHIData::Database:
        Q_ASSERT( data->options() & PHIData::NoCache );
        id=PHIImageCache::instance()->createUid( _req );
        if ( data->options() & PHIData::FileName ) {
            QByteArray matchedLang;
            saveImage( loadImageFromFile( loadTextFromDB( data->sqlStatement(), data->templateText() ), matchedLang ), id );
        } else {
            saveImage( loadImageFromDB( data->sqlStatement() ), id );
        }
        if ( _currentItem ) _currentItem->setDirtyFlag( PHIBaseItem::DFDoNotCache );
        return id;
    case PHIData::File:
        Q_ASSERT( data->options() & PHIData::FileName );
        if ( Q_UNLIKELY( data->options() & PHIData::NoCache ) ) {
            QByteArray matchedLang;
            id=PHIImageCache::instance()->createUid( _req );
            saveImage( loadImageFromFile( data->fileName(), matchedLang ), id );
            if ( _currentItem ) _currentItem->setDirtyFlag( PHIBaseItem::DFDoNotCache );
            return id;
        } else {
            if ( _currentItem ) {
                _currentItem->setDirtyFlag( PHIBaseItem::DFHasFilePath );
                _currentItem->setDirtyFlag( PHIBaseItem::DFUseFilePathInHTML );
            }
            QByteArray matchedLang;
            return resolveImageFile( data->fileName(), matchedLang );
        }
    case PHIData::Url:
        Q_ASSERT( data->options() & PHIData::NoCache );
        id=PHIImageCache::instance()->createUid( _req );
        if ( data->options() & PHIData::FileName ) {
            QByteArray matchedLang;
            saveImage( loadImageFromFile( loadTextFromUrl( data->url() ), matchedLang ), id );
        } else {
            saveImage( loadImageFromUrl( data->url() ), id );
        }
        if ( _currentItem ) _currentItem->setDirtyFlag( PHIBaseItem::DFDoNotCache );
        return id;
    case PHIData::Process:
        Q_ASSERT( data->options() & PHIData::NoCache );
        id=PHIImageCache::instance()->createUid( _req );
        if ( data->options() & PHIData::FileName ) {
            QByteArray matchedLang;
            saveImage( loadImageFromFile( QString::fromUtf8( loadFromProcess( data->processName(), data->attributes() ) ), matchedLang ), id );
        } else {
            saveImage( QImage::fromData( loadFromProcess( data->processName(), data->attributes() ) ), id );
        }
        if ( _currentItem ) _currentItem->setDirtyFlag( PHIBaseItem::DFDoNotCache );
        return id;
    default:;
    }
    Q_ASSERT( 0 );
    return id;
}

PHIByteArrayList PHIDataParser::imagePathes( PHIImageBookData *data ) const
{
    Q_ASSERT( data );
    if ( data->options() & PHIData::TmpObjCreated ) {
        qDebug() << "from cache" << (_currentItem ? _currentItem->name() : _pageId);
        if ( data->source()==PHIData::Static ) return data->imageIds();
        Q_ASSERT( data->source()==PHIData::Translated );
        return data->imageIds( _matchingLanguage( _req, data ) );
    }
    QByteArray id, matchedLang;
    PHIByteArrayList ids;
    PHIImageHash images;
    switch ( data->source() ) {
    case PHIData::Static:
        Q_ASSERT( data->options() & PHIData::NoCache );
        images=data->imageBook();
        for ( int i=0; i<images.count(); i++ ) {
            id=PHIImageCache::instance()->createUid( _req );
            saveImage( images.value( QByteArray::number( i ) ), id );
            ids.append( id );
        }
        if ( _currentItem ) _currentItem->setDirtyFlag( PHIBaseItem::DFDoNotCache );
        return ids;
    case PHIData::Translated:
        Q_ASSERT( data->options() & PHIData::NoCache );
        images=data->imageBook( _matchingLanguage( _req, data ) );
        for ( int i=0; i<images.count(); i++ ) {
            id=PHIImageCache::instance()->createUid( _req );
            saveImage( images.value( QByteArray::number( i ) ), id );
            ids.append( id );
        }
        if ( _currentItem ) _currentItem->setDirtyFlag( PHIBaseItem::DFDoNotCache );
        return ids;
    case PHIData::Database:
        Q_ASSERT( data->options() & PHIData::NoCache );
        if ( _currentItem ) _currentItem->setDirtyFlag( PHIBaseItem::DFDoNotCache );
        if ( data->options() & PHIData::FileName ) {
            QStringList list=loadTextFromDB( data->sqlStatement(), data->templateText() )
                .split( QRegExp( SL( "\\s" ) ), QString::SkipEmptyParts );
            QString path;
            foreach ( path, list ) {
                id=PHIImageCache::instance()->createUid( _req );
                saveImage( loadImageFromFile( path, matchedLang ), id );
                ids.append( id );
            }
            return ids;
        } else {
            QList <QImage> list=loadImagesFromDB( data->sqlStatement() );
            foreach ( QImage img, list ) {
                id=PHIImageCache::instance()->createUid( _req );
                saveImage( img, id );
                ids.append( id );
            }
        }
        return ids;
    case PHIData::File:
        Q_ASSERT( data->options() & PHIData::FileName );
        if ( Q_UNLIKELY( data->options() & PHIData::NoCache ) ) {
            QString fn;
            foreach( fn, data->fileNames() ) {
                id=PHIImageCache::instance()->createUid( _req );
                saveImage( loadImageFromFile( fn, matchedLang ), id );
                ids.append( id );
            }
            if ( _currentItem ) _currentItem->setDirtyFlag( PHIBaseItem::DFDoNotCache );
            return ids;
        } else {
            if ( _currentItem ) {
                _currentItem->setDirtyFlag( PHIBaseItem::DFHasFilePath );
                _currentItem->setDirtyFlag( PHIBaseItem::DFUseFilePathInHTML );
            }
            QString fn;
            foreach( fn, data->fileNames() ) {
                id=resolveImageFile( fn, matchedLang );
                ids.append( id );
            }
        }
        return ids;
    case PHIData::Url:
    {
        Q_ASSERT( data->options() & PHIData::NoCache );
        Q_ASSERT( data->options() & PHIData::FileName );
        QStringList list=loadTextFromUrl( data->url() )
            .split( QRegExp( SL( "\\s" ) ), QString::SkipEmptyParts );
        QString path;
        foreach ( path, list ) {
            id=PHIImageCache::instance()->createUid( _req );
            saveImage( loadImageFromFile( path, matchedLang ), id );
            ids.append( id );
        }
        if ( _currentItem ) _currentItem->setDirtyFlag( PHIBaseItem::DFDoNotCache );
        return ids;
    }
    case PHIData::Process:
    {
        Q_ASSERT( data->options() & PHIData::NoCache );
        Q_ASSERT( data->options() & PHIData::FileName );
        QStringList list=QString::fromUtf8( loadFromProcess( data->processName(), data->attributes() ) )
            .split( QRegExp( SL( "\\s" ) ), QString::SkipEmptyParts );
        QString path;
        foreach ( path, list ) {
            id=PHIImageCache::instance()->createUid( _req );
            saveImage( loadImageFromFile( path, matchedLang ), id );
            ids.append( id );
        }
        if ( _currentItem ) _currentItem->setDirtyFlag( PHIBaseItem::DFDoNotCache );
        return ids;
    }
    default:;
    }
    Q_ASSERT( 0 );
    return ids;
}

// static
QByteArray PHIDataParser::createTransformedImage( const PHIRequest *req, const PHIBaseItem *it, int num, QRectF &br )
{
    qDebug() << "createTransformedImage" << it->name();
    Q_ASSERT( it );
    QTransform t=it->computeTransformation( false );
    it->setDirtyFlag( PHIBaseItem::DFUseFilePathInHTML, false );
    QByteArray arr;
    if ( Q_UNLIKELY( it->dirtyFlags() & PHIBaseItem::DFDoNotCache ) ) {
        arr=PHIImageCache::instance()->createUid( req ); // don't cache
    } else {
        arr=it->id()+QByteArray::number( num )+req->currentLangByteArray()+QByteArray::number( t.determinant() )
        +QByteArray::number( it->hasGraphicEffect() ? 1 : 0 )+req->canonicalFilename().toUtf8();
        arr=QCryptographicHash::hash( arr, QCryptographicHash::Md5 ).toHex();
        QFileInfo fi( req->imgDir()+QLatin1Char( '/')+QString::fromUtf8( arr )+SL( ".png" ) );
        if ( fi.exists() && fi.lastModified()>=req->lastModified() ) {
            QRectF r=br;
            qDebug() << "using cache" << br;
            if ( it->hasTransformation() ) {
                if ( Q_LIKELY( req->agentFeatures() & PHIRequest::Transform3D ) ) {
                    br=graphicsImageRect( arr );
                    if ( !br.isNull() ) return arr;
                } else if ( t.isAffine() && req->agentFeatures() & PHIRequest::Transform2D ) {
                    br=graphicsImageRect( arr );
                    if ( !br.isNull() ) return arr;
                } else {
                    br=transformedImageRect( arr );
                    if ( !br.isNull() ) return arr;
                }
            }
            br=graphicsImageRect( arr );
            qDebug() << "cached" << br;
            if ( !br.isNull() ) return arr;
            br=r;
        }
    }
    QString fn;
    if ( it->hasImages() ) {
        PHIByteArrayList imgIds=it->imagePathes();
        if ( Q_UNLIKELY( num>=imgIds.count() ) ) {
            req->responseRec()->log( PHILOGERR, PHIRC_OBJ_ACCESS_ERROR,
                tr( "Could not resolve image path for item '%1'." ).arg( it->name() ) );
            br=it->adjustedRect();
            return QByteArray();
        }
        fn=QString::fromUtf8( imgIds.at( num ) );
    } else fn=QString::fromUtf8( it->imagePath() );
    if ( it->dirtyFlags() & PHIBaseItem::DFHasFilePath ) fn=req->documentRoot()+fn;
    else fn=req->imgDir()+QLatin1Char( '/' )+fn+SL( ".png" );
    QImage img( fn );
    if ( Q_UNLIKELY( img.isNull() ) ) {
        req->responseRec()->log( PHILOGERR, PHIRC_IO_FILE_ACCESS_ERROR,
            tr( "Could not load image '%1' for item '%2'." ).arg( fn ).arg( it->name() ) );
        br=it->adjustedRect();
        return QByteArray();
    }
    if ( Q_UNLIKELY( img.size()!=it->realSize().toSize() ) ) {
        qDebug() << "resizing image for" << it->id();
        img=img.scaled( it->realSize().toSize(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
    }
    if ( it->hasGraphicEffect() ) {
        switch ( it->effect()->graphicsType() ) {
        case PHIEffect::GTShadow: {
            QColor c;
            qreal offx, offy, radius;
            it->effect()->shadow( c, offx, offy, radius );
            img=PHI::dropShadowedImage( img, c, radius, offx, offy, br );
            break;
        }
        case PHIEffect::GTColorize: {
            QColor c;
            qreal strength;
            it->effect()->colorize( c, strength );
            qDebug() << "colorize" << br;
            img=PHI::colorizedImage( img, c, strength, br );
            break;
        }
        case PHIEffect::GTBlur: {
            qreal radius;
            it->effect()->blur( radius );
            img=PHI::bluredImage( img, radius, br );
            break;
        }
        case PHIEffect::GTReflection: {
            qreal offy, size;
            it->effect()->reflection( offy, size );
            img=PHI::reflectedImage( img, offy, size, br );
            break;
        }
        default:;
        }
    }
    if ( !t.isIdentity() ) {
        if ( Q_UNLIKELY( (!t.isAffine() && !(req->agentFeatures() & PHIRequest::Transform3D))
                || (t.isAffine() && !(req->agentFeatures() & PHIRequest::Transform2D)) ) ) {
            t=it->computeTransformation( true ); // with translation
            br=t.mapRect( br );
            arr.append( 't' );
            insertTransformedImageRect( arr, br );
            img.transformed( t, Qt::SmoothTransformation )
                .save( req->imgDir()+QLatin1Char( '/' )+QString::fromLatin1( arr )+SL( ".png" ), "PNG" );
            return arr;
        } else {
            QSizeF s=br.size();
            br=QRectF( t.map( br.topLeft() ), s );
        }
    }
    insertGraphicsImageRect( arr, br );
    qDebug() << "saving transformed image" << it->id() << arr << br << it->adjustedRect();
    img.save( req->imgDir()+QLatin1Char( '/' )+QString::fromLatin1( arr )+SL( ".png" ), "PNG" );
    return arr;
}

QString PHIDataParser::resolvePath( const QString &filename ) const
{
    QString fn=QDir::fromNativeSeparators( filename );
    if ( fn.startsWith( QLatin1Char( '/' ) ) ) {
        fn=_req->documentRoot()+fn;
    } else {
        fn=QFileInfo( _req->canonicalFilename() ).absolutePath()+QLatin1Char( '/' )+fn;
    }
    if ( !_currentItem ) fn.prepend( QLatin1Char( '@' ) ); // marker for page image (DFUseFilePath does not exist)
    qDebug() << (_currentItem ? _currentItem->name() : _pageId) << "resolved path" << fn;
    return fn;
}
