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
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QSqlRecord>
#include "phissql.h"

PHISSqlObj::PHISSqlObj( const PHISInterface *interf )
    : PHISScriptObj( interf ), _query( interf->database() ), _engine( 0 )
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

QScriptValue PHISSqlObj::initObject( QScriptEngine *engine, const QString &key )
{
    _engine=engine; // needed for clone()
    return PHISScriptObj::initObject( engine, key );
}

QScriptValue PHISSqlObj::clone() const
{
    Q_ASSERT( _engine );
    PHISSqlObj *clone=new PHISSqlObj( PHIS_IF() );
    return _engine->newQObject( clone, QScriptEngine::ScriptOwnership,
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
