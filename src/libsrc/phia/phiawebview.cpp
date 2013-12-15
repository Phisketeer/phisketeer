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
#include <QPrinter>
#include <QVBoxLayout>
#include "phiawebview.h"
#include "phiagraphicsscene.h"
#include "phigraphicsview.h"
#include "phibasepage.h"
#include "phi.h"

PHIAWebView::PHIAWebView( QWidget *parent )
    : PHIAAbstractWebView( parent )
{
    PHIAGraphicsScene *s=new PHIAGraphicsScene( this );
    _view=new PHIGraphicsView( s, 0 );
    QVBoxLayout *l=new QVBoxLayout();
    l->setContentsMargins( 0, 0, 0, 0 );
    l->addWidget( _view );
    setLayout( l );
    connect( s, &PHIAGraphicsScene::iconChanged, this, &PHIAWebView::slotIconChanged );
    connect( s, &PHIAGraphicsScene::titleChanged, this, &PHIAWebView::slotTitleChanged );
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
        PHIAExtractWindowOpts wo( opts );
        QRect r=PHI::defaultWindowRect();
        if ( wo.height()>-1 ) r.setHeight( wo.height() );
        if ( wo.width()>-1 ) r.setWidth( wo.width() );
        if ( wo.top()>-1 ) r.setY( wo.top() );
        if ( wo.left()>-1 ) r.setX( wo.left() );
        if ( r!=PHI::defaultWindowRect() ) emit geometryChangeRequested( r );
        if ( wo.contains( L1( "status" ) ) ) emit statusBarVisibilityChangeRequested( wo.showStatusBar() );
        if ( wo.contains( L1( "menubar" ) ) ) emit menuBarVisibilityChangeRequested( wo.showMenuBar() );
        if ( wo.contains( L1( "scrollbars" ) ) ) emit scrollBarVisibilityChangeRequested( wo.showScrollBars() );
        if ( wo.contains( L1( "toolbar" ) ) ) emit toolBarVisibilityChangeRequested( wo.showToolBar() );
        if ( wo.contains( L1( "location" ) ) ) emit locationBarVisibilityChangeRequested( wo.showLocationBar() );
        if ( wo.contains( L1( "resizable" ) ) ) emit resizableChangeRequested( wo.resizable() );
        setScrollBarsEnabled( wo.showScrollBars() );
        setUrl( PHI::createUrlForLink( url(), u ) );
    } //else PHIAMainWindow::createWindow( u, url(), target, opts );
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
    setUrl( url );
}

void PHIAWebView::slotBack()
{

}

void PHIAWebView::slotForward()
{

}

void PHIAWebView::slotReload()
{

}

void PHIAWebView::slotStop()
{

}

void PHIAWebView::slotPrint( QPrinter *printer )
{

}

void PHIAWebView::slotSetZoomFactor( qreal f )
{

}

void PHIAWebView::slotZoomIn()
{

}

void PHIAWebView::slotZoomOut()
{

}

void PHIAWebView::slotZoomNormal()
{

}
