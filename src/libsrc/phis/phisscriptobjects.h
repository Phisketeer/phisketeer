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
#ifndef PHISSCRIPTOBJECTS_H
#define PHISSCRIPTOBJECTS_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QtScript>
#include <QFile>
#include <QDir>
#include <QTcpSocket>
#include <QCryptographicHash>
#include <QUuid>
#include <QUrl>
#include <QDateTime>
#include <QLocale>
#include "phisrequest.h"
#include "phiresponserec.h"
#include "phisysinfo.h"
#include "phierror.h"

class PHISGlobalScriptObj : public QObject
{
    Q_OBJECT

public:
    explicit PHISGlobalScriptObj( const PHISRequest *req, QScriptEngine *e );
    virtual ~PHISGlobalScriptObj();
};

class PHISSqlObj : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString lasterror READ lastError )
    Q_PROPERTY( QStringList tables READ tables )
    Q_PROPERTY( bool next READ next )
    Q_PROPERTY( int numrowsaffected READ numRowsAffected )
    Q_PROPERTY( QList<QVariant> values READ values )
    Q_PROPERTY( int rows READ rows )
    Q_PROPERTY( QStringList properties READ properties )

public:
    explicit PHISSqlObj( const QSqlDatabase &db, QScriptEngine*, PHIResponseRec* , bool useSql );
    explicit PHISSqlObj( const QSqlDatabase &db, QScriptEngine*, PHIResponseRec* );
    virtual ~PHISSqlObj();

public slots:
    inline QString lastError() const { return _query.lastError().text(); }
    inline QStringList tables() const { return _db.isOpen() ? _db.tables() : QStringList(); }
    inline bool next() const { return _query.next(); }
    inline QVariant value( int index ) const { return _query.value( index ); }
    inline int numRowsAffected() const { return _query.numRowsAffected(); }
    inline QStringList properties() const { return PHI::properties( this ); }
    inline int rows() const { return _query.isValid() ? _query.size() : 0; }
    inline bool transaction() { return _db.isOpen() ? _db.transaction() : false; }
    inline bool commit() { return _db.isOpen() ? _db.commit() : false; }
    inline bool rollback() { return _db.isOpen() ? _db.rollback() : false; }
    bool query( const QString& ) const;
    QList<QVariant> values() const;
    QScriptValue clone() const;

private:
    QSqlDatabase _db;
    mutable QSqlQuery _query;
    PHIResponseRec *_resp;
};

#endif // PHISSCRIPTOBJECTS_H
