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
#include <QMetaProperty>
#include <QUuid>
#include <QPainter>
#include <QMimeDatabase>
#include "qmath.h"
#include "phi.h"
#include "phierror.h"
#include "phisysinfo.h"
#include "qpixmapfilter_p.h"

#ifdef PHIAPPSTORE
#include "macfilebookmark.h"
#endif

const char* PHI::_phiDT="yyyyMMddHHmmsszzz";
const char* PHI::_phiDate="yyyy-MM-dd";
const char* PHI::_phiMimeType="application/x-phi";
const char* PHI::_emailRegExp="[A-Z0-9._%-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}";
const char* PHI::_phoneNumberRegExp="[0-9+][0-9 ]{3,25}[0-9]";

const QString& PHI::libVersion()
{
    static QString ver=QString::fromLatin1( PHIVERSION );
    return ver;
}

QStringList PHI::properties( const QObject *obj )
{
    Q_ASSERT( obj );
    const QMetaObject *mo=obj->metaObject();
    QStringList properties;
    for( int i=1; i < mo->propertyCount(); ++i )
        properties << QString::fromLatin1( mo->property( i ).name() );
    properties.sort();
    return properties;
}

QByteArray PHI::mimeTypeForFile( const QFileInfo &file )
{
    static QMimeDatabase db;
    static QString phis=QString::fromLatin1( "phis" );
    static QByteArray phismime="application/x-phis";
    if ( file.suffix()==phis ) return phismime;
    return db.mimeTypeForFile( file ).name().toUtf8();
}

QString PHI::tag( const QString &tag, const QString &msg )
{
    // note: QRegExp is too slow!!
    QString startTag( QLatin1Char( '<' )+tag+QLatin1Char( '>' ) );
    QString endTag( QLatin1String( "</" )+tag+QLatin1Char( '>' ) );
    qint32 start( msg.indexOf( startTag ) );
    if ( start==-1 ) return msg;
    qint32 end( msg.lastIndexOf( endTag ) );
    if ( end==-1 ) return msg;
    return msg.mid( start+startTag.length(), end-( start+startTag.length() ) );
}

QString PHI::createPngUuid()
{
    QString id=QUuid::createUuid().toString();
    id.remove( 0, 1 );
    id.chop( 1 );
    id+=QLatin1String( ".png" );
    return id;
}

void PHI::extractNumbers( const QByteArray &s, int &value, int &min, int &max, int &step )
{
    QList <QByteArray> list=s.split( ':' );
    if ( list.count() ) value=list.at(0).toInt();
    else value=0;
    if ( list.count()>1 ) min=list.at(1).toInt();
    else min=0;
    if ( list.count()>2 ) max=list.at(2).toInt();
    else max=100;
    if ( list.count()>3 ) step=list.at(3).toInt();
    else step=1;
}

void PHI::extractRealNumbers( const QByteArray &s, qreal &value, qreal &min, qreal &max,
    qreal &step, int &prec )
{
    QList <QByteArray> list=s.split( ':' );
    if ( list.count() ) value=list.at(0).toDouble();
    else value=0;
    if ( list.count()>1 ) min=list.at(1).toDouble();
    else min=0;
    if ( list.count()>2 ) max=list.at(2).toDouble();
    else max=10.;
    if ( list.count()>3 ) step=list.at(3).toDouble();
    else step=0.5;
    if ( list.count()>4 ) prec=list.at(4).toInt();
    else prec=1;
    if ( prec>10 ) prec=10;
}

QColor PHI::percentColor( const QColor &c, qreal fac )
{
    qreal h, s, v, hh, ss, ll;
    c.getHsvF( &h, &s, &v );
    PHI::hsvToHsl( h, s, v, &hh, &ss, &ll );
    QColor l;
    qreal n=ll+(1-ll)*fac;
    PHI::hslToHsv( hh, ss, n, &h, &s, &v );
    l.setHsvF( h, s, v );
    return l;
}

void PHI::hsvToHsl( qreal h, qreal s, qreal v, qreal *hh, qreal *ss, qreal *ll )
{
    *hh = h;
    *ll = (2 - s) * v;
    *ss = s * v;
    *ss /= (*ll <= 1) ? (*ll) : 2.0 - (*ll);
    *ll /= 2.0;
}

void PHI::hslToHsv( qreal hh, qreal ss, qreal ll, qreal *h, qreal *s, qreal *v )
{
    *h = hh;
    ll *= 2;
    ss *= (ll <= 1) ? ll : 2 - ll;
    *v = (ll + ss) / 2.0;
    *s = (2 * ss) / (ll + ss);
}

/*
QString PHI::getLocalFilePath( const QString &docroot, const QString &referer, const QString &filename )
{
    // we have an absolute file path related to documentroot:
    if ( filename.startsWith( QLatin1Char( '/' ) ) || filename.startsWith( QLatin1Char( '\\' ) ) )
        return QFileInfo( docroot+filename ).canonicalFilePath();
    QString path=QFileInfo( referer ).canonicalPath();
    return QFileInfo( path+QDir::separator()+filename ).canonicalFilePath();
}

QString PHI::getRelativeFilePath( const QString &referer, const QString &filename )
{
    QString path=QFileInfo( referer ).canonicalPath();
    QString file=QFileInfo( filename ).canonicalFilePath();
    file.replace( path, QString() );
    if ( file.startsWith( QLatin1Char( '/' ) ) || file.startsWith( QLatin1Char( '\\' ) ) )
        file.remove( 0, 1 );
    return file;
}

QString PHI::linkStyleSheet( const QPalette &pal, bool underline )
{
    QString a=QString::fromLatin1( "a:link { text-decoration: none; color: %1; }\n"
        "a:visited { text-decoration: none; color: %2; }\na:hover { text-decoration: %3; }\n" )
        .arg( pal.color( QPalette::Link ).name() ).arg( pal.color( QPalette::LinkVisited ).name() )
        .arg( underline ? QStringLiteral( "underline" ) : QStringLiteral( "none" ) );
    return a;
}

void PHI::getItemCheckData( QString &data, QString &opt, bool &isChecked )
{
    isChecked=false;
    opt=QString();
    int start, startsel, end;
    start=data.indexOf( QLatin1Char ( '[' ) );
    if ( start==-1 ) {
        opt=data;
        return;
    }
    end=data.indexOf( QLatin1Char( ']' ) );
    if ( end>0 ) {
        opt=data.mid( start+1, end-start-1 );
        startsel=data.indexOf( QLatin1Char( '[' ), start+1 );
        if ( startsel!=-1 ) {
            end=data.indexOf( QLatin1Char( ']' ), startsel );
            if ( end>0 ) {
                QString selected=data.mid( startsel+1, end-startsel-1 ).toLower();
                if ( selected.toInt() || selected==QLatin1String( "true" ) ) isChecked=true;
            }
        }
        data.truncate( start );
    }
}
*/

QImage PHI::bluredImage( const QImage &img, qreal radius )
{
    if ( radius <=1. ) return img;
    QPixmapBlurFilter f;
    f.setRadius( radius );
    f.setBlurHints( QGraphicsBlurEffect::QualityHint );
    QRect r=f.boundingRectFor( img.rect() ).toAlignedRect();
    QImage dest=QImage( r.width(), r.height(), QImage::Format_ARGB32_Premultiplied );
    dest.fill( 0 );
    QPainter p( &dest );
    f.draw( &p, QPointF(), QPixmap::fromImage( img ) );
    p.end();
    return dest;
}

QImage PHI::dropShadowedImage( const QImage &img, const QColor &color, qreal radius,
    qreal xOff, qreal yOff )
{
    if ( radius <=1. ) return img;
    QPixmapDropShadowFilter f;
    f.setBlurRadius( radius );
    f.setColor( color );
    f.setOffset( xOff, yOff );
    QRect r=f.boundingRectFor( img.rect() ).toAlignedRect();
    QImage dest=QImage( r.width(), r.height(), QImage::Format_ARGB32_Premultiplied );
    dest.fill( 0 );
    QPainter p( &dest );
    f.draw( &p, QPointF(), QPixmap::fromImage( img ) );
    p.end();
    return dest;
}

QImage PHI::colorizedImage( const QImage &img, const QColor &c, qreal strength )
{
    QPixmapColorizeFilter f;
    f.setColor( c );
    f.setStrength( strength );
    QRect r=f.boundingRectFor( img.rect() ).toAlignedRect();
    QImage dest=QImage( r.width(), r.height(), QImage::Format_ARGB32_Premultiplied );
    dest.fill( 0 );
    QPainter p( &dest );
    f.draw( &p, QPointF(), QPixmap::fromImage( img ) );
    p.end();
    return dest;
}

QPixmap PHI::colorizedPixmap( const QPixmap &src, const QColor &c, qreal strength )
{
    QPixmapColorizeFilter f;
    f.setColor( c );
    f.setStrength( strength );
    QRect r=f.boundingRectFor( QRectF( QPointF(), QSize( src.width(), src.height() ) ) ).toAlignedRect();
    QPixmap dest( r.width(), r.height() );
    dest.fill( Qt::transparent );
    QPainter p( &dest );
    f.draw( &p, QPointF(), src );
    p.end();
    return dest;
}

QPixmap PHI::dropShadowedPixmap( const QPixmap &src, const QPointF &off, const QColor &c, qreal radius )
{
    QPixmapDropShadowFilter f;
    f.setOffset( off );
    f.setBlurRadius( radius );
    f.setColor( c );
    QRect r=f.boundingRectFor( QRectF( QPointF(), QSize( src.width(), src.height() ) ) ).toAlignedRect();
    QPixmap dest( r.width(), r.height() );
    dest.fill( Qt::transparent );
    QPainter p( &dest );
    f.draw( &p, QPointF(), src );
    p.end();
    return dest;
}

/*
void PHI::grayscale( const QImage &image, QImage &dest, const QRect& rect )
{
    //qDebug() << image.width() << image.height() << dest.width() << dest.height() << image.format();
    QRect destRect = rect;
    QRect srcRect = rect;
    if ( rect.isNull() ) {
        srcRect = dest.rect();
        destRect = dest.rect();
    }
    if ( &image!=&dest ) destRect.moveTo( QPoint(0, 0) );

    unsigned int *data = (unsigned int*)image.bits();
    unsigned int *outData = (unsigned int*)dest.bits();

    if (dest.size() == image.size() && image.rect() == srcRect) {
        // a bit faster loop for grayscaling everything
        int pixels = dest.width() * dest.height();
        for (int i = 0; i < pixels; ++i) {
            int val = qGray(data[i]);
            outData[i] = qRgba(val, val, val, qAlpha(data[i]));
        }
    } else {
        int yd = destRect.top();
        for (int y = srcRect.top(); y <= srcRect.bottom() && y < image.height(); y++) {
            data = (unsigned int*)image.scanLine(y);
            outData = (unsigned int*)dest.scanLine(yd++);
            int xd = destRect.left();
            for (int x = srcRect.left(); x <= srcRect.right() && x < image.width(); x++) {
                int val = qGray(data[x]);
                outData[xd++] = qRgba(val, val, val, qAlpha(data[x]));
            }
        }
    }
}
*/

QImage PHI::reflectedImage( const QImage &img, qreal off, qreal size )
{
    if ( img.isNull() ) return img;
    QImage newimg( img.width(), qMax( img.height(), img.height()+static_cast<int>(size+off) ),
        QImage::Format_ARGB32_Premultiplied );
    QPainter pixPainter;
    pixPainter.begin( &newimg );

    pixPainter.setRenderHints( QPainter::Antialiasing
        | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing );
    pixPainter.setCompositionMode( QPainter::CompositionMode_Clear );
    pixPainter.fillRect( 0., 0., img.width(), newimg.height()+1, Qt::transparent );
    pixPainter.setCompositionMode( QPainter::CompositionMode_SourceOver );
    pixPainter.drawImage( 0, 0, img );

    //pixPainter.drawRect( brect );

    QTransform t;
    t.rotate( 180., Qt::XAxis );
    t.translate( 0., static_cast<qreal>(-img.height())*2.-off );
    pixPainter.setTransform( t );
    pixPainter.drawImage( 0, 1, img );
    pixPainter.resetTransform();
    pixPainter.translate( 0., static_cast<qreal>(img.height())+off );

    QLinearGradient gradient( 0., 0., 0., 1. );
    gradient.setColorAt( 0., QColor( 0, 0, 0, 220 ) );
    gradient.setColorAt( 0.78, QColor( 0, 0, 0, 30 ));
    gradient.setColorAt( 1., Qt::transparent );
    gradient.setCoordinateMode( QGradient::ObjectBoundingMode );

    pixPainter.setCompositionMode( QPainter::CompositionMode_DestinationIn );
    pixPainter.fillRect( 0., 0., static_cast<qreal>(img.width()), size+1, gradient );
    //pixPainter.setCompositionMode( QPainter::CompositionMode_SourceOver );
    //pixPainter.drawRect( 0., brect.height()-1., brect.width(), _size );
    pixPainter.end();
    return newimg;
}

/*
Qt::Alignment PHI::toQtAlignment( quint8 align )
{
    switch ( static_cast<PHI::Alignment>(align) ) {
    case PHI::AlignCenter: return Qt::AlignVCenter | Qt::AlignHCenter;
    case PHI::AlignBottom: return Qt::AlignBottom | Qt::AlignHCenter;
    case PHI::AlignTopHCenter:
    case PHI::AlignTop: return Qt::AlignTop | Qt::AlignHCenter;
    case PHI::AlignJustify: return Qt::AlignJustify;
    case PHI::AlignRightTop:
    case PHI::AlignRight: return Qt::AlignTop | Qt::AlignRight;
    case PHI::AlignRightVCenter: return Qt::AlignVCenter | Qt::AlignRight;
    case PHI::AlignRightBottom: return Qt::AlignBottom | Qt::AlignRight;
    case PHI::AlignLeftVCenter: return Qt::AlignVCenter | Qt::AlignLeft;
    case PHI::AlignLeftBottom: return Qt::AlignBottom | Qt::AlignLeft;
    default: return Qt::AlignLeft | Qt::AlignTop;
    }
}
*/

/*
// remove in V2
void PHI::setEffect( QGraphicsItem *it, const PHIEffect &e )
{
    Q_ASSERT( it );
    if ( e.effects() & PHIEffect::EGraphics ) {
        qreal eStrength, eRadius, eXOff, eYOff;
        QColor eColor;
        switch ( e.graphicsType() ) {
        case PHIEffect::GTBlur: {
            e.blur( eRadius );
            QGraphicsBlurEffect *blur=new QGraphicsBlurEffect();
            blur->setBlurHints( QGraphicsBlurEffect::AnimationHint );
            blur->setBlurRadius( eRadius );
            it->setGraphicsEffect( blur );
            break;
        }
        case PHIEffect::GTColorize: {
            e.colorize( eColor, eStrength );
            QGraphicsColorizeEffect *colorize=new QGraphicsColorizeEffect();
            colorize->setStrength( eStrength );
            colorize->setColor( eColor );
            it->setGraphicsEffect( colorize );
            break;
        }
        case PHIEffect::GTShadow: {
            e.shadow( eColor, eXOff, eYOff, eRadius );
            QGraphicsDropShadowEffect *shadow=new QGraphicsDropShadowEffect();
            shadow->setBlurRadius( eRadius*2. );
            shadow->setColor( eColor );
            shadow->setXOffset( eXOff );
            shadow->setYOffset( eYOff );
            it->setGraphicsEffect( shadow );
            break;
        }
        case PHIEffect::GTSurface: {
            e.surface( eYOff, eRadius );
            PHISurfaceEffect *surface=new PHISurfaceEffect();
            surface->setYOffset( eYOff );
            surface->setSize( eRadius );
            it->setGraphicsEffect( surface );
            break;
        }
        default:;
        }
    } else it->setGraphicsEffect( 0 );
}
// end remove

QByteArray PHI::emptyHtmlDoc()
{
    QByteArray arr="<html><head></head><body>";
    arr+=QObject::tr( "empty HTML document" ).toUtf8();
    arr+="</body></html>";
    return arr;
}

QByteArray PHI::emptyYouTubeDoc()
{
    QByteArray arr="<html><head></head><body>";
    arr+=QObject::tr( "ID provided at runtime." ).toUtf8();
    arr+="</body></html>";
    return arr;
}
*/

QByteArray PHI::toEasingCurveByteArray( quint8 ease )
{
    QByteArray s;
    switch ( static_cast<QEasingCurve::Type>(ease) ) {
    case QEasingCurve::Linear: s="linear"; break;
    case QEasingCurve::InQuad: s="easeInQuad"; break;
    case QEasingCurve::OutQuad: s="easeOutQuad"; break;
    case QEasingCurve::InOutQuad: s="easeInOutQuad"; break;
    case QEasingCurve::InCubic: s="easeInCubic"; break;
    case QEasingCurve::OutCubic: s="easeOutCubic"; break;
    case QEasingCurve::InOutCubic: s="easeInOutCubic"; break;
    case QEasingCurve::InQuart: s="easeInQuart"; break;
    case QEasingCurve::OutQuart: s="easeOutQuart"; break;
    case QEasingCurve::InOutQuart: s="easeInOutQuart"; break;
    case QEasingCurve::InQuint: s="easeInQuint"; break;
    case QEasingCurve::OutQuint: s="easeOutQuint"; break;
    case QEasingCurve::InOutQuint: s="easeInOutQuint"; break;
    case QEasingCurve::InSine: s="easeInSine"; break;
    case QEasingCurve::OutSine: s="easeOutSine"; break;
    case QEasingCurve::InOutSine: s="easeInOutSine"; break;
    case QEasingCurve::InExpo: s="easeInExpo"; break;
    case QEasingCurve::OutExpo: s="easeOutExpo"; break;
    case QEasingCurve::InOutExpo: s="easeInOutExpo"; break;
    case QEasingCurve::InCirc: s="easeInCirc"; break;
    case QEasingCurve::OutCirc: s="easeOutCirc"; break;
    case QEasingCurve::InOutCirc: s="easeInOutCirc"; break;
    case QEasingCurve::InElastic: s="easeInElastic"; break;
    case QEasingCurve::OutElastic: s="easeOutElastic"; break;
    case QEasingCurve::InOutElastic: s="easeInOutElastic"; break;
    case QEasingCurve::InBack: s="easeInBack"; break;
    case QEasingCurve::OutBack: s="easeOutBack"; break;
    case QEasingCurve::InOutBack: s="easeInOutBack"; break;
    case QEasingCurve::InBounce: s="easeInBounce"; break;
    case QEasingCurve::OutBounce: s="easeOutBounce"; break;
    case QEasingCurve::InOutBounce: s="easeInOutBounce"; break;
    default:
        s=PHI::defaultEasingCurve().toLatin1();
    }
    return s;
}

QEasingCurve::Type PHI::toEasingCurveType( const QString &name )
{
    if ( name==QLatin1String( "linear" ) ) return QEasingCurve::Linear;
    if ( name==QLatin1String( "easeInQuad" ) ) return QEasingCurve::InQuad;
    if ( name==QLatin1String( "easeOutQuad" ) ) return QEasingCurve::OutQuad;
    if ( name==QLatin1String( "easeInOutQuad" ) ) return QEasingCurve::InOutQuad;
    if ( name==QLatin1String( "easeInCubic" ) ) return QEasingCurve::InCubic;
    if ( name==QLatin1String( "easeOutCubic" ) ) return QEasingCurve::OutCubic;
    if ( name==QLatin1String( "easeInOutCubic" ) ) return QEasingCurve::InOutCubic;
    if ( name==QLatin1String( "easeInQuart" ) ) return QEasingCurve::InQuart;
    if ( name==QLatin1String( "easeOutQuart" ) ) return QEasingCurve::OutQuart;
    if ( name==QLatin1String( "easeInOutQuart" ) ) return QEasingCurve::InOutQuart;
    if ( name==QLatin1String( "easeInQuint" ) ) return QEasingCurve::InQuint;
    if ( name==QLatin1String( "easeOutQuint" ) ) return QEasingCurve::OutQuint;
    if ( name==QLatin1String( "easeInOutQuint" ) ) return QEasingCurve::InOutQuint;
    if ( name==QLatin1String( "easeInSine" ) ) return QEasingCurve::InSine;
    if ( name==QLatin1String( "easeOutSine" ) ) return QEasingCurve::OutSine;
    if ( name==QLatin1String( "easeInOutSine" ) ) return QEasingCurve::InOutSine;
    if ( name==QLatin1String( "easeInExpo" ) ) return QEasingCurve::InExpo;
    if ( name==QLatin1String( "easeOutExpo" ) ) return QEasingCurve::OutExpo;
    if ( name==QLatin1String( "easeInOutExpo" ) ) return QEasingCurve::InOutExpo;
    if ( name==QLatin1String( "easeInCirc" ) ) return QEasingCurve::InCirc;
    if ( name==QLatin1String( "easeOutCirc" ) ) return QEasingCurve::OutCirc;
    if ( name==QLatin1String( "easeInOutCirc" ) ) return QEasingCurve::InOutCirc;
    if ( name==QLatin1String( "easeInElastic" ) ) return QEasingCurve::InElastic;
    if ( name==QLatin1String( "easeOutElastic" ) ) return QEasingCurve::OutElastic;
    if ( name==QLatin1String( "easeInOutElastic" ) ) return QEasingCurve::InOutElastic;
    if ( name==QLatin1String( "easeInBack" ) ) return QEasingCurve::InBack;
    if ( name==QLatin1String( "easeOutBack" ) ) return QEasingCurve::OutBack;
    if ( name==QLatin1String( "easeInOutBack" ) ) return QEasingCurve::InOutBack;
    if ( name==QLatin1String( "easeInBounce" ) ) return QEasingCurve::InBounce;
    if ( name==QLatin1String( "easeOutBounce" ) ) return QEasingCurve::OutBounce;
    if ( name==QLatin1String( "easeInOutBounce" ) ) return QEasingCurve::InOutBounce;
    return QEasingCurve::OutQuad;
}

QStringList PHI::availableEasingCurves()
{
    static QStringList list;
    if ( list.count() ) return list;
    list
    << QStringLiteral( "linear" )
    << QStringLiteral( "easeInQuad" )
    << QStringLiteral( "easeOutQuad" )
    << QStringLiteral( "easeInOutQuad" )
    << QStringLiteral( "easeInCubic" )
    << QStringLiteral( "easeOutCubic" )
    << QStringLiteral( "easeInOutCubic" )
    << QStringLiteral( "easeInQuart" )
    << QStringLiteral( "easeOutQuart" )
    << QStringLiteral( "easeInOutQuart" )
    << QStringLiteral( "easeInQuint" )
    << QStringLiteral( "easeOutQuint" )
    << QStringLiteral( "easeInOutQuint" )
    << QStringLiteral( "easeInSine" )
    << QStringLiteral( "easeOutSine" )
    << QStringLiteral( "easeInOutSine" )
    << QStringLiteral( "easeInExpo" )
    << QStringLiteral( "easeOutExpo" )
    << QStringLiteral( "easeInOutExpo" )
    << QStringLiteral( "easeInCirc" )
    << QStringLiteral( "easeOutCirc" )
    << QStringLiteral( "easeInOutCirc" )
    << QStringLiteral( "easeInElastic" )
    << QStringLiteral( "easeOutElastic" )
    << QStringLiteral( "easeInOutElastic" )
    << QStringLiteral( "easeInBack" )
    << QStringLiteral( "easeOutBack" )
    << QStringLiteral( "easeInOutBack" )
    << QStringLiteral( "easeInBounce" )
    << QStringLiteral( "easeOutBounce" )
    << QStringLiteral( "easeInOutBounce" );
    return list;
}

Qt::CursorShape PHI::toCursorShape( const QByteArray &s )
{
    if ( s=="crosshair" ) return Qt::CrossCursor;
    if ( s=="default" ) return Qt::ArrowCursor;
    if ( s=="e-resize" ) return Qt::SizeHorCursor;
    if ( s=="help" ) return Qt::WhatsThisCursor;
    if ( s=="move" ) return Qt::OpenHandCursor;
    if ( s=="n-resize" ) return Qt::SizeVerCursor;
    if ( s=="ne-resize" ) return Qt::SizeBDiagCursor;
    if ( s=="nw-resize" ) return Qt::SizeFDiagCursor;
    if ( s=="pointer" ) return Qt::PointingHandCursor;
    if ( s=="progress" ) return Qt::BusyCursor;
    if ( s=="s-resize" ) return Qt::SizeVerCursor;
    if ( s=="se-resize" ) return Qt::SizeFDiagCursor;
    if ( s=="sw-resize" ) return Qt::SizeBDiagCursor;
    if ( s=="text" ) return Qt::IBeamCursor;
    if ( s=="w-resize" ) return Qt::SizeHorCursor;
    if ( s=="wait" ) return Qt::WaitCursor;
    return Qt::ArrowCursor;
}

bool PHI::isUpToDate( const QString &localV, const QString &serverV )
{
    QString l=localV;
    QString s=serverV;
    int lm=l.replace( QRegExp( QStringLiteral( "\\..*" ) ), QString() ).toInt();
    int sm=s.replace( QRegExp( QStringLiteral( "\\..*" ) ), QString() ).toInt();
    if ( lm > sm ) return true;
    if ( lm < sm ) return false;
    l=localV.mid( l.length()+1 );
    s=serverV.mid( s.length()+1 );
    lm=l.replace( QRegExp( QStringLiteral( "\\..*" ) ), QString() ).toInt();
    sm=s.replace( QRegExp( QStringLiteral( "\\..*" ) ), QString() ).toInt();
    if ( lm > sm ) return true;
    if ( lm < sm ) return false;
    lm=localV.mid( localV.lastIndexOf( QLatin1Char( '.' ) )+1 ).toInt();
    sm=serverV.mid( serverV.lastIndexOf( QLatin1Char( '.' ) )+1 ).toInt();
    return lm >= sm;
}

QString PHI::toLocale( const QString &lang )
{
    if ( lang.length()==2 ) return lang.left( 2 ).toLower();
    if ( lang.length()==5 ) return lang.left( 2 ).toLower()+QLatin1Char( '_' )+lang.right( 2 ).toUpper();
    if ( lang.length()==6 ) return lang.left( 2 ).toLower()+QLatin1Char( '_' )+lang.right( 3 ).toUpper();
    return lang.left( 2 ).toLower();
}

PHIRC PHI::socketError( QAbstractSocket::SocketError err )
{
    switch ( err ){
    case QAbstractSocket::ConnectionRefusedError:
        return PHIRC_TCP_CONNECTION_REFUSED;
    case QAbstractSocket::RemoteHostClosedError:
        return PHIRC_TCP_REMOTE_HOST_CLOSED;
    case QAbstractSocket::HostNotFoundError:
        return PHIRC_TCP_HOST_NOT_FOUND_ERROR;
    case QAbstractSocket::SocketTimeoutError:
        return PHIRC_TCP_SOCKET_TIMEOUT_ERROR;
    case QAbstractSocket::NetworkError:
        return PHIRC_TCP_NETWORK_ERROR;
    default:;
    }
    return PHIRC_TCP_UNKNOWN_SOCKET_ERROR;
}

/*
// Remove in V2
QSizeF PHI::sizeHint( const QSizeF &size, PHI::Widget wid, const QFont &font )
{
    bool isCombo( false ), change( false );
    QSizeF s=size;
    QFont f=font;
    switch ( wid ) {
    case PHI::BUTTON:
    case PHI::SUBMIT_BUTTON:
    case PHI::RESET_BUTTON:
    case PHI::FILE_BUTTON:
    case PHI::DATEEDIT:
    case PHI::LINE_EDIT:
    case PHI::SEARCH:
    case PHI::EMAIL:
    case PHI::DECIMAL:
    case PHI::REALNUMBER:
    case PHI::PHONE:
    case PHI::PASSWD: change=true; break;
    case PHI::LANG_SELECTOR:
    case PHI::COMBO_BOX:
    case PHI::COUNTRY: change=true; isCombo=true; break;
    default:;
    }

    if ( change ) {
#ifdef PHIMACFONT
        f.setPointSizeF( font.pointSizeF()/PHI::fontSizeFactor() );
#endif

        if ( f.pointSize() < 12 ) s.setHeight( PHI::defaultHeight() );
        else if ( f.pointSize() > 14 ) s.setHeight( f.pointSize()*2 );
        else s.setHeight( f.pointSize()+14 );
    }
    if ( isCombo ) if ( s.height()<25. ) s.setHeight( 24. );
    //qDebug( "SIZE HINT font %d, %f (%d)", f.pointSize(), f.pointSizeF(), font.pointSize() );
    return s;
}

// object is APHProxyWidget
bool PHI::isWidgetItem( PHI::Widget wid )
{
    switch ( wid ) {
    case PHI::LINE_EDIT:
    case PHI::MULTI_LINE_EDIT:
    case PHI::PASSWD:
    case PHI::CHECK_BOX:
    case PHI::RADIO_BUTTON:
    case PHI::BUTTON:
    case PHI::SUBMIT_BUTTON:
    case PHI::RESET_BUTTON:
    case PHI::FILE_BUTTON:
    case PHI::COMBO_BOX:
    case PHI::HIDDEN: return true;
    case PHI::IMAGE_BUTTON:
    case PHI::LAYOUT_VBOX:
    case PHI::LAYOUT_HBOX:
    case PHI::LAYOUT_GRID:
    //case PHI::LAYOUT_GROUP_BOX:
    case PHI::LAYOUT_FORM:
    case PHI::LABEL:
    case PHI::LIST:
    case PHI::DATEEDIT:
    case PHI::CALENDAR: return true;
    case PHI::LANG_SELECTOR:
    case PHI::RICH_TEXT:
    case PHI::ROLLOVER_BUTTON:
    case PHI::LINK:
    case PHI::IMAGE_BOOK:
    case PHI::CHECK_LIST:
    case PHI::TABLE:
    case PHI::SEARCH: return true;
    case PHI::EMAIL:
    case PHI::DECIMAL:
    case PHI::REALNUMBER:
    case PHI::PHONE:
    case PHI::COUNTRY:
    case PHI::PHISYS_LINK:
    case PHI::HTML_DOC: return true;
    case PHI::PROGRESSBAR:
    case PHI::GOOGLE_STATIC_MAP:
    case PHI::FACEBOOK_LIKE:
    case PHI::GOOGLE_PLUS:
    case PHI::TWITTER:
    case PHI::YOUTUBE:
    case PHI::GOOGLE_CALENDAR:
    case PHI::GOOGLE_MAPS: return true;
    case PHI::LAYOUT_LOGIN:
    case PHI::LAYOUT_ADDRESS:
    case PHI::LAYOUT_DELIVERY:
    case PHI::LAYOUT_CREDIT_CARD:
    case PHI::LAYOUT_CONTACT:
    case PHI::LAYOUT_PERIOD:
    case PHI::LAYOUT_REGISTER:
    case PHI::HSPACE:
    case PHI::VSPACE:
        return true;
    default: ;
    }
    return false;
}

// item is able to send data in a form
bool PHI::isInputItem( PHI::Widget wid )
{
    switch ( wid ) {
    case PHI::LINE_EDIT:
    case PHI::MULTI_LINE_EDIT:
    case PHI::PASSWD:
    case PHI::CHECK_BOX:
    case PHI::RADIO_BUTTON:
    case PHI::SUBMIT_BUTTON:
    case PHI::FILE_BUTTON:
    case PHI::COMBO_BOX:
    case PHI::LIST:
    case PHI::HIDDEN: return true;
    case PHI::CALENDAR:
    case PHI::DATEEDIT:
    case PHI::LANG_SELECTOR:
    case PHI::CHECK_LIST:
    case PHI::COUNTRY: return true;
    case PHI::TABLE:
    case PHI::SEARCH:
    case PHI::EMAIL:
    case PHI::DECIMAL:
    case PHI::REALNUMBER:
    case PHI::PHONE:
    case PHI::PROGRESSBAR:
        return true;
    default: ;
    }
    return false;
}

// item has a tab index
bool PHI::isTabOrderItem( PHI::Widget  wid )
{
    if ( wid==PHI::HIDDEN ) return false;
    if ( PHI::isInputItem( wid ) ) return true;
    switch ( wid ) {
    case PHI::IMAGE_BUTTON: return true;
    case PHI::BUTTON: return true;
    case PHI::LINK: return true;
    case PHI::RESET_BUTTON: return true;
    case PHI::ROLLOVER_BUTTON: return true;
    case PHI::RICH_TEXT: return true;
    case PHI::IMAGE_BOOK: return true;
    case PHI::PHISYS_LINK: return true;
    case PHI::HTML_DOC: return true;
    case PHI::GOOGLE_STATIC_MAP: return true;
    case PHI::FACEBOOK_LIKE: return true;
    case PHI::GOOGLE_PLUS: return true;
    case PHI::TWITTER: return true;
    case PHI::YOUTUBE: return true;
    case PHI::GOOGLE_CALENDAR: return true;
    case PHI::GOOGLE_MAPS: return true;
    default: ;
    }
    return false;
}

// represents text items ie. for text shadows (no box elment)
bool PHI::isTextItem( PHI::Widget wid ) {
    switch( wid ) {
    case PHI::LABEL: return true;
    case PHI::TEXT: return true;
    case PHI::RICH_TEXT: return true;
    case PHI::TABLE: return true;
    case PHI::CHECK_BOX: return true;
    case PHI::RADIO_BUTTON: return true;
    case PHI::CHECK_LIST: return true;
    case PHI::LINK: return true;
    default:;
    }
    if ( PHI::isLayoutContainer( wid ) ) return true;
    return false;
}

// Layout which contains other items
bool PHI::isWidgetContainer( PHI::Widget wid )
{
    switch( wid ) {
    case PHI::LAYOUT_LOGIN: return true;
    case PHI::LAYOUT_ADDRESS: return true;
    case PHI::LAYOUT_DELIVERY: return true;
    case PHI::LAYOUT_CREDIT_CARD: return true;
    case PHI::LAYOUT_CONTACT: return true;
    case PHI::LAYOUT_PERIOD: return true;
    case PHI::LAYOUT_REGISTER: return true;
    default: ;
    }
    return false;
}

// Item represents a layout
bool PHI::isLayoutContainer( PHI::Widget wid )
{
    switch ( wid ) {
    case PHI::LAYOUT_VBOX: return true;
    case PHI::LAYOUT_HBOX: return true;
    case PHI::LAYOUT_GRID: return true;
    case PHI::LAYOUT_FORM: return true;
    //case PHI::LAYOUT_GROUP_BOX:
    case PHI::LAYOUT_LOGIN: return true;
    case PHI::LAYOUT_ADDRESS: return true;
    case PHI::LAYOUT_DELIVERY: return true;
    case PHI::LAYOUT_CREDIT_CARD: return true;
    case PHI::LAYOUT_CONTACT: return true;
    case PHI::LAYOUT_PERIOD: return true;
    case PHI::LAYOUT_REGISTER: return true;
    default: ;
    }
    return false;
}

// Item is not widget based
bool PHI::isDisplayItem( PHI::Widget wid )
{
    switch ( wid ) {
    case PHI::ELLIPSE: return true;
    case PHI::RECT: return true;
    case PHI::ROUNDED_RECT: return true;
    case PHI::LINE: return true;
    case PHI::IMAGE: return true;
    case PHI::GRAPH_TEXT: return true;
    case PHI::TEXT: return true;
    case PHI::SVG: return true;
    case PHI::DIA_SHOW: return true;
    case PHI::CANVAS: return true;
    default: ;
    }
    return false;
}

bool PHI::isDraggable( PHI::Widget wid )
{
    if ( isDisplayItem( wid ) ) return true;
    //if ( isLayoutContainer( wid ) ) return true;
    switch( wid ) {
    case PHI::TEXT: return true;
    case PHI::LABEL: return true;
    case PHI::LINK: return true;
    case PHI::PROGRESSBAR: return true;
    case PHI::GOOGLE_STATIC_MAP: return true;
    //case PHI::HTML_DOC: return true;
    //case PHI::YOUTUBE: return true;
    default:;
    }
    return false;
}

bool PHI::isDroppable( PHI::Widget wid )
{
    if ( isDisplayItem( wid ) ) return true;
    if ( isLayoutContainer( wid ) ) return true;
    return false;
}

// used to reset gradient
bool PHI::isShapeItem( PHI::Widget wid )
{
    switch ( wid ) {
    case PHI::ELLIPSE:
    case PHI::RECT:
    case PHI::ROUNDED_RECT:
    case PHI::GRAPH_TEXT:
        return true;
    default: ;
    }
    return false;
}
// end remove
*/
