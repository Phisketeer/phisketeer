/***************************************************************************
 *   $Id: omqconfig.cpp,v 1.3 2007/02/06 19:14:40 linuxprofi Exp $
 *   Copyright (c) 2008-2009 Phi Technologies, Bernd Schumacher
 ***************************************************************************/
#include <QPainter>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QPrintDialog>
#include "ampwebview.h"
#include "ampwebpage.h"
#include "amptools.h"

AMPWebView::AMPWebView( PHIAHistory *history, QWidget *parent )
    : PHIAAbstractWebView( history, parent ), _webPage( 0 ), _webView( 0 ),
    _isLoading( false ), _reply( 0 )
    //_cursorSetByWebCore( false ), _usesWebCoreCursor( true )
{
    qDebug( "AMPWebView::AMPWebView()" );
    _webView=new QWebView( this );
    _webPage=new AMPWebPage( this );
    QVBoxLayout *layout=new QVBoxLayout();
    layout->setContentsMargins( 0, 0, 0 ,0 );
    layout->addWidget( _webView );
    setLayout( layout );

    Q_ASSERT( _networkAccessManager );
    _webPage->setNetworkAccessManager( _networkAccessManager );
    _webPage->setForwardUnsupportedContent( true );
    _webView->setPage( _webPage );
    //_webPage->setLinkDelegationPolicy( QWebPage::DelegateAllLinks );

    QWebFrame *frame=_webPage->mainFrame();
    connect( frame, SIGNAL( titleChanged( const QString& ) ), this, SLOT( slotTitleChanged( const QString& ) ) );
    connect( frame, SIGNAL( iconChanged() ), this, SLOT( slotIconChanged() ) );
    connect( frame, SIGNAL( urlChanged( const QUrl& ) ), this, SLOT( slotUrlChanged( const QUrl& ) ) );
    connect( _webPage, SIGNAL( loadProgress( int ) ), this, SIGNAL( loadProgress( int ) ) );
    connect( _webPage, SIGNAL( statusBarMessage( const QString& ) ), this,
        SLOT( slotStatusBarMessage( const QString& ) ) );
    connect( _webPage, SIGNAL( windowCloseRequested() ), this, SIGNAL( windowCloseRequested() ) );
    connect( _webPage, SIGNAL( unsupportedContent( QNetworkReply* ) ), this,
        SLOT( slotUnsupportedContent( QNetworkReply* ) ) );
    connect( _webPage, SIGNAL( linkHovered( const QString&, const QString&, const QString& ) ), this,
        SLOT( slotLinkHovered( const QString&, const QString&, const QString& ) ) );
    connect( _webPage, SIGNAL( loadFinished( bool ) ), this, SLOT( slotLoadFinished( bool ) ) );
    connect( _webPage, SIGNAL( loadStarted() ), this, SLOT( slotLoadStarted() ) );
    connect( _webPage, SIGNAL( javaScriptMessage( const QString&, int, const QString& ) ), this,
        SIGNAL( javaScriptConsoleMessage( const QString&, int, const QString& ) ) );
    connect( _webPage, SIGNAL( geometryChangeRequested( const QRect& ) ), this,
        SIGNAL( geometryChangeRequested( const QRect& ) ) );
    connect( _webPage, SIGNAL( statusBarVisibilityChangeRequested( bool ) ), this,
        SIGNAL( statusBarVisibilityChangeRequested( bool ) ) );
    connect( _webPage, SIGNAL( menuBarVisibilityChangeRequested( bool ) ), this,
        SIGNAL( menuBarVisibilityChangeRequested( bool ) ) );
    connect( _webPage, SIGNAL( toolBarVisibilityChangeRequested( bool ) ), this,
        SIGNAL( toolBarVisibilityChangeRequested( bool ) ) );
    connect( _webPage, SIGNAL( printRequested( QWebFrame* ) ), this,
        SLOT( slotPrintRequest( QWebFrame* ) ) );
    connect( _networkAccessManager, SIGNAL( finished( QNetworkReply* ) ), this,
        SLOT( slotReplyFinished( QNetworkReply* ) ) );
    update();
}

AMPWebView::~AMPWebView()
{
    qDebug( "AMPWebView::~AMPWebView()" );
}

void AMPWebView::slotSslErrors( QNetworkReply *reply, const QList<QSslError> &errors )
{
    if ( _webPage->requestedUrl()!=reply->request().url() ) return;
    PHIAAbstractWebView::slotSslErrors( reply, errors );
}

void AMPWebView::slotAuthRequest( QNetworkReply *reply, QAuthenticator *auth )
{
    if ( _webPage->requestedUrl()!=reply->request().url() ) return;
    PHIAAbstractWebView::slotAuthRequest( reply, auth );
}

void AMPWebView::slotUpdateGeometry( const QRect &r )
{
    //qDebug( "geometry change" );
    setGeometry( r );
}

void AMPWebView::slotPrintRequest( QWebFrame *frame )
{
    if ( !frame ) return;
    _printFrame=frame;
    QPrintDialog printDlg( this );
    connect( &printDlg, SIGNAL( accepted( QPrinter* ) ), this, SLOT( slotPrintFrame( QPrinter* ) ) );
    printDlg.exec();
}

void AMPWebView::slotPrintFrame( QPrinter *p )
{
    p->setResolution( 1200 );
    QPainter painter( p );
    painter.setRenderHint( QPainter::Antialiasing, true );
    painter.setRenderHint( QPainter::TextAntialiasing, true );
    painter.setRenderHint( QPainter::SmoothPixmapTransform, true );
    _printFrame->render( &painter );
}

void AMPWebView::slotPrint( QPrinter *p )
{
    p->setResolution( 1200 );
    QPainter painter( p );
    painter.setRenderHint( QPainter::Antialiasing, true );
    painter.setRenderHint( QPainter::TextAntialiasing, true );
    painter.setRenderHint( QPainter::SmoothPixmapTransform, true );
    renderDocument( &painter );
}

void AMPWebView::slotUrlChanged( const QUrl &url )
{
    qDebug( "AMPWebView::slotUrlChanged()" );
    // emited by frame as soon as the title of the html header was recieved
    emit statusBarMessage( tr( "Loading:" )+' '+url.toString(), 20000 );
    PHIAHistoryItem it( url, title(), icon(), QDateTime::currentDateTime() );
    _history->addHistoryItem( it );
    emit forwardAvailable( _history->canGoForward() );
    emit backAvailable( _history->canGoBack() );
    emit urlChanged( url );
}

void AMPWebView::slotIconChanged()
{
    qDebug( "AMPWebView::slotIconChanged" );
    setWindowIcon( icon() );
    PHIAHistoryStorage::instance()->updateIcon( url(), icon() );
    emit iconChanged( this );
}

void AMPWebView::slotTitleChanged( const QString& )
{
    qDebug( "AMPWebView::slotTitleChanged" );
    emit titleChanged( this );
    setWindowTitle( title() );
    PHIAHistoryStorage::instance()->updateTitle( url(), title() );
}

void AMPWebView::slotUnsupportedContent( QNetworkReply *rep )
{
    emit unsupportedContent( this, rep );
}

void AMPWebView::slotLoadStarted()
{
    qDebug( "AMPWebView::slotLoadStarted()" );
    _isLoading=true;
    emit progressMaximum( 100 );
    emit loading( true );
}

void AMPWebView::slotLoadFinished( bool )
{
    //PHIAHistoryItem it( url(), title(), icon(), QDateTime::currentDateTime() );
    //_history->addHistoryItem( it );
    _isLoading=false;
    emit loading( false );
    qDebug( "AMPWebView::slotLoadFinished()" );
}

void AMPWebView::slotStatusBarMessage( const QString &s )
{
    emit statusBarMessage( s, 30000 );
}

void AMPWebView::slotLinkHovered( const QString &link, const QString &title, const QString &content )
{
    qDebug( "slotLinkHovered() %s %s %s", qPrintable( link ), qPrintable( title ), qPrintable( content ) );
    emit linkHovered( link, title, content );
}

void AMPWebView::slotNetworkRequest( const QNetworkRequest &req )
{
    qDebug( "mainFrame()->load()" );
    _webPage->mainFrame()->load( req );
    qDebug( "AMPWebView::slotNetworkRequest()" );
}

void AMPWebView::slotBack()
{
    if ( !canGoBack() ) return;
    _history->back();
    emit backAvailable( canGoBack() );
    QWebHistoryItem wit=_webPage->history()->backItem();
    PHIAHistoryItem it=_history->currentItem();
    if ( wit.isValid() && it.url()==wit.url() ) {
        qDebug( "slotBack(): matches back item" );
        _webPage->triggerAction( QWebPage::Back );
    } else {
        qDebug( "%s<>%s", qPrintable( wit.url().toString() ), qPrintable( it.url().toString() ) );
        QNetworkRequest req;
        req.setUrl( it.url() );
        slotNetworkRequest( req );
    }
}

void AMPWebView::slotForward()
{
    if ( !canGoForward() ) return;
    _history->forward();
    emit forwardAvailable( canGoForward() );
    QWebHistoryItem wit=_webPage->history()->forwardItem();
    PHIAHistoryItem it=_history->currentItem();
    if ( wit.isValid() && it.url()==wit.url() ) {
        qDebug( "slotForward(): matches forward item" );
        _webPage->triggerAction( QWebPage::Forward );
    } else {
        qDebug( "%s<>%s", qPrintable( wit.url().toString() ), qPrintable( it.url().toString() ) );
        QNetworkRequest req;
        req.setUrl( it.url() );
        slotNetworkRequest( req );
    }
}

void AMPWebView::slotReload()
{
    _webPage->triggerAction( QWebPage::Reload );
}

void AMPWebView::slotStop()
{
    _webPage->triggerAction( QWebPage::Stop );
}

void AMPWebView::slotZoomIn()
{
    _webPage->mainFrame()->setZoomFactor( _webPage->mainFrame()->zoomFactor()+.02 );
}

void AMPWebView::slotZoomOut()
{
    _webPage->mainFrame()->setZoomFactor( _webPage->mainFrame()->zoomFactor()-.02 );
}

void AMPWebView::slotZoomNormal()
{
    _webPage->mainFrame()->setZoomFactor( 1. );
}

void AMPWebView::slotSetZoomFactor( qreal f )
{
    _webPage->mainFrame()->setZoomFactor( f );
}

void AMPWebView::setNetworkReply( QNetworkReply *r )
{
    qDebug( "AMPWebView::setNetworkReply()" );
    if ( !r ) return;
    _reply=r;
    connect( _reply, SIGNAL( finished() ), this, SLOT( slotReplyFinished() ) );
    connect( _reply, SIGNAL( downloadProgress( qint64, qint64 ) ), this,
        SLOT( slotReplyDownloadProgress( qint64, qint64 ) ) );
    slotLoadStarted();
}

void AMPWebView::slotReplyFinished()
{
    qDebug( "AMPWebView::slotReplyFinished()" );
    try {
        QByteArray arr=_reply->readAll();
        _webPage->mainFrame()->setContent( arr,
            _reply->request().header( QNetworkRequest::ContentTypeHeader ).toString(),
            _reply->request().url() );
    } catch ( std::bad_alloc& ) {
        QMessageBox::critical( this, tr( "Error" ), tr( "Out of memory!" ), QMessageBox::Ok );
    }
    PHIAHistoryItem it( _reply->request().url(), title(), icon(), QDateTime::currentDateTime() );
    _history->addHistoryItem( it );
    emit urlChanged( _reply->request().url() );
    _sslConfig=_reply->sslConfiguration();
    _reply->deleteLater();
    _reply=0;
    slotLoadFinished( true );
}

void AMPWebView::slotReplyFinished( QNetworkReply *reply )
{
    qDebug( "AMPWebView::slotReplyFinished %s %s", qPrintable( _webPage->requestedUrl().toString() ),
        qPrintable( reply->request().url().toString() ) );
    if ( _webPage->requestedUrl()!=reply->request().url() ) return;
    if ( reply->error()!=QNetworkReply::NoError ) {
        QMessageBox::warning( this, tr( "Network error"),
            tr( "Reply error string:")+"\n"+reply->errorString() );
        return;
    }

    // This is used to get the SSL config from Webkit. Do NOT delete reply or change it.
    //qDebug( "request origin %p", reply->request().originatingObject() );
    _sslConfig=reply->sslConfiguration();
    qDebug( "_sslConfig=%d (%s)", _sslConfig.isNull() ? 0 : 1, qPrintable( reply->request().url().toString() ) );
}

void AMPWebView::slotReplyDownloadProgress( qint64 progress, qint64 total )
{
    emit progressMaximum( static_cast<int>(total) );
    emit loadProgress( static_cast<int>(progress) );
}

/*
QSize AMPWebView::sizeHint() const
{
    return QSize( 800, 600 );
}

void AMPWebView::resizeEvent( QResizeEvent *e )
{
    _webPage->setViewportSize( e->size() );
}

void AMPWebView::paintEvent( QPaintEvent *e )
{
    QWebFrame *frame=_webPage->mainFrame();
    QPainter p( this );
    frame->render( &p, e->region() );
}


void AMPWebView::mouseMoveEvent( QMouseEvent *e )
{
    const bool accepted=e->isAccepted();
    _webPage->event( e );
    e->setAccepted( accepted );
}

void AMPWebView::mousePressEvent( QMouseEvent *e )
{
    const bool accepted=e->isAccepted();
    _webPage->event( e );
    e->setAccepted( accepted );
}

void AMPWebView::mouseDoubleClickEvent( QMouseEvent *e )
{
    const bool accepted=e->isAccepted();
    _webPage->event( e );
    e->setAccepted( accepted );
}

void AMPWebView::mouseReleaseEvent( QMouseEvent *e )
{
    const bool accepted=e->isAccepted();
    _webPage->event( e );
    e->setAccepted( accepted );
}

void AMPWebView::contextMenuEvent( QContextMenuEvent *e )
{
    const bool accepted=e->isAccepted();
    _webPage->event( e );
    e->setAccepted( accepted );
}

void AMPWebView::wheelEvent( QWheelEvent *e )
{
    const bool accepted=e->isAccepted();
    _webPage->event( e );
    e->setAccepted( accepted );
}

void AMPWebView::keyPressEvent( QKeyEvent *e )
{
    _webPage->event( e );
    if ( !e->isAccepted() ) QWidget::keyPressEvent( e );
}

void AMPWebView::keyReleaseEvent( QKeyEvent *e )
{
    _webPage->event( e );
    if ( !e->isAccepted() ) QWidget::keyReleaseEvent( e );
}

void AMPWebView::focusInEvent( QFocusEvent *e )
{
    _webPage->event( e );
}

void AMPWebView::focusOutEvent( QFocusEvent *e )
{
    _webPage->event( e );
}

void AMPWebView::dragEnterEvent( QDragEnterEvent *e )
{
    _webPage->event( e );
}

void AMPWebView::dragLeaveEvent( QDragLeaveEvent *e )
{
    _webPage->event( e );
}

void AMPWebView::dragMoveEvent( QDragMoveEvent *e )
{
    _webPage->event( e );
}

void AMPWebView::dropEvent( QDropEvent *e )
{
    _webPage->event( e );
}

bool AMPWebView::focusNextPrevChild( bool next )
{
    if ( _webPage->focusNextPrevChild( next ) ) return true;
    return QWidget::focusNextPrevChild( next );
}

QVariant AMPWebView::inputMethodQuery( Qt::InputMethodQuery property ) const
{
    return _webPage->inputMethodQuery(property);
}

void AMPWebView::inputMethodEvent( QInputMethodEvent *e )
{
    _webPage->event( e );
}

void AMPWebView::changeEvent( QEvent *e )
{
    if ( e->type()==QEvent::PaletteChange ) {
        _webPage->setPalette( palette() );
    }
}

bool AMPWebView::event( QEvent *e )
{
    if ( e->type()==QEvent::ContextMenu ) {
        if ( !isEnabled() ) return false;
        QContextMenuEvent *event = static_cast<QContextMenuEvent *>(e);
        if ( _webPage->swallowContextMenuEvent( event ) ) {
            e->accept();
            return true;
        }
        _webPage->updatePositionDependentActions( event->pos() );
    } else if ( e->type()==QEvent::ShortcutOverride ) {
        _webPage->event( e );
//    } else if ( e->type()==static_cast<QEvent::Type>(WebCore::SetCursorEvent::EventType) ) {
//        setCursor( static_cast<WebCore::SetCursorEvent*>(e)->cursor() );
    } else if ( e->type()==QEvent::CursorChange ) {
        _usesWebCoreCursor=_cursorSetByWebCore;
        _cursorSetByWebCore=false;
        if ( _usesWebCoreCursor && cursor().shape()==Qt::ArrowCursor ) {
            _usesWebCoreCursor=true;
            setCursor( _webCoreCursor );
        }
    } else if ( e->type()==QEvent::Leave ) {
        _webPage->event( e );
    }
    return QWidget::event( e );
}

void AMPWebView::setCursor( const QCursor& newCursor )
{
    _webCoreCursor=newCursor;
    if ( _usesWebCoreCursor ) {
        _cursorSetByWebCore=true;
        setCursor( _webCoreCursor );
    }
}
*/
