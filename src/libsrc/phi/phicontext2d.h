/*
#    Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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
#ifndef PHICONTEXT2D_H
#define PHICONTEXT2D_H

#include <QObject>
#include <QPainterPath>
#include <QGradient>
#include <QPainter>
#include <QStack>
#include <QScriptValue>
#include "phi.h"
#include "phidomimage.h"

class QScriptEngine;

class PHIEXPORT PHICanvasGradient : public QObject
{
    Q_OBJECT

public:
    explicit PHICanvasGradient( const QGradient &v, QObject *parent=0 )
        : QObject( parent ), _gradient( v ) {}
    explicit PHICanvasGradient( QObject *parent=0 ) : QObject( parent ) {}
    inline QGradient gradient() const { return _gradient; }

public slots:
    PHICanvasGradient* addColorStop( qreal offset, const QString &color );

protected:
    QGradient _gradient;
};

Q_DECLARE_METATYPE( PHICanvasGradient* )

PHIEXPORT QScriptValue canvasGradientToScriptValue( QScriptEngine*, PHICanvasGradient* const &in );
PHIEXPORT void canvasGradientFromScriptValue( const QScriptValue&, PHICanvasGradient* &out );

class PHIImageData
{
};

Q_DECLARE_METATYPE( PHIImageData )

class PHIEXPORT PHIDomRect // used for measureText()
{
public:
    explicit PHIDomRect( const QRect &r ) : _rect( r ) {}
    explicit PHIDomRect() {}
    inline int width() const { return _rect.width(); }
    inline void setWidth( int w ) { _rect.setWidth( w ); }

private:
    QRect _rect;
};

Q_DECLARE_METATYPE( PHIDomRect )

PHIEXPORT QScriptValue domRectToScriptValue( QScriptEngine*, const PHIDomRect &in );
PHIEXPORT void domRectFromScriptValue( const QScriptValue&, PHIDomRect &out );

class PHIEXPORT PHIContext2D : public QObject
{
    Q_OBJECT
    Q_PROPERTY( qreal globalAlpha READ globalAlpha WRITE setGlobalAlpha )
    Q_PROPERTY( QString globalCompositeOperation READ globalCompositeOperation WRITE setGlobalCompositeOperation )
    Q_PROPERTY( QVariant strokeStyle READ strokeStyle WRITE setStrokeStyle )
    Q_PROPERTY( QVariant fillStyle READ fillStyle WRITE setFillStyle )
    Q_PROPERTY( qreal lineWidth READ lineWidth WRITE setLineWidth )
    Q_PROPERTY( QString lineCap READ lineCap WRITE setLineCap )
    Q_PROPERTY( QString lineJoin READ lineJoin WRITE setLineJoin )
    Q_PROPERTY( qreal miterLimit READ miterLimit WRITE setMiterLimit )
    Q_PROPERTY( qreal shadowOffsetX READ shadowOffsetX WRITE setShadowOffsetX )
    Q_PROPERTY( qreal shadowOffsetY READ shadowOffsetY WRITE setShadowOffsetY )
    Q_PROPERTY( qreal shadowBlur READ shadowBlur WRITE setShadowBlur )
    Q_PROPERTY( QString shadowColor READ shadowColor WRITE setShadowColor )
    Q_PROPERTY( QString textAlign READ textAlign WRITE setTextAlign )
    Q_PROPERTY( QString textBaseline READ textBaseline WRITE setTextBaseline )
    Q_PROPERTY( QString font READ font WRITE setFont )

public:
    PHIContext2D( QObject *parent=0 );
    void setSize( int width, int height );
    void setSize( const QSize &size );
    QSize size() const;

    void clear();
    void reset();

    qreal globalAlpha() const; // (default 1.0)
    QString globalCompositeOperation() const; // (default over)
    QVariant strokeStyle() const; // (default black)
    QVariant fillStyle() const; // (default black)

    void setGlobalAlpha( qreal alpha );
    void setGlobalCompositeOperation( const QString &op );
    void setStrokeStyle( const QVariant &style );
    void setFillStyle( const QVariant &style );

    qreal lineWidth() const; // (default 1)
    QString lineCap() const; // "butt", "round", "square" (default "butt")
    QString lineJoin() const; // "round", "bevel", "miter" (default "miter")
    qreal miterLimit() const; // (default 10)
    QString textAlign() const; // "start" "left" "end" "right" "center" (default "start")
    QString textBaseline() const; // "top" "hanging" "middle" "alphabetic" "ideographic" "bottom"
    QString font() const; // (default "10px Helvetica")

    void setLineWidth( qreal w );
    void setLineCap( const QString &s );
    void setLineJoin( const QString &s );
    void setMiterLimit( qreal m );
    void setTextAlign( const QString &s );
    void setTextBaseline( const QString &s );
    void setFont( const QString &f );

    qreal shadowOffsetX() const; // (default 0)
    qreal shadowOffsetY() const; // (default 0)
    qreal shadowBlur() const; // (default 0)
    QString shadowColor() const; // (default black)

    void setShadowOffsetX( qreal x );
    void setShadowOffsetY( qreal y );
    void setShadowBlur( qreal b );
    void setShadowColor( const QString &str );

public slots:
    void save(); // push state on state stack
    void restore(); // pop state stack and restore state

    void scale( qreal x, qreal y );
    void rotate( qreal angle );
    void translate( qreal x, qreal y );
    void transform( qreal m11, qreal m12, qreal m21, qreal m22, qreal dx, qreal dy );
    void setTransform( qreal m11, qreal m12, qreal m21, qreal m22, qreal dx, qreal dy );

    PHICanvasGradient* createLinearGradient( qreal x0, qreal y0, qreal x1, qreal y1 );
    PHICanvasGradient* createRadialGradient( qreal x0, qreal y0, qreal r0, qreal x1, qreal y1, qreal r1);
    // not part of the W3C specification:
    PHICanvasGradient* createConicalGradient( qreal x, qreal y, qreal angle );

    void clearRect( qreal x, qreal y, qreal w, qreal h);
    void fillRect( qreal x, qreal y, qreal w, qreal h);
    void strokeRect( qreal x, qreal y, qreal w, qreal h);

    void fillText( const QString &t, qreal x, qreal y, qreal maxwidth=-1. );
    void strokeText( const QString &t, qreal x, qreal y, qreal maxwidth=-1. );
    PHIDomRect measureText( const QString &t );

    void beginPath();
    void closePath();
    void moveTo( qreal x, qreal y );
    void lineTo( qreal x, qreal y );
    void quadraticCurveTo( qreal cpx, qreal cpy, qreal x, qreal y );
    void bezierCurveTo( qreal cp1x, qreal cp1y, qreal cp2x, qreal cp2y, qreal x, qreal y );
    void arcTo( qreal x1, qreal y1, qreal x2, qreal y2, qreal radius );
    void rect( qreal x, qreal y, qreal w, qreal h );
    void arc( qreal x, qreal y, qreal radius, qreal startAngle, qreal endAngle, bool anticlockwise );
    void ellipse( qreal x, qreal y, qreal w, qreal h );
    void fill();
    void stroke();
    void clip();
    bool isPointInPath( qreal x, qreal y ) const;

    void drawImage( PHIDomImage *image, qreal dx, qreal dy );
    void drawImage( PHIDomImage *image, qreal dx, qreal dy, qreal dw, qreal dh );
    void drawImage( PHIDomImage *image, qreal sx, qreal sy, qreal sw, qreal sh, qreal dx, qreal dy, qreal dw, qreal dh );

    PHIImageData getImageData( qreal sx, qreal sy, qreal sw, qreal sh );
    void putImageData( PHIImageData image, qreal dx, qreal dy );

signals:
    void changed( const QImage &image );

protected:
    void timerEvent( QTimerEvent *e );

private:
    void beginPainting();
    const QImage &endPainting();
    void scheduleChange();

    int _changeTimerId;
    QImage _image;
    QPainter _painter;
    QPainterPath _path;

    enum DirtyFlag {
        DirtyTransformationMatrix = 0x00001,
        DirtyClippingRegion       = 0x00002,
        DirtyStrokeStyle          = 0x00004,
        DirtyFillStyle            = 0x00008,
        DirtyGlobalAlpha          = 0x00010,
        DirtyLineWidth            = 0x00020,
        DirtyLineCap              = 0x00040,
        DirtyLineJoin             = 0x00080,
        DirtyMiterLimit           = 0x00100,
        MDirtyPen                 = DirtyStrokeStyle
                               | DirtyLineWidth
                               | DirtyLineCap
                               | DirtyLineJoin
                               | DirtyMiterLimit,
        DirtyShadowOffsetX        = 0x00200,
        DirtyShadowOffsetY        = 0x00400,
        DirtyShadowBlur           = 0x00800,
        DirtyShadowColor          = 0x01000,
        DirtyGlobalCompositeOperation = 0x2000,
        DirtyFont                 = 0x04000,
        DirtyTextAlign            = 0x08000,
        DirtyTextBaseline         = 0x10000,
        AllIsFullOfDirt           = 0xfffff
    };

    struct State {
        State() : flags(0) {}
        QMatrix matrix;
        QPainterPath clipPath;
        QBrush strokeStyle;
        QBrush fillStyle;
        qreal globalAlpha;
        qreal lineWidth;
        Qt::PenCapStyle lineCap;
        Qt::PenJoinStyle lineJoin;
        qreal miterLimit;
        qreal shadowOffsetX;
        qreal shadowOffsetY;
        qreal shadowBlur;
        QColor shadowColor;
        QPainter::CompositionMode globalCompositeOperation;
        QFont font;
        qint8 textAlign;
        qint8 textBaseline;
        int flags;
    };
    State _state;
    QStack<State> _stateStack;
};

Q_DECLARE_METATYPE( PHIContext2D* )

PHIEXPORT QScriptValue context2DToScriptValue( QScriptEngine*, PHIContext2D* const &in );
PHIEXPORT void context2DFromScriptValue( const QScriptValue&, PHIContext2D* &out );

inline void PHIContext2D::save()
{
    _stateStack.push( _state );
}

inline void PHIContext2D::restore()
{
    if ( !_stateStack.isEmpty() ) {
        _state=_stateStack.pop();
        _state.flags=AllIsFullOfDirt;
    }
}

inline void PHIContext2D::scale( qreal x, qreal y )
{
    _state.matrix.scale( x, y );
    _state.flags |= DirtyTransformationMatrix;
}

inline qreal PHIContext2D::miterLimit() const
{
    return _state.miterLimit;
}

inline void PHIContext2D::setMiterLimit( qreal m )
{
    _state.miterLimit=m;
    _state.flags |= DirtyMiterLimit;
}

inline void PHIContext2D::setShadowOffsetX( qreal x )
{
    _state.shadowOffsetX = x;
    _state.flags |= DirtyShadowOffsetX;
}

inline void PHIContext2D::setShadowOffsetY( qreal y )
{
    _state.shadowOffsetY = y;
    _state.flags |= DirtyShadowOffsetY;
}

inline void PHIContext2D::setShadowBlur( qreal b )
{
    _state.shadowBlur = b;
    _state.flags |= DirtyShadowBlur;
}

inline qreal PHIContext2D::shadowOffsetX() const
{
    return _state.shadowOffsetX;
}

inline qreal PHIContext2D::shadowOffsetY() const
{
    return _state.shadowOffsetY;
}

inline qreal PHIContext2D::shadowBlur() const
{
    return _state.shadowBlur;
}

inline QString PHIContext2D::shadowColor() const
{
    return _state.shadowColor.name();
}

#endif // PHICONTEXT2D_H
