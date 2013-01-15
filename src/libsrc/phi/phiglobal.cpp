/*
#    Copyright (C) 2008  Trolltech AS
#    Copyright (C) 2010-2012  Marius B. Schumacher
#    Copyright (C) 2011-2012  Phisys AG, Switzerland
#    Copyright (C) 2012  Phisketeer.org team
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
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QObject>
#include <QMutex>
#include <QMetaObject>
#include <QMetaProperty>
#include <QFileInfo>
#include <QFile>
#include <QBuffer>
#include <QUuid>
#include <QPainter>
#include <QPalette>
#include <QTransform>
#include <QLinearGradient>
#include <QSettings>
#include <QTranslator>
#include <QMetaType>
#include <QLocale>
#include <QGraphicsBlurEffect>
#include <QGraphicsColorizeEffect>
#include <QGraphicsDropShadowEffect>
#include <QThread>
#include <QDir>
#include <QProcess>
#include <QApplication>
#include <QUrlQuery>

#ifdef Q_OS_MAC
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#endif

#include "qmath.h"
#include "phi.h"
#include "phierr.h"
#include "phisysinfo.h"
#include "phieffect.h"
#include "phieffects.h"
#include "phimemrotate.h"

const char* PHI::_phiDT="yyyyMMddHHmmsszzz";
const char* PHI::_phiDate="yyyy-MM-dd";
const char* PHI::_phiMimeType="application/x-phi";
const char* PHI::_phiDom=PHIDOM;
const char* PHI::_phiOrg=PHIORG;
QHash <QByteArray, QByteArray> PHI::_mimeTypes;
//QTranslator PHI::_translator;

QMutex PHISysInfo::_mutex;

PHISysInfo::~PHISysInfo()
{
    qDebug( "PHISysInfo::~PHISysInfo()" );
}

// Code of the templates are based on Qt's image rendering algorithm
template <int shift>
inline int phi_static_shift(int value)
{
    if (shift == 0)
        return value;
    else if (shift > 0)
        return value << (uint(shift) & 0x1f);
    else
        return value >> (uint(-shift) & 0x1f);
}

const int phi_alphaIndex = (QSysInfo::ByteOrder == QSysInfo::BigEndian ? 0 : 3);

template<int aprec, int zprec>
inline void phi_blurinner_alphaOnly(uchar *bptr, int &z, int alpha)
{
    const int A_zprec = int(*(bptr)) << zprec;
    const int z_zprec = z >> aprec;
    z += alpha * (A_zprec - z_zprec);
    *(bptr) = z >> (zprec + aprec);
}

template<int aprec, int zprec>
inline void phi_blurinner(uchar *bptr, int &zR, int &zG, int &zB, int &zA, int alpha)
{
    QRgb *pixel = (QRgb *)bptr;

#define Z_MASK (0xff << zprec)
    const int A_zprec = phi_static_shift<zprec - 24>(*pixel) & Z_MASK;
    const int R_zprec = phi_static_shift<zprec - 16>(*pixel) & Z_MASK;
    const int G_zprec = phi_static_shift<zprec - 8>(*pixel)  & Z_MASK;
    const int B_zprec = phi_static_shift<zprec>(*pixel)      & Z_MASK;
#undef Z_MASK

    const int zR_zprec = zR >> aprec;
    const int zG_zprec = zG >> aprec;
    const int zB_zprec = zB >> aprec;
    const int zA_zprec = zA >> aprec;

    zR += alpha * (R_zprec - zR_zprec);
    zG += alpha * (G_zprec - zG_zprec);
    zB += alpha * (B_zprec - zB_zprec);
    zA += alpha * (A_zprec - zA_zprec);

#define ZA_MASK (0xff << (zprec + aprec))
    *pixel =
        phi_static_shift<24 - zprec - aprec>(zA & ZA_MASK)
        | phi_static_shift<16 - zprec - aprec>(zR & ZA_MASK)
        | phi_static_shift<8 - zprec - aprec>(zG & ZA_MASK)
        | phi_static_shift<-zprec - aprec>(zB & ZA_MASK);
#undef ZA_MASK
}

template<int aprec, int zprec, bool alphaOnly>
inline void phi_blurrow(QImage & im, int line, int alpha)
{
    uchar *bptr = im.scanLine(line);

    int zR = 0, zG = 0, zB = 0, zA = 0;

    if (alphaOnly && im.format() != QImage::Format_Indexed8)
        bptr += phi_alphaIndex;

    const int stride = im.depth() >> 3;
    const int im_width = im.width();
    for (int index = 0; index < im_width; ++index) {
        if (alphaOnly)
            phi_blurinner_alphaOnly<aprec, zprec>(bptr, zA, alpha);
        else
            phi_blurinner<aprec, zprec>(bptr, zR, zG, zB, zA, alpha);
        bptr += stride;
    }

    bptr -= stride;

    for (int index = im_width - 2; index >= 0; --index) {
        bptr -= stride;
        if (alphaOnly)
            phi_blurinner_alphaOnly<aprec, zprec>(bptr, zA, alpha);
        else
            phi_blurinner<aprec, zprec>(bptr, zR, zG, zB, zA, alpha);
    }
}

template <int aprec, int zprec, bool alphaOnly>
void phi_expblur(QImage &img, qreal radius, bool improvedQuality = false, int transposed = 0)
{
    // half the radius if we're using two passes
    if (improvedQuality)
        radius *= qreal(0.5);

    Q_ASSERT(img.format() == QImage::Format_ARGB32_Premultiplied
             || img.format() == QImage::Format_RGB32
             || img.format() == QImage::Format_Indexed8);

    // choose the alpha such that pixels at radius distance from a fully
    // saturated pixel will have an alpha component of no greater than
    // the cutOffIntensity
    const qreal cutOffIntensity = 2;
    int alpha = radius <= qreal(1e-5)
        ? ((1 << aprec)-1)
        : qRound((1<<aprec)*(1 - qPow(cutOffIntensity * (1 / qreal(255)), 1 / radius)));

    int img_height = img.height();
    for (int row = 0; row < img_height; ++row) {
        for (int i = 0; i <= static_cast<int>(improvedQuality); ++i)
            phi_blurrow<aprec, zprec, alphaOnly>(img, row, alpha);
    }

    QImage temp(img.height(), img.width(), img.format());
    if (transposed >= 0) {
        if (img.depth() == 8) {
            phi_memrotate270(reinterpret_cast<const quint8*>(img.bits()),
                            img.width(), img.height(), img.bytesPerLine(),
                            reinterpret_cast<quint8*>(temp.bits()),
                            temp.bytesPerLine());
        } else {
            phi_memrotate270(reinterpret_cast<const quint32*>(img.bits()),
                            img.width(), img.height(), img.bytesPerLine(),
                            reinterpret_cast<quint32*>(temp.bits()),
                            temp.bytesPerLine());
        }
    } else {
        if (img.depth() == 8) {
            phi_memrotate90(reinterpret_cast<const quint8*>(img.bits()),
                           img.width(), img.height(), img.bytesPerLine(),
                           reinterpret_cast<quint8*>(temp.bits()),
                           temp.bytesPerLine());
        } else {
            phi_memrotate90(reinterpret_cast<const quint32*>(img.bits()),
                           img.width(), img.height(), img.bytesPerLine(),
                           reinterpret_cast<quint32*>(temp.bits()),
                           temp.bytesPerLine());
        }
    }

    img_height = temp.height();
    for (int row = 0; row < img_height; ++row) {
        for (int i = 0; i <= static_cast<int>(improvedQuality); ++i)
            phi_blurrow<aprec, zprec, alphaOnly>(temp, row, alpha);
    }

    if (transposed == 0) {
        phi_memrotate90( reinterpret_cast<const quint32*>(temp.bits()),
           temp.width(), temp.height(), temp.bytesPerLine(),
           reinterpret_cast<quint32*>(img.bits()),
           img.bytesPerLine() );
    } else {
        img = temp;
    }
}

void phi_blurImage( QImage &blurImage, qreal radius, bool quality, int transposed=0 )
{
    if ( blurImage.format() == QImage::Format_Indexed8 )
        phi_expblur<12, 10, true>( blurImage, radius, quality, transposed );
    else
        phi_expblur<12, 10, false>( blurImage, radius, quality, transposed );
}

PHI::PHI()
{
    qDebug( "PHI::PHI()" );
}

PHI::~PHI()
{
    qDebug( "PHI::~PHI()" );
}

QString PHI::libVersion()
{
    return QString( PHIVERSION );
}

QSettings* PHI::globalSettings()
{
    static QSettings *s=0;
    if ( s ) return s;
#ifdef Q_OS_MAC
    s=new QSettings( "phisys.com", "phis", qApp ); // required for Mac app store
    qDebug( "Application name %s", qPrintable( qApp->applicationName() ) );
    qDebug( "Application domain %s", qPrintable( qApp->organizationDomain() ) );
    qDebug( "Application organisation %s", qPrintable( qApp->organizationName() ) );
#else
    s=new QSettings( QSettings::SystemScope, PHI::organisation(), "phis", qApp );
    s->setFallbacksEnabled( false );
#endif
    qDebug( "Global settings file name: %s", qPrintable( s->fileName() ) );
    return s;
}

// returns the (compiled) root directory for the application
QString PHI::compiledPrefix()
{
#ifdef Q_OS_WIN
    if ( QFile::exists( "C:\\Program Files (x86)" ) ) {
        return QString( "C:\\Program Files (x86)\\" )+QString( PHIORG );
    }
    return QString( "C:\\Program Files\\" )+QString( PHIORG );
#elif defined Q_OS_UNIX
#ifdef Q_OS_MAC
    QDir appdir( QCoreApplication::applicationDirPath() ); // bundleID/Contents/MacOS
    qDebug( "PATH:%s", qPrintable( appdir.canonicalPath() ) );
    appdir.cdUp(); // bundleID/Contents/
    appdir.cdUp(); // bundleID/
    return appdir.canonicalPath();
#else
    return QString( "/opt/"+QString( PHIORG ).toLower() );
#endif
#else
#error Could not determine compiledPrefix
#endif
}

// Documents root directory for Phis server
QString PHI::compiledRoot()
{
#ifdef Q_OS_WIN
    return compiledPrefix()+QDir::separator()+QString( "phis" );
#elif defined Q_OS_MAC
    return QString( QDir::homePath()+"/Library/Artephis/phis" ); // required for the app store
#else
    return QString( "/var/phis" );
#endif
}

// Binary directory
QString PHI::compiledBin()
{
#ifdef Q_OS_WIN
    return compiledPrefix()+QDir::separator()+QString( "bin" );
#elif defined Q_OS_UNIX
#ifdef Q_OS_MAC
    return QCoreApplication::applicationDirPath();
#else
    return compiledPrefix()+QString( "/bin" );
#endif
#else
#error Could not determine compiledBin
#endif
}

void PHI::updateTranslations()
{
    QCoreApplication *app=QCoreApplication::instance();
    if ( !app ) return;
    QString lang=QLocale::system().name();
    QSettings s;
    lang=s.value( "Language", lang ).toString();
    QLocale::setDefault( QLocale( lang ) );
    qDebug( "SETTING LOCALE %s", qPrintable( lang ) );

    QString dir=app->applicationDirPath()+QDir::separator()+".."+QDir::separator()+"ts";
    if ( !QFile::exists( dir ) ) {
#ifdef Q_OS_MAC
        dir=compiledPrefix()+"/Contents/Resources/ts";
#else
        dir=compiledPrefix()+QDir::separator()+"ts";
#endif
        if ( !QFile::exists( dir ) ) {
            QSettings *gs=PHI::globalSettings();
            dir=gs->value( "BinDir", compiledBin() ).toString()+QDir::separator()+".."+QDir::separator()+"ts";
        }
    }
    QTranslator *tr=new QTranslator( app );
    if ( !tr->load( "qt_"+lang, dir ) ) {
        qDebug( "Could not load %s in %s", qPrintable( "qt_"+lang ), qPrintable( dir ) );
        delete tr;
    } else {
        app->installTranslator( tr );
    }
    tr=new QTranslator( app );
    if ( !tr->load( "phia_"+lang, dir ) ) {
        qDebug( "Could not load %s in %s", qPrintable( "phia_"+lang ), qPrintable( dir ) );
        delete tr;
    } else {
        app->installTranslator( tr );
    }
    tr=new QTranslator( app );
    if ( !tr->load( app->applicationName().toLower()+"_"+lang, dir ) ) {
        qDebug( "Could not load %s in %s", qPrintable( app->applicationName().toLower()+"_"+lang ), qPrintable( dir ) );
    } else {
        app->installTranslator( tr );
    }
}

QStringList PHI::properties( const QObject *obj )
{
    Q_ASSERT( obj );
    const QMetaObject *mo=obj->metaObject();
    QStringList properties;
    for( int i=mo->propertyOffset(); i < mo->propertyCount(); ++i )
        properties << QString::fromLatin1( mo->property( i ).name() );
    return properties;
}

void PHI::setupApplication( QGuiApplication *app )
{
    if ( !app ) return;
#ifdef Q_OS_MAC // needed for app store
    app->setOrganizationDomain( "phisys.com" );
    app->setOrganizationName( "Phisys AG" );
#else
    app->setOrganizationDomain( QString::fromLatin1( PHI::domain() ) );
    app->setOrganizationName( QString::fromLatin1( PHI::organisation() ) );
#endif
    QSettings *s=PHI::globalSettings();

    QString pluginPath;
#ifdef Q_OS_UNIX
#ifdef Q_OS_MAC
    pluginPath=s->value( "PluginsPath", PHI::compiledPrefix()+"/Contents/PlugIns" ).toString();
#else
    pluginPath=s->value( "PluginsPath", PHI::compiledPrefix()+"/plugins" ).toString();
#endif
#elif defined Q_OS_WIN
    pluginPath=s->value( "PluginsPath", PHI::compiledPrefix()+"\\bin\\plugins" ).toString();
#else
#error Could not determine pluginPath
#endif
    app->addLibraryPath( pluginPath );
    qDebug( "Adding PluginPath %s", qPrintable( pluginPath ) );

    updateTranslations();

    qRegisterMetaTypeStreamOperators<PHIRectHash>("PHIRectHash");
    qRegisterMetaTypeStreamOperators<PHIByteArrayList>("PHIByteArrayList");
    qRegisterMetaTypeStreamOperators<PHIImageHash>("PHIImageHash");
    qRegisterMetaTypeStreamOperators<QGradientStops>("QGradientStops");
}

// returns -1 on error, 0 not running, 1 running
int PHI::checkService()
{
    QSettings *s=PHI::globalSettings();
#ifdef Q_OS_MAC
    // Sandboxing in Mac OS X requires a little hack: we check if the server responds
    // to a QNetworkRequest to determine if the phis service is running
    s->beginGroup( "default" );
    QUrl url( "http://localhost/phi.phis?ping=1" );
    url.setPort( s->value( "ListenerPort", 8080 ).toInt() );
    s->endGroup();
    QNetworkRequest req;
    req.setUrl( url );
    QNetworkAccessManager mg;
    QNetworkReply *rep=mg.get( req );
    while ( !rep->isFinished() ) {
        qApp->processEvents();
    }
    rep->deleteLater();
    if ( rep->error()!=QNetworkReply::NoError ) {
        // phis responses with "access denied" to phi.phis?ping=1
        // so we can asume the service is running
        if( rep->error()==QNetworkReply::ContentOperationNotPermittedError ) return 1;
        qDebug( "REPLY %d %s", rep->error(), qPrintable( rep->errorString() ) );
        return 0;
    }
    return 0;
#else // Linux and Windows
    QString bin;
#ifdef Q_OS_WIN
    bin=s->value( "BinDir", qApp->applicationDirPath() ).toString()+"\\phis.exe";
#else
    bin=s->value( "BinDir", qApp->applicationDirPath() ).toString()+"/phis";
#endif
    if ( !QFile::exists( bin ) ) return -1;
    qDebug( "Starting process %s", qPrintable( bin ) );
    QProcess proc;
    proc.setProcessChannelMode( QProcess::MergedChannels );
    proc.start( bin, QStringList() << "-v" );
    if ( !proc.waitForStarted() ) return -1;
    if ( !proc.waitForFinished() ) return -1;
    QByteArray arr=proc.readAll();
    qDebug( "arr=%s", arr.data() );
    if ( arr.contains( "not running" ) ) return 0;
    return 1;
#endif
}

bool PHI::startPhisService()
{
    QString bin=PHI::globalSettings()->value( "BinDir", qApp->applicationDirPath() ).toString()
        +QDir::separator()+"phis";
#ifdef Q_OS_MAC
    return QProcess::startDetached( bin );
#else
    QProcess proc;
#ifdef Q_OS_WIN
    proc.execute( bin, QStringList() << "-i" );
#endif
    proc.start( bin );
    if ( !proc.waitForStarted() ) return false;
    if ( !proc.waitForFinished() ) return false;
    return true;
#endif
}

bool PHI::stopPhisService()
{
    qDebug( "stop phis service" );
#ifdef Q_OS_MAC
    // Sandboxing in Mac OS X requires a little hack: we send
    // a QNetworkRequest to stop the phis service
    QSettings *s=PHI::globalSettings();
    s->beginGroup( "default" );
    QUrl url( "http://localhost/phi.phis?stop=1" );
    url.setPort( s->value( "ListenerPort", 8080 ).toInt() );
    s->endGroup();
    QNetworkRequest req;
    req.setUrl( url );
    QNetworkAccessManager mg;
    QNetworkReply *rep=mg.get( req );
    while ( !rep->isFinished() ) {
        qApp->processEvents();
    }
    rep->deleteLater();
    if ( rep->error()!=QNetworkReply::NoError ) {
        // phis responses with "access denied" to phi.phis?stop=1
        if( rep->error()==QNetworkReply::ContentOperationNotPermittedError ) return 1;
        qWarning( "REPLY %d %s", rep->error(), qPrintable( rep->errorString() ) );
        return 0;
    }
    qWarning( "service stopped" );
    return 0;
#else
    QProcess proc;
    QString bin=PHI::globalSettings()->value( "BinDir", qApp->applicationDirPath() ).toString()
        +QDir::separator()+"phis";
    int res=proc.execute( bin, QStringList() << "-t" );
    if ( res ) return false;
    return true;
#endif
}

bool PHI::clearPhisServiceCache()
{
#ifdef Q_OS_MAC
    // sandboxing in Mac OS X requires a little hack: we send
    // a QNetworkRequest to invalidate the phis cache
    QSettings *s=PHI::globalSettings();
    s->beginGroup( "default" );
    QUrl url( "http://localhost/phi.phis?invalidate=1" );
    url.setPort( s->value( "ListenerPort", 8080 ).toInt() );
    s->endGroup();
    QNetworkRequest req;
    req.setUrl( url );
    QNetworkAccessManager mg;
    QNetworkReply *rep=mg.get( req );
    while ( !rep->isFinished() ) {
        qApp->processEvents();
    }
    rep->deleteLater();
    if ( rep->error()!=QNetworkReply::NoError ) {
        // phis responses with "access denied" to phi.phis?invalidate=1
        if( rep->error()==QNetworkReply::ContentOperationNotPermittedError ) return 1;
        qDebug( "REPLY %d %s", rep->error(), qPrintable( rep->errorString() ) );
        return 0;
    }
    return 0;
#else
    QProcess proc;
    QString bin=PHI::globalSettings()->value( "BinDir", qApp->applicationDirPath() ).toString()
        +QDir::separator()+"phis";
    int res=proc.execute( bin, QStringList() << "-c" );
    if ( res ) return false;
    return true;
#endif
}

QSize PHI::defaultPageSize()
{
    return QSize( 1000, 750 );
}

QString PHI::tag( const QString &tag, const QString &msg )
{
    // note: QRegExp is too slow!!
    QString startTag( '<'+tag+'>' );
    QString endTag( "</"+tag+'>' );
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
    id+=".png";
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

QDateTime PHI::dateTimeFromHeader( const QByteArray &modified )
{
    QByteArray tmp=modified.mid( 5 ), arr;
    if ( tmp.endsWith( " GMT" ) ) arr=tmp.left( tmp.length()-4 );
    else arr=tmp;
    tmp=arr.right( 8 );
    QTime time=QTime::fromString( tmp, "hh:mm:ss" ).addMSecs( 999 );
    if ( !time.isValid() ) {
        qDebug( "------------------>>>> If-Modified-Since is invalid" );
        return QDateTime( QDate( 1970, 1, 1 ) );
    }
    if ( arr.length()==20 ) {
        tmp=arr.left( 2 );
        arr.remove( 0, 3 );
    } else {
        tmp=arr.left( 1 );
        arr.remove( 0, 2 );
    }
    arr.chop( 9 );
    int d=tmp.toInt();
    int y=arr.right( 4 ).toInt();
    int m=1;
    arr.chop( 5 );
    if ( arr=="Feb" ) m=2;
    else if ( arr=="Mar" ) m=3;
    else if ( arr=="Apr" ) m=4;
    else if ( arr=="May" ) m=5;
    else if ( arr=="Jun" ) m=6;
    else if ( arr=="Jul" ) m=7;
    else if ( arr=="Aug" ) m=8;
    else if ( arr=="Sep" ) m=9;
    else if ( arr=="Oct" ) m=10;
    else if ( arr=="Nov" ) m=11;
    else if ( arr=="Dec" ) m=12;
    //qDebug( "DATETIME %s", qPrintable( QDateTime( QDate( y, m, d ), time, Qt::UTC ).toString() ) );
    return QDateTime( QDate ( y, m, d ), time, Qt::UTC );
}

QUrl PHI::createUrlForLink( const QUrl &ref, const QString &l )
{
    QUrl url=ref;
    //qDebug( "ref path: %s", qPrintable( url.path()) );
    QUrl link( l );
    if ( !link.scheme().isEmpty() ) url.setScheme( link.scheme() );
    if ( !link.host().isEmpty() ) {
        url.setHost( link.host() );
        url.setPort( -1 );
    }
    if ( !link.userInfo().isEmpty() ) url.setUserInfo( link.userInfo() );
    if ( link.port()!=-1 ) url.setPort( link.port() );
    if ( l.startsWith( '/' ) || l.startsWith( '\\' ) || !link.host().isEmpty() ) {
        url.setPath( link.path() );
        //qDebug( "path: %s", qPrintable( url.path() ) );
    } else { // we have a relative Url
        QString path=QFileInfo( ref.path() ).path();
        qDebug( "path: %s link: %s", qPrintable( ref.path() ), qPrintable( link.path() ) );
        link.path().startsWith( '/' ) ? path+=link.path() : path+='/'+link.path();
        if ( path.startsWith( '/' ) || path.startsWith( '\\' ) ) path.remove( 0, 1 );
        url.setPath( path );
    }
    url.setQuery( QUrlQuery( link ) );
    qDebug( "createUrlForLink <%s>", url.toEncoded().data() );
    return url;
}

QString PHI::getLocalFilePath( const QString &docroot, const QString &referer, const QString &filename )
{
    // we have an absolute file path related to documentroot:
    if ( filename.startsWith( '/' ) || filename.startsWith( '\\' ) )
        return QFileInfo( docroot+filename ).canonicalFilePath();
    QString path=QFileInfo( referer ).canonicalPath();
    return QFileInfo( path+QDir::separator()+filename ).canonicalFilePath();
}

QString PHI::getRelativeFilePath( const QString &referer, const QString &filename )
{
    QString path=QFileInfo( referer ).canonicalPath();
    QString file=QFileInfo( filename ).canonicalFilePath();
    file.replace( path, "" );
    if ( file.startsWith( '/' ) || file.startsWith( '\\' ) ) file.remove( 0, 1 );
    return file;
}

QString PHI::linkStyleSheet( const QPalette &pal, bool underline )
{
    QString a=QString( "a:link { text-decoration: none; color: %1; }\n"
        "a:visited { text-decoration: none; color: %2; }\na:hover { text-decoration: %3; }\n" )
        .arg( pal.color( QPalette::Link ).name() ).arg( pal.color( QPalette::LinkVisited ).name() )
        .arg( underline ? QString( "underline" ) : QString( "none" ) );
    return a;
}

void PHI::getItemCheckData( QString &data, QString &opt, bool &isChecked )
{
    isChecked=false;
    opt="";
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
                QString selected=data.mid( startsel+1, end-startsel-1 ).toLower();
                if ( selected.toInt() || selected=="true" ) isChecked=true;
            }
        }
        data.truncate( start );
    }
}

QImage PHI::getBluredImage( const QImage &img, qreal radius, qreal factor )
{
    if ( img.isNull() || radius <=1. ) return img;
    int r=static_cast<int>(radius);
    QImage dest=QImage( img.width()+r*2, img.height()+r*2, QImage::Format_ARGB32_Premultiplied );
    dest.fill( 0 );

    QPainter p( &dest );
    p.drawImage( r, r, img );
    p.end();

    phi_blurImage( dest, radius*factor, true, 0 );
    return dest;
}

QImage PHI::getShadowedImage( const QImage &img, const QColor &color, qreal radius,
    qreal xOff, qreal yOff )
{
    QImage tmp=PHI::getBluredImage( img, radius, 1.3 );
    QPainter tmpPainter( &tmp );
    //qDebug( "bluring org %d %d, new %d %d", img.width(), img.height(), tmp.width(), tmp.height() );

    // blacken the image...
    tmpPainter.setCompositionMode( QPainter::CompositionMode_SourceIn );
    tmpPainter.fillRect( tmp.rect(), color );
    tmpPainter.end();

    if ( radius<=1. ) radius=0;
    QImage dest=QImage( img.width()+qAbs(static_cast<int>(xOff))+2*radius, img.height()
            +qAbs(static_cast<int>(yOff))+2*radius, QImage::Format_ARGB32_Premultiplied );
    dest.fill( 0 );
    QPainter destPainter( &dest );
    qreal xoff=0, yoff=0;
    if ( xOff>0 ) xoff=xOff;
    if ( yOff>0 ) yoff=yOff;
    // draw the blurred drop shadow...
    destPainter.drawImage( xoff, yoff, tmp );

    if ( xOff>=0 ) xoff=radius;
    else xoff=-xOff+radius;
    if ( yOff>=0 ) yoff=radius;
    else yoff=-yOff+radius;
    // Draw the actual pixmap...
    destPainter.drawImage( xoff, yoff, img );
    destPainter.end();
    return dest;
}

QImage PHI::getColorizedImage( const QImage &img, const QColor &c, qreal strength )
{
    if ( img.isNull() ) return img;
    QImage src=img;
    if ( img.format()!=QImage::Format_ARGB32 && img.format()!=QImage::Format_ARGB32_Premultiplied )
        src=img.convertToFormat( QImage::Format_ARGB32_Premultiplied );
    QImage destImage( img.width(), img.height(), QImage::Format_ARGB32_Premultiplied );

    // do colorizing
    QPainter destPainter( &destImage );
    PHI::grayscale( src, destImage, src.rect() );
    destPainter.setCompositionMode( QPainter::CompositionMode_Screen );
    destPainter.fillRect( src.rect(), c );
    destPainter.end();

    if ( strength<1. ) {
        QImage buffer=img;
        QPainter bufPainter( &buffer );
        bufPainter.setOpacity( strength );
        bufPainter.drawImage( 0, 0, destImage );
        bufPainter.end();
        destImage=buffer;
    }
    if ( img.hasAlphaChannel() ) destImage.setAlphaChannel( img.alphaChannel() );
    return destImage;
}

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

QImage PHI::getSurfacedImage( const QImage &img, qreal off, qreal size )
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

void PHI::createMimeTypes( QString &filename )
{
    QBuffer buf;
    QIODevice *io;
    qDebug( "PHI::createMimeTpyes: filename=%s", qPrintable( filename ) );
    QFile f( filename );
    if ( !f.open( QIODevice::ReadOnly ) ) {
        buf.setData( PHI::stdMimeTypes() );
        buf.open( QIODevice::ReadOnly );
        io=&buf;
        filename=QObject::tr( "build in mime types" );
    } else io=&f;
    _mimeTypes.clear();
    QByteArray tmp, key, value;
    int start, end;
    while ( !io->atEnd() ) {
        tmp=io->readLine().simplified();
        if ( tmp.isEmpty() || tmp.startsWith( '#' ) ) continue;
        value=tmp.left( start=tmp.indexOf( ' ' ) );
        if ( start==-1 ) continue;
        end=tmp.indexOf( ' ', start+1 );
        while ( end!=-1 ) {
            key=tmp.mid( start+1, end-start-1 );
            _mimeTypes.insert( key, value );
            start=end;
            end=tmp.indexOf( ' ', start+1 );
        }
        key=tmp.mid( start+1 );
        _mimeTypes.insert( key, value );
    }
    io->close();
}

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
    if ( name=="linear" ) return QEasingCurve::Linear;
    if ( name=="easeInQuad" ) return QEasingCurve::InQuad;
    if ( name=="easeOutQuad" ) return QEasingCurve::OutQuad;
    if ( name=="easeInOutQuad" ) return QEasingCurve::InOutQuad;
    if ( name=="easeInCubic" ) return QEasingCurve::InCubic;
    if ( name=="easeOutCubic" ) return QEasingCurve::OutCubic;
    if ( name=="easeInOutCubic" ) return QEasingCurve::InOutCubic;
    if ( name=="easeInQuart" ) return QEasingCurve::InQuart;
    if ( name=="easeOutQuart" ) return QEasingCurve::OutQuart;
    if ( name=="easeInOutQuart" ) return QEasingCurve::InOutQuart;
    if ( name=="easeInQuint" ) return QEasingCurve::InQuint;
    if ( name=="easeOutQuint" ) return QEasingCurve::OutQuint;
    if ( name=="easeInOutQuint" ) return QEasingCurve::InOutQuint;
    if ( name=="easeInSine" ) return QEasingCurve::InSine;
    if ( name=="easeOutSine" ) return QEasingCurve::OutSine;
    if ( name=="easeInOutSine" ) return QEasingCurve::InOutSine;
    if ( name=="easeInExpo" ) return QEasingCurve::InExpo;
    if ( name=="easeOutExpo" ) return QEasingCurve::OutExpo;
    if ( name=="easeInOutExpo" ) return QEasingCurve::InOutExpo;
    if ( name=="easeInCirc" ) return QEasingCurve::InCirc;
    if ( name=="easeOutCirc" ) return QEasingCurve::OutCirc;
    if ( name=="easeInOutCirc" ) return QEasingCurve::InOutCirc;
    if ( name=="easeInElastic" ) return QEasingCurve::InElastic;
    if ( name=="easeOutElastic" ) return QEasingCurve::OutElastic;
    if ( name=="easeInOutElastic" ) return QEasingCurve::InOutElastic;
    if ( name=="easeInBack" ) return QEasingCurve::InBack;
    if ( name=="easeOutBack" ) return QEasingCurve::OutBack;
    if ( name=="easeInOutBack" ) return QEasingCurve::InOutBack;
    if ( name=="easeInBounce" ) return QEasingCurve::InBounce;
    if ( name=="easeOutBounce" ) return QEasingCurve::OutBounce;
    if ( name=="easeInOutBounce" ) return QEasingCurve::InOutBounce;
    return QEasingCurve::OutQuad;
}

QStringList PHI::availableEasingCurves()
{
    QStringList list;
    list
    << "linear"
    << "easeInQuad"
    << "easeOutQuad"
    << "easeInOutQuad"
    << "easeInCubic"
    << "easeOutCubic"
    << "easeInOutCubic"
    << "easeInQuart"
    << "easeOutQuart"
    << "easeInOutQuart"
    << "easeInQuint"
    << "easeOutQuint"
    << "easeInOutQuint"
    << "easeInSine"
    << "easeOutSine"
    << "easeInOutSine"
    << "easeInExpo"
    << "easeOutExpo"
    << "easeInOutExpo"
    << "easeInCirc"
    << "easeOutCirc"
    << "easeInOutCirc"
    << "easeInElastic"
    << "easeOutElastic"
    << "easeInOutElastic"
    << "easeInBack"
    << "easeOutBack"
    << "easeInOutBack"
    << "easeInBounce"
    << "easeOutBounce"
    << "easeInOutBounce";
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
    int lm=l.replace( QRegExp( "\\..*" ), "" ).toInt();
    int sm=s.replace( QRegExp( "\\..*" ), "" ).toInt();
    if ( lm > sm ) return true;
    if ( lm < sm ) return false;
    l=localV.mid( l.length()+1 );
    s=serverV.mid( s.length()+1 );
    lm=l.replace( QRegExp( "\\..*" ), "" ).toInt();
    sm=s.replace( QRegExp( "\\..*" ), "" ).toInt();
    if ( lm > sm ) return true;
    if ( lm < sm ) return false;
    lm=localV.mid( localV.lastIndexOf( '.' )+1 ).toInt();
    sm=serverV.mid( serverV.lastIndexOf( '.')+1 ).toInt();
    return lm >= sm;
}

QString PHI::toLocale( const QString &lang )
{
    if ( lang.length()==2 ) return lang.left( 2 ).toLower();
    if ( lang.length()==5 ) return lang.left( 2 ).toLower()+'_'+lang.right( 2 ).toUpper();
    if ( lang.length()==6 ) return lang.left( 2 ).toLower()+'_'+lang.right( 3 ).toUpper();
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
    case PHI::LAYOUT_LOGIN:
    case PHI::LAYOUT_ADDRESS:
    case PHI::LAYOUT_DELIVERY:
    case PHI::LAYOUT_CREDIT_CARD:
    case PHI::LAYOUT_CONTACT:
    case PHI::LAYOUT_PERIOD:
    case PHI::LAYOUT_REGISTER:
    case PHI::HSPACE:
    case PHI::VSPACE:
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
        return true;
    default: ;
    }
    return false;
}

/* item is able to send data in a form */
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

/* item has a tab index */
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
    default: ;
    }
    return false;
}

/* represents text items ie. for text shadows (no box elment) */
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

/* Layout which contains other items */
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

/* Item represents a layout */
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

/* Item is not widget based */
bool PHI::isDisplayItem( PHI::Widget wid )
{
    switch ( wid ) {
    case PHI::ELLIPSE:
    case PHI::RECT:
    case PHI::ROUNDED_RECT:
    case PHI::LINE:
    case PHI::IMAGE:
    case PHI::GRAPH_TEXT:
    case PHI::TEXT:
    case PHI::SVG:
    case PHI::DIA_SHOW:
        return true;
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
