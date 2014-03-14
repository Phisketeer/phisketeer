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
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "phiwebpage.h"
#include "phinetmanager.h"

#ifdef PHIWEBKIT
#include <QWebFrame>
#include <QWebSettings>
#include <QWebView>

PHIWebPage::PHIWebPage( QObject *parent )
    : QWebPage( parent ), _view( 0 ), _loading( false )
{
    setNetworkAccessManager( PHINetManager::instance()->defaultNetworkAccessManager() );
    mainFrame()->setScrollBarPolicy( Qt::Vertical, Qt::ScrollBarAlwaysOff );
    mainFrame()->setScrollBarPolicy( Qt::Horizontal, Qt::ScrollBarAlwaysOff );
    setLinkDelegationPolicy( QWebPage::DelegateExternalLinks );
    connect( this, &QWebPage::loadStarted, this, &PHIWebPage::slotLoadingStart );
    connect( this, &QWebPage::loadFinished, this, &PHIWebPage::slotLoadingFinished );
}

void PHIWebPage::setContent( const QByteArray &content, const QString &mimeType, const QUrl &baseUrl )
{
    mainFrame()->setContent( QByteArray() ); // Some QtWebkit versions crashed without clearing the content before
    mainFrame()->setContent( content, mimeType, baseUrl );
}

void PHIWebPage::setUrl( const QUrl &url )
{
    mainFrame()->setUrl( url );
}

QUrl PHIWebPage::url() const
{
    return mainFrame()->url();
}

void PHIWebPage::slotUpdateGeometry( const QRect &r )
{
    if ( !_view ) return;
    _view->resize( r.width(), r.height() );
}

void PHIWebPage::slotLinkClicked( const QUrl &url )
{
    if ( !_view ) return;
    _view->setUrl( url );
}

void PHIWebPage::slotIconChanged()
{
    if ( !_view ) return;
    _view->setWindowIcon( _view->icon() );
}

QWebPage* PHIWebPage::createWindow( QWebPage::WebWindowType type )
{
    QWebView *view=new QWebView();
    PHIWebPage *page=new PHIWebPage();
    view->setPage( page );
    page->setView( view );
    if ( type==QWebPage::WebModalDialog ) view->setWindowModality( Qt::ApplicationModal);
    view->setPage( page );
    view->setAttribute( Qt::WA_DeleteOnClose );
    connect( page, SIGNAL( geometryChangeRequested( const QRect& ) ),
        page, SLOT( slotUpdateGeometry( const QRect& ) ) );
    connect( page, SIGNAL( windowCloseRequested() ), view, SLOT( close() ) );
    connect( page, SIGNAL( microFocusChanged() ), view, SLOT( updateMicroFocus() ) );
    connect( page, SIGNAL( linkClicked( const QUrl& ) ), page, SLOT( slotLinkClicked( const QUrl& ) ) );
    connect( view, &QWebView::titleChanged, view, &QWebView::setWindowTitle );
    connect( view, &QWebView::iconChanged, page, &PHIWebPage::slotIconChanged );
    view->show();
    return page;
}

#else // don't use webkit

PHIWebPage::PHIWebPage( QObject *parent )
    : QObject( parent ), _loading( false )
{
}

void PHIWebPage::setContent( const QByteArray &content, const QString &mimeType, const QUrl &baseUrl )
{
    Q_UNUSED( content )
    Q_UNUSED( mimeType )
    Q_UNUSED( baseUrl )
}

#endif // PHIWEBKIT
