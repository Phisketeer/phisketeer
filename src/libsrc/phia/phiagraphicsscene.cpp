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
#include <QScriptEngine>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMessageBox>
#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsSceneDragDropEvent>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QMenuBar>
#include <QMenu>
#include <QPrinter>
#include <QPrintPreviewDialog>
#include <QPrintDialog>
#include <QInputDialog>
#include <QTimer>
#include "phiagraphicsscene.h"
#include "phibasepage.h"
#include "phiawebview.h"
#include "phiitemfactory.h"
#include "phiagraphicsitem.h"
#include "phiabstractitems.h"
#include "phiascriptobjects.h"
#include "phiaappwindow.h"
#include "phiimagerequest.h"

PHIAGraphicsScene::PHIAGraphicsScene( QObject *parent )
    : PHIGraphicsScene( parent ), _reply( 0 ), _engine( 0 )
{
    _printer=new QPrinter();
    _printer->setResolution( 1200 );
    setItemIndexMethod( NoIndex );
}

PHIAGraphicsScene::~PHIAGraphicsScene()
{
    abort();
    delete _engine;
    delete _printer;
}

QScriptEngine* PHIAGraphicsScene::scriptEngine() const
{
    return page()->scriptEngine();
}

PHIAWebView* PHIAGraphicsScene::webView() const
{
    PHIAWebView *view=qobject_cast<PHIAWebView*>(parent());
    Q_ASSERT( view );
    return view;
}

void PHIAGraphicsScene::abort()
{
    if ( _reply ) {
        _reply->abort();
        _reply->deleteLater();
        _reply=0;
    }
}

void PHIAGraphicsScene::setUrl( const QUrl &url )
{
    _requestedUrl=url;
    if ( _reply ) {
        _reply->abort();
        _reply->deleteLater();
    }
    _contentTypeChecked=false;
    QNetworkRequest req( _requestedUrl );
    _reply=webView()->networkAccessManager()->get( req );
    connect( _reply, &QNetworkReply::metaDataChanged, this, &PHIAGraphicsScene::slotMetaDataChanged );
    connect( _reply, &QNetworkReply::readyRead, this, &PHIAGraphicsScene::slotDataAvailable );
    connect( _reply, &QNetworkReply::finished, this, &PHIAGraphicsScene::slotReplyFinished );
    connect( _reply, &QNetworkReply::downloadProgress, webView(), &PHIAAbstractWebView::loadProgress );
    emit webView()->statusBarMessage( _reply->url().toString(), 30000 );
    emit webView()->loading( true );
    slotDataAvailable();
}

void PHIAGraphicsScene::slotMetaDataChanged()
{
    _sslConfig=_reply->sslConfiguration();
}

void PHIAGraphicsScene::slotDataAvailable()
{
    if ( !_reply ) return;
    qint64 size, length=_reply->bytesAvailable();
    if ( length==0 ) return;
    QDataStream in( _reply );
    in.setVersion( PHI_DSV );
    qDebug( "slotDataAvailable length=%lld", length );

    if ( !_contentTypeChecked ) {
        if ( _reply->header( QNetworkRequest::ContentTypeHeader ).toByteArray()!=PHI::phiMimeType() ) {
            disconnect( _reply, 0, this, 0 );
            qDebug() << "unsupported content" << _reply->header( QNetworkRequest::ContentTypeHeader ).toByteArray();
            emit webView()->loading( false );
            emit webView()->unsupportedContent( webView(), _reply->url() );
            _reply->abort();
            return;
        } else {
            delete _engine;
            _engine=0;
            _contentTypeChecked=true;
            _readingType=RTHeader;
        }
    }
    while ( length>0 ) {
        if ( _readingType==RTHeader ) {
            size=static_cast<qint64>(sizeof(quint32)+(sizeof(quint8)*2) );
            if ( length >= size ) {
                length-=size;
                quint32 magic;
                quint8 messageType;
                in >> magic >> _version >> messageType;
                if ( static_cast<quint32>(PHI_MAGIC)!=magic ) {
                    QMessageBox::critical( webView(), tr( "Magic number" ),
                        tr( "Content type is '%1' but document contains invalid data." )
                        .arg( QString::fromLatin1( PHI::phiMimeType() ) ), QMessageBox::Abort );
                    emit webView()->loading( false );
                    disconnect( _reply, 0, this, 0 );
                    _reply->abort();
                    return;
                }
                if ( _version<3 ) {
                    disconnect( _reply, 0, this, 0 );
                    emit webView()->loading( false );
                    emit webView()->unsupportedContent( webView(), _reply->url() );
                    _reply->abort();
                    return;
                }
                _readingType=RTPageSize;
                _layouts.clear();
            } else break;
        }
        if ( _readingType==RTPageSize ) {
            size=static_cast<qint64>(sizeof(quint32));
            if ( length >= size ) {
                length-=size;
                in >> _itemSize;
                _readingType=RTPage;
            } else break;
        }
        if ( _readingType==RTPage ) {
            size=static_cast<qint64>(_itemSize);
            if ( length >= size ) {
                length-=size;
                page()->load( in, static_cast<qint32>(_version), true );
                views().first()->viewport()->hide();
                emit page()->documentSizeChanged();
                if ( page()->flags() & PHIBasePage::FPageLeftAlign ) setAlignment( Qt::AlignLeft | Qt::AlignTop );
                else setAlignment( Qt::AlignHCenter | Qt::AlignTop );
                page()->setBaseUrl( _reply->url() );
                setBackgroundBrush( page()->backgroundColor() );
                emit titleChanged( page()->title() );
                emit iconChanged( QIcon( QPixmap::fromImage( page()->favicon() ) ) );
                if ( !page()->bgImageUrl().isEmpty() ) {
                    QUrl url=page()->baseUrl();
                    if ( page()->bgImageUrl().startsWith( QLatin1Char( '/' ) ) ) {
                        url.setPath( page()->bgImageUrl() );
                    } else {
                        url.setPath( L1( "/phi.phis" ) );
                        QUrlQuery query;
                        query.addQueryItem( L1( "i" ), page()->bgImageUrl() );
                        query.addQueryItem( L1( "t" ) , L1( "1" ) );
                        url.setQuery( query );
                    }
                    PHIImageRequest *req=new PHIImageRequest( this, url );
                    connect( req, &PHIImageRequest::imageReady, this, &PHIAGraphicsScene::slotBgImageReady );
                }
                _readingType=RTItemSize;
            } else break;
        }
        if ( _readingType==RTItemSize ) {
            size=static_cast<qint64>(sizeof(quint32));
            if ( length >= size ) {
                length-=size;
                in >> _itemSize;
                _readingType=RTItem;
            } else break;
        }
        if ( _readingType==RTItem ) {
            size=static_cast<qint64>(_itemSize);
            if ( length >= size ) {
                length-=size;
                QByteArray id, arr;
                quint16 wid;
                in >> id >> wid >> arr;
                PHIBaseItemPrivate p( PHIBaseItemPrivate::TClientItem, page(), new PHIAGraphicsItem() );
                PHIBaseItem *it=PHIItemFactory::instance()->item( static_cast<PHIWID>(wid), p );
                if ( it ) {
                    it->setId( id );
                    it->load( arr, _version );
                    addItem( it->gw() );
                    connect( it, &PHIBaseItem::linkRequested, this, &PHIAGraphicsScene::slotLinkActivated );
                }
                PHIAbstractLayoutItem *l=qobject_cast<PHIAbstractLayoutItem*>(it);
                if ( l ) _layouts.append( l );
                PHIAbstractInputItem *ii=qobject_cast<PHIAbstractInputItem*>(it);
                if ( ii ) {
                    connect( ii, &PHIAbstractInputItem::submitClicked, this,
                        &PHIAGraphicsScene::slotSubmitForm, Qt::QueuedConnection );
                    connect( ii, &PHIAbstractInputItem::resetClicked, this,
                        &PHIAGraphicsScene::slotResetForm, Qt::QueuedConnection );
                    connect( ii, &PHIAbstractInputItem::langChangeRequested, this,
                        &PHIAGraphicsScene::slotLangChangeRequested, Qt::QueuedConnection );
                }
                _readingType=RTItemSize;
            } else break;
        }
    }
}

void PHIAGraphicsScene::slotReplyFinished()
{
    emit webView()->loading( false );
    QUrl url=_reply->attribute( QNetworkRequest::RedirectionTargetAttribute ).toUrl();
    if ( url.isValid() ) return setUrl( url );
    if ( _reply->error()==QNetworkReply::NoError ) init();
    else {
        views().first()->viewport()->show();
        qDebug() << _reply->errorString();
    }
    _reply->deleteLater();
    _reply=0;
}

void PHIAGraphicsScene::init()
{
    emit pagePaletteChanged( page()->phiPalette() );
    emit pageFontChanged( page()->font() );
    updateTabOrder();
    foreach( PHIAbstractLayoutItem *l, _layouts ) l->activateLayout();
    _engine=new QScriptEngine( page() );
    new PHIAScriptWindowObj( webView() ); // constructor sets _engine as parent
    new PHIAScriptNavigatorObj( webView() ); // constructor sets _engine as parent
    new PHIAScriptPhiObj( webView() ); // constructor sets _engine as parent
    PHIAAppWindow *appwin=qobject_cast<PHIAAppWindow*>(webView()->parent());
    // we need the menu bar object of the main window to initialize the menu for scripting
    if ( appwin ) {
        initMenuBar( appwin->menuBar() );
        new PHIAScriptMenuObj( webView(), appwin->menuBar() );
    }
    invalidate();
    QTimer::singleShot( 50, this, SLOT(slotRun()) );
}

void PHIAGraphicsScene::slotRun()
{
    foreach( PHIAbstractLayoutItem *lit, _layouts ) {
        foreach( PHIBaseItem *it, lit->childItems() ) {
            it->gw()->setPos( it->realPos() );
            it->gw()->resize( it->realSize() );
        }
    }
    startAnimations();
    QString script=page()->javascript();
    if ( _engine->canEvaluate( script ) ) {
        QScriptValue doc=_engine->evaluate( script );
        if ( doc.isError() ) {
            webView()->throwJavaScriptError( doc );
            qDebug() << doc.toString();
        } else {
            QScriptValue onload=_engine->globalObject().property( L1( "phi" ) ).property( L1( "onload" ) );
            if ( onload.isFunction() ) doc=onload.call();
            else if ( onload.isString() ) doc=_engine->evaluate( onload.toString() );
            if ( doc.isError() ) {
                webView()->throwJavaScriptError( doc );
                qDebug() << doc.toString();
            }
        }
    } else emit webView()->javaScriptConsoleMessage( tr( "Could not evaluate JavaScript." ), 0, _requestedUrl.toString() );
    views().first()->viewport()->show();
}

void PHIAGraphicsScene::updateTabOrder()
{
    QMultiMap <qint16, QGraphicsWidget*> map;
    foreach ( PHIBaseItem *it, page()->items() ) {
        if ( it->isFocusable() ) map.insert( it->realTabIndex(), it->gw() );
    }
    QList <QGraphicsWidget*> list=map.values();
    for ( int i=0; i<list.count()-1; i++ ) {
        QGraphicsWidget::setTabOrder( list.at( i ), list.at( i+1 ) );
    }
}

void PHIAGraphicsScene::startAnimations()
{
    QList <PHIBaseItem*> list=page()->items();
    foreach ( PHIBaseItem *it, list ) {
        if ( it->effect()->effects()==PHIEffect::ENone ) continue;
        qint32 start, duration;
        quint8 ease;
        if ( it->effect()->effects() & PHIEffect::ERotate ) {
            qreal stepx, stepy, stepz;
            it->effect()->rotate( ease, stepx, stepy, stepz );
            it->rotate( ease, stepx, stepy, stepz );
        }
        if ( it->effect()->effects() & PHIEffect::ERotateIn ) {
            it->effect()->rotateIn( ease, start, duration );
            it->rotateIn( ease, start, duration, PHI::defaultEasingCurve() );
        }
        if ( it->effect()->effects() & PHIEffect::ERotateOut ) {
            it->effect()->rotateOut( ease, start, duration );
            it->rotateOut( ease, start, duration, PHI::defaultEasingCurve() );
        }
        if ( it->effect()->effects() & PHIEffect::EMoveBy ) {
            qint32 dx, dy, dw, dh;
            it->effect()->moveBy( start, duration, dx, dy, dw, dh, ease );
            it->moveBy( dx, dy, dw, dh, start, duration, PHI::toEasingCurveString( ease ) );
        }
        if ( it->effect()->effects() & PHIEffect::EMoveTo ) {
            qint32 left, top;
            it->effect()->moveTo( start, duration, left, top, ease );
            it->moveTo( left, top, start, duration, PHI::toEasingCurveString( ease ) );
        }
        if ( it->effect()->effects() & PHIEffect::EFadeIn ) {
            qreal maxOpac;
            it->effect()->fadeIn( start, duration, maxOpac, ease );
            it->fadeIn( start, duration, maxOpac, PHI::toEasingCurveString( ease ) );
        }
        if ( it->effect()->effects() & PHIEffect::EFadeOut ) {
            qreal minOpac;
            it->effect()->fadeOut( start, duration, minOpac, ease );
            it->fadeOut( start, duration, minOpac, PHI::toEasingCurveString( ease ) );
        }
    }
}

void PHIAGraphicsScene::dragMoveEvent( QGraphicsSceneDragDropEvent *event )
{
    setDragMovePos( event->scenePos() );
    PHIGraphicsScene::dragMoveEvent( event );
}

void PHIAGraphicsScene::drawBackground( QPainter *painter, const QRectF &rect )
{
    PHIGraphicsScene::drawBackground( painter, rect );
    if ( page()->flags() & PHIBasePage::FUseBgImage ) {
        QPointF off=QPointF( page()->bgImageXOff(), page()->bgImageYOff() );
        quint8 opts=page()->bgImageOptions();
        if ( opts & PHIBasePage::IFixed ) off+=views().first()->mapToScene( QPoint() );
        QImage img=page()->bgImage();
        if ( !img.isNull() ) {
            QRectF r=sceneRect();
            painter->translate( off );
            if ( (opts & PHIBasePage::IRepeatX) && (opts & PHIBasePage::IRepeatY) ) {
                for ( qreal x=-img.width(); x<r.width()+img.width(); x=x+img.width() ) {
                    for ( qreal y=-img.height(); y<r.height()+img.height(); y=y+img.height() ) {
                        painter->drawImage( x, y, img );
                    }
                }
            } else if ( opts & PHIBasePage::IRepeatX ) {
                for ( qreal x=-img.width(); x<r.width()+img.width(); x=x+img.width() ) {
                    painter->drawImage( x, 0, img );
                }
            } else if ( opts & PHIBasePage::IRepeatY ) {
                for ( qreal y=-img.height(); y<r.height()+img.height(); y=y+img.height() ) {
                    painter->drawImage( 0, y, img );
                }
            } else painter->drawImage( 0, 0, img );
            painter->translate( -off );
        }
    }
}

void PHIAGraphicsScene::slotBgImageReady( const QImage &img )
{
    page()->setBgImage( img );
}

void PHIAGraphicsScene::slotLinkActivated( const QString &s )
{
    QUrl url=PHI::createUrlForLink( _requestedUrl, s );
    QUrlQuery query( url );
    if ( !query.hasQueryItem( L1( "philang" ) ) ) query.addQueryItem( L1( "philang" ), page()->lang() );
    if ( !query.hasQueryItem( L1( "phisid" ) ) && !page()->session().isEmpty() )
        query.addQueryItem( L1( "phisid" ), page()->session() );
    url.setQuery( query );
    emit linkRequested( url );
}

void PHIAGraphicsScene::slotSubmitForm( const QString &buttonId )
{
    QScriptValue phi=_engine->globalObject().property( L1( "phi" ) );
    Q_ASSERT( phi.isValid() );
    QScriptValue res, func=phi.property( L1( "onsubmit" ) );
    if ( func.isFunction() ) res=func.call( phi );
    if ( res.isValid() && res.isBool() ) { if ( res.toBool()==false ) return; }
    else if ( res.isError() ) emit webView()->throwJavaScriptError( res );
    if ( page()->action().isEmpty() ) return;
    QString submitValue;

    QNetworkAccessManager *nam=webView()->networkAccessManager();
    QHttpMultiPart *multiPart=new QHttpMultiPart( QHttpMultiPart::FormDataType );
    const QList <PHIBaseItem*> list=page()->items();
    foreach ( PHIBaseItem *it, list ) {
        PHIAbstractInputItem *input=qobject_cast<PHIAbstractInputItem*>(it);
        if ( input ) {
            input->clientPostData( multiPart );
            if ( input->name()==buttonId ) submitValue=input->realValue();
        }
    }
    if ( !buttonId.isEmpty() ) {
        QHttpPart part;
        part.setHeader( QNetworkRequest::ContentTypeHeader, BL( "text/plain" ) );
        part.setHeader( QNetworkRequest::ContentDispositionHeader, BL( "form-data; name=\"" )+buttonId.toUtf8()+'"' );
        part.setBody( submitValue.toUtf8() );
        multiPart->append( part );
    }
    if ( !page()->session().isEmpty() ) {
        QHttpPart part;
        part.setHeader( QNetworkRequest::ContentTypeHeader, BL( "text/plain" ) );
        part.setHeader( QNetworkRequest::ContentDispositionHeader, BL( "form-data; name=\"phisid\"" ) );
        part.setBody( page()->session().toUtf8() );
        multiPart->append( part );
    }
    if ( !(page()->flags() & PHIBasePage::FHasPhiLangItem ) ) {
        QHttpPart part;
        part.setHeader( QNetworkRequest::ContentTypeHeader, BL( "text/plain" ) );
        part.setHeader( QNetworkRequest::ContentDispositionHeader, BL( "form-data; name=\"philang\"" ) );
        part.setBody( page()->currentLang() );
        multiPart->append( part );
    }
    QUrl url=PHI::createUrlForLink( _requestedUrl, page()->action() );
    QNetworkRequest req( url );
    if( _reply ) {
        _reply->abort();
        _reply->deleteLater();
    }
    _reply=nam->post( req, multiPart );
    multiPart->setParent( _reply );
    connect( _reply, &QNetworkReply::metaDataChanged, this, &PHIAGraphicsScene::slotMetaDataChanged );
    connect( _reply, &QNetworkReply::readyRead, this, &PHIAGraphicsScene::slotDataAvailable );
    connect( _reply, &QNetworkReply::finished, this, &PHIAGraphicsScene::slotReplyFinished );
    emit webView()->statusBarMessage( _reply->url().toString(), 30000 );
    emit webView()->loading( true );
    _contentTypeChecked=false;
    slotDataAvailable();
}

void PHIAGraphicsScene::slotResetForm( const QString &buttonId )
{
    Q_UNUSED( buttonId )
    const QList<PHIBaseItem*> items=page()->items();
    foreach ( PHIBaseItem *it, items ) it->reset();
}

void PHIAGraphicsScene::slotLangChangeRequested( const QString &lang )
{
    QUrl url=_requestedUrl;
    QUrlQuery query( url );
    query.removeQueryItem( L1( "philang" ) );
    query.addQueryItem( L1( "philang" ), lang );
    url.setQuery( query );
    setUrl( url );
}

void PHIAGraphicsScene::initMenuBar( QMenuBar *bar )
{
    //bar->setNativeMenuBar( false );
    bar->clear();
    if ( page()->flags() & PHIBasePage::FHidePhiMenu ) return;
    qDebug( "add phi menu" );
    QMenu *menu=new QMenu( L1( "PhiApp" ) );
    QAction *act;

    act=menu->addAction( QIcon( L1( ":/gnome/info" ) ), tr( "Page info" ) );
    act->setShortcuts( QKeySequence::HelpContents );
    connect( act, SIGNAL(triggered()), this, SLOT(slotShowPageInfo()) );
    act=menu->addAction( QIcon( L1( ":/gnome/inetfolder" ) ), tr( "Open URL" ) );
    act->setShortcuts( QKeySequence::Open );
    connect( act, SIGNAL(triggered()), this, SLOT(slotOpenUrlTriggered()) );
    act=menu->addAction( QIcon( L1( ":/gnome/script" ) ), tr( "JS console" ) );
    act->setShortcut( QKeySequence( L1( "Ctrl+J" ) ) );
    connect( act, SIGNAL(triggered()), webView(), SIGNAL(showJavaScriptConsole()) );

    QMenu *sub=new QMenu( tr( "Zoom" ) );
    act=sub->addAction( QIcon( L1( ":/gnome/zoomin" ) ), tr( "Zoom in" ) );
    act->setShortcuts( QKeySequence::ZoomIn );
    connect( act, SIGNAL(triggered()), webView(), SLOT(slotZoomIn()) );
    act=sub->addAction( QIcon( L1( ":/gnome/zoomout" ) ), tr( "Zoom out" ) );
    act->setShortcuts( QKeySequence::ZoomOut );
    connect( act, SIGNAL(triggered()), webView(), SLOT(slotZoomOut()) );
    act=sub->addAction( QIcon( L1( ":/gnome/zoomnormal" ) ), tr( "Zoom normal" ) );
    act->setShortcut( QKeySequence( L1( "Ctrl+0" ) ) );
    connect( act, SIGNAL(triggered()), webView(), SLOT(slotZoomNormal()) );
    menu->addMenu( sub );

    menu->addSeparator();
    act=menu->addAction( QIcon( L1( ":/gnome/print" ) ), tr( "Print" ) );
    act->setShortcuts( QKeySequence::Print );
    connect( act, SIGNAL(triggered()), this, SLOT(slotRequestPrint()) );
    act=menu->addAction( QIcon( L1( ":/gnome/printpreview" ) ), tr( "Print preview" ) );
    connect( act, SIGNAL(triggered()), this, SLOT(slotPrintPreview()) );

    menu->addSeparator();
    act=menu->addAction( QIcon( L1( ":/gnome/quit" ) ), tr( "Close" ) );
    act->setShortcuts( QKeySequence::Close );
    connect( act, SIGNAL(triggered()), webView(), SIGNAL(windowCloseRequested()) );

    bar->addMenu( menu );
}

void PHIAGraphicsScene::slotShowPageInfo()
{
    QMessageBox::information( webView(), tr( "Page information" ),
        L1( "<b>" )+tr( "Author:" )+L1( "</b> <i>" )+page()->author()+L1( "</i><br>" )
        +L1( "<b>" )+tr( "Company:" )+L1( "</b> <i>" )+page()->company()+L1( "</i><br>" )
        +L1( "<b>" )+tr( "Copyright:" )+L1( "</b> <i>" )+page()->copyright()+L1( "</i><br>" )
        +L1( "<b>" )+tr( "Version:" )+L1( "</b> <i>" )+page()->version()+L1( "</i><br>" )
        +L1( "<b>" )+tr( "Description:" )+L1( "</b> <i>" )+page()->description()+L1( "</i><br>" ),
        QMessageBox::Ok );
}

void PHIAGraphicsScene::slotOpenUrlTriggered()
{
    QString u=QInputDialog::getText( webView(), tr( "Open new URL" ), tr( "Please enter the new URL" ),
        QLineEdit::Normal, _requestedUrl.toString(), 0, Qt::Sheet );
    QUrl url( u );
    if ( url.isRelative() ) url=PHI::createUrlForLink( _requestedUrl, u );
    setUrl( url );
}

void PHIAGraphicsScene::slotRequestPrint( QPrinter *printer )
{
    if ( !printer ) printer=_printer;
    if ( page()->width()>page()->height() ) printer->setOrientation( QPrinter::Landscape );
    else printer->setOrientation( QPrinter::Portrait );
    QPrintDialog dlg( printer, webView() );
    if ( dlg.exec()!=QPrintDialog::Accepted ) return;
    QPainter p( printer );
    render( &p );
    p.end();
}

void PHIAGraphicsScene::slotPrintPreview()
{
    if ( page()->width()>page()->height() ) _printer->setOrientation( QPrinter::Landscape );
    else _printer->setOrientation( QPrinter::Portrait );
    QPrintPreviewDialog dlg( _printer, webView() );
    connect( &dlg, &QPrintPreviewDialog::paintRequested, this, &PHIAGraphicsScene::slotPaintRequested );
    dlg.exec();
}

void PHIAGraphicsScene::slotPaintRequested( QPrinter *printer )
{
    QPainter p( printer );
    render( &p );
    p.end();
}
