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
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PHI_H
#define PHI_H

#ifdef PHILIB
#define PHIEXPORT Q_DECL_EXPORT
#else
#define PHIEXPORT Q_DECL_IMPORT
#endif

#define PHI_DSV QDataStream::Qt_4_7
#define PHI_DSV2 QDataStream::Qt_5_1
#define PHI_SFV 3
#define PHI_MAGIC 0x5c34bb28
#define L1(s) QLatin1String(s)
#define SL(s) QStringLiteral(s)
#define BL(s) QByteArrayLiteral(s)

#include <QObject>
#include <QMetaType>
#include <QUrl>
#include <QAbstractSocket>
#include <QColor>
#include <QString>
#include <QHash>
#include <QGradientStops>
#include <QFont>
#include <QSize>
#include <QPoint>
#include <QFileInfo>
#include <QImage>
#include <QEasingCurve>

typedef QHash <QByteArray, QRect> PHIRectHash;
typedef QList <QByteArray> PHIByteArrayList;
typedef QHash <QByteArray, QVariant> PHIVariantHash;
typedef QHash <quint8, QColor> PHIColorHash;
typedef QHash <QByteArray, QImage> PHIImageHash;
typedef QHash <qint8, QVariant> PHIKeyHash;
typedef quint16 PHIRC;
typedef quint16 PHIWID;

Q_DECLARE_METATYPE( PHIRectHash )
Q_DECLARE_METATYPE( PHIByteArrayList )
Q_DECLARE_METATYPE( PHIVariantHash )
Q_DECLARE_METATYPE( PHIColorHash )
Q_DECLARE_METATYPE( PHIImageHash )
Q_DECLARE_METATYPE( PHIKeyHash )
Q_DECLARE_METATYPE( QGradientStops )

class PHIEXPORT PHI
{
    Q_DISABLE_COPY( PHI )
    PHI() {}

public:
    enum LogFilter { None=0x00, Trace=0x01, Error=0x02, Warning=0x04, Critical=0x08,
        Debug=0x10, User=0x20 };
    //enum Alignment { AlignLeftVCenter=0, AlignLeftTop=1, AlignLeftBottom=2, AlignRightVCenter=3, AlignRightTop=4,
    //    AlignRightBottom=5, AlignTopHCenter=6, AlignBottomHCenter=7, AlignJustify=8, AlignCenter=9,
    //    AlignLeft=10, AlignRight=11, AlignTop=12, AlignBottom=13 }; //quint8 in DataStream
    enum ContentData { CDAll=0, CDUpdate=1, CDRequest=2, CDMessage=3, CDTemplatePage=4, CDImage=5,
        CDStream=6 }; // quint8 in DataStream
    enum Origin { Custom=0, TopLeft=1, TopMid=2, TopRight=3, MidLeft=4, MidMid=5,
        Center=MidMid, MidRight=6, BottomLeft=7, BottomMid=8, BottomRight=9, Invalid=10 };

    static Qt::Alignment toQtAlignment( quint8 );
    static PHIRC socketError( QAbstractSocket::SocketError err );
    static QString tag( const QString &tag, const QString &msg ); // fast extractor <tag>msg</tag>
    static QStringList properties( const QObject* );
    static const QString &libVersion();

    //static void hsvToHsl( qreal h, qreal s, qreal v, qreal *hh, qreal *ss, qreal *ll );
    //static void hslToHsv( qreal hh, qreal ss, qreal ll, qreal *h, qreal *s, qreal *v );
    //static QColor percentColor( const QColor &c, qreal fac );
    static const QByteArray& nl();
    static const QByteArray& dtFormat();
    static const QString& dtFormatString();
    static QByteArray isoDateFormat();
    static QByteArray emailRegExp();
    static const QByteArray& phoneNumberRegExp();
    static const QByteArray& phiMimeType();
    static QByteArray mimeTypeForFile( const QFileInfo &info );
    static Qt::CursorShape toCursorShape( const QByteArray &s );
    static bool isUpToDate( const QString &localV, const QString &serverV );
    static QString toLocale( const QString &lang );
    static const QString& defaultString(); // returns "default"
    static QUrl createUrlForLink( const QUrl &ref, const QString &link );
    static QString createPngUuid();
    static QImage reflectedImage( const QImage &img, qreal off, qreal size );
    static QImage colorizedImage( const QImage &img, const QColor &c, qreal strength );
    static QImage dropShadowedImage( const QImage &img, const QColor &color,
        qreal radius, qreal xOff, qreal yOff );
    static QImage bluredImage( const QImage &img, qreal radius );
    static QPixmap colorizedPixmap( const QPixmap &pix, const QColor &c=QColor( Qt::black ), qreal strength=1. );
    static QPixmap dropShadowedPixmap( const QPixmap &src, const QPointF &off=QPointF( 1., 2. ),
        const QColor &c=QColor( 0, 0, 0, 220 ), qreal radius=3. );

    static void extractNumbers( const QByteArray &s, int &value, int &min, int &max, int &step );
    static void extractRealNumbers( const QByteArray &s, qreal &value, qreal &min, qreal &max,
        qreal &step, int &decimals );
    static qreal adjustedFontSize( qreal s);

    static inline const QString& idValidator() { static QString v=QStringLiteral( "[A-Za-z][-_A-Za-z0-9]*" ); return v; }
    static inline QSize patternIconSize() { return QSize( 48, 12 ); }
    static inline QSize colorIconSize() { return QSize( 14, 14 ); }
    static inline qreal defaultSpacing() { return 6.; }
    static inline qreal fontSizeFactor() { return static_cast<qreal>(1.33333333333); }
    static inline int defaultFontSize() { return 10; }
    static inline const QFont& invalidFont() { static QFont f( QStringLiteral( "phi" ), 3 ); return f; }
    static inline const QFont& defaultFont() { static QFont f( QStringLiteral( "Arial" ), 10 ); return f; }
    static inline int defaultDragDistance() { return 5; }
    static inline QSize defaultPageSize() { return QSize( 1000, 750 ); }
    static inline QPoint defaultHotSpot() { return QPoint( 5, 5 ); }
    static inline qint16 maxZIndex() { return static_cast<qint16>(30000); }
    static inline qreal gradToRad( qreal g ) { return g*static_cast<qreal>(0.01745329251994); }
    static inline qreal radToGrad( qreal r ) { return r*static_cast<qreal>(57.29577951309314); }
    static inline int defaultToolBarHeight() { return 24; }

    // move to PHIBaseItem
    static inline QString defaultEasingCurve() { return QStringLiteral( "easeOutQuad" ); }
    static inline quint8 defaultEasingCurveType() {
        return static_cast<quint8>(toEasingCurveType(defaultEasingCurve())); }
    static inline QString toEasingCurveString( quint8 ease ) {
        return QString::fromLatin1( toEasingCurveByteArray( ease ) ); }
    static QEasingCurve::Type toEasingCurveType( const QString &name );
    static QByteArray toEasingCurveByteArray( quint8 ease );
    static QStringList availableEasingCurves();
    static void getItemCheckData( QString &data, QString &opt, bool &isChecked );

private:
    static const char* _phiDT;
    static const char* _phiDate;
    static const char* _emailRegExp;
    static const char* _phoneNumberRegExp;
    static const char* _phiMimeType;
};

inline QUrl PHI::createUrlForLink( const QUrl &ref, const QString &l )
{
    return ref.resolved( QUrl( l ) );
}

inline const QString& PHI::defaultString()
{
    static QString def=SL( "default" );
    return def;
}

inline const QByteArray& PHI::dtFormat()
{
    static QByteArray dt=QByteArray::fromRawData( _phiDT, qstrlen( _phiDT ) );
    return dt;
}

inline const QString& PHI::dtFormatString()
{
    static QString dt=QString::fromLatin1( dtFormat() );
    return dt;
}

inline QByteArray PHI::isoDateFormat()
{
    static QByteArray isoDate=QByteArray::fromRawData( _phiDate, qstrlen( _phiDate ) );
    return isoDate;
}

inline QByteArray PHI::emailRegExp() {
    static QByteArray email=QByteArray::fromRawData( _emailRegExp, qstrlen( _emailRegExp ) );
    return email;
}

inline const QByteArray& PHI::phoneNumberRegExp()
{
    static QByteArray phone=QByteArray::fromRawData( _phoneNumberRegExp, qstrlen( _phoneNumberRegExp ) );
    return phone;
}

inline const QByteArray& PHI::phiMimeType()
{
    static QByteArray mime=QByteArray::fromRawData( _phiMimeType, qstrlen( _phiMimeType ) );
    return mime;
}

inline const QByteArray& PHI::nl()
{
#ifdef Q_OS_WIN
    static QByteArray nl=QByteArray::fromRawData( "\r\n", 2 );
#else
    static QByteArray nl=QByteArray::fromRawData( "\n", 1 );
#endif
    return nl;
}

inline qreal PHI::adjustedFontSize( qreal s )
{
#ifdef Q_OS_MAC
    return s*fontSizeFactor();
#else
    return s;
#endif
}

#endif /* PHI_H */
