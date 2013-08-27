/*
#    Copyright (C) 2008  Trolltech AS
#    Copyright (C) 2012  Digia Plc and/or its subsidiary(-ies).
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
#
#    Note: some parts are based on code provided by the Qt source.
*/
#include <QGuiApplication>
#include <QDataStream>
#include "phipalette.h"

PHIPalette::PHIPalette()
{
    _pal=QGuiApplication::palette();
    _pal.setColor( QPalette::Window, Qt::transparent );
    _userColors.insert( Custom, Qt::black );
    _userColors.insert( Black, Qt::black );
    _userColors.insert( White, Qt::white );
    _userColors.insert( Error, QColor( 180, 0, 0 ) );
    _userColors.insert( ErrorBackground, QColor( 255, 250, 230 ) );
    createPercentColors( User1_100, QColor( 255, 0, 0 ) );
    createPercentColors( User2_100, QColor( 0, 255, 0 ) );
    createPercentColors( User3_100, QColor( 0, 0, 255 ) );
    createPercentColors( User4_100, QColor( 255, 255, 0 ) );
    createPercentColors( User5_100, QColor( 0, 255, 255 ) );
    createPercentColors( User6_100, QColor( 255, 0, 255 ) );
}

PHIPalette::PHIPalette( const PHIPalette &p )
{
    _pal=p._pal;
    _userColors=p._userColors;
}

PHIPalette& PHIPalette::operator=( const PHIPalette &p )
{
    _pal=p._pal;
    _userColors=p._userColors;
    return *this;
}

bool PHIPalette::operator==( const PHIPalette &p )
{
    if ( _pal!=p._pal ) return false;
    if ( _userColors!=p._userColors ) return false;
    return true;
}

void PHIPalette::createPercentColors( ColorRole role, const QColor &col )
{
    switch ( role ) {
    case User1_100:
    case User2_100:
    case User3_100:
    case User4_100:
    case User5_100:
    case User6_100:
        break;
    default:
        qWarning( "setting a derived color is not permitted" );
        return;
    }
    _userColors.insert( role, col );
    quint8 num=static_cast<quint8>(role);
    int f=120;
    for ( quint8 i=num-1; i>num-5; i-- ) {
        QColor l=col.lighter( f );
        _userColors.insert( i, l );
        f+=20;
    }
    f=80;
    for( quint8 i=num+1; i<num+5; i++ ) {
        QColor d=col.lighter( f );
        _userColors.insert( i, d );
        f-=20;
    }
}

QColor PHIPalette::color( ColorRole role ) const
{
    quint8 num=static_cast<quint8>(role);
    if ( num<17 ) return _pal.color( static_cast<QPalette::ColorRole>(num) );
    return _userColors.value( num, QColor() );
}

void PHIPalette::setPalette( const QPalette &pal )
{
    _pal=pal;
    _pal.setColor( QPalette::Window, Qt::transparent );
}

void PHIPalette::setColor( ColorRole role, const QColor &col )
{
    quint8 num=static_cast<quint8>(role);
    if ( num<17 ) {
        _pal.setColor( static_cast<QPalette::ColorRole>(num), col );
        return;
    }
    if ( role==Error || role==ErrorBackground ) {
        _userColors.insert( role, col );
        return;
    }
    if ( role==Black || role==White || role==Custom || role==NoRole ) {
        return;
    }
    createPercentColors( role, col );
}

QDataStream& operator <<( QDataStream &out, const PHIPalette &p )
{
    for ( quint8 i=0; i<17; i++ ) out << p.color( static_cast<PHIPalette::ColorRole>(i) );
    out << p.color( PHIPalette::User1_100 )
        << p.color( PHIPalette::User2_100 ) << p.color( PHIPalette::User3_100 )
        << p.color( PHIPalette::User4_100 ) << p.color( PHIPalette::User5_100 )
        << p.color( PHIPalette::User6_100 ) << p.color( PHIPalette::Error )
        << p.color( PHIPalette::ErrorBackground );
    return out;
}

QDataStream& operator >>( QDataStream &in, PHIPalette &p )
{
    QColor c;
    for ( quint8 i=0; i<18; i++ ) {
        in >> c;
        p.setColor( static_cast<PHIPalette::ColorRole>(i), c );
    }
    in >> c; p.setColor( PHIPalette::User1_100, c );
    in >> c; p.setColor( PHIPalette::User2_100, c );
    in >> c; p.setColor( PHIPalette::User3_100, c );
    in >> c; p.setColor( PHIPalette::User4_100, c );
    in >> c; p.setColor( PHIPalette::User5_100, c );
    in >> c; p.setColor( PHIPalette::User6_100, c );
    in >> c; p.setColor( PHIPalette::Error, c );
    in >> c; p.setColor( PHIPalette::ErrorBackground, c );
    return in;
}
