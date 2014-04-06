/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2014  Phisys AG, Switzerland
#    Copyright (C) 2012-2014  Phisketeer.org team
#
#    This C++ library is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
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
    Q_CLASSINFO( "Version", "1.2" )
    Q_CLASSINFO( "License", "LGPL" )
    Q_CLASSINFO( "Copyright", "2013 Phisys AG, 2013-2014 Phisketeer Team" )

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
    inline QVariant value( const QString &col ) const { return _query.value( col ); }
    inline void bind( const QString &placeholder, const QScriptValue &value ) const {
        if ( value.isArray() ) _query.bindValue( placeholder, value.property( 0 ).toVariant() );
        else _query.bindValue( placeholder, value.toVariant() );
    }
    inline void bind( int pos, const QScriptValue &value ) const {
        if ( value.isArray() ) _query.bindValue( pos, value.property( 0 ).toVariant() );
        else _query.bindValue( pos, value.toVariant() );
    }
    inline int numRowsAffected() const { return _query.numRowsAffected(); }
    inline int rows() const { return _query.isValid() ? _query.size() : 0; }
    inline bool transaction() { return PHIS_IF()->database().isOpen() ?
        PHIS_IF()->database().transaction() : false; }
    inline bool commit() { return PHIS_IF()->database().isOpen() ?
        PHIS_IF()->database().commit() : false; }
    inline bool rollback() { return PHIS_IF()->database().isOpen() ?
        PHIS_IF()->database().rollback() : false; }
    bool query( const QString& ) const;
    bool prepare( const QString &query ) const;
    bool exec() const;
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

inline bool PHISSqlObj::query( const QString &q ) const
{
    bool res=_query.exec( q );
    if ( !res ) {
        PHIS_ERROR( tr( "SQL query '%1' failed. DB error: [%2] %3" ).arg( q )
            .arg( _query.lastError().number() ).arg( _query.lastError().text() ) );
    }
    return res;
}

inline bool PHISSqlObj::prepare( const QString &query ) const
{
    bool res=_query.prepare( query );
    if ( !res ) {
        PHIS_ERROR( tr( "SQL prepare '%1' failed. DB error: [%2] %3" ).arg( query )
            .arg( _query.lastError().number() ).arg( _query.lastError().text() ) );
    }
    return res;
}

inline bool PHISSqlObj::exec() const
{
    bool res=_query.exec();
    if ( !res ) {
        PHIS_ERROR( tr( "SQL exec '%1' failed. DB error: [%2] %3" ).arg( _query.executedQuery() )
            .arg( _query.lastError().number() ).arg( _query.lastError().text() ) );
    }
    return res;
}

#endif // PHISSQL_H
