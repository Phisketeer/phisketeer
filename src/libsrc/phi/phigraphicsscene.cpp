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
#include "phigraphicsscene.h"
#include "phibaseitem.h"
#include "phibasepage.h"
#include "phiitemfactory.h"
#include "phigraphicsitem.h"
#include "phiundo.h"

PHIGraphicsScene::PHIGraphicsScene( QObject *parent )
    : QGraphicsScene( parent ), _page( 0 )
{
    qDebug( "PHIGraphicsScene::PHIGraphicsScene()" );
    _page=new PHIBasePage( this );
    _undoStack=new QUndoStack( this );
    setSceneRect( QRectF( QPoint(), QSizeF( _page->width(), _page->height() ) ) );
    connect( _undoStack, &QUndoStack::cleanChanged, this, &PHIGraphicsScene::cleanChanged );
}

PHIGraphicsScene::~PHIGraphicsScene()
{
    delete _page; // don't let delete this with QObject parent!
    qDebug( "PHIGraphicsScene::~PHIGraphicsScene()" );
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
}

void PHIGraphicsScene::setItemFont( const QFont &f )
{

}

void PHIGraphicsScene::drawBackground( QPainter *painter, const QRectF &rect )
{
    QGraphicsScene::drawBackground( painter, rect );
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
    qDebug( "scene dropEvent %d", e->dropAction() );
    if ( !_handleOwnDragDropEvent ) return;
    PHIWID wid=widFromMimeData( e->mimeData() );
    if ( wid ) {
        PHIBaseItem *it=PHIItemFactory::instance()->item( wid, PHIBaseItem::TIDEItem, page() );
        if ( !it ) return;
        it->setPos( e->scenePos() );
        it->setId( page()->nextFreeId( it->listName() ) );
        _undoStack->push( new PHIUndoAdd( it, this ) );
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
    qDebug( "scene key press not accepted" );
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
