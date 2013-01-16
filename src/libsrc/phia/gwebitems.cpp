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
#include <QAction>
#include <QWebPage>
#include <QWebFrame>
#include "gwebitems.h"
#include "phiawebview.h"
#include "phiaitem.h"
#include "phia.h"
#include "phi.h"
#include "phianetmanager.h"

PHIAWebPage::PHIAWebPage( QWidget *parent )
    : QWebPage( parent )
{
    QWebPage *page=this;
    page->setNetworkAccessManager( PHIANetManager::instance()->networkAccessManagerNoPhiAccept() );
    page->settings()->setAttribute( QWebSettings::PluginsEnabled, false );
    page->settings()->setAttribute( QWebSettings::JavascriptCanOpenWindows, true );
    page->settings()->setAttribute( QWebSettings::LocalContentCanAccessRemoteUrls, true );
    page->settings()->setAttribute( QWebSettings::JavascriptEnabled, true );
    page->mainFrame()->setScrollBarPolicy( Qt::Vertical, Qt::ScrollBarAlwaysOff );
    page->mainFrame()->setScrollBarPolicy( Qt::Horizontal, Qt::ScrollBarAlwaysOff );
    page->setLinkDelegationPolicy( QWebPage::DelegateExternalLinks );
}

PHIAWebPage::~PHIAWebPage()
{
}

QWebPage* PHIAWebPage::createWindow( QWebPage::WebWindowType type )
{
    Q_UNUSED( type );
    qDebug( "CREATE window" );
    _view=new QWebView();
    PHIAWebPage *p=new PHIAWebPage( 0 );
    _view->setPage( p );
    _view->setWindowTitle( _view->title() );
    _view->setWindowIcon( _view->icon() );
    _view->setAttribute( Qt::WA_DeleteOnClose );
    connect( p, SIGNAL( geometryChangeRequested( const QRect& ) ),
        this, SLOT( slotUpdateGeometry( const QRect& ) ) );
    connect( p, SIGNAL( windowCloseRequested() ), _view, SLOT( close() ) );
    _view->show();
    return p;
}

void PHIAWebPage::slotUpdateGeometry( const QRect &r )
{
    _view->resize( r.width(), r.height() );
}

GWebItem::GWebItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : GWidgetItem( w, h, it, parent )
{
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    _view=new QWebView();
    /*
    PHIAWebPage *page=new PHIAWebPage( _view );
    page->mainFrame()->setScrollBarPolicy( Qt::Vertical, Qt::ScrollBarAsNeeded );
    page->mainFrame()->setScrollBarPolicy( Qt::Horizontal, Qt::ScrollBarAsNeeded );
    _view->setPage( page );
    */
    QWebPage *page=_view->page();
    page->settings()->setAttribute( QWebSettings::PluginsEnabled, true );
    setWidget( _view );
    connect( page, SIGNAL( linkHovered( const QString&, const QString&, const QString& ) ),
        it, SIGNAL( linkHovered( const QString&, const QString&, const QString& ) ) );
    connect( page, SIGNAL( linkClicked( const QUrl& ) ),
        it, SLOT( slotLinkActivated( const QUrl& ) ) );
    resize( w, h );
}

GWebItem::~GWebItem()
{
}

void GWebItem::setUrl( const QString &u )
{
    _url.setUrl( u );
    if ( u.isEmpty() ) {
        _view->setContent( "<html><head></head><body></body></html>" );
        return;
    }
    qDebug( "------set url %s", qPrintable( _url.toString() ) );
    _view->setUrl( _url );
}

GExternalWebItem::GExternalWebItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : GWidgetItem( w, h, it, parent )
{
    _view=new QWebView();
    setWidget( _view );
    setPreferredSize( w, h );
    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );

    PHIAWebPage *page=new PHIAWebPage( _view );
    _view->setPage( page );
    QPalette palette=_view->palette();
    palette.setBrush( QPalette::Base, Qt::transparent ) ;
    //page->setPalette( palette );
    _view->setPalette( palette );
    _view->setAttribute( Qt::WA_OpaquePaintEvent, false );
    resize( w, h );
    connect( page, SIGNAL( linkHovered( const QString&, const QString&, const QString& ) ),
        it, SIGNAL( linkHovered( const QString&, const QString&, const QString& ) ) );
    connect( page, SIGNAL( linkClicked( const QUrl& ) ),
        it, SLOT( slotLinkActivated( const QUrl& ) ) );
}

GExternalWebItem::~GExternalWebItem()
{
}

void GExternalWebItem::setContent( const QByteArray &arr )
{
    _view->setContent( arr, "text/html", _item->view()->url() );
}

void GExternalWebItem::setWidth( qreal w )
{
    setPreferredWidth( w );
    resize( w, height() );
    setValue( _value );
}

void GExternalWebItem::setHeight( qreal h )
{
    setPreferredHeight( h );
    resize( width(), h );
    setValue( _value );
}

GStaticGoogleMapItem::GStaticGoogleMapItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : GExternalWebItem( w, h, it, parent )
{
    setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
}

GStaticGoogleMapItem::~GStaticGoogleMapItem()
{
}

void GStaticGoogleMapItem::setValue( const QString &s )
{
    qDebug( "SET CONTENT %s", qPrintable(s) );
    QByteArray arr="<html><head><style>body { margin:0; padding:0; }</style></head><body>"
        "<img src=\"http://maps.google.com/maps/api/staticmap?size="+QByteArray::number( width() )
        +'x'+QByteArray::number( height() )+'&'+s.toUtf8()+"\"></body></html>";
    setContent( arr );
    _value=s;
}

GTwitterItem::GTwitterItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : GExternalWebItem( w, h, it, parent )
{
    setMaximumSize( PHI::maximumSizeForTweet() );
    setMinimumSize( PHI::minimumSizeForTweet() );
}

GTwitterItem::~GTwitterItem()
{
}

void GTwitterItem::setValue( const QString &v )
{
    QByteArray lang="&lang="+_item->view()->page()->lang().left( 2 ).toLatin1();
    QByteArray arr="<html><head><style>body { margin:0; padding:0; }</style>"
        "<script type=\"text/javascript\" charset=\"utf-8\" src=\"http://platform.twitter.com/widgets.js\"></script></head><body>"
        "<a href=\"https://twitter.com/share?"+v.toUtf8()+lang+"\" class=\"twitter-share-button\">Tweet</a></body></html>";
    setContent( arr );
    _value=v;
}

GFBLikeItem::GFBLikeItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : GExternalWebItem( w, h , it, parent )
{
}

GFBLikeItem::~GFBLikeItem()
{
}

void GFBLikeItem::setValue( const QString &v )
{
    QByteArray lang=_item->view()->page()->lang().toLatin1();
    lang.replace( "-", "_" );
    if ( lang!="C" ) lang="&locale="+(lang.size()==2?lang+"_"+lang.toUpper():lang);
    else lang="";
    QByteArray arr="<html><head><style>body { margin:0; padding:0; background-color:transparent; }</style></head>"
        "<body><iframe src=\"http://www.facebook.com/plugins/like.php?"+v.toUtf8()+"&width="
        +QByteArray::number( static_cast<int>(width()) )+"&height="
        +QByteArray::number( static_cast<int>(height()) )+lang+"\" scrolling=\"no\" frameborder=\"0\" style=\""
        "border:none;overflow:hidden;width:"+QByteArray::number( static_cast<int>(width()) )+"px;height:"
        +QByteArray::number( static_cast<int>(height()) )+"px;\" allowTransparency=\"true\"></iframe>"
        "</body></html>";
    setContent( arr );
    _value=v;
}

GGooglePlusItem::GGooglePlusItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : GExternalWebItem( w, h , it, parent )
{
    // using the phi internal NetworkAccessManager does not display the button?!
    _view->page()->setNetworkAccessManager( new QNetworkAccessManager( _view ) );
}

GGooglePlusItem::~GGooglePlusItem()
{
}

void GGooglePlusItem::setValue( const QString &v )
{
    qDebug( "setting GooglePlus %s", qPrintable( v ) );
    QStringList list=v.split( '&', QString::SkipEmptyParts );
    QString tmp, size, href, cb, annotation;
    foreach ( tmp, list ) {
        if ( tmp.startsWith( "href=" ) ) href=tmp.mid( 5 );
        else if ( tmp.startsWith( "size=") ) size=tmp.mid( 5 );
        else if ( tmp.startsWith( "callback=" ) ) cb=tmp.mid( 9 );
        else if ( tmp.startsWith( "annotation=" ) ) annotation=tmp.mid( 11 );
    }
    QByteArray arr="<html><head><style>body { margin:0; padding:0; }</style>"
        "<script type=\"text/javascript\" src=\"http://apis.google.com/js/plusone.js\">{parsetags:'explicit'}</script></head><body>"
        "<g:plusone href=\""+href.toUtf8()+"\" size=\""+size.toUtf8()+"\" callback=\""+cb.toUtf8()+"\" "
        "annotation=\""+annotation.toUtf8()+"\"></g:plusone>"
        "<script type=\"text/javascript\">window.___gcfg={lang:'"
        +_item->view()->page()->lang().left( 2 ).toLatin1()
        +"'};gapi.plusone.go();</script></body></html>";
    setContent( arr );
    _value=v;
}

GExternalView::GExternalView( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : QGraphicsWebView( parent ), _item( it ), _page( 0 )
{
    if ( PHI::isTabOrderItem( it->wid() ) ) setFlag( ItemIsFocusable, true );
    else setFlag( ItemIsFocusable, false );
    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
    setPreferredSize( w, h );
    resize( w, h );


    _page=new PHIAWebPage();
    _page->settings()->setAttribute( QWebSettings::PluginsEnabled, true );
    //_page->mainFrame()->setScrollBarPolicy( Qt::Vertical, Qt::ScrollBarAsNeeded );
    //_page->mainFrame()->setScrollBarPolicy( Qt::Horizontal, Qt::ScrollBarAsNeeded );
    setPage( _page );

    connect( this, SIGNAL( onClick( QGraphicsSceneMouseEvent* ) ),
        it, SLOT( slotOnClick( QGraphicsSceneMouseEvent* ) ) );
    connect( this, SIGNAL( onDblClick( QGraphicsSceneMouseEvent* ) ),
        it, SLOT( slotOnDblClick( QGraphicsSceneMouseEvent* ) ) );
    connect( this, SIGNAL( onFocus( QFocusEvent* ) ), it, SLOT( slotOnFocus( QFocusEvent* ) ) );
    connect( this, SIGNAL( onBlur( QFocusEvent* ) ), it, SLOT( slotOnBlur( QFocusEvent* ) ) );
    connect( this, SIGNAL( onMouseUp( QGraphicsSceneMouseEvent* ) ),
        it, SLOT( slotOnMouseUp( QGraphicsSceneMouseEvent* ) ) );
    connect( this, SIGNAL( onMouseDown( QGraphicsSceneMouseEvent* ) ),
        it, SLOT( slotOnMouseDown( QGraphicsSceneMouseEvent* ) ) );
    connect( this, SIGNAL( onMouseOver( QGraphicsSceneHoverEvent* ) ),
        it, SLOT( slotOnMouseOver( QGraphicsSceneHoverEvent* ) ) );
    connect( this, SIGNAL( onMouseOut( QGraphicsSceneHoverEvent* ) ),
        it, SLOT( slotOnMouseOut( QGraphicsSceneHoverEvent* ) ) );
    connect( this, SIGNAL( onKeyPress( QKeyEvent* ) ), it, SLOT( slotOnKeyPress( QKeyEvent* ) ) );
    connect( this, SIGNAL( onKeyDown( QKeyEvent* ) ), it, SLOT( slotOnKeyDown( QKeyEvent* ) ) );
    connect( this, SIGNAL( onKeyUp( QKeyEvent* ) ), it, SLOT( slotOnKeyUp( QKeyEvent* ) ) );
    connect( this, SIGNAL( onMouseMove( QGraphicsSceneHoverEvent* ) ),
        it, SLOT( slotOnMouseMove( QGraphicsSceneHoverEvent* ) ) );
    connect( this, SIGNAL( onChange() ), it, SLOT( slotOnChange() ) );
}

GExternalView::~GExternalView()
{
    delete _page;
}

void GExternalView::setAccessKey( const QString &k )
{
    if ( !k.length()==1 ) return;
    QAction *act=0;
    QList<QAction*> list=actions();
    if ( !list.isEmpty() ) act=list.first();
    else act=new QAction( this );
    act->setShortcut( QKeySequence( "Shift+Alt+"+k ) );
    addAction( act );
}

void GExternalView::setWidth( qreal w )
{
    setPreferredWidth( w );
    resize( w, height() );
    setValue( _value );
}

void GExternalView::setHeight( qreal h )
{
    setPreferredHeight( h );
    resize( width(), h );
    setValue( _value );
}

void GExternalView::mouseReleaseEvent( QGraphicsSceneMouseEvent *e )
{
    QGraphicsWebView::mouseReleaseEvent( e );
    emit onMouseUp( e );
    if ( boundingRect().contains( e->pos() ) ) emit onClick( e );
}

void GExternalView::mouseDoubleClickEvent( QGraphicsSceneMouseEvent *e )
{
    QGraphicsWebView::mouseDoubleClickEvent( e );
    emit onDblClick( e );
}

void GExternalView::mousePressEvent( QGraphicsSceneMouseEvent *e )
{
    _item->checkForDragInMousePressEvent( e );
    qDebug( "buttons %d", int( e->buttons()) );
    QGraphicsWebView::mousePressEvent( e );
    emit onMouseDown( e );
    e->accept();
}

void GExternalView::mouseMoveEvent( QGraphicsSceneMouseEvent *e )
{
    _item->checkForDragInMouseMoveEvent( e );
    QGraphicsWebView::mouseMoveEvent( e );
    //emit onMouseMove( e );
}

void GExternalView::focusInEvent( QFocusEvent *e )
{
    QGraphicsWebView::focusInEvent( e );
    emit onFocus( e );
}

void GExternalView::focusOutEvent( QFocusEvent *e )
{
    QGraphicsWebView::focusOutEvent( e );
    emit onBlur( e );
}

void GExternalView::hoverEnterEvent( QGraphicsSceneHoverEvent *e )
{
    QGraphicsWebView::hoverEnterEvent( e );
    emit onMouseOver( e );
}

void GExternalView::hoverLeaveEvent( QGraphicsSceneHoverEvent *e )
{
    QGraphicsWebView::hoverLeaveEvent( e );
    emit onMouseOut( e );
}

void GExternalView::hoverMoveEvent( QGraphicsSceneHoverEvent *e )
{
    QGraphicsWebView::hoverMoveEvent( e );
    emit onMouseMove( e );
}

void GExternalView::keyPressEvent( QKeyEvent *e )
{
    emit onKeyDown( e );
    emit onKeyPress( e );
    QGraphicsWebView::keyPressEvent( e );
}

void GExternalView::keyReleaseEvent( QKeyEvent *e )
{
    QGraphicsWebView::keyReleaseEvent( e );
    emit onKeyUp( e );
}

void GExternalView::dragEnterEvent( QGraphicsSceneDragDropEvent *e )
{
    QGraphicsWebView::dragEnterEvent( e );
    _item->checkDragEnterEvent( e );
    qDebug() << "dragEnter " << e->isAccepted();
}

void GExternalView::dropEvent( QGraphicsSceneDragDropEvent *e )
{
    QGraphicsWebView::dropEvent( e );
    _item->checkDropEvent( e );
}

void GExternalView::dragMoveEvent( QGraphicsSceneDragDropEvent *e )
{
    QGraphicsWebView::dragMoveEvent( e );
    _item->checkDragMoveEvent( e );
}

void GExternalView::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    QGraphicsWebView::paint( painter, option, widget );
}

GYouTubeItem::GYouTubeItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : GExternalView( w, h, it, parent )
{
}

GYouTubeItem::~GYouTubeItem()
{
}

void GYouTubeItem::setValue( const QString &s )
{
    qDebug( "SET CONTENT %s", qPrintable(s) );
    QByteArray arr="<html><head><style>body { margin:0; padding:0; }</style></head><body>";
    /*
    arr+="<embed src=\"http://www.youtube-nocookie.com/v/"+s.toLatin1();
    arr+="?version=3\" type=\"application/x-shockwave-flash\" width=\"";
    arr+=QByteArray::number( static_cast<int>(width()) );
    arr+="\" height=\""+QByteArray::number( static_cast<int>(width()*0.5605) );
    arr+="\" allowscriptaccess=\"always\" allowfullscreen=\"true\"></embed></body></html>";

    arr+="<object type=\"application/x-shockwave-flash\" width=\""+QByteArray::number( static_cast<int>(width()) );
    arr+="\" height=\""+QByteArray::number( static_cast<int>(height()) )+"\">";
    arr+="<param name=\"movie\" value=\"http://www.youtube-nocookie.com/v/"+s.toUtf8()+"\">";
    arr+="<param name=\"allowFullScreen\" value=\"true\">";
    arr+="<param name=\"allowscriptaccess\" value=\"always\">";
    arr+="</object></body></html>";
    */
    arr+="<iframe width=\""+QByteArray::number( static_cast<int>(width()) )+"\" height=\""
        +QByteArray::number( static_cast<int>(height()) )+"\" src="
        "\"http://www.youtube-nocookie.com/embed/"+s.toUtf8()
        +"\" frameborder=\"0\" allowfullscreen></iframe></body></html>";

    qDebug( "video %s", arr.data() );
    setContent( arr, "text/html", _item->view()->url() );
    _value=s;
}
