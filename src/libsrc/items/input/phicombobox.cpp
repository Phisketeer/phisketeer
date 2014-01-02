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
#include <QKeyEvent>
#include <QMenu>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QAction>
#include "phicombobox.h"
#include "phibaseitem.h"

PHIComboBox::PHIComboBox( PHIBaseItem *it, QWidget *parent )
    : QComboBox( parent ), _it( it ), _menu( 0 )
{
    qDebug( "PHIComboBox::PHIComboBox()" );
}

PHIComboBox::~PHIComboBox()
{
    delete _menu;
    qDebug( "PHIComboBox::~PHIComboBox()" );
}

void PHIComboBox::showPopup()
{
    if ( count()==0 ) return;
    const QGraphicsWidget* gw=_it->graphicsWidget();
    QGraphicsView *view=gw->scene()->views().first();
    Q_ASSERT( view );

    _menu=new QMenu( view );
    QAction *defAct( 0 );
    for ( int i=0; i<count(); i++ ) {
        QAction *act=_menu->addAction( itemText( i ) );
        act->setData( i );
        act->setCheckable( true );
        if ( currentIndex()==i ) {
            act->setChecked( true );
            defAct=act;
        }
    }
    QPoint pos;
    if ( _it->isChild() ) pos=view->mapFromScene( _it->graphicsWidget()->mapToScene( QPointF() ) );
    else pos=view->mapFromScene( _it->realX(), _it->realY() );
    pos=view->mapToGlobal( pos );
    defAct=_menu->exec( pos, defAct );
    if ( defAct ) setCurrentIndex( defAct->data().toInt() );
    repaint();
}

bool PHIComboBox::event( QEvent *e )
{
    if ( e->type()==QEvent::MouseButtonPress || e->type()==QEvent::MouseButtonDblClick ) {
        showPopup();
        e->ignore();
        return true;
    }
    return QComboBox::event( e );
}

void PHIComboBox::keyPressEvent( QKeyEvent *e )
{
    if ( e->key()==Qt::Key_Space ) {
        showPopup();
        e->ignore();
        return;
    }
    return QComboBox::keyPressEvent( e );
}
