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

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "phidatasources.h"
#include "phiresponserec.h"
#include "phibaseitem.h"
#include "phisitem.h"
#include "phisrequest.h"

class PHIDataParser
{
    Q_DISABLE_COPY( PHIDataParser )

public:
    enum Type { Header, Cookie, Post, Get, Request, Server, All };
    explicit PHIDataParser( const QByteArray &pageId, const PHISRequest *req, const QSqlDatabase &db );
    ~PHIDataParser();
    QString parseVariables( const QString& ) const;
    PHIBaseItem* parseItem( const QByteArray &id ) const;
    QString text( const QString &itemId, const PHIData* ) const;
    QByteArray matchingLanguage( const PHIData* ) const;
    inline void setPageId( const QByteArray &id ) { _pageId=id; }

protected:
    QString findValue( const QString &key, int index, Type ) const;
    inline QString sqlValue( int index ) const { return _query.isValid() ?
        _query.value( index ).toString() : QString(); }
    QString replaceDefaultLangC( const QString&, bool useRoot=false ) const;
    QString genImageId( const QString &itemId, const PHISItem *it ) const;
    QString loadTextFromFile( const QString &fileName, const QString &codec ) const;
    QString loadTextFromDatabase( const QString &query, const QString &pattern ) const;
    QImage loadImageFromDatabase( const QString &query ) const;
    QByteArray loadFromProcess( const QString &procName, const QString &args ) const;
    QString loadTextFromUrl( const QString &url ) const;
    QImage loadImageFromUrl( const QString &url ) const;
    QByteArray loadFromLibrary( const QString &libName ) const;
    QStringList genImageBookIds( const QString &itemId, const PHIImageBookData *data,
        const PHISItem *it ) const;

private:
    mutable QSqlQuery _query;
    QByteArray _pageId;
    const PHISRequest *_req;
};

#endif // PHIDATAPARSER_H
