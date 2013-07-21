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
#include "phipagemenuentry.h"

PHIPageMenuEntry::PHIPageMenuEntry()
{
    qDebug( "PHIPageMenuEntry::PHIPageMenuEntry()" );
}

PHIPageMenuEntry::PHIPageMenuEntry( const QByteArray &id, const QByteArray &parent,
    const QImage &image, const QByteArray &text, Options options )
    : _id( id ), _parent( parent ), _text( text ), _img( image ), _options( options )
{
    qDebug( "PHIPageMenuEntry::PHIPageMenuEntry()" );
}

PHIPageMenuEntry::PHIPageMenuEntry( const PHIPageMenuEntry &e )
{
    _id=e._id;
    _parent=e._parent;
    _img=e._img;
    _text=e._text;
    _options=e._options;
}

PHIPageMenuEntry& PHIPageMenuEntry::operator=( const PHIPageMenuEntry &e )
{
    _id=e._id;
    _parent=e._parent;
    _img=e._img;
    _text=e._text;
    _options=e._options;
    return *this;
}

PHIPageMenuEntry::~PHIPageMenuEntry()
{
    qDebug( "PHIPageMenuEntry::~PHIPageMenuEntry()" );
}

QDataStream& operator<<( QDataStream &out, const PHIPageMenuEntry &m )
{
    out << m._id << m._parent << m._img << m._text << static_cast<quint8>(m._options);
    return out;
}

QDataStream& operator>>( QDataStream &in, PHIPageMenuEntry &m )
{
    quint8 opt;
    in >> m._id >> m._parent >> m._img >> m._text >> opt;
    m._options=static_cast<PHIPageMenuEntry::Options>(opt);
    return in;
}
