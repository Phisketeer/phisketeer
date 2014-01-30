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
#ifndef PHIDATASOURCES_H
#define PHIDATASOURCES_H

#include <QDataStream>
#include <QVariant>
#include <QImage>
#include <QHash>
#include <QFlags>
#include <QStringList>
#include <QColor>
#include <QDate>
#include "phi.h"

class PHIEXPORT PHIData
{
friend PHIEXPORT QDataStream& operator>>( QDataStream &in, PHIData *&phiData );
friend PHIEXPORT QDataStream& operator<<( QDataStream &out, PHIData *phiData );

public:
    enum Type { Unknown=0, Text=1, Image=2, ImageBook=3, Audio=4, Video=5, Stream=6, Integer=7,
        Boolean=8, StringList=9 }; //quint8
    enum Source { Undefined=0, Static=1, Translated=2, Database=3, File=4, Url=5, Process=6, Library=7 }; // quint8
    enum Option { None=0x0, Parse=0x1, FileName=Parse, NoCache=0x2, DontSaveImage=0x4,
        /* used in server: */ TmpObjCreated=0x8, UseFilePath=0x10  }; // quint8

#ifdef PHIDEBUG
    Q_DECLARE_FLAGS( Options, Option )
#else
    typedef quint8 Options;
#endif

    PHIData();
    virtual ~PHIData(){}
    PHIData( const PHIData& );
    PHIData& operator=( const PHIData& );
    bool operator==( const PHIData& );
    inline bool operator!=( const PHIData &d ) { return ! operator==(d); }
    virtual Type type() const=0;
    virtual inline Source source() const { return _source; }
    virtual inline void setSource( Source s ) { _source=s; }
    virtual void squeeze();
    inline Options options() const { return _options; }
    inline void setOptions( Options o ) { _options=o; }
    inline void setOption( Option o ) { _options |= o; }
    inline void removeOption( Option o ) { _options &= ~o; }
    inline bool isStatic() const { return _source==Static; }
    inline bool isTranslated() const { return _source==Translated; }
    inline bool isUnparsedStatic() const { return _source==Static && !(_options & Parse); }
    inline bool isUnparsedTranslated() const { return _source==Translated && !(_options & Parse); }
    inline PHIByteArrayList keys() const { return _data.keys(); }
    inline PHIByteArrayList langs() const {
        PHIByteArrayList list;
        QByteArray l;
        foreach ( l, _data.keys() ) if ( !l.startsWith( '#' ) ) list.append( l );
        return list;
    }
    inline void remove( const QByteArray &l ) { _data.remove( l ); }
    inline PHIVariantHash data() const { return _data; }

    inline QVariant variant( const QByteArray &l=_c ) const { return _data.value( l ); }
    inline QString libraryName() const { return _data.value( "#I" ).toString(); }
    inline void setLibraryName( const QString &s ) {
        _data.insert( _c, QByteArray( "LIB" ) ); _data.insert( "#I", s );
    }
    inline QString processName() const { return _data.value( "#P" ).toString(); }
    inline void setProcessName( const QString &s ) {
        _data.insert( _c, QByteArray( "EXE" ) ); _data.insert( "#P", s );
    }
    inline QString attributes() const { return _data.value( "#A" ).toString(); }
    inline void setAttributes( const QString &s ) { _data.insert( "#A", s ); }
    inline QString fileName() const { return _data.value( "#N" ).toString(); }
    inline void setFileName( const QString &s ) {
        if ( type()==Text ) _data.insert( _c, QByteArray( "FIL" ) ); _data.insert( "#N", s );
    }
    inline QString sqlStatement() const { return _data.value( "#S" ).toString(); }
    inline void setSqlStatement( const QString &s ) {
        _data.insert( _c, QByteArray( "SQL" ) ); _data.insert( "#S", s );
    }
    inline QString templateText() const { return _data.value( "#T" ).toString(); }
    inline void setTemplateText( const QString &s ) { _data.insert( "#T", s ); }
    inline QString textCodec() const { return _data.value( "#C" ).toString(); }
    inline void setTextCodec( const QString &s ) { _data.insert( "#C", s ); }
    inline QString header() const { return _data.value( "#H" ).toString(); }
    inline void setHeader( const QString &s ) { _data.insert( "#H", s ); }
    inline QString footer() const { return _data.value( "#F" ).toString(); }
    inline void setFooter( const QString &s ) { _data.insert( "#F", s ); }
    inline QString delimiter() const { return _data.value( "#D" ).toString(); }
    inline void setDelimiter( const QString &s ) { _data.insert( "#D", s ); }
    inline QString url() const { return _data.value( "#U" ).toString(); }
    inline void setUrl( const QString &s ) { _data.insert( _c, QByteArray( "URL" ) ), _data.insert( "#U", s ); }
    inline QString value() const { return _data.value( "#V" ).toString(); }
    inline void setValue( const QString &s ) { _data.insert( "#V", s ); }
    inline QString label() const { return _data.value( "#L" ).toString(); }
    inline void setLabel( const QString &s ) { _data.insert( "#L", s ); }
    inline QString buddy() const { return _data.value( "#B" ).toString(); }
    inline void setBuddy( const QString &s ) { _data.insert( "#B", s ); }
    inline QString shortCut() const { return _data.value( "#O" ).toString(); }
    inline void setShortCut( const QString &s ) { _data.insert( "#O", s ); }
    inline QStringList fileNames() const { return _data.value( "#Y" ).toStringList(); }
    inline void setFileNames( const QStringList &l ) { _data.insert( "#Y", l ); }
    static QDataStream& readData( QDataStream &in, PHIData *phiData );
    static const QByteArray c() { return _c; }

protected:
    Source _source;
    Options _options;
    PHIVariantHash _data;
    static const QByteArray _c;
};

#ifdef PHIDEBUG
    Q_DECLARE_OPERATORS_FOR_FLAGS( PHIData::Options )
#endif

PHIEXPORT QDataStream& operator>>( QDataStream&, PHIData*& );
PHIEXPORT QDataStream& operator<<( QDataStream&, PHIData* );

class PHIEXPORT PHITextData : public PHIData
{
public:
    explicit PHITextData( const QString &t=QString() );
    virtual ~PHITextData(){}
    inline virtual Type type() const { return Text; }
    inline void setText( const QString &t, const QByteArray &l=_c ) { if ( t.isEmpty() ) _data.remove( l ); else _data.insert( l, t.toUtf8() ); }
    inline void setText( const QByteArray &a, const QByteArray &l=_c ) { if ( a.isEmpty() ) _data.remove( l ); else _data.insert( l, a ); }
    inline QString text( const QByteArray &l=_c ) const { return _data.value( l ).type()==12 ?
        QString::fromUtf8( _data.value( l ).toByteArray() ) : _data.value( l ).toString(); }
};

PHIEXPORT QDataStream& operator>>( QDataStream&, PHITextData* );
Q_DECLARE_METATYPE( PHITextData )

class PHIEXPORT PHIImageData : public PHIData
{
public:
    explicit PHIImageData( const QImage &im=QImage() );
    virtual ~PHIImageData(){}
    inline virtual Type type() const { return Image; }
    QImage image( const QByteArray &l=_c ) const;
    inline void setImage( const QImage &im, const QByteArray &l=_c )
        { if ( im.isNull() ) _data.remove( l ); else _data.insert( l, im ); }
    inline void setImageId( const QByteArray &id, const QByteArray &l=_c )
        { _data.insert( l, id ); _options |= TmpObjCreated; }
    inline QByteArray imageId( const QByteArray &l=_c )
        { return _data.value( l ).toByteArray(); }
};

PHIEXPORT QDataStream& operator>>( QDataStream&, PHIImageData* );
Q_DECLARE_METATYPE( PHIImageData )

class PHIEXPORT PHIImageBookData : public PHIData
{
public:
    explicit PHIImageBookData( const PHIImageHash &book=PHIImageHash() );
    virtual ~PHIImageBookData(){}
    inline virtual Type type() const { return ImageBook; }
    PHIImageHash imageBook( const QByteArray &l=_c ) const;
    inline void setImageBook( const PHIImageHash &book, const QByteArray &l=_c )
        { QVariant v; v.setValue( book ); if ( book.isEmpty() ) _data.remove( l ); else _data.insert( l, v ); }
    inline void setImageIds( const PHIByteArrayList &list, const QByteArray &l=_c )
        { QVariant v; v.setValue( list ); _data.insert( l, v ); _options |= TmpObjCreated; }
    inline PHIByteArrayList imageIds( const QByteArray &l=_c )
        { return _data.value( l ).value<PHIByteArrayList>(); }
};

PHIEXPORT QDataStream& operator>>( QDataStream&, PHIImageBookData* );
Q_DECLARE_METATYPE( PHIImageBookData )

class PHIEXPORT PHIIntData : public PHIData
{
public:
    explicit PHIIntData( qint32 i=0 );
    virtual ~PHIIntData(){}
    inline virtual Type type() const { return Integer; }
    inline qint32 integer( const QByteArray &l=_c ) const { return static_cast<qint32>(_data.value( l, 0 ).toInt()); }
    inline void setInteger( qint32 i, const QByteArray &l=_c ) { if ( i ) _data.insert( l, i ); else _data.remove( l ); }
    inline void setText( const QString &t, const QByteArray &l=_c ) { if ( t.isEmpty() ) _data.remove( l ); else _data.insert( l, t ); }
    inline QString text( const QByteArray &l=_c ) const { return _data.value( l ).toString(); }
};

PHIEXPORT QDataStream& operator>>( QDataStream&, PHIIntData* );
Q_DECLARE_METATYPE( PHIIntData )

class PHIEXPORT PHIBooleanData : public PHIData
{
public:
    explicit PHIBooleanData( bool b=false );
    virtual ~PHIBooleanData(){}
    inline virtual Type type() const { return Boolean; }
    inline bool boolean( const QByteArray &l=_c ) const { return _data.value( l, false ).toBool(); }
    inline void setBoolean( bool b, const QByteArray &l=_c ) { if ( b ) _data.insert( l, b ); else _data.remove( l ); }
    inline void setText( const QString &t, const QByteArray &l=_c ) { if ( t.isEmpty() ) _data.remove( l ); else _data.insert( l, t ); }
    inline QString text( const QByteArray &l=_c ) const { return _data.value( l ).toString(); }
};

PHIEXPORT QDataStream& operator>>( QDataStream&, PHIBooleanData* );
Q_DECLARE_METATYPE( PHIBooleanData )

class PHIEXPORT PHIStringListData : public PHIData
{
public:
    explicit PHIStringListData( const QStringList &l=QStringList() );
    virtual ~PHIStringListData(){}
    inline virtual Type type() const { return StringList; }
    inline QStringList stringList( const QByteArray &l=_c ) const { return _data.value( l ).toStringList(); }
    inline void setStringList( const QStringList &sl, const QByteArray &l=_c ) { _data.insert( l, sl ); }
};

PHIEXPORT QDataStream& operator>>( QDataStream&, PHIStringListData* );
Q_DECLARE_METATYPE( PHIStringListData )

inline PHIData::PHIData() : _source( PHIData::Static ), _options( PHIData::None )
{
}

inline PHIData::PHIData( const PHIData &d )
    : _source( d._source ), _options( d._options ), _data( d._data )
{
}

inline PHIData& PHIData::operator=( const PHIData &d )
{
    _source=d._source;
    _options=d._options;
    _data=d._data;
    return *this;
}

inline QDataStream& PHIData::readData( QDataStream &in, PHIData *phiData )
{
    quint8 type, source, options;
    in >> type >> source >> options;
    Q_ASSERT( static_cast<PHIData::Type>(type)==phiData->type() );
    phiData->setSource( static_cast<PHIData::Source>(source) );
    phiData->setOptions( static_cast<PHIData::Options>(options) );
    in >> phiData->_data;
    phiData->squeeze();
    return in;
}

inline bool PHIData::operator==( const PHIData &d )
{
    if ( type()!=d.type() ) return false;
    if ( _source!=d._source ) return false;
    if ( _options!=d._options ) return false;
    if ( _data!=d._data ) return false;
    return true;
}

inline PHITextData::PHITextData( const QString &t )
{
    if ( !t.isEmpty() ) _data.insert( _c, t.toUtf8() );
}

inline QDataStream& operator>>( QDataStream &in, PHITextData *phiData )
{
    return PHIData::readData( in, phiData );
}

inline PHIImageData::PHIImageData( const QImage &im )
{
    _data.insert( _c, im );
}

inline QImage PHIImageData::image( const QByteArray &l ) const
{
    QVariant v=_data.value( l );
    if ( v.isValid() && v.canConvert<QImage>() ) return v.value<QImage>();
    return QImage();
}

inline QDataStream& operator>>( QDataStream &in, PHIImageData *phiData )
{
    return PHIData::readData( in, phiData );
}

inline PHIImageBookData::PHIImageBookData( const PHIImageHash &imgHash )
{
    QVariant v;
    v.setValue( imgHash );
    _data.insert( _c, v );
}

inline PHIImageHash PHIImageBookData::imageBook( const QByteArray &l ) const
{
    QVariant v=_data.value( l );
    PHIImageHash imgHash;
    if ( v.isValid() && v.canConvert<PHIImageHash>() ) imgHash=v.value<PHIImageHash>();
    return imgHash;
}

inline QDataStream& operator>>( QDataStream &in, PHIImageBookData *phiData )
{
    return PHIData::readData( in, phiData );
}

inline PHIIntData::PHIIntData( qint32 i )
{
    _data.insert( _c, i );
}

inline QDataStream& operator>>( QDataStream &in, PHIIntData *phiData )
{
    return PHIData::readData( in, phiData );
}

inline PHIBooleanData::PHIBooleanData( bool b )
{
    if ( b ) _data.insert( _c, b );
}

inline QDataStream& operator>>( QDataStream &in, PHIBooleanData *phiData )
{
    return PHIData::readData( in, phiData );
}

inline PHIStringListData::PHIStringListData( const QStringList &list )
{
    _data.insert( _c, list );
}

inline QDataStream& operator>>( QDataStream &in, PHIStringListData *phiData )
{
    return PHIData::readData( in, phiData );
}

#endif // PHIDATASOURCES_H
