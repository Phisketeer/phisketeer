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
#include <QSizePolicy>
#include <QSettings>
#include <QFile>
#include <QDataStream>
#include <QWebSettings>
#include <QMessageBox>
#include "amptools.h"
#include "ampmainwindow.h"
#include "phia.h"
#include "phi.h"
#include "phiahistory.h"
#include "phiaconfig.h"

AMPUrlCombo::AMPUrlCombo( QWidget *parent )
    : QComboBox( parent )
{
    qDebug( "AMPUrlCombo::AMPUrlCombo()" );
    setEditable( true );
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    setMaximumWidth( 400 );
    setAutoCompletion( true );
    setInsertPolicy( QComboBox::InsertAlphabetically );

    PHIAHistoryStorage *storage=PHIAHistoryStorage::instance();
    connect( storage, SIGNAL( historyItemsChanged() ), this, SLOT( slotHistoryChanged() ) );
    if ( storage->currentItem().isValid() ) _urlString=storage->currentItem().url().toString();
    slotHistoryChanged();
}

AMPUrlCombo::~AMPUrlCombo()
{
    qDebug( "AMPUrlCombo::~AMPUrlCombo()" );
}

void AMPUrlCombo::slotSetCurrentUrl( const QUrl &url )
{
    qDebug( "AMPUrlCombo::slotSetCurrentUrl( %s )", qPrintable( url.toString() ) );
    _urlString=url.toString();
    blockSignals( true );
    int pos=findText( _urlString );
    if ( pos > -1 ) {
        setCurrentIndex( pos );
    } else {
        setCurrentIndex( 0 );
    }
    blockSignals( false );
}

void AMPUrlCombo::slotHistoryChanged()
{
    qDebug( "AMPUrlCombo::slotHistoryChanged( %s )", qPrintable( _urlString ) );
    blockSignals( true );
    clear();
    QList <PHIAHistoryItem> list=PHIAHistoryStorage::instance()->allItems();
    PHIAHistoryItem it;
    foreach ( it, list ) addItem( it.icon(), it.url().toString() );

    int pos=findText( _urlString );
    if ( pos > -1 ) {
        setCurrentIndex( pos );
    } else {
        setCurrentIndex( 0 );
    }
    blockSignals( false );
    qDebug( "AMPUrlCombo::slotHistoryChanged end" );
}

AMPTabWidget::AMPTabWidget( QWidget *parent )
    : QTabWidget( parent )
{
    qDebug( "AMPTabWidget::AMPTabWidget()" );
    setTabsClosable( true );
    connect( this, SIGNAL( tabCloseRequested( int ) ), this, SLOT( slotTabCloseRequested( int ) ) );
    connect( this, SIGNAL( currentChanged( int ) ), this, SLOT( slotCurrentTabChanged( int ) ) );
    tabBar()->hide();
    newTabRequested();
}

AMPTabWidget::~AMPTabWidget()
{
    qDebug( "AMPTabWidget::~AMPTabWidget()" );
}

void AMPTabWidget::connectSignals( PHIAAbstractWebView *view )
{
    qDebug( "AMPTabWidget::connectSignales" );
    Q_ASSERT( view );
    connect( view, SIGNAL( titleChanged( PHIAAbstractWebView* ) ), this,
        SLOT( slotTitleChanged( PHIAAbstractWebView* ) ) );
    connect( view, SIGNAL( iconChanged( PHIAAbstractWebView* ) ), this,
        SLOT( slotIconChanged( PHIAAbstractWebView* ) ) );
    connect( view, SIGNAL( unsupportedContent( PHIAAbstractWebView*, QNetworkReply* ) ), this,
        SLOT( slotUnsupportedContent( PHIAAbstractWebView*, QNetworkReply* ) ) );
}

void AMPTabWidget::newTabRequested( PHIAAbstractWebView *newWebView, int index )
{
    qDebug( "AMPTabWidget::newTabRequested" );
    if ( !newWebView ) newWebView=new AMPWebView( 0, this );
    connectSignals( newWebView );
    QString title=tr( "New page" );
    if ( index==-1 ) addTab( newWebView, newWebView->icon(), title );
    else insertTab( index, newWebView, newWebView->icon(), title );
    setCurrentWidget( newWebView );

    AMPMainWindow *mw=qobject_cast<AMPMainWindow*>(parent());
    Q_ASSERT( mw );
    mw->setCurrentView( newWebView );
    if ( count()>1 ) tabBar()->show();
}

int AMPTabWidget::tabIdForView( PHIAAbstractWebView* view )
{
    PHIAAbstractWebView *v;
    int i;
    for ( i=0; i<count(); i++ ) {
        v=qobject_cast<PHIAAbstractWebView*>(widget(i));
        if ( v==view ) return i;
    }
    return -1;
}

void AMPTabWidget::slotUnsupportedContent( PHIAAbstractWebView *view, QNetworkReply *reply )
{
    if ( !view || !reply ) return;
    PHIAHistory *history=view->history();
    QString contentType=reply->header( QNetworkRequest::ContentTypeHeader ).toString();
    qDebug( "AMPTabWidget::slotUnsupportedContent %s", qPrintable( contentType ) );

    bool deleteView=false;
    bool setReply=false;
    //PHIAWebView *phiView=qobject_cast<PHIAWebView*>(view);
    //AMPWebView *htmlView=qobject_cast<AMPWebView*>(view);
    int tabId=tabIdForView( view );
    if ( tabId>-1 ) {
        setCurrentIndex( tabId );
        slotCurrentTabChanged( tabId );
        //removeTab( tabId );
    }

    PHIAAbstractWebView *newView=0;
    if ( contentType.startsWith( QString::fromUtf8( PHI::mimeType() ) ) ) {
        qDebug( "newView with PHI::mimeType()" );
        newView=new PHIAWebView( history, this );
        deleteView=true;
        setReply=true;
    } else if ( contentType.startsWith( QString::fromUtf8( "text/html" ) ) ||
            contentType.startsWith( QString::fromUtf8( "application/xml" ) ) ||
            contentType.startsWith( QString::fromUtf8( "application/xhtml+xml" ) ) ||
            contentType.startsWith( QString::fromUtf8( "text/plain" ) ) ) {
        newView=new AMPWebView( history, this );
        deleteView=true;
        setReply=true;
    } else if ( contentType.isEmpty() ) {
        QString suffix=QFileInfo( reply->request().url().path() ).suffix();
        //if ( suffix=="phi" ){
        //    newView=new PHIAWebView( history, this );
        //} else
        newView=new AMPWebView( history, this );
        deleteView=true;
        if ( suffix=="html" || suffix=="htm" || suffix=="txt" || suffix=="xml" ) {
            // text files handled by WebKit
            setReply=true;
        } else {
            downloadUnsupportedContent( reply );
        }
    } else {
        newView=view;
        deleteView=false;
        downloadUnsupportedContent( reply );
        setReply=false;
    }
    Q_ASSERT( newView );
    if ( deleteView ) {
        qDebug( "view deleteLater(), removeTab, newTab" );
        view->deleteLater();
        if ( tabId>-1 ) removeTab( tabId );
        newTabRequested( newView, tabId );
    }
    if ( setReply ) newView->setNetworkReply( reply );
}

void AMPTabWidget::downloadUnsupportedContent( QNetworkReply *reply )
{
    PHIAConfig::instance()->addDownload( reply, this );
}

void AMPTabWidget::slotTitleChanged( PHIAAbstractWebView *view )
{
    int tab=tabIdForView( view );
    if ( tab>-1 ) setTabText( tab, view->title() );
}

void AMPTabWidget::slotIconChanged( PHIAAbstractWebView *view )
{
    int tab=tabIdForView( view );
    if ( tab==-1 ) return;
    if ( tab>-1 ) setTabIcon( tab, view->icon() );
    qDebug( "set tab icon %d", tab );
}

void AMPTabWidget::slotCurrentTabChanged( int tab )
{
    AMPMainWindow *mw=qobject_cast<AMPMainWindow*>(parent());
    Q_ASSERT( mw );
    PHIAAbstractWebView *oldWebView=mw->currentView();
    PHIAAbstractWebView *newWebView=qobject_cast<PHIAAbstractWebView*>(widget(tab));
    if ( oldWebView==newWebView ) return;
    qDebug( "slotCurrentTabChanged count=%d", count() );
    if ( newWebView ) mw->setCurrentView( newWebView );
}

void AMPTabWidget::slotTabCloseRequested( int tab )
{
    qDebug( "slotTabCloseRequested" );
    if ( count()==1 ) return;
    setCurrentIndex( tab );
    PHIAAbstractWebView *v=qobject_cast<PHIAAbstractWebView*>(widget( tab ));
    removeTab( tab );
    v->deleteLater();
    if ( count()==1 ) tabBar()->hide();
    AMPMainWindow *mw=qobject_cast<AMPMainWindow*>(parent());
    Q_ASSERT( mw );
    mw->setCurrentView( qobject_cast<PHIAAbstractWebView*>(currentWidget()) );
}
