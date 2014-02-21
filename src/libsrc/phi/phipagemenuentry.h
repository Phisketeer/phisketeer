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
#ifndef PHIPAGEMENUENTRY_H
#define PHIPAGEMENUENTRY_H
#include "phidatasources.h"
#include "phi.h"

class QDataStream;

class PHIEXPORT PHIPageMenuEntry
{
    friend PHIEXPORT QDataStream& operator<<( QDataStream&, const PHIPageMenuEntry& );
    friend PHIEXPORT QDataStream& operator>>( QDataStream&, PHIPageMenuEntry& );

public:
    enum Option { None=0x0, SubMenu=0x1, Checkable=0x2, Checked=0x4, Disabled=0x8 };
#ifdef PHIDEBUG
    Q_DECLARE_FLAGS( Options, Option ) // quint8
#else
    typedef quint8 Options;
#endif

    PHIPageMenuEntry() : _textData( new PHITextData() ) {}
    PHIPageMenuEntry( const QByteArray &id, const QByteArray &parent, const QImage &image,
        Options options, const PHITextData *data=0 );
    virtual ~PHIPageMenuEntry() { delete _textData; }
    PHIPageMenuEntry( const PHIPageMenuEntry &e ) : _id( e._id ), _parent( e._parent ),
        _img( e._img ), _options( e._options ),
        _textData( new PHITextData( *e._textData ) ) {}
    PHIPageMenuEntry& operator=( const PHIPageMenuEntry& );
    bool operator==( const PHIPageMenuEntry &p );
    inline bool operator!=( const PHIPageMenuEntry &p ) { return !(operator==(p)); }

    inline QString id() const { return QString::fromUtf8( _id ); }
    inline QString parent() const { return QString::fromUtf8( _parent ); }
    inline QString text() const { return _textData->text(); }
    inline Options options() const { return _options; }
    inline QImage image() const { return _img; }
    inline QPixmap pixmap() const { return QPixmap::fromImage( _img ); }
    inline PHITextData* textData() const { return _textData; }
    inline void setText( const QByteArray &text ) { *_textData=PHITextData(); _textData->setText( text ); }

protected:
    QByteArray _id, _parent;
    QImage _img;
    Options _options;
    PHITextData *_textData;
};

#ifdef PHIDEBUG
Q_DECLARE_OPERATORS_FOR_FLAGS( PHIPageMenuEntry::Options )
#endif

PHIEXPORT QDataStream& operator<<( QDataStream&, const PHIPageMenuEntry* );
PHIEXPORT QDataStream& operator>>( QDataStream&, PHIPageMenuEntry* );

#endif // PHIPAGEMENUENTRY_H
