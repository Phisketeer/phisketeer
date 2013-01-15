/*
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
#include <QNetworkRequest>
#include <QSettings>
#include <QDesktopServices>
#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QIcon>
#include <QStatusBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QPrintPreviewDialog>
#include <QPrintDialog>
#include <QDate>
#include <QNetworkCookie>
#include "phiamainwindow.h"
#include "phianetmanager.h"
#include "phiawebview.h"
#include "phiaauthdlg.h"
#include "phia.h"
#include "phiaconfig.h"
#include "phiatools.h"

QHash <QString, PHIAMainWindow*> PHIAMainWindow::_activeWindows=QHash <QString, PHIAMainWindow*>();

PHIAMainWindow* PHIAMainWindow::createWindow( const QString &url, const QUrl &referer,
    const QString &target, const QString &options )
{
    PHIAMainWindow *mw=_activeWindows.value( target, 0 );
    if ( !mw ) mw=new PHIAMainWindow();
    if ( target!="_blank" && target!="_self" ) _activeWindows.insert( target, mw );

    PHIAExtractWindowOpts wo( options );
    QRect r=PHI::defaultWindowSize();
    if ( wo.height()>-1 ) r.setHeight( wo.height() );
    if ( wo.width()>-1 ) r.setWidth( wo.width() );
    mw->slotUpdateGeometry( r );
    if ( wo.top()>-1 ) mw->move( mw->pos().x(), wo.top() );
    if ( wo.left()>-1 ) mw->move( wo.left(), mw->pos().y() );
    mw->slotShowStatusBar( wo.showStatusBar() );
    mw->slotShowMenu( wo.showMenuBar() );
    mw->slotResizable( wo.resizable() );
    mw->slotShowScrollBars( wo.showScrollBars() );

    QUrl reqUrl=PHI::createUrlForLink( referer, url );
    mw->slotSetUrl( reqUrl );
    mw->show();
    mw->setFocus();
    if ( mw->windowTitle().isEmpty() ) mw->setWindowTitle( PHIA::browserName() );
    return mw;
}

PHIAMainWindow::PHIAMainWindow( const QString &url, QWidget *parent )
    : QMainWindow( parent ), _view( 0 ), _saveGeometry( true ), _jsLog( 0 )
{
    qDebug( "PHIAMainWindow::PHIAMainWindow()" );
    setAttribute( Qt::WA_DeleteOnClose );
    setupUi( this );
    QSettings s;
    restoreGeometry( s.value( PHIA::configName( PHIA::Geometry ) ).toByteArray() );
    setWindowTitle( PHIA::viewerName() );

    QString path=s.value( PHIA::configName( PHIA::CacheDirectory ) ).toString();
    if ( path.isEmpty() ) path=QStandardPaths::writableLocation( QStandardPaths::CacheLocation )
        +QDir::separator()+PHIA::phiaDir();
    QDir cacheDir( path );
    if ( !cacheDir.exists() ) cacheDir.mkpath( path );
    s.setValue( PHIA::configName( PHIA::CacheDirectory ), path );

    PHIANetManager *netManager=PHIANetManager::instance();
    PHIAAbstractWebView::setNetworkAccessManager( netManager->networkAccessManager() );
    connect( netManager->networkAccessManager(), SIGNAL( proxyAuthenticationRequired(
        const QNetworkProxy&, QAuthenticator* ) ), this, SLOT( slotProxyRequest(
        const QNetworkProxy&, QAuthenticator* ) ) );
    PHIAAbstractWebView::setDefaultIcon( QIcon() );
    PHIAAbstractWebView::setMissingIcon( QIcon() );

    _progressBar=new QProgressBar( this );
    _progressBar->setFixedSize( 150, 20 );
    statusBar()->addPermanentWidget( _progressBar );
    _progressBar->hide();

    _sslTool=new QToolButton( this );
    connect( _sslTool, SIGNAL( clicked() ), this, SLOT( slotSslToolClicked() ) );
    _sslTool->setIcon( QPixmap( ":/gnome/unsecure" ) );
    statusBar()->addPermanentWidget( _sslTool );

    _jsLog=new PHIAJavaScriptLog();
    qDebug( "PHIAMainWindow::new PHIAWebView (with args)" );
    setWebView( new PHIAWebView( 0, 0 ) );
    slotSetUrl( url );
}

// Usage for JavaScript window.open()
PHIAMainWindow::PHIAMainWindow() : QMainWindow( 0 ), _view( 0 ), _saveGeometry( false ), _jsLog( 0 )
{
    qDebug( "PHIAMainWindow::PHIAMainWindow()" );
    setAttribute( Qt::WA_DeleteOnClose );
    setupUi( this );
    setWindowTitle( PHIA::viewerName() );

    _progressBar=new QProgressBar( this );
    _progressBar->setFixedSize( 150, 20 );
    statusBar()->addPermanentWidget( _progressBar );
    _progressBar->hide();

    _sslTool=new QToolButton( this );
    connect( _sslTool, SIGNAL( clicked() ), this, SLOT( slotSslToolClicked() ) );
    _sslTool->setIcon( QPixmap( ":/gnome/unsecure" ) );
    statusBar()->addPermanentWidget( _sslTool );

    _jsLog=new PHIAJavaScriptLog();
    qDebug( "PHIAMainWindow::new PHIAWebView" );
    setWebView( new PHIAWebView( 0, 0 ) );
}

PHIAMainWindow::~PHIAMainWindow()
{
    QString target;
    QHash<QString, PHIAMainWindow*>::iterator i;
    for ( i=_activeWindows.begin(); i!=_activeWindows.end(); ++i ) {
        if ( i.value()==this ) target=i.key();
    }
    _activeWindows.remove( target );
    delete _jsLog;
    qDebug( "PHIAMainWindow::~PHIAMainWindow()" );
}

void PHIAMainWindow::closeEvent( QCloseEvent *e )
{
    if ( _saveGeometry ) {
        QSettings s;
        s.setValue( PHIA::configName( PHIA::Geometry ), saveGeometry() );
    }
    e->accept();
}

void PHIAMainWindow::slotSslToolClicked()
{
    _view->openSslDlg();
}

void PHIAMainWindow::setWebView( PHIAAbstractWebView *v )
{
    Q_ASSERT( v );
    qDebug( "before delete view" );
    delete _view;
    _view=v;
    _view->setParent( this );
    setCentralWidget( _view );
    //connectSignals
    connect( v, SIGNAL( progressMaximum( int ) ), this, SLOT( slotProgressMaximum( int ) ) );
    connect( v, SIGNAL( loadProgress( int ) ), this, SLOT( slotLoadProgress( int ) ) );
    connect( v, SIGNAL( loading( bool ) ), this, SLOT( slotLoading( bool ) ) );
    connect( v, SIGNAL( titleChanged( PHIAAbstractWebView* ) ), this,
        SLOT( slotTitleChanged( PHIAAbstractWebView* ) ) );
    connect( v, SIGNAL( iconChanged( PHIAAbstractWebView* ) ), this,
        SLOT( slotIconChanged( PHIAAbstractWebView* ) ) );
    connect( v, SIGNAL( statusBarMessage( const QString&, int ) ), this,
        SLOT( slotStatusBarMessage( const QString&, int ) ) );
    connect( v, SIGNAL( geometryChangeRequested( const QRect& ) ), this,
        SLOT( slotUpdateGeometry( const QRect& ) ) );
    connect( v, SIGNAL( javaScriptConsoleMessage( const QString&, int, const QString& ) ),
        this, SLOT( slotJavaScriptConsoleMessage( const QString&, int, const QString& ) ) );
    connect( v, SIGNAL( menuBarVisibilityChangeRequested( bool ) ), this,
        SLOT( slotShowMenu( bool ) ) );
    connect( v, SIGNAL( toolBarVisibilityChangeRequested( bool ) ), this,
        SLOT( slotShowToolBar( bool ) ) );
    connect( v, SIGNAL( statusBarVisibilityChangeRequested( bool ) ), this,
        SLOT( slotShowStatusBar( bool ) ) );
    connect( v, SIGNAL( scrollBarVisibilityChangeRequested( bool ) ), this,
        SLOT( slotShowScrollBars( bool ) ) );
    connect( v, SIGNAL( resizableChangeRequested( bool ) ), this,
        SLOT( slotResizable( bool ) ) );
    connect( v, SIGNAL( unsupportedContent( PHIAAbstractWebView*, QNetworkReply* ) ), this,
        SLOT( slotUnsupportedContent( PHIAAbstractWebView*, QNetworkReply* ) ) );
}

void PHIAMainWindow::slotJavaScriptConsoleMessage( const QString &msg, int line, const QString &id )
{
    //QMessageBox::warning( this, id, tmp, QMessageBox::Ok );
    _jsLog->slotJavaScriptConsoleMessage( msg, line, id );
}

void PHIAMainWindow::slotResizable( bool b )
{
    if ( b ) {
        setMinimumSize( 100, 100 );
        setMaximumSize( QApplication::desktop()->availableGeometry().size() );
    } else {
        setFixedSize( geometry().size() );
    }
}

void PHIAMainWindow::slotShowScrollBars( bool b )
{
    if ( !_view ) return;
    _view->setScrollBarsEnabled( b );
}

void PHIAMainWindow::slotShowMenu( bool b )
{
    menuBar()->setVisible( b );
}

void PHIAMainWindow::slotShowStatusBar( bool b )
{
    statusBar()->setVisible( b );
}

void PHIAMainWindow::slotShowToolBar( bool b )
{
    Q_UNUSED( b );
    //tool->setVisible( b );
}

void PHIAMainWindow::slotProxyRequest( const QNetworkProxy &proxy, QAuthenticator *auth )
{
    PHIAConfig::instance()->showProxyDialog( this, proxy, auth );
}

void PHIAMainWindow::slotUpdateGeometry( const QRect &geom )
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
}

void PHIAMainWindow::slotSetUrl( const QString &u )
{
    Q_ASSERT( _view );
    QUrl url( u );
    slotSetUrl( url );
}

void PHIAMainWindow::slotSetUrl( const QUrl &url )
{
    Q_ASSERT( _view );
    QNetworkRequest req;
    req.setUrl( url );
    _view->slotNetworkRequest( req );
}

void PHIAMainWindow::slotTitleChanged( PHIAAbstractWebView *view )
{
    if ( !view ) return;
    if ( view->title().isEmpty() ) setWindowTitle( PHIA::viewerName() );
    //else if ( _saveGeometry ) setWindowTitle( PHIA::viewerName()+" - "+view->title() );
    else setWindowTitle( view->title() );
}

void PHIAMainWindow::slotIconChanged( PHIAAbstractWebView *view )
{
    QIcon icon=view->icon();
    if ( icon.isNull() ) icon=QIcon( QPixmap( ":/phia/phi/images/own/philogo.png" ) );
    setWindowIcon( icon );
}

void PHIAMainWindow::slotProgressMaximum( int max )
{
    _progressBar->setMaximum( max );
}

void PHIAMainWindow::slotLoadProgress( int prog )
{
    _progressBar->setValue( prog );
}

void PHIAMainWindow::slotLoading( bool loading )
{
    _progressBar->setVisible( loading );
    QSslConfiguration sslConfig=_view->sslConfiguration();
    if ( sslConfig.peerCertificate().isNull() ) _sslTool->setIcon( QPixmap( ":/gnome/unsecure" ) );
    else _sslTool->setIcon( QPixmap( ":/gnome/secure" ) );
}

void PHIAMainWindow::slotStatusBarMessage( const QString &text, int timeout )
{
    QStatusBar *bar=statusBar();
    bar->showMessage( text, timeout );
}

void PHIAMainWindow::slotWindowCloseRequested()
{
    close();
}

void PHIAMainWindow::slotUnsupportedContent( PHIAAbstractWebView*, QNetworkReply *reply )
{
    QMessageBox::Button res=QMessageBox::warning( this, tr( "Unsupported content" ),
        tr( "Phi can not display the contents of the requested file." )+'\n'+
        tr( "Do you want to open it in a new browser window or save it to the local file system?" ),
        QMessageBox::Save | QMessageBox::Open | QMessageBox::Abort, QMessageBox::Save );
    if ( res==QMessageBox::Save ) {
        PHIAConfig::instance()->addDownload( reply, this );
        return;
    } else if ( res==QMessageBox::Open ) {
        QDesktopServices::openUrl( reply->url() );
    }
    reply->deleteLater();
}

void PHIAMainWindow::on_actionAbout_triggered()
{
    QMessageBox::information( this, tr( "About %1" ).arg( PHIA::viewerName() ),
        PHIA::viewerName()+' '+tr( "version:" )
        +' '+qApp->applicationVersion()+'\n'+tr( "Phi library version:" )+' '
        +PHI::libVersion()+'\n'+tr( "Phia library version:" )+" "+PHIA::libVersion()+'\n'
        +"\nCopyright 2010-2012 by Marius Schumacher"
        +"\nCopyright 2011-2012 by Phisys AG, Switzerland"
        +"\nCopyright 2012-"+QDate::currentDate().toString( "yyyy" )+" by "
        "Phisketeer team", QMessageBox::Ok );
}

void PHIAMainWindow::on_actionQuit_triggered()
{
    close();
}

void PHIAMainWindow::on_actionSettings_triggered()
{
    _view->settingsClicked();
}

void PHIAMainWindow::on_actionOpen_triggered()
{
    // Disabled in version 1
    /** @todo Phi files could be saved and opened */
    QString file=QFileDialog::getOpenFileName( this, tr( "Open file" ), QDir::homePath(),
        tr( "Phi page" )+" (*.phi)" );
    if ( file.isEmpty() ) return;
    QFileInfo info( file );
    if ( info.canonicalFilePath().isEmpty() ) return;
    QNetworkRequest req;
    req.setUrl( QUrl( "file://"+info.canonicalFilePath() ) );
    _view->slotNetworkRequest( req );
}

void PHIAMainWindow::on_actionPrintPreview_triggered()
{
    Q_ASSERT( _view );
    QPrintPreviewDialog printDlg( this );
    connect( &printDlg, SIGNAL( paintRequested( QPrinter* ) ), _view, SLOT( slotPrint( QPrinter* ) ) );
    printDlg.exec();
}

void PHIAMainWindow::on_actionPrint_triggered()
{
    Q_ASSERT( _view );
    QPrintDialog printDlg( this );
    connect( &printDlg, SIGNAL( accepted( QPrinter* ) ), _view, SLOT( slotPrint( QPrinter* ) ) );
    printDlg.exec();
}

void PHIAMainWindow::on_actionSave_triggered()
{
    QString filename=QFileDialog::getSaveFileName( this, tr( "Save as" ), QDir::homePath(), "(*.phi)" );
    if ( filename.isEmpty() ) return;
    if ( !filename.endsWith( ".phi" ) ) filename+=".phi";
    QFile file( filename );
    if ( !file.open( QIODevice::WriteOnly ) ) {
        QMessageBox::warning( this, tr( "Error" ), tr( "Could not open file '%1' for writing.")
            .arg( filename ), QMessageBox::Ok );
        return;
    }
    if ( !qobject_cast<PHIAWebView*>(_view) ) return;
    file.write( qobject_cast<PHIAWebView*>(_view)->phiOutput() );
    file.close();
}

void PHIAMainWindow::on_actionConsole_triggered()
{
    _jsLog->show();
}

void PHIAMainWindow::on_actionCookies_triggered()
{
    PHIACookieDlg dlg( this );
    dlg.exec();
}

void PHIAMainWindow::on_actionDownloads_triggered()
{
    PHIAConfig::instance()->showDownloadDialog( this );
}

void PHIAMainWindow::on_actionZoom_in_triggered()
{
    _view->slotZoomIn();
}

void PHIAMainWindow::on_actionZoom_out_triggered()
{
    _view->slotZoomOut();
}

void PHIAMainWindow::on_actionZoom_normal_triggered()
{
    _view->slotZoomNormal();
}
