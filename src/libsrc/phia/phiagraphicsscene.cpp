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
#include "phiagraphicsscene.h"
#include "phibasepage.h"
#include "phiawebview.h"
#include "phiitemfactory.h"
#include "phiagraphicsitem.h"
#include "phiabstractitems.h"
#include "phiascriptobjects.h"
#include "phiaappwindow.h"

PHIAGraphicsScene::PHIAGraphicsScene( QObject *parent )
    : PHIGraphicsScene( parent ), _reply( 0 ), _engine( 0 )
{
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
    if ( !_reply ) return;
    _reply->abort();
    _reply->deleteLater();
    _reply=0;
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
    emit webView()->statusBarMessage( _requestedUrl.toString(), 30000 );
    emit webView()->loading( true );
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
            emit webView()->unsupportedContent( webView(), _reply );
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
                emit page()->documentSizeChanged();
                if ( page()->flags() & PHIBasePage::FPageLeftAlign ) setAlignment( Qt::AlignLeft | Qt::AlignTop );
                else setAlignment( Qt::AlignHCenter | Qt::AlignTop );
                setBackgroundBrush( page()->backgroundColor() );
                emit titleChanged( page()->title() );
                emit iconChanged( QIcon( QPixmap::fromImage( page()->favicon() ) ) );
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
                }
                PHIAbstractLayoutItem *l=qobject_cast<PHIAbstractLayoutItem*>(it);
                if ( l ) _layouts.append( l );
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
    _reply->deleteLater();
    _reply=0;
}

void PHIAGraphicsScene::init()
{
    qDebug() << "initItems" << _layouts.count();
    emit pagePaletteChanged( page()->phiPalette() );
    emit pageFontChanged( page()->font() );
    foreach( PHIAbstractLayoutItem *l, _layouts ) l->activateLayout();
    _engine=new QScriptEngine( page() );
    new PHIAScriptWindowObj( webView() ); // constructor sets _engine as parent
    new PHIAScriptNavigatorObj( webView() ); // constructor sets _engine as parent
    new PHIAScriptPhiObj( webView() ); // constructor sets _engine as parent
    PHIAAppWindow *appwin=qobject_cast<PHIAAppWindow*>(webView()->parent());
    // we need the menu bar object of the main window to initialize the menu for scripting
    if ( appwin ) new PHIAScriptMenuObj( webView(), appwin->menuBar() );
    qDebug( "evaluate script" );
    QString script=page()->javascript();
    if ( _engine->canEvaluate( script ) ) {
        QScriptValue doc=_engine->evaluate( script );
        if ( doc.isError() ) webView()->throwJavaScriptError( doc );
    } else {
        emit webView()->javaScriptConsoleMessage( tr( "Could not evaluate JavaScript." ), 0, _requestedUrl.toString() );
    }
}

void PHIAGraphicsScene::drawBackground( QPainter *painter, const QRectF &rect )
{
    PHIGraphicsScene::drawBackground( painter, rect );
    if ( page()->flags() & PHIBasePage::FUseBgImage ) {
        QPointF off=QPointF( page()->bgImageXOff(), page()->bgImageYOff() );
        quint8 opts=page()->bgImageOptions();
        if ( opts & PHIBasePage::IFixed ) off+=views().first()->mapToScene( QPoint( 50, 50 ) );
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

void PHIAGraphicsScene::slotSubmitForm( const QString &buttonId )
{
    Q_UNUSED( buttonId )
}

void PHIAGraphicsScene::slotResetForm()
{

}

void PHIAGraphicsScene::slotRequestPrint()
{

}
