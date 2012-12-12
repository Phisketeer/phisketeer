/***************************************************************************
 *   $Id: omqconfig.cpp,v 1.3 2007/02/06 19:14:40 linuxprofi Exp $
 *   Copyright (c) 2008-2009 Phi Media, Bernd Schumacher
 ***************************************************************************/
#include <QApplication>
#include <QAction>
#include <QWebSettings>
#include <QMessageBox>
#include "ampwebpage.h"
#include "amptools.h"
#include "ampwebview.h"
#include "phiaconfig.h"

AMPWebPage::AMPWebPage( QObject *parent )
    : QWebPage( parent )
{
    qDebug( "AMPWebPage::AMPWebPage()" );
    action( Copy )->setIcon( QIcon( ":/gnome/copy" ) );
    action( Reload )->setIcon( QIcon( ":/gnome/reload" ) );
}

AMPWebPage::~AMPWebPage()
{
    qDebug( "AMPWebPage::~AMPWebPage()" );
}

bool AMPWebPage::acceptNavigationRequest( QWebFrame *frame, const QNetworkRequest &request, NavigationType type )
{
    _reqUrl=request.url();
    if ( type==QWebPage::NavigationTypeFormSubmitted || type==QWebPage::NavigationTypeFormResubmitted ) {
        qDebug( "Referer: %s", request.rawHeader( "Referer" ).constData() );
        if ( request.url().scheme()!="https" && !request.rawHeader( "Referer" ).startsWith( "https" ) ) {
            if ( PHIAConfig::instance()->askForUnsecure() ) {
                QWidget *par=qobject_cast<QWidget*>(parent());
                QMessageBox::Button res=QMessageBox::warning( par, request.url().toString(),
                    tr( "Do you want to send POST data over an insecure connection?")+'\n'
                    +tr( "(You may disable this warning in 'User config'.)"),
                    QMessageBox::Yes | QMessageBox::Abort, QMessageBox::Abort );
                if ( res==QMessageBox::Abort ) return false;
            }
        }
    }
    qDebug( "NAV REQUEST %s %d", qPrintable( request.url().toString() ), static_cast<int>(type) );
    return QWebPage::acceptNavigationRequest( frame, request, type );
}

void AMPWebPage::javaScriptConsoleMessage( const QString &msg, int lineNumber, const QString &sourceId )
{
    emit javaScriptMessage( msg, lineNumber, sourceId );
}

QWebPage* AMPWebPage::createWindow( QWebPage::WebWindowType type )
{
    if ( type==QWebPage::WebBrowserWindow ) qDebug( "request browser window" );
    else qDebug( "request modal window" );
    /** @todo Add popup blocker */
    /*
    AMPWebView *parentView=qobject_cast<AMPWebView*>(parent());
    // if opened window should be part of the tab widget:
    QObject *par;
    if ( parentView ) par=parentView->tabWidget();
    AMPWebView *view;
    AMPTabWidget *tab=qobject_cast<AMPTabWidget*>(par);
    if ( tab ) {
        view=new AMPWebView( networkAccessManager(), tab );
        tab->newTabRequested( view );
    } else {
    */
        AMPWebView *view=new AMPWebView( 0, 0 );
        view->setWindowTitle( view->title() );
        view->setWindowIcon( view->icon() );
        view->setAttribute( Qt::WA_DeleteOnClose );
        connect( view, SIGNAL( geometryChangeRequested( const QRect& ) ),
            view, SLOT( slotUpdateGeometry( const QRect& ) ) );
        connect( view, SIGNAL( windowCloseRequested() ), view, SLOT( close() ) );
        view->show();
    // }
    return qobject_cast<QWebPage*>(view->webPage());
}
