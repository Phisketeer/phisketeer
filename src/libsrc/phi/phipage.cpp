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
#include <QUuid>
#include <QCryptographicHash>
#include "phipage.h"
#include "phi.h"

PHIPage::PHIPage()
{
    qDebug( "PHIPage::PHIPage()" );
    _bgColor=QColor( Qt::white );
    _palette=QPalette();
    _internalVersion=1;
    _attributes=ALandscape;
    _itemCount=0;
    _image=QImage();
    _height=PHI::defaultPageSize().height();
    _width=PHI::defaultPageSize().width();
    _font=PHI::invalidFont();
    _extensions=ENone;
    _scriptModules=SNone;
    _extensionData=QByteArray();
}

PHIPage::~PHIPage()
{
    qDebug( "PHIPage::~PHIPage()" );
}

PHIPage::PHIPage( const PHIPage &p )
{
    _id=p._id;
    _width=p._width;
    _height=p._height;
    _internalVersion=p._internalVersion;
    _image=p._image;
    _palette=p._palette;
    _bgColor=p._bgColor;
    _attributes=p._attributes;
    _itemCount=p._itemCount;
    _extensions=p._extensions;
    _font=p._font;
    _scriptModules=p._scriptModules;
    _extensionData=p._extensionData;
}

PHIPage& PHIPage::operator=( const PHIPage &p )
{
    qDebug( "PHIPage::operator=()" );
    _id=p._id;
    _width=p._width;
    _height=p._height;
    _internalVersion=p._internalVersion;
    _image=p._image;
    _palette=p._palette;
    _bgColor=p._bgColor;
    _attributes=p._attributes;
    _itemCount=p._itemCount;
    _extensions=p._extensions;
    _font=p._font;
    _scriptModules=p._scriptModules;
    _extensionData=p._extensionData;
    return *this;
}

QDataStream& operator<<( QDataStream &ds, const PHIPage *p )
{
    QByteArray id=p->_id;
    if ( id.isEmpty() ) {
        id=QUuid::createUuid().toString().toLatin1();
        id.replace( '-', "" );
        id.chop( 23 );
        id.remove( 0, 1 );
    }
    ds << p->_internalVersion << static_cast<qint32>(p->_attributes)
       << id << p->_width << p->_height << static_cast<quint16>(p->itemCount());

    qDebug( "PHIPage::_itemCount=%d", p->itemCount() );
    if ( p->_attributes & PHIPage::ABgColor ) {
        qDebug( "writing bg color" );
        ds << p->_bgColor;
    }
    if ( p->_attributes & PHIPage::APalette ) {
        qDebug( "writing palette" );
        QPalette pal=p->_palette;
        ds << pal.windowText().color() << pal.base().color() << pal.text().color()
           << pal.button().color() << pal.buttonText().color() << pal.highlight().color()
           << pal.highlightedText().color() << pal.link().color() << pal.linkVisited().color();
    }
    if ( p->_attributes & PHIPage::AIcon ) {
        qDebug( "writing icon" );
        ds << p->_image;
    }
    if ( p->_attributes & PHIPage::AExtensions ) {
        qDebug( "writing extensions" );
        ds << static_cast<qint32>(p->_extensions) << p->_extensionData;
    }
    ds << p->_font;
    if ( p->_attributes & PHIPage::AServerscript ) {
        qDebug( "writing sript data" );
        ds << static_cast<qint32>(p->_scriptModules);
    }
    return ds;
}

QDataStream& operator>>( QDataStream &ds, PHIPage *p )
{
    qint32 flags;
    ds  >> p->_internalVersion >> flags >> p->_id >> p->_width >> p->_height >> p->_itemCount;
    Q_ASSERT( p->_internalVersion==static_cast<quint8>(1) );

    p->_attributes=static_cast<PHIPage::Attributes>(flags);

    if ( p->_attributes & PHIPage::ABgColor ) ds >> p->_bgColor;
    else p->_bgColor=QColor( Qt::white );
    if ( p->_attributes & PHIPage::APalette ) {
        QPalette pal;
        QColor cwt, cba, ct, cb, cbt, ch, cht, cl, clv;
        ds >> cwt >> cba >> ct >> cb >> cbt >> ch >> cht >> cl >> clv;
        pal.setColor( QPalette::WindowText, cwt );
        pal.setColor( QPalette::Base, cba );
        pal.setColor( QPalette::Text, ct );
        pal.setColor( QPalette::Button, cb );
        pal.setColor( QPalette::ButtonText, cbt );
        pal.setColor( QPalette::Highlight, ch );
        pal.setColor( QPalette::HighlightedText, cht );
        pal.setColor( QPalette::Link, cl );
        pal.setColor( QPalette::LinkVisited, clv );
        p->_palette=pal;
    } else p->_palette=QPalette();
    //qDebug( "image" );
    if ( p->_attributes & PHIPage::AIcon ) ds >> p->_image;
    else p->_image=QImage();
    if ( p->_attributes & PHIPage::AExtensions ) {
        ds >> flags >> p->_extensionData;
    } else {
        flags=0;
        p->_extensionData.clear();
    }
    p->_extensions=static_cast<PHIPage::Extensions>(flags);
    ds >> p->_font;
    qDebug( "load page font %f", p->_font.pointSizeF() );
    if ( p->_attributes & PHIPage::AServerscript ) ds >> flags;
    else flags=0;
    p->_scriptModules=static_cast<PHIPage::ScriptModules>(flags);
    return ds;
}
