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
#include <QUuid>
#include <QGuiApplication>
#include "phibasepage.h"
#include "phibaseitem.h"
#include "phiitemfactory.h"
#include "phidatasources.h"
#include "phidataparser.h"
#include "phirequest.h"

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
    : _title( 0 ), _styleSheet( 0 ), _author( 0 ), _version( 0 ),
      _company( 0 ), _copyright( 0 ), _template( 0 ), _javascript( 0 ),
      _serverscript( 0 ), _action( 0 ), _keys( 0 ), _sessionRedirect( 0 ),
      _description( 0 ), _opengraph( 0 ), _bgImage( 0 )
{
    if ( p._title ) _title=new PHITextData();
    if ( p._styleSheet ) _styleSheet=new PHITextData();
    if ( p._author ) _author=new PHITextData();
    if ( p._version ) _version=new PHITextData();
    if ( p._company ) _company=new PHITextData();
    if ( p._copyright ) _copyright=new PHITextData();
    if ( p._template ) _template=new PHITextData();
    if ( p._javascript ) _javascript=new PHITextData();
    if ( p._serverscript ) _serverscript=new PHITextData();
    if ( p._action ) _action=new PHITextData();
    if ( p._keys ) _keys=new PHITextData();
    if ( p._sessionRedirect ) _sessionRedirect=new PHITextData();
    if ( p._description ) _description=new PHITextData();
    if ( p._opengraph ) _opengraph=new PHITextData();
    if ( p._bgImage ) _bgImage=new PHIImageData();
    operator=( p );
}

PHIDynPageData& PHIDynPageData::operator=( const PHIDynPageData &p )
{
    if ( p._title ) *_title=*p._title;
    if ( p._styleSheet ) *_styleSheet=*p._styleSheet;
    if ( p._author ) *_author=*p._author;
    if ( p._version ) *_version=*p._version;
    if ( p._company ) *_company=*p._company;
    if ( p._copyright ) *_copyright=*p._copyright;
    if ( p._template ) *_template=*p._template;
    if ( p._javascript ) *_javascript=*p._javascript;
    if ( p._serverscript ) *_serverscript=*p._serverscript;
    if ( p._action ) *_action=*p._action;
    if ( p._keys ) *_keys=*p._keys;
    if ( p._sessionRedirect ) *_sessionRedirect=*p._sessionRedirect;
    if ( p._description ) *_description=*p._description;
    if ( p._opengraph ) *_opengraph=*p._opengraph;
    if ( p._bgImage ) *_bgImage=*p._bgImage;
    return *this;
}

// IDE only
bool PHIDynPageData::operator==( const PHIDynPageData &p )
{
    if ( *_title!=*p._title ) return false;
    if ( *_styleSheet!=*p._styleSheet ) return false;
    if ( *_author!=*p._author ) return false;
    if ( *_version!=*p._version ) return false;
    if ( *_company!=*p._company ) return false;
    if ( *_copyright!=*p._copyright ) return false;
    if ( *_template!=*p._template ) return false;
    if ( *_javascript!=*p._javascript ) return false;
    if ( *_serverscript!=*p._serverscript ) return false;
    if ( *_action!=*p._action ) return false;
    if ( *_keys!=*p._keys ) return false;
    if ( *_sessionRedirect!=*p._sessionRedirect ) return false;
    if ( *_description!=*p._description ) return false;
    if ( *_opengraph!=*p._opengraph ) return false;
    if ( *_bgImage!=*p._bgImage ) return false;
    return true;
}

QDataStream& operator<<( QDataStream &out, const PHIDynPageData *p )
{
    out << p->_title << p->_styleSheet << p->_author << p->_version
        << p->_company << p->_copyright << p->_template << p->_javascript
        << p->_serverscript << p->_action << p->_keys << p->_sessionRedirect
        << p->_description << p->_opengraph << p->_bgImage;
    return out;
}

QDataStream& operator>>( QDataStream &in, PHIDynPageData *p )
{
    in >> p->_title >> p->_styleSheet >> p->_author >> p->_version
        >> p->_company >> p->_copyright >> p->_template >> p->_javascript
        >> p->_serverscript >> p->_action >> p->_keys >> p->_sessionRedirect
        >> p->_description >> p->_opengraph >> p->_bgImage;
    return in;
}

PHIBasePage::PHIBasePage( QObject *parent )
    : QObject( parent ), _font( PHI::defaultFont() )
{
    _pageData=new PHIDynPageData();
    setGeometry( G4_3 );
    _variants.insert( DLanguages, QStringList() << SL( "en" ) );
    _variants.insert( DDefaultLang, BL( "en" ) );
    _bgColor=QColor( Qt::white );
    _font=PHI::defaultFont();
    QByteArray arr=QUuid::createUuid().toByteArray();
    arr.replace( '-', QByteArray() );
    _id=arr.mid( 5, 10 );
    _currentLang="en";
    _dbPort=3306;
    _dbDriver=SL( "QSQLITE" );
    _favicon=QImage( SL( ":/file/phiappview" ) );
    _dirtyFlags=DFClean;
}

PHIBasePage& PHIBasePage::operator=( const PHIBasePage &p )
{
    *_pageData=*(p._pageData);
    _id=p._id;
    _currentLang=p._currentLang;
    _width=p._width;
    _height=p._height;
    _variants=p._variants;
    _dbName=p._dbName;
    _dbHost=p._dbHost;
    _dbPasswd=p._dbPasswd;
    _dbUser=p._dbUser;
    _dbDriver=p._dbDriver;
    _dbOptions=p._dbOptions;
    _dbFileName=p._dbFileName;
    _dbPort=p._dbPort;
    _favicon=p._favicon;
    _font=p._font;
    _bgColor=p._bgColor;
    _menuEntries=p._menuEntries;
    _flags=p._flags;
    _dirtyFlags=p._dirtyFlags;
    _pal=p._pal;
    return *this;
}

// expensive (used in the IDE only)
bool PHIBasePage::operator==( const PHIBasePage &p )
{
    if ( _id!=p._id ) return false;
    if ( *_pageData!=*p._pageData ) return false;
    if ( _width!=p._width ) return false;
    if ( _height!=p._height ) return false;
    if ( _variants!=p._variants ) return false;
    if ( _dbName!=p._dbName ) return false;
    if ( _dbHost!=p._dbHost ) return false;
    if ( _dbPasswd!=p._dbPasswd ) return false;
    if ( _dbUser!=p._dbUser ) return false;
    if ( _dbDriver!=p._dbDriver ) return false;
    if ( _dbOptions!=p._dbOptions ) return false;
    if ( _dbFileName!=p._dbFileName ) return false;
    if ( _dbPort!=p._dbPort ) return false;
    if ( _favicon!=p._favicon ) return false;
    if ( _font!=p._font ) return false;
    if ( _bgColor!=p._bgColor ) return false;
    if ( _menuEntries!=p._menuEntries ) return false;
    if ( _flags!=p._flags ) return false;
    if ( _dirtyFlags!=p._dirtyFlags ) return false;
    if ( _pal!=p._pal ) return false;
    return true;
}

QList <PHIBaseItem*> PHIBasePage::items() const
{
    return findChildren<PHIBaseItem*>();
}

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
    PHIBaseItemPrivate p( PHIBaseItemPrivate::TServerItem, this, 0 );
    PHIBaseItem *it=PHIItemFactory::instance()->item( wid, p );
    if ( !it ) return 0;
    it->setId( id );
    it->setX( x );
    it->setY( y );
    if ( width > 0 ) it->setWidth( width );
    if ( height > 0 ) it->setHeight( height );
    return it;
}

QStringList PHIBasePage::itemIds() const
{
    QStringList ids;
    PHIBaseItem *it;
    QList <PHIBaseItem*> list=findChildren<PHIBaseItem*>();
    foreach( it, list ) ids.append( it->name() );
    return ids;
}

PHIByteArrayList PHIBasePage::itemIdsByteArray() const
{
    PHIByteArrayList ids;
    PHIBaseItem *it;
    QList <PHIBaseItem*> list=findChildren<PHIBaseItem*>();
    foreach( it, list ) ids.append( it->id() );
    return ids;
}

QString PHIBasePage::nextFreeId( const QString &requestedId ) const
{
    QString rid=requestedId.simplified();
    rid.replace( L1( " " ), QString() );
    rid.replace( QRegExp( L1( "_[0-9]*$" ) ), QString() );
    PHIBaseItem *it=findChild<PHIBaseItem*>(rid);
    if ( !it ) return rid;
    int i=1;
    QString id=rid+L1( "_" )+QString::number( ++i );
    while ( findChild<PHIBaseItem*>(id) ) {
        id=rid+L1( "_" )+QString::number( ++i );
    }
    return id;
}

void PHIBasePage::setGeometry( Geometry g )
{
    _variants.insert( DGeometry, static_cast<quint8>(g) );
    switch( g ) {
    case GCustom: return;
    case GA4: setSize( 1000, 1414 ); break;
    case GLetter: setSize( 1000, 1292 ); break;
    case GPhi: setSize( 618, 1000 ); break;
    case G4_3: setSize( 750, 1000 ); break;
    case G16_9: setSize( 720, 1280 ); break;
    case GiPad: setSize( 768, 1024 ); break;
    default:;
    }
}

quint16 PHIBasePage::itemCount() const
{
    return static_cast<quint16>(findChildren<PHIBaseItem*>().count());
}

PHIBaseItem* PHIBasePage::findItem( const QString &id ) const
{
    PHIBaseItem *it;
    foreach( it, findChildren<PHIBaseItem*>() ) {
        if ( it->name()==id ) return it;
    }
    return 0;
}

PHIBaseItem* PHIBasePage::findItem( const QByteArray &id ) const
{
    PHIBaseItem *it;
    foreach( it, findChildren<PHIBaseItem*>() ) {
        if ( it->id()==id ) return it;
    }
    return 0;
}

// only run once at loading time from server
void PHIBasePage::createTmpData( const PHIDataParser &p )
{
    squeeze();
    _dirtyFlags=DFClean;
    if ( _flags & FUseBgImage ) p.createTmpImages( bgImageData() );
    else {
        delete _pageData->_bgImage; _pageData->_bgImage=0;
    }
    if ( companyData()->unparsedStatic() ) {
        if ( !companyData()->text().isEmpty() ) _variants.insert( DCompany, companyData()->variant() );
        delete _pageData->_company; _pageData->_company=0;
    }
    if ( titleData()->unparsedStatic() ) {
        if ( !titleData()->text().isEmpty() ) _variants.insert( DTitle, titleData()->variant() );
        delete _pageData->_title; _pageData->_title=0;
    }
    if ( styleSheetData()->unparsedStatic() ) {
        if ( !styleSheetData()->text().isEmpty() ) _variants.insert( DStyleSheet, styleSheetData()->variant() );
        delete _pageData->_styleSheet; _pageData->_styleSheet=0;
    }
    if ( versionData()->unparsedStatic() ) {
        if ( !versionData()->text().isEmpty() ) _variants.insert( DVersion, versionData()->variant() );
        delete _pageData->_version; _pageData->_version=0;
    }
    if ( opengraphData()->unparsedStatic() ) {
        if ( !opengraphData()->text().isEmpty() ) _variants.insert( DOpenGraph, opengraphData()->variant() );
        delete _pageData->_opengraph; _pageData->_opengraph=0;
    }
    if ( javascriptData()->unparsedStatic() ) {
        if ( !javascriptData()->text().isEmpty() ) _variants.insert( DJavascript, javascriptData()->variant() );
        delete _pageData->_javascript; _pageData->_javascript=0;
    }
    if ( Q_LIKELY( serverscriptData()->unparsedStatic() || !(serverscriptData()->options() & PHIData::NoCache) ) ) {
        // languages are ignored if 'PHIData::NoCache' is not specified however parsing
        // every time is very expensive and a user can specifiy 'NoCache' if he really wants
        // script execution based on different languages which should be a rare case...
        QByteArray arr=p.text( serverscriptData() ).toByteArray();
        if ( Q_LIKELY( !arr.isEmpty() ) ) _script=QScriptProgram( QString::fromUtf8( arr ) );
        delete _pageData->_serverscript; _pageData->_serverscript=0;
    }
    if ( authorData()->unparsedStatic() ) {
        if ( !authorData()->text().isEmpty() ) _variants.insert( DAuthor, authorData()->variant() );
        delete _pageData->_author; _pageData->_author=0;
    }
    if ( copyrightData()->unparsedStatic() ) {
        if ( !copyrightData()->text().isEmpty() ) _variants.insert( DCopyright, copyrightData()->variant() );
        delete _pageData->_copyright; _pageData->_copyright=0;
    }
    if ( sessionRedirectData()->unparsedStatic() ) {
        if ( !sessionRedirectData()->text().isEmpty() ) _variants.insert( DSessionRedirect, sessionRedirectData()->variant() );
        delete _pageData->_sessionRedirect; _pageData->_sessionRedirect=0;
    }
    if ( actionData()->unparsedStatic() ) {
        if ( !actionData()->text().isEmpty() ) _variants.insert( DAction, actionData()->variant() );
        delete _pageData->_action; _pageData->_action=0;
    }
    if ( templatePageData()->unparsedStatic() ) {
        if ( !templatePageData()->text().isEmpty() ) _variants.insert( DTemplatePage, templatePageData()->variant() );
        delete _pageData->_template; _pageData->_template=0;
    }
    if ( descriptionData()->unparsedStatic() ) {
        if ( !descriptionData()->text().isEmpty() ) _variants.insert( DDescription, descriptionData()->variant() );
        delete _pageData->_description; _pageData->_description=0;
    }
    if ( keywordsData()->unparsedStatic() ) {
        if ( !keywordsData()->text().isEmpty() ) _variants.insert( DKeys, keywordsData()->variant() );
        delete _pageData->_keys; _pageData->_keys=0;
    }
    if ( !_favicon.isNull() ) {
        _favicon.save( p.request()->imgDir()+QLatin1Char( '/' )+id()+L1( ".ico" ), "ICO" );
        _favicon=QImage();
    }
}

void PHIBasePage::parseData( const PHIDataParser &p )
{
    if ( Q_LIKELY( titleData() ) ) _variants.insert( DTitle, p.text( titleData() ) );
    if ( Q_UNLIKELY( companyData() ) ) _variants.insert( DCompany, p.text( companyData() ) );
    if ( Q_UNLIKELY( versionData() ) ) _variants.insert( DVersion, p.text( versionData() ) );
    if ( Q_UNLIKELY( opengraphData() ) ) _variants.insert( DOpenGraph, p.text( opengraphData() ) );
    if ( Q_UNLIKELY( javascriptData() ) ) _variants.insert( DJavascript, p.text( javascriptData() ) );
    if ( Q_UNLIKELY( authorData() ) ) _variants.insert( DAuthor, p.text( authorData() ) );
    if ( Q_UNLIKELY( copyrightData() ) ) _variants.insert( DCopyright, p.text( copyrightData() ) );
    if ( Q_UNLIKELY( sessionRedirectData() ) ) _variants.insert( DSessionRedirect, p.text( sessionRedirectData() ) );
    if ( Q_UNLIKELY( actionData() ) ) _variants.insert( DAction, p.text( actionData() ) );
    if ( Q_UNLIKELY( templatePageData() ) ) _variants.insert( DTemplatePage, p.text( templatePageData() ) );
    if ( Q_UNLIKELY( keywordsData() ) ) _variants.insert( DKeys, p.text( keywordsData() ) );
    if ( Q_UNLIKELY( descriptionData() ) ) _variants.insert( DDescription, p.text( descriptionData() ) );
    if ( Q_UNLIKELY( serverscriptData() ) ) _script=QScriptProgram( QString::fromUtf8( p.text( serverscriptData() ).toByteArray() ) );
    if ( Q_UNLIKELY( styleSheetData() ) ) {
        _dirtyFlags |= DFStyleSheet;
        _variants.insert( DStyleSheet, p.text( styleSheetData() ) );
    }
    if ( _flags & FUseBgImage ) {
        _variants.insert( DBgImageUrl, p.imagePath( bgImageData() ) );
    }
    PHIPageMenuEntry entry;
    foreach ( entry, _menuEntries ) {
        entry.setText( p.text( entry.textData() ).toString() );
    }
}

void PHIBasePage::copyMasterData( const PHIBasePage *m )
{
    if ( _flags & FUseMasterPalette ) {
        _pal=m->phiPalette();
        PHIBaseItem *it;
        foreach( it, findChildren<PHIBaseItem*>() ) it->phiPaletteChanged( _pal );
        _bgColor=m->backgroundColor();
    }
    if ( !_variants.value( DTitle ).isValid() ) _variants.insert( DTitle, m->data().value( DTitle ) );
    if ( !_variants.value( DCopyright ).isValid() ) _variants.insert( DCopyright, m->data().value( DCopyright ) );
    if ( !_variants.value( DAuthor ).isValid() ) _variants.insert( DAuthor, m->data().value( DAuthor ) );
    if ( !_variants.value( DOpenGraph ).isValid() ) _variants.insert( DOpenGraph, m->data().value( DOpenGraph ) );
    if ( !_variants.value( DCompany ).isValid() ) _variants.insert( DCompany, m->data().value( DCompany ) );
    if ( !_variants.value( DSessionRedirect ).isValid() ) _variants.insert( DSessionRedirect, m->data().value( DSessionRedirect ) );
    if ( !_variants.value( DVersion ).isValid() ) _variants.insert( DVersion, m->data().value( DVersion ) );
    if ( !_variants.value( DKeys ).isValid() ) _variants.insert( DKeys, m->data().value( DKeys ) );
    if ( !_variants.value( DAction ).isValid() ) _variants.insert( DAction, m->data().value( DAction ) );
    if ( Q_LIKELY( m->data().value( DJavascript ).isValid() ) ) {
        if ( Q_LIKELY( _variants.value( DJavascript ).isValid() ) ) {
            _variants.insert( DJavascript, m->data().value( DJavascript ).toByteArray()
                +'\n'+_variants.value( DJavascript ).toByteArray() );
        } else {
            _variants.insert( DJavascript, m->data().value( DJavascript ) );
        }
    }
    if ( Q_UNLIKELY( m->data().value( DStyleSheet ).isValid() ) ) {
        if ( Q_UNLIKELY( _variants.value( DStyleSheet ).isValid() ) ) {
            _variants.insert( DStyleSheet, m->data().value( DStyleSheet ).toByteArray()
                +'\n'+_variants.value( DStyleSheet ).toByteArray() );
        } else {
            _variants.insert( DStyleSheet, m->data().value( DStyleSheet ) );
        }
    }
    if ( m->flags() & FUseBgImage ) {
        if ( !(_flags & FUseBgImage ) ) {
            _variants.insert( DBgImageUrl, m->data().value( DBgImageUrl ) );
            _variants.insert( DBgImageXOff, m->data().value( DBgImageXOff ) );
            _variants.insert( DBgImageYOff, m->data().value( DBgImageYOff ) );
            _variants.insert( DBgImageOptions, m->data().value( DBgImageOptions ) );
        }
    }
    if ( Q_UNLIKELY( m->flags() & FServerModulesCombat ) ) {
        qint32 modules=serverModules();
        modules |= m->serverModules();
        setServerModules( modules );
    }
    if ( _menuEntries.count()==0 ) {
        _menuEntries=m->menuEntries();
    }
}

void PHIBasePage::squeeze()
{
    _variants.remove( DDBDriver );
    _variants.remove( DDBUser );
    _variants.remove( DDBFileName );
    _variants.remove( DDBPasswd );
    _variants.remove( DDBPort );
    _variants.remove( DDBOptions );
    _variants.remove( DDBHost );
    _variants.remove( DWidth );
    _variants.remove( DHeight );
    _variants.remove( DGeometry );
    if ( serverModules()==0 ) _variants.remove( DServerModules );
    _variants.squeeze();
    _pal.squeeze();
}

void PHIBasePage::save( QDataStream &out, qint32 version )
{
    Q_ASSERT( version>2 );
    out.setVersion( PHI_DSV2 );
    if ( _width==750 ) _variants.remove( DWidth );
    else _variants.insert( DWidth, _width );
    if ( _height==1000 ) _variants.remove( DHeight );
    else _variants.insert( DHeight, _height );
    if ( _bgColor!=QColor( Qt::white ) ) _variants.insert( DBgColor, _bgColor );
    else _variants.remove( DBgColor );
    if ( _flags & FUseDB ) {
        if ( _flags & FDBFile ) {
            _variants.insert( DDBFileName, _dbFileName.toUtf8() );
        } else {
            _variants.insert( DDBDriver, _dbDriver.toUtf8() );
            _variants.insert( DDBHost, _dbHost.toUtf8() );
            _variants.insert( DDBName, _dbName.toUtf8() );
            _variants.insert( DDBUser, _dbUser.toUtf8() );
            _variants.insert( DDBPort, _dbPort );
            _variants.insert( DDBPasswd, _dbPasswd.toUtf8() );
            _variants.insert( DDBOptions, _dbOptions.toUtf8() );
        }
    }
    out << _id << static_cast<quint32>(_flags) << _pal << _pageData
        << static_cast<quint16>( itemCount() ) << _variants
        << _favicon << _font << _menuEntries;
}

quint16 PHIBasePage::load( QDataStream &in, qint32 version )
{
    Q_ASSERT( items().count()==0 );
    if ( Q_UNLIKELY( version<3 ) ) return loadVersion1_x( in );
    in.setVersion( PHI_DSV2 );
    quint32 flags;
    quint16 itemCount;
    in >> _id >> flags >> _pal >> _pageData >> itemCount >> _variants
        >> _favicon >> _font >> _menuEntries;
    _flags=static_cast<Flags>(flags);
    _width=_variants.value( DWidth, 750 ).value<quint32>();
    _height=_variants.value( DHeight, 1000 ).value<quint32>();
    _bgColor=_variants.value( DBgColor, QColor( Qt::white ) ).value<QColor>();
    _dbFileName=QString::fromUtf8( _variants.value( DDBFileName ).toByteArray() );
    _dbHost=QString::fromUtf8( _variants.value( DDBHost ).toByteArray() );
    _dbName=QString::fromUtf8( _variants.value( DDBName ).toByteArray() );
    _dbUser=QString::fromUtf8( _variants.value( DDBUser ).toByteArray() );
    _dbPasswd=QString::fromUtf8( _variants.value( DDBPasswd ).toByteArray() );
    _dbOptions=QString::fromUtf8( _variants.value( DDBOptions ).toByteArray() );
    _dbDriver=QString::fromUtf8( _variants.value( DDBDriver, QByteArray( "QSQLITE" ) ).toByteArray() );
    _dbPort=_variants.value( DDBPort, 3360 ).value<qint32>();
    _currentLang=_variants.value( DDefaultLang, QByteArray( "en" ) ).toByteArray();
    if ( !(_flags & FUseOwnPalette) ) _pal.setPalette( QGuiApplication::palette() );
    return itemCount;
}

quint16 PHIBasePage::loadVersion1_x( QDataStream &in )
{
    enum Attribute { ANone=0x0, ARequiresSession=0x1, ARequiresLogin=0x2, AFormAction=0x4, APalette=0x8,
        ABgColor=0x10, AStyleSheet=0x20, ACreateSession=0x40, AKeywords=0x80, ALandscape=0x100, ATemplate=0x200,
        AIcon=0x400, AExtensions=0x800, AJavascript=0x1000, ADatabase=0x2000, ASetCookie=0x4000,
        AServerscript=0x8000, AUsejQueryUI=0x10000, AApplication=0x20000, AUseTemplatePalette=0x40000,
        ANoSystemCSS=0x80000, AHasCalendar=0x100000, APhiNoObjectTag=0x200000,
        ADbSettingsFromFile=0x400000, ANoUiCSS=0x800000, AHasDragDrop=0x1000000,
        AForceHtmlOutput=0x2000000, ADocumentLeft=0x4000000, AUseOpenGraph=0x8000000,
        ANoUnderlineLinks=0x10000000, ABgImage=0x20000000, AMax=0x40000000 }; // qint32
    enum Extension { ENone=0x0, EProgressBar=0x1, EHasFacebookLike=0x2, EHasTwitter=0x4,
        EHasGooglePlus=0x8, EHidePhiMenu=0x10, EHasCanvas=0x20 }; // qint32

    qint32 attributes( 0 ), extensions( 0 ), modules( 0 );
    quint16 itemCount;
    quint8 internalVersion;
    QByteArray extensionData, eventData, editorData, dynamicData, reserved;

    in >> internalVersion >> attributes >> _id >> _width >> _height >> itemCount;
    if ( attributes & ABgColor ) in >> _bgColor;
    else _bgColor=QColor( Qt::white );
    qDebug() << attributes << _id << _width << _height << itemCount;
    if ( attributes & APalette ) {
        QPalette pal;
        QColor cwt, cba, ct, cb, cbt, ch, cht, cl, clv;
        in >> cwt >> cba >> ct >> cb >> cbt >> ch >> cht >> cl >> clv;
        pal.setColor( QPalette::WindowText, cwt );
        pal.setColor( QPalette::Base, cba );
        pal.setColor( QPalette::Text, ct );
        pal.setColor( QPalette::Button, cb );
        pal.setColor( QPalette::ButtonText, cbt );
        pal.setColor( QPalette::Highlight, ch );
        pal.setColor( QPalette::HighlightedText, cht );
        pal.setColor( QPalette::Link, cl );
        pal.setColor( QPalette::LinkVisited, clv );
        _pal.setPalette( pal );
    }
    if ( attributes & AIcon ) in >> _favicon;
    if ( attributes & AExtensions ) in >> extensions >> extensionData;
    in >> _font;
    if ( attributes & AServerscript ) in >> modules;
    in >> dynamicData >> eventData >> reserved >> editorData >> reserved >> reserved;
    QDataStream dsd( &dynamicData, QIODevice::ReadOnly );
    dsd.setVersion( PHI_DSV );
    {
        PHITextData txtData;
        if ( attributes & ADatabase ) {
            if ( attributes & ADbSettingsFromFile ) dsd >> _dbFileName;
            else dsd >> _dbName >> _dbHost >> _dbPasswd >> _dbUser >> _dbDriver >> _dbOptions >> _dbPort;
        }
        QStringList langs;
        qint32 sessionTimeout;
        PHIDynPageData *d=_pageData;
        dsd >> d->_title >> d->_styleSheet >> d->_author >> d->_version >> d->_company >> d->_copyright
           >> d->_template >> d->_javascript >> d->_serverscript >> d->_action >> sessionTimeout
           >> d->_keys >> d->_sessionRedirect >> d->_description >> d->_opengraph >> &txtData >> &txtData
           >> langs;
        langs.removeOne( L1( "C" ) );
        langs.append( L1( "en" ) );
        setLanguages( langs );
        setDefaultLanguage( L1( "en" ) );
        setSessionTimeout( sessionTimeout );
        if ( attributes & AApplication ) dsd >> _menuEntries;
        if ( attributes & ABgImage ){
            qint16 imageOptions;
            qint32 bgImgXOff, bgImgYOff;
            dsd >> imageOptions >> d->_bgImage >> bgImgXOff >> bgImgYOff;
            setBgImageOptions( static_cast<quint8>(imageOptions) );
            setBgImageXOff( bgImgXOff );
            setBgImageYOff( bgImgYOff );
        }
    }
    QDataStream dse( &editorData, QIODevice::ReadOnly );
    dse.setVersion( PHI_DSV );
    {
        quint8 geom;
        dse >> geom;
        setGeometry( static_cast<Geometry>(geom) );
        QColor c;
        QList <PHIPalette::ColorRole> roles;
        roles << PHIPalette::User1_100 << PHIPalette::User2_100 << PHIPalette::User3_100
              << PHIPalette::User4_100 << PHIPalette::User5_100 << PHIPalette::User6_100;
        foreach ( PHIPalette::ColorRole r, roles ) {
            dse >> c;
            _pal.setColor( r, c );
        }
    }
    // map session options
    {
        quint8 opts( 0 );
        if ( attributes & ARequiresSession ) opts |= SRequiresSession;
        if ( attributes & ARequiresLogin ) opts |= SRequiresLogin;
        if ( attributes & ACreateSession ) opts |= SCreateSession;
        if ( attributes & ASetCookie ) opts |= SSessionCookie;
        setSessionOptions( opts );
    }
    // map attributes
    {
        _flags=FNone;
        if ( attributes & AStyleSheet ) _flags |= FUseCSS;
        if ( attributes & AJavascript ) _flags |= FJavaScript;
        if ( attributes & ADatabase ) _flags |= FUseDB;
        if ( attributes & AServerscript ) _flags |= FServerScript;
        if ( attributes & AApplication ) _flags |= FApplicationMode;
        if ( attributes & AUseTemplatePalette ) _flags |= FUseMasterPalette;
        if ( attributes & ANoSystemCSS ) _flags |= FNoSystemCSS;
        if ( attributes & ADbSettingsFromFile ) _flags |= FDBFile;
        if ( attributes & ANoUiCSS ) _flags |= FNoUiThemeCSS;
        if ( attributes & ADocumentLeft ) _flags |= FPageLeftAlign;
        if ( attributes & AUseOpenGraph ) _flags |= FUseOpenGraph;
        if ( attributes & ANoUnderlineLinks ) _flags |= FNoUnderlinedLinks;
        if ( extensions & EHidePhiMenu ) _flags |= FHidePhiMenu;
    }
    // store old Serverscript modules style
    {
        if ( modules ) {
            setServerModules( modules );
            _flags |= FServerModulesCombat;
        }
    }
    return itemCount;
}
