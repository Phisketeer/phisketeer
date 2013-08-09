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
#include <QFontMetrics>
#include <QGraphicsSceneDragDropEvent>
#include <QPainter>
#include <QMimeData>
#include <QUndoStack>
#include "phiabstractitems.h"
#include "phibasepage.h"
#include "phiundo.h"

PHIAbstractTextItem::PHIAbstractTextItem( Type type, PHIBasePage *page )
    : PHIBaseItem( type, page )
{
}

void PHIAbstractTextItem::setText( const QString &t, const QString &lang )
{
    Q_UNUSED( lang )
    setWidgetText( t );
}

QString PHIAbstractTextItem::text( const QString &lang ) const
{
    Q_UNUSED( lang )
    return QString();
}

QSizeF PHIAbstractTextItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    Q_UNUSED( constraint )
    if ( which==Qt::MinimumSize ) {
        if ( isSingleLine() ) return QSizeF( 21, 21 );
        return QSizeF( 21, 42 );
    }
    if ( which==Qt::PreferredSize ) {
        QFontMetrics m( font() );
        return QSizeF( 150, qMax( 21, m.height()+4 ) );
    }
    return QSizeF();
}

qreal PHIAbstractShapeItem::_dropRegion=6.;

static void _extractShapeDefs( const QString &s, PHIPalette::ColorRole &cr, quint8 &pattern, quint8 &style, qreal &penWidth )
{
    QStringList list=s.split( L1( ";" ), QString::SkipEmptyParts );
    for ( int i=0; i<list.count(); i++ ) {
        QString t=list.at( i );
        if ( t.startsWith( L1( "Pattern:" ) ) ) {
            pattern=static_cast<quint8>(t.mid( 8 ).toUShort());
            continue;
        }
        if ( t.startsWith( L1( "ColorRole:" ) ) ) {
            cr=static_cast<PHIPalette::ColorRole>(t.mid( 10 ).toUShort());
            continue;
        }
        if ( t.startsWith( L1( "PenStyle:" ) ) ) {
            style=static_cast<quint8>(t.mid( 9 ).toShort());
            continue;
        }
        if ( t.startsWith( L1( "PenWidth:" ) ) ) {
            penWidth=static_cast<qreal>(t.mid( 9 ).toDouble());
            continue;
        }
    }
}

PHIAbstractShapeItem::PHIAbstractShapeItem( Type type, PHIBasePage *page )
    : PHIBaseItem( type, page )
{
    _colorRole=PHIPalette::Black;
    _outlineColorRole=PHIPalette::Black;
}

void PHIAbstractShapeItem::paint( QPainter *p, const QRectF &exposed )
{
    p->save();
    QPen pen;
    Qt::PenStyle style=static_cast<Qt::PenStyle>(line());
    Qt::BrushStyle pat=static_cast<Qt::BrushStyle>(pattern());
    if ( style!=Qt::NoPen ) {
        pen.setColor( outlineColor() );
        pen.setWidthF( penWidth() );
        pen.setJoinStyle( Qt::MiterJoin );
        pen.setCapStyle( Qt::FlatCap );
        pen.setStyle( style );
    } else pen.setStyle( Qt::NoPen );
    QBrush brush;
    if ( pattern()< 15 ) { // no gradient
        brush.setStyle( pat );
        brush.setColor( color() );
    }
    p->setBrush( brush );
    p->setPen( pen );
    drawShape( p, exposed );
    p->restore();
}

QColor PHIAbstractShapeItem::color( PHIPalette::ItemRole role ) const
{
    if ( role==PHIPalette::Foreground ) return color();
    if ( role==PHIPalette::Background ) return outlineColor();
    return QColor();
}

PHIPalette::ColorRole PHIAbstractShapeItem::colorRole( PHIPalette::ItemRole role ) const
{
    if ( role==PHIPalette::Foreground ) return _colorRole;
    if ( role==PHIPalette::Background ) return _outlineColorRole;
    return PHIPalette::NoRole;
}

void PHIAbstractShapeItem::setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col )
{
    if ( ir==PHIPalette::Foreground ) {
        _colorRole=cr;
        _variants.insert( DColor, col );
        update();
        return;
    }
    if ( ir==PHIPalette::Background ) {
        _outlineColorRole=cr;
        _variants.insert( DOutlineColor, col );
        update();
        return;
    }
}

void PHIAbstractShapeItem::setPattern( quint8 p )
{
    Qt::BrushStyle s;
    if ( p > static_cast<quint8>(Qt::LinearGradientPattern) ) s=Qt::NoBrush;
    else s=static_cast<Qt::BrushStyle>(p);
    _variants.insert( DPatternStyle, static_cast<quint8>(s) );
    if ( s==Qt::SolidPattern ) _variants.remove( DPatternStyle );
    update();
}

void PHIAbstractShapeItem::setPenWidth( qreal w )
{
    if ( w==1. ) _variants.remove( DPenWidth );
    else _variants.insert( DPenWidth, w );
    update();
}

void PHIAbstractShapeItem::setLine( quint8 l )
{
    Qt::PenStyle s;
    if ( l > static_cast<quint8>(Qt::DashDotDotLine) ) s=Qt::NoPen;
    else s=static_cast<Qt::PenStyle>(l);
    _variants.insert( DLineStyle, static_cast<quint8>(s) );
    if ( s==Qt::NoPen ) _variants.remove( DLineStyle );
    update();
}

QRectF PHIAbstractShapeItem::boundingRect() const
{
    int off=static_cast<int>(penWidth()/2.)+1;
    return QRectF( -off, -off, width()+2*off, height()+2*off );
}

QSizeF PHIAbstractShapeItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    Q_UNUSED( constraint )
    if ( which==Qt::PreferredSize ) return QSizeF( 100, 100 );
    if ( which==Qt::MinimumSize ) return QSizeF( 16, 16 );
    return QSizeF();
}

void PHIAbstractShapeItem::ideDragEnterEvent( QGraphicsSceneDragDropEvent *e )
{
    if ( !e->mimeData()->hasColor() ) return e->ignore();
    _variants.insert( DTmpColor, _variants.value( DColor ) );
    _variants.insert( DTmpOutlineColor, _variants.value( DOutlineColor ) );
    if ( e->mimeData()->hasText() ) {
        _variants.insert( DTmpPatternStyle, pattern() );
        _variants.insert( DTmpLineStyle, line() );
        _variants.insert( DTmpPenWidth, penWidth() );
    }
    e->setDropAction( Qt::MoveAction );
    e->accept();
}

void PHIAbstractShapeItem::ideDragMoveEvent( QGraphicsSceneDragDropEvent *e )
{
    if ( !e->mimeData()->hasColor() ) return;
    QRectF r=QRectF( QPointF( _dropRegion, _dropRegion ),
        QSizeF( width()-_dropRegion, height()-_dropRegion ) );
    if ( e->mimeData()->hasText() ) {
        bool forceForeground=false;
        bool forceBackground=false;
        PHIPalette::ColorRole newCR=PHIPalette::Custom;
        qreal newPenWidth=_variants.value( DTmpPenWidth, 1. ).value<qreal>();
        quint8 newPattern=_variants.value( DTmpPatternStyle, 1 ).value<quint8>();
        quint8 newLineStyle=_variants.value( DTmpLineStyle, 0 ).value<quint8>();
        _extractShapeDefs( e->mimeData()->text(), newCR, newPattern, newLineStyle, newPenWidth );
        if ( newPenWidth!=_variants.value( DTmpPenWidth, 1. ).value<qreal>() ) forceBackground=true;
        if ( newPattern!=_variants.value( DTmpPatternStyle, 1 ).value<qreal>() ) forceForeground=true;
        if ( newLineStyle!=_variants.value( DTmpLineStyle, 0 ).value<qreal>() ) forceBackground=true;
        if ( forceForeground ) {
            setOutlineColor( _variants.value( DTmpOutlineColor, QColor( Qt::black ) ).value<QColor>() );
            setColor( e->mimeData()->colorData().value<QColor>() );
            setPattern( newPattern );
            update();
            return;
        } else if ( forceBackground ) {
            setOutlineColor( e->mimeData()->colorData().value<QColor>() );
            setColor( _variants.value( DTmpColor, QColor( Qt::black ) ).value<QColor>() );
            setPenWidth( newPenWidth );
            setLine( newLineStyle );
            update();
            return;
        }
    }
    if ( !r.contains( e->pos() ) ) {
        setOutlineColor( e->mimeData()->colorData().value<QColor>() );
        setColor( _variants.value( DTmpColor, QColor( Qt::black ) ).value<QColor>() );
    } else {
        setOutlineColor( _variants.value( DTmpOutlineColor, QColor( Qt::black ) ).value<QColor>() );
        setColor( e->mimeData()->colorData().value<QColor>() );
    }
    update();
}

void PHIAbstractShapeItem::ideDragLeaveEvent( QGraphicsSceneDragDropEvent *e )
{
    if ( !e->mimeData()->hasColor() ) return;
    setColor( _variants.value( DTmpColor, QColor( Qt::black ) ).value<QColor>() );
    setOutlineColor( _variants.value( DTmpOutlineColor, QColor( Qt::black ) ).value<QColor>() );
    if ( e->mimeData()->hasText() ) {
        setPattern( _variants.value( DTmpPatternStyle, 1 ).value<quint8>() );
        setLine( _variants.value( DTmpLineStyle, 0 ).value<quint8>() );
        setPenWidth( _variants.value( DTmpPenWidth, 1. ).value<qreal>() );
    }
    update();
}

void PHIAbstractShapeItem::ideDropEvent( QGraphicsSceneDragDropEvent *e )
{
    if ( !e->mimeData()->hasColor() ) return;
    QString ut=QString( L1( " <%1>" ) ).arg( name() );
    // restore old colors:
    setColor( _variants.value( DTmpColor, QColor( Qt::black ) ).value<QColor>() );
    setOutlineColor( _variants.value( DTmpOutlineColor, QColor( Qt::black ) ).value<QColor>() );
    QRectF r=QRectF( QPointF( _dropRegion, _dropRegion ),
        QSizeF( width()-_dropRegion, height()-_dropRegion ) );
    if ( e->mimeData()->hasText() ) {
        // restore old styles:
        setPattern( _variants.value( DTmpPatternStyle, 1 ).value<quint8>() );
        setLine( _variants.value( DTmpLineStyle, 0 ).value<quint8>() );
        setPenWidth( _variants.value( DTmpPenWidth, 1. ).value<qreal>() );

        bool forceForeground=false;
        bool forceBackground=false;
        PHIPalette::ColorRole newCR=PHIPalette::Custom;
        qreal newPenWidth=penWidth();
        quint8 newPattern=pattern();
        quint8 newLineStyle=line();
        _extractShapeDefs( e->mimeData()->text(), newCR, newPattern, newLineStyle, newPenWidth );
        if ( newPenWidth!=penWidth() ) forceBackground=true;
        if ( newPattern!=pattern() ) forceForeground=true;
        if ( newLineStyle!=line() ) forceBackground=true;
        if ( forceForeground ) { // drag from pattern tool
            undoStack()->beginMacro( tr( "Pattern" )+ut );
            undoStack()->push( new PHIUndoColor( this, PHIPalette::Foreground,
                newCR, e->mimeData()->colorData().value<QColor>() ) );
            undoStack()->push( new PHIUndoProperty( this, PHIUndoCommand::Pattern, newPattern ) );
            undoStack()->endMacro();
            update();
            return;
        } else if ( forceBackground ) { // drag from pen tool
            undoStack()->beginMacro( tr( "Outline" )+ut );
            undoStack()->push( new PHIUndoColor( this, PHIPalette::Background,
                newCR, e->mimeData()->colorData().value<QColor>() ) );
            undoStack()->push( new PHIUndoProperty( this, PHIUndoCommand::Line, newLineStyle ) );
            undoStack()->push( new PHIUndoProperty( this, PHIUndoCommand::PenWidth, newPenWidth ) );
            undoStack()->endMacro();
            update();
            return;
        }
        // drag from color button
        if ( r.contains( e->pos() ) ) {
            undoStack()->push( new PHIUndoColor( this, PHIPalette::Foreground,
                newCR, e->mimeData()->colorData().value<QColor>() ) );
        } else {
            undoStack()->push( new PHIUndoColor( this, PHIPalette::Background,
                newCR, e->mimeData()->colorData().value<QColor>() ) );
        }
        update();
        return;
    }
    // drag from external app
    if ( r.contains( e->pos() ) ) {
        undoStack()->push( new PHIUndoColor( this, PHIPalette::Foreground,
            PHIPalette::Custom, e->mimeData()->colorData().value<QColor>() ) );
    } else {
        undoStack()->push( new PHIUndoColor( this, PHIPalette::Background,
            PHIPalette::Custom, e->mimeData()->colorData().value<QColor>() ) );
    }
    update();
}
