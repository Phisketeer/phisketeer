/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2013  Phisys AG, Switzerland
#    Copyright (C) 2012-2013  Phisketeer.org team
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
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
#include <QGraphicsItem>
#include <QGraphicsItemGroup>
#include <QToolBar>
#include <QComboBox>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QButtonGroup>
#include <QStatusBar>
#include <QSettings>
#include <QIcon>
#include <QCloseEvent>
#include <QWebSettings>
#include <QDesktopServices>
#include <QWebHistoryInterface>
#include <QTimer>
#include <QKeyEvent>
#include <QApplication>
#include <QStandardPaths>
#include "ampmainwindow.h"
#include "phia.h"
#include "phi.h"
#include "phiatools.h"
#include "phiaabstractwebview.h"
#include "phiasettingsdlg.h"
#include "phiaconfig.h"

AMPMainWindow::AMPMainWindow( QWidget *parent )
    : QMainWindow( parent ), _tabWidget( 0 ), _currentView( 0 ), _urlCombo( 0 ), _jsLog( 0 )
{
    qDebug( "AMPMainWindow::AMPMainWindow()" );
    setObjectName( PHIA::browserName() );
    setupUi( this );
    setAttribute( Qt::WA_DeleteOnClose );

    QSettings s;
    restoreState( s.value( PHIA::configName( PHIA::DockWidgets ) ).toByteArray() );
    restoreGeometry( s.value( PHIA::configName( PHIA::Geometry ) ).toByteArray() );
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

    _progressBar=new QProgressBar( this );
    _progressBar->setFixedSize( 150, 20 );
    statusBar()->addPermanentWidget( _progressBar );
    _progressBar->hide();
    //AMPWebHistory *webHistory=AMPWebHistory::instance();
    //QWebHistoryInterface::setDefaultInterface( webHistory );
    _sslTool=new QToolButton( this );
    _sslTool->setIcon( QPixmap( ":/gnome/unsecure" ) );
    statusBar()->addPermanentWidget( _sslTool );
    connect( _sslTool, SIGNAL( clicked() ), this, SLOT( slotSslToolClicked() ) );

    _netManager=PHIANetManager::instance();
    PHIAAbstractWebView::setNetworkAccessManager( _netManager->networkAccessManager() );
    connect( _netManager->networkAccessManager(), SIGNAL( proxyAuthenticationRequired(
        const QNetworkProxy& , QAuthenticator* ) ), this, SLOT( slotProxyRequest(
        const QNetworkProxy&, QAuthenticator* ) ) );
    PHIAAbstractWebView::setDefaultIcon( ws->webGraphic( QWebSettings::DefaultFrameIconGraphic ) );
    PHIAAbstractWebView::setMissingIcon( ws->webGraphic( QWebSettings::MissingImageGraphic ) );

    _urlCombo=new AMPUrlCombo( this );
    connect( _urlCombo, SIGNAL( activated( const QString& ) ), this, SLOT( slotUrlComboActivated( const QString& ) ) );
    navBar->addWidget( _urlCombo );

    _jsLog=new PHIAJavaScriptLog();
    _tabWidget=new AMPTabWidget( this );
    setCentralWidget( _tabWidget );
    QTimer::singleShot( 0, this, SLOT( slotInit() ) );
    //slotInit();
}

AMPMainWindow::~AMPMainWindow()
{
    PHIAHistoryStorage::instance()->save();
    qDebug( "AMPMainWindow::~AMPMainWindow()" );
}

void AMPMainWindow::closeEvent( QCloseEvent *e )
{
    QSettings s;
    s.setValue( PHIA::configName( PHIA::DockWidgets ), saveState() );
    s.setValue( PHIA::configName( PHIA::Geometry ), saveGeometry() );
    QWebSettings *ws=QWebSettings::globalSettings();
    s.setValue( PHIA::configName( PHIA::CacheDirectory ), ws->offlineStoragePath() );
    s.setValue( PHIA::configName( PHIA::AutoLoadImages ), ws->testAttribute( QWebSettings::AutoLoadImages ) );
    s.setValue( PHIA::configName( PHIA::EnableJavascript ), ws->testAttribute( QWebSettings::JavascriptEnabled ) );
    s.setValue( PHIA::configName( PHIA::EnableJava ), ws->testAttribute( QWebSettings::JavaEnabled ) );
    s.setValue( PHIA::configName( PHIA::PrintBackgrounds ), ws->testAttribute( QWebSettings::PrintElementBackgrounds ) );
    s.setValue( PHIA::configName( PHIA::JSOpenWindow ), ws->testAttribute( QWebSettings::JavascriptCanOpenWindows ) );
    s.setValue( PHIA::configName( PHIA::EnablePlugins ), ws->testAttribute( QWebSettings::PluginsEnabled ) );
    e->accept();
}

void AMPMainWindow::slotInit()
{
    navBar->show();
    if ( currentView() ) currentView()->setUrl( _urlCombo->currentText() );
    _urlCombo->setFocus();
    QSettings s;
    if ( !s.value( "WelcomeMsgShown", false ).toBool() ) {
        QMessageBox::information( this, tr( "Welcome" ),
            tr( "Welcome to the Amphibia browser. Please note: Amphibia is not intented to be"
            " a replacement for other common browsers. Its purpose is more to be able to"
            " easily test your Phi pages (i.e. Amphibia includes native Phi) or to"
            " provide Phi support on systems where a Phi plug-in is currently not available."
            " However Amphibia contains a full featured WebKit implementation to view also"
            " 'conventional' web pages." )+"\n\n"+tr( "Phisketeer is free software"
            " and we need your help to develop new features and quality. If you like this software"
            " you may want to contribute or donate. Find more information on %1." )
            .arg( "http://www.phisketeer.org" ), QMessageBox::Ok );
        s.setValue( "WelcomeMsgShown", true );
    }
}

void AMPMainWindow::setCurrentView( PHIAAbstractWebView *newView )
{
    qDebug( "AMPMainWindow::setCurrentView" );
    Q_ASSERT( newView );
    if ( _currentView ) {
        _currentView->disconnect( _progressBar );
        _currentView->disconnect( statusBar() );
        _currentView->disconnect( _netManager );
        _currentView->disconnect( this );
        _currentView->disconnect( actionBack );
        _currentView->disconnect( actionForward );
        _currentView->disconnect( actionStop );
        _currentView->disconnect( _urlCombo );
        _currentView->disconnect( navBar );
        //_currentView->dumpObjectInfo();
    }
    connect( newView, SIGNAL( loadProgress( int ) ), _progressBar, SLOT( setValue( int ) ) );
    connect( newView, SIGNAL( progressMaximum( int ) ), _progressBar, SLOT( setMaximum(int) ) );
    connect( newView, SIGNAL( statusBarMessage( const QString&, int ) ), statusBar(),
        SLOT( showMessage( const QString&, int ) ) );
    connect( newView, SIGNAL( backAvailable( bool ) ), actionBack, SLOT( setEnabled( bool ) ) );
    connect( newView, SIGNAL( forwardAvailable( bool ) ), actionForward, SLOT( setEnabled( bool ) ) );
    connect( newView, SIGNAL( windowCloseRequested() ), this, SLOT( on_actionClose_triggered() ) );
    connect( newView, SIGNAL( javaScriptConsoleMessage( const QString&, int, const QString& ) ),
        this, SLOT( slotJavaScriptConsoleMessage( const QString&, int, const QString& ) ) );
    connect( newView, SIGNAL( loading( bool ) ), this, SLOT( slotPageLoading( bool ) ) );
    connect( newView, SIGNAL( urlChanged( const QUrl& ) ), _urlCombo,
        SLOT( slotSetCurrentUrl( const QUrl& ) ) );
    connect( newView, SIGNAL( linkHovered( const QString&, const QString&, const QString& ) ),
        this, SLOT( slotLinkHovered( const QString&, const QString&, const QString& ) ) );
    connect( newView, SIGNAL( statusBarVisibilityChangeRequested( bool ) ),
        statusBar(), SLOT( setVisible( bool ) ) );
    connect( newView, SIGNAL( menuBarVisibilityChangeRequested( bool ) ),
        menuBar(), SLOT( setVisible( bool ) ) );
    connect( newView, SIGNAL( locationBarVisibilityChangeRequested( bool ) ),
        navBar, SLOT( setVisible( bool ) ) );

    actionBack->setEnabled( newView->canGoBack() );
    actionForward->setEnabled( newView->canGoForward() );
    actionStop->setEnabled( newView->isLoading() );
    _urlCombo->slotSetCurrentUrl( newView->url() );
    setWindowTitle( PHIA::browserName()+" - "+newView->title() );
    QSslConfiguration sslConfig=newView->sslConfiguration();
    if ( sslConfig.peerCertificate().isNull() ) _sslTool->setIcon( QPixmap( ":/gnome/unsecure" ) );
    else _sslTool->setIcon( QPixmap( ":/gnome/secure" ) );
    _currentView=newView;
}

void AMPMainWindow::slotSslToolClicked()
{
    if ( !_currentView ) return;
    _currentView->openSslDlg();
}

void AMPMainWindow::slotUrlComboActivated( const QString &s )
{
    QString url=s;
    if ( !url.contains( "://" ) ) url.prepend( "http://" );
    if ( !url.startsWith( "http" ) && !url.startsWith( "file" ) ) {
        QMessageBox::information( this, windowTitle(), tr( "%1 does not support other protocols"
            " than 'http', 'https' and 'file'." ).arg( PHIA::browserName() ), QMessageBox::Ok );
        return;
    }
    QUrl newUrl( url );
    if ( !newUrl.isValid() ) return;
    QNetworkRequest req;
    req.setUrl( newUrl );
    if ( _currentView ) {
        PHIAAbstractWebView *view=qobject_cast<PHIAWebView*>(_currentView);
        if ( newUrl.path().endsWith( ".phis", Qt::CaseSensitive ) ) {
            if ( !view ) { // we need to load a PHIAWebView
                qDebug( "view ist not a PHIAWebView (currentView deleteLater)" );
                PHIAHistory *history=_currentView->history();
                _currentView->deleteLater();
                _currentView=new PHIAWebView( history, this );
                int tabId=_tabWidget->currentIndex();
                if ( tabId>-1 ) _tabWidget->removeTab( tabId );
                _tabWidget->newTabRequested( view, tabId );
            }
        }
        _currentView->setScrollBarsEnabled( true );
        _currentView->history()->removeItem( newUrl );
        _currentView->slotNetworkRequest( req );
    }
}

void AMPMainWindow::slotPageLoading( bool loading )
{
    actionStop->setEnabled( loading );
    if ( statusBar()->isVisible() ) _progressBar->setVisible( loading );
    if ( !loading && _currentView ) setWindowTitle( PHIA::browserName()+" - "+_currentView->title() );
    if ( !loading && _currentView ) {
        qDebug( "setting SSL icon" );
        if ( _currentView->sslConfiguration().peerCertificate().isNull() )
            _sslTool->setIcon( QPixmap( ":/gnome/unsecure" ) );
        else _sslTool->setIcon( QPixmap( ":/gnome/secure" ) );
    }
}

void AMPMainWindow::slotLinkHovered( const QString &link, const QString &title, const QString &content )
{
    Q_UNUSED( title );
    Q_UNUSED( content );
    statusBar()->showMessage( link );
}

void AMPMainWindow::slotProxyRequest( const QNetworkProxy &proxy, QAuthenticator *auth )
{
    PHIAConfig::instance()->showProxyDialog( this, proxy, auth );
}

void AMPMainWindow::slotJavaScriptConsoleMessage( const QString &msg, int lineNumber, const QString &sourceId )
{
    _jsLog->slotJavaScriptConsoleMessage( msg, lineNumber, sourceId );
}

void AMPMainWindow::on_actionConsole_triggered()
{
    _jsLog->show();
}

void AMPMainWindow::on_actionHome_triggered()
{
    QString home=PHIAConfig::instance()->home();
    slotUrlComboActivated( home );
}

void AMPMainWindow::on_actionFullScreen_triggered()
{
    showMaximized();
}

void AMPMainWindow::on_actionCopy_triggered()
{
    if ( !QApplication::focusWidget() ) return;
    QKeyEvent press( QEvent::KeyPress, Qt::Key_C, Qt::ControlModifier );
    qApp->notify( QApplication::focusWidget(), &press );
}

void AMPMainWindow::on_actionCut_triggered()
{
    if ( !QApplication::focusWidget() ) return;
    QKeyEvent press( QEvent::KeyPress, Qt::Key_X, Qt::ControlModifier );
    qApp->notify( QApplication::focusWidget(), &press );
}

void AMPMainWindow::on_actionPaste_triggered()
{
    if ( !QApplication::focusWidget() ) return;
    QKeyEvent press( QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier );
    qApp->notify( QApplication::focusWidget(), &press );
}

void AMPMainWindow::on_actionOpenUrl_triggered()
{
    QString url=QInputDialog::getText( this, tr( "Open URL" ), tr( "Please enter a valid URL:" ) );
    if ( url.isEmpty() ) return;
    slotUrlComboActivated( url );
}

void AMPMainWindow::on_actionCookies_triggered()
{
    PHIACookieDlg dlg( this );
    dlg.exec();
}

void AMPMainWindow::on_actionDownloads_triggered()
{
    PHIAConfig::instance()->showDownloadDialog( this );
}

void AMPMainWindow::on_actionZoomIn_triggered()
{
    _currentView->slotZoomIn();
}

void AMPMainWindow::on_actionZoomOut_triggered()
{
    _currentView->slotZoomOut();
}

void AMPMainWindow::on_actionZoomNormal_triggered()
{
    _currentView->slotZoomNormal();
}

void AMPMainWindow::on_actionNewWindow_triggered()
{
    AMPMainWindow *mw=new AMPMainWindow();
    mw->show();
}

void AMPMainWindow::on_actionBack_triggered()
{
    _currentView->slotBack();
}

void AMPMainWindow::on_actionForward_triggered()
{
    _currentView->slotForward();
}

void AMPMainWindow::on_actionReload_triggered()
{
    _currentView->slotReload();
}

void AMPMainWindow::on_actionStop_triggered()
{
    _currentView->slotStop();
}

void AMPMainWindow::on_actionSaveAs_triggered()
{
    QString suffix;
    PHIAWebView *phiview=qobject_cast<PHIAWebView*>(_currentView);
    if ( phiview ) {
        QMessageBox::Button res=QMessageBox::warning( this, tr( "Save as" ),
            tr( "You can not save phi based content directly." )
            +"\n"+tr( "Do you want to load the HTML version instead?" ),
            QMessageBox::Cancel | QMessageBox::Yes, QMessageBox::Cancel );
        if ( res==QMessageBox::Cancel ) return;
        QUrl url=phiview->url();
        QUrlQuery query( url );
        query.addQueryItem( "phi", "0" );
        url.setQuery( query );
        slotUrlComboActivated( url.toString() );
        return;
    }
    AMPWebView *ampview=qobject_cast<AMPWebView*>(_currentView);
    if ( phiview ) suffix="(*.phi)";
    else suffix="(*.html *.htm *.txt)";
    QString filename=QFileDialog::getSaveFileName( this, tr( "Save as" ), QDir::homePath(), suffix );
    if ( filename.isEmpty() ) return;
    if ( phiview ) {
        if ( !filename.endsWith( ".phi" ) ) filename+=".phi";
        QFile phifile( filename );
        if ( !phifile.open( QIODevice::WriteOnly ) ) {
            QMessageBox::warning( this, tr( "Error" ), tr( "Could not open file '%1' for writing." )
                .arg( filename ), QMessageBox::Ok );
            return;
        }
        phifile.write( phiview->phiOutput() );
        phifile.close();
        return;
    }
    if ( !filename.endsWith( ".txt" ) && !filename.endsWith( ".htm" ) && !filename.endsWith( ".html" ) ) {
        filename+=".txt";
    }
    Q_ASSERT( ampview );
    QFile txtfile( filename );
    if ( !txtfile.open( QIODevice::WriteOnly ) ) {
        QMessageBox::warning( this, tr( "Error" ), tr( "Could not open file '%1' for writing." )
            .arg( filename ), QMessageBox::Ok );
        return;
    }
    txtfile.write( ampview->source().toLocal8Bit() );
    txtfile.close();
}

void AMPMainWindow::on_actionSource_triggered()
{
    PHIASourceDialog *sd=new PHIASourceDialog( this );
    sd->slotSetSource( _currentView->source() );
    sd->setWindowTitle( PHIA::browserName()+" - "+_currentView->title() );
    sd->setWindowIcon( _currentView->icon() );
    sd->show();
}

void AMPMainWindow::on_actionPrintPreview_triggered()
{
    if ( !_currentView ) return;
    QPrintPreviewDialog printDlg( this );
    connect( &printDlg, SIGNAL( paintRequested( QPrinter* ) ), _currentView, SLOT( slotPrint( QPrinter* ) ) );
    printDlg.exec();
}

void AMPMainWindow::on_actionPrint_triggered()
{
    if ( !_currentView ) return;
    QPrintDialog printDlg( this );
    connect( &printDlg, SIGNAL( accepted( QPrinter* ) ), _currentView, SLOT( slotPrint( QPrinter* ) ) );
    printDlg.exec();
}

void AMPMainWindow::on_actionSendLink_triggered()
{
    QDesktopServices::openUrl( QUrl( QLatin1String( "mailto:?subject=Link&body=" )
        +_currentView->url().toString() ) );
}

void AMPMainWindow::on_actionSettings_triggered()
{
    PHIASettingsDlg dlg( this );
    if ( dlg.exec()==QDialog::Rejected ) return;

    QSettings s;
    QString path=s.value( PHIA::configName( PHIA::CacheDirectory ) ).toString();
    if ( path.isEmpty() ) path=QStandardPaths::writableLocation( QStandardPaths::CacheLocation )
        +QDir::separator()+PHIA::phiaDir();
    QDir cacheDir( path );
    if ( !cacheDir.exists() ) cacheDir.mkpath( path );

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
    //_currentView->slotSettingsClicked();
}

void AMPMainWindow::on_actionClose_triggered()
{
    _tabWidget->slotTabCloseRequested( _tabWidget->currentIndex() );
}

void AMPMainWindow::on_actionOpenFile_triggered()
{
    QString file=QFileDialog::getOpenFileName( this, tr( "Open file" ), QDir::homePath() );
    if ( file.isEmpty() ) return;
    QFileInfo info( file );
    if ( info.canonicalFilePath().isEmpty() ) return;
    slotUrlComboActivated( "file://"+info.canonicalFilePath() );
}

void AMPMainWindow::on_actionNewTab_triggered()
{
    _tabWidget->newTabRequested();
}

void AMPMainWindow::on_actionQuit_triggered()
{
    close();
}

void AMPMainWindow::on_actionAbout_triggered()
{
    QMessageBox::information( this, tr( "About %1" ).arg( PHIA::browserName() ),
        PHIA::browserName()+" "+tr( "version:" )
        +" "+qApp->applicationVersion()+"\n"+tr( "Phi library version:")+" "+PHI::libVersion()
        +"\n"+tr( "Phia library version:")+" "+PHIA::libVersion()
        +"\n\nCopyright 2010-2012 by Marius Schumacher"
        +"\nCopyright 2011-2012 by Phisys AG, Switzerland"
        +"\nCopyright 2012-"+QDate::currentDate().toString( "yyyy" )+" by Phisketeer team"
        +"\nhttp://www.phisketeer.org", QMessageBox::Ok );
}

void AMPMainWindow::on_actionHelp_triggered()
{
    on_actionNewTab_triggered();
    slotUrlComboActivated( "http://wiki.phisketeer.org/index.php/Amphibia_browser" );
}
