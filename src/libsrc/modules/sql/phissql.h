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
#ifndef PHISSQL_H
#define PHISSQL_H
#include <QSqlQuery>
#include <QSqlError>
#include "phismodule.h"

class PHISEmailModule : public PHISModule
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.phisketeer.phis.module.sql" FILE "phissql.json")
    Q_CLASSINFO( "Author", "Marius Schumacher" )
    Q_CLASSINFO( "Url", "http://www.phisketeer.org" )
    Q_CLASSINFO( "Version", "1.0" )
    Q_CLASSINFO( "License", "LGPL" )
    Q_CLASSINFO( "Copyright", "2013 Phisys AG, 2013 Phisketeer Team" )

public:
    virtual PHISScriptObj* create( const QString &key, const PHISInterface* ) const;
    virtual QStringList keys() const;
};

class PHISSqlObj : public PHISScriptObj
{
    Q_OBJECT
    Q_PROPERTY( QString lasterror READ lastError )
    Q_PROPERTY( QStringList tables READ tables )
    Q_PROPERTY( bool next READ next )
    Q_PROPERTY( int numrowsaffected READ numRowsAffected )
    Q_PROPERTY( QList<QVariant> values READ values )
    Q_PROPERTY( int rows READ rows )

public:
    explicit PHISSqlObj( const PHISInterface *interf );

public slots:
    inline QString lastError() const { return _query.lastError().text(); }
    inline QStringList tables() const { return PHIS_IF()->database().isOpen() ?
        PHIS_IF()->database().tables() : QStringList(); }
    inline bool next() const { return _query.next(); }
    inline QVariant value( int index ) const { return _query.value( index ); }
    inline int numRowsAffected() const { return _query.numRowsAffected(); }
    inline int rows() const { return _query.isValid() ? _query.size() : 0; }
    inline bool transaction() { return PHIS_IF()->database().isOpen() ?
        PHIS_IF()->database().transaction() : false; }
    inline bool commit() { return PHIS_IF()->database().isOpen() ?
        PHIS_IF()->database().commit() : false; }
    inline bool rollback() { return PHIS_IF()->database().isOpen() ?
        PHIS_IF()->database().rollback() : false; }
    bool query( const QString& ) const;
    QList<QVariant> values() const;
    QScriptValue clone() const;

protected:
    virtual QScriptValue initObject( QScriptEngine *engine, const QString &key );

private:
    mutable QSqlQuery _query;
    QScriptEngine *_engine;
};

inline PHISScriptObj* PHISEmailModule::create( const QString &key, const PHISInterface *interface ) const
{
    if ( key==QLatin1String( "sql" ) ) return new PHISSqlObj( interface );
    return 0;
}

inline QStringList PHISEmailModule::keys() const
{
    return QStringList() << QStringLiteral( "sql" );
}

#endif // PHISSQL_H
