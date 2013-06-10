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
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QVBoxLayout>
#include <QMessageBox>
#include <QScriptValue>
#include <QScriptContext>
#include <QDateTime>
#include <QTimer>
#include <QLocale>
#include <QFileDialog>
#include <QSettings>
#include <QPrinter>
#include <QPrintDialog>
#include <QNetworkCookie>
#include "phiawebview.h"
#include "phiaitem.h"
#include "phi.h"
#include "phia.h"
#include "phiatools.h"
#include "phiascriptobjects.h"
#include "phiaformprocessor.h"
#include "phiamainwindow.h"
#include "phiaconfig.h"

PHIAWebView::PHIAWebView( PHIAHistory *history, QWidget *parent )
    : PHIAAbstractWebView( history, parent ), _ioDev( 0 ), _engine( 0 ),
    _isLoading( false ), _readingType( PHIAWebView::RTHeader ), _zoomFactor( 1. )
{
    qDebug( "PHIAWebView::PHIAWebView()" );
    _page=new PHIAPage( this );
    _scene=new PHIAScene( this );
    _scene->setWebView( this ); //needed for Drag&Drop
    _view=new PHIAGraphicsView( this );
    _view->setScene( _scene );
    //_streamBuffer.setData( _stream );

    QVBoxLayout *layout=new QVBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0 ,0 );
    layout->addWidget( _view );
    setLayout( layout );

    connect( _scene, SIGNAL( returnButtonPressed() ), this, SLOT( slotReturnButtonPressed() ), Qt::QueuedConnection );
    connect( _page, SIGNAL( titleChanged() ), this, SLOT( slotTitleChanged() ) );
    connect( _page, SIGNAL( iconChanged() ), this, SLOT( slotIconChanged() ) );
}

PHIAWebView::~PHIAWebView()
{
    delete _ioDev;
    delete _page;
    delete _view;
    delete _scene;
    qDebug( "PHIAWebView::~PHIAWebView()" );
}

/*
void PHIAWebView::switchBuffer()
{
    if ( _page==_page1 ) _page=_page2;
    else _page=_page1;
    if ( _scene==_scene1 ) _scene=_scene2;
    else _scene=_scene1;
}
*/

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
    if ( !_page ) return tr( "No page loaded." );
    QString tmp=_page->javascript();
    if( tmp.isEmpty() ) tmp=tr( "There is no JavaScript attached to this Phi page." );
    return tmp;
}

int PHIAWebView::selectMenu( const QStringList &items, const QPointF &pos )
{
    QPoint p=_view->mapFromScene( pos );
    QList <QAction*> _actions;
    QAction *act;
    QMenu m;
    QString text;
    foreach ( text, items ) {
        act=m.addAction( text );
        _actions.append( act );
    }
    act=m.exec( mapToGlobal( p ) );
    for ( int i=0; i<_actions.count(); i++ )
        if ( _actions.at( i )==act ) return i;
    return -1;
}

void PHIAWebView::slotNetworkRequest( const QNetworkRequest &request )
{
    qDebug( "slotNetworkRequest()" );
    Q_ASSERT( _networkAccessManager );
    QNetworkReply *reply=_networkAccessManager->get( request );
    if ( reply ) setNetworkReply( reply );
}

void PHIAWebView::setNetworkReply( QNetworkReply *reply )
{
    if ( !reply ) return;
    QNetworkRequest req=reply->request();
    _sslConfig=reply->sslConfiguration();
    _reqUrl=req.url();
    qDebug( "setNetworkReply url=%s", qPrintable( _reqUrl.toString() ) );
    if ( reply->error()!=QNetworkReply::NoError ) {
        qDebug( "reply %d %s %s", reply->error(), qPrintable( reply->errorString() ), qPrintable( req.url().toString() ) );
    } else {
        qDebug( "reply %s", qPrintable( req.url().toString() ) );
    }
    connect( reply, SIGNAL( finished() ), this, SLOT( slotFinished() ) );
    connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this,
        SLOT( slotNetworkError( QNetworkReply::NetworkError ) ) );
    connect( reply, SIGNAL( metaDataChanged() ), this, SLOT( slotMetaDataChanged() ) );
    emit statusBarMessage( _reqUrl.toString(), 30000 );
    //if ( req.url().scheme()=="file" ) setIODevice( reply, true );
    //else setIODevice( reply );
    setIODevice( reply, false );
}

void PHIAWebView::slotSslErrors( QNetworkReply *reply, const QList<QSslError> &errors )
{
    if ( _reqUrl!=reply->request().url() ) return;
    PHIAAbstractWebView::slotSslErrors( reply, errors );
}

void PHIAWebView::slotAuthRequest( QNetworkReply *reply, QAuthenticator *auth )
{
    if ( _reqUrl!=reply->request().url() ) return;
    PHIAAbstractWebView::slotAuthRequest( reply, auth );
}

void PHIAWebView::slotMetaDataChanged()
{
    QNetworkReply *reply=qobject_cast<QNetworkReply*>(_ioDev);
    if ( !reply ) return;
    _sslConfig=reply->sslConfiguration();
    qDebug( "HTTP code %s", reply->attribute( QNetworkRequest::RedirectionTargetAttribute ).toUrl().toString().toLatin1().data() );
    qDebug( "HTTP code %d", reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt() );
}

void PHIAWebView::slotNetworkError( QNetworkReply::NetworkError )
{
    qDebug( "PHIAWebView::slotNetworkError()" );
    // Handled in slotFinished()
    //if ( _ioDev ) {
    //    QMessageBox::warning( this, tr( "Network error" ), _ioDev->errorString(), QMessageBox::Ok );
    //}
}

void PHIAWebView::slotCreateWindow( const QString &u, const QString &target, const QString &opts )
{
    if ( target=="_self" ) {
        PHIAExtractWindowOpts wo( opts );
        QUrl requrl=PHI::createUrlForLink( url(), u );
        slotLinkRequested( requrl );
        QRect r=PHI::defaultWindowSize();
        if ( wo.height()>-1 ) r.setHeight( wo.height() );
        if ( wo.width()>-1 ) r.setWidth( wo.width() );
        if ( wo.top()>-1 ) r.setY( wo.top() );
        if ( wo.left()>-1 ) r.setX( wo.left() );
        if ( r!=PHI::defaultWindowSize() ) emit geometryChangeRequested( r );
        if ( wo.contains( "status" ) ) emit statusBarVisibilityChangeRequested( wo.showStatusBar() );
        if ( wo.contains( "menubar" ) ) emit menuBarVisibilityChangeRequested( wo.showMenuBar() );
        if ( wo.contains( "scrollbars" ) ) emit scrollBarVisibilityChangeRequested( wo.showScrollBars() );
        if ( wo.contains( "toolbar" ) ) emit toolBarVisibilityChangeRequested( wo.showToolBar() );
        if ( wo.contains( "location" ) ) emit locationBarVisibilityChangeRequested( wo.showLocationBar() );
        if ( wo.contains( "resizable" ) ) emit resizableChangeRequested( wo.resizable() );
        setScrollBarsEnabled( wo.showScrollBars() );
    } else PHIAMainWindow::createWindow( u, url(), target, opts );
}

void PHIAWebView::slotRequestPrint()
{
    QPrintDialog printDlg( this );
    connect( &printDlg, SIGNAL( accepted( QPrinter* ) ), this, SLOT( slotPrint( QPrinter* ) ) );
    printDlg.exec();
}

void PHIAWebView::slotPrint( QPrinter *p )
{
    p->setResolution( 1200 );
    QPainter painter( p );
    painter.setRenderHint( QPainter::Antialiasing, true );
    painter.setRenderHint( QPainter::TextAntialiasing, true );
    painter.setRenderHint( QPainter::SmoothPixmapTransform, true );
    renderDocument( &painter );
}

void PHIAWebView::setIODevice( QIODevice *dev, bool callSlotFinished )
{
    qDebug( "setIODevice():%d", callSlotFinished );
    emit loadProgress( 0 );
    emit loading( true );
    _ioDev=dev;
    connect( _ioDev, SIGNAL( readyRead() ), this, SLOT( slotDataAvailable() ) );
    _progress=0;
    _readingType=RTHeader;
    _isLoading=true;
    _contentTypeChecked=false;
    //_stream.clear();
    _bgReply=0;

    slotDataAvailable();
    if ( callSlotFinished ) {
        emit loading( true );
        slotFinished();
        qDebug( "callSlotFinished=true" );
    }
}

void PHIAWebView::slotDataAvailable()
{
    if ( !_ioDev ) return;
    qDebug( "slotDataAvailable()" );
    qint64 length=_ioDev->bytesAvailable();
    if ( length==0 ) return;
    qint64 size;
    QDataStream in( _ioDev );
    in.setVersion( PHI_DSV );
    //QDataStream out( &_stream, QIODevice::WriteOnly );
    //out.setVersion( PHI_DSV );
    qDebug( "slotDataAvailable length=%lld", length );

    QNetworkReply *reply=qobject_cast<QNetworkReply*>(_ioDev);
    if ( !_contentTypeChecked && reply && length > 0 ) {
        QFileInfo info( reply->url().path() );
        if ( reply->header( QNetworkRequest::ContentTypeHeader ).toByteArray()!=PHI::mimeType()
                && info.suffix()!="phi" ) {
            //qWarning( "slotDataAvailable() %s", reply->header( QNetworkRequest::ContentTypeHeader )
            //  .toByteArray().data() );
            disconnect( reply, 0, this, 0 );
            _isLoading=false;
            _ioDev=0;
            //qWarning( "emiting unsupported content" );
            emit loading( false );
            emit unsupportedContent( this, reply );
            return;
        } else {
            //switchBuffer();
            qDebug( "_page->clearItems() reqUrl=%s - replyUrl=%s", qPrintable( _reqUrl.toString() ),
                    qPrintable( reply->url().toString() ) );
            _page->clearItems();
            _scene->clear();
            if ( _engine ) {
                delete _engine;
                _engine=0;
            }
        }
        _contentTypeChecked=true;
    }

    while ( length>0 ) {
        if ( _readingType==RTHeader ) {
            size=static_cast<qint64>(sizeof(quint32)+(sizeof(quint8)*2) );
            if ( length >= size ) {
                length-=size;
                quint32 magic;
                quint8 version, messageType;
                in >> magic >> version >> messageType;
                //out << magic << version << messageType;
                if ( static_cast<quint32>(PHI_MAGIC)!=magic ) {
                    QMessageBox::critical( this, tr( "Magic number" ),
                        tr( "Content type is '%1' but document contains invalid data." )
                        .arg( QString::fromLatin1( PHI::mimeType() ) ), QMessageBox::Abort );
                    slotFinished();
                    return;
                }
                /*
                qDebug( "version check %d", version );
                if ( static_cast<quint8>(1)<version ) {
                    QMessageBox::critical( this, tr( "Magic number" ),
                        tr( "Content type is '%1' but document contains an unknown version number." )
                        .arg( QString::fromLatin1( PHI::mimeType() ) ), QMessageBox::Abort );
                    slotFinished();
                    return;
                }
                */
                _readingType=RTPageSize;
            } else break;
        }
        if ( _readingType==RTPageSize ) {
            size=static_cast<qint64>(sizeof(quint32));
            if ( length >= size ) {
                length-=size;
                in >> _itemSize;
                //out << _itemSize;
                _readingType=RTPage;
            } else break;
        }
        if ( _readingType==RTPage ) {
            size=static_cast<qint64>(_itemSize);
            if ( length >= size ) {
                qDebug( "reading page %s", qPrintable( _reqUrl.toString() ) );
                length-=size;
                in >> _page;
                //out << _page;
                setupPage();
                _readingType=RTItemSize;
            } else break;
        }
        if ( _readingType==RTItemSize ) {
            size=static_cast<qint64>(sizeof(quint32));
            if ( length >= size ) {
                length-=size;
                in >> _itemSize;
                //out << _itemSize;
                _readingType=RTItem;
            } else break;
        }
        if ( _readingType==RTItem ) {
            size=static_cast<qint64>(_itemSize);
            if ( length >= size ) {
                length-=size;
                PHIAItem *it=new PHIAItem( this );
                in >> it;
                //out << it;
                it->setPalette( _alphaPal );
                _page->insertElementById( it->id(), it );
                _readingType=RTItemSize;
            } else break;
        }
    }
}

void PHIAWebView::slotFinished()
{
    qDebug( "PHIAWebView::slotFinished()" );
    QNetworkReply *reply=qobject_cast<QNetworkReply*>(_ioDev);
    if ( _ioDev ) _ioDev->deleteLater();
    _ioDev=0;

    if ( reply ) {
        qDebug( "valid reply" );
        if ( reply->error()!=QNetworkReply::NoError ) {
            QMessageBox::warning( this, tr( "Network error"),
                tr( "Reply error string:")+"\n"+reply->errorString() );
            return;
        }
        _sslConfig=reply->sslConfiguration();
        QUrl url=reply->attribute( QNetworkRequest::RedirectionTargetAttribute ).toUrl();
        if ( url.isValid() ) return slotLinkRequested( url );
    }
    //_isLoading=false; //moved to slotItemSetupDone();
    //emit loading( false ); // moved to slotItemSetupDone();

    // updating parent items to layout in containers
    PHIBaseItem *bit;
    foreach ( bit, _page->items() ) {
        PHIBaseItem *parent=_page->items().value( bit->parentId() );
        if ( !parent ) continue;

        PHIAItem *pit=qobject_cast<PHIAItem*>(parent);
        Q_ASSERT(pit);
        pit->addChild( qobject_cast<PHIAItem*>(bit) );
    }
    updateTabOrdering();
    _scene->update();
    setFocus();
    runScript( _page->javascript() );
    QTimer::singleShot( 0, this, SLOT( slotCheckContainerItems() ) );
    qDebug( "slotFinished end" );
}

void PHIAWebView::requestBgImage()
{
    QUrl u=_reqUrl;
    u.setPath( "/phi.phis", QUrl::StrictMode );
    //u.setScheme( QString( "http" ) ); // use unsecure connection

    QList <QPair <QString, QString> > list;
    QPair <QString, QString> pair;
    pair.first="phiimg";
    pair.second=_page->bgImageUrl();
    list << pair;
    if ( _page->bgImageUrl().startsWith( "phi" ) ) {
        pair.first="phitmp";
        pair.second="1";
        list << pair;
    }
    QUrlQuery query;
    query.setQueryItems( list );
    u.setQuery( query );

    QNetworkRequest request;
    request.setUrl( u );
    request.setRawHeader( "Accept", "image/png,image/*;q=0.7" );
    request.setRawHeader( "Referer", url().toEncoded() );
    request.setAttribute( QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache );

    qDebug( "REQUEST BG IMAGE %s", qPrintable( u.toString() ) );
    _bgReply=networkAccessManager()->get( request );
    if ( !_bgReply ) {
        slotRequestForImageFinished();
        return;
    }
    connect( _bgReply, SIGNAL ( finished() ), this, SLOT( slotRequestForImageFinished() ) );
}

void PHIAWebView::slotRequestForImageFinished()
{
    if ( !_bgReply ) return;
    if ( _bgReply->error()!=QNetworkReply::NoError ) {
        qDebug( "Reply error for loading bg image: %s", qPrintable( _bgReply->errorString() ) );
        _bgReply->deleteLater();
        _bgReply=0;
    }
    QByteArray arr=_bgReply->readAll();
    QImage img;
    img.loadFromData( arr );
    _scene->setBgImage( img );
    _bgReply->deleteLater();
    _bgReply=0;
}

void PHIAWebView::runScript( const QString &script )
{
    if ( !_engine ) {
        _engine=new QScriptEngine( this );
        new PHIAScriptWindowObj( this ); // constructor sets _engine as parent
        new PHIAScriptNavigatorObj( this ); // constructor sets _engine as parent
        new PHIAScriptPhiObj( this ); // constructor sets _engine as parent
        PHIAMainWindow *mainwin=qobject_cast<PHIAMainWindow*>(parent());
        if ( _page->attributes() & PHIPage::AApplication && mainwin ) {
            // we need the menu bar object of the main window to initialize the menu for scripting
            new PHIAScriptMenuObj( this, mainwin->menuBar() );
        }
    }
    qDebug( "test evaluate script" );
    if ( _engine->canEvaluate( script ) ) {
        QScriptValue doc=_engine->evaluate( script );
        if ( doc.isError() ) throwJavaScriptError( doc );
    } else {
        emit javaScriptConsoleMessage( tr( "Could not evaluate JavaScript." ), 0, _reqUrl.toString() );
    }
}

void PHIAWebView::throwJavaScriptError( const QScriptValue &err )
{
    if ( !err.isError() || !_engine ) return;
    QStringList list=_engine->uncaughtExceptionBacktrace();
    QString tmp=err.toString();
    tmp+=QString( "\n\n" )+list.join( "\n" );
    emit javaScriptConsoleMessage( tmp, _engine->uncaughtExceptionLineNumber(), _reqUrl.toString() );
}

void PHIAWebView::setupPage()
{
    qDebug( "page loaded %d %s %d", _page->storedItemCount(), qPrintable( _page->title() ), _page->font().pointSize() );
    emit progressMaximum( static_cast<int>(_page->storedItemCount()+1) );
    emit loadProgress( ++_progress );
    _scene->setSceneRect( 0, 0, _page->width(), _page->height() );
    _scene->setBackgroundBrush( QBrush( QColor( _page->bgColor() ) ) );
    if ( _page->attributes() & PHIPage::ADocumentLeft ) _view->setAlignment( Qt::AlignLeft | Qt::AlignTop );
    else _view->setAlignment( Qt::AlignCenter | Qt::AlignTop );
    /*
    QString lang=_page->lang();
    if ( lang.size()==5 ) lang=lang.left( 2 )+'_'+lang.right( 2 ).toUpper();
    QLocale::setDefault( QLocale( lang ) );
    */
    _scene->setFont( _page->font() );
    _scene->setBgOffset( QPoint( _page->bgImageXOff(), _page->bgImageYOff() ) );
    _scene->setBgOptions( _page->bgImageOptions() );
    if ( !_page->bgImageUrl().isEmpty() ) requestBgImage();

    QPalette pal;
    if ( _page->attributes() & PHIPage::APalette ) pal=_page->palette();
    else pal=qApp->palette();
    _alphaPal=pal;
    _alphaPal.setColor( QPalette::Window, QColor( 0, 0, 0, 0 ) );

    setWindowTitle( _page->title() );
    emit urlChanged( _reqUrl );
    emit titleChanged( this );
    emit iconChanged( this );

    PHIAHistoryItem it( _reqUrl, title(), icon(), QDateTime::currentDateTime() );
    _history->addHistoryItem( it );
    emit forwardAvailable( _history->canGoForward() );
    emit backAvailable( _history->canGoBack() );

    PHIAMainWindow *mainwin=qobject_cast<PHIAMainWindow*>(parent());
    if ( mainwin ) {
        if ( _page->menuEntries().count() || !(_page->attributes() & PHIPage::AApplication) ) {
            mainwin->menuBar()->clear();
            qDebug( "mainwin" );
            if ( !(_page->extensions() & PHIPage::EHidePhiMenu) ) {
                qDebug( "hideMenu" );
                mainwin->menuBar()->addMenu( mainwin->phiMenu() );
            }
        }
    } else {
        if ( _page->attributes() & PHIPage::AApplication )
            QTimer::singleShot( 0, this, SLOT( slotCheckAppMode() ) );
    }
}

void PHIAWebView::slotCheckAppMode()
{
    QSettings *s=PHI::globalSettings();
    QString phiapp;
#ifdef Q_OS_WIN
    phiapp=s->value( "BinDir", qApp->applicationDirPath() ).toString()+"\\phiapp.exe";
#else
    phiapp=s->value( "BinDir", qApp->applicationDirPath() ).toString()+"/phiapp";
#endif
    if ( !QFile::exists( phiapp ) ) {
        phiapp=QFileDialog::getOpenFileName( this, tr( "Path to PhiApp" ), qApp->applicationDirPath(),
            tr( "All" )+" (*.*)" );
        if ( !QFile::exists( phiapp ) ) {
            QMessageBox::warning( this, tr( "Error" ),
                tr( "Could not open the PhiApp for the application mode of Phi." ), QMessageBox::Ok );
            return;
        }
    }
    if ( PHIAConfig::instance()->askForApp() ) {
        QMessageBox::Button res=QMessageBox::warning( this, tr( "Start")+" "+PHIA::viewerName(),
            tr( "This page is marked to run in application mode. Do you want to start it?" ),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );
        if ( res==QMessageBox::No ) return;
    }
    qDebug( "phiapp found in %s", qPrintable( phiapp ) );
    QProcess::startDetached( phiapp, QStringList() << _reqUrl.toString() );
}

void PHIAWebView::updateTabOrdering()
{
    QMap <quint16, QGraphicsProxyWidget*> map;
    PHIBaseItem *bit;
    PHIAItem *it;
    foreach ( bit, page()->items() ) {
        it=qobject_cast<PHIAItem*>(bit);
        QGraphicsProxyWidget *p=qgraphicsitem_cast<QGraphicsProxyWidget*>(it->graphicsItem());
        if ( p ) {
            map.insertMulti( it->tabIndex(), p );
            qDebug( "inserting tab order %d %s", it->tabIndex(), qPrintable( it->name() ) );
        }
    }
    QList <QGraphicsProxyWidget*> list=map.values();
    if ( list.isEmpty() ) return;
    QGraphicsWidget::setTabOrder( 0, list.first() );
    for ( int i=0; i<list.count()-1; i++ )
        QGraphicsWidget::setTabOrder( list.at( i ), list.at( i+1 ) );
}

void PHIAWebView::slotCheckContainerItems()
{
    qDebug( "slotCheckContainerItems" );
    PHIBaseItem *bit;
    PHIAItem *it;
    PHIAConfig *conf=PHIAConfig::instance();
    bool autoFillDlgShown=false;
    int i;
    if ( !conf->addresses().isEmpty() ) autoFillDlgShown=true;
    if ( !conf->creditCards().isEmpty() ) autoFillDlgShown=true;
    if ( !conf->contacts().isEmpty() ) autoFillDlgShown=true;

    foreach ( bit, page()->items() ) {
        QStringList menuList;
        it=qobject_cast<PHIAItem*>(bit);
        if ( it->wid()==PHI::LAYOUT_ADDRESS ) {
            QList <PHIAAddress> addrs=conf->addresses();
            if ( addrs.isEmpty() ) {
                if ( autoFillDlgShown ) continue;
                conf->showUsedDialog( this, PHIAConfig::UAutoFillOut, tr( "Auto fill out" ),
                    tr( "Phi can fill out address information automatically "
                    "if you define it in the user profile menu." ) );
                autoFillDlgShown=true;
                continue;
            }
            if ( !conf->showPasswdDialog( this ) ) continue;
            PHIAAddress a;
            i=0;
            if ( addrs.count()>1 ) {
                for ( i=0; i<addrs.count(); i++ ) {
                    a=addrs.at( i );
                    menuList.append( a.firstname+" "+a.lastname+" "+a.street+" "+a.city );
                }
                i=selectMenu( menuList, QPointF( it->x(), it->y() ) );
                if ( i==-1 ) continue;
            }
            a=addrs.at( i );
            PHIA::setContainerLineEdit( it, 0, a.title );
            PHIA::setContainerLineEdit( it, 1, a.firstname );
            PHIA::setContainerLineEdit( it, 2, a.lastname );
            PHIA::setContainerLineEdit( it, 3, a.organisation );
            PHIA::setContainerLineEdit( it, 4, a.unit );
            PHIA::setContainerLineEdit( it, 5, a.street );
            PHIA::setContainerLineEdit( it, 6, a.state );
            PHIA::setContainerLineEdit( it, 7, a.city );
            PHIA::setContainerLineEdit( it, 8, a.zip );
            PHIA::setContainerLineEdit( it, 9, a.country );
        } else if ( it->wid()==PHI::LAYOUT_CREDIT_CARD ) {
            QList <PHIACreditCard> cards=conf->creditCards();
            if ( cards.isEmpty() ) {
                if ( autoFillDlgShown ) continue;
                conf->showUsedDialog( this, PHIAConfig::UAutoFillOut, tr( "Auto fill out" ),
                    tr( "Phi can fill out credit card information automatically "
                    "if you define it in the user profile menu." ) );
                autoFillDlgShown=true;
                continue;
            }
            if ( !conf->showPasswdDialog( this ) ) continue;
            PHIACreditCard card;
            i=0;
            if ( cards.count()>1 ) {
                for ( i=0; i<cards.count(); i++ ) {
                    card=cards.at( i );
                    menuList.append( card.type+" "+card.name+" "+card.valid.toString( "MM yyyy") );
                }
                i=selectMenu( menuList, QPointF( it->x(), it->y() ) );
                if ( i==-1 ) continue;
            }
            card=cards.at( i );
            PHIA::setContainerComboBoxData( it, 0, card.type );
            PHIA::setContainerLineEdit( it, 0, card.name );
            PHIA::setContainerLineEdit( it, 1, card.number );
            QComboBox *box=PHIA::containerComboBox( it, 1 ); // month list
            if ( box ) box->setCurrentIndex( card.valid.month()-1 );
            PHIA::setContainerComboBoxData( it, 2, card.valid.toString( "yyyy" ) );
            PHIA::setContainerLineEdit( it, 2, card.civ );
        } else if ( it->wid()==PHI::LAYOUT_CONTACT || it->wid()==PHI::LAYOUT_DELIVERY ) {
            QList <PHIAContact> contacts=conf->contacts();
            if ( contacts.isEmpty() ) {
                if ( autoFillDlgShown ) continue;
                conf->showUsedDialog( this, PHIAConfig::UAutoFillOut, tr( "Auto fill out" ),
                    tr( "Phi can fill out contact information automatically "
                    "if you define it in the user profile menu." ) );
                autoFillDlgShown=true;
                continue;
            }
            if ( !conf->showPasswdDialog( this ) ) continue;
            PHIAContact c;
            i=0;
            if ( contacts.count()>1 ) {
                for ( i=0; i<contacts.count(); i++ ) {
                    c=contacts.at( i );
                    menuList.append( c.name+" "+c.email );
                }
                i=selectMenu( menuList, QPointF( it->x(), it->y() ) );
                if ( i==-1 ) continue;
            }
            c=contacts.at( i );
            PHIA::setContainerLineEdit( it, 0, c.name);
            PHIA::setContainerLineEdit( it, 1, c.email );
            PHIA::setContainerLineEdit( it, 2, c.mobile );
            PHIA::setContainerLineEdit( it, 3, c.phone );
            PHIA::setContainerLineEdit( it, 4, c.fax );
            PHIA::setContainerLineEdit( it, 5, c.url );
        } else if ( it->wid()==PHI::LAYOUT_LOGIN ) {
            QList <PHIAContact> contacts=conf->contacts();
            if ( contacts.isEmpty() ) continue;
            PHIAContact c;
            i=0;
            if ( contacts.count()>1 ) {
                for ( i=0; i<contacts.count(); i++ ) {
                    c=contacts.at( i );
                    menuList.append( c.name+" "+c.email );
                }
                i=selectMenu( menuList, QPointF( it->x(), it->y() ) );
                if ( i==-1 ) continue;
            }
            c=contacts.at( i );
            PHIA::setContainerLineEdit( it, 0, c.email );
        } else if ( it->wid()==PHI::LAYOUT_REGISTER ) {
            QList <PHIAContact> contacts=conf->contacts();
            if ( contacts.isEmpty() ) continue;
            PHIAContact c;
            i=0;
            if ( contacts.count()>1 ) {
                for ( i=0; i<contacts.count(); i++ ) {
                    c=contacts.at( i );
                    menuList.append( c.name+" "+c.email );
                }
                i=selectMenu( menuList, QPointF( it->x(), it->y() ) );
                if ( i==-1 ) continue;
            }
            c=contacts.at( i );
            PHIA::setContainerLineEdit( it, 0, c.name );
            PHIA::setContainerLineEdit( it, 1, c.email );
        } else if ( it->wid()==PHI::LAYOUT_PERIOD ) {
            /** @todo add autofillout for period */
        }
    }
    qDebug( "slotCheckContainerItems end" );
}

void PHIAWebView::slotItemSetupDone( const PHIAItem* )
{
    emit loadProgress( ++_progress );
    if ( _progress >_page->storedItemCount() ) {
        _isLoading=false;
        emit loading( false );
        if ( _engine ) {
            QScriptValue phi=_engine->globalObject().property( "phi" );
            if ( phi.isValid() ) {
                QScriptValue func=phi.property( "onload" );
                if ( func.isFunction() ) {
                    QScriptValue err=func.call();
                    if ( err.isError() ) throwJavaScriptError( err );
                }
            }
        }
    }
}

void PHIAWebView::slotTitleChanged()
{
    emit titleChanged( this );
    PHIAHistoryStorage::instance()->updateTitle( _reqUrl, title() );
}

void PHIAWebView::slotIconChanged()
{
    emit iconChanged( this );
    PHIAHistoryStorage::instance()->updateIcon( _reqUrl, icon() );
}

void PHIAWebView::slotLinkRequested( const QUrl &url )
{
    // PHI::createUrlForLink() is done already in the PHIAItem class
    // we can asume a valid link
    qDebug( "slotLinkRequested %s", qPrintable( url.toString() ) );

    QNetworkRequest request;
    request.setUrl( url );
    request.setRawHeader( "Referer", _reqUrl.toEncoded() );
    //request.setAttribute( QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork );
    slotNetworkRequest( request );
}

void PHIAWebView::slotResetForm()
{
    qDebug( "slotResetForm()" );
    PHIBaseItem *it;
    foreach( it, _page->items() ) it->resetItem();
}

void PHIAWebView::slotReturnButtonPressed()
{
    QHash <QByteArray, PHIBaseItem*> items=_page->items();
    PHIBaseItem *it;
    int count=0;
    foreach ( it, items ) {
        if ( it->wid()==PHI::SUBMIT_BUTTON ) count++;
    }
    if ( count==1 ) {
        foreach ( it, items ) {
            if ( it->wid()==PHI::SUBMIT_BUTTON ) {
                slotSubmitFormRequested( it->name() );
                return;
            }
        }
    }
}

void PHIAWebView::slotSubmitFormRequested( const QString &buttonid )
{
    if ( !_engine ) return slotSubmitForm( buttonid ); // no JavaScript attached so we can send form
    emit checkSubmit( buttonid );
}

void PHIAWebView::slotSubmitForm( const QString &buttonid )
{
    qDebug( "slotSubmitForm()" );
    PHIAFormProcessor *fp=new PHIAFormProcessor( this );
    QNetworkReply *reply=fp->process( _page, buttonid );
    setNetworkReply( reply );
}

void PHIAWebView::slotBack()
{
    if ( !canGoBack() ) return;
    _history->back();
    PHIAHistoryItem it=_history->currentItem();
    emit backAvailable( _history->canGoBack() );
    QNetworkRequest req;
    req.setUrl( it.url() );
    slotNetworkRequest( req );
}

void PHIAWebView::slotForward()
{
    if ( !canGoForward() ) return;
    _history->forward();
    PHIAHistoryItem it=_history->currentItem();
    emit forwardAvailable( _history->canGoForward() );
    QNetworkRequest req;
    req.setUrl( it.url() );
    slotNetworkRequest( req );
}

void PHIAWebView::slotStop()
{
    QNetworkReply *reply=qobject_cast<QNetworkReply*>(_ioDev);
    if ( !reply ) return;
    reply->abort();
    reply->deleteLater();
    _ioDev=0;
    _isLoading=false;
    emit loading( false );
}

void PHIAWebView::slotReload()
{
    QNetworkRequest req;
    req.setUrl( _reqUrl );
    slotNetworkRequest( req );
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
