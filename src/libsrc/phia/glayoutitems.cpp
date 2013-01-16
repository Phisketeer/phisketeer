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
#include <QGraphicsLayout>
#include "glayoutitems.h"
#include "phiaitem.h"

GAbstractLayoutItem::GAbstractLayoutItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : GWidgetItem( w, h, it, parent ), _width( w ), _height( h )
{
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    setHandlesChildEvents( false );
    setMinimumSize( 0, 0 );
    setMaximumSize( w, h );
    resize( w, h );
    //qWarning( "resized %f %f", w, h );
    // resize again after returning to the main event loop - prevents wrong size for layouts
    //connect( this, SIGNAL( updateSize( int ) ), this, SLOT( slotUpdateSize( int ) ), Qt::QueuedConnection );
}

GAbstractLayoutItem::~GAbstractLayoutItem()
{
}

void GAbstractLayoutItem::prepareSlideEffect()
{
    QGraphicsLayout *l=layout();
    Q_ASSERT( l );
    GWidgetItem *it;
    int i;
    for ( i=0; i<l->count(); i++ ) {
        it=dynamic_cast<GWidgetItem*>(l->itemAt( i ));
        if ( PHI::isLayoutContainer( it->item()->wid() ) ) {
            qDebug( "----------" );
            GAbstractLayoutItem *lit=dynamic_cast<GAbstractLayoutItem*>(it);
            Q_ASSERT( lit );
            lit->prepareSlideEffect();
            continue;
        }
        _policyHash.insert( i, it->sizePolicy() );
        _minimumSizeHash.insert( i, it->minimumSize() );
        it->setMinimumSize( 0, 0 );
        it->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
        qDebug( "presaved policy for %s", it->item()->id().data() );
    }
    _policyHash.insert( i, sizePolicy() );
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}

void GAbstractLayoutItem::restoreSlideEffect()
{
    QGraphicsLayout *l=layout();
    Q_ASSERT( l );
    GWidgetItem *it;
    int i;
    for ( i=0; i<l->count(); i++ ) {
        it=dynamic_cast<GWidgetItem*>(l->itemAt( i ));
        if ( PHI::isLayoutContainer( it->item()->wid() ) ) {
            GAbstractLayoutItem *lit=dynamic_cast<GAbstractLayoutItem*>(it);
            Q_ASSERT( lit );
            lit->restoreSlideEffect();
            continue;
        }
        it->setSizePolicy( _policyHash.value( i ) );
        it->setMinimumSize( _minimumSizeHash.value( i ) );
        //qDebug( "restored policy for %s", it->item()->id().data() );
    }
    setSizePolicy( _policyHash.value( i ) );
    l->invalidate();
}

void GAbstractLayoutItem::setWidth( qreal w )
{
    prepareGeometryChange();
    setMaximumSize( w, _height );
    resize( w, _height );
    _width=w;
}

void GAbstractLayoutItem::setHeight( qreal h )
{
    prepareGeometryChange();
    setMaximumSize( _width, h );
    resize( _width, h );
    _height=h;
}

void GAbstractLayoutItem::slotUpdateSize( int )
{
    /*
    qWarning( "slotUpdateSize %f %f %d", _width, _height, i );
    //setMaximumSize( _width, _height );
    if ( i==0 ) {
        resize( .0, .0 );
        layout()->invalidate();
        updateGeometry();
        emit updateSize( 1 );
        return;
    }
    resize( _width, _height );
    layout()->invalidate();
    updateGeometry();
    //setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    */
}

GLinearLayoutItem::GLinearLayoutItem( qreal w, qreal h, const PHIAItem *it, const PHIByteArrayList &l,
   bool horizontal, QGraphicsItem *parent ) : GAbstractLayoutItem( w, h, it, parent ), _childIds( l )
{
    qDebug( "GLinearLayoutItem::GLinearLayoutItem()" );
    _layout=new QGraphicsLinearLayout();
    if ( horizontal ) _layout->setOrientation( Qt::Horizontal );
    else _layout->setOrientation( Qt::Vertical );
    _layout->setContentsMargins( 0, 0, 0, 0 );
    dynamic_cast<QGraphicsLinearLayout*>(_layout)->setSpacing( PHI::defaultSpacing() );
    setLayout( _layout );
    //_layout->setMaximumSize( w, h );
}

GLinearLayoutItem::~GLinearLayoutItem()
{
    for ( int i=_layout->count(); i>0; i-- ) {
        QGraphicsLayoutItem *li=_layout->itemAt( i-1 );
        Q_ASSERT( li );
        QGraphicsItem *git=li->graphicsItem();
        if ( git ) git->setParentItem( 0 );
        _layout->removeAt( i-1 );
    }
    qDebug( "GLinearLayoutItem::~GLinearLayoutItem() " );
}

void GLinearLayoutItem::activate()
{
    //resize( _width, _height );
    _layout->invalidate();
    _layout->activate();
    //emit updateSize( 0 );
}

void GLinearLayoutItem::insertChild( const PHIAItem *it )
{
    QGraphicsProxyWidget *p=qgraphicsitem_cast<QGraphicsProxyWidget*>(it->graphicsItem());
    if ( !p ) return;
    QGraphicsLayout *cl=p->layout();
    if ( cl ) {
        cl->setContentsMargins( 0, 0, 0, 0 );
        cl->updateGeometry();
        cl->invalidate();
    }
    Q_ASSERT( _childIds.contains( it->id() ) );

    int pos=_childIds.indexOf( it->id(), 0 );
    //qDebug( "Found item %s at pos %d", it->id().data(), pos );
    const PHIItem *cItem; int current=0, ins=0, cpos=0;
    foreach ( cItem, _children ) {
        ++current;
        cpos=_childIds.indexOf( cItem->id(), 0 );
        if ( pos > cpos ) ins=current;
    }

    _layout->insertItem( ins, p );
    _children.insert( ins, it );
    if ( _childIds.count()==_layout->count() ) activate();
}

// Used to set the line edit items in a phi container
QList <QLineEdit*> GLinearLayoutItem::lineEditItems() const
{
    QList <QLineEdit*> list;
    for ( int i=0; i<layout()->count(); i++ ) {
        QGraphicsLayoutItem *li=layout()->itemAt( i );
        Q_ASSERT( li );
        QGraphicsProxyWidget* proxy=qgraphicsitem_cast<QGraphicsProxyWidget*>(li->graphicsItem());
        if ( !proxy ) continue;
        QLineEdit *e=qobject_cast<QLineEdit*>(proxy->widget());
        if ( e ) list.append( e );
    }
    return list;
}

// Used to set the select (combobox) items in a phi container
QList <QComboBox*> GLinearLayoutItem::comboBoxItems() const
{
    QList <QComboBox*> list;
    for ( int i=0; i<layout()->count(); i++ ) {
        QGraphicsLayoutItem *li=layout()->itemAt( i );
        Q_ASSERT( li );
        QGraphicsProxyWidget* proxy=qgraphicsitem_cast<QGraphicsProxyWidget*>(li->graphicsItem());
        if ( !proxy ) continue;
        QComboBox *c=qobject_cast<QComboBox*>(proxy->widget());
        if ( c ) list.append( c );
    }
    return list;
}

GGridLayoutItem::GGridLayoutItem( qreal w, qreal h, const PHIAItem *it, const PHIRectHash &ip,
    QGraphicsItem *parent ) : GAbstractLayoutItem( w, h, it, parent ), _itemPos( ip )
{
    qDebug( "GGridLayoutItem::GGridLayoutItem()" );
    _layout=new QGraphicsGridLayout();
    _layout->setContentsMargins( 0, 0, 0, 0 );
    dynamic_cast<QGraphicsGridLayout*>(_layout)->setSpacing( PHI::defaultSpacing() );
    setLayout( _layout );
    //_layout->setMaximumSize( w, h );
}

GGridLayoutItem::~GGridLayoutItem()
{
    for ( int i=_layout->count(); i>0; i-- ) {
        QGraphicsLayoutItem *li=_layout->itemAt( i-1 );
        Q_ASSERT( li );
        QGraphicsItem *git=li->graphicsItem();
        if ( git ) git->setParentItem( 0 );
        _layout->removeAt( i-1 );
    }
    qDebug( "GGridLayoutItem::~GGridLayoutItem()" );
}

void GGridLayoutItem::insertChild( const PHIAItem *it )
{
    QRect r=_itemPos.value( it->id() );
    QGraphicsProxyWidget *p=qgraphicsitem_cast<QGraphicsProxyWidget*>(it->graphicsItem());
    Q_ASSERT( p );
    _layout->addItem( p, r.y(), r.x(), r.height(), r.width() );
    int row=r.x()*100+r.y(); // column based sorting into rows
    QLineEdit *e=qobject_cast<QLineEdit*>(p->widget());
    if ( e ) _lineEditItems.insert( row, e );
    QComboBox *c=qobject_cast<QComboBox*>(p->widget());
    if ( c ) _comboBoxItems.insert( row, c );
    qDebug( "Adding child %s %d %d %d %d", it->id().data(), r.y(), r.x(), r.height(), r.width() );
    _children.append( it );
    if ( _layout->count()==_itemPos.count() ) activate();
}

void GGridLayoutItem::activate()
{
    qDebug( "Activating grid layout" );
    //resize( _width, _height );
    _layout->invalidate();
    _layout->activate();
    //emit updateSize( 0 );
}

// Used to set the line edit items in a phi container
QList <QLineEdit*> GGridLayoutItem::lineEditItems() const
{
    return _lineEditItems.values();
}

// Used to set the select (combobox) items in a phi container
QList <QComboBox*> GGridLayoutItem::comboBoxItems() const
{
    return _comboBoxItems.values();
}

GHSpacerItem::GHSpacerItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : GWidgetItem( w, h, it, parent )
{
    qDebug( "GHSpacerItem::GHSpacerItem() h=%f", w );
    QWidget *l=new QLabel();
    l->setMinimumWidth( static_cast<int>(w) );
    setWidget( l );
    setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Maximum, QSizePolicy::Label );
    setFlag( ItemHasNoContents, true );
    setMinimumSize( w, PHI::minimumHeight() );
    //setPreferredSize( w, PHI::minimumHeight() );
}

GHSpacerItem::~GHSpacerItem()
{
    qDebug( "GHSpacerItem::~GHSpacerItem()" );
}

GVSpacerItem::GVSpacerItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : GWidgetItem( w, h, it, parent )
{
    qDebug( "GVSpacerItem::GVSpacerItem()" );
    QWidget *l=new QLabel();
    l->setMinimumHeight( static_cast<int>(h) );
    setWidget( l );
    setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Minimum, QSizePolicy::Label );
    setFlag( ItemHasNoContents, true );
    setMinimumSize( PHI::minimumHeight(), h );
    //setPreferredSize( PHI::minimumWidth(), h );
}

GVSpacerItem::~GVSpacerItem()
{
    qDebug( "GVSpacerItem::~GVSpacerItem()" );
}
