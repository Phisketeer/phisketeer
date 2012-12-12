/*
#    Copyright (C) 2010-2012  Marius B. Schumacher
#    Copyright (C) 2011-2012  Phisys AG, Switzerland
#    Copyright (C) 2012  Phisketeer.org team
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
#include <QPainter>
#include <QTextDocument>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QBuffer>
#include "gitems.h"
#include "phia.h"
#include "phiaitem.h"

GItemHandler::GItemHandler( const PHIAItem *it )
    : QObject(), _item( it )
{
    connect( this, SIGNAL( onClick( QGraphicsSceneMouseEvent* ) ),
        it, SLOT( slotOnClick( QGraphicsSceneMouseEvent* ) ) );
    connect( this, SIGNAL( onDblClick( QGraphicsSceneMouseEvent* ) ),
        it, SLOT( slotOnDblClick( QGraphicsSceneMouseEvent* ) ) );
    connect( this, SIGNAL( onFocus( QFocusEvent* ) ), it, SLOT( slotOnFocus( QFocusEvent* ) ) );
    connect( this, SIGNAL( onBlur( QFocusEvent* ) ), it, SLOT( slotOnBlur( QFocusEvent* ) ) );
    connect( this, SIGNAL( onMouseUp( QGraphicsSceneMouseEvent* ) ),
        it, SLOT( slotOnMouseUp( QGraphicsSceneMouseEvent* ) ) );
    connect( this, SIGNAL( onMouseDown( QGraphicsSceneMouseEvent* ) ),
        it, SLOT( slotOnMouseDown( QGraphicsSceneMouseEvent* ) ) );
    connect( this, SIGNAL( onMouseOver( QGraphicsSceneHoverEvent* ) ),
        it, SLOT( slotOnMouseOver( QGraphicsSceneHoverEvent* ) ) );
    connect( this, SIGNAL( onMouseOut( QGraphicsSceneHoverEvent* ) ),
        it, SLOT( slotOnMouseOut( QGraphicsSceneHoverEvent* ) ) );
    connect( this, SIGNAL( onKeyPress( QKeyEvent* ) ), it, SLOT( slotOnKeyPress( QKeyEvent* ) ) );
    connect( this, SIGNAL( onKeyDown( QKeyEvent* ) ), it, SLOT( slotOnKeyDown( QKeyEvent* ) ) );
    connect( this, SIGNAL( onKeyUp( QKeyEvent* ) ), it, SLOT( slotOnKeyUp( QKeyEvent* ) ) );
    connect( this, SIGNAL( onMouseMove( QGraphicsSceneHoverEvent* ) ),
        it, SLOT( slotOnMouseMove( QGraphicsSceneHoverEvent* ) ) );
    connect( this, SIGNAL( onChange() ), it, SLOT( slotOnChange() ) );
}

GItemHandler::~GItemHandler()
{
}

GGraphicsTextItem::GGraphicsTextItem( const QString &v, const QFont &f, const PHIAItem *it,
    QGraphicsItem *parent ) : GItemHandler( it ), QGraphicsSimpleTextItem( parent )
{
    qDebug( "GGraphicsTextItem::GGraphicsTextItem()" );
    setText( v );
    setFont( f );
    QPen pen;
    pen.setColor( Qt::black );
    pen.setStyle( static_cast<Qt::PenStyle>(it->line()) );
    QBrush brush;
    brush.setColor( Qt::white );
    setPen( pen );
    setBrush( brush );
    setAcceptHoverEvents( true );
}

GGraphicsTextItem::~GGraphicsTextItem()
{
    qDebug( "GGraphicsTextItem::~GGraphicsTextItem()" );
}

void GGraphicsTextItem::paint( QPainter *p, const QStyleOptionGraphicsItem *o, QWidget *w )
{
    p->setRenderHint( QPainter::TextAntialiasing, true );
    p->setRenderHint( QPainter::Antialiasing, true );
    QGraphicsSimpleTextItem::paint( p, o, w );
}

void GGraphicsTextItem::mouseReleaseEvent( QGraphicsSceneMouseEvent *e )
{
    QGraphicsSimpleTextItem::mouseReleaseEvent( e );
    emit onMouseUp( e );
    if ( boundingRect().contains( e->pos() ) ) emit onClick( e );
}

void GGraphicsTextItem::mouseDoubleClickEvent( QGraphicsSceneMouseEvent *e )
{
    QGraphicsSimpleTextItem::mouseDoubleClickEvent( e );
    emit onDblClick( e );
}

void GGraphicsTextItem::mousePressEvent( QGraphicsSceneMouseEvent *e )
{
    _item->checkForDragInMousePressEvent( e );
    emit onMouseDown( e );
    e->accept();
}

void GGraphicsTextItem::mouseMoveEvent( QGraphicsSceneMouseEvent *e )
{
    _item->checkForDragInMouseMoveEvent( e );
    //emit onMouseMove( e );
}

void GGraphicsTextItem::focusInEvent( QFocusEvent *e )
{
    QGraphicsSimpleTextItem::focusInEvent( e );
    emit onFocus( e );
}

void GGraphicsTextItem::focusOutEvent( QFocusEvent *e )
{
    QGraphicsSimpleTextItem::focusOutEvent( e );
    emit onBlur( e );
}

void GGraphicsTextItem::hoverEnterEvent( QGraphicsSceneHoverEvent *e )
{
    QGraphicsSimpleTextItem::hoverEnterEvent( e );
    emit onMouseOver( e );
}

void GGraphicsTextItem::hoverLeaveEvent( QGraphicsSceneHoverEvent *e )
{
    QGraphicsSimpleTextItem::hoverLeaveEvent( e );
    emit onMouseOut( e );
}

void GGraphicsTextItem::hoverMoveEvent( QGraphicsSceneHoverEvent *e )
{
    QGraphicsSimpleTextItem::hoverMoveEvent( e );
    emit onMouseMove( e );
}

void GGraphicsTextItem::keyPressEvent( QKeyEvent *e )
{
    QGraphicsSimpleTextItem::keyPressEvent( e );
    emit onKeyDown( e );
    emit onKeyPress( e );
}

void GGraphicsTextItem::keyReleaseEvent( QKeyEvent *e )
{
    QGraphicsSimpleTextItem::keyReleaseEvent( e );
    emit onKeyUp( e );
}

void GGraphicsTextItem::dragEnterEvent( QGraphicsSceneDragDropEvent *e )
{
    _item->checkDragEnterEvent( e );
}

void GGraphicsTextItem::dragMoveEvent( QGraphicsSceneDragDropEvent *e )
{
    _item->checkDragMoveEvent( e );
}

void GGraphicsTextItem::dropEvent( QGraphicsSceneDragDropEvent *e )
{
    _item->checkDropEvent( e );
}

GTextItem::GTextItem( const QString &v, const QFont &f, qreal w, const PHIAItem *it, QGraphicsItem *parent )
    : QGraphicsTextItem( parent ), _item( it )
{
    qDebug( "GTextItem::GTextItem()" );
    setFont( f );
    setHtml( v );
    setTextWidth( w );
    setTextInteractionFlags( PHITEXTINTERACTIONFLAGS );
    connect( this, SIGNAL( linkActivated( const QString& ) ),
        it, SLOT( slotLinkActivated( const QString& ) ) );
    connect( this, SIGNAL( linkHovered( const QString& ) ),
        it, SLOT( slotLinkHovered( const QString& ) ) );

    connect( this, SIGNAL( onClick( QGraphicsSceneMouseEvent* ) ),
        it, SLOT( slotOnClick( QGraphicsSceneMouseEvent* ) ) );
    connect( this, SIGNAL( onDblClick( QGraphicsSceneMouseEvent* ) ),
        it, SLOT( slotOnDblClick( QGraphicsSceneMouseEvent* ) ) );
    connect( this, SIGNAL( onFocus( QFocusEvent* ) ), it, SLOT( slotOnFocus( QFocusEvent* ) ) );
    connect( this, SIGNAL( onBlur( QFocusEvent* ) ), it, SLOT( slotOnBlur( QFocusEvent* ) ) );
    connect( this, SIGNAL( onMouseUp( QGraphicsSceneMouseEvent* ) ),
        it, SLOT( slotOnMouseUp( QGraphicsSceneMouseEvent* ) ) );
    connect( this, SIGNAL( onMouseDown( QGraphicsSceneMouseEvent* ) ),
        it, SLOT( slotOnMouseDown( QGraphicsSceneMouseEvent* ) ) );
    connect( this, SIGNAL( onMouseOver( QGraphicsSceneHoverEvent* ) ),
        it, SLOT( slotOnMouseOver( QGraphicsSceneHoverEvent* ) ) );
    connect( this, SIGNAL( onMouseOut( QGraphicsSceneHoverEvent* ) ),
        it, SLOT( slotOnMouseOut( QGraphicsSceneHoverEvent* ) ) );
    connect( this, SIGNAL( onKeyPress( QKeyEvent* ) ), it, SLOT( slotOnKeyPress( QKeyEvent* ) ) );
    connect( this, SIGNAL( onKeyDown( QKeyEvent* ) ), it, SLOT( slotOnKeyDown( QKeyEvent* ) ) );
    connect( this, SIGNAL( onKeyUp( QKeyEvent* ) ), it, SLOT( slotOnKeyUp( QKeyEvent* ) ) );
    connect( this, SIGNAL( onMouseMove( QGraphicsSceneHoverEvent* ) ),
        it, SLOT( slotOnMouseMove( QGraphicsSceneHoverEvent* ) ) );
    connect( this, SIGNAL( onChange() ), it, SLOT( slotOnChange() ) );

    setAcceptHoverEvents( true );
}

GTextItem::~GTextItem()
{
    qDebug( "GTextItem::~GTextItem()" );
}

void GTextItem::paint( QPainter *p, const QStyleOptionGraphicsItem *o, QWidget *w )
{
    p->setRenderHint( QPainter::Antialiasing, true );
    p->setRenderHint( QPainter::TextAntialiasing, true );
    QGraphicsTextItem::paint( p, o, w );
}

void GTextItem::mouseReleaseEvent( QGraphicsSceneMouseEvent *e )
{
    QGraphicsTextItem::mouseReleaseEvent( e );
    emit onMouseUp( e );
    if ( boundingRect().contains( e->pos() ) ) emit onClick( e );
}

void GTextItem::mouseDoubleClickEvent( QGraphicsSceneMouseEvent *e )
{
    QGraphicsTextItem::mouseDoubleClickEvent( e );
    emit onDblClick( e );
}

void GTextItem::mousePressEvent( QGraphicsSceneMouseEvent *e )
{
    _item->checkForDragInMousePressEvent( e );
    emit onMouseDown( e );
    e->accept();
}

void GTextItem::mouseMoveEvent( QGraphicsSceneMouseEvent *e )
{
    _item->checkForDragInMouseMoveEvent( e );
    //emit onMouseMove( e );
}

void GTextItem::focusInEvent( QFocusEvent *e )
{
    QGraphicsTextItem::focusInEvent( e );
    emit onFocus( e );
}

void GTextItem::focusOutEvent( QFocusEvent *e )
{
    QGraphicsTextItem::focusOutEvent( e );
    emit onBlur( e );
}

void GTextItem::hoverEnterEvent( QGraphicsSceneHoverEvent *e )
{
    QGraphicsTextItem::hoverEnterEvent( e );
    emit onMouseOver( e );
}

void GTextItem::hoverLeaveEvent( QGraphicsSceneHoverEvent *e )
{
    QGraphicsTextItem::hoverLeaveEvent( e );
    emit onMouseOut( e );
}

void GTextItem::hoverMoveEvent( QGraphicsSceneHoverEvent *e )
{
    QGraphicsTextItem::hoverMoveEvent( e );
    emit onMouseMove( e );
}

void GTextItem::keyPressEvent( QKeyEvent *e )
{
    QGraphicsTextItem::keyPressEvent( e );
    emit onKeyDown( e );
    emit onKeyPress( e );
}

void GTextItem::keyReleaseEvent( QKeyEvent *e )
{
    QGraphicsTextItem::keyReleaseEvent( e );
    emit onKeyUp( e );
}

void GTextItem::dragEnterEvent( QGraphicsSceneDragDropEvent *e )
{
    _item->checkDragEnterEvent( e );
}

void GTextItem::dragMoveEvent( QGraphicsSceneDragDropEvent *e )
{
    _item->checkDragMoveEvent( e );
}

void GTextItem::dropEvent( QGraphicsSceneDragDropEvent *e )
{
    _item->checkDropEvent( e );
}

void GTextItem::setValue( const QString &v )
{
    setHtml( v );
    _value=v;
}

void GTextItem::setStyleSheet( const QString &s )
{
    document()->setDefaultStyleSheet( s );
    setHtml( _value );
}

QString GTextItem::styleSheet() const
{
    return document()->defaultStyleSheet();
}

GEllipseItem::GEllipseItem( qreal width, qreal height, qint16 sta, qint16 spa, const PHIAItem *it, QGraphicsItem *parent )
    : GItemHandler( it ), QGraphicsEllipseItem( parent )
{
    qDebug( "GEllipseItem::GEllipseItem()" );
    QPen pen;
    pen.setColor( Qt::black );
    pen.setStyle( static_cast<Qt::PenStyle>(it->line()) );
    QBrush brush;
    brush.setColor( Qt::white );
    //brush.setStyle( static_cast<Qt::BrushStyle>(it->pattern()) );
    setPen( pen );
    setBrush( brush );

    setRect( 0, 0, width, height );
    setStartAngle( static_cast<int>(sta) );
    setSpanAngle( static_cast<int>(spa) );
    setAcceptHoverEvents( true );
}

GEllipseItem::~GEllipseItem()
{
    qDebug( "GEllipseItem::~GEllipseItem()" );
}

void GEllipseItem::paint( QPainter *p, const QStyleOptionGraphicsItem *o, QWidget *w )
{
    p->setRenderHint( QPainter::Antialiasing, true );
    QGraphicsEllipseItem::paint( p, o, w );
}

void GEllipseItem::mouseReleaseEvent( QGraphicsSceneMouseEvent *e )
{
    QGraphicsEllipseItem::mouseReleaseEvent( e );
    emit onMouseUp( e );
    if ( boundingRect().contains( e->pos() ) ) emit onClick( e );
}

void GEllipseItem::mouseDoubleClickEvent( QGraphicsSceneMouseEvent *e )
{
    QGraphicsEllipseItem::mouseDoubleClickEvent( e );
    emit onDblClick( e );
}

void GEllipseItem::mousePressEvent( QGraphicsSceneMouseEvent *e )
{
    _item->checkForDragInMousePressEvent( e );
    emit onMouseDown( e );
    e->accept();
}

void GEllipseItem::mouseMoveEvent( QGraphicsSceneMouseEvent *e )
{
    _item->checkForDragInMouseMoveEvent( e );
    //emit onMouseMove( e );
}

void GEllipseItem::focusInEvent( QFocusEvent *e )
{
    QGraphicsEllipseItem::focusInEvent( e );
    emit onFocus( e );
}

void GEllipseItem::focusOutEvent( QFocusEvent *e )
{
    QGraphicsEllipseItem::focusOutEvent( e );
    emit onBlur( e );
}

void GEllipseItem::hoverEnterEvent( QGraphicsSceneHoverEvent *e )
{
    QGraphicsEllipseItem::hoverEnterEvent( e );
    emit onMouseOver( e );
}

void GEllipseItem::hoverLeaveEvent( QGraphicsSceneHoverEvent *e )
{
    QGraphicsEllipseItem::hoverLeaveEvent( e );
    emit onMouseOut( e );
}

void GEllipseItem::hoverMoveEvent( QGraphicsSceneHoverEvent *e )
{
    QGraphicsEllipseItem::hoverMoveEvent( e );
    emit onMouseMove( e );
}

void GEllipseItem::keyPressEvent( QKeyEvent *e )
{
    QGraphicsEllipseItem::keyPressEvent( e );
    emit onKeyDown( e );
    emit onKeyPress( e );
}

void GEllipseItem::keyReleaseEvent( QKeyEvent *e )
{
    QGraphicsEllipseItem::keyReleaseEvent( e );
    emit onKeyUp( e );
}

void GEllipseItem::dragEnterEvent( QGraphicsSceneDragDropEvent *e )
{
    _item->checkDragEnterEvent( e );
}
void GEllipseItem::dragMoveEvent( QGraphicsSceneDragDropEvent *e )
{
    _item->checkDragMoveEvent( e );
}

void GEllipseItem::dropEvent( QGraphicsSceneDragDropEvent *e )
{
    _item->checkDropEvent( e );
}

GRectItem::GRectItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : GItemHandler( it ), QGraphicsRectItem( parent )
{
    //qDebug( "GRectItem::GRectItem()" );
    QPen pen;
    pen.setColor( Qt::black );
    pen.setStyle( static_cast<Qt::PenStyle>(it->line()) );
    QBrush brush;
    brush.setColor( Qt::white );
    //brush.setStyle( static_cast<Qt::BrushStyle>(it->pattern()) );
    setPen( pen );
    setBrush( brush );
    setRect( 0, 0, w, h );
    setAcceptHoverEvents( true );
}

GRectItem::~GRectItem()
{
    qDebug( "GRectItem::~GRectItem()" );
}

void GRectItem::paint( QPainter *p, const QStyleOptionGraphicsItem *o, QWidget *w )
{
    if ( transform().isIdentity() ) p->setRenderHint( QPainter::Antialiasing, false );
    else p->setRenderHint( QPainter::Antialiasing, true );
    QGraphicsRectItem::paint( p, o, w );
}

void GRectItem::mouseReleaseEvent( QGraphicsSceneMouseEvent *e )
{
    qDebug( "GRECT mouserelease" );
    QGraphicsRectItem::mouseReleaseEvent( e );
    emit onMouseUp( e );
    if ( boundingRect().contains( e->pos() ) ) emit onClick( e );
}

void GRectItem::mouseDoubleClickEvent( QGraphicsSceneMouseEvent *e )
{
    QGraphicsRectItem::mouseDoubleClickEvent( e );
    emit onDblClick( e );
}

void GRectItem::mousePressEvent( QGraphicsSceneMouseEvent *e )
{
    _item->checkForDragInMousePressEvent( e );
    emit onMouseDown( e );
    e->accept();
}

void GRectItem::mouseMoveEvent( QGraphicsSceneMouseEvent *e )
{
    _item->checkForDragInMouseMoveEvent( e );
    //emit onMouseMove( e );
}

void GRectItem::focusInEvent( QFocusEvent *e )
{
    QGraphicsRectItem::focusInEvent( e );
    emit onFocus( e );
}

void GRectItem::focusOutEvent( QFocusEvent *e )
{
    QGraphicsRectItem::focusOutEvent( e );
    emit onBlur( e );
}

void GRectItem::hoverEnterEvent( QGraphicsSceneHoverEvent *e )
{
    QGraphicsRectItem::hoverEnterEvent( e );
    emit onMouseOver( e );
}

void GRectItem::hoverLeaveEvent( QGraphicsSceneHoverEvent *e )
{
    QGraphicsRectItem::hoverLeaveEvent( e );
    emit onMouseOut( e );
}

void GRectItem::hoverMoveEvent( QGraphicsSceneHoverEvent *e )
{
    QGraphicsRectItem::hoverMoveEvent( e );
    emit onMouseMove( e );
}

void GRectItem::keyPressEvent( QKeyEvent *e )
{
    QGraphicsRectItem::keyPressEvent( e );
    emit onKeyDown( e );
    emit onKeyPress( e );
}

void GRectItem::keyReleaseEvent( QKeyEvent *e )
{
    QGraphicsRectItem::keyReleaseEvent( e );
    emit onKeyUp( e );
}

void GRectItem::dragEnterEvent( QGraphicsSceneDragDropEvent *e )
{
    _item->checkDragEnterEvent( e );
}

void GRectItem::dragMoveEvent( QGraphicsSceneDragDropEvent *e )
{
    _item->checkDragMoveEvent( e );
}

void GRectItem::dropEvent( QGraphicsSceneDragDropEvent *e )
{
    _item->checkDropEvent( e );
}

GPixmapItem::GPixmapItem( const QPixmap &pix, const PHIAItem *it, QGraphicsItem *parent )
    : GRectItem( static_cast<qreal>(pix.width()), static_cast<qreal>(pix.height()), it, parent )
{
    qDebug( "GPixmapItem::GPixmapItem()" );
    _pix=pix;
}

GPixmapItem::~GPixmapItem()
{
    qDebug( "GPixmapItem::~GPixmapItem()" );
}

void GPixmapItem::paint( QPainter *painter, const QStyleOptionGraphicsItem*, QWidget* )
{
    painter->setRenderHint( QPainter::Antialiasing, false );
    if ( !_pix.isNull() ) painter->drawPixmap( rect().toRect(), _pix );
}

GRoundedRectItem::GRoundedRectItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : GRectItem( w, h, it, parent ), _radius( 15 )
{
    qDebug( "GRoundedRectItem::GRoundedRectItem()" );
}

GRoundedRectItem::~GRoundedRectItem()
{
    qDebug( "GRoundedRectItem::~GRoundedRectItem()" );
}

void GRoundedRectItem::paint( QPainter *painter, const QStyleOptionGraphicsItem*, QWidget* )
{
    painter->setRenderHint( QPainter::Antialiasing, true );
    //if ( pen().style()==Qt::NoPen ) painter->setRenderHint( QPainter::Antialiasing, true );
    //else if ( pen().width()!=1 ) painter->setRenderHint( QPainter::Antialiasing, true );
    painter->setBrush( brush() );
    painter->setPen( pen() );
    painter->drawRoundedRect( rect(), _radius, _radius );
}

GDiaShowItem::GDiaShowItem( qreal width, qreal height, const PHIAItem *it, QGraphicsItem *parent )
    : GRectItem( width, height, it, parent ), _currentImage( 0 ), _ft( 4 ), _fi( 2 ), _opac( 1. )
{
    qDebug( "GDiaShowItem::GDiaShowItem()" );
    _fadeTimer=new QTimer( this );
    _pauseTimer=new QTimer( this );
    _fadeTimer->setInterval( 50 );
    _pauseTimer->setInterval( _fi*1000 );
    connect( _fadeTimer, SIGNAL( timeout() ), this, SLOT( slotFadeTimeout() ) );
    connect( _pauseTimer, SIGNAL( timeout() ), this, SLOT( slotPauseTimeout() ) );
}

GDiaShowItem::~GDiaShowItem()
{
    qDebug( "GDiaShowItem::~GDiaShowItem()" );
}

void GDiaShowItem::setFadeInterval( qint16 fi )
{
    _fi=fi;
    if ( fi<_ft ) _fi=_ft;
    _pauseTimer->setInterval( _fi*1000 );
}

void GDiaShowItem::setFadeTime( qint16 ft )
{
    _ft=ft;
    if ( ft>_fi )_ft=_fi;
    _step=50./(_ft*1000.);
}

void GDiaShowItem::setPixmap( const QPixmap &pix, int i )
{
    _images.insert( i, pix );
    qDebug( "insert pix %d", i );
    if ( _images.contains( 0 ) && _images.contains( 1 ) ) {
        qDebug( "starting fader" );
        _fadeTimer->start();
        _pauseTimer->start();
    } else update();
}

void GDiaShowItem::paint( QPainter *painter, const QStyleOptionGraphicsItem*, QWidget* )
{
    if ( !_images.count() ) return;
    painter->setRenderHint( QPainter::Antialiasing, false );
    QPixmap pix1, pix2;
    int next=_currentImage+1;
    if ( next>=_images.count() ) next=0;
    pix1=_images.value( _currentImage );
    pix2=_images.value( next );
    painter->setOpacity( _opac );
    if ( !pix1.isNull() ) painter->drawPixmap( rect().toRect(), pix1 );
    painter->setOpacity( 1.-_opac );
    if ( !pix2.isNull() ) painter->drawPixmap( rect().toRect(), pix2 );
}

void GDiaShowItem::slotFadeTimeout()
{
    _opac=_opac-_step;
    if ( _opac<0 ) {
        _opac=1.;
        _fadeTimer->stop();
        _currentImage++;
        if ( _currentImage>=_images.count() ) _currentImage=0;
        if ( _currentImage < _titles.count() ) setToolTip( _titles.at( _currentImage ) );
        else if ( _titles.count()>1 ) setToolTip( QString() );
        else setToolTip( _titles.count() ? _titles.first() : QString() );
        //_pauseTimer->start();
    }
    update();
}

void GDiaShowItem::slotPauseTimeout()
{
    _fadeTimer->stop();
    _pauseTimer->stop();
    _fadeTimer->start();
    _pauseTimer->start();
}

void GDiaShowItem::hoverEnterEvent( QGraphicsSceneHoverEvent *e )
{
    if ( _images.count()<2 ) return GRectItem::hoverEnterEvent( e );
    //setCursor( QCursor( Qt::WaitCursor ) );
    _fadeTimer->stop();
    _pauseTimer->stop();
    if ( _opac<1. ) {
        _currentImage++;
        if ( _currentImage>=_images.count() ) _currentImage=0;
        if ( _currentImage < _titles.count() ) setToolTip( _titles.at( _currentImage ) );
        else if ( _titles.count()>1 ) setToolTip( QString() );
        else setToolTip( _titles.count() ? _titles.first() : QString() );
    }
    _opac=1.;
    GRectItem::hoverEnterEvent( e );
    update();
}

void GDiaShowItem::hoverLeaveEvent( QGraphicsSceneHoverEvent *e )
{
    if( _images.count()<2 ) return GRectItem::hoverLeaveEvent( e );
    //setCursor( QCursor( Qt::ArrowCursor ) );
    _fadeTimer->start();
    _pauseTimer->start();
    GRectItem::hoverLeaveEvent( e );
    update();
}

GSvgItem::GSvgItem( const PHIAItem *it, qreal w, qreal h, QGraphicsItem *parent )
    : QGraphicsSvgItem( parent ), _item( it ), _size( QSizeF( w, h ) ), _rect( 0, 0, w, h )
{
    connect( this, SIGNAL( onClick( QGraphicsSceneMouseEvent* ) ),
        it, SLOT( slotOnClick( QGraphicsSceneMouseEvent* ) ) );
    connect( this, SIGNAL( onDblClick( QGraphicsSceneMouseEvent* ) ),
        it, SLOT( slotOnDblClick( QGraphicsSceneMouseEvent* ) ) );
    connect( this, SIGNAL( onFocus( QFocusEvent* ) ), it, SLOT( slotOnFocus( QFocusEvent* ) ) );
    connect( this, SIGNAL( onBlur( QFocusEvent* ) ), it, SLOT( slotOnBlur( QFocusEvent* ) ) );
    connect( this, SIGNAL( onMouseUp( QGraphicsSceneMouseEvent* ) ),
        it, SLOT( slotOnMouseUp( QGraphicsSceneMouseEvent* ) ) );
    connect( this, SIGNAL( onMouseDown( QGraphicsSceneMouseEvent* ) ),
        it, SLOT( slotOnMouseDown( QGraphicsSceneMouseEvent* ) ) );
    connect( this, SIGNAL( onMouseOver( QGraphicsSceneHoverEvent* ) ),
        it, SLOT( slotOnMouseOver( QGraphicsSceneHoverEvent* ) ) );
    connect( this, SIGNAL( onMouseOut( QGraphicsSceneHoverEvent* ) ),
        it, SLOT( slotOnMouseOut( QGraphicsSceneHoverEvent* ) ) );
    connect( this, SIGNAL( onKeyPress( QKeyEvent* ) ), it, SLOT( slotOnKeyPress( QKeyEvent* ) ) );
    connect( this, SIGNAL( onKeyDown( QKeyEvent* ) ), it, SLOT( slotOnKeyDown( QKeyEvent* ) ) );
    connect( this, SIGNAL( onKeyUp( QKeyEvent* ) ), it, SLOT( slotOnKeyUp( QKeyEvent* ) ) );
    connect( this, SIGNAL( onMouseMove( QGraphicsSceneHoverEvent* ) ),
        it, SLOT( slotOnMouseMove( QGraphicsSceneHoverEvent* ) ) );
    connect( this, SIGNAL( onChange() ), it, SLOT( slotOnChange() ) );
    setAcceptHoverEvents( true );
}

GSvgItem::~GSvgItem()
{
    qDebug( "GSvgItem::~GSvgItem()" );
}

void GSvgItem::setValue( const QString &src )
{
    _value=src;
    _renderer.load( src.toUtf8() );
    setSharedRenderer( &_renderer );
    emit onChange();
}

void GSvgItem::setSize( const QSizeF &s )
{
    prepareGeometryChange();
    scale( s.width()/_size.width(), s.height()/_size.height() );
    _size=s;
}

void GSvgItem::scale( qreal sx, qreal sy )
{
    prepareGeometryChange();
    _rect.setWidth( _rect.width()*sx );
    _rect.setHeight( _rect.height()*sy );
    QGraphicsSvgItem::scale( sx, sy );
}

void GSvgItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    painter->setRenderHint( QPainter::Antialiasing, true );
    QGraphicsSvgItem::paint( painter, option, widget );
}

void GSvgItem::mouseReleaseEvent( QGraphicsSceneMouseEvent *e )
{
    QGraphicsSvgItem::mouseReleaseEvent( e );
    emit onMouseUp( e );
    if ( boundingRect().contains( e->pos() ) ) emit onClick( e );
}

void GSvgItem::mouseDoubleClickEvent( QGraphicsSceneMouseEvent *e )
{
    QGraphicsSvgItem::mouseDoubleClickEvent( e );
    emit onDblClick( e );
}

void GSvgItem::mousePressEvent( QGraphicsSceneMouseEvent *e )
{
    _item->checkForDragInMousePressEvent( e );
    emit onMouseDown( e );
    e->accept();
}

void GSvgItem::mouseMoveEvent( QGraphicsSceneMouseEvent *e )
{
    _item->checkForDragInMouseMoveEvent( e );
    //emit onMouseMove( e );
}

void GSvgItem::focusInEvent( QFocusEvent *e )
{
    QGraphicsSvgItem::focusInEvent( e );
    emit onFocus( e );
}

void GSvgItem::focusOutEvent( QFocusEvent *e )
{
    QGraphicsSvgItem::focusOutEvent( e );
    emit onBlur( e );
}

void GSvgItem::hoverEnterEvent( QGraphicsSceneHoverEvent *e )
{
    QGraphicsSvgItem::hoverEnterEvent( e );
    emit onMouseOver( e );
}

void GSvgItem::hoverLeaveEvent( QGraphicsSceneHoverEvent *e )
{
    QGraphicsSvgItem::hoverLeaveEvent( e );
    emit onMouseOut( e );
}

void GSvgItem::hoverMoveEvent( QGraphicsSceneHoverEvent *e )
{
    QGraphicsSvgItem::hoverMoveEvent( e );
    emit onMouseMove( e );
}

void GSvgItem::keyPressEvent( QKeyEvent *e )
{
    QGraphicsSvgItem::keyPressEvent( e );
    emit onKeyDown( e );
    emit onKeyPress( e );
}

void GSvgItem::keyReleaseEvent( QKeyEvent *e )
{
    QGraphicsSvgItem::keyReleaseEvent( e );
    emit onKeyUp( e );
}

void GSvgItem::dragEnterEvent( QGraphicsSceneDragDropEvent *e )
{
    _item->checkDragEnterEvent( e );
}

void GSvgItem::dragMoveEvent( QGraphicsSceneDragDropEvent *e )
{
    _item->checkDragMoveEvent( e );
}

void GSvgItem::dropEvent( QGraphicsSceneDragDropEvent *e )
{
    _item->checkDropEvent( e );
}
