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
#include <QStringList>
#include <QKeyEvent>
#include "phiacomboboxhack.h"
#include "phiaitem.h"
#include "phiawebview.h"

PHIAComboButton::PHIAComboButton( QWidget *parent )
    : QToolButton( parent ), _popup( 0 )
{
    qDebug( "PHIAComboButton::PHIAComboButton()" );
    setPopupMode( QToolButton::InstantPopup );
    setEnabled( false );
    setVisible( false );
}

PHIAComboButton::~PHIAComboButton()
{
    qDebug( "PHIAComboButton::~PHIAComboButton()" );
}

void PHIAComboButton::setItems( const QStringList &list, int selected )
{
    if ( _popup ) delete _popup;
    _popup=new QMenu();
    QActionGroup *group=new QActionGroup( _popup );
    group->setExclusive( true );
    QList <QAction*> actions;
    for ( int i=0; i<list.count();i++ ) {
        QAction *action=new QAction( list.at( i ), 0 );
        action->setCheckable( true );
        action->setData( i );
        group->addAction( action );
        if ( i==selected ) action->setChecked( true );
        actions << action;
    }

    _popup->addActions( actions );
    _popup->setMinimumWidth( 100 );

    if ( actions.count() > selected ) setDefaultAction( actions.at( selected ) );

    setMenu( _popup );
    connect( _popup, SIGNAL( triggered( QAction* ) ), this,
        SIGNAL( selectedAction( QAction* ) ) );
}

PHIAComboBox::PHIAComboBox( const PHIAItem *item, QWidget *parent )
    : QComboBox( parent ), _item( item )
{
    qDebug( "PHIAComboBox::PHIAComboBox()" );
    QGraphicsView *v=_item->view()->scene()->views().first();
    _toolButton=new PHIAComboButton( v );
    connect( _toolButton, SIGNAL( selectedAction( QAction* ) ),
        this, SLOT( updateItem( QAction* ) ) );
}

PHIAComboBox::~PHIAComboBox()
{
    qDebug( "PHIAComboBox::~PHIAComboBox()" );
}

void PHIAComboBox::showPopup()
{
    if ( count()==0 ) return;
    QStringList list;
    for ( int i=0; i<count(); i++ ) list << itemText( i );
    _toolButton->setItems( list, currentIndex() );

    QGraphicsView *v=_item->view()->scene()->views().first();
    QPoint pos=v->mapFromScene( _item->graphicsItem()->mapToScene( QPoint() ) );
    _toolButton->move( pos.x()+4, pos.y()-5 );
    _toolButton->showMenu();
}

bool PHIAComboBox::event( QEvent *e )
{
    if ( e->type()==QEvent::MouseButtonPress || e->type()==QEvent::MouseButtonDblClick ) {
        showPopup();
        e->ignore();
        return true;
    }
    return QComboBox::event( e );
}

void PHIAComboBox::keyPressEvent( QKeyEvent *e )
{
    if ( e->key()==Qt::Key_Space ) {
        showPopup();
        e->ignore();
        return;
    }
    return QComboBox::keyPressEvent( e );
}

void PHIAComboBox::updateItem( QAction *act )
{
    setCurrentIndex( act->data().toInt() );
}
