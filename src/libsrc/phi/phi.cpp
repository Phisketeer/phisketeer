/*
#    Copyright (C) 2008  Trolltech AS
#    Copyright (C) 2012  Digia Plc and/or its subsidiary(-ies).
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

#if ( QT_VERSION <= QT_VERSION_CHECK( 5, 1, 0 ) )
#error To compile Phi(sketeer) successfully you need at least Qt 5.1.1
#endif
#define PHIBOUND(val, min, max) qMin(qMax(val, min), max)

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
    for( int i=1; i < mo->propertyCount(); ++i ) {
        if ( !mo->property( i ).isScriptable() ) continue;
        properties << QString::fromLatin1( mo->property( i ).name() );
    }
    properties.sort();
    return properties;
}

QByteArray PHI::mimeTypeForFile( const QFileInfo &file )
{
    static QMimeDatabase db;
    static QByteArray phismime="application/x-phis";
    if ( file.suffix()==L1( "phis" ) ) return phismime;
    return db.mimeTypeForFile( file ).name().toLatin1();
}

QString PHI::tag( const QString &tag, const QString &msg )
{
    // note: QRegExp is too slow!!
    QString startTag( QLatin1Char( '<' )+tag+QLatin1Char( '>' ) );
    QString endTag( L1( "</" )+tag+QLatin1Char( '>' ) );
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

Qt::Alignment PHI::textAlignToQtAlignment( const QString &a )
{
    if ( a==L1( "right" ) ) return Qt::AlignRight;
    if ( a==L1( "center" ) ) return Qt::AlignHCenter;
    if ( a==L1( "justify" ) ) return Qt::AlignJustify;
    return Qt::AlignLeft;
}

QString PHI::qtAlignmentToTextAlign( Qt::Alignment a )
{
    if ( a & Qt::AlignRight ) return SL( "right" );
    if ( a & Qt::AlignHCenter || a & Qt::AlignCenter ) return SL( "center" );
    if ( a & Qt::AlignJustify ) return SL( "justify" );
    return SL( "left" );
}

/*
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
*/

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
*/

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
                if ( selected.toInt() || selected.toLower()==L1( "true" ) ) isChecked=true;
            }
        }
        data.truncate( start );
    }
}

void PHI::getItemCheckData( QByteArray &data, QByteArray &opt, bool &isChecked )
{
    isChecked=false;
    opt=QByteArray();
    int start, startsel, end;
    start=data.indexOf( '[' );
    if ( start==-1 ) {
        opt=data;
        return;
    }
    end=data.indexOf( ']' );
    if ( end>0 ) {
        opt=data.mid( start+1, end-start-1 );
        startsel=data.indexOf( '[', start+1 );
        if ( startsel!=-1 ) {
            end=data.indexOf( ']', startsel );
            if ( end>0 ) {
                QByteArray selected=data.mid( startsel+1, end-startsel-1 ).toLower();
                if ( selected.toInt() || selected.toLower()=="true" ) isChecked=true;
            }
        }
        data.truncate( start );
    }
}

QImage PHI::bluredImage( const QImage &img, qreal radius, QRectF &br )
{
    QPixmapBlurFilter f;
    f.setRadius( radius );
    f.setBlurHints( QGraphicsBlurEffect::AnimationHint );
    QRectF r=f.boundingRectFor( img.rect() );
    br.setX( br.x()+r.x()+1 );
    br.setY( br.y()+r.y()+1 );
    br.setWidth( r.width()-3 );
    br.setHeight( r.height()-3 );

    QPixmap pix( r.width()-3, r.height()-3 );
    pix.fill( Qt::transparent );
    QPainter p( &pix );
    p.drawImage( -r.x()-1, -r.y()-1, img );
    p.end();

    QImage dest( pix.size(), QImage::Format_ARGB32_Premultiplied );
    dest.fill( 0 );
    p.begin( &dest );
    f.draw( &p, QPointF(), pix );
    //p.setPen( QPen( Qt::DotLine ) );
    //p.drawRect( 0, 0, dest.width()-1, dest.height()-1 );
    p.end();
    return dest;
}

QImage PHI::dropShadowedImage( const QImage &img, const QColor &color, qreal radius,
    qreal xOff, qreal yOff, QRectF &br )
{
    QPixmapDropShadowFilter f;
    f.setBlurRadius( radius*2. );
    f.setColor( color );
    f.setOffset( xOff, yOff );
    QRectF r=f.boundingRectFor( img.rect() );
    if ( r.x()<0 ) br.setX( br.x()+r.x()+1 );
    if ( r.y()<0 ) br.setY( br.y()+r.y()+1 );
    br.setWidth( r.width()-1 );
    br.setHeight( r.height()-1 );

    QPixmap pix( r.width()-1, r.height()-1 );
    pix.fill( Qt::transparent );
    QPainter p( &pix );
    p.drawImage( r.x()<0 ? -r.x()-1 : r.x(), r.y()<0 ? -r.y()-1 : r.y(), img );
    p.end();

    QImage dest( pix.size(), QImage::Format_ARGB32_Premultiplied );
    dest.fill( 0 );
    p.begin( &dest );
    f.draw( &p, QPointF(), pix );
    //p.setPen( QPen( Qt::DotLine ) );
    //p.drawRect( 0, 0, dest.width()-1, dest.height()-1 );
    p.end();
    return dest;
}

QImage PHI::colorizedImage( const QImage &img, const QColor &c, qreal strength, QRectF &br )
{
    QPixmapColorizeFilter f;
    f.setColor( c );
    f.setStrength( strength );
    QImage dest( br.size().toSize(), QImage::Format_ARGB32_Premultiplied );
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
    f.setBlurRadius( radius*2. );
    f.setColor( c );
    QRect r=f.boundingRectFor( QRectF( QPointF(), QSize( src.width(), src.height() ) ) ).toAlignedRect();
    QPixmap dest( r.width(), r.height() );
    dest.fill( Qt::transparent );
    QPainter p( &dest );
    f.draw( &p, QPointF(), src );
    p.end();
    return dest;
}

QImage PHI::reflectedImage( const QImage &img, qreal off, qreal size, QRectF &br )
{
    if ( img.isNull() ) return img;
    br.setWidth( img.width() );
    br.setHeight( qMax(img.height(), img.height()+qRound(size+off)) );
    QImage dest( br.width(), br.height(), QImage::Format_ARGB32_Premultiplied );
    dest.fill( 0 );

    QPoint start( 0, 0 );
    QPoint end( 0, qRound( size ) );
    QLinearGradient gradient( start, end );
    gradient.setColorAt( 0, Qt::white );
    gradient.setColorAt( .9, Qt::black );
    gradient.setColorAt( 1., Qt::black );

    QImage mask=img;
    QPainter p( &mask );
    p.fillRect( QRectF( 0, 0, img.width(), size ), gradient );
    p.end();
    QImage mirrored=img.mirrored();
    mirrored.setAlphaChannel( mask );

    p.begin( &dest );
    p.drawImage( 0, 0, img );
    p.setOpacity( 0.8 );
    p.drawImage( QPointF( 0, img.height()-2+off ), mirrored, QRectF( 0, 0, img.width(), size ) );
    p.end();
    return dest;
}

QList<qreal> PHI::parseNumbersList( QString::const_iterator &itr )
{
    QList<qreal> points;
    QString temp;
    while ( (*itr).isSpace() ) ++itr;
    while ( (*itr).isNumber() || (*itr)==QLatin1Char( '-' )
        || (*itr)==QLatin1Char( '+' ) || (*itr)==QLatin1Char( '.' ) ) {
        temp=QString();
        if ( (*itr)==QLatin1Char( '-' ) ) temp+=*itr++;
        else if ( (*itr)==QLatin1Char( '+' ) ) temp+=*itr++;
        while ( (*itr).isDigit() ) temp+=*itr++;
        if ( (*itr)==QLatin1Char( '.' ) ) temp+=*itr++;
        while ( (*itr).isDigit() ) temp+=*itr++;
        while ( (*itr).isSpace() ) ++itr;
        if ( (*itr)==QLatin1Char( ',' ) ) ++itr;
        points.append( temp.toDouble() );
        //eat spaces
        while ( (*itr).isSpace() ) ++itr;
    }
    return points;
}

QColor PHI::colorFromString( const QString &name )
{
    QString::const_iterator itr=name.constBegin();
    QList<qreal> compo;
    if ( name.startsWith( QStringLiteral( "rgba(" ) ) ) {
        ++itr; ++itr; ++itr; ++itr; ++itr;
        compo=PHI::parseNumbersList( itr );
        if ( compo.size()!=4 ) {
            return QColor();
        }
        //alpha seems to be always between 0-1
        compo[3]*=255;
        return QColor( (int)compo[0], (int)compo[1], (int)compo[2], (int)compo[3]);
    } else if ( name.startsWith( SL( "rgb(" ) ) ) {
        ++itr; ++itr; ++itr; ++itr;
        compo=parseNumbersList( itr );
        if (compo.size() != 3) {
            return QColor();
        }
        return QColor( (int)PHIBOUND(compo[0], qreal(0), qreal(255)),
            (int)PHIBOUND(compo[1], qreal(0), qreal(255)),
            (int)PHIBOUND(compo[2], qreal(0), qreal(255)));
    } else {
        return QColor(name);
    }
}

QString PHI::colorToString( const QColor &c )
{
    QString s;
    if ( c.alpha()==255 ) s=SL( "rgb(" );
    else s=SL( "rgba(" );
    s+=QString::number( c.red() )+SL( ", " );
    s+=QString::number( c.green() )+SL( ", " );
    s+=QString::number( c.blue() );
    if ( c.alpha()==255 ) return s+=QLatin1Char( ')' );
    return s+=SL( ", " )+QString::number( c.alphaF(), 'f', 3 )+QLatin1Char( ')' );
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

QByteArray PHI::toCursorString( Qt::CursorShape s )
{
    if ( s==Qt::CrossCursor ) return BL( "crosshair" );
    if ( s==Qt::ArrowCursor ) return BL( "default" );
    if ( s==Qt::SizeHorCursor ) return BL( "e-resize" );
    if ( s==Qt::WhatsThisCursor ) return BL( "help" );
    if ( s==Qt::OpenHandCursor ) return BL( "move" );
    if ( s==Qt::SizeVerCursor ) return BL( "n-resize" );
    if ( s==Qt::SizeBDiagCursor ) return BL( "ne-resize" );
    if ( s==Qt::SizeFDiagCursor ) return BL( "nw-resize" );
    if ( s==Qt::PointingHandCursor ) return BL( "pointer" );
    if ( s==Qt::BusyCursor ) return BL( "progress" );
    if ( s==Qt::SizeVerCursor ) return BL( "s-resize" );
    if ( s==Qt::SizeFDiagCursor ) return BL( "se-resize" );
    if ( s==Qt::SizeBDiagCursor ) return BL( "sw-resize" );
    if ( s==Qt::IBeamCursor ) return BL( "text" );
    if ( s==Qt::SizeHorCursor ) return BL( "w-resize" );
    if ( s==Qt::WaitCursor ) return BL( "wait" );
    return BL( "default" );
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
    if ( name==L1( "linear" ) ) return QEasingCurve::Linear;
    if ( name==L1( "easeInQuad" ) ) return QEasingCurve::InQuad;
    if ( name==L1( "easeOutQuad" ) ) return QEasingCurve::OutQuad;
    if ( name==L1( "easeInOutQuad" ) ) return QEasingCurve::InOutQuad;
    if ( name==L1( "easeInCubic" ) ) return QEasingCurve::InCubic;
    if ( name==L1( "easeOutCubic" ) ) return QEasingCurve::OutCubic;
    if ( name==L1( "easeInOutCubic" ) ) return QEasingCurve::InOutCubic;
    if ( name==L1( "easeInQuart" ) ) return QEasingCurve::InQuart;
    if ( name==L1( "easeOutQuart" ) ) return QEasingCurve::OutQuart;
    if ( name==L1( "easeInOutQuart" ) ) return QEasingCurve::InOutQuart;
    if ( name==L1( "easeInQuint" ) ) return QEasingCurve::InQuint;
    if ( name==L1( "easeOutQuint" ) ) return QEasingCurve::OutQuint;
    if ( name==L1( "easeInOutQuint" ) ) return QEasingCurve::InOutQuint;
    if ( name==L1( "easeInSine" ) ) return QEasingCurve::InSine;
    if ( name==L1( "easeOutSine" ) ) return QEasingCurve::OutSine;
    if ( name==L1( "easeInOutSine" ) ) return QEasingCurve::InOutSine;
    if ( name==L1( "easeInExpo" ) ) return QEasingCurve::InExpo;
    if ( name==L1( "easeOutExpo" ) ) return QEasingCurve::OutExpo;
    if ( name==L1( "easeInOutExpo" ) ) return QEasingCurve::InOutExpo;
    if ( name==L1( "easeInCirc" ) ) return QEasingCurve::InCirc;
    if ( name==L1( "easeOutCirc" ) ) return QEasingCurve::OutCirc;
    if ( name==L1( "easeInOutCirc" ) ) return QEasingCurve::InOutCirc;
    if ( name==L1( "easeInElastic" ) ) return QEasingCurve::InElastic;
    if ( name==L1( "easeOutElastic" ) ) return QEasingCurve::OutElastic;
    if ( name==L1( "easeInOutElastic" ) ) return QEasingCurve::InOutElastic;
    if ( name==L1( "easeInBack" ) ) return QEasingCurve::InBack;
    if ( name==L1( "easeOutBack" ) ) return QEasingCurve::OutBack;
    if ( name==L1( "easeInOutBack" ) ) return QEasingCurve::InOutBack;
    if ( name==L1( "easeInBounce" ) ) return QEasingCurve::InBounce;
    if ( name==L1( "easeOutBounce" ) ) return QEasingCurve::OutBounce;
    if ( name==L1( "easeInOutBounce" ) ) return QEasingCurve::InOutBounce;
    return QEasingCurve::OutQuad;
}

QStringList PHI::availableEasingCurves()
{
    static QStringList list;
    if ( list.count() ) return list;
    list << QStringLiteral( "linear" )
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

*/
