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
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QUndoStack>
#include <QTimer>
#include <QPainter>
#include <QGraphicsView>
#include "phigraphicsscene.h"
#include "phibaseitem.h"
#include "phibasepage.h"
#include "phiitemfactory.h"
#include "phigraphicsitem.h"
#include "phiundo.h"

qreal PHIGraphicsScene::_selectAnimOff=0;

PHIGraphicsScene::PHIGraphicsScene( QObject *parent )
    : QGraphicsScene( parent ), _page( 0 ), _gridSize( 16 )
{
    qDebug( "PHIGraphicsScene::PHIGraphicsScene()" );
    _page=new PHIBasePage( this );
    _undoStack=new QUndoStack( this );
    _selectAnimTimer=new QTimer( this );
    connect( _undoStack, &QUndoStack::cleanChanged, this, &PHIGraphicsScene::cleanChanged );
    connect( _selectAnimTimer, &QTimer::timeout, this, &PHIGraphicsScene::updateSelectAnimation );
    connect( _page, &PHIBasePage::documentSizeChanged, this, &PHIGraphicsScene::documentSizeChanged );
    documentSizeChanged();
}

PHIGraphicsScene::~PHIGraphicsScene()
{
    delete _page; // don't let delete this with usual QObject parent!!!
    qDebug( "PHIGraphicsScene::~PHIGraphicsScene()" );
}

void PHIGraphicsScene::documentSizeChanged()
{
    setSceneRect( QRectF( QPoint( -50, -50 ), QSizeF( _page->width()+100, _page->height()+100 ) ) );
}

void PHIGraphicsScene::saveAs( const QString &f )
{
    setObjectName( f );
    save();
}

void PHIGraphicsScene::open( const QString &f )
{
    setObjectName( f );
}

void PHIGraphicsScene::save()
{
    _undoStack->setClean();
}

QList<PHIBaseItem*> PHIGraphicsScene::selectedBaseItems() const
{
    QList<QGraphicsItem*> items=selectedItems();
    QList<PHIBaseItem*> list;
    QGraphicsItem *it;
    foreach( it, items ) {
        list.append( qgraphicsitem_cast<PHIGraphicsItem*>(it)->phiBaseItem() );
    }
    return list;
}

void PHIGraphicsScene::setAlignment( Qt::Alignment align )
{
    views().first()->setAlignment( align );
}

void PHIGraphicsScene::setSelectAnimation( bool animate )
{
    if ( animate ) {
        _selectAnimTimer->setInterval( 50 );
        _selectAnimTimer->start();
    } else _selectAnimTimer->stop();
}

void PHIGraphicsScene::updateSelectAnimation()
{
    _selectAnimOff+=.25;
    if ( _selectAnimOff>6 ) _selectAnimOff=0;
    QList <QGraphicsItem*> list=selectedItems();
    QGraphicsItem *it;
    foreach( it, list ) {
        it->update();
    }
}

QPointF PHIGraphicsScene::gridPos( const QPointF &p ) const
{
    if ( _gridSize==0 ) return p;
    int x=static_cast<int>(p.x());
    int y=static_cast<int>(p.y());
    int mod=x%_gridSize;
    x=x-(x%_gridSize);
    if ( mod > _gridSize/2 ) x+=_gridSize;
    mod=y%_gridSize;
    y=y-(y%_gridSize);
    if ( mod > _gridSize/2 ) y+=_gridSize;
    return QPointF( x, y );
}

void PHIGraphicsScene::drawBackground( QPainter *painter, const QRectF &rect )
{
    QGraphicsScene::drawBackground( painter, rect );
    painter->setRenderHint( QPainter::Antialiasing, false );
    /*
    if ( p->attributes() & PHIPage::ABgImage ) {
        QPointF off=p->bgImageOffset();
        PHIPage::ImageOptions opts=p->bgImageOptions();
        if ( opts & PHIPage::IFixed ) off+=scene()->views().first()->mapToScene( QPoint( 50, 50 ) );
        QImage img=p->bgImageData()->image();
        if ( !img.isNull() ) {
            painter->translate( off );
            if ( opts & PHIPage::IRepeatX && opts & PHIPage::IRepeatY ) {
                for ( qreal x=-img.width(); x<rect().width()+img.width(); x=x+img.width() ) {
                    for ( qreal y=-img.height(); y<rect().height()+img.height(); y=y+img.height() ) {
                        painter->drawImage( x, y, img );
                    }
                }
            } else if ( opts & PHIPage::IRepeatX ) {
                for ( qreal x=-img.width(); x<rect().width()+img.width(); x=x+img.width() ) {
                    painter->drawImage( x, 0, img );
                }
            } else if ( opts & PHIPage::IRepeatY ) {
                for ( qreal y=-img.height(); y<rect().height()+img.height(); y=y+img.height() ) {
                    painter->drawImage( 0, y, img );
                }
            } else painter->drawImage( 0, 0, img );
            painter->translate( -off );
        }
    }
    */
    QRect r=page()->rect();
    if ( _gridSize!=0 ) {
        QPen pen( Qt::darkGray );
        painter->setPen( pen );
        for ( int x=0; x<r.width(); x+=_gridSize ) {
            for ( int y=0; y<r.height(); y+=_gridSize ) {
                painter->drawPoint( x, y );
            }
        }
    }
    painter->setBrush( Qt::NoBrush );
    painter->drawRect( r );
}

void PHIGraphicsScene::mouseMoveEvent( QGraphicsSceneMouseEvent *e )
{
    QGraphicsScene::mouseMoveEvent( e );
    emit mousePos( e->scenePos() );
}

void PHIGraphicsScene::drawForeground( QPainter *painter, const QRectF &rect )
{
    QGraphicsScene::drawForeground( painter, rect );
}

void PHIGraphicsScene::dragEnterEvent( QGraphicsSceneDragDropEvent *e )
{
    // allways accepts event for dispatching to items:
    QGraphicsScene::dragEnterEvent( e );
#ifdef PHIDEBUG
    const QMimeData *md=e->mimeData();
    qDebug() << md->formats() << md->urls() << md->text();
#endif
}

void PHIGraphicsScene::dragMoveEvent( QGraphicsSceneDragDropEvent *e )
{
    QGraphicsScene::dragMoveEvent( e );
    _handleOwnDragDropEvent=false;
    if ( e->isAccepted() ) {
        qDebug( "scene: drag move accepted by item" );
        return;
    }
    PHIWID wid=widFromMimeData( e->mimeData() );
    if ( wid ) {
        _handleOwnDragDropEvent=true;
        e->setDropAction( Qt::CopyAction );
        e->accept();
    }
}

void PHIGraphicsScene::dragLeaveEvent( QGraphicsSceneDragDropEvent *e )
{
    qDebug( "scene drag leave" );
    QGraphicsScene::dragLeaveEvent( e );
}

void PHIGraphicsScene::dropEvent( QGraphicsSceneDragDropEvent *e )
{
    QGraphicsScene::dropEvent( e );
    if ( !_handleOwnDragDropEvent ) return;
    PHIWID wid=widFromMimeData( e->mimeData() );
    if ( wid ) {
        PHIBaseItem *it=PHIItemFactory::instance()->item( wid, PHIBaseItem::TIDEItem, page() );
        if ( !it ) return;
        it->setPos( gridPos( e->scenePos() ) );
        it->setId( page()->nextFreeId( it->listName().toLower() ) );
        _undoStack->push( new PHIUndoAdd( it, this ) );
        emit newBaseItemAdded( it ); // initialize item
        clearSelection();
        it->setSelected( true );
        views().first()->setFocus();
        setFocusItem( it->graphicsWidget() );
    }
}

void PHIGraphicsScene::keyPressEvent( QKeyEvent *e )
{
    QGraphicsScene::keyPressEvent( e );
    if ( e->isAccepted() ) return;
    if ( e->key()==Qt::Key_Backspace || e->key()==Qt::Key_Delete ) {
        e->accept();
        deleteSelectedBaseItems();
        return;
    }
    int k=e->key();
    if ( k!=Qt::Key_Left && k!=Qt::Key_Up && k!=Qt::Key_Down && k!=Qt::Key_Right  ) return;
    QList <PHIBaseItem*> list=selectedBaseItems();
    if ( list.count() ) {
        PHIBaseItem *it;
        if ( list.count()>1 ) undoStack()->beginMacro( tr( "Move" ) );
        foreach( it, list ) {
            QPointF old=it->pos();
            if ( k==Qt::Key_Left ) it->setPos( it->pos().x()-1, it->pos().y() );
            else if ( k==Qt::Key_Right ) it->setPos( it->pos().x()+1, it->pos().y() );
            else if ( k==Qt::Key_Up ) it->setPos( it->pos().x(), it->pos().y()-1 );
            else it->setPos( it->pos().x(), it->pos().y()+1 );
            undoStack()->push( new PHIUndoMove( it, old ) );
        }
        if ( list.count()>1 ) undoStack()->endMacro();
        e->accept();
    }
}

void PHIGraphicsScene::deleteSelectedBaseItems()
{
    qDebug( "delete items" );
    QList<PHIBaseItem*> list=selectedBaseItems();
    if ( list.isEmpty() ) return;
    if ( list.count()==1 ) {
        _undoStack->push( new PHIUndoDelete( list.first(), this ) );
        return;
    }
    _undoStack->beginMacro( tr( "Delete elements" ) );
    foreach( PHIBaseItem *it, list )
        _undoStack->push( new PHIUndoDelete( it, this ) );
    _undoStack->endMacro();
}

PHIWID PHIGraphicsScene::widFromMimeData( const QMimeData *md )
{
    Q_ASSERT( md );
    if ( md->hasFormat( QLatin1String( "application/x-phi-wid" ) ) ) {
        QByteArray arr=md->data( QLatin1String( "application/x-phi-wid" ) );
        QDataStream ds( &arr, QIODevice::ReadOnly );
        int id;
        ds >> id;
        if ( id ) return static_cast<PHIWID>( id );
    }
    return 0;
}

QPixmap PHIGraphicsScene::pixmapFromMimeData( const QMimeData *md )
{
    Q_ASSERT( md );
    if ( md->hasImage() ) {
        return QPixmap::fromImage( qvariant_cast<QImage>( md->imageData() ) );
    }
    return QPixmap();
}

QString PHIGraphicsScene::pathFromMimeData( const QMimeData *md )
{
    Q_ASSERT( md );
    if ( md->hasText() ) {
        QString file=md->text();
        if ( file.startsWith( L1( "file://" ) ) ) {
            file=file.mid( 7 );
            qDebug( "file=%s", qPrintable( file ) );
            return file;
        }
    }
    return QString();
}

QUrl PHIGraphicsScene::urlFromMimeData( const QMimeData *md )
{
    Q_ASSERT( md );
    if ( md->hasUrls() ) {
        QList<QUrl> list=md->urls();
        QUrl url=list.first();
        qDebug( "url=%s", qPrintable( url.toLocalFile() ) );
        return url;
    }
    return QUrl();
}

QColor PHIGraphicsScene::colorFromMimeData( const QMimeData *md )
{
    Q_ASSERT( md );
    if ( md->hasColor() ) {
        QColor c=md->colorData().value<QColor>();
        return c;
    }
    return QColor();
}
