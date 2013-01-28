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
#include <QSqlRecord>
#include "phissql.h"

PHISSqlObj::PHISSqlObj( const PHISInterface *interf )
    : PHISScriptObj( interf ), _query( interf->database() )
{
    qDebug( "PHISSqlObj::PHISSqlObj()" );
    _query.setForwardOnly( true );
    QSqlDatabase db=interf->database();
    if ( !db.isOpen() ) {
        PHIS_ERROR( tr( "A page loads the 'sql' module, but the server is not "
            "connected. The script 'sql' property is not initialized properly. "
            "DB error: [%1] %2" ).arg( db.lastError().number() ).arg( db.lastError().text() ) );
    }
}

QScriptValue PHISSqlObj::clone() const
{
    QScriptEngine *engine=interface()->scriptEngine();
    Q_ASSERT( engine );
    PHISSqlObj *clone=new PHISSqlObj( interface() );
    return engine->newQObject( clone, QScriptEngine::ScriptOwnership,
        QScriptEngine::PreferExistingWrapperObject
        | QScriptEngine::ExcludeSuperClassMethods
        | QScriptEngine::ExcludeDeleteLater );
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
    if ( !res ) {
        PHIS_ERROR( tr( "SQL query '%1' failed. DB error: [%2] %3" ).arg( q )
            .arg( _query.lastError().number() ).arg( _query.lastError().text() ) );
    }
    return res;
}
