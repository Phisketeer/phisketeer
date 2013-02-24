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
#include "phibasepage.h"

PHIPageMenuEntry::PHIPageMenuEntry()
{
    qDebug( "PHIPageMenuEntry::PHIPageMenuEntry()" );
}

PHIPageMenuEntry::PHIPageMenuEntry( const QByteArray &id, const QByteArray &parent,
    const QImage &image, const QByteArray &text, Options options )
    : _id( id ), _parent( parent ), _text( text ), _img( image ), _options( options )
{
    qDebug( "PHIPageMenuEntry::PHIPageMenuEntry()" );
}

PHIPageMenuEntry::PHIPageMenuEntry( const PHIPageMenuEntry &e )
{
    _id=e._id;
    _parent=e._parent;
    _img=e._img;
    _text=e._text;
    _options=e._options;
}

PHIPageMenuEntry& PHIPageMenuEntry::operator=( const PHIPageMenuEntry &e )
{
    _id=e._id;
    _parent=e._parent;
    _img=e._img;
    _text=e._text;
    _options=e._options;
    return *this;
}

PHIPageMenuEntry::~PHIPageMenuEntry()
{
    qDebug( "PHIPageMenuEntry::~PHIPageMenuEntry()" );
}

QDataStream& operator<<( QDataStream &out, const PHIPageMenuEntry &m )
{
    out << m._id << m._parent << m._img << m._text << static_cast<quint8>(m._options);
    return out;
}

QDataStream& operator>>( QDataStream &in, PHIPageMenuEntry &m )
{
    quint8 opt;
    in >> m._id >> m._parent >> m._img >> m._text >> opt;
    m._options=static_cast<PHIPageMenuEntry::Options>(opt);
    return in;
}

PHIBasePage::PHIBasePage( const PHIPage &p, QObject *parent )
    : QObject( parent ), PHIPage( p )
{
    qDebug( "PHIBasePage::PHIBasePage( const PHIPage&, QObject* )" );
}

PHIBasePage::PHIBasePage( QObject *parent )
    : QObject( parent ), PHIPage()
{
    qDebug( "PHIBasePage::PHIBasePage( QObject* )" );
}

PHIBasePage::~PHIBasePage()
{
    PHIBaseItem *it;
    foreach ( it, _items ) { delete it; }
    qDebug( "PHIBasePage::~PHIBasePage()" );
}

PHIBaseItem* PHIBasePage::createElementById( quint16 type, const QString &id, qreal x, qreal y,
    qreal width, qreal height )
{
    PHI::Widget wid=static_cast<PHI::Widget>(type);
    QByteArray newid=id.toLatin1();
    PHIBaseItem *it( 0 );
    try {
        it=new PHIBaseItem( wid, newid );
    } catch( std::bad_alloc& ) {
        return 0;
    }
    insertElementById( newid, it );
    if ( width>0 ) it->setWidth( width );
    else it->setWidth( PHI::defaultWidth() );
    if ( height>0 ) it->setHeight( height );
    else it->setHeight( PHI::defaultHeight() );
    it->setX( x );
    it->setY( y );
    it->setPageId( this->pageId() );
    it->setVisible( true );
    it->setDisabled( false );

    if ( wid==PHI::CALENDAR || wid==PHI::DATEEDIT ) _attributes |= PHIPage::AHasCalendar;
    else if ( wid==PHI::PHISYS_LINK ) it->setZValue( PHI::maxZValue()+1 );
    else if ( wid==PHI::PROGRESSBAR ) _extensions |= PHIPage::EProgressBar;
    else if ( wid==PHI::FACEBOOK_LIKE ) _extensions |= PHIPage::EHasFacebookLike;
    else if ( wid==PHI::TWITTER ) _extensions |= PHIPage::EHasTwitter;
    else if ( wid==PHI::GOOGLE_PLUS ) _extensions |= PHIPage::EHasGooglePlus;
    else if ( wid==PHI::CANVAS ) _extensions |= PHIPage::EHasCanvas;
    return it;
}

QStringList PHIBasePage::itemIds() const
{
    QStringList list;
    QByteArray id;
    foreach( id, _items.keys() ) list.append( QString::fromUtf8( id ) );
    return list;
}

bool PHIBasePage::needjQueryUI() const
{
    if ( _attributes & AHasCalendar ) return true;
    if ( _extensions & EProgressBar ) return true;
    if ( _attributes & AHasDragDrop ) return true;
    return false;
}

QColor PHIBasePage::additionalColor( PHIPage::DataType c ) const
{
    switch ( c ) {
    case DColorError: return _variants.value( DColorError, QColor( 0xcd, 0x0a, 0x0a ) ).value<QColor>();
    case DColorBgError: return _variants.value( DColorBgError, QColor( 0xfe, 0xf1, 0xec ) ).value<QColor>();
    case DColorHover: return _variants.value( DColorHover, QColor( 0x21, 0x21, 0x21 ) ).value<QColor>();
    default:;
    }
    return QColor();
}

QDataStream& operator<<( QDataStream &out, const PHIBasePage *p )
{
    out << dynamic_cast<const PHIPage*>(p) << p->_variants;
    if ( p->_attributes & PHIPage::AApplication ) out << p->_menuEntries;
    return out;
}

QDataStream& operator>>( QDataStream &in, PHIBasePage *p )
{
    in >> dynamic_cast<PHIPage*>(p) >> p->_variants;
    if ( p->_attributes & PHIPage::AApplication ) in >> p->_menuEntries;
    return in;
}
