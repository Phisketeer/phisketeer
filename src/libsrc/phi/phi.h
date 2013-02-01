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
#define PHI_SFV 2
#define PHI_PSV 2
#define PHI_MAGIC 0x5c34bb28

#define PHI_LOG_FILTER_NONE     0x00
#define PHI_LOG_FILTER_TRACE    0x01
#define PHI_LOG_FILTER_ERROR    0x02
#define PHI_LOG_FILTER_WARN     0x04
#define PHI_LOG_FILTER_CRIT     0x08
#define PHI_LOG_FILTER_DEBUG    0x10
#define PHI_LOG_FILTER_USER     0x20

#include <QAbstractSocket>
#include <QObject>
#include <QStringList>
#include <QByteArray>
#include <QSize>
#include <QSizeF>
#include <QDir>
#include <QUrl>
#include <QRect>
#include <QColor>
#include <QHash>
#include <QMetaType>
#include <QVariant>
#include <QImage>
#include <QGradient>
#include <QDateTime>
#include <QGuiApplication>
#include <QFont>
#include <QEasingCurve>
#include <QStandardPaths>

class QSettings;
class QGraphicsItem;
class PHIEffect;

typedef QHash <QByteArray, QRect> PHIRectHash;
typedef QList <QByteArray> PHIByteArrayList;
typedef QHash <QByteArray, QVariant> PHIVariantHash;
typedef QHash <quint8, QColor> PHIColorHash;
typedef QHash <QByteArray, QImage> PHIImageHash;
typedef quint16 PHIRC;

Q_DECLARE_METATYPE( PHIRectHash )
Q_DECLARE_METATYPE( PHIByteArrayList )
Q_DECLARE_METATYPE( PHIVariantHash )
Q_DECLARE_METATYPE( PHIColorHash )
Q_DECLARE_METATYPE( PHIImageHash )
Q_DECLARE_METATYPE( QGradientStops )

class PHIEXPORT PHI
{
    Q_DISABLE_COPY( PHI )

public:
    // remove in V2
    enum Widget { NO_ITEM=0, LINE_EDIT=1, MULTI_LINE_EDIT=2, PASSWD=3, CHECK_BOX=4, RADIO_BUTTON=5, BUTTON=6,
        SUBMIT_BUTTON=7, RESET_BUTTON=8, FILE_BUTTON=9, COMBO_BOX=10, HIDDEN=11, IMAGE_BUTTON=12, ELLIPSE=13,
        RECT=14, LINE=15, IMAGE=16, LAYOUT_LOGIN=17, LIST=18, LAYOUT_GRID=19, LAYOUT_VBOX=20, LAYOUT_HBOX=21,
        LAYOUT_FORM=22, LINK=23, TAB=24, ROUNDED_RECT=25, HSPACE=26, VSPACE=27, GRAPH_TEXT=28, TEXT=29,
        LABEL=30, DATEEDIT=31, CALENDAR=32, LANG_SELECTOR=33, LAYOUT_ADDRESS=34, LAYOUT_DELIVERY=35,
        LAYOUT_CREDIT_CARD=36, ROLLOVER_BUTTON=37, LAYOUT_CONTACT=38, LAYOUT_PERIOD=39, LAYOUT_REGISTER=40,
        RICH_TEXT=41, SVG=42, CHECK_LIST=43, DIA_SHOW=44, IMAGE_BOOK=45, TABLE=46, COUNTRY=47, PHISYS_LINK=48,
        HTML_DOC=49, SEARCH=50, EMAIL=51, DECIMAL=52, REALNUMBER=53, PHONE=54, FACEBOOK_LIKE=55, GOOGLE_STATIC_MAP=56,
        GOOGLE_PLUS=57, TWITTER=58, PROGRESSBAR=59, YOUTUBE=60, CANVAS=61
    }; // quint8 in DataStream
    // end remove
    enum Alignment { AlignLeftVCenter=0, AlignLeftTop=1, AlignLeftBottom=2, AlignRightVCenter=3, AlignRightTop=4,
        AlignRightBottom=5, AlignTopHCenter=6, AlignBottomHCenter=7, AlignJustify=8, AlignCenter=9,
        AlignLeft=10, AlignRight=11, AlignTop=12, AlignBottom=13 }; //quint8 in DataStream
    enum ContentData { CDAll=0, CDUpdate=1, CDRequest=2, CDMessage=3, CDTemplatePage=4, CDImage=5,
        CDStream=6 }; // quint8 in DataStream

    static Qt::Alignment toQtAlignment( quint8 );
    static PHIRC socketError( QAbstractSocket::SocketError err );
    static QString tag( const QString &tag, const QString &msg ); // fast extractor <tag>msg</tag>
    static QStringList properties( const QObject* );
    static QString libVersion();
    static void setupApplication( QGuiApplication *app );
    static QString applicationRoot();
    static QString libPath();
    static QString pluginPath();
    static void hsvToHsl( qreal h, qreal s, qreal v, qreal *hh, qreal *ss, qreal *ll );
    static void hslToHsv( qreal hh, qreal ss, qreal ll, qreal *h, qreal *s, qreal *v );
    static QColor percentColor( const QColor &c, qreal fac );
    static inline quint16 maxLength() { return static_cast<quint16>(100); }
    static inline quint8 maxPatternStyle() { return static_cast<quint8>(15); }
    static inline quint8 maxLineStyle() { return static_cast<quint8>(5); }
    static inline int maxZValue() { return 1000; }
    static QByteArray nl();
    static QByteArray dtFormat();
    static QString dtFormatString();
    static QByteArray isoDateFormat();
    static QByteArray domain();
    static QByteArray organisation();
    static inline QString idValidator() { return QStringLiteral( "[A-Za-z][-_A-Za-z0-9]*" ); }

    // Remove in V2:
    static inline qreal minimumHeightForImageButton() { return 30.; }
    static inline qreal minimumHeight() { return 22.; }
    static inline qreal minimumWidth() { return 20.; }
    static inline qreal defaultWidth() { return 80.; }
    static inline qreal defaultHeight() { return 24.; }
    static inline QRect defaultWindowSize() { return QRect( 0, 0, 400, 400 ); }
    static inline qreal minimumWidthForDateEdit() { return 130.; }
    static inline qreal minimumWidthForFileUpload() { return 278.; }
    static inline qreal minimumHeightForFileUpload() { return 26.; }
    static inline qreal minimumWidthForYouTube() { return 130.; }
    static inline QSizeF defaultSizeForBox() { return QSizeF( 200., 160. ); }
    static inline QSizeF minimumSizeForBox() { return QSizeF( 40., 40. ); }
    static inline QSizeF minimumSizeForCalendar() { return QSizeF( 240., 190. ); }
    static inline QSizeF minimumSizeForPix() { return QSizeF( 16., 16. ); }
    static inline QSizeF minimumSizeForTweet() { return QSizeF( 80., 20. ); }
    static inline QSizeF maximumSizeForTweet() { return QSizeF( 130., 62. ); }
    static void setEffect( QGraphicsItem *it, const PHIEffect &e );
    static QByteArray emptyHtmlDoc();
    static QByteArray emptyYouTubeDoc();
    static QSizeF sizeHint( const QSizeF &size, PHI::Widget wid, const QFont &font );
    static bool isWidgetItem( PHI::Widget wid );
    static bool isInputItem( PHI::Widget wid );
    static bool isTabOrderItem( PHI::Widget wid );
    static bool isTextItem( PHI::Widget wid );
    static bool isLayoutContainer( PHI::Widget wid );
    static bool isWidgetContainer( PHI::Widget wid );
    static bool isDisplayItem( PHI::Widget wid );
    static bool isDraggable( PHI::Widget wid );
    static bool isDroppable( PHI::Widget wid );
    static bool isShapeItem( PHI::Widget wid );
    // end remove

    static inline QSize patternIconSize() { return QSize( 48, 12 ); }
    static inline QSize colorIconSize() { return QSize( 14, 14 ); }
    static inline qreal defaultSpacing() { return 6.; }
    static inline qreal fontSizeFactor() { return static_cast<qreal>(1.33333333333); }
    static inline int defaultFontSize() { return 10; }
    static inline QFont invalidFont() { static QFont f( QStringLiteral( "phi" ), 3 ); return f; }
    static inline QFont defaultFont() { static QFont f( QStringLiteral( "Arial" ), 11 ); return f; }
    static inline int defaultDragDistance() { return 5; }
    static inline QSize PHI::defaultPageSize() { return QSize( 1000, 750 ); }

    static QByteArray emailRegExp();
    static QByteArray phoneNumberRegExp();
    static QByteArray mimeType();
    static QByteArray mimeType( const QFileInfo &info );
    static QString stdTmpPath();
    static QString getLocalFilePath( const QString &docroot, const QString &referer, const QString &filename );
    static QString getRelativeFilePath( const QString &referer, const QString &filename );
    static QString createPngUuid();
    static QImage getSurfacedImage( const QImage &img, qreal off, qreal size );
    static void getItemCheckData( QString &data, QString &opt, bool &isChecked );
    static void grayscale( const QImage &image, QImage &dest, const QRect& rect=QRect() );
    static QImage getColorizedImage( const QImage &img, const QColor &c, qreal strength );
    static QImage getShadowedImage( const QImage &img, const QColor &color,
        qreal radius, qreal xOff, qreal yOff );
    static QImage getBluredImage( const QImage &img, qreal radius, qreal factor=2.5 );
    static QDateTime dateTimeFromHeader( const QByteArray &modified );
    static void extractNumbers( const QByteArray &s, int &value, int &min, int &max, int &step );
    static void extractRealNumbers( const QByteArray &s, qreal &value, qreal &min, qreal &max,
        qreal &step, int &decimals );
    static void updateTranslations();
    static QEasingCurve::Type toEasingCurveType( const QString &name );
    static QString toEasingCurveString( quint8 ease ) {
        return QString::fromLatin1( toEasingCurveByteArray( ease ) ); }
    static QByteArray toEasingCurveByteArray( quint8 ease );
    static QStringList availableEasingCurves();
    static inline QString defaultEasingCurve() { return QStringLiteral( "easeOutQuad" ); }
    static inline quint8 defaultEasingCurveType() {
        return static_cast<quint8>(toEasingCurveType(defaultEasingCurve())); }
    static QPoint defaultHotSpot() { return QPoint( 5, 5 ); }
    static Qt::CursorShape toCursorShape( const QByteArray &s );
    static bool isUpToDate( const QString &localV, const QString &serverV );
    static QString toLocale( const QString &lang );
    static QSettings* globalSettings();
    static int checkService();
    static QString linkStyleSheet( const QPalette &p, bool underline=true );
    static bool startPhisService();
    static bool stopPhisService();
    static bool clearPhisServiceCache();
    static QString defaultString();
    static QString serverBin();
    static QUrl createUrlForLink( const QUrl &ref, const QString &link );

protected:
    PHI() {}

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

inline QString PHI::defaultString()
{
    static QString def=QStringLiteral( "default" );
    return def;
}

inline QString PHI::stdTmpPath()
{
    static QString tmp=QStandardPaths::writableLocation(
        QStandardPaths::TempLocation )+QDir::separator()+QStringLiteral( "phi" );
    return tmp;
}

inline QByteArray PHI::dtFormat()
{
    static QByteArray dt=QByteArray::fromRawData( _phiDT, qstrlen( _phiDT )-1 );
    return dt;
}

inline QString PHI::dtFormatString()
{
    static QString dt=QString::fromLatin1( dtFormat() );
    return dt;
}

inline QByteArray PHI::isoDateFormat()
{
    static QByteArray isoDate=QByteArray::fromRawData( _phiDate, qstrlen( _phiDate )-1 );
    return isoDate;
}

inline QByteArray PHI::emailRegExp() {
    static QByteArray email=QByteArray::fromRawData( _emailRegExp, qstrlen( _emailRegExp )-1 );
    return email;
}

inline QByteArray PHI::phoneNumberRegExp()
{
    static QByteArray phone=QByteArray::fromRawData( _phoneNumberRegExp,
        qstrlen( _phoneNumberRegExp )-1 );
    return phone;
}

inline QByteArray PHI::mimeType()
{
    static QByteArray mime=QByteArray::fromRawData( _phiMimeType, qstrlen( _phiMimeType )-1 );
    return mime;
}

inline QByteArray PHI::nl()
{
#ifdef Q_OS_WIN
    static QByteArray nl=QByteArray::fromRawData( "\r\n", 2 );
#else
    static QByteArray nl=QByteArray::fromRawData( "\n", 1 );
#endif
    return nl;
}

#endif /* PHI_H */
