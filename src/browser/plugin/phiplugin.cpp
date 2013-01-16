/***************************************************************************
 *   $Id: omqconfig.cpp,v 1.3 2007/02/06 19:14:40 linuxprofi Exp $
 *   Copyright (c) 2008-2009 Phi Technologies, Bernd Schumacher
 ***************************************************************************/
#include <QApplication>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QNetworkRequest>
#include <QUrl>
#include <QProcess>
#include <QSettings>
#include <QIcon>
#include <QDesktopServices>
#include <QWebSettings>
#include <QStandardPaths>
#include "phiplugin.h"
#include "phianetmanager.h"
#include "phia.h"
#include "phiaconfig.h"

#ifdef PHIDEBUG
void redirectMsg( QtMsgType type, const QMessageLogContext &context, const QString &msg )
{
    Q_UNUSED( type );
    Q_UNUSED( context );
    AMMsg::instance()->sendMsg( msg );
}

AMMsg* AMMsg::_instance=0;

AMMsg::AMMsg( QObject *parent )
    : QObject( parent )
{
}

AMMsg::~AMMsg()
{
}

AMMsg* AMMsg::instance()
{
    if ( _instance ) return _instance;
    _instance=new AMMsg( qApp );
    return _instance;
}

void AMMsg::sendMsg( const QString &s )
{
    emit error( s );
}

#endif // PHIDEBUG

PhiPlugIn::PhiPlugIn( QWidget *parent )
    : QWidget( parent ), _phi( 0 )
{
#ifdef PHIDEBUG
    _edit=new QPlainTextEdit( "Start" );
    connect( AMMsg::instance(), SIGNAL( error( const QString& ) ), this,
        SLOT( slotError( const QString& ) ) );
#endif
    qDebug( "PhiPlugIn::PhiPlugIn()" );
    qApp->setApplicationName( PHIA::browserName() );
    qApp->setApplicationVersion( PHIA::libVersion() );
    PHI::setupApplication( qApp );

    QSettings s;
    //setWindowTitle( PHIA::browserName() );

    QString path=s.value( PHIA::configName( PHIA::CacheDirectory ) ).toString();
    if ( path.isEmpty() ) path=QStandardPaths::writableLocation( QStandardPaths::CacheLocation )
        +QDir::separator()+PHIA::phiaDir();
    QDir cacheDir( path );
    if ( !cacheDir.exists() ) cacheDir.mkpath( path );
    s.setValue( PHIA::configName( PHIA::CacheDirectory ), path );

    QWebSettings *ws=QWebSettings::globalSettings();
    ws->setOfflineStoragePath( path );
    ws->setIconDatabasePath( path );

    bool set=s.value( PHIA::configName( PHIA::AutoLoadImages ), true ).toBool();
    ws->setAttribute( QWebSettings::AutoLoadImages, set );
    set=s.value( PHIA::configName( PHIA::EnableJavascript ), true ).toBool();
    ws->setAttribute( QWebSettings::JavascriptEnabled, set );
    set=s.value( PHIA::configName( PHIA::JSOpenWindow ), true ).toBool();
    ws->setAttribute( QWebSettings::JavascriptCanOpenWindows, set );
    ws->setAttribute( QWebSettings::DeveloperExtrasEnabled, true );
    set=s.value( PHIA::configName( PHIA::EnableJava ), true ).toBool();
    ws->setAttribute( QWebSettings::JavaEnabled, set );
    set=s.value( PHIA::configName( PHIA::PrintBackgrounds ), true ).toBool();
    ws->setAttribute( QWebSettings::PrintElementBackgrounds, set );
    set=s.value( PHIA::configName( PHIA::EnablePlugins ), true ).toBool();
    ws->setAttribute( QWebSettings::PluginsEnabled, set );

    PHIANetManager *netManager=PHIANetManager::instance();
    PHIAAbstractWebView::setNetworkAccessManager( netManager->networkAccessManager() );
    connect( netManager->networkAccessManager(), SIGNAL( proxyAuthenticationRequired( const QNetworkProxy&,
        QAuthenticator* ) ), this, SLOT( slotProxyRequest( const QNetworkProxy&,
        QAuthenticator* ) ) );
    PHIAAbstractWebView::setDefaultIcon( ws->webGraphic( QWebSettings::DefaultFrameIconGraphic ) );
    PHIAAbstractWebView::setMissingIcon( ws->webGraphic( QWebSettings::MissingImageGraphic ) );
    qDebug( "PhiPlugIn::new PHIAWebView" );
    _view=new PHIAWebView( 0, this );
    _view->setWindowIcon( QIcon( QPixmap( ":/phi/logo" ) ) );

    QVBoxLayout *l=new QVBoxLayout();
    l->setContentsMargins( 0, 0, 0, 0 );
    l->addWidget( _view );
    setLayout( l );

    connect( _view, SIGNAL( loading( bool ) ), this, SLOT( slotLoading( bool ) ), Qt::QueuedConnection );
    connect( _view, SIGNAL( unsupportedContent( PHIAAbstractWebView*, QNetworkReply* ) ),
        this, SLOT( slotUnsupportedContent( PHIAAbstractWebView*, QNetworkReply* ) ) );
    connect( _view, SIGNAL( titleChanged( PHIAAbstractWebView* ) ),
        this, SLOT( slotTitleChanged( PHIAAbstractWebView* ) ) );
    connect( _view, SIGNAL( iconChanged( PHIAAbstractWebView* ) ),
        this, SLOT( slotIconChanged( PHIAAbstractWebView* ) ) );
    connect( _view, SIGNAL( geometryChangeRequested( const QRect&) ),
        this, SLOT( slotGeometryChanged( const QRect& ) ) );
    connect( _view, SIGNAL( urlChanged( const QUrl& ) ),
        this, SLOT( slotUrlChanged( const QUrl& ) ) );
    connect( _view, SIGNAL( linkHovered( const QString&, const QString&, const QString& ) ),
        this, SLOT( slotLinkHovered( const QString&, const QString&, const QString& ) ) );
}

PhiPlugIn::~PhiPlugIn()
{
#ifdef PHIDEBUG
    qInstallMessageHandler( 0 );
    delete _edit;
    _edit=0;
#endif
    qDebug( "PhiPlugIn::~PhiPlugIn()" );
}

void PhiPlugIn::setPhi( qint32 i )
{
     _phi=i;
    qWarning( "phiChanged(%d)", _phi );
    emit phiChanged( _phi );
}

void PhiPlugIn::slotError( const QString &s )
{
#ifdef PHIDEBUG
    if ( _edit->isVisible() ) _edit->appendPlainText( s );
#else
    qWarning( qPrintable( s ) );
#endif
}

void PhiPlugIn::slotTitleChanged( PHIAAbstractWebView *view )
{
    if ( !view ) return;
    qWarning( "titleChanged(%s)", qPrintable( view->title() ) );
    emit titleChanged( view->title() );
}

void PhiPlugIn::slotLinkHovered( const QString &l, const QString&, const QString& )
{
    qWarning( "linkHovered(%s)", qPrintable( l ) );
    emit linkHovered( l );
}

void PhiPlugIn::slotUrlChanged( const QUrl &url )
{
    Q_UNUSED( url );
    qWarning( "contentChanged(%s)", qPrintable( url.toString() ) );
    emit contentChanged( url.toString() );
}

void PhiPlugIn::slotIconChanged( PHIAAbstractWebView *view )
{
    if ( !view ) return;
    if ( view->icon().isNull() ) return;
    view->setWindowIcon( view->icon() );
    QUrl url=view->url();
    QString pageId=QFileInfo( url.path() ).baseName()+".ico";
    QString path=QString( "/phi.phis?phiimg=%1&phitmp=1" ).arg( pageId );
    qWarning( "iconPathChanged(%s)", qPrintable( path ) );
    emit iconPathChanged( path );
}

void PhiPlugIn::slotGeometryChanged( const QRect &geom )
{
    QRect r=geometry();
    if ( geom.x()>0 ) r.setX( geom.x() );
    if ( geom.y()>0 ) r.setY( geom.y() );
    if ( geom.x()==-1 ) r.setWidth( geometry().width()+geom.width() );
    else r.setWidth( geom.width() );
    if ( geom.y()==-1 ) r.setHeight( geometry().height()+geom.height() );
    else r.setHeight( geom.height() );
    if ( geom.x()==0 && geom.y()==0 ) resize( r.width(), r.height() );
    else setGeometry( r );
    emit geometryChanged( r.x(), r.y(), r.width(), r.height() );
}

void PhiPlugIn::slotLoading( bool b )
{
    if ( b ) qWarning( "Start loading %s", qPrintable( _view->url().toString() ) );
    else {
        qWarning( "Loading finished" );
        //qWarning( "contentChanged(%s)", qPrintable( _view->url().toString() ) );
        emit contentChanged( _view->url().toString() );
    }
}

void PhiPlugIn::setSource( const QString &src )
{
    QUrl url( src );
    _src=url.toString();
#ifdef PHIDEBUG
    if ( !QUrlQuery( url ).queryItemValue( "phidebug" ).isEmpty() ) {
        _edit->show();
        qInstallMessageHandler( redirectMsg );
    } else qInstallMessageHandler( 0 );
#endif
    /*
    if ( url.queryItemValue( "phi" ).isEmpty() ) {
        url.addQueryItem( "phi", "1" );
        _src=url.toString();
        int i=openUrl( _src, "_self" );
        if ( i!=-1 ) return;
    }
    */
    QNetworkRequest req;
    req.setUrl( url );
    _view->slotNetworkRequest( req );
    //_view->show();
}

bool PhiPlugIn::readData( QIODevice*, const QString &format )
{
    Q_UNUSED( format );
    qWarning( "PhiPlugIn::readData()" );
/*
    QUrl url( _src );
    _src=url.toString();
#ifdef PHIDEBUG
    if ( !url.queryItemValue( "phidebug" ).isEmpty() ) {
        _edit->show();
        qInstallMsgHandler( redirectMsg );
    } else qInstallMsgHandler( 0 );
#endif
    if ( url.queryItemValue( "phi" ).isEmpty() ) {
        url.addQueryItem( "phi", "1" );
        _src=url.toString();
        int i=openUrl( _src, "_self" );
        if ( i!=-1 ) return false;
    }
    QNetworkRequest req;
    req.setUrl( url );
    _view->slotNetworkRequest( req );    qDebug( "readData %s", qPrintable( format ) );
*/
    return false;
}

void PhiPlugIn::slotJavaScriptConsoleMessage( const QString &msg, int lineNumber, const QString &sourceId )
{
    QString tmp=msg+'\n'+tr( "Line:" )+' '+QString::number( lineNumber );
#ifdef PHIDEBUG
    if ( _edit->isVisible() ) _edit->appendPlainText( tmp );
    else QMessageBox::warning( this, sourceId, tmp, QMessageBox::Ok );
#else
    QMessageBox::warning( this, sourceId, tmp, QMessageBox::Ok );
#endif
}

void PhiPlugIn::slotProxyRequest( const QNetworkProxy &proxy, QAuthenticator *auth )
{
    PHIAConfig::instance()->showProxyDialog( this, proxy, auth );
}

void PhiPlugIn::slotUnsupportedContent( PHIAAbstractWebView*, QNetworkReply *reply )
{
    qDebug( "slotUnsupportedContent()" );
    QUrl url=reply->request().url();
    reply->deleteLater();
    int i=openUrl( url.toString(), "_top" );
    if ( i==-1 ) QDesktopServices::openUrl( url );
}

QTNPFACTORY_BEGIN( "Phi Plug-In (" PHIVERSION ")", "Plug-In for displaying Phi based content" );
    QTNPCLASS(PhiPlugIn)
QTNPFACTORY_END()

#ifdef QAXSERVER
#include <ActiveQt/QAxFactory>
QAXFACTORY_BEGIN( "{acd22722-6b9f-4702-851b-012db4c113fd}", "{c0a18c03-894f-4b2c-868f-b3ff63bbc567}" )
    QAXCLASS(PhiPlugIn)
QAXFACTORY_END()
#endif
