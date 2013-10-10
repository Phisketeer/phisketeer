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
#include "phiimageitems.h"
#include "phibasepage.h"

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

void PHISvgItem::initIDE()
{
    _textData.setText( svgDefaultSource() );
}

void PHISvgItem::updateData()
{
    if ( _textData.unparsedStatic() ) setData( DSvgSource, _textData.text().toLatin1() );
    else if ( _textData.translated() )
        setData( DSvgSource, _textData.text( page()->currentLang() ).toLatin1() );
    else setData( DSvgSource, QByteArray() );
    initWidget();
}

void PHISvgItem::setText( const QString &s, const QByteArray &lang )
{
    if ( _textData.translated() ) _textData.setText( s, lang );
    else _textData.setText( s );
    updateData();
}

QString PHISvgItem::text( const QByteArray &lang ) const
{
    if ( _textData.translated() ) return _textData.text( lang );
    return _textData.text();
}

QSizeF PHISvgItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    Q_UNUSED( which )
    Q_UNUSED( constraint )
    if ( _renderer.isValid() ) return QSizeF( _renderer.defaultSize() );
    return QSizeF( 32., 32. );
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
    _renderer.load( data( DSvgSource ).toByteArray() );
    if ( _renderer.isValid() ) resize( QSizeF( _renderer.defaultSize() ) );
    else resize( QSizeF( 32., 32. ) );
    update();
}

void PHISvgItem::paint( QPainter *p, const QRectF &exposed )
{
    Q_UNUSED( exposed )
    if ( _renderer.isValid() ) _renderer.render( p, rect() );
    else {
        p->fillRect( rect(), QBrush( Qt::lightGray ) );
        QFont f=font();
        f.setPointSizeF( PHI::adjustedFontSize( 10. ) );
        p->setPen( Qt::darkGray );
        p->drawText( rect(), tr( "SVG" ), QTextOption( Qt::AlignCenter ) );
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
