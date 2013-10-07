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

void PHIHorizontalLayoutItem::addBaseItems( const QList<PHIBaseItem*> &list )
{
    if ( !list.count()>1 ) return;
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
        out << id << static_cast<quint8>(r.y()) << static_cast<quint8>(r.x());
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
        insertBaseItem( it, r.y(), r.x(), r.height(), r.width() );
    }
    resize( tmpWidth, tmpHeight );
}

void PHIFormLayoutItem::addBaseItems( const QList<PHIBaseItem *> &list )
{
    _childRects.clear();
    QList <PHIBaseItem*> itemListX=list, itemListY=list, itemList1, itemList2;
    qSort( itemListX.begin(), itemListX.end(), xLessThan );
    qSort( itemListY.begin(), itemListY.end(), yLessThan );
    QPointF ref( itemListX.at( 0 )->x(), itemListY.at( 0 )->y() );
    PHIBaseItem *it;
    QMap <qreal, PHIBaseItem*> col1, col2;
    Q_ASSERT( itemListY.count()>1 );
    if ( itemListY.count()==2 ) {
        if ( itemListY.at( 0 )->x() < itemListY.at( 1 )->x() ) {
            insertBaseItem( itemListY.at( 0 ), 0, 0 );
            insertBaseItem( itemListY.at( 1 ), 0, 1 );
        } else {
            insertBaseItem( itemListY.at( 1 ), 0, 0 );
            insertBaseItem( itemListY.at( 0 ), 0, 1 );
        }
        setPos( ref );
        resize( layout()->preferredSize() );
        return;
    }
    qreal minWidth=qMin( 32., itemListX.at( 0 )->width() );
    foreach ( it, list ) {
        if ( it->x()>ref.x()+minWidth-10 ) col2.insertMulti( it->y(), it );
        else col1.insertMulti( it->y(), it );
    }
    itemList1=col1.values();
    itemList2=col2.values();
    int srcRow1=0, srcRow2=0, destRow1=0, destRow2=0;
    while ( srcRow1<itemList1.count() || srcRow2<itemList2.count() ) {
        bool skip1=false, skip2=false;
        qreal height1=0, height2=0, y1=0, y2=0;
        if ( srcRow1<itemList1.count() ) {
            height1=itemList1.at( srcRow1 )->height();
            y1=itemList1.at( srcRow1 )->y();
        }
        if ( srcRow2<itemList2.count() ) {
            height2=itemList2.at( srcRow2 )->height();
            y2=itemList2.at( srcRow2 )->y();
        }
        if ( y1 > y2+height2 && height2>0 ) { destRow1++; skip1=true; }
        else if ( y2 > y1+height1 && height1>0 ) { destRow2++; skip2=true; }
        if ( !skip1 && height1>0 ) {
            _childRects.insert( itemList1.at( srcRow1 )->id(), QRect( 0, destRow1, 1, 1 ) );
            insertBaseItem( itemList1.at( srcRow1++ ), destRow1++, 0 );
        }
        if ( !skip2 && height2>0 ) {
            _childRects.insert( itemList2.at( srcRow2 )->id(), QRect( 1, destRow2, 1, 1 ) );
            insertBaseItem( itemList2.at( srcRow2++ ), destRow2++, 1 );
        }
    }
    setPos( ref );
    resize( layout()->preferredSize() );
}

void PHIGridLayoutItem::loadItemData( QDataStream &in, int version )
{
    PHIAbstractLayoutItem::loadItemData( in, version );
    PHIRectHash childRects;
    QByteArray id;
    quint8 row, col, rowspan, colspan;
    qint16 count;
    in >> count;
    for ( qint16 i=0; i<count; i++ ) {
        in >> id >> row >> col >> rowspan >> colspan;
        QRect r( static_cast<int>(col), static_cast<int>(row), static_cast<int>(colspan), static_cast<int>(rowspan) );
        childRects.insert( id, r );
    }
    setChildRects( childRects );
}

void PHIGridLayoutItem::saveItemData( QDataStream &out, int version )
{
    const PHIRectHash rects=childRects();
    PHIAbstractLayoutItem::saveItemData( out, version );
    out << static_cast<qint16>(rects.count());
    foreach ( QByteArray id, rects.keys() ) {
        QRect r=rects.value( id );
        out << id << static_cast<quint8>(r.y()) << static_cast<quint8>(r.x())
            << static_cast<quint8>(r.height()) << static_cast<quint8>(r.width());
        qDebug() << id << r.y() << r.x() << r.height() << r.width();
    }
}

void PHIGridLayoutItem::addBaseItems( const QList<PHIBaseItem *> &list )
{
    PHIBaseItem *it;
    QMap <qreal, PHIBaseItem*> map;
    foreach( it, list ) {
        map.insertMulti( it->x(), it );
        map.insertMulti( it->x()+it->width(), it );
    }
    QMap <int, PHIBaseItem*> xmap;
    int col=0;
    qreal x=map.keys().first(), last=-100000.;
    foreach( qreal curx, map.keys() ) {
        if ( last==curx ) continue;
        if ( x+20<curx ) {
            x=curx;
            col++;
        }
        QList <PHIBaseItem*> items=map.values( curx );
        foreach ( it, items ) xmap.insertMulti( col, it );
        last=curx;
    }
    x=map.keys().first();
    map.clear();
    foreach( it, list ) {
        map.insertMulti( it->y(), it );
        map.insertMulti( it->y()+it->height(), it );
    }
    QMap <int, PHIBaseItem*> ymap;
    qreal y=map.keys().first();
    last=-100000.;
    int row=0;
    foreach( qreal cury, map.keys() ) {
        if ( last==cury ) continue;
        if ( y+20<cury ) {
            y=cury;
            row++;
        }
        QList <PHIBaseItem*> items=map.values( cury );
        foreach ( it, items ) ymap.insertMulti( row, it );
        last=cury;
    }
    y=map.keys().first();
    QHash <PHIBaseItem*, QPoint> matrix;
    QHash <PHIBaseItem*, QPoint> size;
    for ( row=0; row<=ymap.keys().last(); row++ ) {
        QList <PHIBaseItem*> rItems=ymap.values( row );
        for ( col=0; col<=xmap.keys().last(); col++ ) {
            QList <PHIBaseItem*> cItems=xmap.values( col );
            foreach ( PHIBaseItem *yItem, rItems ) {
                foreach ( PHIBaseItem *xItem, cItems ) {
                    if ( yItem==xItem ) {
                        // this equals four times for each item (x,y and width,height)
                        if ( !matrix.contains( yItem ) )
                            matrix.insert( yItem, QPoint( col, row ) );
                        else {
                            if ( matrix.value( yItem )!=QPoint( col, row ) )
                                size.insert( yItem, QPoint( col, row ) );
                        }
                    }
                }
            }
        }
    }
    PHIRectHash rects;
    for ( row=0; row<=ymap.keys().last(); row++ ) {
        for ( col=0; col<=xmap.keys().last(); col++ ) {
            foreach( it, matrix.keys() ) {
                QPoint m=matrix.value( it );
                if ( m==QPoint( col, row ) ) {
                    m=size.value( it );
                    qDebug() << row << col << it->id();
                    qDebug() << m.y() << m.x() << it->id();
                    int spanx=m.x()-col;
                    if ( spanx<1 ) spanx=1;
                    int spany=m.y()-row;
                    if ( spany<1 ) spany=1;
                    insertBaseItem( it, row, col, spany, spanx );
                    rects.insert( it->id(), QRect( col, row, spanx, spany ) );
                }
            }
        }
    }
    setChildRects( rects );
    setPos( x, y );
    resize( layout()->preferredSize() );
}
