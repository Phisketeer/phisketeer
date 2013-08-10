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
#include "phibaseitem.h"
#include "phiitemfactory.h"

PHIDynPageData::PHIDynPageData()
{
    _title=new PHITextData();
    _styleSheet=new PHITextData();
    _author=new PHITextData();
    _version=new PHITextData();
    _company=new PHITextData();
    _copyright=new PHITextData();
    _template=new PHITextData();
    _javascript=new PHITextData();
    _serverscript=new PHITextData();
    _action=new PHITextData();
    _keys=new PHITextData();
    _sessionRedirect=new PHITextData();
    _description=new PHITextData();
    _opengraph=new PHITextData();
    _bgImage=new PHIImageData();
}

PHIDynPageData::~PHIDynPageData()
{
    delete _title;
    delete _styleSheet;
    delete _author;
    delete _version;
    delete _company;
    delete _copyright;
    delete _template;
    delete _javascript;
    delete _serverscript;
    delete _action;
    delete _keys;
    delete _sessionRedirect;
    delete _description;
    delete _opengraph;
    delete _bgImage;
}

PHIDynPageData::PHIDynPageData( const PHIDynPageData &p )
{
    qDebug( "PHISPage::PHISPage( const PHISPage& )" );
    _title=new PHITextData();
    _styleSheet=new PHITextData();
    _author=new PHITextData();
    _version=new PHITextData();
    _company=new PHITextData();
    _copyright=new PHITextData();
    _template=new PHITextData();
    _javascript=new PHITextData();
    _serverscript=new PHITextData();
    _action=new PHITextData();
    _keys=new PHITextData();
    _sessionRedirect=new PHITextData();
    _description=new PHITextData();
    _opengraph=new PHITextData();
    _bgImage=new PHIImageData();

    *_title=*p._title;
    *_styleSheet=*p._styleSheet;
    *_author=*p._author;
    *_version=*p._version;
    *_company=*p._company;
    *_copyright=*p._copyright;
    *_template=*p._template;
    *_javascript=*p._javascript;
    *_serverscript=*p._serverscript;
    *_action=*p._action;
    *_keys=*p._keys;
    *_sessionRedirect=*p._sessionRedirect;
    *_description=*p._description;
    *_opengraph=*p._opengraph;
    *_bgImage=*p._bgImage;
}

PHIDynPageData& PHIDynPageData::operator=( const PHIDynPageData &p )
{
    *_title=*p._title;
    *_styleSheet=*p._styleSheet;
    *_author=*p._author;
    *_version=*p._version;
    *_company=*p._company;
    *_copyright=*p._copyright;
    *_template=*p._template;
    *_javascript=*p._javascript;
    *_serverscript=*p._serverscript;
    *_action=*p._action;
    *_keys=*p._keys;
    *_sessionRedirect=*p._sessionRedirect;
    *_description=*p._description;
    *_opengraph=*p._opengraph;
    *_bgImage=*p._bgImage;
    return *this;
}

PHIBasePage::PHIBasePage( QObject *parent )
    : QObject( parent ), _font( PHI::defaultFont() )
{
    qDebug( "PHIBasePage::PHIBasePage()" );
    _pageData=new PHIDynPageData();
    _width=PHI::defaultPageSize().width();
    _height=PHI::defaultPageSize().height();
}

PHIBasePage::~PHIBasePage()
{
    delete _pageData;
    qDebug( "PHIBasePage::~PHIBasePage()" );
}

QList <PHIBaseItem*> PHIBasePage::items() const
{
    return findChildren<PHIBaseItem*>();
}

/*
PHIBaseItem* PHIBasePage::findItem( const QString &id ) const
{
    return findChild<PHIBaseItem*>(id);
    if ( list.isEmpty() ) return 0;
    return list.first();
    PHIBaseItem *it;
    foreach( it, list ) {
        if ( it->name()==id ) return it;
    }
    return 0;
}
*/

// only available for Serverscript:
bool PHIBasePage::removeElementById( const QString &id )
{
    PHIBaseItem *it=findItem( id );
    if ( !it ) return false;
    delete it;
    return true;
}

// only available for Serverscript:
PHIBaseItem* PHIBasePage::createElementById( PHIWID wid, const QString &id,
    qreal x, qreal y, qreal width, qreal height )
{
    if ( containsItemId( id ) ) return 0;
    PHIBaseItem *it=PHIItemFactory::instance()->item( wid );
    if ( !it ) return 0;
    it->setId( id );
    it->setX( x );
    it->setY( y );
    if ( width > 0 ) it->setWidth( width );
    if ( height > 0 ) it->setHeight( height );
    return it;
}

quint16 PHIBasePage::itemCount() const
{
    return static_cast<quint16>(findChildren<PHIBaseItem*>().count());
}

PHIBaseItem* PHIBasePage::findItem( const QString &id ) const
{
    return findChild<PHIBaseItem*>(id);
}

QStringList PHIBasePage::itemIds() const
{
    QStringList ids;
    PHIBaseItem *it;
    QList <PHIBaseItem*> list=findChildren<PHIBaseItem*>();
    foreach( it, list ) ids.append( it->name() );
    return ids;
}

QString PHIBasePage::nextFreeId( const QString &requestedId ) const
{
    PHIBaseItem *it=findChild<PHIBaseItem*>(requestedId);
    if ( !it ) return requestedId;
    int i=1;
    QString id=requestedId+L1( "_" )+QString::number( ++i );
    while ( findChild<PHIBaseItem*>(id) ) {
        id=requestedId+L1( "_" )+QString::number( ++i );
    }
    return id;
}
/*
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
*/
    /*
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
    */
/*
    return 0;
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
*/
