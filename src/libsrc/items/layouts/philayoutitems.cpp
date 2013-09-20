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
#include <QGraphicsLayout>
#include <QGraphicsLinearLayout>
#include <QGraphicsWidget>
#include <QPainter>
#include "philayoutitems.h"
#include "phibasepage.h"

static bool xLessThan( const PHIBaseItem *it1, const PHIBaseItem *it2 )
{
    return it1->x() < it2->x();
}

static bool yLessThan( const PHIBaseItem *it1, const PHIBaseItem *it2 )
{
    return it1->y() < it2->y();
}

PHIHorizontalLayoutItem::PHIHorizontalLayoutItem()
    : PHIAbstractLayoutItem()
{
}

void PHIHorizontalLayoutItem::addBaseItems( const QList<PHIBaseItem*> &list )
{
    if ( !list.count() ) return;
    _childIds.clear();
    QList <PHIBaseItem*>slist=list;
    qSort( slist.begin(), slist.end(), xLessThan );
    QPointF pos=slist.first()->pos();
    for( int i=0; i<slist.count(); i++ ) {
        insertBaseItem( slist.at( i ), 0, i );
        _childIds.append( slist.at( i )->id() );
    }
    resize( layout()->preferredSize() );
    setPos( pos );
}

void PHIHorizontalLayoutItem::updateChildId( const QString &oldId, const QString &newId )
{
    int pos=_childIds.indexOf( oldId.toLatin1() );
    Q_ASSERT( pos!=-1 );
    _childIds.replace( pos, newId.toLatin1() );
}

void PHIHorizontalLayoutItem::loadItemData( QDataStream &in, int version )
{
    PHIAbstractLayoutItem::loadItemData( in, version );
    in >> _childIds;
}

void PHIHorizontalLayoutItem::saveItemData( QDataStream &out, int version )
{
    PHIAbstractLayoutItem::saveItemData( out, version );
    out << _childIds;
}

void PHIHorizontalLayoutItem::activateLayout()
{
    qreal tmpWidth=width(); // preserve width
    qreal tmpHeight=height(); // preserve height
    for ( int i=0; i<_childIds.count(); i++ ) {
        PHIBaseItem *it=page()->findItem( _childIds.at( i ) );
        Q_ASSERT( it );
        qobject_cast<PHIVerticalLayoutItem*>(this) ? insertBaseItem( it, i ) : insertBaseItem( it, 0, i );
    }
    resize( tmpWidth, tmpHeight );
}

PHIVerticalLayoutItem::PHIVerticalLayoutItem()
    : PHIHorizontalLayoutItem()
{
}

void PHIVerticalLayoutItem::addBaseItems( const QList<PHIBaseItem*> &list )
{
    if ( !list.count() ) return;
    PHIByteArrayList childIds;
    QList <PHIBaseItem*>slist=list;
    qSort( slist.begin(), slist.end(), yLessThan );
    QPointF pos=slist.first()->pos();
    for( int i=0; i<slist.count(); i++ ) {
        insertBaseItem( slist.at( i ), i );
        childIds.append( slist.at( i )->id() );
    }
    setChildIds( childIds );
    resize( layout()->preferredSize() );
    setPos( pos );
}

PHIFormLayoutItem::PHIFormLayoutItem()
    : PHIAbstractLayoutItem()
{
}

void PHIFormLayoutItem::updateChildId( const QString &oldId, const QString &newId )
{
    QRect r=_childRects.take( oldId.toLatin1() );
    Q_ASSERT( r.isValid() );
    _childRects.insert( newId.toLatin1(), r );
}

void PHIFormLayoutItem::loadItemData( QDataStream &in, int version )
{
    PHIAbstractLayoutItem::loadItemData( in, version );
    _childRects.clear();
    QByteArray id;
    quint8 row, col;
    qint16 count;
    in >> count;
    for ( qint16 i=0; i<count; i++ ) {
        in >> id >> row >> col;
        QRect r( static_cast<int>(col), static_cast<int>(row), 1, 1 );
        _childRects.insert( id, r );
    }
}

void PHIFormLayoutItem::saveItemData( QDataStream &out, int version )
{
    PHIAbstractLayoutItem::saveItemData( out, version );
    out << static_cast<qint16>(_childRects.count());
    foreach ( QByteArray id, _childRects.keys() ) {
        QRect r=_childRects.value( id );
        out << id << static_cast<quint8>( r.y() ) << static_cast<quint8>( r.x() );
        qDebug() << id << r.y() << r.x();
    }
}

void PHIFormLayoutItem::activateLayout()
{
    qreal tmpWidth=width(); // preserve width
    qreal tmpHeight=height(); // preserve height
    foreach ( QByteArray id, _childRects.keys() ) {
        PHIBaseItem *it=page()->findItem( id );
        Q_ASSERT( it );
        QRect r=_childRects.value( id );
        insertBaseItem( it, r.y(), r.x() );
    }
    resize( tmpWidth, tmpHeight );
}

void PHIFormLayoutItem::addBaseItems( const QList<PHIBaseItem *> &list )
{

}
