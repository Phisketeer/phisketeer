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

public:
    enum Type { Unknown=0, Text=1, Image=2, ImageBook=3, Audio=4, Video=5, Stream=6, Integer=7,
        Boolean=8, StringList=9, Color=10 };
    /** Source provider. @note internaly quint8 representation. */
    enum Source { Undefined=0, Static=1, Translated=2, Database=3, File=4, Url=5, Process=6, Library=7 };
    /** Data Options. For example: should we parse text. @note internaly quint8 representation. */
    enum Option { None=0x0, Parse=0x1, FileName=Parse, NoCache=0x2, DontSaveImage=0x4 };

#ifdef PHIDEBUG
    Q_DECLARE_FLAGS( Options, Option )
#else
    typedef quint8 Options;
#endif

    PHIData();
    virtual ~PHIData();
    PHIData( const PHIData& );
    PHIData& operator=( const PHIData& );

    virtual Type type() const=0;
    virtual inline Source source() const { return _source; }
    virtual inline void setSource( Source s ) { _source=s; }
    // inline QStringList languages() const { return _data.keys(); }
    inline Options options() const { return _options; }
    inline void setOptions( Options o ) { _options=o; }
    inline void setOption( Option o ) { _options |= o; }
    inline void removeOption( Option o ) { _options &= ~o; }
    inline bool unparsedStatic() const { return _source==Static && !(_options & Parse); }
    inline QList<QByteArray> keys() const { return _data.keys(); }
    inline void remove( const QByteArray &l ) { _data.remove( l ); }
    inline PHIVariantHash data() const { return _data; }

    inline QVariant variant( const QByteArray &l=_c ) const { return _data.value( l ); }
    inline QString libraryName() const { return _data.value( "#I" ).toString(); }
    inline void setLibraryName( const QString &s ) {
        _data.insert( _c, QByteArrayLiteral( "LIB" ) ); _data.insert( "#I", s );
    }
    inline QString processName() const { return _data.value( "#P" ).toString(); }
    inline void setProcessName( const QString &s ) {
        _data.insert( _c, QByteArrayLiteral( "EXE" ) ); _data.insert( "#P", s );
    }
    inline QString attributes() const { return _data.value( "#A" ).toString(); }
    inline void setAttributes( const QString &s ) { _data.insert( "#A", s ); }
    inline QString fileName() const { return _data.value( "#N" ).toString(); }
    inline void setFileName( const QString &s ) {
        if ( type()==Text ) _data.insert( _c, QByteArrayLiteral( "FIL" ) ); _data.insert( "#N", s );
    }
    inline QString sqlStatement() const { return _data.value( "#S" ).toString(); }
    inline void setSqlStatement( const QString &s ) {
        _data.insert( _c, QByteArrayLiteral( "SQL" ) ); _data.insert( "#S", s );
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
    inline void setUrl( const QString &s ) {
        _data.insert( _c, QByteArrayLiteral( "URL" ) ), _data.insert( "#U", s );
    }
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
PHIEXPORT QDataStream& operator<<( QDataStream&, const PHIData* );

class PHIEXPORT PHITextData : public PHIData
{
public:
    PHITextData( const QString &t=QString() );
    virtual ~PHITextData();
    inline virtual Type type() const { return Text; }
    inline QString text( const QByteArray &l=_c ) const { return _data.value( l ).toString(); }
    inline void setText( const QString &t, const QByteArray &l=_c ) { _data.insert( l, t ); }
};

PHIEXPORT QDataStream& operator>>( QDataStream&, PHITextData* );

/*
class PHIEXPORT PHIDateData : public PHIData
{
public:
    PHIDateData( const QDate &d=QDate() );
    virtual ~PHIDateData();
    inline virtual Type type() const { return Date; }
    inline QDate date( const QByteArray &l=_c ) const { return _data.value( l ).toDate(); }
    inline void setDate( const QDate &d, const QByteArray &l=_c ) { _data.insert( l, d ); }
};

PHIEXPORT QDataStream& operator>>( QDataStream&, PHIDateData* );
*/

class PHIEXPORT PHIImageData : public PHIData
{
public:
    PHIImageData( const QImage &im=QImage() );
    virtual ~PHIImageData();
    inline virtual Type type() const { return Image; }
    QImage image( const QByteArray &l=_c ) const;
    inline void setImage( const QImage &im, const QByteArray &l=_c ) { _data.insert( l, im ); }
};

PHIEXPORT QDataStream& operator>>( QDataStream&, PHIImageData* );

class PHIEXPORT PHIImageBookData : public PHIData
{
public:
    PHIImageBookData( const PHIImageHash &book=PHIImageHash() );
    virtual ~PHIImageBookData();
    inline virtual Type type() const { return ImageBook; }
    PHIImageHash imageBook( const QByteArray &l=_c ) const;
    inline void setImageBook( PHIImageHash &book, const QByteArray &l=_c )
        { QVariant v; v.setValue( book ); _data.insert( l, v ); }
};

PHIEXPORT QDataStream& operator>>( QDataStream&, PHIImageBookData* );

class PHIEXPORT PHIIntData : public PHIData
{
public:
    PHIIntData( qint32 i=0 );
    virtual ~PHIIntData();
    inline virtual Type type() const { return Integer; }
    inline qint32 integer( const QByteArray &l=_c ) const { return static_cast<qint32>(_data.value( l ).toInt()); }
    inline void setInteger( qint32 i, const QByteArray &l=_c ) { _data.insert( l, i ); }
    inline void setText( const QString &t, const QByteArray &l=_c ) { _data.insert( l, t ); }
    inline QString text( const QByteArray &l=_c ) const { return _data.value( l ).toString(); }
};

PHIEXPORT QDataStream& operator>>( QDataStream&, PHIIntData* );

class PHIEXPORT PHIBooleanData : public PHIData
{
public:
    PHIBooleanData( bool b=false );
    virtual ~PHIBooleanData();
    inline virtual Type type() const { return Boolean; }
    inline bool boolean( const QByteArray &l=_c ) const { return _data.value( l ).toBool(); }
    inline void setBoolean( bool b, const QByteArray &l=_c ) { _data.insert( l, b ); }
    inline void setText( const QString &t, const QByteArray &l=_c ) { _data.insert( l, t ); }
    inline QString text( const QByteArray &l=_c ) const { return _data.value( l ).toString(); }
};

PHIEXPORT QDataStream& operator>>( QDataStream&, PHIBooleanData* );

class PHIEXPORT PHIStringListData : public PHIData
{
public:
    PHIStringListData( const QStringList &l=QStringList() );
    virtual ~PHIStringListData();
    inline virtual Type type() const { return StringList; }
    inline QStringList stringList( const QByteArray &l=_c ) const { return _data.value( l ).toStringList(); }
    inline void setStringList( const QStringList &sl, const QByteArray &l=_c ) { _data.insert( l, sl ); }
};

PHIEXPORT QDataStream& operator>>( QDataStream&, PHIStringListData* );

class PHIEXPORT PHIColorData : public PHIData
{
public:
    PHIColorData( const QColor &c=QColor( Qt::black ) );
    virtual ~PHIColorData();
    inline virtual Type type() const { return Color; }
    inline QColor color( const QByteArray &l=_c ) const { return _data.value( l ).value<QColor>(); }
    inline void setColor( const QColor &c, const QByteArray &l=_c ) { _data.insert( l, c ); }
};

PHIEXPORT QDataStream& operator>>( QDataStream&, PHIColorData* );

#endif // PHIDATASOURCES_H
