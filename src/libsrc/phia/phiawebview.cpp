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
#include <QPrinter>
#include <QVBoxLayout>
#include <QDesktopServices>
#include "phiawebview.h"
#include "phiagraphicsscene.h"
#include "phigraphicsview.h"
#include "phibasepage.h"
#include "phi.h"

PHIAWebView::PHIAWebView( QWidget *parent )
    : PHIAAbstractWebView( parent ), _zoomFactor( 1. )
{
    PHIAGraphicsScene *s=new PHIAGraphicsScene( this );
    _view=new PHIGraphicsView( s, 0 );
    QVBoxLayout *l=new QVBoxLayout();
    l->setContentsMargins( 0, 0, 0, 0 );
    l->addWidget( _view );
    setLayout( l );
    connect( s, &PHIAGraphicsScene::iconChanged, this, &PHIAWebView::slotIconChanged );
    connect( s, &PHIAGraphicsScene::titleChanged, this, &PHIAWebView::slotTitleChanged );
    connect( s, &PHIAGraphicsScene::linkRequested, this, &PHIAWebView::slotLinkRequested );
}

PHIAGraphicsScene* PHIAWebView::scene() const
{
    return qobject_cast<PHIAGraphicsScene*>(_view->scene());
}

QScriptEngine* PHIAWebView::scriptEngine() const
{
    return scene()->scriptEngine();
}

QString PHIAWebView::title() const
{
    return scene()->page()->title();
}

void PHIAWebView::slotTitleChanged( const QString &title )
{
    Q_UNUSED( title )
    emit titleChanged( this );
}

QIcon PHIAWebView::icon() const
{
    return QIcon( QPixmap::fromImage( scene()->page()->favicon() ) );
}

QSslConfiguration PHIAWebView::sslConfiguration() const
{
    return scene()->sslConfiguration();
}

void PHIAWebView::slotIconChanged( const QIcon &icon )
{
    Q_UNUSED( icon )
    emit iconChanged( this );
}

QUrl PHIAWebView::url() const
{
    return scene()->url();
}

void PHIAWebView::setUrl( const QUrl &url )
{
    scene()->setUrl( url );
}

void PHIAWebView::renderDocument( QPainter *painter )
{
    scene()->render( painter );
}

void PHIAWebView::setScrollBarsEnabled( bool enable )
{
    if ( enable ) {
        _view->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
        _view->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
    } else {
        _view->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
        _view->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    }
}

QString PHIAWebView::source() const
{
    QString tmp=scene()->page()->javascript();
    if( tmp.isEmpty() ) tmp=tr( "There is no JavaScript attached to this Phi page." );
    return tmp;
}

void PHIAWebView::slotCreateWindow( const QString &u, const QString &target, const QString &opts )
{
    if ( target==L1( "_self" ) ) {
        PHIAWindowOpts wo( opts );
        QRect r( wo.left(), wo.top(), wo.width(), wo.height() );
        emit geometryChangeRequested( r );
        if ( wo.contains( L1( "status" ) ) ) emit statusBarVisibilityChangeRequested( wo.showStatusBar() );
        if ( wo.contains( L1( "menubar" ) ) ) emit menuBarVisibilityChangeRequested( wo.showMenuBar() );
        if ( wo.contains( L1( "scrollbars" ) ) ) emit scrollBarVisibilityChangeRequested( wo.showScrollBars() );
        if ( wo.contains( L1( "toolbar" ) ) ) emit toolBarVisibilityChangeRequested( wo.showToolBar() );
        if ( wo.contains( L1( "location" ) ) ) emit locationBarVisibilityChangeRequested( wo.showLocationBar() );
        if ( wo.contains( L1( "resizable" ) ) ) emit resizableChangeRequested( wo.resizable() );
        setScrollBarsEnabled( wo.showScrollBars() );
        setUrl( PHI::createUrlForLink( url(), u ) );
    } else openWindowRequested( u, url(), target, opts );
}

void PHIAWebView::throwJavaScriptError( const QScriptValue &err )
{
    if ( !err.isError() || !scriptEngine() ) return;
    QStringList list=scriptEngine()->uncaughtExceptionBacktrace();
    QString tmp=err.toString();
    tmp+=L1( "\n\n" )+list.join( L1( "\n" ) );
    emit javaScriptConsoleMessage( tmp, scriptEngine()->uncaughtExceptionLineNumber(), url().toString() );
}

void PHIAWebView::slotLinkRequested( const QUrl &url )
{
    if ( !url.scheme().startsWith( L1 ( "http" ) ) ) QDesktopServices::openUrl( url );
    else setUrl( url );
}

void PHIAWebView::slotBack()
{

}

void PHIAWebView::slotForward()
{

}

void PHIAWebView::slotStop()
{
    scene()->abort();
}

void PHIAWebView::slotPrint( QPrinter *printer )
{
    scene()->slotRequestPrint( printer );
}

void PHIAWebView::slotReload()
{
    QUrl url=scene()->url();
    scene()->setUrl( url );
    // @todo: check for resending POST data
}

void PHIAWebView::slotZoomIn()
{
    qreal f=PHIA::zoomFactor();
    _zoomFactor=_zoomFactor+f;
    if ( _zoomFactor > 20. ) return;
    _view->scale( 1.+f, 1.+f );
}

void PHIAWebView::slotZoomOut()
{
    qreal f=-PHIA::zoomFactor();
    _zoomFactor=_zoomFactor+f;
    if ( _zoomFactor < .2 ) return;
    _view->scale( 1.+f, 1.+f );
}

void PHIAWebView::slotZoomNormal()
{
    _zoomFactor=1.;
    _view->resetTransform();
}

void PHIAWebView::slotSetZoomFactor( qreal zf )
{
    _zoomFactor=zf;
    _view->scale( _zoomFactor, _zoomFactor );
}
