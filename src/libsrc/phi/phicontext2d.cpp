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
#
#    Note: The provided classes are based on the Qt example for Context2D.
*/
#include "phicontext2d.h"
#include <QVariant>
#include <math.h>
static const double PHI_PI=3.14159265358979323846;

#define DEGREES(t) ((t) * 180.0 / PHI_PI )
#define PHIBOUND(val, min, max) qMin(qMax(val, min), max)

static QList<qreal> parseNumbersList( QString::const_iterator &itr )
{
    QList<qreal> points;
    QString temp;
    while ( (*itr).isSpace() ) ++itr;
    while ( (*itr).isNumber() || (*itr) == '-' || (*itr) == '+' || (*itr) == '.' ) {
        temp=QString();
        if ( (*itr)=='-' ) temp+=*itr++;
        else if ( (*itr)=='+' ) temp+=*itr++;
        while ( (*itr).isDigit() ) temp+=*itr++;
        if ( (*itr)=='.' ) temp+=*itr++;
        while ( (*itr).isDigit() ) temp+=*itr++;
        while ( (*itr).isSpace() ) ++itr;
        if ( (*itr)==',' ) ++itr;
        points.append( temp.toDouble() );
        //eat spaces
        while ( (*itr).isSpace() ) ++itr;
    }
    return points;
}

QColor colorFromString( const QString &name )
{
    QString::const_iterator itr=name.constBegin();
    QList<qreal> compo;
    if ( name.startsWith( QStringLiteral( "rgba(" ) ) ) {
        ++itr; ++itr; ++itr; ++itr; ++itr;
        compo=parseNumbersList( itr );
        if ( compo.size()!=4 ) {
            return QColor();
        }
        //alpha seems to be always between 0-1
        compo[3]*=255;
        return QColor( (int)compo[0], (int)compo[1], (int)compo[2], (int)compo[3]);
    } else if ( name.startsWith( QStringLiteral( "rgb(" ) ) ) {
        ++itr; ++itr; ++itr; ++itr;
        compo=parseNumbersList( itr );
        if (compo.size() != 3) {
            return QColor();
        }
        return QColor( (int)PHIBOUND(compo[0], qreal(0), qreal(255)),
            (int)PHIBOUND(compo[1], qreal(0), qreal(255)),
            (int)PHIBOUND(compo[2], qreal(0), qreal(255)));
    } else {
        //QRgb color;
        //CSSParser::parseColor(name, color);
        return QColor(name);
    }
}

static QPainter::CompositionMode compositeOperatorFromString( const QString &compositeOperator )
{
    if ( compositeOperator=="source-over" ) {
        return QPainter::CompositionMode_SourceOver;
    } else if ( compositeOperator=="source-out" ) {
        return QPainter::CompositionMode_SourceOut;
    } else if ( compositeOperator=="source-in" ) {
        return QPainter::CompositionMode_SourceIn;
    } else if ( compositeOperator=="source-atop" ) {
        return QPainter::CompositionMode_SourceAtop;
    } else if ( compositeOperator=="destination-atop" ) {
        return QPainter::CompositionMode_DestinationAtop;
    } else if ( compositeOperator=="destination-in" ) {
        return QPainter::CompositionMode_DestinationIn;
    } else if ( compositeOperator=="destination-out" ) {
        return QPainter::CompositionMode_DestinationOut;
    } else if ( compositeOperator=="destination-over" ) {
        return QPainter::CompositionMode_DestinationOver;
    } else if ( compositeOperator=="darker" ) {
        return QPainter::CompositionMode_SourceOver;
    } else if ( compositeOperator=="lighter" ) {
        return QPainter::CompositionMode_SourceOver;
    } else if ( compositeOperator=="copy" ) {
        return QPainter::CompositionMode_Source;
    } else if ( compositeOperator=="xor" ) {
        return QPainter::CompositionMode_Xor;
    }
    return QPainter::CompositionMode_SourceOver;
}

static QString compositeOperatorToString( QPainter::CompositionMode op )
{
    switch (op) {
    case QPainter::CompositionMode_SourceOver:
        return "source-over";
    case QPainter::CompositionMode_DestinationOver:
        return "destination-over";
    case QPainter::CompositionMode_Clear:
        return "clear";
    case QPainter::CompositionMode_Source:
        return "source";
    case QPainter::CompositionMode_Destination:
        return "destination";
    case QPainter::CompositionMode_SourceIn:
        return "source-in";
    case QPainter::CompositionMode_DestinationIn:
        return "destination-in";
    case QPainter::CompositionMode_SourceOut:
        return "source-out";
    case QPainter::CompositionMode_DestinationOut:
        return "destination-out";
    case QPainter::CompositionMode_SourceAtop:
        return "source-atop";
    case QPainter::CompositionMode_DestinationAtop:
        return "destination-atop";
    case QPainter::CompositionMode_Xor:
        return "xor";
    case QPainter::CompositionMode_Plus:
        return "plus";
    case QPainter::CompositionMode_Multiply:
        return "multiply";
    case QPainter::CompositionMode_Screen:
        return "screen";
    case QPainter::CompositionMode_Overlay:
        return "overlay";
    case QPainter::CompositionMode_Darken:
        return "darken";
    case QPainter::CompositionMode_Lighten:
        return "lighten";
    case QPainter::CompositionMode_ColorDodge:
        return "color-dodge";
    case QPainter::CompositionMode_ColorBurn:
        return "color-burn";
    case QPainter::CompositionMode_HardLight:
        return "hard-light";
    case QPainter::CompositionMode_SoftLight:
        return "soft-light";
    case QPainter::CompositionMode_Difference:
        return "difference";
    case QPainter::CompositionMode_Exclusion:
        return "exclusion";
    default:
        break;
    }
    return QString();
}

void PHIContext2D::save()
{
    _stateStack.push( _state );
}

void PHIContext2D::restore()
{
    if ( !_stateStack.isEmpty() ) {
        _state=_stateStack.pop();
        _state.flags=AllIsFullOfDirt;
    }
}

void PHIContext2D::scale( qreal x, qreal y )
{
    _state.matrix.scale( x, y );
    _state.flags |= DirtyTransformationMatrix;
}

void PHIContext2D::rotate( qreal angle )
{
    _state.matrix.rotate( DEGREES(angle) );
    _state.flags |= DirtyTransformationMatrix;
}

void PHIContext2D::translate( qreal x, qreal y )
{
    _state.matrix.translate( x, y );
    _state.flags |= DirtyTransformationMatrix;
}

void PHIContext2D::transform( qreal m11, qreal m12, qreal m21, qreal m22, qreal dx, qreal dy )
{
    QMatrix mat( m11, m12, m21, m22, dx, dy );
    _state.matrix*=mat;
    _state.flags |= DirtyTransformationMatrix;
}

void PHIContext2D::setTransform( qreal m11, qreal m12, qreal m21, qreal m22, qreal dx, qreal dy )
{
    QMatrix mat( m11, m12, m21, m22, dx, dy );
    _state.matrix=mat;
    _state.flags |= DirtyTransformationMatrix;
}

QString PHIContext2D::globalCompositeOperation() const
{
    return compositeOperatorToString( _state.globalCompositeOperation );
}

void PHIContext2D::setGlobalCompositeOperation( const QString &op )
{
    QPainter::CompositionMode mode=compositeOperatorFromString( op );
    _state.globalCompositeOperation=mode;
    _state.flags |= DirtyGlobalCompositeOperation;
}

QVariant PHIContext2D::strokeStyle() const
{
    return _state.strokeStyle;
}

void PHIContext2D::setStrokeStyle( const QVariant &style )
{
    if ( style.canConvert<PHICanvasGradient>() ) {
        PHICanvasGradient cg=qvariant_cast<PHICanvasGradient>(style);
        _state.strokeStyle=cg.value;
    } else {
        QColor color=colorFromString( style.toString() );
        _state.strokeStyle=color;
    }
    _state.flags |= DirtyStrokeStyle;
}

QVariant PHIContext2D::fillStyle() const
{
    return _state.fillStyle;
}

void PHIContext2D::setFillStyle( const QVariant &style )
{
    if (style.canConvert<PHICanvasGradient>()) {
        PHICanvasGradient cg=qvariant_cast<PHICanvasGradient>(style);
        _state.fillStyle=cg.value;
    } else {
        QColor color=colorFromString( style.toString() );
        _state.fillStyle=color;
    }
    _state.flags |= DirtyFillStyle;
}

qreal PHIContext2D::globalAlpha() const
{
    return _state.globalAlpha;
}

void PHIContext2D::setGlobalAlpha( qreal alpha )
{
    _state.globalAlpha=alpha;
    _state.flags |= DirtyGlobalAlpha;
}

PHICanvasGradient PHIContext2D::createLinearGradient( qreal x0, qreal y0, qreal x1, qreal y1 )
{
    QLinearGradient g( x0, y0, x1, y1 );
    return PHICanvasGradient( g );
}

PHICanvasGradient PHIContext2D::createRadialGradient( qreal x0, qreal y0, qreal r0, qreal x1, qreal y1, qreal r1 )
{
    QRadialGradient g( QPointF( x1, y1 ), r0+r1, QPointF( x0, y0 ) );
    return PHICanvasGradient( g );
}

qreal PHIContext2D::lineWidth() const
{
    return _state.lineWidth;
}

void PHIContext2D::setLineWidth( qreal w )
{
    _state.lineWidth=w;
    _state.flags |= DirtyLineWidth;
}

QString PHIContext2D::lineCap() const
{
    switch (_state.lineCap) {
    case Qt::FlatCap:
        return "butt";
    case Qt::SquareCap:
        return "square";
    case Qt::RoundCap:
        return "round";
    default:;
    }
    return QString();
}

void PHIContext2D::setLineCap( const QString &capString )
{
    Qt::PenCapStyle style;
    if ( capString=="round" ) style=Qt::RoundCap;
    else if ( capString=="square" ) style=Qt::SquareCap;
    else style=Qt::FlatCap;
    _state.lineCap=style;
    _state.flags |= DirtyLineCap;
}

QString PHIContext2D::lineJoin() const
{
    switch ( _state.lineJoin ) {
    case Qt::RoundJoin: return "round";
    case Qt::BevelJoin: return "bevel";
    case Qt::MiterJoin: return "miter";
    default:;
    }
    return QString();
}

void PHIContext2D::setLineJoin( const QString &joinString )
{
    Qt::PenJoinStyle style;
    if ( joinString=="round" ) style=Qt::RoundJoin;
    else if ( joinString=="bevel" ) style=Qt::BevelJoin;
    else style=Qt::MiterJoin;
    _state.lineJoin=style;
    _state.flags |= DirtyLineJoin;
}

qreal PHIContext2D::miterLimit() const
{
    return _state.miterLimit;
}

void PHIContext2D::setMiterLimit( qreal m )
{
    _state.miterLimit=m;
    _state.flags |= DirtyMiterLimit;
}

void PHIContext2D::setShadowOffsetX( qreal x )
{
    _state.shadowOffsetX = x;
    _state.flags |= DirtyShadowOffsetX;
}

void PHIContext2D::setShadowOffsetY( qreal y )
{
    _state.shadowOffsetY = y;
    _state.flags |= DirtyShadowOffsetY;
}

void PHIContext2D::setShadowBlur( qreal b )
{
    _state.shadowBlur = b;
    _state.flags |= DirtyShadowBlur;
}

void PHIContext2D::setShadowColor( const QString &str )
{
    _state.shadowColor=colorFromString( str );
    _state.flags |= DirtyShadowColor;
}

qreal PHIContext2D::shadowOffsetX() const
{
    return _state.shadowOffsetX;
}

qreal PHIContext2D::shadowOffsetY() const
{
    return _state.shadowOffsetY;
}

qreal PHIContext2D::shadowBlur() const
{
    return _state.shadowBlur;
}

QString PHIContext2D::shadowColor() const
{
    return _state.shadowColor.name();
}

void PHIContext2D::clearRect( qreal x, qreal y, qreal w, qreal h )
{
    beginPainting();
    _painter.save();
    _painter.setMatrix( _state.matrix, false );
    _painter.setCompositionMode( QPainter::CompositionMode_Source );
    _painter.fillRect( QRectF( x, y, w, h ), QColor( 0, 0, 0, 0 ) );
    _painter.restore();
    scheduleChange();
}

void PHIContext2D::fillRect(qreal x, qreal y, qreal w, qreal h)
{
    beginPainting();
    _painter.save();
    _painter.setMatrix( _state.matrix, false );
    _painter.fillRect( QRectF( x, y, w, h ), _painter.brush() );
    _painter.restore();
    scheduleChange();
}

void PHIContext2D::strokeRect( qreal x, qreal y, qreal w, qreal h )
{
    QPainterPath path;
    path.addRect( x, y, w, h );
    beginPainting();
    _painter.save();
    _painter.setMatrix( _state.matrix, false );
    _painter.strokePath( path, _painter.pen() );
    _painter.restore();
    scheduleChange();
}

void PHIContext2D::beginPath()
{
    _path=QPainterPath();
}

void PHIContext2D::closePath()
{
    _path.closeSubpath();
}

void PHIContext2D::moveTo( qreal x, qreal y )
{
    QPointF pt=_state.matrix.map( QPointF(x, y) );
    _path.moveTo( pt );
}

void PHIContext2D::lineTo( qreal x, qreal y )
{
    QPointF pt=_state.matrix.map( QPointF( x, y ) );
    _path.lineTo( pt );
}

void PHIContext2D::quadraticCurveTo( qreal cpx, qreal cpy, qreal x, qreal y )
{
    QPointF cp=_state.matrix.map( QPointF( cpx, cpy ) );
    QPointF xy=_state.matrix.map( QPointF( x, y ) );
    _path.quadTo( cp, xy );
}

void PHIContext2D::bezierCurveTo( qreal cp1x, qreal cp1y, qreal cp2x, qreal cp2y, qreal x, qreal y )
{
    QPointF cp1=_state.matrix.map( QPointF( cp1x, cp1y ) );
    QPointF cp2=_state.matrix.map( QPointF( cp2x, cp2y ) );
    QPointF end=_state.matrix.map( QPointF( x, y ) );
    _path.cubicTo( cp1, cp2, end );
}

void PHIContext2D::arcTo( qreal x1, qreal y1, qreal x2, qreal y2, qreal radius )
{
    //FIXME: this is surely busted
    QPointF st=_state.matrix.map( QPointF( x1, y1 ) );
    QPointF end=_state.matrix.map( QPointF( x2, y2 ) );
    _path.arcTo( st.x(), st.y(), end.x()-st.x(), end.y()-st.y(), radius, 90 );
}

void PHIContext2D::rect( qreal x, qreal y, qreal w, qreal h )
{
    QPainterPath path;
    path.addRect( x, y, w, h );
    path=_state.matrix.map( path );
    _path.addPath( path );
}

void PHIContext2D::arc( qreal xc, qreal yc, qreal radius, qreal sar, qreal ear, bool anticlockwise )
{
    //### HACK
    // In Qt we don't switch the coordinate system for degrees
    // and still use the 0,0 as bottom left for degrees so we need
    // to switch
    sar=-sar;
    ear=-ear;
    anticlockwise=!anticlockwise;
    //end hack
    float sa=DEGREES(sar);
    float ea=DEGREES(ear);
    double span=0;
    double xs=xc-radius;
    double ys=yc-radius;
    double width=radius*2;
    double height=radius*2;

    if ( !anticlockwise && (ea<sa) ) {
        span+=360;
    } else if (anticlockwise && (sa<ea) ) {
        span-=360;
    }
    //### this is also due to switched coordinate system
    // we would end up with a 0 span instead of 360
    if ( !(qFuzzyCompare( span+(ea-sa)+1, 1 ) && qFuzzyCompare( qAbs( span ), 360 ) ) ) {
        span+=ea-sa;
    }
    QPainterPath path;
    path.moveTo( QPointF( xc+radius*cos( sar ), yc-radius*sin( sar ) ) );
    path.arcTo( xs, ys, width, height, sa, span );
    path=_state.matrix.map( path );
    _path.addPath( path );
}

void PHIContext2D::fill()
{
    beginPainting();
    _painter.fillPath( _path, _painter.brush() );
    scheduleChange();
}

void PHIContext2D::stroke()
{
    beginPainting();
    _painter.save();
    _painter.setMatrix( _state.matrix, false );
    QPainterPath tmp=_state.matrix.inverted().map( _path );
    _painter.strokePath( tmp, _painter.pen() );
    _painter.restore();
    scheduleChange();
}

void PHIContext2D::clip()
{
    _state.clipPath=_path;
    _state.flags |= DirtyClippingRegion;
}

bool PHIContext2D::isPointInPath( qreal x, qreal y ) const
{
    return _path.contains( QPointF( x, y ) );
}

/*
ImageData PHIContext2D::getImageData(qreal sx, qreal sy, qreal sw, qreal sh)
{
 Q_UNUSED(sx);
 Q_UNUSED(sy);
 Q_UNUSED(sw);
 Q_UNUSED(sh);
 return ImageData();
}

void PHIContext2D::putImageData(ImageData image, qreal dx, qreal dy)
{
 Q_UNUSED(image);
 Q_UNUSED(dx);
 Q_UNUSED(dy);
}
*/

PHIContext2D::PHIContext2D( QObject *parent )
    : QObject( parent ), _changeTimerId(-1)
{
    qDebug( "PHIContext2D::PHIContext2D()" );
    reset();
}

const QImage &PHIContext2D::endPainting()
{
    if ( _painter.isActive() ) _painter.end();
    return _image;
}

void PHIContext2D::beginPainting()
{
    if ( !_painter.isActive() ) {
        _painter.begin( &_image );
        _painter.setRenderHint( QPainter::Antialiasing );
        if ( !_state.clipPath.isEmpty() )
        _painter.setClipPath( _state.clipPath );
        _painter.setBrush( _state.fillStyle );
        _painter.setOpacity( _state.globalAlpha );
        QPen pen;
        pen.setBrush( _state.strokeStyle );
        if ( pen.style()==Qt::NoPen ) pen.setStyle( Qt::SolidLine );
        pen.setCapStyle( _state.lineCap );
        pen.setJoinStyle( _state.lineJoin );
        pen.setWidthF( _state.lineWidth );
        pen.setMiterLimit( _state.miterLimit );
        _painter.setPen( pen );
    } else {
        if ( (_state.flags & DirtyClippingRegion) && !_state.clipPath.isEmpty() )
            _painter.setClipPath(_state.clipPath);
        if ( _state.flags & DirtyFillStyle ) _painter.setBrush( _state.fillStyle );
        if ( _state.flags & DirtyGlobalAlpha )_painter.setOpacity( _state.globalAlpha );
        if ( _state.flags & DirtyGlobalCompositeOperation )
            _painter.setCompositionMode(_state.globalCompositeOperation);
        if ( _state.flags & MDirtyPen ) {
            QPen pen=_painter.pen();
            if ( _state.flags & DirtyStrokeStyle ) pen.setBrush( _state.strokeStyle );
            if ( _state.flags & DirtyLineWidth ) pen.setWidthF( _state.lineWidth );
            if ( _state.flags & DirtyLineCap ) pen.setCapStyle( _state.lineCap );
            if ( _state.flags & DirtyLineJoin ) pen.setJoinStyle( _state.lineJoin );
            if (_state.flags & DirtyMiterLimit) pen.setMiterLimit( _state.miterLimit );
            _painter.setPen( pen );
        }
        _state.flags=0;
    }
}

void PHIContext2D::clear()
{
    endPainting();
    _image.fill( qRgba( 0, 0, 0, 0 ) );
    scheduleChange();
}

void PHIContext2D::reset()
{
    _stateStack.clear();
    _state.matrix=QMatrix();
    _state.clipPath=QPainterPath();
    _state.globalAlpha=1.0;
    _state.globalCompositeOperation=QPainter::CompositionMode_SourceOver;
    _state.strokeStyle=Qt::black;
    _state.fillStyle=Qt::black;
    _state.lineWidth=1;
    _state.lineCap=Qt::FlatCap;
    _state.lineJoin=Qt::MiterJoin;
    _state.miterLimit=10;
    _state.shadowOffsetX=0;
    _state.shadowOffsetY=0;
    _state.shadowBlur=0;
    _state.shadowColor=qRgba( 0, 0, 0, 0 );
    _state.flags=AllIsFullOfDirt;
    clear();
}

void PHIContext2D::setSize(int width, int height)
{
    endPainting();
    QImage newimg( width, height, QImage::Format_ARGB32_Premultiplied );
    newimg.fill( qRgba( 0, 0, 0, 0 ) );
    QPainter p( &newimg );
    p.drawImage( 0, 0, _image );
    p.end();
    _image=newimg;
    scheduleChange();
}

void PHIContext2D::setSize( const QSize &size )
{
    setSize( size.width(), size.height() );
}

QSize PHIContext2D::size() const
{
    return _image.size();
}

void PHIContext2D::drawImage( PHIDomImage *image, qreal dx, qreal dy )
{
    if ( !image ) return;
    if ( dx<0 ) {
        qreal sx = qAbs(dx);
        qreal sy = qAbs(dy);
        qreal sw = image->width()-sx;
        qreal sh = image->height()-sy;
        drawImage( image, sx, sy, sw, sh, 0, 0, sw, sh );
    } else {
        beginPainting();
        _painter.drawImage( QPointF( dx, dy ), image->image() );
        scheduleChange();
    }
}

void PHIContext2D::drawImage( PHIDomImage *image, qreal dx, qreal dy, qreal dw, qreal dh )
{
    if ( !image ) return;
    beginPainting();
    _painter.drawImage( QRectF( dx, dy, dw, dh ).toRect(), image->image() );
    scheduleChange();
}

void PHIContext2D::drawImage( PHIDomImage *image, qreal sx, qreal sy, qreal sw, qreal sh,
    qreal dx, qreal dy, qreal dw, qreal dh )
{
    if ( !image ) return;
    beginPainting();
    _painter.drawImage( QRectF( dx, dy, dw, dh ), image->image(), QRectF( sx, sy, sw, sh ) );
    scheduleChange();
}

void PHIContext2D::scheduleChange()
{
    if (_changeTimerId == -1) _changeTimerId = startTimer(0);
}

void PHIContext2D::timerEvent( QTimerEvent *e )
{
    if ( e->timerId()==_changeTimerId ) {
        killTimer( _changeTimerId );
        _changeTimerId=-1;
        emit changed( endPainting() );
    } else {
        QObject::timerEvent( e );
    }
}
