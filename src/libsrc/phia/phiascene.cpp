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
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QAction>
#include "phiascene.h"
#include "phiawebview.h"
#include "phiasettingsdlg.h"

PHIAScene::PHIAScene( QObject *parent )
    : QGraphicsScene( parent )
{
    qDebug( "PHIAScene::PHIAScene()" );
    //Q_ASSERT( qobject_cast<PHIAWebView*>(parent) );
}

PHIAScene::~PHIAScene()
{
    qDebug( "PHIAScene::~PHIAScene()" );
}

void PHIAScene::contextMenuEvent( QGraphicsSceneContextMenuEvent *event )
{
    QGraphicsScene::contextMenuEvent( event );
    if ( event->isAccepted() ) return;
    QMenu m;
    QAction *settings=m.addAction(  QIcon( QPixmap( ":/gnome/config" ) ),
        QObject::tr( "User profile" ) );
    QAction *javascript=m.addAction( QIcon( QPixmap( ":/gnome/script" ) ),
        QObject::tr( "JavaScript" ) );
    QAction *act=m.exec( event->screenPos() );
    if ( !act ) return;
    if ( act==settings ) {
        PHIASettingsDlg dlg( _view );
        dlg.exec();
    } else if ( act==javascript ) {
        _view->showJavaScriptClicked( _view->source() );
    }
}

void PHIAScene::mouseMoveEvent( QGraphicsSceneMouseEvent *e )
{
    _mousePos=e->scenePos();
    _screenPos=e->screenPos();
    emit xPositionChanged( _mousePos.x() );
    emit yPositionChanged( _mousePos.y() );
    QGraphicsScene::mouseMoveEvent( e );
}

void PHIAScene::mousePressEvent( QGraphicsSceneMouseEvent *e )
{
    _pressedButtons=0;
    Qt::MouseButtons b=e->buttons();
    if ( b & Qt::LeftButton ) _pressedButtons |= 1;
    if ( b & Qt::RightButton ) _pressedButtons |= 2;
    if ( b & Qt::MidButton ) _pressedButtons |= 4;
    QGraphicsScene::mousePressEvent( e );
}

void PHIAScene::mouseReleaseEvent( QGraphicsSceneMouseEvent *e )
{
    _pressedButtons=0;
    Qt::MouseButtons b=e->buttons();
    if ( b & Qt::LeftButton ) _pressedButtons |= 1;
    if ( b & Qt::RightButton ) _pressedButtons |= 2;
    if ( b & Qt::MidButton ) _pressedButtons |= 4;
    QGraphicsScene::mouseReleaseEvent( e );
}

void PHIAScene::keyPressEvent( QKeyEvent *e )
{
    if ( e->key()==Qt::Key_Return ) emit returnButtonPressed();
    QGraphicsScene::keyPressEvent( e );
}

void PHIAScene::dragEnterEvent( QGraphicsSceneDragDropEvent *e )
{
    e->accept();
    QGraphicsScene::dragEnterEvent( e );
}

void PHIAScene::dragMoveEvent( QGraphicsSceneDragDropEvent *e )
{
    if ( !e->mimeData()->hasFormat( "application/x-phi-dd" ) ) return e->ignore();
    QByteArray input=e->mimeData()->data( "application/x-phi-dd" );
    QBuffer inputBuffer( &input );
    inputBuffer.open( QIODevice::ReadOnly );
    QDataStream ds( &inputBuffer );
    ds.setVersion( PHI_DSV );
    quint8 version;
    QString id;
    qint32 opts;
    ds >> version >> id >> opts;
    PHIBaseItem *item=_view->page()->getElementById( id );
    if ( !item ) return e->ignore(); // should never executed
    item->setData( PHIItem::DDragStartPos, e->scenePos() );
    QGraphicsScene::dragMoveEvent( e );
    if ( !item->dragRevertOnIgnore() && !item->dragRevertOnAccept()
        && item->dragMoveAction() ) return e->accept();
}

void PHIAScene::dropEvent( QGraphicsSceneDragDropEvent *event )
{
    QGraphicsScene::dropEvent( event );
}

QNetworkAccessManager* PHIAScene::networkAccessManager() const
{
    Q_ASSERT( qobject_cast<PHIAWebView*>(parent()) );
    return qobject_cast<PHIAWebView*>(parent())->networkAccessManager();
}

PHIAGraphicsView::PHIAGraphicsView( QWidget *parent )
    : QGraphicsView( parent )
{
    qDebug( "PHIAGraphicsView::PHIAGraphicsView()" );
    setAcceptDrops( true );
    setInteractive( true );
    setDragMode( NoDrag );
    setRenderHint( QPainter::Antialiasing, true );
    setRenderHint( QPainter::SmoothPixmapTransform, true );
    setRenderHint( QPainter::TextAntialiasing, true );
    setFrameShape( QFrame::NoFrame );
    setFrameShadow( QFrame::Plain );
    setViewportUpdateMode( QGraphicsView::SmartViewportUpdate );
    setCacheMode( CacheNone );
    setAlignment( Qt::AlignHCenter | Qt::AlignTop );
}

PHIAGraphicsView::~PHIAGraphicsView()
{
    qDebug( "PHIAGraphicsView::~PHIAGraphicsView()" );
}
