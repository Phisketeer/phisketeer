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
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QPainter>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsTextItem>
#include <QTextDocument>
#include <QTextEdit>
#include <QStyleOptionGraphicsItem>
#include <QStyle>
#include <QBrush>
#include <QPen>
#include <QRectF>
#include <QSvgRenderer>
#include <QFontMetrics>
#include <QLinearGradient>
#include "phisimagecache.h"
#include "phisitemcache.h"
#include "phispagecache.h"
#include "phishtmlbase.h"
#include "phierror.h"
#include "phinetrequest.h"

QImage* PHISHtmlBase::deviceReferenceImage() const
{
    static QImage img( 1, 1, QImage::Format_ARGB32_Premultiplied );
    img.setDotsPerMeterX( 3780 );
    img.setDotsPerMeterY( 3780 );
    return &img;
}

QImage PHISHtmlBase::createHeaderImage() const
{
    QFont f=_it->font();
    if ( f==PHI::invalidFont() ) f=_p->font();
    int height=QFontMetrics( f ).height();
    QColor c=_p->palette().color( QPalette::Button );
    QColor cmid=PHI::percentColor( c, 0.3 );
    QColor clight=PHI::percentColor( c, 0.9 );
    //clight.setAlphaF( 0.9 );
    QImage img( 1, height+12, QImage::Format_ARGB32_Premultiplied );
    img.fill( 0 );

    QLinearGradient gradient( 0., 0., 0., 1. );
    gradient.setColorAt( 0., clight );
    gradient.setColorAt( 0.5, cmid );
    gradient.setColorAt( 0.5001, c );
    gradient.setColorAt( 1., c );
    gradient.setCoordinateMode( QGradient::ObjectBoundingMode );

    QPainter p( &img );
    p.setCompositionMode( QPainter::CompositionMode_SourceOver );
    p.fillRect( 0, 0, img.width(), img.height(), gradient );
    p.end();
    return img;
}

QImage PHISHtmlBase::createSvgImage() const
{
    QSvgRenderer renderer;
    renderer.load( _it->valueData() );
    if ( !renderer.isValid() ) {
        _req->responseRec()->log( PHILOGERR, PHIRC_OBJ_ACCESS_ERROR,
            QObject::tr( "Could not parse XML for SVG item '%1' in page '%2'." )
                .arg( _it->name() ).arg( _p->id() ) );
        return QImage();
    }
    QImage img( renderer.defaultSize().width(), renderer.defaultSize().height(),
        QImage::Format_ARGB32_Premultiplied );
    img.fill( 0 );
    QPainter p;
    p.begin( &img );
    p.setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing );
    renderer.render( &p );
    p.end();
    return img;
}

QImage PHISHtmlBase::createStaticGoogleMapImage() const
{
    PHINetRequestThread request( _req->responseRec() );
    QString url;
    url.reserve( 200 );
    url+="http://maps.google.com/maps/api/staticmap?size=";
    url+=QString::number( static_cast<int>(_it->width()) )+'x';
    url+=QString::number( static_cast<int>(_it->height()) )+'&';
    url+=_it->value();
    return request.getImage( url );
}

QImage PHISHtmlBase::createImageImage( const QString &imageId ) const
{
    QImage img;
    if ( imageId.startsWith( "phi" ) ) img.load( _req->imgDir()+QDir::separator()+imageId );
    else {
        img.load( _req->documentRoot()+imageId );
        img=img.scaled( static_cast<int>(_it->width()), static_cast<int>(_it->height()),
            Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
    }
    return img;
}

QImage PHISHtmlBase::createRolloverTextImage( int over, const QImage &image ) const
{
    int width=static_cast<int>(_it->width());
    int height=static_cast<int>(_it->height());
    QImage img( width, height, QImage::Format_ARGB32_Premultiplied );
    img.fill( 0 );
    QPainter p( &img );
    p.setRenderHint( QPainter::Antialiasing, true );
    p.setRenderHint( QPainter::HighQualityAntialiasing, true );
    p.setCompositionMode( QPainter::CompositionMode_SourceOver );
    if ( over ) {
        if ( _it->rolloverBackgroundColor().isValid() )
            p.fillRect( 0, 0, width, height, _it->rolloverBackgroundColor() );
        else p.fillRect( 0, 0, width, height, Qt::transparent );
        if ( !image.isNull() ) p.drawImage( 0, 0, image );
        p.setPen( _it->rolloverTextColor() );
    } else {
        if ( _it->outlineColor().isValid() )
            p.fillRect( 0, 0, width, height, _it->outlineColor() );
        else p.fillRect( 0, 0, width, height, Qt::transparent );
        if ( !image.isNull() ) p.drawImage( 0, 0, image );
        p.setPen( _it->color().isValid() ? _it->color() : _p->palette().color( QPalette::WindowText ) );
    }
    p.setRenderHint( QPainter::TextAntialiasing, true );
    QFont tf=_it->font();
    if ( tf==PHI::invalidFont() ) tf=_p->font();
    QFont f( tf, deviceReferenceImage() );
//#ifdef PHIMACFONT
    f.setPointSizeF( f.pointSizeF()*PHI::fontSizeFactor() );
//#endif
    //QPen pen=p.pen();
    //pen.setStyle( Qt::SolidLine );
    //pen.setWidthF( 0.05 );
    //p.setPen( pen );
    p.setFont( f );
    p.drawText( QRect( 3, 3, width-6, height-6 ), PHI::toQtAlignment( _it->alignment() ), _it->value() );
    p.end();
    return img;
}

QImage PHISHtmlBase::createEllipseImage() const
{
    Q_ASSERT( _it );
    QBrush brush( _it->color().isValid() ? _it->color() : QColor( Qt::black ) );
    brush.setStyle( static_cast<Qt::BrushStyle>(_it->pattern()<15 ? _it->pattern() : 1) );
    if ( _it->pattern()==0 ) brush.setColor( Qt::transparent );
    QPen pen( _it->outlineColor().isValid() ? _it->outlineColor() : QColor( Qt::black ) );
    pen.setWidthF( _it->penWidth() );
    pen.setStyle( static_cast<Qt::PenStyle>(_it->line()) );
    pen.setJoinStyle( Qt::MiterJoin );
    QRectF r;
    QImage img;
    if ( _it->itemProperties() & PHIItem::PLine  ) {
        int off=static_cast<int>(_it->penWidth()/2.);
        if ( static_cast<int>(_it->penWidth()) % 2 ) off++;
        if ( static_cast<int>(_it->penWidth())==1. ) {
            img=QImage( static_cast<int>(_it->width())+2, static_cast<int>(_it->height())+2,
                QImage::Format_ARGB32_Premultiplied );
            r=QRectF( 1, 1, _it->width(), _it->height() );
        } else {
            img=QImage( static_cast<int>(_it->width())+off*2, static_cast<int>(_it->height())+off*2,
                QImage::Format_ARGB32_Premultiplied );
            r=QRectF( off, off, _it->width(), _it->height() );
        }
    } else {
        img=QImage( static_cast<int>(_it->width()), static_cast<int>(_it->height()),
            QImage::Format_ARGB32_Premultiplied );
        r=QRectF( 0, 0, _it->width(), _it->height() );
    }
    img.fill( 0 );
    QPainter p( &img );
    p.setCompositionMode( QPainter::CompositionMode_Source );
    p.setRenderHints( QPainter::SmoothPixmapTransform | QPainter::Antialiasing );
    p.setBrush( _it->pattern()==15 ? QBrush( _it->gradient() ) : brush );
    p.setPen( pen );
    if ( _it->startAngle()==0 && _it->spanAngle()==5760 ) p.drawEllipse( r );
    else p.drawPie( r, static_cast<int>(_it->startAngle()), static_cast<int>(_it->spanAngle()) );
    p.end();
    return img;
}

QImage PHISHtmlBase::createRoundedRectImage() const
{
    Q_ASSERT( _it );
    QBrush brush( _it->color().isValid() ? _it->color() : QColor( Qt::black ) );
    brush.setStyle( static_cast<Qt::BrushStyle>(_it->pattern()<15 ? _it->pattern() : 1) );
    if ( _it->pattern()==0 ) brush.setColor( Qt::transparent );
    //qDebug( "Color %s", qPrintable( it->color().name() ) );
    //qDebug( "Outline color %s", qPrintable( it->outlineColor().name() ) );
    QPen pen( _it->outlineColor().isValid() ? _it->outlineColor() : QColor( Qt::black ) );
    pen.setWidthF( _it->penWidth() );
    pen.setStyle( static_cast<Qt::PenStyle>(_it->line()) );
    pen.setJoinStyle( Qt::MiterJoin );
    QRectF r;
    QImage img;
    if ( _it->itemProperties() & PHIItem::PLine  ) {
        int off=static_cast<int>(_it->penWidth()/2.);
        if ( static_cast<int>(_it->penWidth()) % 2 ) off++;
        if ( static_cast<int>(_it->penWidth())==1. ) {
            img=QImage( static_cast<int>(_it->width())+2, static_cast<int>(_it->height())+2,
                QImage::Format_ARGB32_Premultiplied );
            r=QRectF( 1, 1, _it->width(), _it->height() );
        } else {
            img=QImage( static_cast<int>(_it->width())+off*2, static_cast<int>(_it->height())+off*2,
                QImage::Format_ARGB32_Premultiplied );
            r=QRectF( off, off, _it->width(), _it->height() );
        }
    } else {
        img=QImage( static_cast<int>(_it->width()), static_cast<int>(_it->height()),
            QImage::Format_ARGB32_Premultiplied );
        r=QRectF( 0, 0, _it->width(), _it->height() );
    }
    img.fill( 0 );
    QPainter p;
    p.begin( &img );
    //p.setCompositionMode( QPainter::CompositionMode_Clear );
    //p.fillRect( 0, 0, img.width(), img.height(), Qt::transparent );
    p.setCompositionMode( QPainter::CompositionMode_Source );
    p.setRenderHints( QPainter::SmoothPixmapTransform | QPainter::Antialiasing );
    p.setBrush( _it->pattern()==15 ? QBrush( _it->gradient() ) : brush );
    p.setPen( pen );
    qDebug( "BORDER %d", _it->borderRadius() );
    p.drawRoundedRect( r, _it->borderRadius(), _it->borderRadius() );
    p.end();
    return img;
}

QImage PHISHtmlBase::createRectImage() const
{
    Q_ASSERT( _it );
    QBrush brush( _it->color().isValid() ? _it->color() : QColor( Qt::black ) );
    brush.setStyle( static_cast<Qt::BrushStyle>(_it->pattern()<15 ? _it->pattern() : 1) );
    if ( _it->pattern()==0 ) brush.setColor( Qt::transparent );
    //qDebug( "Color %s", qPrintable( it->color().name() ) );
    //qDebug( "Outline color %s", qPrintable( it->outlineColor().name() ) );
    QPen pen( _it->outlineColor().isValid() ? _it->outlineColor() : QColor( Qt::black ) );
    pen.setWidthF( _it->penWidth() );
    pen.setStyle( static_cast<Qt::PenStyle>(_it->line()) );
    pen.setJoinStyle( Qt::MiterJoin );
    QRectF r;
    QImage img;
    if ( _it->itemProperties() & PHIItem::PLine  ) {
        int off=static_cast<int>(_it->penWidth()/2.);
        if ( static_cast<int>(_it->penWidth()) % 2 ) off++;
        if ( static_cast<int>(_it->penWidth())==1. ) {
            img=QImage( static_cast<int>(_it->width())+1, static_cast<int>(_it->height())+1,
                QImage::Format_ARGB32_Premultiplied );
            r=QRectF( 0, 0, _it->width(), _it->height() );
        } else {
            img=QImage( static_cast<int>(_it->width())+off*2, static_cast<int>(_it->height())+off*2,
                QImage::Format_ARGB32_Premultiplied );
            r=QRectF( off, off, _it->width(), _it->height() );
        }
    } else {
        img=QImage( static_cast<int>(_it->width()), static_cast<int>(_it->height()),
            QImage::Format_ARGB32_Premultiplied );
        r=QRectF( 0, 0, _it->width(), _it->height() );
    }
    img.fill( 0 );
    QPainter p;
    p.begin( &img );
    p.setCompositionMode( QPainter::CompositionMode_Source );
    p.setRenderHints( QPainter::SmoothPixmapTransform );
    p.setBrush( _it->pattern()==15 ? QBrush( _it->gradient() ) : brush );
    p.setPen( pen );
    p.drawRect( r );
    p.end();
    return img;
}

QImage PHISHtmlBase::createRichTextImage() const
{
    return QImage();
    Q_ASSERT(_it);
    QFont tf;
    if ( _it->font()==PHI::invalidFont() ) tf=_p->font();
    else tf=_it->font();

    QFont f( tf, deviceReferenceImage() );
#ifdef PHIMACFONT
    f.setPointSizeF( tf.pointSizeF()*PHI::fontSizeFactor() );
#endif
    QGraphicsTextItem item;
    item.setFont( f );
    item.setTextWidth( _it->width() );
    if ( _it->color().isValid() ) item.setDefaultTextColor( _it->color() );
    else {
        QPalette pal=_p->palette();
        item.setDefaultTextColor( pal.color( QPalette::WindowText ) );
    }
    QTextDocument *doc=item.document();
    QTextOption o=doc->defaultTextOption();
    o.setAlignment( PHI::toQtAlignment( _it->alignment() ) );
    doc->setDefaultTextOption( o );
    if ( _it->itemProperties() & PHIItem::PStyleSheet ) {
        doc->setDefaultStyleSheet( _it->styleSheet() );
    }
    item.setHtml( _it->value() );

    QStyleOptionGraphicsItem opt;
    opt.state = QStyle::State_None;
    const QRect brect=item.boundingRect().toRect();
    QImage img( brect.width(), brect.height(), QImage::Format_ARGB32_Premultiplied );
    img.fill( 0 );
    QPainter p;
    p.begin( &img );
    p.setCompositionMode( QPainter::CompositionMode_Source );
    p.setRenderHint( QPainter::TextAntialiasing, true );
    p.setRenderHint( QPainter::Antialiasing, true );
    item.paint( &p, &opt, 0 );
    p.end();
    return img;
}

QImage PHISHtmlBase::createGraphicsTextImage() const
{
    Q_ASSERT(_it);
    QBrush brush( _it->color().isValid() ? _it->color() : QColor( Qt::black ) );
    brush.setStyle( static_cast<Qt::BrushStyle>(_it->pattern()<15 ? _it->pattern() : 1) );
    if ( _it->pattern()==0 ) brush.setColor( Qt::transparent );
    QPen pen( _it->outlineColor().isValid() ? _it->outlineColor() : QColor( Qt::black ) );
    pen.setWidthF( _it->penWidth() );
    pen.setStyle( static_cast<Qt::PenStyle>(_it->line()) );
    pen.setJoinStyle( Qt::MiterJoin );
    QFont f( _it->font(), deviceReferenceImage() );
    f.setPointSizeF( _it->font().pointSizeF() );

    QGraphicsSimpleTextItem item;
    //f.setWeight( f.weight()*0.75);
    item.setFont( f );
    item.setText( _it->value() );

    if ( _it->line()==0 ) {
        pen.setStyle( Qt::SolidLine );
        pen.setWidthF( 0.05 );
    }
    item.setPen( pen );
    if ( _it->pattern()==15 ) item.setBrush( QBrush( _it->gradient() ) );
    else item.setBrush( brush );

    QStyleOptionGraphicsItem opt;
    opt.state = QStyle::State_None;
    //opt.rect = r.toRect();
    //opt.levelOfDetail = 1;
    //opt.exposedRect = r;
    //QTransform ident;
    //opt.matrix = t.toAffine(); // discards perspective
    const QRectF brect = item.boundingRect();
    QImage img( static_cast<int>(brect.width()), static_cast<int>(brect.height()),
        QImage::Format_ARGB32_Premultiplied );
    img.fill( 0 );
    QPainter p;
    p.begin( &img );
    p.setCompositionMode( QPainter::CompositionMode_Source );
    p.setRenderHint( QPainter::TextAntialiasing, true );
    p.setRenderHint( QPainter::Antialiasing, true );
    item.paint( &p, &opt, 0 );
    p.end();
    return img;
}

QRect PHISHtmlBase::transformationPosition( const QTransform &t ) const
{
    int height=static_cast<int>(_it->height());
    int width=static_cast<int>(_it->width());
    qreal xOff=0, yOff=0;
    if ( _it->wid()==PHI::GRAPH_TEXT ) { // Adjust size for other languages than default
        QFont f( _it->font(), deviceReferenceImage() ); // Hack to get the right font size for an image device
        f.setPointSizeF( _it->font().pointSizeF() );

        QGraphicsSimpleTextItem item;
        item.setFont( f );
        item.setText( _it->value() );
        //QBrush brush( Qt::black );
        QPen pen( Qt::black );
        pen.setWidthF( _it->penWidth() );
        pen.setStyle( static_cast<Qt::PenStyle>(_it->line()) );
        pen.setJoinStyle( Qt::MiterJoin );
        if ( _it->line()==0 ) {
            pen.setStyle( Qt::SolidLine );
            pen.setWidthF( 0.05 );
        }
        item.setPen( pen );
        const QRectF brect = item.boundingRect();
        if ( _it->transformPosition()>0 ) {
            qreal xFactor=.0;
            qreal yFactor=.0;
            switch( _it->transformPosition() ) {
            case 1: break;
            case 2: xFactor=.5; yFactor=0; break;
            case 3: xFactor=1; yFactor=0; break;
            case 4: xFactor=0; yFactor=.5; break;
            case 5: xFactor=.5; yFactor=.5; break;
            case 6: xFactor=1; yFactor=.5; break;
            case 7: xFactor=0; yFactor=1; break;
            case 8: xFactor=.5; yFactor=1; break;
            case 9: xFactor=1; yFactor=1; break;
            }
            xOff=xFactor*(_it->width()-brect.width());
            yOff=yFactor*(_it->height()-brect.height());
        }
        height=static_cast<int>(brect.height());
        width=static_cast<int>(brect.width());
    }
    qint8 blur=0, sxOff=0, syOff=0;
    if ( _it->effect()->graphicsType()==PHIEffect::GTSurface ) {
        qreal size, off;
        _it->effect()->surface( off, size );
        height=height+off+size;
    } else if ( _it->effect()->graphicsType()==PHIEffect::GTBlur ) {
        qreal radius;
        _it->effect()->blur( radius );
        if ( radius>1 ) {
            blur=static_cast<qint8>(radius);
            height+=2*radius;
            width+=2*radius;
        }
    } else if ( _it->effect()->graphicsType()==PHIEffect::GTShadow ) {
        QColor c;
        qreal offX, offY, radius;
        _it->effect()->shadow( c, offX, offY, radius );
        if ( radius<=1 ) radius=0;
        width+=static_cast<int>(qAbs( offX ))+static_cast<int>(2*radius);
        height+=static_cast<int>(qAbs( offY ))+static_cast<int>(2*radius);
        if ( offX<0 ) sxOff=static_cast<qint8>(offX);
        if ( offY<0 ) syOff=static_cast<qint8>(offY);
        blur=static_cast<qint8>(radius);
    }
    QRect r;
    if ( PHI::isShapeItem( _it->wid() ) ) {
        if ( _it->itemProperties() & PHIItem::PLine) {
            int off=static_cast<int>(_it->penWidth()/2.);
            if ( static_cast<int>(_it->penWidth()) % 2 ) off++;
            if ( static_cast<int>(_it->penWidth())==1 ) {
                if ( _it->wid()==PHI::RECT ) r=t.mapRect( QRect( -blur+sxOff, -blur+syOff, width+1, height+1 ) );
                else r=t.mapRect( QRect( xOff-blur-1+sxOff, yOff-blur-1+syOff, width+2, height+2 ) );
            } else {
                r=t.mapRect( QRect( xOff-off-blur+sxOff, yOff-off-blur+syOff, width+off*2, height+off*2 ) );
            }
        } else r=t.mapRect( QRect( xOff-blur+sxOff, yOff-blur+syOff, width, height ) );
    } else {
        r=t.mapRect( QRect( -blur+sxOff, -blur+syOff, width, height ) );
    }
    return r;
}

QByteArray PHISHtmlBase::transformationPositionStyle( const QTransform &t, bool size ) const
{
    Q_ASSERT( _it );
    // Check if have a cashed position:
    QByteArray arr=PHISItemCache::instance()->position( _req->hostname(), _it->pageId(), _it->id()+_lang );
    if ( !arr.isNull() ) {
        qDebug( "Found cashed position %s", _it->id().data() );
        return arr;
    }
    QRect r=transformationPosition( t );
    arr.reserve( 100 );
    arr+="left:"+QByteArray::number( static_cast<int>(_it->x())+r.x() )+"px;top:"
        +QByteArray::number( static_cast<int>(_it->y())+r.y() )+"px;z-index:"
        +QByteArray::number( static_cast<int>(_it->zValue()) )+';';
    if ( size ) {
        arr+="width:"+QByteArray::number( static_cast<int>(r.width()) )+"px;height:"
            +QByteArray::number( static_cast<int>(r.height()) )+"px;";
    }
    if ( !(_it->itemProperties() & PHIItem::PNoCache) )
        PHISItemCache::instance()->insertPosition( _req->hostname(), _it->pageId(),
            _it->id()+_lang, arr );
    qDebug( "size %s %d %d", arr.data(), arr.capacity(), arr.size() );
    return arr;
}

QString PHISHtmlBase::storeImage( const QImage &img, const QByteArray &postfix, int number ) const
{
    QString id;
    if ( _it->itemProperties() & PHIItem::PNoCache ) {
        // do not cache item, so we create a uuid:
        PHISImageCache cache( _req->responseRec(), _req->tmpDir() );
        id=cache.createId();
        QFile f( _req->imgDir()+QDir::separator()+id );
        if ( !f.open( QIODevice::WriteOnly ) ) {
            _req->responseRec()->log( PHILOGERR, PHIRC_IO_FILE_CREATION_ERROR,
                QObject::tr( "Could not save image '%1'." )
                .arg( _req->imgDir()+QDir::separator()+id )
                +PHI::errorText().arg( f.errorString() ) );
            return QString();
        } else {
            img.save( &f, "PNG" );
            f.close();
            return id;
        }
    }
    id=PHISItemCache::instance()->imageId( _req->hostname(), _it->pageId(),
        _it->id()+postfix, _lang, number );
    if ( !id.isNull() && QFile::exists( _req->imgDir()+QDir::separator()+id ) ) return id;

    if ( number>-1 && number<_it->pictureBookIds().count() ) id=_it->pictureBookIds().at( number );
    else id=_it->imageId();
    if ( id.isEmpty() ) { // not a picture book image nor an image path
        id="phi"+_it->pageId()+'_'+_it->id();
        if ( _it->wid()==PHI::GRAPH_TEXT ) id+='_'+_lang;
        if ( _it->wid()==PHI::ROLLOVER_BUTTON ) id+='_'+QByteArray::number( number )+'_'+_lang;
        id+=postfix+".png";
    } else { // file already saved before on system
        if ( !id.startsWith( "phi" ) ) {
            id.prepend( "phi" );
            id.replace( '\\', '_' );
            id.replace( '/', '_' );
        }
        id.chop( 4 );
        id.append( postfix+".png" );
    }
    QFile sf( _req->imgDir()+QDir::separator()+id );
    if ( sf.exists() ) {
        // File exists but is not in cache, check if the image file was already updated by another thread:
        QDateTime pageModified=PHISPageCache::modified( _req, _p->pageId() );
        QFileInfo fi( sf );
        if ( fi.lastModified() >= pageModified ) {
            qDebug( "Image file %s is up to date", qPrintable( id ) );
            PHISItemCache::instance()->insertImageId( _req->hostname(), _it->pageId(),
                _it->id()+postfix, id, _lang, number );
            return id;
        }
    }
    if ( !sf.open( QIODevice::WriteOnly ) ) {
        _req->responseRec()->log( PHILOGERR, PHIRC_IO_FILE_CREATION_ERROR,
            QObject::tr( "Could not save image '%1'." )
            .arg( _req->imgDir()+QDir::separator()+id )+PHI::errorText().arg( sf.errorString() ) );
        return QString();
    }
    img.save( &sf, "PNG" );
    sf.close();
    qDebug( "saved image id %s", qPrintable( id ) );
    PHISItemCache::instance()->insertImageId( _req->hostname(), _it->pageId(),
        _it->id()+postfix, id, _lang, number );
    return id;
}

QImage PHISHtmlBase::graphicsEffectImage( const QImage &img ) const
{
    if ( !_it->hasGraphicEffect() ) return img;
    if ( _it->effect()->graphicsType()==PHIEffect::GTSurface ) return surfaceEffectImage( img );
    if ( _it->effect()->graphicsType()==PHIEffect::GTShadow ) return shadowEffectImage( img );
    if ( _it->effect()->graphicsType()==PHIEffect::GTBlur ) return blurEffectImage( img );
    if ( _it->effect()->graphicsType()==PHIEffect::GTColorize ) return colorizeEffectImage( img );
    return img;
}

QImage PHISHtmlBase::blurEffectImage( const QImage &img ) const
{
    Q_ASSERT( _it->effect()->graphicsType()==PHIEffect::GTBlur );
    qreal radius;
    _it->effect()->blur( radius );
    return PHI::getBluredImage( img, radius );
}

QImage PHISHtmlBase::surfaceEffectImage( const QImage &img ) const
{
    Q_ASSERT( _it->effect()->graphicsType()==PHIEffect::GTSurface );
    qreal size, off;
    _it->effect()->surface( off, size );
    return PHI::getSurfacedImage( img, off, size );
}

QImage PHISHtmlBase::colorizeEffectImage( const QImage &img ) const
{
    Q_ASSERT( _it->effect()->graphicsType()==PHIEffect::GTColorize );
    QColor c;
    qreal strength;
    _it->effect()->colorize( c, strength );
    qDebug( "colorize" );
    return PHI::getColorizedImage( img, c, strength );
}

QImage PHISHtmlBase::shadowEffectImage( const QImage &img ) const
{
    Q_ASSERT( _it->effect()->graphicsType()==PHIEffect::GTShadow );
    QColor c;
    qreal xOff, yOff, radius;
    _it->effect()->shadow( c, xOff, yOff, radius );
    return PHI::getShadowedImage( img, c, radius, xOff, yOff );
}
