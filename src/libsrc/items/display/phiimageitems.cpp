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
#include <QTimer>
#include <QSvgRenderer>
#include "phiimageitems.h"
#include "phibasepage.h"
#include "phidataparser.h"
#include "phirequest.h"
#include "phicontext2d.h"

void PHIImageItem::updateImage()
{
    update();
}

QString PHISvgItem::svgDefaultSource()
{
    QByteArray svg(
//"<?xml version=\"1.0\" standalone=\"no\"?>\n"
"<svg width=\"2cm\" height=\"2cm\" preserveAspectRatio=\"none\" viewBox=\"0 0 400 400\"\n"
"    xmlns=\"http://www.w3.org/2000/svg\"\n"
"    xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n"
"    baseProfile=\"tiny\" version=\"1.1\">\n"
"  <title>Spheres</title>\n"
"  <desc>Gradient filled spheres with different colors.</desc>\n"
"    <defs>\n"
"      <!-- Create radial gradients for each circle to make them look like\n"
"           spheres. -->\n"
"      <radialGradient id=\"blueSphere\" gradientUnits=\"userSpaceOnUse\"\n"
"                      cx=\"0\" cy=\"0\" r=\"100\" fx=\"-50\" fy=\"-50\">\n"
"        <stop offset=\"0%\" stop-color=\"white\" />\n"
"        <stop offset=\"75%\" stop-color=\"blue\" />\n"
"        <stop offset=\"100%\" stop-color=\"#222244\" />\n"
"      </radialGradient>\n"
"      <radialGradient id=\"redSphere\" gradientUnits=\"userSpaceOnUse\"\n"
"                      cx=\"0\" cy=\"0\" r=\"100\" fx=\"-50\" fy=\"-50\">\n"
"        <stop offset=\"0%\" stop-color=\"white\" />\n"
"        <stop offset=\"75%\" stop-color=\"red\" />\n"
"        <stop offset=\"100%\" stop-color=\"#442222\" />\n"
"      </radialGradient>\n"
"      <radialGradient id=\"greenSphere\" gradientUnits=\"userSpaceOnUse\"\n"
"                      cx=\"0\" cy=\"0\" r=\"100\" fx=\"-50\" fy=\"-50\">\n"
"        <stop offset=\"0%\" stop-color=\"white\" />\n"
"        <stop offset=\"75%\" stop-color=\"green\" />\n"
"        <stop offset=\"100%\" stop-color=\"#113311\" />\n"
"      </radialGradient>\n"
"      <radialGradient id=\"yellowSphere\" gradientUnits=\"userSpaceOnUse\"\n"
"                      cx=\"0\" cy=\"0\" r=\"100\" fx=\"-50\" fy=\"-50\">\n"
"        <stop offset=\"0%\" stop-color=\"white\" />\n"
"        <stop offset=\"75%\" stop-color=\"yellow\" />\n"
"        <stop offset=\"100%\" stop-color=\"#444422\" />\n"
"      </radialGradient>\n"
"      <radialGradient id=\"shadowGrad\" gradientUnits=\"userSpaceOnUse\"\n"
"                      cx=\"0\" cy=\"0\" r=\"100\" fx=\"-50\" fy=\"50\">\n"
"        <stop offset=\"0%\" stop-color=\"black\" stop-opacity=\"1.0\" />\n"
"        <stop offset=\"100%\" stop-color=\"white\" stop-opacity=\"0.0\" />\n"
"      </radialGradient>\n"
"      <!-- Define a shadow for each sphere. -->\n"
"      <circle id=\"shadow\" fill=\"url(#shadowGrad)\" cx=\"0\" cy=\"0\" r=\"100\" />\n"
"    </defs>\n"
"  <g fill=\"#ffee99\" stroke=\"none\" >\n"
"    <rect x=\"0\" y=\"0\" width=\"400\" height=\"400\" />\n"
"  </g>\n"
"  <g fill=\"white\" stroke=\"none\" >\n"
"    <rect x=\"0\" y=\"175\" width=\"400\" height=\"225\" />\n"
"  </g>\n"
"  <g transform=\"translate(200,175)\">\n"
"    <use xlink:href=\"#shadow\" transform=\"translate(25,55) scale(1.0,0.5)\" />\n"
"    <circle fill=\"url(#blueSphere)\" cx=\"0\" cy=\"0\" r=\"100\" />\n"
"  </g>\n"
"  <g transform=\"translate(315,240)\">\n"
"    <g transform=\"scale(0.5,0.5)\">\n"
"      <use xlink:href=\"#shadow\" transform=\"translate(25,55) scale(1.0,0.5)\" />\n"
"      <circle fill=\"url(#redSphere)\" cx=\"0\" cy=\"0\" r=\"100\" />\n"
"    </g>\n"
"  </g>\n"
"  <g transform=\"translate(80,275)\">\n"
"    <g transform=\"scale(0.65,0.65)\">\n"
"      <use xlink:href=\"#shadow\" transform=\"translate(25,55) scale(1.0,0.5)\" />\n"
"      <circle fill=\"url(#greenSphere)\" cx=\"0\" cy=\"0\" r=\"100\" />\n"
"    </g>\n"
"  </g>\n"
"  <g transform=\"translate(255,325)\">\n"
"    <g transform=\"scale(0.3,0.3)\">\n"
"      <use xlink:href=\"#shadow\" transform=\"translate(25,55) scale(1.0,0.5)\" />\n"
"      <circle fill=\"url(#yellowSphere)\" cx=\"0\" cy=\"0\" r=\"100\" />\n"
"    </g>\n"
"  </g>\n"
"</svg>\n" );
    return QString::fromLatin1( svg );
}

void PHISvgItem::ideInit()
{
    _textData.setText( svgDefaultSource() );
}

void PHISvgItem::ideUpdateData()
{
    if ( _textData.isUnparsedStatic() ) setData( DSvgSource, _textData.text().toLatin1() );
    else if ( _textData.isUnparsedTranslated() )
        setData( DSvgSource, _textData.text( page()->currentLang() ).toLatin1() );
    else setData( DSvgSource, QByteArray() );
    Q_ASSERT( _renderer );
    _renderer->load( data( DSvgSource ).toByteArray() );
    if ( _renderer->isValid() ) resize( _renderer->defaultSize() );
    update();
}

void PHISvgItem::ideSetText( const QString &s, const QByteArray &lang )
{
    if ( _textData.isTranslated() ) _textData.setText( s, lang );
    else _textData.setText( s );
    ideUpdateData();
}

QString PHISvgItem::ideText( const QByteArray &lang ) const
{
    if ( _textData.isTranslated() ) return _textData.text( lang );
    return _textData.text();
}

QSizeF PHISvgItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    Q_UNUSED( which )
    Q_UNUSED( constraint )
    return realSize();
}

void PHISvgItem::squeeze()
{
    removeData( DSvgSource );
}

void PHISvgItem::loadItemData( QDataStream &in, int version )
{
    Q_UNUSED( version )
    in >> &_textData;
}

void PHISvgItem::saveItemData( QDataStream &out, int version )
{
    Q_UNUSED( version )
    out << &_textData;
}

void PHISvgItem::initWidget()
{
    _renderer=new QSvgRenderer( this );
    _renderer->load( data( DSvgSource ).toByteArray() );
    if ( _renderer->isValid() ) resize( QSizeF( _renderer->defaultSize() ) );
    update();
}

QImage PHISvgItem::graphicImage( const QByteArray &source ) const
{
    QSvgRenderer renderer;
    renderer.load( source );
    QImage img( realSize().toSize(), QImage::Format_ARGB32_Premultiplied );
    img.fill( 0 );
    QPainter p( &img );
    if ( renderer.isValid() ) renderer.render( &p, rect() );
    else p.fillRect( rect(), Qt::gray );
    return img;
}

void PHISvgItem::paint( QPainter *p, const QRectF &exposed )
{
    Q_UNUSED( exposed )
    if ( _renderer && _renderer->isValid() ) _renderer->render( p, rect() );
    else {
        p->fillRect( rect(), QBrush( Qt::lightGray ) );
        QFont f=font();
        f.setPointSizeF( PHI::adjustedFontSize( 10. ) );
        p->setPen( Qt::darkGray );
        p->drawText( rect(), L1( "SVG" ), QTextOption( Qt::AlignCenter ) );
    }
}

void PHISvgItem::phisCreateData( const PHIDataParser &parser )
{
    setData( DSvgSource, parser.text( &_textData ) );
    if ( _textData.isUnparsedStatic() ) {
        setImagePath( parser.createImage( graphicImage( data( DSvgSource ).toByteArray() ) ) );
    } else if ( _textData.isUnparsedTranslated() ) {
        foreach ( QByteArray l, _textData.langs() ) {
            QByteArray path=parser.createImage( graphicImage( _textData.variant( l ).toByteArray() ), l );
            qDebug() << id() << path << l;
        }
    } else setDirtyFlag( DFText );
}

void PHISvgItem::phisParseData( const PHIDataParser &parser )
{
    if ( Q_UNLIKELY( dirtyFlags() & DFText ) ) {
        setData( DSvgSource, parser.text( &_textData ) );
        // need to create dynamic uncached image
        setImagePath( parser.createImage( graphicImage( data( DSvgSource ).toByteArray() ), PHIData::c(), -1 ) );
    } else {
        if ( !_textData.isUnparsedStatic() ) {
            Q_ASSERT( _textData.isUnparsedTranslated() );
            setData( DSvgSource, _textData.variant( page()->currentLang() ) );
            setImagePath( parser.imagePath( page()->currentLang() ) );
            qDebug() << "imagepath" << page()->currentLang() << imagePath();
        }
    }
}

void PHISvgItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    if ( Q_LIKELY( req->agentFeatures() & PHIRequest::SVG && !hasGraphicEffect() ) ) {
        out+=indent+BL( "<div" );
        htmlBase( req, out, script );
        out+=BL( "\">\n" )+data( DSvgSource ).toByteArray()+'\n'+indent+BL( "</div>\n" );
        script+=BL( "jQuery('#" )+id()+BL( " svg').css({width:'100%',height:'100%'});\n" );
    } else htmlImg( req, out, script, indent );
}

void PHISlideShowItem::ideInit()
{
    PHIAbstractImageBookItem::ideInit();
    QImage img( L1( ":/misc/clipart" ) );
    PHIImageHash hash;
    hash.insert( QByteArray::number( 0 ), img );
    imageBookData()->setImageBook( hash );
    _intervalData.setInteger( 4 );
    _fadeTimeData.setInteger( 2 );
}

void PHISlideShowItem::ideUpdateData()
{
    if ( _intervalData.isUnparsedTranslated() ) setData( DInterval, _intervalData.integer( page()->currentLang() ) );
    else setData( DInterval, _intervalData.integer()>1 ? _intervalData.integer()*1000 : 4000 );
    if ( _fadeTimeData.isUnparsedTranslated() ) setData( DFadeTime, _fadeTimeData.integer( page()->currentLang() ) );
    else setData( DFadeTime, _fadeTimeData.integer()>1 ? _fadeTimeData.integer()*1000 : 2000 );
    PHIAbstractImageBookItem::ideUpdateData();
    updateImages();
}

void PHISlideShowItem::initWidget()
{
    _fadeTimer=new QTimer( this );
    _fadeTimer->setInterval( 50 );
    _pauseTimer=new QTimer( this );
    connect( _fadeTimer, &QTimer::timeout, this, &PHISlideShowItem::fadeTimeout );
    connect( _pauseTimer, &QTimer::timeout, this, &PHISlideShowItem::pauseTimeout );
    updateImages();
}

void PHISlideShowItem::fadeTimeout()
{
    setCurrentOpacity( currentOpacity()-step() );
    if ( currentOpacity()<0 ) {
        setCurrentOpacity( 1. );
        _fadeTimer->stop();
        setCurrentImageNum( currentImageNum()+1 );
        if ( currentImageNum()>=images().count() ) setCurrentImageNum( 0 );
        if ( currentImageNum() < titles().count() ) setTitle( titles().at( currentImageNum() ) );
        else if ( titles().count()>1 ) setTitle( QString() );
        else setTitle( titles().count() ? titles().first() : QString() );
    }
    update();
}

void PHISlideShowItem::pauseTimeout()
{
    Q_ASSERT( _pauseTimer );
    Q_ASSERT( _fadeTimer );
    _fadeTimer->stop();
    _pauseTimer->stop();
    _fadeTimer->start();
    _pauseTimer->start();
}

void PHISlideShowItem::paint( QPainter *painter, const QRectF &exposed )
{
    Q_UNUSED( exposed )
    if ( !images().count() ) {
        QFont f=font();
        f.setPointSizeF( PHI::adjustedFontSize( 10.) );
        painter->setFont( f );
        if ( isIdeItem() ) painter->drawText( rect(), tr( "Image not available" ), QTextOption( Qt::AlignCenter ) );
        else painter->drawText( rect(), tr( "loading..." ), QTextOption( Qt::AlignCenter ) );
        return;
    }
    painter->setRenderHint( QPainter::Antialiasing, false );
    painter->setRenderHint( QPainter::SmoothPixmapTransform, true );
    QImage img1, img2;
    int next=currentImageNum()+1;
    if ( next>=images().count() ) next=0;
    img1=images().value( QByteArray::number( currentImageNum() ) );
    img2=images().value( QByteArray::number( next ) );
    painter->setOpacity( currentOpacity() );
    if ( !img1.isNull() ) painter->drawImage( rect(), img1 );
    painter->setOpacity( 1.-currentOpacity() );
    if ( !img2.isNull() ) painter->drawImage( rect(), img2 );
}

void PHISlideShowItem::updateImages()
{    
    if ( !_pauseTimer || !_fadeTimer ) return;
    _fadeTimer->stop();
    _pauseTimer->stop();
    _pauseTimer->setInterval( fadeIntervalMS() );
    setCurrentImageNum( 0 );
    setStep( 50./static_cast<qreal>(fadeTimeMS()) );
    if ( !isIdeItem() && images().count()>1 ) {
        _pauseTimer->start();
    }
}

void PHISlideShowItem::squeeze()
{
    PHIAbstractImageBookItem::squeeze();
    removeData( DInterval );
    removeData( DFadeTime );
    removeData( DCurrentImageNum );
    removeData( DCurrentOpacity );
    removeData( DCurrentStep );
    removeData( DTitles );
}

void PHISlideShowItem::loadItemData( QDataStream &in, int version )
{
    PHIAbstractImageBookItem::loadItemData( in, version );
    in >> &_intervalData >> &_fadeTimeData;
}

void PHISlideShowItem::saveItemData( QDataStream &out, int version )
{
    PHIAbstractImageBookItem::saveItemData( out, version );
    out << &_intervalData << &_fadeTimeData;
}

void PHICanvasItem::ideInit()
{
    //_textData.setText( svgDefaultSource() );
    resize( 96., 96. );
}

void PHICanvasItem::ideUpdateData()
{
    if ( _textData.isUnparsedStatic() ) setData( DCanvasSource, _textData.text().toLatin1() );
    else if ( _textData.isUnparsedTranslated() )
        setData( DCanvasSource, _textData.text( page()->currentLang() ).toLatin1() );
    else setData( DCanvasSource, QByteArray() );
    update();
}

void PHICanvasItem::ideSetText( const QString &s, const QByteArray &lang )
{
    if ( _textData.isTranslated() ) _textData.setText( s, lang );
    else _textData.setText( s );
    ideUpdateData();
}

QString PHICanvasItem::ideText( const QByteArray &lang ) const
{
    if ( _textData.isTranslated() ) return _textData.text( lang );
    return _textData.text();
}

QSizeF PHICanvasItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    return PHIBaseItem::sizeHint( which, constraint );
}

void PHICanvasItem::squeeze()
{
    removeData( DCanvasSource );
}

void PHICanvasItem::loadItemData( QDataStream &in, int version )
{
    Q_UNUSED( version )
    in >> &_textData;
}

void PHICanvasItem::saveItemData( QDataStream &out, int version )
{
    Q_UNUSED( version )
    out << &_textData;
}

void PHICanvasItem::initWidget()
{
    update();
}

void PHICanvasItem::paint( QPainter *p, const QRectF &exposed )
{
    Q_UNUSED( exposed )
    if ( Q_UNLIKELY( _canvas.isNull() ) ) {
        p->setOpacity( .6 );
        p->fillRect( rect(), QBrush( Qt::lightGray ) );
        QFont f=font();
        f.setPointSizeF( PHI::adjustedFontSize( 10. ) );
        p->setPen( Qt::darkGray );
        p->drawText( rect(), L1( "Canvas" ), QTextOption( Qt::AlignCenter ) );
    } else {
        p->setClipRect( rect() );
        p->drawImage( 0, 0, _canvas );
    }
}

void PHICanvasItem::slotSizeChanged( const QSizeF &s )
{
    if ( _ctx2D ) _ctx2D->setSize( s.toSize() );
}

QScriptValue PHICanvasItem::getContext( const QScriptValue &v )
{
    if ( v.toString().toUpper()!=L1( "2D" ) ) return QScriptValue( QScriptValue::UndefinedValue );
    QScriptEngine *e=page()->scriptEngine();
    if ( !e ) return QScriptValue( QScriptValue::UndefinedValue );
    if ( !_ctx2D ) {
        setDirtyFlag( DFCustom1 ); // if referenced in ServerScript we create an image
        _ctx2D=new PHIContext2D( this );
        _ctx2D->setSize( realSize().toSize() );
        if ( isServerItem() ) _ctx2D->setServerMode();
        connect( _ctx2D, &PHIContext2D::changed, this, &PHICanvasItem::imageChanged );
        connect( this, &PHIBaseItem::sizeChanged, this, &PHICanvasItem::slotSizeChanged );
        qScriptRegisterMetaType( e, domRectToScriptValue, domRectFromScriptValue );
        qScriptRegisterMetaType( e, canvasGradientToScriptValue, canvasGradientFromScriptValue );
    }
    return context2DToScriptValue( e, _ctx2D );
}

void PHICanvasItem::phisCreateData( const PHIDataParser &parser )
{
    setData( DCanvasSource, parser.text( &_textData ) );
    if ( !_textData.isUnparsedStatic() ) setDirtyFlag( DFText );
}

void PHICanvasItem::phisParseData( const PHIDataParser &parser )
{
    if ( Q_UNLIKELY( dirtyFlags() & DFText ) ) setData( DCanvasSource, parser.text( &_textData ) );
}

void PHICanvasItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    if ( dirtyFlags() & DFCustom1 && !_canvas.isNull() ) { // image created via ServerScript
        QByteArray imgid=PHIDataParser::createImage( req, this, _canvas, PHIData::c(), -1 );
        setImagePath( imgid );
        return htmlImg( req, out, script, indent );
    }
    if ( Q_LIKELY( req->agentFeatures() & PHIRequest::Canvas ) ) {
        out+=indent+BL( "<canvas width=\"" )+QByteArray::number( qRound(realWidth()) )
            +BL( "\" height=\"" )+QByteArray::number( qRound(realHeight()) )+'"';
        htmlBase( req, out, script );
        out+=BL( "\"></canvas>\n" );
        QByteArray arr=data( DCanvasSource ).toByteArray();
        if ( !arr.isEmpty() ) script+=arr+'\n';
    } else { // we try to create an image from script
        QScriptEngine *engine=page()->scriptEngine();
        if ( !engine ) return;
        QString t=realText();
        if ( t.isEmpty() ) return;
        qScriptRegisterMetaType( engine, domRectToScriptValue, domRectFromScriptValue );
        qScriptRegisterMetaType( engine, canvasGradientToScriptValue, canvasGradientFromScriptValue );
        QScriptValue res=page()->scriptEngine()->evaluate( t );
        if ( Q_UNLIKELY( res.isError() ) ) {
            QStringList list=engine->uncaughtExceptionBacktrace();
            QString tmp=tr( "Parse error in canvas '%1' in page '%2', line: %3" )
                .arg( name() ).arg( req->canonicalFilename() )
                .arg( engine->uncaughtExceptionLineNumber() )+
                QString::fromLatin1( PHI::nl() )+res.toString();
            tmp+=QString::fromLatin1( PHI::nl()+PHI::nl() )+list.join( QString::fromLatin1( PHI::nl() ) );
            req->responseRec()->log( PHILOGERR, PHIRC_SCRIPT_PARSE_ERROR, tmp );
        }
        if ( _canvas.isNull() ) return;
        QByteArray imgid=PHIDataParser::createImage( req, this, _canvas, PHIData::c(), -1 );
        setImagePath( imgid );
        return htmlImg( req, out, script, indent );
    }
}

QSizeF PHISponsorItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    Q_UNUSED( which )
    Q_UNUSED( constraint )
    return QSizeF( _image.size() );
}

void PHISponsorItem::squeeze()
{
    setXRotation( 0 );
    setYRotation( 0 );
    setHSkew( 0 );
    setVSkew( 0 );
}

void PHISponsorItem::paint( QPainter *painter, const QRectF &exposed )
{
    Q_UNUSED( exposed )
    painter->setRenderHint( QPainter::SmoothPixmapTransform );
    painter->drawImage( 0, 0, _image );
}

void PHISponsorItem::phisCreateData( const PHIDataParser &parser )
{
    setImagePath( parser.createImage( _image ) );
}

void PHISponsorItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    Q_UNUSED( script )
    if ( Q_UNLIKELY( req->agentFeatures() & PHIRequest::IE678 ) ) {
        out+=indent+BL( "<div style=\"filter:progid:DXImageTransform.Microsoft.AlphaImageLoader(src='phi.phis?i=" )
            +imagePath()+BL( "&t=1');z-index:" )+QByteArray::number( PHI::maxZIndex()+1 )+BL( ";position:absolute;left:" )
            +QByteArray::number( qRound(realX()) )+BL( "px;top:" )
            +QByteArray::number( qRound(realY()) )+BL( "px;\"></div>\n" );
    } else {
        out+=indent+BL( "<img style=\"z-index:" )+QByteArray::number( PHI::maxZIndex()+1 )+BL( ";position:absolute;left:" )
            +QByteArray::number( qRound(realX()) )+BL( "px;top:" )+QByteArray::number( qRound(realY()) )
            +BL( "px;\" src=\"phi.phis?i=" )+imagePath()+BL( "&t=1\">\n" );
    }
}
