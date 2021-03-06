/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2014  Phisys AG, Switzerland
#    Copyright (C) 2012-2014  Phisketeer.org team
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
#include <QFontMetricsF>
#include <QGraphicsSceneDragDropEvent>
#include <QPainter>
#include <QMimeData>
#include <QWidget>
#include <QGraphicsGridLayout>
#include <QHttpMultiPart>
#include <QHttpPart>
#include "phiabstractitems.h"
#include "phicolorconfig.h"
#include "philayoutconfig.h"
#include "phibasepage.h"
#include "phigraphicsitem.h"
#include "phidataparser.h"
#include "phiwebpage.h"
#include "phiimagerequest.h"

qreal PHIAbstractTextItem::_dropRegion=8.;
qreal PHIAbstractShapeItem::_dropRegion=8.;

static void _extractColorRole( const QString &s, PHIPalette::ColorRole &cr )
{
    QStringList list=s.split( L1( ";" ), QString::SkipEmptyParts );
    for ( int i=0; i<list.count(); i++ ) {
        QString t=list.at( i );
        if ( t.startsWith( L1( "ColorRole:" ) ) ) {
            cr=static_cast<PHIPalette::ColorRole>(t.mid( 10 ).toUShort());
            break;
        }
    }
}

void PHIAbstractTextItem::ideInit()
{
    setColor( PHIPalette::WidgetText, PHIPalette::Text, page()->phiPalette().color( PHIPalette::Text ) );
    setColor( PHIPalette::WidgetBase, PHIPalette::Base, page()->phiPalette().color( PHIPalette::Base ) );
    textData()->setText( L1( "Example text" ) );
}

PHIConfigWidget* PHIAbstractTextItem::ideConfigWidget()
{
    return new PHIColorConfig( this );
}

// called form IDE only:
void PHIAbstractTextItem::ideSetText( const QString &t, const QByteArray &lang )
{
    if ( _textData.isTranslated() ) {
        _textData.setText( t, lang );
        setWidgetText( t );
    } else if ( _textData.isStatic() ) {
        _textData.setSource( PHIData::Translated );
        _textData.setText( t, lang );
        setWidgetText( t );
    } else setWidgetText( _textData.text() );
    if ( isChild() ) {
        PHIBaseItem *it=page()->findItem( parentId() );
        PHIAbstractLayoutItem *lit=qobject_cast<PHIAbstractLayoutItem*>(it);
        if ( lit ) lit->invalidateLayout();
    }
}

QString PHIAbstractTextItem::ideText( const QByteArray &lang ) const
{
    if ( _textData.isTranslated() ) return _textData.text( lang );
    return _textData.text();
}

void PHIAbstractTextItem::ideUpdateData()
{
    setWidgetAligment( static_cast<Qt::Alignment>(realAlignment()) );
    if ( _textData.isTranslated() ) {
        setWidgetText( _textData.text( page()->currentLang() ) );
        setText( _textData.text( page()->currentLang() ) );
    } else {
        setWidgetText( _textData.text() );
        setText( _textData.text() );
    }
    if ( isChild() ) {
        PHIBaseItem *it=page()->findItem( parentId() );
        PHIAbstractLayoutItem *lit=qobject_cast<PHIAbstractLayoutItem*>(it);
        if ( lit ) lit->invalidateLayout();
    }
}

void PHIAbstractTextItem::phisCreateData( const PHIDataParser &parser )
{
    setData( DText, parser.text( &_textData ) );
    if ( !_textData.isUnparsedStatic() ) setDirtyFlag( DFText );
}

void PHIAbstractTextItem::phisParseData( const PHIDataParser &parser )
{
    if ( dirtyFlags() & DFText ) setData( DText, parser.text( &_textData ) );
}

void PHIAbstractTextItem::clientPrepareData()
{
    if ( _colorRole!=PHIPalette::Custom ) removeData( DColor );
    if ( _backgroundColorRole!=PHIPalette::Custom ) removeData( DBackgroundColor );
    if ( realAlignment()==static_cast<quint16>(Qt::AlignLeft|Qt::AlignVCenter) ) removeData( DAlignment );
    setData( DTmpColor, static_cast<quint8>(_colorRole) );
    setData( DTmpBackgroundColor, static_cast<quint8>(_backgroundColorRole) );
}

void PHIAbstractTextItem::clientInitData()
{
    _colorRole=static_cast<PHIPalette::ColorRole>(data( DTmpColor ).value<quint8>());
    _backgroundColorRole=static_cast<PHIPalette::ColorRole>(data( DTmpBackgroundColor ).value<quint8>());
    setWidgetText( realText() );
    setWidgetAligment( static_cast<Qt::Alignment>(realAlignment()) );
    setColor( PHIPalette::WidgetText, _colorRole, realColor() );
    setColor( PHIPalette::WidgetBase, _backgroundColorRole, realBackgroundColor() );
}

bool PHIAbstractTextItem::paint( QPainter *painter, const QRectF &exposed )
{
    Q_UNUSED( painter )
    Q_UNUSED( exposed )
    return false;
}

void PHIAbstractTextItem::setColorRole( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr )
{
    if ( ir==PHIPalette::WidgetText ) _colorRole=cr;
    else if ( ir==PHIPalette::WidgetBase ) _backgroundColorRole=cr;
}

QSizeF PHIAbstractTextItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    if ( which==Qt::MinimumSize ) {
        if ( isSingleLine() ) return QSizeF( 20, 21 );
        return QSizeF( 20, 36 );
    }
    if ( which==Qt::PreferredSize ) {
        QFont f=font();
        f.setPointSizeF( PHI::adjustedFontSize( font().pointSizeF() ) );
        QFontMetricsF m( f );
        if ( isSingleLine() ) {
            qreal minWidth=62.;
            if ( page() ) {
                foreach ( QString l, page()->languages() ) {
                    l=ideText( l.toLatin1() );
                    if ( m.width( l )>minWidth ) minWidth=qRound(m.width( l ));
                }
            }
            return QSizeF( minWidth+4., qMax( 22., qRound(m.height())+5. ) );
        }
        return QSizeF( 160, qMax( qRound(m.height())+5., 96. ) );
    }
    return PHIBaseItem::sizeHint( which, constraint );
}

QColor PHIAbstractTextItem::realColor() const
{
    if ( data( DColor ).isValid() ) return data( DColor ).value<QColor>();
    if ( !page() ) return Qt::black;
    return page()->phiPalette().color( _colorRole );
}

QColor PHIAbstractTextItem::realBackgroundColor() const
{
    if ( data( DBackgroundColor ).isValid() ) return data( DBackgroundColor ).value<QColor>();
    if ( !page() ) return Qt::white;
    return page()->phiPalette().color( _backgroundColorRole );
}

void PHIAbstractTextItem::setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col )
{
    if ( ir==PHIPalette::WidgetText ) {
        if ( cr==PHIPalette::Custom ) setData( DColor, col );
        _colorRole=cr;
    } else if ( ir==PHIPalette::WidgetBase ) {
        if ( cr==PHIPalette::Custom ) setData( DBackgroundColor, col );
        _backgroundColorRole=cr;
    } else return;
    QWidget *w=widget();
    if ( !w ) return;
    QPalette::ColorRole role=QPalette::Text;
    if ( ir==PHIPalette::WidgetBase ) role=QPalette::Base;
    QPalette pal=w->palette();
    pal.setColor( role, col );
    w->setPalette( pal );
}

QColor PHIAbstractTextItem::colorForRole( PHIPalette::ItemRole role ) const
{
    if ( role==PHIPalette::WidgetText ) return realColor();
    if ( role==PHIPalette::WidgetBase ) return realBackgroundColor();
    return QColor();
}

PHIPalette::ColorRole PHIAbstractTextItem::colorRole( PHIPalette::ItemRole role ) const
{
    if ( role==PHIPalette::WidgetText ) return _colorRole;
    if ( role==PHIPalette::WidgetBase ) return _backgroundColorRole;
    return PHIPalette::NoRole;
}

void PHIAbstractTextItem::ideDragEnterEvent( QGraphicsSceneDragDropEvent *e )
{
    if ( !e->mimeData()->hasColor() ) return e->ignore();
    setData( DTmpColor, realColor() );
    setData( DTmpBackgroundColor, realBackgroundColor() );
    e->setDropAction( Qt::CopyAction );
    e->accept();
}

void PHIAbstractTextItem::ideDragMoveEvent( QGraphicsSceneDragDropEvent *e )
{
    if ( !e->mimeData()->hasColor() ) return;
    QRectF r=QRectF( QPointF( _dropRegion, _dropRegion ),
        QSizeF( realWidth()-_dropRegion-6, realHeight()-_dropRegion-6 ) );
    if ( !r.contains( e->pos() ) ) {
        setColor( PHIPalette::WidgetBase, _backgroundColorRole, e->mimeData()->colorData().value<QColor>() );
        setColor( PHIPalette::WidgetText, _colorRole, data( DTmpColor, QColor( Qt::black ) ).value<QColor>() );
    } else {
        setColor( PHIPalette::WidgetBase, _backgroundColorRole, data( DTmpBackgroundColor, QColor( Qt::black ) ).value<QColor>() );
        setColor( PHIPalette::WidgetText, _colorRole, e->mimeData()->colorData().value<QColor>() );
    }
    update();
}

void PHIAbstractTextItem::ideDragLeaveEvent( QGraphicsSceneDragDropEvent *e )
{
    if ( !e->mimeData()->hasColor() ) return;
    setColor( PHIPalette::WidgetText, _colorRole, data( DTmpColor, QColor( Qt::black ) ).value<QColor>() );
    setColor( PHIPalette::WidgetBase, _backgroundColorRole, data( DTmpBackgroundColor, QColor( Qt::black ) ).value<QColor>() );
    update();
}

void PHIAbstractTextItem::ideDropEvent( QGraphicsSceneDragDropEvent *e )
{
    if ( !e->mimeData()->hasColor() ) return;
    // restore old colors:
    setColor( PHIPalette::WidgetText, _colorRole, data( DTmpColor, QColor( Qt::black ) ).value<QColor>() );
    setColor( PHIPalette::WidgetBase, _backgroundColorRole, data( DTmpBackgroundColor, QColor( Qt::black ) ).value<QColor>() );
    QRectF r=QRectF( QPointF( _dropRegion, _dropRegion ),
        QSizeF( realWidth()-_dropRegion-6, realHeight()-_dropRegion-6 ) );
    if ( e->mimeData()->hasText() ) {
        PHIPalette::ColorRole newCR=PHIPalette::Custom;
        _extractColorRole( e->mimeData()->text(), newCR );
        // drag from color button
        if ( r.contains( e->pos() ) ) {
            emit pushUndoStack( PHIPalette::WidgetText, newCR, e->mimeData()->colorData().value<QColor>() );
        } else {
            emit pushUndoStack( PHIPalette::WidgetBase, newCR, e->mimeData()->colorData().value<QColor>() );
        }
        update();
        return;
    }
    // drag from external app
    if ( r.contains( e->pos() ) ) {
        emit pushUndoStack( PHIPalette::WidgetText, PHIPalette::Custom, e->mimeData()->colorData().value<QColor>() );
    } else {
        emit pushUndoStack( PHIPalette::WidgetBase, PHIPalette::Custom, e->mimeData()->colorData().value<QColor>() );
    }
    update();
}

void PHIAbstractTextItem::squeeze()
{
    removeData( DText );
    removeData( DTmpBackgroundColor );
    removeData( DTmpColor );
    if ( _colorRole!=PHIPalette::Custom ) removeData( DColor );
    if ( _backgroundColorRole!=PHIPalette::Custom ) removeData( DBackgroundColor );
    if ( realAlignment()==static_cast<quint16>(Qt::AlignLeft|Qt::AlignVCenter) ) removeData( DAlignment );
}

void PHIAbstractTextItem::loadItemData( QDataStream &in, int version )
{
    Q_UNUSED( version )
    quint8 cr, bgcr;
    in >> &_textData >> cr >> bgcr;
    _colorRole=static_cast<PHIPalette::ColorRole>(cr);
    _backgroundColorRole=static_cast<PHIPalette::ColorRole>(bgcr);
    setColor( PHIPalette::WidgetText, _colorRole, realColor() );
    setColor( PHIPalette::WidgetBase, _backgroundColorRole, realBackgroundColor() );
}

void PHIAbstractTextItem::saveItemData( QDataStream &out, int version )
{
    Q_UNUSED( version )
    out << &_textData << static_cast<quint8>(_colorRole) << static_cast<quint8>(_backgroundColorRole);
}

QScriptValue PHIAbstractTextItem::textAlign( const QScriptValue &a )
{
    if ( !a.isValid() ) return PHI::qtAlignmentToTextAlign( static_cast<Qt::Alignment>(realAlignment()) );
    setAlignment( PHI::textAlignToQtAlignment( a.toString() ) );
    return self();
}

QScriptValue PHIAbstractTextItem::color( const QScriptValue &c )
{
    if ( !c.isValid() ) return PHI::colorToString( realColor() );
    setColor( PHIPalette::WidgetText, PHIPalette::Custom, PHI::colorFromString( c.toString() ) );
    setDirtyFlag( DFColor );
    return self();
}

QScriptValue PHIAbstractTextItem::bgColor( const QScriptValue &c )
{
    if ( !c.isValid() ) return PHI::colorToString( realBackgroundColor() );
    setColor( PHIPalette::WidgetBase, PHIPalette::Custom, PHI::colorFromString( c.toString() ) );
    setDirtyFlag( DFBackgroundColor );
    return self();
}

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

PHIConfigWidget* PHIAbstractShapeItem::ideConfigWidget()
{
    return new PHIColorConfig( this );
}

void PHIAbstractShapeItem::ideInit()
{
    setColor( PHIPalette::Foreground, PHIPalette::Black, page()->phiPalette().color( PHIPalette::Black ) );
    setColor( PHIPalette::Outline, PHIPalette::Black, page()->phiPalette().color( PHIPalette::Black ) );
    setLine( 1 );
    setPattern( 15 );
}

void PHIAbstractShapeItem::phisCreateData( const PHIDataParser &parser )
{
    setImagePath( parser.createImage( createImage() ) );
}

void PHIAbstractShapeItem::phisParseData( const PHIDataParser &parser )
{
    if ( dirtyFlags() & DFColor || dirtyFlags() & DFBackgroundColor || dirtyFlags() & DFDoNotCache ) {
        setDirtyFlag( DFDoNotCache );
        setImagePath( parser.createImage( createImage(), PHIData::c(), -1 ) );
    }
}

void PHIAbstractShapeItem::clientPrepareData()
{
    PHIAbstractShapeItem::squeeze();
    removeData( DImagePath );
    setData( DTmpColor, static_cast<quint8>(_colorRole) );
    setData( DTmpOutlineColor, static_cast<quint8>(_outlineColorRole) );
}

void PHIAbstractShapeItem::clientInitData()
{
    _colorRole=static_cast<PHIPalette::ColorRole>(data( DTmpColor ).value<quint8>());
    _outlineColorRole=static_cast<PHIPalette::ColorRole>(data( DTmpOutlineColor ).value<quint8>());
}

bool PHIAbstractShapeItem::paint( QPainter *p, const QRectF &exposed )
{
    QPen pen;
    Qt::PenStyle style=static_cast<Qt::PenStyle>(realLine());
    Qt::BrushStyle pat=static_cast<Qt::BrushStyle>(realPattern());
    if ( style!=Qt::NoPen ) {
        pen.setColor( realOutlineColor() );
        pen.setWidthF( realPenWidth() );
        pen.setJoinStyle( Qt::MiterJoin );
        pen.setCapStyle( Qt::FlatCap );
        pen.setStyle( style );
    } else pen.setStyle( Qt::NoPen );
    QBrush brush;
    if ( realPattern()<15 ) { // no gradient style
        brush.setStyle( pat );
        brush.setColor( realColor() );
    } else brush=QBrush( gradient() );
    p->setBrush( brush );
    p->setPen( pen );
    p->setRenderHint( QPainter::SmoothPixmapTransform );
    drawShape( p, exposed );
    return true;
}

QColor PHIAbstractShapeItem::colorForRole( PHIPalette::ItemRole role ) const
{
    if ( role==PHIPalette::Foreground ) return realColor();
    if ( role==PHIPalette::Outline ) return realOutlineColor();
    return QColor();
}

PHIPalette::ColorRole PHIAbstractShapeItem::colorRole( PHIPalette::ItemRole role ) const
{
    if ( role==PHIPalette::Foreground ) return _colorRole;
    if ( role==PHIPalette::Outline ) return _outlineColorRole;
    return PHIPalette::NoRole;
}

void PHIAbstractShapeItem::setColorRole( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr )
{
    if ( ir==PHIPalette::Foreground ) _colorRole=cr;
    else if ( ir==PHIPalette::Outline ) _outlineColorRole=cr;
}

void PHIAbstractShapeItem::setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col )
{
    if ( ir==PHIPalette::Foreground ) {
        _colorRole=cr;
        setData( DColor, col );
    }
    if ( ir==PHIPalette::Outline ) {
        _outlineColorRole=cr;
        setData( DOutlineColor, col );
    }
    update();
}

void PHIAbstractShapeItem::setPattern( quint8 p )
{
    Qt::BrushStyle s;
    if ( p > static_cast<quint8>(Qt::LinearGradientPattern) ) s=Qt::LinearGradientPattern;
    else s=static_cast<Qt::BrushStyle>(p);
    setData( DPatternStyle, static_cast<quint8>(s) );
    if ( s==Qt::SolidPattern ) removeData( DPatternStyle );
    update();
}

void PHIAbstractShapeItem::setPenWidth( qreal w )
{
    if ( w==1. ) removeData( DPenWidth );
    else setData( DPenWidth, w );
    update();
}

QColor PHIAbstractShapeItem::realColor() const
{
    if ( data( DColor ).isValid() ) return data( DColor ).value<QColor>();
    if ( !page() ) return Qt::white;
    return page()->phiPalette().color( _colorRole );
}

QColor PHIAbstractShapeItem::realOutlineColor() const
{
    if ( data( DOutlineColor ).isValid() ) return data( DOutlineColor ).value<QColor>();
    if ( !page() ) return Qt::black;
    return page()->phiPalette().color( _outlineColorRole );
}

QScriptValue PHIAbstractShapeItem::color( const QScriptValue &c )
{
    if ( !c.isValid() ) return PHI::colorToString( realColor() );
    setColor( PHIPalette::Foreground, PHIPalette::Custom, PHI::colorFromString( c.toString() ) );
    setDirtyFlag( DFColor );
    return self();
}

QScriptValue PHIAbstractShapeItem::borderColor( const QScriptValue &c )
{
    if ( !c.isValid() ) return PHI::colorToString( realOutlineColor() );
    setColor( PHIPalette::Outline, PHIPalette::Custom, PHI::colorFromString( c.toString() ) );
    setDirtyFlag( DFBackgroundColor );
    return self();
}

QScriptValue PHIAbstractShapeItem::borderWidth( const QScriptValue &w )
{
    if ( !isServerItem() ) return scriptEngine()->undefinedValue();
    if ( !w.isValid() ) return realPenWidth();
    setPenWidth( w.toNumber() );
    setDirtyFlag( DFDoNotCache );
    return self();
}

QScriptValue PHIAbstractShapeItem::patternStyle( const QScriptValue &p )
{
    if ( !isServerItem() ) return scriptEngine()->undefinedValue();
    if ( !p.isValid() ) return realPattern();
    setPattern( qBound( 0, p.toInt32(), 15 ) );
    setDirtyFlag( DFDoNotCache );
    return self();
}

QScriptValue PHIAbstractShapeItem::lineStyle( const QScriptValue &l )
{
    if ( !isServerItem() ) return scriptEngine()->undefinedValue();
    if ( !l.isValid() ) return realLine();
    setLine( qBound( 0, l.toInt32(), 5 ) );
    setDirtyFlag( DFDoNotCache );
    return self();
}

void PHIAbstractShapeItem::setLine( quint8 l )
{
    Qt::PenStyle s;
    if ( l > static_cast<quint8>(Qt::DashDotDotLine) ) s=Qt::NoPen;
    else s=static_cast<Qt::PenStyle>(l);
    setData( DLineStyle, static_cast<quint8>(s) );
    if ( s==Qt::NoPen ) removeData( DLineStyle );
    update();
}

QRectF PHIAbstractShapeItem::boundingRect() const
{
    qreal off=0;
    if ( realLine()>0 ) off=realPenWidth(); // border
    return QRectF( -off, -off, realWidth()+2*off, realHeight()+2*off );
}

QSizeF PHIAbstractShapeItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    Q_UNUSED( constraint )
    if ( isChild() ) return realSize();
    if ( which==Qt::PreferredSize ) return QSizeF( 96., 96. );
    if ( which==Qt::MinimumSize ) return QSizeF( 16., 16. );
    if ( which==Qt::MaximumSize ) return PHI::maxItemSize();
    return QSizeF();
}

void PHIAbstractShapeItem::ideDragEnterEvent( QGraphicsSceneDragDropEvent *e )
{
    if ( !e->mimeData()->hasColor() ) return e->ignore();
    setData( DTmpColor, realColor() );
    setData( DTmpOutlineColor, realOutlineColor() );
    if ( e->mimeData()->hasText() ) {
        setData( DTmpPatternStyle, realPattern() );
        setData( DTmpLineStyle, realLine() );
        setData( DTmpPenWidth, realPenWidth() );
    }
    e->setDropAction( Qt::CopyAction );
    e->accept();
}

void PHIAbstractShapeItem::ideDragMoveEvent( QGraphicsSceneDragDropEvent *e )
{
    if ( !e->mimeData()->hasColor() ) return;
    QRectF r=QRectF( QPointF( _dropRegion, _dropRegion ),
        QSizeF( realWidth()-_dropRegion-6, realHeight()-_dropRegion-6 ) );
    if ( e->mimeData()->hasText() ) {
        bool forceForeground=false;
        bool forceBackground=false;
        PHIPalette::ColorRole newCR=PHIPalette::Custom;
        qreal newPenWidth=data( DTmpPenWidth, 1. ).value<qreal>();
        quint8 newPattern=data( DTmpPatternStyle, 1 ).value<quint8>();
        quint8 newLineStyle=data( DTmpLineStyle, 0 ).value<quint8>();
        _extractShapeDefs( e->mimeData()->text(), newCR, newPattern, newLineStyle, newPenWidth );
        if ( newPenWidth!=data( DTmpPenWidth, 1. ).value<qreal>() ) forceBackground=true;
        if ( newPattern!=data( DTmpPatternStyle, 1 ).value<qreal>() ) forceForeground=true;
        if ( newLineStyle!=data( DTmpLineStyle, 0 ).value<qreal>() ) forceBackground=true;
        if ( forceForeground ) {
            setColor( PHIPalette::Outline, _outlineColorRole, data( DTmpOutlineColor, QColor( Qt::black ) ).value<QColor>() );
            setColor( PHIPalette::Foreground, _colorRole, e->mimeData()->colorData().value<QColor>() );
            setPattern( newPattern );
            update();
            return;
        } else if ( forceBackground ) {
            setColor( PHIPalette::Outline, _outlineColorRole, e->mimeData()->colorData().value<QColor>() );
            setColor( PHIPalette::Foreground, _colorRole, data( DTmpColor, QColor( Qt::black ) ).value<QColor>() );
            setPenWidth( newPenWidth );
            setLine( newLineStyle );
            update();
            return;
        }
    }
    if ( !r.contains( e->pos() ) ) {
        setColor( PHIPalette::Outline, _outlineColorRole, e->mimeData()->colorData().value<QColor>() );
        setColor( PHIPalette::Foreground, _colorRole, data( DTmpColor, QColor( Qt::black ) ).value<QColor>() );
    } else {
        setColor( PHIPalette::Outline, _outlineColorRole, data( DTmpOutlineColor, QColor( Qt::black ) ).value<QColor>() );
        setColor( PHIPalette::Foreground, _colorRole, e->mimeData()->colorData().value<QColor>() );
    }
    update();
}

void PHIAbstractShapeItem::ideDragLeaveEvent( QGraphicsSceneDragDropEvent *e )
{
    if ( !e->mimeData()->hasColor() ) return;
    setColor( PHIPalette::Foreground, _colorRole, data( DTmpColor, QColor( Qt::black ) ).value<QColor>() );
    setColor( PHIPalette::Outline, _outlineColorRole, data( DTmpOutlineColor, QColor( Qt::black ) ).value<QColor>() );
    if ( e->mimeData()->hasText() ) {
        setPattern( data( DTmpPatternStyle, 1 ).value<quint8>() );
        setLine( data( DTmpLineStyle, 0 ).value<quint8>() );
        setPenWidth( data( DTmpPenWidth, 1. ).value<qreal>() );
    }
    update();
}

void PHIAbstractShapeItem::ideDropEvent( QGraphicsSceneDragDropEvent *e )
{
    if ( !e->mimeData()->hasColor() ) return;
    QString ut=QString( L1( " <%1>" ) ).arg( name() );
    // restore old colors:
    setColor( PHIPalette::Foreground, _colorRole, data( DTmpColor, QColor( Qt::black ) ).value<QColor>() );
    setColor( PHIPalette::Outline, _outlineColorRole, data( DTmpOutlineColor, QColor( Qt::black ) ).value<QColor>() );
    QRectF r=QRectF( QPointF( _dropRegion, _dropRegion ),
        QSizeF( realWidth()-_dropRegion-6, realHeight()-_dropRegion-6 ) );
    if ( e->mimeData()->hasText() ) {
        // restore old styles:
        setPattern( data( DTmpPatternStyle, 1 ).value<quint8>() );
        setLine( data( DTmpLineStyle, 0 ).value<quint8>() );
        setPenWidth( data( DTmpPenWidth, 1. ).value<qreal>() );

        bool forceForeground=false;
        bool forceBackground=false;
        PHIPalette::ColorRole newCR=PHIPalette::Custom;
        qreal newPenWidth=realPenWidth();
        quint8 newPattern=realPattern();
        quint8 newLineStyle=realLine();
        _extractShapeDefs( e->mimeData()->text(), newCR, newPattern, newLineStyle, newPenWidth );
        if ( newPenWidth!=realPenWidth() ) forceBackground=true;
        if ( newPattern!=realPattern() ) forceForeground=true;
        if ( newLineStyle!=realLine() ) forceBackground=true;
        if ( forceForeground ) { // drag from pattern tool
            emit beginUndoStackMacro( tr( "Pattern" )+ut );
            emit pushUndoStack( PHIPalette::Foreground, newCR, e->mimeData()->colorData().value<QColor>() );
            emit pushUndoStack( "_pattern", newPattern );
            emit endUndoStackMacro();
            update();
            return;
        } else if ( forceBackground ) { // drag from pen tool
            emit beginUndoStackMacro( tr( "Outline" )+ut );
            emit pushUndoStack( PHIPalette::Background, newCR, e->mimeData()->colorData().value<QColor>() );
            emit pushUndoStack( "_line", newLineStyle );
            emit pushUndoStack( "_penWidth", newPenWidth );
            emit endUndoStackMacro();
            update();
            return;
        }
        // drag from color button
        if ( r.contains( e->pos() ) ) {
            emit beginUndoStackMacro( tr( "Pattern" )+ut );
            emit pushUndoStack( PHIPalette::Foreground, newCR, e->mimeData()->colorData().value<QColor>() );
            if ( realPattern()==15 ) emit pushUndoStack( "_pattern", 1 );
            emit endUndoStackMacro();
        } else {
            emit pushUndoStack( PHIPalette::Background, newCR, e->mimeData()->colorData().value<QColor>() );
        }
        update();
        return;
    }
    // drag from external app
    if ( r.contains( e->pos() ) ) {
        emit beginUndoStackMacro( tr( "Pattern" )+ut );
        emit pushUndoStack( PHIPalette::Foreground, PHIPalette::Custom, e->mimeData()->colorData().value<QColor>() );
        if ( realPattern()==15 ) emit pushUndoStack( "_pattern", 1 );
        emit endUndoStackMacro();
    } else {
        emit pushUndoStack( PHIPalette::Background, PHIPalette::Custom, e->mimeData()->colorData().value<QColor>() );
    }
    update();
}

void PHIAbstractShapeItem::squeeze()
{
    removeData( DTmpLineStyle );
    removeData( DTmpColor );
    removeData( DTmpOutlineColor);
    removeData( DTmpPenWidth );
    removeData( DTmpPatternStyle );
    if ( realPenWidth()==1. ) removeData( DPenWidth );
    if ( realLine()==0 ) {
        removeData( DPenWidth );
        removeData( DLineStyle );
    }
    if ( realPattern()==1 ) removeData( DPatternStyle );
    if ( realPattern()==0 ) removeData( DColor );
    if ( _colorRole!=PHIPalette::Custom ) removeData( DColor );
    if ( _outlineColorRole!=PHIPalette::Custom ) removeData( DOutlineColor );
}

void PHIAbstractShapeItem::loadItemData( QDataStream &in, int version )
{
    Q_UNUSED( version )
    quint8 cr, bgcr;
    in >> cr >> bgcr;
    _colorRole=static_cast<PHIPalette::ColorRole>(cr);
    _outlineColorRole=static_cast<PHIPalette::ColorRole>(bgcr);
}

void PHIAbstractShapeItem::saveItemData( QDataStream &out, int version )
{
    Q_UNUSED( version )
    out << static_cast<quint8>(_colorRole) << static_cast<quint8>(_outlineColorRole);
}

void PHIAbstractImageItem::ideUpdateData()
{
    if ( _imageData.isTranslated() ) setImage( _imageData.image( page()->currentLang() ) );
    else setImage( _imageData.image() );
    update();
}

void PHIAbstractImageItem::squeeze()
{
    removeData( DImage );
    removeData( DTmpImage );
}

void PHIAbstractImageItem::saveItemData( QDataStream &out, int version )
{
    Q_UNUSED( version )
    if ( _imageData.isTranslated() ) {
        foreach ( QByteArray lang, _imageData.langs() ) {
            QImage img=_imageData.image( lang );
            if ( img.isNull() ) _imageData.remove( lang );
            else {
                if ( img.size()!=realSize().toSize() )
                    img=img.scaled( realSize().toSize(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
                _imageData.setImage( img, lang );
            }
        }
    } else {
        QImage img=_imageData.image();
        if ( img.isNull() ) _imageData.remove( _imageData.c() );
        else {
            foreach( QByteArray key, _imageData.langs() ) {
                _imageData.remove( key );
            }
            if ( img.size()!=realSize().toSize() )
                img=img.scaled( realSize().toSize(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
            _imageData.setImage( img );
        }
    }
    QByteArray arr;
    QDataStream ds( &arr, QIODevice::WriteOnly );
    ds.setVersion( PHI_DSV2 );
    ds << &_imageData;
    out << qCompress( arr, 9 );
}

void PHIAbstractImageItem::loadItemData( QDataStream &in, int version )
{
    Q_UNUSED( version )
    QByteArray arr;
    in >> arr;
    arr=qUncompress( arr );
    QDataStream ds( &arr, QIODevice::ReadOnly );
    ds.setVersion( PHI_DSV2 );
    ds >> &_imageData;
}

QSizeF PHIAbstractImageItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    if ( isChild() ) return realSize();
    if ( which==Qt::MinimumSize ) return QSizeF( 16, 16 );
    if ( which==Qt::PreferredSize ) {
        if ( !realImage().isNull() ) return QSizeF( realImage().size() );
        return QSizeF( 96., 96. );
    }
    return PHIBaseItem::sizeHint( which, constraint );
}

bool PHIAbstractImageItem::paint( QPainter *p, const QRectF &exposed )
{
    Q_UNUSED( exposed )
    if ( realImage().isNull() ) {
        p->setOpacity( .7 );
        p->fillRect( rect(), QBrush( Qt::lightGray ) );
        QFont f=font();
        f.setPointSizeF( PHI::adjustedFontSize( 10. ) );
        p->setFont( f );
        p->setPen( Qt::darkGray );
        if ( isIdeItem() || isTemplateItem() ) p->drawText( rect(), tr( "Image not available" ), QTextOption( Qt::AlignCenter ) );
        else p->drawText( rect(), tr( "Image is loading..." ) );
    } else {
        p->setRenderHint( QPainter::SmoothPixmapTransform );
        p->setRenderHint( QPainter::Antialiasing );
        p->drawImage( rect(), realImage() );
    }
    return true;
}

void PHIAbstractImageItem::phisCreateData( const PHIDataParser &parser )
{
    parser.createImages( &_imageData );
}

void PHIAbstractImageItem::phisParseData( const PHIDataParser &parser )
{
    setImagePath( parser.imagePath( &_imageData ) );
}

void PHIAbstractImageItem::clientInitData()
{
    setData( DIsImage, true );
    QUrl url=page()->baseUrl();
    if ( imagePath().startsWith( '/' ) ) {
        url.setPath( QString::fromUtf8( imagePath() ) );
    } else {
        url.setPath( L1( "/phi.phis" ) );
        QUrlQuery query;
        query.addQueryItem( L1( "i" ), QString::fromUtf8( imagePath() ) );
        query.addQueryItem( L1( "t" ) , L1( "1" ) );
        url.setQuery( query );
    }
    PHIImageRequest *req=new PHIImageRequest( this, url );
    connect( req, &PHIImageRequest::imageReady, this, &PHIAbstractImageItem::slotImageReady );
}

QScriptValue PHIAbstractImageItem::src( const QScriptValue &v )
{
    if ( !isClientItem() ) return scriptEngine()->undefinedValue();
    if ( !v.isValid() ) {
        if ( imagePath().startsWith( '/' ) ) return QString::fromUtf8( imagePath() );
        return L1( "phi.phis?i=" )+QString::fromUtf8( imagePath() )+L1( "&amp;t=1" );
    }
    if ( !v.toString().startsWith( QLatin1Char( '/' ) ) ) {
        emit javaScriptError( QScriptValue( tr( "Image 'src' path must be absolute for '%1'." ).arg( name() ) ) );
        return self();
    }
    setImagePath( v.toString().toUtf8() );
    if ( !isClientItem() ) return self();
    QUrl url=page()->baseUrl();
    url.setPath( v.toString() );
    qDebug() << "src=" << imagePath() << url;
    PHIImageRequest *req=new PHIImageRequest( this, url );
    connect( req, &PHIImageRequest::imageReady, this, &PHIAbstractImageItem::slotImageReady );
    return self();
}

QScriptValue PHIAbstractImageItem::onload( const QScriptValue v )
{
    if ( v.isValid() ) return on( SL( "onload" ), v );
    return trigger( SL( "onload" ) );
}

void PHIAbstractImageItem::slotImageReady( const QImage &image )
{
    trigger( L1( "load" ) );
    trigger( L1( "onload" ) );
    setImage( image );
}

void PHIAbstractImageItem::ideDragEnterEvent( QGraphicsSceneDragDropEvent *e )
{
    QImage img=imageFromMimeData( e->mimeData() );
    if ( img.isNull() ) return e->ignore();
    setData( DTmpImage, data( DImage ) );
    setData( DImage, img );
    update();
}

void PHIAbstractImageItem::ideDragLeaveEvent( QGraphicsSceneDragDropEvent *e )
{
    Q_UNUSED( e )
    setData( DImage, data( DTmpImage ) );
    update();
}

void PHIAbstractImageItem::ideDropEvent( QGraphicsSceneDragDropEvent *e )
{
    QImage img=imageFromMimeData( e->mimeData() );
    setData( DImage, data( DTmpImage ) );
    emit pushUndoStack( img );
}

void PHIAbstractImageBookItem::ideUpdateData()
{
    QVariant v;
    if ( _imageBookData.isTranslated() ) v.setValue( _imageBookData.imageBook( page()->currentLang() ) );
    else v.setValue( _imageBookData.imageBook() );
    setData( DImages, v );
}

bool PHIAbstractImageBookItem::paint( QPainter *p, const QRectF &exposed )
{
    Q_UNUSED( exposed )
    if ( !realImages().count() ) {
        p->setOpacity( .7 );
        p->fillRect( rect(), QBrush( Qt::lightGray ) );
        QFont f=font();
        f.setPointSizeF( PHI::adjustedFontSize( 10. ) );
        p->setFont( f );
        p->setPen( Qt::darkGray );
        if ( !isClientItem() ) p->drawText( rect(), tr( "Image not available" ), QTextOption( Qt::AlignCenter ) );
        else p->drawText( rect(), tr( "Image is loading..." ) );
    }
    return true;
}

void PHIAbstractImageBookItem::phisCreateData( const PHIDataParser &parser )
{
    parser.createImages( &_imageBookData );
}

void PHIAbstractImageBookItem::phisParseData( const PHIDataParser &parser )
{
    setImagePathes( parser.imagePathes( &_imageBookData ) );
}

void PHIAbstractImageBookItem::clientInitData()
{
    setData( DIsImage, true );
    for ( int i=0; i<imagePathes().count(); i++ ) {
        QByteArray path=imagePathes().value( i );
        QUrl url=page()->baseUrl();
        if ( path.startsWith( '/' ) ) {
            url.setPath( QString::fromUtf8( path ) );
        } else {
            url.setPath( L1( "/phi.phis" ) );
            QUrlQuery query;
            query.addQueryItem( L1( "i" ), QString::fromUtf8( path ) );
            query.addQueryItem( L1( "t" ) , L1( "1" ) );
            url.setQuery( query );
        }
        qDebug() << "init data" << url << i;
        PHIImageRequest *req=new PHIImageRequest( this, url, i );
        connect( req, &PHIImageRequest::imageReady, this, &PHIAbstractImageBookItem::slotImageReady );
    }
}

void PHIAbstractImageBookItem::slotImageReady( const QImage &image, int num )
{
    PHIImageHash hash=realImages();
    hash.insert( QByteArray::number( num ), image );
    setImages( hash );
}

void PHIAbstractImageBookItem::squeeze()
{
    removeData( DImages );
    removeData( DTmpImages );
}

void PHIAbstractImageBookItem::loadItemData( QDataStream &in, int version )
{
    Q_UNUSED( version )
    QByteArray arr;
    in >> arr;
    arr=qUncompress( arr );
    QDataStream ds( &arr, QIODevice::ReadOnly );
    ds.setVersion( PHI_DSV2 );
    ds >> &_imageBookData;
}

void PHIAbstractImageBookItem::saveItemData( QDataStream &out, int version )
{
    Q_UNUSED( version )
    QByteArray arr;
    QDataStream ds( &arr, QIODevice::WriteOnly );
    ds.setVersion( PHI_DSV2 );
    PHIByteArrayList langs;
    if ( _imageBookData.isUnparsedStatic() ) langs << PHIData::c();
    else if ( _imageBookData.isUnparsedTranslated() ) langs=_imageBookData.langs();
    foreach ( QByteArray l, langs ) {
        PHIImageHash hash=_imageBookData.imageBook( l );
        for ( int i=0; i<hash.count(); i++ ) {
            QImage img=hash.value( QByteArray::number( i ) );
            img=img.scaled( realSize().toSize(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
            hash.insert( QByteArray::number( i ), img );
        }
        _imageBookData.setImageBook( hash, l );
    }
    ds << &_imageBookData;
    out << qCompress( arr, 9 );
}

void PHIAbstractImageBookItem::ideDragEnterEvent( QGraphicsSceneDragDropEvent *e )
{
    e->ignore();
}

void PHIAbstractImageBookItem::ideDragLeaveEvent( QGraphicsSceneDragDropEvent *e )
{
    Q_UNUSED( e )
}

void PHIAbstractImageBookItem::ideDropEvent( QGraphicsSceneDragDropEvent *e )
{
    Q_UNUSED( e )
}

QSizeF PHIAbstractImageBookItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    if ( isChild() ) return realSize();
    if ( which==Qt::MinimumSize ) return QSizeF( 16, 16 );
    if ( which==Qt::PreferredSize ) {
        if ( !realImages().isEmpty() ) return QSizeF( realImages().values().first().size() );
        return QSizeF( 96., 96. );
    }
    return PHIBaseItem::sizeHint( which, constraint );
}

void PHIAbstractLayoutItem::initLayout()
{
    _l=new QGraphicsGridLayout();
    _l->setContentsMargins( leftMargin(), topMargin(), rightMargin(), bottomMargin() );
    gw()->setLayout( _l );
    setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    connect( this, &PHIAbstractLayoutItem::layoutChanged, this, &PHIAbstractLayoutItem::updateLayoutGeometry, Qt::QueuedConnection );
    setColor( PHIPalette::Foreground, PHIPalette::Window, QColor( Qt::transparent ) );
    if ( page() ) setColor( PHIPalette::Background, PHIPalette::Button, page()->phiPalette().color( PHIPalette::Button ) );
    else setColor( PHIPalette::Background, PHIPalette::Button, qApp->palette().color( QPalette::Button ) );
}

void PHIAbstractLayoutItem::ideInit()
{
    // transparent content:
    setColor( PHIPalette::Foreground, PHIPalette::Window, page()->phiPalette().color( PHIPalette::Window ) );
    // button background color border:
    setColor( PHIPalette::Background, PHIPalette::Button, page()->phiPalette().color( PHIPalette::Button ) );
    setLine( 1 ); // solid line
    setPattern( 1 ); // solid pattern
}

void PHIAbstractLayoutItem::setAlignment( quint16 align )
{
    setData( DAlignment, align );
    foreach ( PHIBaseItem *it, _children ) {
        if ( it->property( "_align" ).isValid() ) {
            it->setProperty( "_align", align );
            break;
        }
    }
    update();
}

void PHIAbstractLayoutItem::ideUpdateData()
{
    PHIAbstractShapeItem::ideUpdateData();
    if ( !_l ) return;
    _l->setContentsMargins( leftMargin(), topMargin(), rightMargin(), bottomMargin() );
    _l->setHorizontalSpacing( horizontalSpacing() );
    _l->setVerticalSpacing( verticalSpacing() );
    invalidateLayout();
}

void PHIAbstractLayoutItem::invalidateLayout()
{
    if ( !_l ) return;
    _l->invalidate();
    if ( isClientItem() ) return;
    emit layoutChanged(); // queued connection to updateLayoutGeometry()
    foreach ( PHIBaseItem *it, childItems() ) {
        PHIGraphicsItem *git=qgraphicsitem_cast<PHIGraphicsItem*>(it->gw());
        Q_ASSERT( git );
        git->updateGeometry();
        PHIAbstractLayoutItem *lit=qobject_cast<PHIAbstractLayoutItem*>(it);
        if ( lit ) lit->invalidateLayout();
    }
}

void PHIAbstractLayoutItem::updateLayoutGeometry()
{
    foreach ( PHIBaseItem *it, childItems() ) {
        if ( isIdeItem() || isTemplateItem() ) {
            it->setPos( it->gw()->pos() );
            it->resize( it->gw()->size() );
        } else {
            it->gw()->setPos( it->realPos() );
            it->gw()->resize( it->realSize() );
            qDebug() << it->id() << it->realSize();
        }
        it->update();
    }
    // hack to adjust layout size:
    // QSizeF oldSize=size();
    QSizeF s=realSize();
    if ( !isClientItem() ) {
        if ( gw()->minimumWidth()>s.width() ) s.setWidth( gw()->minimumWidth() );
        if ( gw()->minimumHeight()>s.height() ) s.setHeight( gw()->minimumHeight() );
        if ( gw()->maximumWidth()<s.width() ) s.setWidth( gw()->maximumWidth() );
        if ( gw()->maximumHeight()<s.height() ) s.setHeight( gw()->maximumHeight() );
        resize( 1, 1 );
    }
    resize( s );
    //if ( s!=oldSize ) emit pushUndoStack( oldSize );
    update();
}

bool PHIAbstractLayoutItem::isWidthChangeable() const
{
    if ( !_l ) return true;
    if ( _l->minimumWidth()==_l->maximumWidth() ) return false;
    return true;
}

bool PHIAbstractLayoutItem::isHeightChangeable() const
{
    if ( !_l ) return true;
    if ( _l->minimumHeight()==_l->maximumHeight() ) return false;
    return true;
}

void PHIAbstractLayoutItem::setFont( const QFont &font )
{
    foreach ( PHIBaseItem *it, _children ) {
        it->setFont( font );
    }
    PHIBaseItem::setFont( font );
}

void PHIAbstractLayoutItem::squeeze()
{
    PHIAbstractShapeItem::squeeze();
    if ( topLeftRadius()==0 ) removeData( DRadiusTopLeft );
    if ( topRightRadius()==0 ) removeData( DRadiusTopRight );
    if ( bottomLeftRadius()==0 ) removeData( DRadiusBottomLeft );
    if ( bottomRightRadius()==0 ) removeData( DRadiusBottomRight );
    if ( leftMargin()==6 ) removeData( DMarginLeft );
    if ( topMargin()==6 ) removeData( DMarginTop );
    if ( rightMargin()==6 ) removeData( DMarginRight );
    if ( horizontalSpacing()==6 ) removeData( DHorizontalSpacing );
    if ( verticalSpacing()==6 ) removeData( DVerticalSpacing );
    if ( bottomMargin()==6 ) removeData( DMarginBottom );
    if ( realAlignment()==static_cast<quint16>(Qt::AlignLeft|Qt::AlignVCenter) ) removeData( DAlignment );
    if ( realLine()==0 && realPattern()==0 ) {
        removeData( DRadiusTopLeft );
        removeData( DRadiusTopRight );
        removeData( DRadiusBottomLeft );
        removeData( DRadiusBottomRight );
    }
}

void PHIAbstractLayoutItem::phisCreateData( const PHIDataParser &parser )
{
    PHIAbstractShapeItem::phisCreateData( parser );
    setData( DHeader, parser.text( &_textData ) );
    if ( !_textData.isUnparsedStatic() ) setDirtyFlag( DFHeader );
}

void PHIAbstractLayoutItem::phisParseData( const PHIDataParser &parser )
{
    PHIAbstractShapeItem::phisParseData( parser );
    if ( dirtyFlags() & DFHeader ) setData( DHeader, parser.text( &_textData ) );
}

void PHIAbstractLayoutItem::clientInitData()
{
    PHIAbstractShapeItem::clientInitData();
    setHeader( realHeader() );
    Q_ASSERT( _l );
    _l->setContentsMargins( leftMargin(), topMargin(), rightMargin(), bottomMargin() );
    _l->setHorizontalSpacing( horizontalSpacing() );
    _l->setVerticalSpacing( verticalSpacing() );
    invalidateLayout();
    update();
}

void PHIAbstractLayoutItem::cssGraphicEffect( const PHIRequest *req, QByteArray &out, QByteArray &script ) const
{
    PHIBaseItem::cssGraphicEffect( req, out, script );
}

void PHIAbstractLayoutItem::loadItemData( QDataStream &in, int version )
{
    PHIAbstractShapeItem::loadItemData( in, version );
    if ( flags() & FLayoutHeader ) in >> &_textData;
}

void PHIAbstractLayoutItem::saveItemData( QDataStream &out, int version )
{
    PHIAbstractShapeItem::saveItemData( out, version );
    if ( flags() & FLayoutHeader ) out << &_textData;
}

QScriptValue PHIAbstractLayoutItem::textAlign( const QScriptValue &a )
{
    if ( !a.isValid() ) return PHI::qtAlignmentToTextAlign( static_cast<Qt::Alignment>(realAlignment()) );
    setAlignment( PHI::textAlignToQtAlignment( a.toString() ) );
    return self();
}

void PHIAbstractLayoutItem::insertBaseItem( PHIBaseItem *it, int row, int column, int rowSpan, int columnSpan )
{
    _children.append( it );
    if ( !_l ) return; // server
    setChildItem( it );
    _l->addItem( it->gw(), row, column, rowSpan, columnSpan );
    //_l->invalidate();
    update();
}

void PHIAbstractLayoutItem::setChildItem( PHIBaseItem *it )
{
    Q_ASSERT( gw() );
    it->setParentId( id() );
    it->setFlag( PHIBaseItem::FChild, true );
    if ( isClientItem() ) return;
    it->gw()->setFlag( QGraphicsItem::ItemIsMovable, false );
    it->gw()->setFlag( QGraphicsItem::ItemIsSelectable, false );
    it->gw()->setFlag( QGraphicsItem::ItemIsFocusable, false );
    it->gw()->setAcceptHoverEvents( false );
}

void PHIAbstractLayoutItem::releaseItem( PHIBaseItem *it )
{
    Q_ASSERT( gw() );
    it->setFlag( PHIBaseItem::FChild, false );
    it->setParentId( QByteArray() );
    it->gw()->setParentItem( 0 );
    if ( isClientItem() ) return;
    it->gw()->setFlag( QGraphicsItem::ItemIsMovable, true );
    it->gw()->setFlag( QGraphicsItem::ItemIsSelectable, true );
    it->gw()->setFlag( QGraphicsItem::ItemIsFocusable, true );
    it->gw()->setAcceptHoverEvents( true );
}

void PHIAbstractLayoutItem::drawShape( QPainter *p, const QRectF &exposed )
{
    Q_UNUSED( exposed )
    if ( p->pen().style()!=Qt::NoPen || p->brush().style()!=Qt::NoBrush ) {
        qreal rtl=data( DRadiusTopLeft, 0 ).toReal();
        qreal rtr=data( DRadiusTopRight, 0 ).toReal();
        qreal rbr=data( DRadiusBottomRight, 0 ).toReal();
        qreal rbl=data( DRadiusBottomLeft, 0 ).toReal();
        QRectF cr=rect();
        if ( realLine()>0 ) { // border
            qreal off=realPenWidth()/2.;
            cr=QRectF( -off, -off, realWidth()+off, realHeight()+off );
        }
        QPainterPath path;
        path.moveTo( cr.x(), cr.y()+rtl );
        path.arcTo( cr.x(), cr.y(), rtl*2, rtl*2, 180., -90. );
        path.lineTo( cr.width()-rtr, cr.y() );
        path.arcTo( cr.width()-rtr*2, cr.y(), rtr*2, rtr*2, 90., -90. );
        path.lineTo( cr.width(), cr.height()-rbr );
        path.arcTo( cr.width()-rbr*2, cr.height()-rbr*2, rbr*2, rbr*2, 0, -90. );
        path.lineTo( cr.x()+rbl, cr.height() );
        path.arcTo( cr.x(), cr.height()-rbl*2, rbl*2, rbl*2, -90., -90. );
        path.lineTo( cr.x(), cr.y()+rtl );
        p->setRenderHint( QPainter::Antialiasing, hasTransformation() );
        p->drawPath( path );
    }
    if ( !isIdeItem() || flags() & FIDEHideChildRects ) return;
    p->setBrush( Qt::NoBrush );
    QPen pen( Qt::blue );
    pen.setWidth( 1 );
    pen.setStyle( Qt::DotLine );
    p->setPen( pen );
    foreach ( PHIBaseItem *it, _children ) {
        p->drawRect( QRectF( it->realPos(), QSizeF( it->realWidth()-1, it->realHeight()-1 ) ) );
    }
}

bool PHIAbstractLayoutItem::paint( QPainter *p, const QRectF &exposed )
{
    PHIAbstractShapeItem::paint( p, exposed ); // draw background
    return false;
}

QSizeF PHIAbstractLayoutItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    if ( !_l ) return PHIAbstractShapeItem::sizeHint( which, constraint );
    switch( which ) {
    case Qt::PreferredSize: return _l->preferredSize();
    case Qt::MinimumSize: return _l->minimumSize();
    case Qt::MaximumSize: return _l->maximumSize();
    default:;
    }
    return QSizeF();
}

void PHIAbstractLayoutItem::breakLayout()
{
    Q_ASSERT( _gw );
    foreach ( PHIBaseItem *it, _children ) {
        _l->removeItem( it->gw() );
        releaseItem( it );
    }
    _children.clear();
    invalidateLayout();
}

void PHIAbstractLayoutItem::ideSetText( const QString &t, const QByteArray &lang )
{
    if ( _textData.isTranslated() ) {
        _textData.setText( t, lang );
    } else if ( _textData.isStatic() ) {
        _textData.setSource( PHIData::Translated );
        _textData.setText( t, lang );
    }
    if ( isChild() ) {
        PHIBaseItem *it=page()->findItem( parentId() );
        PHIAbstractLayoutItem *lit=qobject_cast<PHIAbstractLayoutItem*>(it);
        if ( lit ) lit->invalidateLayout();
    }
}

QString PHIAbstractLayoutItem::ideText( const QByteArray &lang ) const
{
    if ( _textData.isTranslated() ) return _textData.text( lang );
    return _textData.text();
}

QRectF PHIAbstractLayoutItem::boundingRect() const
{
    return PHIAbstractShapeItem::boundingRect();
}

PHIConfigWidget* PHIAbstractLayoutItem::ideConfigWidget()
{
    return new PHILayoutConfig( this );
}

QSizeF PHIAbstractInputItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    if ( which!=Qt::PreferredSize ) return PHIAbstractTextItem::sizeHint( which, constraint );
    return QSizeF( 128., PHIAbstractTextItem::sizeHint( which, constraint ).height() );
}

void PHIAbstractInputItem::squeeze()
{
    if ( data( DAccessKey ).toString().isEmpty() ) removeData( DAccessKey );
    PHIAbstractTextItem::squeeze();
}

void PHIAbstractInputItem::loadItemData( QDataStream &in, int version )
{
    PHIAbstractTextItem::loadItemData( in, version );
    in >> &_readOnlyData;
}

void PHIAbstractInputItem::saveItemData( QDataStream &out, int version )
{
    PHIAbstractTextItem::saveItemData( out, version );
    out << &_readOnlyData;
}

void PHIAbstractInputItem::ideUpdateData()
{
    PHIAbstractTextItem::ideUpdateData();
    if ( _readOnlyData.isUnparsedStatic() ) setReadOnly( _readOnlyData.boolean() );
    else if ( _readOnlyData.isUnparsedTranslated() ) setReadOnly( _readOnlyData.boolean( page()->currentLang() ) );
    else setReadOnly( false );
}

void PHIAbstractInputItem::phisCreateData( const PHIDataParser &parser )
{
    PHIAbstractTextItem::phisCreateData( parser );
    setReadOnly( parser.text( &_readOnlyData ).toBool() );
    if ( !_readOnlyData.isUnparsedStatic() ) setDirtyFlag( DFReadOnlyData );
}

void PHIAbstractInputItem::phisParseData( const PHIDataParser &parser )
{
    PHIAbstractTextItem::phisParseData( parser );
    if ( dirtyFlags() & DFReadOnlyData ) setReadOnly( parser.text( &_readOnlyData ).toBool() );
}

void PHIAbstractInputItem::clientInitData()
{
    setReadOnly( flags() & FReadOnly );
    setAccessKey( realAccessKey() );
    PHIAbstractTextItem::clientInitData();
}

void PHIAbstractInputItem::reset()
{
    PHIAbstractTextItem::reset();
    setValue( realText() );
}

QScriptValue PHIAbstractInputItem::readOnly( const QScriptValue &r )
{
    if ( !r.isValid() ) return realReadOnly();
    setReadOnly( r.toBool() );
    return self();
}

QScriptValue PHIAbstractInputItem::val( const QScriptValue &v )
{
    if ( !v.isValid() ) return realValue();
    setValue( v.toString() );
    return self();
}

QScriptValue PHIAbstractInputItem::accessKey( const QScriptValue &a )
{
    if ( !a.isValid() ) return realAccessKey();
    setAccessKey( a.toString() );
    return self();
}

void PHIAbstractInputItem::clientPostData( QHttpMultiPart *multiPart ) const
{
    QHttpPart hp;
    hp.setHeader( QNetworkRequest::ContentTypeHeader, BL( "text/plain" ) );
    hp.setHeader( QNetworkRequest::ContentDispositionHeader, BL( "form-data; name=\"" )+id()+BL( "\"" ) );
    hp.setBody( realValue().toUtf8() );
    multiPart->append( hp );
}

#ifdef PHIWEBKIT
#include <QWebView>
#endif

void PHIAbstractExternalItem::ideInit()
{
    setColor( PHIPalette::WidgetText, PHIPalette::Text, page()->phiPalette().color( PHIPalette::Text ) );
    setColor( PHIPalette::WidgetBase, PHIPalette::Window, page()->phiPalette().color( PHIPalette::Window ) );
}

QSizeF PHIAbstractExternalItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    if ( which==Qt::PreferredSize ) return QSizeF( 300., 200. );
    return PHIBaseItem::sizeHint( which, constraint );
}

bool PHIAbstractExternalItem::paint( QPainter *painter, const QRectF &exposed )
{
    Q_UNUSED( painter )
    Q_UNUSED( exposed )
    return false;
}

void PHIAbstractExternalItem::phisCreateData( const PHIDataParser &parser )
{
    setData( DText, escapeChars( parser.text( textData() ).toByteArray() ) );
    if ( !textData()->isUnparsedStatic() ) setDirtyFlag( DFText );
}

void PHIAbstractExternalItem::phisParseData( const PHIDataParser &parser )
{
    if ( dirtyFlags() & DFText ) {
        setData( DText, escapeChars( parser.text( textData() ).toByteArray() ) );
    }
}

QByteArray PHIAbstractExternalItem::escapeChars( const QByteArray &a ) const
{
    QByteArray arr=a;
    arr.replace( '"', BL( "&quot;" ) );
    arr.replace( '&', BL( "&amp;" ) );
    return arr;
}

QScriptValue PHIAbstractExternalItem::accessKey( const QScriptValue &a )
{
    if ( !a.isValid() ) return realAccessKey();
    setAccessKey( a.toString() );
    return self();
}

void PHIAbstractExternalItem::initWidget()
{
    connect( this, &PHIBaseItem::sizeChanged, this, &PHIAbstractExternalItem::slotSizeChanged );
#ifdef PHIWEBKIT
    QWebView *view=new QWebView();
    _webPage=new PHIWebPage( this );
    view->setPage( _webPage );
    QPalette pal=view->palette();
    pal.setBrush( QPalette::Base, Qt::transparent ) ;
    view->setPalette( pal );
    view->setAttribute( Qt::WA_OpaquePaintEvent, false );
    view->setAttribute( Qt::WA_TranslucentBackground );
    setWidget( view );
#endif
    setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding, QSizePolicy::Frame ) );
}

void PHIAbstractExternalItem::setWidgetText( const QString &t )
{
    if ( !_webPage ) return;
    QUrl url( QString::fromLatin1( source() )+t );
#ifdef PHIWEBKIT
    if ( url==_webPage->url() ) return;
    _webPage->setContent( QByteArray() );
    _webPage->setUrl( url);
#endif
}

QByteArray PHIAbstractExternalItem::mapLanguage( const QByteArray &lang ) const
{
    if ( lang=="C" ) return BL( "en" );
    return lang.left( 2 );
}

void PHIAbstractExternalItem::slotSizeChanged( const QSizeF &size )
{
    Q_UNUSED( size );
    setWidgetText( realText() ); // updated content
}
