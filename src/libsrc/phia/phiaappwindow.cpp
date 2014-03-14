/*
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
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QDesktopServices>
#include <QNetworkReply>
#include <QStatusBar>
#include <QSettings>
#include <QProgressBar>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QDateTime>
#include "phiaappwindow.h"
#include "phiawebview.h"
#include "phiapplication.h"

PHIAAppWindow::PHIAAppWindow( QWidget *parent )
    : QMainWindow( parent )
{
    qDebug( "PHIAAppWindow::PHIAAppWindow()" );
    setAttribute( Qt::WA_DeleteOnClose );
    PHIAAbstractWebView::setMissingIcon( QIcon( L1( ":/gnome/brokenimage" ) ) );
    _view=new PHIAWebView( this );
    _progress=new QProgressBar( this );
    statusBar()->addPermanentWidget( _progress );
    setCentralWidget( _view );
    connectSignals( _view );

    _jsWidget=new QWidget( this, Qt::Tool );
    _jsWidget->resize( 450, 300 );
    QVBoxLayout *l=new QVBoxLayout();
    l->setContentsMargins( 6, 6, 6, 6 );
    _jsConsole=new QPlainTextEdit();
    _jsConsole->setReadOnly( true );
    _jsConsole->setMaximumBlockCount( 5000 );
    _jsConsole->setCenterOnScroll( true );
    _jsConsole->appendPlainText( tr( "Start:" )+L1( " " )+QDateTime::currentDateTime().toString() );
    l->addWidget( _jsConsole );
    _jsWidget->setLayout( l );
    _jsWidget->setWindowTitle( tr( "JavaScript log" ) );

    QSettings *s=phiApp->settings();
    restoreGeometry( s->value( L1( "PHIAAppWindow" ) ).toByteArray() );
    _jsWidget->restoreGeometry( s->value( L1( "PHIAJSConsole" ) ).toByteArray() );
}

PHIAAppWindow::~PHIAAppWindow()
{
    QSettings *s=phiApp->settings();
    s->setValue( L1( "PHIAAppWindow" ), saveGeometry() );
    s->setValue( L1( "PHIAJSConsole" ), _jsWidget->saveGeometry() );
    delete _jsWidget;
    qDebug( "PHIAAppWindow::~PHIAAppWindow()" );
}

void PHIAAppWindow::connectSignals( PHIAAbstractWebView *view )
{
    connect( view, &PHIAAbstractWebView::iconChanged, this, &PHIAAppWindow::slotFaviconChanged );
    connect( view, &PHIAAbstractWebView::titleChanged, this, &PHIAAppWindow::slotTitleChanged );
    connect( view, &PHIAAbstractWebView::unsupportedContent, this, &PHIAAppWindow::slotUnsupportedContent );
    connect( view, &PHIAAbstractWebView::statusBarVisibilityChangeRequested, this, &PHIAAppWindow::slotShowStatusBar );
    connect( view, &PHIAAbstractWebView::geometryChangeRequested, this, &PHIAAppWindow::slotGeometryChangeRequested );
    connect( view, &PHIAAbstractWebView::windowCloseRequested, this, &PHIAAppWindow::slotWindowCloseRequested );
    connect( view, &PHIAAbstractWebView::openWindowRequested, this, &PHIAAppWindow::slotOpenWindowRequested );
    connect( view, &PHIAAbstractWebView::loading, this, &PHIAAppWindow::slotIsLoading );
    connect( view, &PHIAAbstractWebView::loadProgress, this, &PHIAAppWindow::slotLoadProgress );
    connect( view, &PHIAAbstractWebView::statusBarMessage, this, &PHIAAppWindow::slotStatusBarMessage );
    connect( view, &PHIAAbstractWebView::showJavaScriptConsole, this, &PHIAAppWindow::slotShowJSConsole );
    connect( view, &PHIAAbstractWebView::javaScriptConsoleMessage, this, &PHIAAppWindow::slotJSMessage );

}

void PHIAAppWindow::setUrl( const QUrl &url )
{
    _view->setUrl( url );
}

void PHIAAppWindow::slotFaviconChanged( PHIAAbstractWebView *view )
{
    setWindowIcon( view->icon() );
}

void PHIAAppWindow::slotTitleChanged( PHIAAbstractWebView *view )
{
    setWindowTitle( view->title() );
}

void PHIAAppWindow::slotUnsupportedContent( PHIAAbstractWebView *view, const QUrl &url )
{
    Q_UNUSED( view )
    QDesktopServices::openUrl( url );
}

void PHIAAppWindow::slotShowStatusBar( bool b )
{
    statusBar()->setVisible( b );
}

void PHIAAppWindow::slotShowJSConsole()
{
    _jsWidget->show();
}

void PHIAAppWindow::slotJSMessage( const QString &error, int linenumber, const QString &id )
{
    Q_UNUSED( linenumber )
    _jsConsole->appendPlainText( QString( L1( "%1: %2\n" ) ).arg( id ).arg( error ) );
}

void PHIAAppWindow::slotGeometryChangeRequested( const QRect &geom )
{
    int x=geom.x();
    int y=geom.y();
    int w=geom.width();
    int h=geom.height();
    if ( x==-1 ) x=geometry().x();
    if ( y==-1 ) y=geometry().y();
    if ( w==-1 ) w=geometry().width();
    if ( h==-1 ) h=geometry().height();
    setGeometry( x, y, w, h );
}

void PHIAAppWindow::slotWindowCloseRequested()
{
    close();
}

void PHIAAppWindow::slotOpenWindowRequested( const QString &link, const QUrl &ref, const QString &target, const QString &opts )
{
    Q_UNUSED( target )
    PHIAWindowOpts o( opts );
    PHIAAppWindow *win=new PHIAAppWindow();
    QUrl url=PHI::createUrlForLink( ref, link );
    QRect r=PHI::defaultWindowRect();
    if ( o.left()==-1 ) r.setX( geometry().x()+10 );
    else r.setX( o.left() );
    if ( o.top()==-1 ) r.setY( geometry().y()+10 );
    else r.setY( o.top() );
    if ( o.width()!=-1 ) r.setWidth( o.width() );
    if ( o.height()!=-1 ) r.setHeight( o.height() );
    win->slotGeometryChangeRequested( r );
    win->slotShowStatusBar( o.showStatusBar() );
    win->show();
    win->setUrl( url );
}

void PHIAAppWindow::slotIsLoading( bool b )
{
    _progress->setValue( 0 );
    _progress->setVisible( b );
}

void PHIAAppWindow::slotLoadProgress( qint64 p, qint64 max )
{
    if ( max>1000000 ) {
        _progress->setMaximum( static_cast<int>(max/10000) );
        _progress->setValue( static_cast<int>(p/10000) );
    } else {
        _progress->setMaximum( static_cast<int>(max) );
        _progress->setValue( static_cast<int>(p) );
    }
}

void PHIAAppWindow::slotStatusBarMessage(const QString &s, int timeout)
{
    statusBar()->showMessage( s, timeout );
}
