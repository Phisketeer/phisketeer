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
#include <QTextStream>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QRegExp>
#include <QSqlError>
#include <QUuid>
#include <QProcess>
#include <QLibrary>
#include <QUrl>
#include <QStringList>
#include "phidataparser.h"
#include "phi.h"
#include "phierror.h"
#include "phisitemcache.h"
#include "phinetrequest.h"
#include "phisimagecache.h"

static const char *_phiregid="^phi..";

PHIDataParser::PHIDataParser( const QByteArray &pageId, const PHISRequest *req,
    const QSqlDatabase &db ) : _query( db ), _pageId( pageId ), _req( req )
{
    //qDebug( "PHIDataParser::PHIDataParser()" );
    _query.setForwardOnly( true );
}

PHIDataParser::~PHIDataParser()
{
    //qDebug( "PHIDataParser::~PHIDataParser()" );
}

PHIBaseItem* PHIDataParser::parseItem( const QByteArray &name ) const
{
    const PHISItem *it=PHISItemCache::instance()->item( _req->hostname(), _pageId, name );
    if ( !it ) return 0;
    //QByteArray tmpPageId=_pageId;
    PHIBaseItem *bit;
    try {
        bit=new PHIBaseItem( *it );
        //if ( it->isTemplateItem() ) {
        //    _pageId=it->templatePageId();
        //}
    } catch ( std::bad_alloc& ) {
        _req->responseRec()->log( PHILOGCRIT, PHIRC_OUT_OF_MEMORY,
            QObject::tr( "Could not create item instance for page '%1'. Out of memory." ).arg( _pageId.data() ) );
        //_pageId=tmpPageId;
        return 0;
    }
    bit->setPageId( _pageId );
    PHIItem::Properties p=bit->itemProperties();
    QString id=it->id(); // id() performs an Utf8 conversion - to store it once is more efficient
    if ( p & PHIItem::PText ) {
        //qDebug( "text %s", qPrintable( id ) );
        bit->setValue( text( id, it->textData() ) );
    }
    if ( p & PHIItem::PStyleSheet ) {
        //qDebug( "stylesheet %s", qPrintable( id ) );
        bit->setStyleSheet( text( "phis_"+id, it->stylesheetData() ) );
    }
    if ( p & PHIItem::PToolTip ) {
        //qDebug( "tooltip %s", qPrintable( id ) );
        bit->setToolTip( text( "phit_"+id, it->toolTipData() ) );
    }
    if ( p & PHIItem::PImageBook ) {
        //qDebug( "imagebook %s", qPrintable( id ) );
        bit->setPictureBookIds( genImageBookIds( id, it->imageBookData(), it ) );
    }
    if ( p & PHIItem::PDisable ) bit->setDisabled( QVariant( text( id, it->disabledData() ) ).toBool() );
    if ( p & PHIItem::PVisible ) bit->setVisible( QVariant( text( id, it->visibleData() ) ).toBool() );
    if ( p & PHIItem::PCheckable ) bit->setChecked( QVariant( text( id, it->checkableData() ) ).toBool() );
    if ( p & PHIItem::PReadOnly ) bit->setReadOnly( QVariant( text( id, it->readOnlyData() ) ).toBool() );
    if ( p & PHIItem::PStartAngle ) bit->setStartAngle( text( id, it->startAngleData() ).toInt() );
    if ( p & PHIItem::PSpanAngle ) bit->setSpanAngle( text( id, it->spanAngleData() ).toInt() );
    if ( p & PHIItem::PImage ) bit->setImageId( genImageId( id, it ) );
    if ( p & PHIItem::PShortCut ) bit->setAccessKey( it->textData()->shortCut() );
    if ( p & PHIItem::PLabel ) {
        // text represents label of checkbox, radiobutton, combobox, etc. - so swap data
        Q_ASSERT( p & PHIItem::PText );
        bit->setLabel( bit->value() );
        bit->setValue( it->textData()->value() );
        bit->setDelimiter( it->textData()->delimiter() );
    }
    if ( p & PHIItem::PDelimiter ) bit->setDelimiter( it->textData()->delimiter() );
    // Copied internaly through _variants Hash:
    //if ( p & PHIItem::PColor ) bit->setColor( it->color() );
    //if ( p & PHIItem::POutlineColor ) bit->setOutlineColor( it->outlineColor() );
    //if ( p & PHIItem::PPenWidth ) bit->setPenWidth( it->penWidth() );
    //if ( p & PHIItem::PPattern ) bit->setPattern( it->pattern() );
    //if ( p & PHIItem::PLine ) bit->setLine( it->line() );
    //if ( p & PHIItem::PFont ) bit->setFont( it->font() );
    //if ( p & PHIItem::PAlignment ) bit->setAlignment( it->alignment() );
    //_pageId=tmpPageId;
    return bit;
}

QByteArray PHIDataParser::matchingLanguage( const PHIData *data ) const
{
    if ( data->keys().contains( _req->currentLangByteArray() ) ) return _req->currentLangByteArray();
    QString lang;
    foreach ( lang, _req->acceptedLanguages() )
        if ( data->keys().contains( lang.toAscii() ) ) return lang.toAscii();
    foreach ( lang, _req->acceptedLanguages() ) {
        if ( data->keys().contains( lang.left( 2 ).toAscii() ) ) return lang.left( 2 ).toAscii();
    }
    return PHIData::c();
}

QString PHIDataParser::parseVariables( const QString &s ) const
{
    QString t=s, type, key, sub, replace;
    QStringList list;
    int pos=0, start, end;
    pos=t.indexOf( "$PHI", pos );
    while ( pos!=-1 ) {
        start=t.indexOf( '[', pos );
        end=t.indexOf( ']', start );
        if ( start==-1 || end==-1 ) break;
        type=t.mid( pos+4, start-pos-4 );
        key=t.mid( start+1, end-start-1 );
        start=t.indexOf( '[', ++start );
        if ( start==end+1 ) {
            end=t.indexOf( ']', start );
            if ( end==-1 ) break;
            sub=t.mid( start+1, end-start-1 );
        } else sub="0";
        replace="";
        if ( type=="SERVER" ) replace=findValue( key, sub.toInt(), Server );
        else if ( type=="SQL" ) replace=sqlValue( key.toInt() );
        else if ( type=="POST" ) replace=findValue( key, sub.toInt(), Post );
        else if ( type=="GET" ) replace=findValue( key, sub.toInt(), Get );
        else if ( type=="REQUEST" ) replace=findValue( key, sub.toInt(), Request );
        else if ( type=="HEADER" ) replace=findValue( key, sub.toInt(), Header );
        else if ( type=="FILE" ) {
            if ( sub=="size" ) replace=_req->fileSize( key.toUtf8() );
            else if ( sub=="filename" ) replace=_req->fileName( key );
            else if ( sub=="tmpfile" ) replace=_req->tmpFile( key );
        } else if ( type=="COOKIE" ) replace=_req->cookieValue( key );
        else if ( type=="ALL" ) replace=findValue( key, sub.toInt(), All );
        t.replace( pos, end-pos+1, replace );
        pos=t.indexOf( "$PHI", ++pos );
    }
    return t;
}

QString PHIDataParser::findValue( const QString &key, int index, Type type ) const
{
    QStringList list;
    switch ( type ) {
    case Server:
        if ( key=="today" ) {
            return QDate::currentDate().addDays( index ).toString( PHI::isoDateFormat() );
        } else if ( key=="monthlist" ) {
            QLocale locale( _req->currentLang() );
            QString monthnames;
            for( int i=1; i<13; i++ ) {
                monthnames+=locale.monthName( i, QLocale::LongFormat )
                    +'['+QString::number(i)+"]\n";
            }
            return monthnames;
        } else if ( key=="month" ) {
            QLocale locale( _req->currentLang() );
            if ( index>12 || index<0 ) index=0;
            if ( index==0 ) return locale.monthName( QDate::currentDate().month(), QLocale::LongFormat );
            return locale.monthName( index, QLocale::LongFormat );
        } else if ( key=="year" ) {
            int year=QDate::currentDate().year();
            return QString::number( year+index );
        } else if ( key=="yearlist" ) {
            if ( index>1000 || index<0 ) index=0;
            if ( index==0 ) return QDate::currentDate().toString( "yyyy" );
            int year=QDate::currentDate().year();
            QString years=QString::number( year );
            for ( int i=1; i<index; i++ ) {
                years+='\n'+QString::number( year+i );
            }
            return years;
        } else if ( key=="version" ) return _req->version();
        else if ( key=="userengine" ) return _req->userEngine();
        else if ( key=="useragent" ) return _req->userAgent();
        else if ( key=="useros" ) return _req->userOS();
        return _req->serverValue( key );
    case Header:
        return _req->headerValue( key );
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
        if ( !_req->headerValue( key ).isEmpty() ) list << _req->headerValue( key );
    }
    if ( list.count()>0 ) {
        if ( index>-1 && index<list.count() ) return list.at( index );
        return list.first();
    }
    return QString();
}

QString PHIDataParser::replaceDefaultLangC( const QString &fileName, bool useRoot ) const
{
    QString fn=fileName;
    if ( useRoot ) {
        fn=PHI::getLocalFilePath( _req->documentRoot(), _req->canonicalFilename(), fileName );
    }
    QString cmatch="/C/";
    if ( fileName.contains( cmatch ) ) {
        QString testfn;
        testfn=fn;
        testfn.replace( cmatch, '/'+_req->currentLang()+'/' );
        if ( QFile::exists( testfn ) ) return testfn;
        QString lang;
        foreach ( lang, _req->acceptedLanguages() ) {
            testfn=fn;
            testfn.replace( cmatch, '/'+lang+'/' );
            if ( QFile::exists( testfn ) ) return testfn;
        }
    }
    cmatch="\\C\\";
    if ( fileName.contains( cmatch ) ) {
        QString testfn;
        testfn=fn;
        testfn.replace( cmatch, '\\'+_req->currentLang()+'\\' );
        if ( QFile::exists( testfn ) ) return testfn;
        QString lang;
        foreach ( lang, _req->acceptedLanguages() ) {
            testfn=fn;
            testfn.replace( cmatch, '\\'+lang+'\\' );
            if ( QFile::exists( testfn ) ) return testfn;
        }
    }
    return fn;
}

QString PHIDataParser::loadTextFromFile( const QString &fileName, const QString &codec ) const
{
    QString tmp=replaceDefaultLangC( fileName, true );
    QFile f( tmp );
    if ( !f.open( QIODevice::ReadOnly ) ) {
        _req->responseRec()->log( PHILOGERR, PHIRC_IO_FILE_ACCESS_ERROR,
            QObject::tr( "Could not open file '%1' for reading in page '%2'." )
            .arg( tmp ).arg( _pageId.data() )+PHI::errorText().arg( f.errorString() ) );
        return QString();
    }
    QTextStream t( &f );
    if ( !codec.isEmpty() ) t.setCodec( codec.toUtf8() );
    tmp=t.readAll();
    f.close();
    return tmp;
}

QString PHIDataParser::loadTextFromDatabase( const QString &statement, const QString &templateText ) const
{
    if ( !_query.exec( parseVariables( statement ) ) ) {
        _req->responseRec()->log( PHILOGERR, PHIRC_QUERY_ERROR,
            QObject::tr( "Could not execute query '%1' for page '%2'." )
            .arg( parseVariables( statement ) ).arg( _pageId.data() )+
            PHI::errorText().arg( _query.lastError().text() ) );
        return templateText;
    }
    QString tmp;
    if ( _query.size() < 1 ) {
        tmp=statement;
        tmp.replace( "$PHISERVER[lang]", "C" );
        _query.exec( parseVariables( tmp ) );
        tmp.clear();
    }
    while ( _query.next() ) {
        tmp.append( parseVariables( templateText ) );
    }
    return tmp;
}

QString PHIDataParser::text( const QString &itemId, const PHIData *data ) const
{
    Q_ASSERT( data );
    QString text;
    PHISItemCache *cache=PHISItemCache::instance();
    bool noCache=data->options() & PHIData::NoCache;
    if ( !dynamic_cast<const PHITextData*>(data) ) noCache=true;

    if ( data->source()==PHIData::Static ) text=data->variant().toString();
    else if ( data->source()==PHIData::Translated ) {
        text=data->variant( matchingLanguage( data ) ).toString();
        if ( text.isEmpty() ) text=data->variant().toString();
    } else if ( data->source()==PHIData::File ) {
        if ( noCache )
            text=loadTextFromFile( data->fileName(), data->textCodec() );
        else {
            text=cache->text( _pageId+itemId );
            if ( text.isEmpty() ) {
                text=loadTextFromFile( data->fileName(), data->textCodec() );
                cache->insertText( _pageId+itemId, text );
            }
        }
    } else if ( data->source()==PHIData::Database ) {
        if ( noCache ) {
            text=data->header()+
                loadTextFromDatabase( data->sqlStatement(), data->templateText() )
                +data->footer();
        } else {
            text=cache->text( _pageId+itemId );
            if ( text.isEmpty() ) {
                text=data->header()+
                    loadTextFromDatabase( data->sqlStatement(), data->templateText() )
                    +data->footer();
                cache->insertText( _pageId+itemId, text );
            }
        }
    } else if ( data->source()==PHIData::Process ) {
        if ( noCache ) {
            text=QString::fromUtf8( loadFromProcess( data->processName(), data->attributes() ) );
        } else {
            text=cache->text( _pageId+itemId );
            if ( text.isEmpty() ) {
                text=QString::fromUtf8( loadFromProcess( data->processName(), data->attributes() ) );
                cache->insertText( _pageId+itemId, text );
            }
        }
    } else if ( data->source()==PHIData::Library ) {
        if ( noCache ) {
            text=QString::fromUtf8( loadFromLibrary( data->libraryName() ) );
        } else {
            text=cache->text( _pageId+itemId );
            if ( text.isEmpty() ) {
                text=QString::fromUtf8( loadFromLibrary( data->libraryName() ) );
                cache->insertText( _pageId+itemId, text );
            }
        }
    } else if ( data->source()==PHIData::Url ) {
        if ( noCache ) {
            text=loadTextFromUrl( data->url() );
        } else {
            text=cache->text( _pageId+itemId );
            if ( text.isEmpty() ) {
                text=loadTextFromUrl( data->url() );
                cache->insertText( _pageId+itemId, text );
            }
        }
    } else {
        text=itemId;
        text.replace( QRegExp( _phiregid ), "" );
        _req->responseRec()->log( PHILOGERR, PHIRC_ARGUMENT_ERROR,
            QObject::tr( "Unregistered data source for item '%1' in page '%2'." )
            .arg( text ).arg( _pageId.data() ) );
        return QString();
    }
    if ( data->options() & PHIData::Parse ) text=parseVariables( text );
    return text;
}

QImage PHIDataParser::loadImageFromDatabase( const QString &query ) const
{
    if ( !_query.exec( parseVariables( query ) ) ) {
        _req->responseRec()->log( PHILOGERR, PHIRC_QUERY_ERROR,
            QObject::tr( "Could not load raw image data from DB for query '%1' in page '%2'." )
            .arg( parseVariables( query ) ).arg( _pageId.data() )+
            PHI::errorText().arg( _query.lastError().text() ) );
        return QImage();
    }
    if ( _query.isValid() && _query.next() ) {
        QImage img=QImage::fromData( _query.value( 0 ).toByteArray() );
        if ( img.isNull() ) {
            _req->responseRec()->log( PHILOGERR, PHIRC_OBJ_TYPE_ERROR,
                QObject::tr( "Could not convert data from DB to image data for query '%1'"
                " in page '%2'." ).arg( parseVariables( query ) ).arg( _pageId.data() ) );
        } else return img;
    } else {
        _req->responseRec()->log( PHILOGERR, PHIRC_QUERY_ERROR,
            QObject::tr( "No result set for DB query '%1' in page '%2'.")
            .arg( parseVariables( query ).arg( _pageId.data() ) )+
            PHI::errorText().arg( _query.lastError().text() ) );
    }
    return QImage();
}

QByteArray PHIDataParser::loadFromLibrary( const QString &libName ) const
{
    Q_UNUSED( libName );
    _req->responseRec()->log( PHILOGWARN, PHIRC_NOT_IMPL_ERROR,
        QObject::tr( "Loading from a library is not yet implemented.") );
    return QByteArray();
}

QByteArray PHIDataParser::loadFromProcess( const QString &procName, const QString &a ) const
{
    //QString name=replaceDefaultLangC( procName, true );
    QString name=parseVariables( procName );
    QProcess proc;
    QString arg=parseVariables( a );
    QStringList args=arg.split( QRegExp( "\\s" ), QString::SkipEmptyParts );
    proc.start( name, args, QIODevice::ReadOnly );
    if ( !proc.waitForStarted() ) {
        _req->responseRec()->log( PHILOGERR, PHIRC_OBJ_ACCESS_ERROR,
            QObject::tr( "Could not start process '%1' with arguments '%2' for page '%3'." )
            .arg( name ).arg( args.join( " " ) )
            .arg( _pageId.data() )+PHI::errorText().arg( proc.errorString() ) );
        return QByteArray();
    }
    if ( !proc.waitForFinished() ) {
        _req->responseRec()->log( PHILOGERR, PHIRC_OBJ_ACCESS_ERROR,
            QObject::tr( "Could not finish process '%1' with arguments '%2' for page '%3'.")
            .arg( name ).arg( args.join( " " ) )
            .arg( _pageId.data() )+PHI::errorText().arg( proc.errorString() ) );
        return QByteArray();
    }
    if ( proc.exitCode() ) {
        _req->responseRec()->log( PHILOGWARN, PHIRC_OBJ_ACCESS_ERROR,
            QObject::tr( "Process '%1' exit code is '%2' for page '%3'.")
            .arg( name ).arg( proc.exitCode() ).arg( _pageId.data() )
            +PHI::errorText().arg( QString::fromUtf8( proc.readAllStandardError() ) ) );
    }
    return proc.readAllStandardOutput();
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

QStringList PHIDataParser::genImageBookIds( const QString &itemId, const PHIImageBookData *data,
    const PHISItem *it ) const
{
    Q_ASSERT( data );
    QByteArray lang=matchingLanguage( data );
    QString id;
    QStringList ids;
    PHIImageHash hash;

    switch ( data->source() ) {
    case PHIData::Static:
        hash=data->imageBook( PHIData::c() ); break;
    case PHIData::Translated:
        hash=data->imageBook( lang ); break;
    case PHIData::File: {
        QString root;
        QStringList chkIds=data->fileNames();
        foreach ( id, chkIds ) {
            if ( !id.startsWith( '/' ) && !id.startsWith( '\\' ) ) {
                QFileInfo info( _req->canonicalFilename() );
                QFileInfo ri( _req->documentRoot() );
                root=info.canonicalPath()+QDir::separator()+id;
                id=root;
                id.replace( ri.canonicalFilePath(), "" );
            } else root=_req->documentRoot()+id;
            if ( !QFile::exists( root ) ) {
                _req->responseRec()->log( PHILOGERR, PHIRC_OBJ_NOT_FOUND_ERROR,
                    QObject::tr( "Image file '%1' for item '%2' in page '%3' not found." )
                    .arg( root ).arg( itemId ).arg( _pageId.data() ) );
                continue;
            }
            ids.append( id );
        }
        return ids;
    }
    default: {
        QString root;
        id=text( itemId, data );
        QStringList chkIds=id.split( QRegExp( "\\s" ), QString::SkipEmptyParts );
        foreach ( id, chkIds ) {
            if ( !id.startsWith( '/' ) && !id.startsWith( '\\' ) ) {
                QFileInfo info( _req->canonicalFilename() );
                QFileInfo ri( _req->documentRoot() );
                root=info.canonicalPath()+QDir::separator()+id;
                id=root;
                id.replace( ri.canonicalFilePath(), "" );
            } else root=_req->documentRoot()+id;
            if ( !QFile::exists( root ) ) {
                _req->responseRec()->log( PHILOGERR, PHIRC_OBJ_NOT_FOUND_ERROR,
                    QObject::tr( "Image file '%1' for item '%2' in page '%3' not found." )
                    .arg( root ).arg( itemId ).arg( _pageId.data() ) );
                continue;
            }
            ids.append( id );
        }
        return ids;
    }
    }

    QImage img;
    for ( int i=0; i<hash.count(); i++ ) {
        if ( data->options() & PHIData::NoCache ) {
            PHISImageCache cache( _req->responseRec(), _req->tmpDir() );
            id=cache.createId();
        } else {
            id="phi"+_pageId+'_'+itemId+'_'+QString::fromAscii( lang )+"_"+QString::number( i )+".png";
            if ( QFile::exists( _req->imgDir()+QDir::separator()+id ) ) {
                QDateTime pageModified=QFileInfo( _req->canonicalFilename() ).lastModified();
                QFileInfo fi( _req->imgDir()+QDir::separator()+id );
                if ( fi.lastModified() >= pageModified ) {
                    qDebug( "Image file %s is up to date", qPrintable( id ) );
                    ids.append( id );
                    continue;
                }
            }
        }
        img=hash.value( QByteArray::number( i ) );
        if ( img.isNull() ) {
            _req->responseRec()->log( PHILOGERR, PHIRC_OBJ_NOT_FOUND_ERROR,
                QObject::tr( "Invalid image data for item '%1' in page '%2', image #%3." )
                .arg( itemId ).arg( _pageId.data() ).arg( i ) );
            continue;
        }
        if ( img.width()!=static_cast<int>(it->width()) || img.height()!=static_cast<int>(it->height()) ) {
            img=img.scaled( static_cast<int>(it->width()),
            static_cast<int>(it->height()), Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
        }
        QDir dir( _req->imgDir() );
        QFileInfoList imglist=dir.entryInfoList( QStringList() << id+'*', QDir::Files );
        QFileInfo info;
        foreach ( info, imglist ) {  // removing existing transformed images
            qDebug( "deleting %s", qPrintable( info.canonicalFilePath() ) );
            QFile::remove( info.canonicalFilePath() );
        }

        // saving the untransformed image without any graphics effect on it
        img.save( _req->imgDir()+QDir::separator()+id, "PNG" );
        ids.append( id );
    }
    return ids;
}

QString PHIDataParser::genImageId( const QString &itId, const PHISItem *it ) const
{
    QString id, itemId=itId;
    const PHIImageData *data=it->imageData();

    Q_ASSERT( data );

    if ( data->options() & PHIData::FileName ) {
        qDebug( "image is filename");
        PHISItemCache *cache=PHISItemCache::instance();
        itemId="phii_"+itId; //new cache id (otherwise conflicts with textData)
        switch ( data->source() ) {
        case PHIData::File:
            if ( data->options() & PHIData::NoCache ) {
                id=replaceDefaultLangC( parseVariables( data->fileName() ) );
            } else {
                id=cache->text( _pageId+itemId );
                if ( id.isEmpty() ) {
                    id=replaceDefaultLangC( parseVariables( data->fileName() ) );
                    cache->insertText( _pageId+itemId, id );
                }
            }
            break;
        case PHIData::Database:
            if ( data->options() & PHIData::NoCache ) {
                id=replaceDefaultLangC( loadTextFromDatabase(
                    data->sqlStatement(), data->templateText() ) );
                if ( id==data->templateText() ) id=QString(); // DB query error
            } else {
                id=cache->text( _pageId+itemId );
                if ( id.isEmpty() ) {
                    id=replaceDefaultLangC( loadTextFromDatabase(
                        data->sqlStatement(), data->templateText() ) );
                    if ( id==data->templateText() ) id=QString(); // DB query error
                    cache->insertText( _pageId+itemId, id );
                }
            }
            break;
        case PHIData::Process:
            if ( data->options() & PHIData::NoCache ) {
                id=replaceDefaultLangC( QString::fromUtf8(
                    loadFromProcess( data->processName(), data->attributes() ) ) );
            } else {
                id=cache->text( _pageId+itemId );
                if ( id.isEmpty() ) {
                    id=replaceDefaultLangC( QString::fromUtf8(
                        loadFromProcess( data->processName(), data->attributes() ) ) );
                    cache->insertText( _pageId+itemId, id );
                }
            }
            break;
        case PHIData::Library:
            if ( data->options() & PHIData::NoCache )
                id=replaceDefaultLangC( QString::fromUtf8( loadFromLibrary( data->libraryName() ) ) );
            else {
                id=cache->text( _pageId+itemId );
                if ( id.isEmpty() ) {
                    id=replaceDefaultLangC( QString::fromUtf8( loadFromLibrary( data->libraryName() ) ) );
                    cache->insertText( _pageId+itemId, id );
                }
            }
            break;
        case PHIData::Url:
            if ( data->options() & PHIData::NoCache )
                id=replaceDefaultLangC( loadTextFromUrl( data->url() ) );
            else {
                id=cache->text( _pageId+itemId );
                if ( id.isEmpty() ) {
                    id=replaceDefaultLangC( loadTextFromUrl( data->url() ) );
                    cache->insertText( _pageId+itemId, id );
                }
            }
            break;
        default:
            _req->responseRec()->log( PHILOGERR, PHIRC_OBJ_TYPE_ERROR,
                QObject::tr( "Option 'FileName' is set on raw image data for item '%1' "
                "in page '%2'." ).arg( itId ).arg( _pageId.data() ) );
            return QString();
        }

        if ( id.isEmpty()  ) {
            _req->responseRec()->log( PHILOGERR, PHIRC_OBJ_ACCESS_ERROR,
                QObject::tr( "Could not resolve path for item '%1' in page '%2'." )
                .arg( id ).arg( itId ) );
            return QString();
        }
        QString root;
        if ( !id.startsWith( '/' ) && !id.startsWith( '\\' ) ) {
            QFileInfo info( _req->canonicalFilename() );
            QFileInfo ri( _req->documentRoot() );
            root=info.canonicalPath()+QDir::separator()+id;
            //qDebug( "PATH id %s", qPrintable( root ) );
            id=root;
            id.replace( ri.canonicalFilePath(), "" );
            //qDebug( "PATH id %s", qPrintable( id ) );
        } else root=_req->documentRoot()+id;
        if ( !QFile::exists( root ) ) {
            _req->responseRec()->log( PHILOGERR, PHIRC_OBJ_NOT_FOUND_ERROR,
                QObject::tr( "Image file '%1' for item '%2' in page '%3' not found." )
                .arg( root ).arg( itId ).arg( _pageId.data() ) );
            return QString();
        }
        return id;
    }

    QByteArray lang=matchingLanguage( data );

    if ( data->options() & PHIData::NoCache ) {
        PHISImageCache cache( _req->responseRec(), _req->tmpDir() );
        id=cache.createId();
    } else {
        id="phi"+_pageId+'_'+itId+'_'+QString::fromAscii( lang )+".png";
        if ( QFile::exists( _req->imgDir()+QDir::separator()+id ) ) {
            QDateTime pageModified=QFileInfo( _req->canonicalFilename() ).lastModified();
            QFileInfo fi( _req->imgDir()+QDir::separator()+id );
            if ( fi.lastModified() >= pageModified ) {
                qDebug( "Image file %s is up to date", qPrintable( id ) );
                return id;
            }
        }
    }

    QImage img;
    if ( data->source()==PHIData::Static ) img=data->image();
    else if ( data->source()==PHIData::Translated ) img=data->image( lang );
    else if ( data->source()==PHIData::Database )
        img=loadImageFromDatabase( data->sqlStatement() );
    else if ( data->source()==PHIData::Process )
        img=QImage::fromData( loadFromProcess( data->processName(), data->attributes() ) );
    else if ( data->source()==PHIData::Library )
        img=QImage::fromData( loadFromLibrary( data->libraryName() ) );
    else if ( data->source()==PHIData::Url )
        img=loadImageFromUrl( data->url() );
    else {
        _req->responseRec()->log( PHILOGERR, PHIRC_ARGUMENT_ERROR,
            QObject::tr( "Unregistered image data source for item '%1' in page '%2'." )
            .arg( itId ).arg( _pageId.data() ) );
        return QString();
    }
    if ( img.isNull() ) {
        _req->responseRec()->log( PHILOGERR, PHIRC_OBJ_NOT_FOUND_ERROR,
            QObject::tr( "Invalid image data for item '%1' in page '%2'." )
            .arg( itId ).arg( _pageId.data() ) );
        return QString();
    }
    if ( img.width()!=static_cast<int>(it->width()) || img.height()!=static_cast<int>(it->height()) ) {
        if ( it->wid()==PHI::IMAGE_BUTTON ) {
            img=img.scaled( static_cast<int>(it->height()/1.5),
                static_cast<int>(it->height()/1.5), Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
        } else {
            img=img.scaled( static_cast<int>(it->width()),
                static_cast<int>(it->height()), Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
        }
    }
    QDir dir( _req->imgDir() );
    QFileInfoList imglist=dir.entryInfoList( QStringList() << id+'*', QDir::Files );
    QFileInfo info;
    foreach ( info, imglist ) { // removing existing transformed images
        qDebug( "deleting %s", qPrintable( info.canonicalFilePath() ) );
        QFile::remove( info.canonicalFilePath() );
    }

    // saving the untransformed image without any graphics effect on it
    img.save( _req->imgDir()+QDir::separator()+id, "PNG" );
    return id;
}
