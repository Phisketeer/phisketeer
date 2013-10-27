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
#include "phidatasources.h"

class PHIRequest;
class PHIBasePage;
class PHIBaseItem;

class PHIDataParser
{
    Q_DECLARE_TR_FUNCTIONS( PHIDataParser )

public:
    explicit PHIDataParser( const PHIRequest *req, const QString &pageId, const QSqlDatabase &db )
        : _req( req ), _pageId( pageId ), _query( db ), _currentItem( 0 ) { _query.setForwardOnly( true ); }
    QVariant text( PHIData *data ) const;
    void createTmpImages( PHIImageData *data ) const;
    void createTmpImages( PHIImageBookData *data ) const;
    QByteArray imagePath( PHIImageData *data ) const;
    PHIByteArrayList imagePathes( PHIImageBookData *data ) const;
    QByteArray createTmpImage( QImage &img, const QByteArray &lang=PHIData::c(), int i=0 ) const;
    inline void setCurrentItem( const PHIBaseItem *it ) const { _currentItem=it; }
    inline const PHIRequest* request() const { return _req; }

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
    QByteArray createImageId( const QByteArray &name, const QByteArray &lang=PHIData::c(), int num=0 ) const;
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
};

#endif // PHIDATAPARSER_H
