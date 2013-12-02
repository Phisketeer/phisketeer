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
#ifndef PHIDATAPARSER_H
#define PHIDATAPARSER_H
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QCoreApplication>
#include <QReadWriteLock>
#include <QCryptographicHash>
#include <QDir>
#include "phidatasources.h"
#include "phiimagecache.h"
#include "phirequest.h"
#include "phiresponserec.h"
#include "phibaseitem.h"

class PHIDataParser
{
    Q_DECLARE_TR_FUNCTIONS( PHIDataParser )

public:
    explicit PHIDataParser( const PHIRequest *req, const QString &pageId, const QSqlDatabase &db )
        : _req( req ), _pageId( pageId ), _query( db ), _currentItem( 0 ) { _query.setForwardOnly( true ); }
    QVariant text( PHIData *data ) const;
    void createImages( PHIImageData *data ) const;
    void createImages( PHIImageBookData *data ) const;
    QByteArray imagePath( PHIImageData *data ) const;
    QByteArray imagePath( const QByteArray &lang=PHIData::c(), int i=0 ) const;
    PHIByteArrayList imagePathes( PHIImageBookData *data ) const;
    QByteArray createImage( const QImage &img, const QByteArray &lang=PHIData::c(), int i=0 ) const;
    inline void setCurrentItem( const PHIBaseItem *it ) const { _currentItem=it; }
    inline const PHIRequest* request() const { return _req; }

    static QByteArray createTransformedImage( const PHIRequest *req, const PHIBaseItem *it, int i, QRectF &br );
    static QByteArray createImageId( const PHIRequest *req, const QByteArray &name, const QByteArray &lang=PHIData::c(), int num=0 );
    static QByteArray createImage( const PHIRequest *req, const PHIBaseItem *it, const QImage &img, const QByteArray &lang=PHIData::c(), int num=0 );
    static void insertTransformedImageRect( const QByteArray &imgId, const QRectF &r );
    static QRectF transformedImageRect( const QByteArray &imgId );
    static void insertGraphicsImageRect( const QByteArray &imgId, const QRectF &r );
    static QRectF graphicsImageRect( const QByteArray &imgId );

protected:
    enum Type { Header, Cookie, Post, Get, Request, Server, All };
    QString parseVariables( const QString &arr ) const;
    QString findValue( const QString &key, int index, Type type ) const;
    QString replaceDefaultLangC( const QString &filename, QByteArray &matchedLang ) const;
    QString loadTextFromFile( const QString &filename, const QString &codec, QByteArray &matchedLang ) const;
    QString loadTextFromUrl( const QString &url ) const;
    QString loadTextFromDB( const QString &statement, const QString &templateText ) const;
    QByteArray loadFromProcess( const QString &procName, const QString &a ) const;
    QImage loadImageFromFile( const QString &filename, QByteArray &matchedLang ) const;
    QImage loadImageFromUrl( const QString &url ) const;
    QImage loadImageFromDB( const QString &statement ) const;
    QList <QImage> loadImagesFromDB( const QString &statement ) const;
    QByteArray resolveImageFile( const QString &filename, QByteArray &matchedLang ) const;
    QByteArray matchedLangForResolvedFilename( const QString &filename ) const;
    QString resolvePath( const QString &path ) const;
    void saveImage( const QImage &img, const QByteArray &id ) const;
    void cacheText( PHIData *data, const QString &t, const QByteArray &lang=PHIData::c() ) const;
    void cacheImageFile( PHIImageData *data, const QString &filename ) const;
    void cacheImageFiles( PHIImageBookData *data, const QStringList &filenames ) const;
    inline QString sqlValue( int index ) const { return _query.isValid() ? _query.value( index ).toString() : QString(); }

private:
    const PHIRequest *_req;
    const QString _pageId;
    mutable QSqlQuery _query;
    mutable const PHIBaseItem *_currentItem;
    static QReadWriteLock _lock;
    static QHash <QByteArray, QRectF> _imageTransformedRects;
    static QHash <QByteArray, QRectF> _imageGraphicsRects;
};

inline QByteArray PHIDataParser::createImageId( const PHIRequest *req, const QByteArray &name, const QByteArray &lang, int num )
{
    QByteArray arr;
    if ( Q_UNLIKELY( num==-1 ) ) arr=PHIImageCache::instance()->createUid( req );
    else {
        arr=name+QByteArray::number( num )+lang+req->canonicalFilename().toUtf8();
        arr=QCryptographicHash::hash( arr, QCryptographicHash::Md5 ).toHex();
        arr.squeeze(); // will be cached in memory so free unused space
    }
    qDebug() << "createImageId" << name << num << lang << arr;
    return arr;
}

inline QByteArray PHIDataParser::imagePath( const QByteArray &lang, int num ) const
{
    Q_ASSERT( num>=0 );
    Q_ASSERT( _currentItem );
    qDebug() << "imagePath" << lang << num << _currentItem->id();
    return createImageId( _req, _currentItem->id(), lang, num );
}

inline QByteArray PHIDataParser::resolveImageFile( const QString &filename, QByteArray &matchedLang  ) const
{
    QString fn=replaceDefaultLangC( filename, matchedLang );
    QString docroot=QDir::fromNativeSeparators( _req->documentRoot() );
    return fn.replace( docroot, QString() ).toUtf8();
}

inline QByteArray PHIDataParser::createImage( const QImage &img, const QByteArray &lang, int i ) const
{
    Q_ASSERT( _currentItem );
    QByteArray id=createImageId( _req, _currentItem->id(), lang, i );
    qDebug() << "createImage" << id << _currentItem->id();
    saveImage( img, id );
    return id;
}

inline QByteArray PHIDataParser::createImage( const PHIRequest *req, const PHIBaseItem *it, const QImage &img, const QByteArray &lang, int num )
{
    QByteArray id=createImageId( req, it->id(), lang, num );
    if ( Q_UNLIKELY( img.isNull() ) ) {
        req->responseRec()->log( PHILOGERR, PHIRC_IO_FILE_CREATION_ERROR,
            tr( "Can not create an 'empty' cached image for page '%1' and id '%2'.")
            .arg( req->canonicalFilename() ).arg( it->name() ) );
        return id;
    }
    QString path( req->imgDir()+QLatin1Char( '/' )+QString::fromLatin1( id )+SL( ".png" ) );
    if ( Q_UNLIKELY( img.format()!=QImage::Format_ARGB32_Premultiplied ) ) {
        img.convertToFormat( QImage::Format_ARGB32_Premultiplied ).save( path, "PNG" );
    } else img.save( path, "PNG" );
    return id;
}

inline void PHIDataParser::saveImage( const QImage &img, const QByteArray &id ) const
{
    if ( Q_UNLIKELY( img.isNull() ) ) {
        _req->responseRec()->log( PHILOGERR, PHIRC_IO_FILE_CREATION_ERROR,
            tr( "Can not create an 'empty' cached image for page '%1' and id '%2'.")
            .arg( _req->canonicalFilename() ).arg( _currentItem ? _currentItem->name() : _pageId ) );
        return;
    }
    QString path( _req->imgDir()+QLatin1Char( '/' )+QString::fromLatin1( id )+SL( ".png" ) );
    if ( img.format()!=QImage::Format_ARGB32_Premultiplied ) {
        img.convertToFormat( QImage::Format_ARGB32_Premultiplied ).save( path, "PNG" );
    } else img.save( path, "PNG" );
}

inline void PHIDataParser::insertTransformedImageRect( const QByteArray &id, const QRectF &br )
{
    _lock.lockForWrite();
    _imageTransformedRects.insert( id, br );
    _lock.unlock();
}

inline QRectF PHIDataParser::transformedImageRect( const QByteArray &id )
{
    _lock.lockForRead();
    QRectF br=_imageTransformedRects.value( id, QRectF() );
    _lock.unlock();
    return br;
}

inline void PHIDataParser::insertGraphicsImageRect( const QByteArray &id, const QRectF &br )
{
    _lock.lockForWrite();
    _imageGraphicsRects.insert( id, br );
    _lock.unlock();
}

inline QRectF PHIDataParser::graphicsImageRect( const QByteArray &id )
{
    _lock.lockForRead();
    QRectF br=_imageGraphicsRects.value( id, QRectF() );
    _lock.unlock();
    return br;
}

#endif // PHIDATAPARSER_H
