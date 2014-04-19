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
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QPainter>
#include <QTimer>
#include <QSvgRenderer>
#include <QSpinBox>
#include <QToolButton>
#include <QLabel>
#include <QHBoxLayout>
#include "phiimageitems.h"
#include "phibasepage.h"
#include "phidataparser.h"
#include "phirequest.h"
#include "phicontext2d.h"
#include "phiimagerequest.h"
#include "phicolorconfig.h"

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
    Q_ASSERT( _renderer );
    _renderer->load( svgDefaultSource().toLatin1() );
    if ( _renderer->isValid() ) resize( QSizeF( _renderer->defaultSize() ) );
    update();
}

void PHISvgItem::ideUpdateData()
{
    if ( _textData.isUnparsedStatic() ) setData( DSvgSource, _textData.text().toLatin1() );
    else if ( _textData.isUnparsedTranslated() )
        setData( DSvgSource, _textData.text( page()->currentLang() ).toLatin1() );
    else setData( DSvgSource, QByteArray() );
    Q_ASSERT( _renderer );
    _renderer->load( PHIBaseItem::data( DSvgSource ).toByteArray() );
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
}

QScriptValue PHISvgItem::data( const QScriptValue &v )
{
    if ( !isServerItem() ) return scriptEngine()->undefinedValue();
    if ( !v.isValid() ) return realText();
    setText( v.toString() );
    return self();
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

bool PHISvgItem::paint( QPainter *p, const QRectF &exposed )
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
    return true;
}

void PHISvgItem::phisCreateData( const PHIDataParser &parser )
{
    setData( DSvgSource, parser.text( &_textData ) );
    if ( _textData.isUnparsedStatic() ) {
        setImagePath( parser.createImage( graphicImage( PHIBaseItem::data( DSvgSource ).toByteArray() ) ) );
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
        setImagePath( parser.createImage( graphicImage( PHIBaseItem::data( DSvgSource ).toByteArray() ), PHIData::c(), -1 ) );
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
        htmlInitItem( script );
        out+=indent+BL( "<div" );
        htmlBase( req, out, script );
        out+=BL( "\">\n" )+PHIBaseItem::data( DSvgSource ).toByteArray()+'\n'+indent+BL( "</div>\n" );
        script+=BL( "jQuery('#" )+id()+BL( " svg').css({width:'100%',height:'100%'});\n" );
    } else {
        htmlImg( req, out, script, indent );
    }
}

void PHISvgItem::clientInitData()
{
    Q_ASSERT( _renderer );
    _renderer->load( realText().toUtf8() );
    if ( _renderer->isValid() ) resize( QSizeF( _renderer->defaultSize() ) );
    update();
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
        if ( currentImageNum()>=realImages().count() ) setCurrentImageNum( 0 );
        if ( currentImageNum() < realTitles().count() ) setTitle( realTitles().at( currentImageNum() ) );
        else if ( realTitles().count()>1 ) setTitle( QString() );
        else setTitle( realTitles().count() ? realTitles().first() : QString() );
    }
    update();
}

PHIConfigWidget *PHISlideShowItem::ideConfigWidget()
{
    return new PHISlideShowConfig( this );
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

QScriptValue PHISlideShowItem::stop()
{
    if ( _pauseTimer ) _pauseTimer->stop();
    return PHIBaseItem::stop();
}

QScriptValue PHISlideShowItem::start()
{
    pauseTimeout();
    return self();
}

QScriptValue PHISlideShowItem::display( const QScriptValue &c )
{
    if ( !c.isValid() ) return currentImageNum();
    if ( _fadeTimer ) _fadeTimer->stop();
    if ( _pauseTimer ) _pauseTimer->stop();
    qint32 num=c.toInt32();
    if ( num>=realImages().count() ) num=0;
    setCurrentImageNum( 0 );
    setCurrentOpacity( 1. );
    update();
    return self();
}

bool PHISlideShowItem::paint( QPainter *painter, const QRectF &exposed )
{
    const PHIImageHash hash=realImages();
    if ( !hash.count() ) {
        return PHIAbstractImageBookItem::paint( painter, exposed );
    }
    if ( !hash.value( "0" ).isNull() && hash.value( "1" ).isNull() ) {
        painter->drawImage( rect(), hash.value( "0" ) );
        return true;
    }
    painter->setRenderHint( QPainter::Antialiasing, false );
    painter->setRenderHint( QPainter::SmoothPixmapTransform, true );
    QImage img1, img2;
    int next=currentImageNum()+1;
    if ( next>=hash.count() ) next=0;
    img1=hash.value( QByteArray::number( currentImageNum() ) );
    img2=hash.value( QByteArray::number( next ) );
    painter->setOpacity( currentOpacity() );
    if ( !img1.isNull() ) painter->drawImage( rect(), img1 );
    painter->setOpacity( 1.-currentOpacity() );
    if ( !img2.isNull() ) painter->drawImage( rect(), img2 );
    return true;
}

void PHISlideShowItem::updateImages()
{    
    if ( !isClientItem() ) return;
    Q_ASSERT( _fadeTimer );
    Q_ASSERT( _pauseTimer );
    _fadeTimer->stop();
    _pauseTimer->stop();
    _pauseTimer->setInterval( realFadeIntervalMS() );
    setCurrentImageNum( 0 );
    setStep( 50./static_cast<qreal>(realFadeTimeMS()) );
    if ( !realImages().value( "0" ).isNull() && !realImages().value( "1" ).isNull() ) {
        _pauseTimer->start();
    }
    update();
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

void PHISlideShowItem::phisCreateData( const PHIDataParser &parser )
{
    PHIAbstractImageBookItem::phisCreateData( parser );
    setFadeInterval( parser.text( &_intervalData ).toInt() );
    if ( !_intervalData.isUnparsedStatic() ) setDirtyFlag( DFInt1 );
    setFadeTime( parser.text( &_fadeTimeData ).toInt() );
    if ( !_fadeTimeData.isUnparsedStatic() ) setDirtyFlag( DFInt2 );
}

void PHISlideShowItem::phisParseData( const PHIDataParser &parser )
{
    PHIAbstractImageBookItem::phisParseData( parser );
    if ( dirtyFlags() & DFInt1 ) setFadeInterval( parser.text( &_intervalData ).toInt() );
    if ( dirtyFlags() & DFInt2 ) setFadeTime( parser.text( &_fadeTimeData ).toInt() );
    setData( DTmpTitle, data( DTitle ) );
    removeData( DTitle ); // suppress creating title for container
}

void PHISlideShowItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    QByteArray ftime=QByteArray::number( realFadeTimeMS() );
    if ( realFadeIntervalMS() < realFadeTimeMS() ) {
        req->responseRec()->log( PHILOGERR, PHIRC_OBJ_ACCESS_ERROR,
            tr( "Fading time must be lesser or equal then fading interval for item '%1' in page '%2'." )
                .arg( name() ).arg( req->canonicalFilename() ) );
        ftime=QByteArray::number( realFadeIntervalMS() );
    }
    htmlImages( req, out, script, indent );
    script+=BL( "$$slideShow('" )+id()+BL( "'," )
        +QByteArray::number( imagePathes().count() )+BL( ").titles('")
        +data( DTmpTitle ).toByteArray()+BL( "');\n" );
    if ( realFadeIntervalMS()!=4000 ) script+=BL( "$('" )+id()+BL( "').fadeIntervalMS(" )
        +QByteArray::number( realFadeIntervalMS() )+BL( ");\n" );
    if ( realFadeTimeMS()!=2000 ) script+=BL( "$('" )+id()+BL( "').fadeTimeMS(" )
        +QByteArray::number( realFadeTimeMS() )+BL( ");\n" );
}

QScriptValue PHISlideShowItem::fadeInterval( const QScriptValue &fi )
{
    if ( !fi.isValid() ) return realFadeInterval();
    setFadeInterval( fi.toInt32() );
    return self();
}

QScriptValue PHISlideShowItem::fadeTime( const QScriptValue &ft )
{
    if ( !ft.isValid() ) return realFadeTime();
    setFadeTime( ft.toInt32() );
    return self();
}

QScriptValue PHISlideShowItem::fadeIntervalMS( const QScriptValue &fi )
{
    if ( !fi.isValid() ) return realFadeIntervalMS();
    setFadeIntervalMS( fi.toInt32() );
    return self();
}

QScriptValue PHISlideShowItem::fadeTimeMS( const QScriptValue &ft )
{
    if ( !ft.isValid() ) return realFadeTimeMS();
    setFadeTimeMS( ft.toInt32() );
    return self();
}

QScriptValue PHISlideShowItem::titles( const QScriptValue &t )
{
    if ( !t.isValid() ) {
        QScriptValue arr=page()->scriptEngine()->newArray(realTitles().count());
        for ( quint32 i=0; i < static_cast<quint32>(realTitles().count()); i++ ) {
            arr.setProperty( i, realTitles().at( i ) );
        }
        return arr;
    }
    if ( t.isArray() ) {
        QStringList l;
        for ( quint32 i=0; i<t.property( SL( "length" ) ).toUInt32(); i++ ) {
            l.append( t.property( i ).toString() );
        }
        setTitles( l );
    } else {
        setTitleList( t.toString() );
    }
    return self();
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
    if ( isChild() ) return realSize();
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

bool PHICanvasItem::paint( QPainter *p, const QRectF &exposed )
{
    Q_UNUSED( exposed )
    QImage img=realImage();
    if ( Q_UNLIKELY( img.isNull() ) ) {
        p->setOpacity( .6 );
        p->fillRect( rect(), QBrush( Qt::lightGray ) );
        QFont f=font();
        f.setPointSizeF( PHI::adjustedFontSize( 10. ) );
        p->setPen( Qt::darkGray );
        p->drawText( rect(), L1( "Canvas" ), QTextOption( Qt::AlignCenter ) );
    } else {
        p->setClipRect( rect() );
        p->drawImage( 0, 0, img );
    }
    return true;
}

void PHICanvasItem::slotSizeChanged( const QSizeF &s )
{
    if ( _ctx2D ) _ctx2D->setSize( s.toSize() );
}

QScriptValue PHICanvasItem::getContext( const QScriptValue &v )
{
    if ( v.toString().toUpper()!=L1( "2D" ) ) return QScriptValue( QScriptValue::UndefinedValue );
    if ( !scriptEngine() || isImage() ) return QScriptValue( QScriptValue::UndefinedValue );
    if ( !_ctx2D ) {
        if ( data( DIsImage ).toBool() ) return QScriptValue( QScriptValue::UndefinedValue );
        setDirtyFlag( DFCustom1 ); // if referenced in ServerScript we create an image
        _ctx2D=new PHIContext2D( this );
        _ctx2D->setSize( realSize().toSize() );
        if ( isServerItem() ) {
            _ctx2D->setServerMode();
            setData( DIsImage, true );
        }
        connect( _ctx2D, &PHIContext2D::changed, this, &PHICanvasItem::setImage );
        connect( this, &PHIBaseItem::sizeChanged, this, &PHICanvasItem::slotSizeChanged );
        // @todo: move to script object in server and client:
        qScriptRegisterMetaType( scriptEngine(), domRectToScriptValue, domRectFromScriptValue );
        qScriptRegisterMetaType( scriptEngine(), canvasGradientToScriptValue, canvasGradientFromScriptValue );
    }
    return context2DToScriptValue( scriptEngine(), _ctx2D );
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
    if ( dirtyFlags() & DFCustom1 && !realImage().isNull() ) { // image created via ServerScript
        setImagePath( PHIDataParser::createImage( req, this, realImage(), PHIData::c(), -1 ) );
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
        if ( !scriptEngine() ) return;
        QString t=realText();
        if ( t.isEmpty() ) {
            // IE678 can try to use excanvas.js
            out+=indent+BL( "<canvas width=\"" )+QByteArray::number( qRound(realWidth()) )
                +BL( "\" height=\"" )+QByteArray::number( qRound(realHeight()) )+'"';
            htmlBase( req, out, script );
            out+=BL( "\"></canvas>\n" );
            return;
        }
        // @todo: move to script object in server and client:
        qScriptRegisterMetaType( scriptEngine(), domRectToScriptValue, domRectFromScriptValue );
        qScriptRegisterMetaType( scriptEngine(), canvasGradientToScriptValue, canvasGradientFromScriptValue );
        QScriptValue res=scriptEngine()->evaluate( t );
        if ( Q_UNLIKELY( res.isError() ) ) {
            QStringList list=scriptEngine()->uncaughtExceptionBacktrace();
            QString tmp=tr( "Parse error in canvas '%1' in page '%2', line: %3" )
                .arg( name() ).arg( req->canonicalFilename() )
                .arg( scriptEngine()->uncaughtExceptionLineNumber() )+
                QString::fromLatin1( PHI::nl() )+res.toString();
            tmp+=QString::fromLatin1( PHI::nl()+PHI::nl() )+list.join( QString::fromLatin1( PHI::nl() ) );
            req->responseRec()->log( PHILOGERR, PHIRC_SCRIPT_PARSE_ERROR, tmp );
        }
        if ( realImage().isNull() ) return;
        QByteArray imgid=PHIDataParser::createImage( req, this, realImage(), PHIData::c(), -1 );
        setImagePath( imgid );
        return htmlImg( req, out, script, indent );
    }
}

PHIWID PHICanvasItem::htmlHeaderExtension( const PHIRequest *req, QByteArray &header ) const
{
    if ( Q_UNLIKELY( req->agentFeatures() & PHIRequest::IE678 ) ) {
        header+=BL( "<script type=\"text/javascript\" src=\"phi.phis?j=excanvas\"></script>\n" );
        return Canvas;
    }
    return Unknown;
}

void PHICanvasItem::clientInitData()
{
    update();
}

QSizeF PHISponsorItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    Q_UNUSED( which )
    Q_UNUSED( constraint )
    return QSizeF( _image.size() );
}

void PHISponsorItem::squeeze()
{
    removeData( DOpacity );
    setXRotation( 0 );
    setYRotation( 0 );
    setHSkew( 0 );
    setVSkew( 0 );
}

bool PHISponsorItem::paint( QPainter *painter, const QRectF &exposed )
{
    Q_UNUSED( exposed )
    painter->setRenderHint( QPainter::SmoothPixmapTransform );
    painter->drawImage( 0, 0, _image );
    return true;
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
            +BL( "px;opacity:1\" alt=\"Phisys AG Switzerland\" src=\"phi.phis?i=" )+imagePath()+BL( "&amp;t=1\">\n" );
    }
}

PHISlideShowConfig::PHISlideShowConfig( PHIBaseItem *it, QWidget *parent )
    : PHIConfigWidget( it, parent )
{
    _orgIntervalData=new PHIIntData();
    _orgFadeData=new PHIIntData();
    _intervalTool=new QToolButton();
    _intervalTool->setArrowType( Qt::RightArrow );
    connect( _intervalTool, &QToolButton::clicked, this, &PHISlideShowConfig::intervalToolClicked );
    _fadeTool=new QToolButton();
    _fadeTool->setArrowType( Qt::RightArrow );
    connect( _fadeTool, &QToolButton::clicked, this, &PHISlideShowConfig::fadeToolClicked );
    _intervalSpin=new QSpinBox();
    _intervalSpin->setRange( 1, 600 );
    _intervalSpin->setSingleStep( 1 );
    _fadeSpin=new QSpinBox();
    _fadeSpin->setRange( 1, 600 );
    _fadeSpin->setSingleStep( 1 );
    _intervalLabel=new QLabel( tr( "Interval time" ) );
    _intervalLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    QVBoxLayout *vbox=new QVBoxLayout();
    vbox->setContentsMargins( 24, 24, 24, 24 );
    QLabel *label=new QLabel();
    label->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding );
    vbox->addWidget( label );
    label=new QLabel( tr( "Note: the interval time value must be higher than the fade time value." ) );
    vbox->addWidget( label );
    label=new QLabel();
    label->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    QHBoxLayout *l=new QHBoxLayout();
    l->setContentsMargins( 12, 6, 12, 6 );
    l->setSpacing( 6 );
    l->addWidget( label );
    l->addWidget( _intervalLabel );
    l->addWidget( _intervalSpin );
    l->addWidget( _intervalTool );
    label=new QLabel();
    label->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    _fadeLabel=new QLabel( tr( "Fade time" ) );
    _fadeLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    l->addWidget( _fadeLabel );
    l->addWidget( _fadeSpin );
    l->addWidget( _fadeTool );
    l->addWidget( label );
    vbox->addItem( l );
    label=new QLabel();
    label->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding );
    vbox->addWidget( label );
    setLayout( vbox );
    PHISlideShowItem *sit=qobject_cast<PHISlideShowItem*>(it);
    _intervalSpin->setEnabled( sit->intData_2()->isUnparsedStatic() );
    _intervalLabel->setEnabled( sit->intData_2()->isUnparsedStatic() );
    _fadeSpin->setEnabled( sit->intData_1()->isUnparsedStatic() );
    _fadeLabel->setEnabled( sit->intData_1()->isUnparsedStatic() );
    _intervalSpin->setValue( sit->intData_2()->isUnparsedStatic() ? sit->intData_2()->integer() : 4 );
    _fadeSpin->setValue( sit->intData_1()->isUnparsedStatic() ? sit->intData_1()->integer() : 2 );
    *_orgIntervalData=*sit->intData_2();
    *_orgFadeData=*sit->intData_1();
    connect( _intervalSpin, SIGNAL(valueChanged(int)), this, SLOT(intervalTimeChanged(int)) );
    connect( _fadeSpin, SIGNAL(valueChanged(int)), this, SLOT(fadeTimeChanged(int)) );
}

PHISlideShowConfig::~PHISlideShowConfig()
{
    delete _orgIntervalData;
    delete _orgFadeData;
}

void PHISlideShowConfig::intervalTimeChanged( int v )
{
    PHISlideShowItem *sit=qobject_cast<PHISlideShowItem*>(item());
    sit->intData_2()->setInteger( v );
    sit->privateUpdateData();
    repaint();
}

void PHISlideShowConfig::fadeTimeChanged( int v )
{
    PHISlideShowItem *sit=qobject_cast<PHISlideShowItem*>(item());
    sit->intData_1()->setInteger( v );
    sit->privateUpdateData();
    repaint();
}

PHIConfigData PHISlideShowConfig::originalData() const
{
    PHISlideShowItem *sit=qobject_cast<PHISlideShowItem*>(item());
    PHIConfigData cd;
    if ( *sit->intData_2()!=*_orgIntervalData ) cd.insert( sit->intData_2(), _orgIntervalData );
    if ( *sit->intData_1()!=*_orgFadeData ) cd.insert( sit->intData_1(), _orgFadeData );
    return cd;
}

void PHISlideShowConfig::intervalToolClicked()
{
    PHISlideShowItem *it=qobject_cast<PHISlideShowItem*>(item());
    emit requestTextConfig( it->intData_2() );
    _intervalSpin->setEnabled( it->intData_2()->isUnparsedStatic() );
    _intervalLabel->setEnabled( it->intData_2()->isUnparsedStatic() );
    _intervalSpin->blockSignals( true );
    if ( it->intData_2()->isUnparsedStatic() ) _intervalSpin->setValue( it->intData_2()->integer() );
    else _intervalSpin->setValue( 4 );
    _intervalSpin->blockSignals( false );
    it->privateUpdateData();
}

void PHISlideShowConfig::fadeToolClicked()
{
    PHISlideShowItem *it=qobject_cast<PHISlideShowItem*>(item());
    emit requestTextConfig( it->intData_1() );
    _fadeSpin->setEnabled( it->intData_1()->isUnparsedStatic() );
    _fadeLabel->setEnabled( it->intData_1()->isUnparsedStatic() );
    _fadeSpin->blockSignals( true );
    if ( it->intData_1()->isUnparsedStatic() ) _fadeSpin->setValue( it->intData_1()->integer() );
    else _fadeSpin->setValue( 2 );
    _fadeSpin->blockSignals( false );
    it->privateUpdateData();
}

bool PHISlideShowConfig::storeData()
{
    PHISlideShowItem *it=qobject_cast<PHISlideShowItem*>(item());
    bool changed=false;
    if ( *_orgIntervalData!=*it->intData_2() ) changed=true;
    if ( *_orgFadeData!=*it->intData_1() ) changed=true;
    return changed;
}

void PHIRolloverItem::ideInit()
{
    setLine( 0 ); // no border
    setPattern( 1 ); // solid pattern
    setColor( PHIPalette::Foreground, PHIPalette::WindowText, page()->phiPalette().color( PHIPalette::WindowText ) );
    setColor( PHIPalette::Background, PHIPalette::Window, page()->phiPalette().color( PHIPalette::Window ) );
    setColor( PHIPalette::Hover, PHIPalette::HighlightText, page()->phiPalette().color( PHIPalette::HighlightText ) );
    setColor( PHIPalette::HoverBackground, PHIPalette::Window, page()->phiPalette().color( PHIPalette::Window ) );
    _textData.setText( L1( "Rollover" ) );
}

// called form IDE only:
void PHIRolloverItem::ideSetText( const QString &t, const QByteArray &lang )
{
    if ( _textData.isTranslated() ) {
        _textData.setText( t, lang );
        setText( t );
    } else if ( _textData.isStatic() ) {
        _textData.setSource( PHIData::Translated );
        _textData.setText( t, lang );
        setText( t );
    } else setText( _textData.text() );
    if ( isChild() ) {
        PHIBaseItem *it=page()->findItem( parentId() );
        PHIAbstractLayoutItem *lit=qobject_cast<PHIAbstractLayoutItem*>(it);
        if ( lit ) lit->invalidateLayout();
    }
}

void PHIRolloverItem::ideUpdateData()
{
    PHIAbstractShapeItem::ideUpdateData();
    if ( _textData.isTranslated() ) {
        setText( _textData.text( page()->currentLang() ) );
    } else {
        setText( _textData.text() );
    }
    if ( _imageBookData.isTranslated() ) {
        setImages( _imageBookData.imageBook( page()->currentLang() ) );
    } else {
        setImages( _imageBookData.imageBook() );
    }
    if ( isChild() ) {
        PHIBaseItem *it=page()->findItem( parentId() );
        PHIAbstractLayoutItem *lit=qobject_cast<PHIAbstractLayoutItem*>(it);
        if ( lit ) lit->invalidateLayout();
    }
}

QString PHIRolloverItem::ideText( const QByteArray &lang ) const
{
    if ( _textData.isTranslated() ) return _textData.text( lang );
    return _textData.text();
}

QColor PHIRolloverItem::realHoverColor() const
{
    if ( _hoverColorRole==PHIPalette::Custom ) return data( DHoverColor, QColor( Qt::black ) ).value<QColor>();
    if ( page() ) return page()->phiPalette().color( _hoverColorRole );
    return Qt::black;
}

QColor PHIRolloverItem::realHoverBgColor() const
{
    if ( _hoverBgColorRole==PHIPalette::Custom ) return data( DHoverBgColor, QColor( Qt::transparent ) ).value<QColor>();
    if ( page() ) return page()->phiPalette().color( _hoverBgColorRole );
    return Qt::transparent;
}

QColor PHIRolloverItem::colorForRole( PHIPalette::ItemRole role ) const
{
    if ( role==PHIPalette::Foreground ) return realColor();
    if ( role==PHIPalette::Background ) return realOutlineColor();
    if ( role==PHIPalette::Hover ) return realHoverColor();
    if ( role==PHIPalette::HoverBackground ) return realHoverBgColor();
    return QColor();
}

PHIPalette::ColorRole PHIRolloverItem::colorRole( PHIPalette::ItemRole role ) const
{
    if ( role==PHIPalette::Foreground ) return PHIAbstractShapeItem::colorRole( role );
    if ( role==PHIPalette::Background ) return PHIAbstractShapeItem::colorRole( role );
    if ( role==PHIPalette::Hover ) return _hoverColorRole;
    if ( role==PHIPalette::HoverBackground ) return _hoverBgColorRole;
    return PHIPalette::NoRole;
}

void PHIRolloverItem::setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col )
{
    if ( ir==PHIPalette::Foreground ) PHIAbstractShapeItem::setColor( ir, cr, col );
    if ( ir==PHIPalette::Background ) PHIAbstractShapeItem::setColor( ir, cr, col );
    if ( ir==PHIPalette::Hover ) {
        _hoverColorRole=cr;
        if ( cr==PHIPalette::Custom ) setData( DHoverColor, col );
    }
    if ( ir==PHIPalette::HoverBackground ) {
        _hoverBgColorRole=cr;
        if ( cr==PHIPalette::Custom ) setData( DHoverBgColor, col );
    }
    update();
}

QSizeF PHIRolloverItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    if ( which==Qt::MinimumSize ) return QSizeF( 20., 20. );
    if ( which==Qt::PreferredSize ) {
        QFont f=font();
        f.setPointSizeF( PHI::adjustedFontSize( font().pointSizeF() ) );
        QFontMetricsF m( f );
        qreal minWidth=64.;
        if ( page() ) {
            foreach ( QString l, page()->languages() ) {
                l=ideText( l.toLatin1() );
                if ( m.width( l )>minWidth ) minWidth=qRound(m.width( l ));
            }
        }
        return QSizeF( minWidth, qMax( 22., qRound(m.height())+8. ) );
    }
    return PHIBaseItem::sizeHint( which, constraint );
}

void PHIRolloverItem::drawShape( QPainter *p, const QRectF &exposed )
{
    Q_UNUSED( exposed )
    p->setRenderHint( QPainter::TextAntialiasing );
    p->setRenderHint( QPainter::SmoothPixmapTransform );
    p->setRenderHint( QPainter::Antialiasing );
    if ( isClientItem() ) {
        QImage img;
        if ( _hover ) img=realImages().value( "1" );
        else img=realImages().value( "0" );
        if ( !img.isNull() ) p->drawImage( rect(), img );
        return;
    }
    QFont f=font();
    f.setPointSizeF( PHI::adjustedFontSize( f.pointSizeF() ) );
    p->setFont( f );
    QBrush brush=p->brush();
    QImage img;
    if ( _hover ) {
        img=realImages().value( "1" );
        if ( realPattern()!=15 ) {
            brush.setColor( realHoverBgColor() );
            p->setBrush( brush );
        }
    } else {
        img=realImages().value( "0" );
        if ( realPattern()!=15 ) {
            brush.setColor( realOutlineColor() );
            p->setBrush( brush );
        }
    }
    p->drawRect( rect() ); // draw background (incl. border if any)
    if ( _hover ) p->setPen( QPen( realHoverColor() ) );
    else p->setPen( QPen( realColor() ) );
    if ( !img.isNull() ) p->drawImage( rect(), img );
    p->drawText( rect(), realText(), QTextOption( Qt::AlignCenter ) );
}

void PHIRolloverItem::squeeze()
{
    PHIAbstractShapeItem::squeeze();
    removeData( DText );
    removeData( DImages );
    if ( _hoverColorRole!=PHIPalette::Custom ) removeData( DHoverColor );
    if ( _hoverBgColorRole!=PHIPalette::Custom ) removeData( DHoverBgColor );
}

void PHIRolloverItem::loadItemData( QDataStream &in, int version )
{
    PHIAbstractShapeItem::loadItemData( in, version );
    quint8 reserved, hoverColor, hoverBgColor;
    in >> &_textData >> hoverColor >> hoverBgColor >> reserved >> reserved;
    _hoverColorRole=static_cast<PHIPalette::ColorRole>(hoverColor);
    _hoverBgColorRole=static_cast<PHIPalette::ColorRole>(hoverBgColor);
    QByteArray arr;
    in >> arr;
    arr=qUncompress( arr );
    QDataStream ds( &arr, QIODevice::ReadOnly );
    ds.setVersion( PHI_DSV2 );
    ds >> &_imageBookData;
}

void PHIRolloverItem::saveItemData( QDataStream &out, int version )
{
    PHIAbstractShapeItem::saveItemData( out, version );
    quint8 reserved( 0 );
    out << &_textData << static_cast<quint8>(_hoverColorRole)
        << static_cast<quint8>(_hoverBgColorRole) << reserved << reserved;
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

QImage PHIRolloverItem::createImage( const QString &t, const QImage &img, const QColor &col, const QColor &bgCol ) const
{
    QFont f=font();
    f.setPointSizeF( PHI::adjustedFontSize( f.pointSizeF() ) );
    QImage dest( realSize().toSize(), QImage::Format_ARGB32_Premultiplied );
    dest.fill( bgCol );
    QPainter p( &dest );
    p.setRenderHint( QPainter::SmoothPixmapTransform );
    p.setRenderHint( QPainter::Antialiasing );
    p.setRenderHint( QPainter::TextAntialiasing );
    if ( !img.isNull() ) p.drawImage( rect(), img );
    p.setPen( col );
    p.setFont( f );
    if ( !t.isEmpty() ) p.drawText( rect(), t, QTextOption( Qt::AlignCenter ) );
    p.end();
    return dest;
}

QString PHIRolloverItem::resolvePath( const PHIRequest *req, const QString &path, QByteArray &lang )
{
    QString fn=QDir::fromNativeSeparators( path );
    if ( fn.startsWith( QLatin1Char( '/' ) ) ) fn=req->documentRoot()+fn;
    else fn=QFileInfo( req->canonicalFilename() ).absolutePath()+QLatin1Char( '/' )+fn;
    if ( fn.indexOf( SL( "/C/" ) )>-1 ) {
        QString l=QLatin1Char( '/' )+QString::fromLatin1( lang )+QLatin1Char( '/' );
        fn.replace( SL( "/C/" ), l );
        if ( QFileInfo( fn ).exists() ) return fn;
        else return fn.replace( l, SL( "/C/" ) );
    }
    lang=PHIData::c();
    return fn;
}

void PHIRolloverItem::phisCreateData( const PHIDataParser &parser )
{
    setData( DText, parser.text( &_textData ) );
    if ( !_textData.isUnparsedStatic() && !_textData.isUnparsedTranslated() ) setDirtyFlag( DFText );
    parser.createImages( &_imageBookData );
    if ( !_imageBookData.isUnparsedStatic() && !_imageBookData.isUnparsedTranslated()
         && _imageBookData.source()!=PHIData::File ) setDirtyFlag( DFCustom1 );
    if ( dirtyFlags() & DFText || dirtyFlags() & DFCustom1 ) return;
    // cacheable static or translated images:
    foreach( QString lang, page()->languages() ) {
        QByteArray l=lang.toLatin1();
        QString text=_textData.text( l );
        if ( text.isEmpty() ) text=_textData.text();
        QStringList pathes;
        if ( _imageBookData.source()==PHIData::File ) {
            if ( _imageBookData.fileNames().count() ) pathes << resolvePath( parser.request(), _imageBookData.fileNames().at( 0 ), l );
            else pathes.append( QString() );
            if ( _imageBookData.fileNames().count()>1 ) pathes << resolvePath( parser.request(), _imageBookData.fileNames().at( 1 ), l );
            else pathes.append( QString() );
        } else {
            PHIByteArrayList list=_imageBookData.imageIds( l );
            if ( list.isEmpty() ) {
                list=_imageBookData.imageIds();
                //l=PHIData::c();
            }
            QString imgDir=parser.request()->imgDir()+QDir::separator();
            if ( list.count() ) pathes.append( imgDir+QString::fromUtf8( list.at( 0 ) )+L1( ".png" ) );
            else pathes.append( QString() );
            if ( list.count()>1 ) pathes.append( imgDir+QString::fromUtf8( list.at( 1 ) )+L1( ".png" ) );
            else pathes.append( QString() );
        }
        Q_ASSERT( pathes.count()==2 );
        QImage img, tmp;
        if ( !pathes.at( 0 ).isEmpty() ) tmp=QImage( pathes.at( 0 ) );
        img=createImage( text, tmp, realColor(), realOutlineColor() );
        PHIByteArrayList list;
        list << parser.createImage( img, l, 0 ); // overwrite
        if ( !pathes.at( 1 ).isEmpty() ) tmp=QImage( pathes.at( 1 ) );
        img=createImage( text, tmp, realHoverColor(), realHoverBgColor() );
        list << parser.createImage( img, l, 1 ); // overwrite
        _imageBookData.setImageIds( list, l ); // |= TmpObjectCreated
        _imageBookData.setSource( PHIData::Translated );
    }
    if ( _imageBookData.source()==PHIData::File ) _imageBookData.setSource( PHIData::Translated );
}

void PHIRolloverItem::phisParseData( const PHIDataParser &parser )
{
    setImagePathes( parser.imagePathes( &_imageBookData ) );
}

void PHIRolloverItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    if ( Q_UNLIKELY( dirtyFlags() & DFText || dirtyFlags() & DFColor || dirtyFlags() & DFCustom1 ) ) {
        PHIByteArrayList pathes=imagePathes();
        pathes << "" << ""; // force at least two entries
        QImage img;
        if ( !pathes.at( 0 ).isEmpty() )
            img=QImage( req->imgDir()+QDir::separator()+QString::fromUtf8( pathes.at( 0 ) )+SL( ".png" ) );
        // expensive:
        pathes[0]=PHIDataParser::createImage( req, this, createImage( realText(), img, realColor(), realOutlineColor() ), PHIData::c(), -1 );
        if ( !pathes.at( 1 ).isEmpty() )
            img=QImage( req->imgDir()+QDir::separator()+QString::fromUtf8( pathes.at( 1 ) )+SL( ".png") );
        pathes[1]=PHIDataParser::createImage( req, this, createImage( realText(), img, realHoverColor(), realHoverBgColor() ), PHIData::c(), -1 );
        setImagePathes( PHIByteArrayList() << pathes[0] << pathes[1] );
    }
    QByteArray arr=data( DUrl ).toByteArray();
    htmlImages( req, out, script, indent );
    script+=BL( "$$rollover('" )+id();
    if ( arr.isEmpty() ) {
        script+=BL( "',undefined);\n" );
        return;
    }
    script+=BL( "',function(){" );
    if ( arr.startsWith( "javascript:" ) ) {
        arr.remove( 0, 11 );
        script+=arr;
    } else {
        arr.replace( '\'', BL( "\\'" ) );
        script+=BL( "phi.href('" )+arr+BL( "');" );
    }
    script+=BL( "}).cursor('pointer');\n" );
}

QScriptValue PHIRolloverItem::url( const QScriptValue &v )
{
    if ( !isServerItem() ) return scriptEngine()->undefinedValue();
    if ( !v.isValid() ) return realUrl();
    setUrl( v.toString() );
    return self();
}

QScriptValue PHIRolloverItem::text( const QScriptValue &v )
{
    if ( !isServerItem() ) return scriptEngine()->undefinedValue();
    if ( !v.isValid() ) return realText();
    setText( v.toString() );
    setDirtyFlag( DFText );
    return self();
}

QScriptValue PHIRolloverItem::hoverColor( const QScriptValue &c )
{
    if ( !isServerItem() ) return scriptEngine()->undefinedValue();
    if ( !c.isValid() ) return PHI::colorToString( realHoverColor() );
    setColor( PHIPalette::Hover, PHIPalette::Custom, PHI::colorFromString( c.toString() ) );
    setDirtyFlag( DFColor );
    return self();
}

void PHIRolloverItem::clientPrepareData()
{
    PHIRolloverItem::squeeze();
    removeData( DHoverColor );
    removeData( DHoverBgColor );
}

void PHIRolloverItem::clientInitData()
{
    setData( DIsImage, true );
    for ( int i=0; i<imagePathes().count(); i++ ) {
        QByteArray path=imagePathes().value( i );
        QUrl url=page()->baseUrl();
        if ( path.startsWith( '/' ) ) { // should never happen - images are always created
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
        connect( req, &PHIImageRequest::imageReady, this, &PHIRolloverItem::slotImageReady );
    }
    if ( !realUrl().isEmpty() ) setCursor( Qt::PointingHandCursor );
}

void PHIRolloverItem::slotImageReady( const QImage &img, int i )
{
    PHIImageHash v=realImages();
    v.insert( QByteArray::number( i ), img );
    setImages( v );
    update();
}

void PHIRolloverItem::click( const QGraphicsSceneMouseEvent *e )
{
    Q_UNUSED( e )
    if ( realUrl().isEmpty() ) return;
    if ( realUrl().startsWith( L1( "javascript:" ) ) ) {
        scriptEngine()->evaluate( realUrl().remove( 0, 11 ) );
        return;
    }
    emit linkRequested( realUrl() );
}

void PHIRolloverItem::mouseover( const QGraphicsSceneHoverEvent *e )
{
    Q_UNUSED( e )
    _hover=true;
    update();
}

void PHIRolloverItem::mouseout( const QGraphicsSceneHoverEvent *e )
{
    Q_UNUSED( e )
    _hover=false;
    update();
}

class PHIRolloverConfig : public PHIHoverColorConfig
{
public:
    PHIRolloverConfig( PHIBaseItem *it, QWidget *parent=0 );
};

PHIRolloverConfig::PHIRolloverConfig( PHIBaseItem *it, QWidget *parent )
    : PHIHoverColorConfig( it, parent )
{
    qDebug() << it->id() << it->colorForRole( PHIPalette::Hover );
}

PHIConfigWidget* PHIRolloverItem::ideConfigWidget()
{
    return new PHIRolloverConfig( this );
}
