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
#include <QDataStream>
#include "phipagemenuentry.h"
#include "phidatasources.h"

PHIPageMenuEntry::PHIPageMenuEntry()
{
    qDebug( "PHIPageMenuEntry::PHIPageMenuEntry()" );
    _textData=new PHITextData();
}

PHIPageMenuEntry::PHIPageMenuEntry( const QByteArray &id, const QByteArray &parent,
    const QImage &image, const QByteArray &text, Options options, const PHITextData *data )
    : _id( id ), _parent( parent ), _text( text ), _img( image ), _options( options )
{
    qDebug( "PHIPageMenuEntry::PHIPageMenuEntry()" );
    _textData=new PHITextData();
    if ( data ) *_textData=*data;
}

PHIPageMenuEntry::PHIPageMenuEntry( const PHIPageMenuEntry &e )
{
    _id=e._id;
    _parent=e._parent;
    _img=e._img;
    _text=e._text;
    _options=e._options;
    _textData=new PHITextData();
    *_textData=*(e._textData);
}

PHIPageMenuEntry& PHIPageMenuEntry::operator=( const PHIPageMenuEntry &e )
{
    _id=e._id;
    _parent=e._parent;
    _img=e._img;
    _text=e._text;
    _options=e._options;
    *_textData=*(e._textData);
    return *this;
}

bool PHIPageMenuEntry::operator==( const PHIPageMenuEntry &p )
{
    if ( _id!=p._id ) return false;
    if ( _parent!=p._parent ) return false;
    if ( _img!=p._img ) return false;
    // if ( _text!=p._text ) return false;
    Q_ASSERT( _text.isEmpty() && p._text.isEmpty() );
    if ( _options!=p._options ) return false;
    if ( *_textData!=*(p._textData) ) return false;
    return true;
}

PHIPageMenuEntry::~PHIPageMenuEntry()
{
    delete _textData;
    qDebug( "PHIPageMenuEntry::~PHIPageMenuEntry()" );
}

void PHIPageMenuEntry::load( QDataStream &in, int version )
{
    Q_UNUSED( version )
    quint8 opt;
    in >> _id >> _parent >> _img >> _textData >> opt;
    _options=static_cast<PHIPageMenuEntry::Options>(opt);
}

void PHIPageMenuEntry::save( QDataStream &out, int version )
{
    Q_UNUSED( version )
    out << _id << _parent << _img << _textData << static_cast<quint8>(_options);
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
