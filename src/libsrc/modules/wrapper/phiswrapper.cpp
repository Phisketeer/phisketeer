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
#include "phiswrapper.h"
#include "phibasepage.h"
#include "phi.h"

static QScriptValue getItemFunc( QScriptContext *ctx, QScriptEngine *engine )
{
    QScriptValue id=ctx->argument( 0 );
    if ( id.isString() ) {
        PHIBasePage *page=qobject_cast<PHIBasePage*>(engine->parent());
        Q_ASSERT( page );
        PHIBaseItem *it=page->getElementById( id.toString() );
        if ( !it ) return engine->undefinedValue();
        return engine->newQObject( new PHISScriptItem( engine, it ), QScriptEngine::ScriptOwnership,
            QScriptEngine::PreferExistingWrapperObject |
            QScriptEngine::ExcludeSuperClassContents | QScriptEngine::ExcludeDeleteLater );
    } else if ( id.isObject() ) return id;
    return engine->undefinedValue();
}

QScriptValue PHISWrapperObj::initObject( QScriptEngine *engine, const QString &key )
{
    Q_UNUSED( key )
    QScriptValue sv=engine->newFunction( getItemFunc, 1 );
    engine->globalObject().setProperty( QStringLiteral( "$" ), sv );
    return sv;
}

QScriptValue PHISScriptItem::checked( const QScriptValue &v, const QScriptValue &c )
{
    qDebug() << "checked " << v.toString() << c.toString();
    if ( !v.isValid() && !c.isValid() ) return QScriptValue( _it->checked() );
    if ( v.isBool() ) { _it->setChecked( v.toBool() ); return self(); }
    if ( !v.isString() ) return self();
    if ( _it->wid()!=43 ) return self(); // checklist
    const QString value=_it->property( "value" ).toString();
    const QString delimiter=_it->property( "delimiter" ).toString();
    QStringList list=value.split( delimiter, QString::SkipEmptyParts );
    if ( list.isEmpty() ) return self();
    QStringList destList;
    destList.append( list.takeFirst() ); // take header
    QString tmp, opt;
    bool isChecked, found=false;
    foreach ( tmp, list ) {
        destList.append( tmp );
        if ( found ) continue;
        QStringList cols=tmp.split( QLatin1Char( '|' ) );
        tmp=cols.at( 0 );
        PHI::getItemCheckData( tmp, opt, isChecked );
        if ( opt==v.toString() ) {
            if ( !c.isValid() ) return QScriptValue( isChecked );
            QLatin1Char l( '[' );
            QLatin1Char r( ']' );
            destList.removeLast();
            tmp=tmp+l+opt+r;
            if ( c.toBool() ) tmp.append( QLatin1String( "[1]" ) );
            cols.removeFirst();
            cols.prepend( tmp );
            destList.append( cols.join( QLatin1Char( '|' ) ) );
            found=true;
        }
    }
    _it->setProperty( "value", destList.join( delimiter ) );
    return self();
}

QScriptValue PHISScriptItem::selected( const QString &v, const QScriptValue &s )
{
    qDebug() << "selected " << v << s.toString();
    QString val, opt;
    bool isSelected;
    const QString value=_it->property( "value" ).toString();
    const QString delimiter=_it->property( "delimiter" ).toString();
    if ( v.isNull() && !s.isValid() ) {
        QScriptValue arr=engine()->newArray();
        quint32 l=0;
        foreach ( val, value.split( delimiter, QString::SkipEmptyParts ) ) {
            PHI::getItemCheckData( val, opt, isSelected );
            if ( isSelected ) arr.setProperty( l++, opt );
        }
        return arr;
    }
    if ( !s.isValid() ) {
        foreach( val, value.split( delimiter, QString::SkipEmptyParts ) ) {
            PHI::getItemCheckData( val, opt, isSelected );
            if ( opt==v ) return QScriptValue( isSelected );
        }
    }
    QStringList destList;
    bool found=false;
    foreach ( val, value.split( delimiter, QString::SkipEmptyParts ) ) {
        destList.append( val );
        if ( found ) continue;
        PHI::getItemCheckData( val, opt, isSelected );
        if ( opt==v ) {
            destList.removeLast();
            val=val+QLatin1Char( '[' )+opt+QLatin1Char( ']' );
            if ( s.toBool() ) val+=QLatin1String( "[1]" );
            destList.append( val );
            found=true;
        }
    }
    _it->setProperty( "value", destList.join( delimiter ) );
    return self();
}

QScriptValue PHISScriptItem::pos( const QScriptValue &l, const QScriptValue &t )
{
    if ( l.isObject() && !t.isValid() ) {
        _it->setX( l.property( QStringLiteral( "left" ) ).toInt32() );
        _it->setY( l.property( QStringLiteral( "top" ) ).toInt32() );
        return self();
    }
    if ( l.isNumber() && t.isNumber() ) {
        _it->setX( l.toInt32() );
        _it->setY( t.toInt32() );
        return self();
    }
    QScriptValue obj=engine()->newObject();
    obj.setProperty( QStringLiteral( "left" ), _it->realX() );
    obj.setProperty( QStringLiteral( "top" ), _it->realY() );
    return obj;
}
