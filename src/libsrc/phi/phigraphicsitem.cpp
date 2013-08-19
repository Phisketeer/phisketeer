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
#include <QWidget>
#include <QGraphicsSceneEvent>
#include <QPainter>
#include <QTime>
#include <QApplication>
#include <QFont>
#include "phigraphicsitem.h"
#include "phibaseitem.h"

PHIGraphicsItemProvider* PHIGraphicsItemProvider::_instance=0;

PHIGraphicsItemProvider::PHIGraphicsItemProvider( QObject *parent )
    : QObject( parent ), _page( 0 )
{
    _instance=this;
}

PHIGraphicsItemProvider::~PHIGraphicsItemProvider()
{
    _instance=0;
}

PHIGraphicsItem::PHIGraphicsItem( PHIBaseItem *it )
    : QGraphicsProxyWidget(), _it( it )
{
    Q_ASSERT( it );
    if ( it->isIdeItem() ) {
        setFlag( ItemIsFocusable, true );
        setFlag( ItemIsSelectable, true );
        setFlag( ItemIsMovable, true );
        setAcceptDrops( true );
        setAcceptHoverEvents( true );
    } else if ( it->isClientItem() ) {
        setFlag( ItemIsFocusable, it->isFocusable() );
        setFlag( ItemIsSelectable, true );
        setFlag( ItemIsMovable, false );
        setAcceptDrops( it->isDroppable() );
        setAcceptHoverEvents( true );
    } else if ( it->isTemplateItem() ) {
        setFlag( ItemIsFocusable, false );
        setFlag( ItemIsSelectable, false );
        setFlag( ItemIsMovable, false );
        setAcceptDrops( false );
        setAcceptHoverEvents( false );
    }
    setCacheMode( NoCache );
}

QPainterPath PHIGraphicsItem::shape() const
{
    QPainterPath path;
    path.addRect( _it->rect() );
    return path;
}

/*
bool PHIGraphicsItem::sceneEvent( QEvent *e )
{
    if ( _it->sceneEvent( e ) ) return true;
    return QGraphicsProxyWidget::sceneEvent( e );
}
*/

QRectF PHIGraphicsItem::boundingRect() const
{
    return _it->boundingRect();
}

QSizeF PHIGraphicsItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    QSizeF size=_it->sizeHint( which, constraint );
    if ( size.isValid() ) return size;
    return QGraphicsProxyWidget::sizeHint( which, constraint );
}

void PHIGraphicsItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    if ( !isVisible() ) return;
    painter->save();
    _it->paint( painter, option, widget );
    painter->restore();
}
