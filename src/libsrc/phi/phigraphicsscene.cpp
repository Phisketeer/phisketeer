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
#include "phigraphicsscene.h"
#include "phibaseitem.h"
#include "phibasepage.h"
#include "phiitemfactory.h"

PHIGraphicsScene::PHIGraphicsScene( QObject *parent )
    : QGraphicsScene( parent ), _page( 0 )
{
    qDebug( "PHIGraphicsScene::PHIGraphicsScene()" );
    _page=new PHIBasePage( this );
}

PHIGraphicsScene::~PHIGraphicsScene()
{
    _items.clear();
    delete _page;
    qDebug( "PHIGraphicsScene::~PHIGraphicsScene()" );
}

void PHIGraphicsScene::drawBackground( QPainter *painter, const QRectF &rect )
{
    QGraphicsScene::drawBackground( painter, rect );
}

void PHIGraphicsScene::drawForeground( QPainter *painter, const QRectF &rect )
{
    QGraphicsScene::drawForeground( painter, rect );
}

void PHIGraphicsScene::addBaseItem( PHIBaseItem *item )
{
    _items.append( item );
    QGraphicsScene::addItem( item->graphicsWidget() );
}

void PHIGraphicsScene::dragEnterEvent( QGraphicsSceneDragDropEvent *e )
{
    qDebug( "scene drag enter" );
    PHIWID wid=widFromMimeData( e->mimeData() );
    if ( wid ) {
        e->setDropAction( Qt::CopyAction );
        e->accept();
        return;
    }
#ifdef PHIDEBUG
    const QMimeData *md=e->mimeData();
    qDebug() << md->formats() << md->urls() << md->text();
#endif
    QGraphicsScene::dragEnterEvent( e );
}

void PHIGraphicsScene::dragMoveEvent( QGraphicsSceneDragDropEvent *e )
{
    PHIWID wid=widFromMimeData( e->mimeData() );
    if ( wid ) return;
    QGraphicsScene::dragMoveEvent( e );
}

void PHIGraphicsScene::dragLeaveEvent( QGraphicsSceneDragDropEvent *e )
{
    qDebug( "scene drag leave" );
    //PHIWID wid=widFromMimeData( e->mimeData() );
    //if ( wid ) return;
    QGraphicsScene::dragLeaveEvent( e );
}

void PHIGraphicsScene::dropEvent( QGraphicsSceneDragDropEvent *e )
{
    qDebug( "scene drop" );
    PHIWID wid=widFromMimeData( e->mimeData() );
    if ( wid ) {
        PHIBaseItem *it;
        it=PHIItemFactory::instance()->item( wid, PHIBaseItem::TIDEItem, _page );
        if ( !it ) return;
        it->setPos( e->scenePos() );
        addBaseItem( it );
    }
    QGraphicsScene::dropEvent( e );
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
