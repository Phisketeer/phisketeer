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
#include "phigraphicsitem.h"
#include "phipalette.h"
#include "phiabstractitems.h"

PHIGraphicsScene::PHIGraphicsScene( QObject *parent )
    : QGraphicsScene( parent ), _page( 0 )
{
    // qDebug( "PHIGraphicsScene::PHIGraphicsScene()" );
    _page=new PHIBasePage( this );
    connect( _page, &PHIBasePage::documentSizeChanged, this, &PHIGraphicsScene::documentSizeChanged );
    documentSizeChanged();
}

PHIGraphicsScene::~PHIGraphicsScene()
{
    foreach ( PHIBaseItem *it, _page->items() ) {
        PHIAbstractLayoutItem *lit=qobject_cast<PHIAbstractLayoutItem*>(it);
        if ( lit ) {
            lit->breakLayout(); // break all page containing layouts
        }
    }
    // don't delete _page with usual QObject parent mechanism
    // graphics items must be deleted before (via the PHIBasePage destructor!)
    delete _page;
    // qDebug( "PHIGraphicsScene::~PHIGraphicsScene()" );
}

void PHIGraphicsScene::documentSizeChanged()
{
    setSceneRect( QRectF( QPoint(), QSizeF( _page->width(), _page->height() ) ) );
}

QList<PHIBaseItem*> PHIGraphicsScene::selectedBaseItems() const
{
    QList<QGraphicsItem*> items=selectedItems();
    QList<PHIBaseItem*> list;
    QGraphicsItem *it;
    foreach( it, items ) {
        list.append( qgraphicsitem_cast<PHIGraphicsItem*>(it)->baseItem() );
    }
    return list;
}

PHIBaseItem* PHIGraphicsScene::focusBaseItem() const
{
    PHIGraphicsItem *it=qgraphicsitem_cast<PHIGraphicsItem*>(focusItem());
    if ( !it ) return 0;
    return it->baseItem();
}

void PHIGraphicsScene::setAlignment( Qt::Alignment align )
{
    views().first()->setAlignment( align );
}

void PHIGraphicsScene::drawBackground( QPainter *painter, const QRectF &rect )
{
    QGraphicsScene::drawBackground( painter, rect );
}

void PHIGraphicsScene::mouseMoveEvent( QGraphicsSceneMouseEvent *e )
{
    QGraphicsScene::mouseMoveEvent( e );
    emit mousePos( e->scenePos() );
}
