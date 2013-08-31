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
#ifndef PHIPAGEMENUENTRY_H
#define PHIPAGEMENUENTRY_H
#include "phi.h"

class PHITextData;
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

    PHIPageMenuEntry();
    PHIPageMenuEntry( const QByteArray &id, const QByteArray &parent, const QImage &image,
        const QByteArray &text, Options options, const PHITextData *data=0 );
    virtual ~PHIPageMenuEntry();
    PHIPageMenuEntry( const PHIPageMenuEntry& );
    PHIPageMenuEntry& operator=( const PHIPageMenuEntry& );
    bool operator==( const PHIPageMenuEntry &p );
    inline bool operator!=( const PHIPageMenuEntry &p ) { return !operator==(p); }

    inline QString id() const { return QString::fromUtf8( _id ); }
    inline QString parent() const { return QString::fromUtf8( _parent ); }
    inline QString text() const { return QString::fromUtf8( _text ); }
    inline Options options() const { return _options; }
    inline QImage image() const { return _img; }
    inline QPixmap pixmap() const { return QPixmap::fromImage( _img ); }
    inline PHITextData* textData() const { return _textData; }
    inline void setText( const QString &text ) { _text=text.toUtf8(); }

protected:
    QByteArray _id, _parent, _text;
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
