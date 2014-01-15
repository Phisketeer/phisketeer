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
#include "phiimagerequest.h"

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
    return self();
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

void PHISlideShowItem::paint( QPainter *painter, const QRectF &exposed )
{
    const PHIImageHash hash=realImages();
    if ( !hash.count() ) {
        return PHIAbstractImageBookItem::paint( painter, exposed );
    }
    if ( !hash.value( "0" ).isNull() && hash.value( "1" ).isNull() ) {
        painter->drawImage( rect(), hash.value( "0" ) );
        return;
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

void PHICanvasItem::paint( QPainter *p, const QRectF &exposed )
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
        if ( t.isEmpty() ) return;
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
            +BL( "px;opacity:1\" src=\"phi.phis?i=" )+imagePath()+BL( "&t=1\">\n" );
    }
}
