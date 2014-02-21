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
#include "phiresponserec.h"
#include "phidomitem.h"
#include "phiabstractitems.h"
#include "phisession.h"

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
    : QObject( parent ), _font( PHI::defaultFont() ), _flags( 0 )
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
}

PHIBasePage& PHIBasePage::operator=( const PHIBasePage &p )
{
    *_pageData=*(p._pageData);
    _id=p._id;
    _currentLang=p._currentLang;
    _width=p._width;
    _height=p._height;
    _variants=p._variants;
    _headerExtensions=p._headerExtensions;
    _scriptExtensions=p._scriptExtensions;
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
    // if ( _extensions!=p._extensions ) return false; //server only
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
    // if ( _dirtyFlags!=p._dirtyFlags ) return false; // server only
    if ( _pal!=p._pal ) return false;
    return true;
}

QList <PHIBaseItem*> PHIBasePage::items() const
{
    return findChildren<PHIBaseItem*>(QString(), Qt::FindDirectChildrenOnly);
}

// only available for Serverscript:
bool PHIBasePage::removeElementById( const QString &id )
{
    PHIBaseItem *it=findItem( id );
    if ( !it ) return false;
    delete it;
    return true;
}

QScriptValue PHIBasePage::getElementById( const QString &id ) const
{
    PHIBaseItem *it=findItem( id );
    if ( !it ) return scriptEngine()->undefinedValue();
    PHIDomItem *dom=new PHIDomItem( it, scriptEngine() );
    return scriptEngine()->newQObject( dom, QScriptEngine::ScriptOwnership,
        QScriptEngine::PreferExistingWrapperObject | QScriptEngine::SkipMethodsInEnumeration |
        QScriptEngine::ExcludeSuperClassContents | QScriptEngine::ExcludeDeleteLater );
}

// only available for ServerScript:
QScriptValue PHIBasePage::createElementById( PHIWID wid, const QString &id,
    qreal x, qreal y, qreal width, qreal height )
{
    if ( _flags & FClient ) return scriptEngine()->undefinedValue();
    if ( containsItemId( id ) ) return scriptEngine()->undefinedValue();
    PHIBaseItemPrivate p( PHIBaseItemPrivate::TServerItem, this, 0 );
    PHIBaseItem *it=PHIItemFactory::instance()->item( wid, p );
    if ( !it ) return scriptEngine()->undefinedValue();
    it->setId( id );
    it->setX( x );
    it->setY( y );
    if ( width > 0 ) it->setWidth( width );
    else it->setWidth( it->sizeHint( Qt::PreferredSize ).width() );
    if ( height > 0 ) it->setHeight( height );
    else it->setHeight( it->sizeHint( Qt::PreferredSize ).height() );
    // @todo: add extensions for HTML generation
    /*
    if ( it->hasHtmlExtension() ) {
        QByteArray ext, script;
        PHIWID extWid=it->htmlHeaderExtension( ext );
        if ( extWid ) insertHtmlHeaderExtension( extWid, ext );
        extWid=it->htmlScriptExtension( script );
        if ( extWid ) insertHtmlScriptExtension( extWid, script );
    }
    */
    PHIDomItem *dom=new PHIDomItem( it, scriptEngine() );
    return scriptEngine()->newQObject( dom, QScriptEngine::ScriptOwnership,
        QScriptEngine::PreferExistingWrapperObject | QScriptEngine::SkipMethodsInEnumeration |
        QScriptEngine::ExcludeSuperClassContents | QScriptEngine::ExcludeDeleteLater );
}

QStringList PHIBasePage::itemIds() const
{
    QStringList ids;
    PHIBaseItem *it;
    QList <PHIBaseItem*> list=findChildren<PHIBaseItem*>(QString(), Qt::FindDirectChildrenOnly);
    foreach( it, list ) ids.append( it->name() );
    return ids;
}

PHIByteArrayList PHIBasePage::itemIdsByteArray() const
{
    PHIByteArrayList ids;
    PHIBaseItem *it;
    QList <PHIBaseItem*> list=findChildren<PHIBaseItem*>(QString(), Qt::FindDirectChildrenOnly);
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
    return static_cast<quint16>(findChildren<PHIBaseItem*>(QString(), Qt::FindDirectChildrenOnly).count());
}

PHIBaseItem* PHIBasePage::findItem( const QString &id ) const
{
    PHIBaseItem *it;
    foreach( it, findChildren<PHIBaseItem*>(QString(), Qt::FindDirectChildrenOnly) ) {
        if ( it->name()==id ) return it;
    }
    return 0;
}

PHIBaseItem* PHIBasePage::findItem( const QByteArray &id ) const
{
    PHIBaseItem *it;
    foreach( it, findChildren<PHIBaseItem*>(QString(), Qt::FindDirectChildrenOnly) ) {
        if ( it->id()==id ) return it;
    }
    return 0;
}

void PHIBasePage::createCSSFile( const PHIRequest *req ) const
{
    QFile file( req->tmpDir()+L1( "/css/" )+id()+L1( ".css" ) );
    if ( !file.open( QIODevice::WriteOnly ) ) {
        req->responseRec()->log( PHILOGERR, PHIRC_IO_FILE_CREATION_ERROR,
            tr( "Could not write system CSS file '%1'.").arg( file.fileName() ) );
        return;
    }
    QByteArray out;
    out.reserve( 2000 );
    out+="@charset \"UTF-8\";\n";
    if ( Q_LIKELY( !(_flags & FNoSystemCSS) ) ) {
        QByteArray fnt="font-family:'"+font().family().toUtf8();
        if ( !font().lastResortFamily().isEmpty() ) {
            fnt+="','"+font().lastResortFamily().toUtf8();
        }
        fnt+="';";
        if ( font().pointSize() > -1 ) {
            fnt+="font-size:"+QByteArray::number( font().pointSize() )+"pt;";
        }
        out+="body{\n\tmargin:0;padding:0;\n";
        out+="\tbackground-color:"+_bgColor.name().toLatin1()+';';
        out+="color:"+_pal.color( PHIPalette::WindowText ).name().toLatin1()+";\n\t"
            +fnt+"\n}\na:link{text-decoration:none;color:"
            +_pal.color( PHIPalette::Link ).name().toLatin1()+"}\n"
            "a:visited{text-decoration:none;color:"
            +_pal.color( PHIPalette::LinkVisited ).name().toLatin1()+"}\n";
        if ( _flags & FNoUnderlinedLinks ) out+="a:hover{text-decoration:none}\n";
        else out+="a:hover{text-decoration:underline}\n";
        QByteArray ori="-webkit-transform-origin:0 0;-ms-transform-origin:0 0;"
            "-o-transform-origin:0 0;-moz-transform-origin:0 0;transform-origin:0 0";
        out+="input.phi{margin:0;padding:0;position:absolute;"+fnt+ori+"}\nselect.phi{position:absolute;"+fnt+ori+"}\n";
        out+="label.phi{position:relative;display:inline-block;padding-top:2px}\n";
        out+="button.phi{position:absolute;"+fnt+ori+"}\ntextarea.phi{position:absolute;"+fnt+ori+"}\n";
        out+="div.phi{position:absolute;cursor:default;"+ori+"}\nimg.phi{position:absolute;border:none;"+ori+"}\n";
        out+="table.phi{position:absolute;height:100%;width:100%;border:none;border-spacing:0;margin:0;padding:0}\n";
        out+="canvas.phi{position:absolute;"+ori+"}\nspan.phi{position:absolute;"+ori+"}\n";
        out+="object.phi{position:absolute;border:none;"+ori
            +"}\niframe.phi{position:absolute;border:none;background-color:transparent;overflow:visible;"+ori+"}\n";
        /*
        out+=".phibuttontext{color:"+_pal.color( PHIPalette::ButtonText ).name().toLatin1()+"}\n";
        out+=".phibutton{background-color:"+_pal.color( PHIPalette::Button ).name().toLatin1()+"}\n";
        out+=".phihighlight{background-color:"+_pal.color( PHIPalette::Highlight ).name().toLatin1()+"}\n";
        out+=".phihighlightedtext{color:"+_pal.color( PHIPalette::HighlightText ).name().toLatin1()+"}\n";
        out+=".phibase{background-color:"+_pal.color( PHIPalette::Base ).name().toLatin1()+"}\n";
        out+=".phitext{color:"+_pal.color( PHIPalette::Text ).name().toLatin1()+"}\n";
        out+=".phiwindow{background-color:transparent}\n";
        out+=".phiwindowtext{color:"+_pal.color( PHIPalette::WindowText ).name().toLatin1()+"}\n";
        out+=".phierror{background-color:"+_pal.color( PHIPalette::Error ).name().toLatin1()+"}\n";
        out+=".phierrortext{background-color:"+_pal.color( PHIPalette::ErrorText ).name().toLatin1()+"}\n";
        out+=".phicleartablecell{background-color:transparent;background-image:none}\n";
        */
        if ( _flags & FPageLeftAlign ) {
            out+=".phicontent{z-index:0;display:block;position:relative;left:0;top:0;width:"
                +QByteArray::number( _width )+"px;margin:0;padding:0}\n";
        } else {
            out+=".phicontent{z-index:0;display:block;position:relative;width:"
                +QByteArray::number( _width )+"px;height:"
                +QByteArray::number( _height )+"px;margin:0 auto;padding:0}\n";
        }
    }
    const PHIBaseItem *it;
    const QList <const PHIBaseItem*> children=findChildren<const PHIBaseItem*>(QString(), Qt::FindDirectChildrenOnly);
    foreach( it, children ) it->privateStaticCSS( req, out );
    out+="\n/* BEGIN custom CSS */\n";
    // User defined global page CSS
    if ( _flags & FUseCSS ) out+=_variants.value( DStyleSheet ).toByteArray();
    out+="\n/* END custom CSS */\n";
    file.write( out );
}

static QString _phireg( const QByteArray &s ) { return SL( " [^ ]*/\\*\\{" )
    +QString::fromLatin1( s )+SL( "\\}\\*/" ); }
static QString _phiurl( const QByteArray &s ) { return SL( " url(phi.phis?i=" )
    +QString::fromLatin1( s )+SL( "&t=1)" ); }

void PHIBasePage::genJQueryThemeFile( const PHIRequest *req ) const
{
    QByteArray fontarr="'"+font().family().toUtf8();
    if ( !font().lastResortFamily().isEmpty() ) fontarr+="','"+font().lastResortFamily().toUtf8();
    fontarr+="'";
    const char* png="PNG";
    if ( !QFile::exists( req->imgDir()+SL( "/uiiconscontent.png" ) ) ) {
        QImage icons( SL( ":/iconsContent" ) ); // Indexed8
        icons.save( req->imgDir()+SL( "/uiiconscontent.png" ), png );
        icons.save( req->imgDir()+SL( "/uiiconsheader.png" ), png );
        icons.load( SL( ":/iconsActive" ) );
        icons.save( req->imgDir()+SL( "/uiiconsactive.png" ), png );
        icons.load( SL( ":/iconsDefault" ) );
        icons.save( req->imgDir()+SL( "/uiiconsdefault.png" ), png );
        icons.load( SL( ":/iconsError" ) );
        icons.save( req->imgDir()+SL( "/uiiconserror.png" ), png );
        icons.load( SL( ":/iconsHighlight" ) );
        icons.save( req->imgDir()+SL( "/uiiconshighlight.png" ), png );
        icons.load( SL( ":/bgImgUrlHover" ) );
        // @todo: colorize background images to respect palette
        icons.save( req->imgDir()+SL( "/uibgimgurlhover.png" ), png );
        icons.load( SL( ":/bgImgUrlActive" ) );
        icons.save( req->imgDir()+SL( "/uibgimgurlactive.png" ), png );
        icons.load( SL( ":/bgImgUrlHeader" ) );
        icons.save( req->imgDir()+SL( "/uibgimgurlheader.png" ), png );
        icons.load( SL( ":/bgImgUrlError" ) );
        icons.save( req->imgDir()+SL( "/uibgimgurlerror.png" ), png );
        icons.load( SL( ":/bgImgUrlContent" ) );
        icons.save( req->imgDir()+SL( "/uibgimgurlcontent.png" ), png );
        icons.load( SL( ":/bgImgUrlDefault" ) );
        icons.save( req->imgDir()+SL( "/uibgimgurldefault.png" ), png );
        icons.load( SL( ":/bgImgUrlOverlay" ) );
        icons.save( req->imgDir()+SL( "/uibgimgurloverlay.png" ), png );
        icons.load( SL( ":/bgImgUrlHighlight" ) );
        icons.save( req->imgDir()+SL( "/uibgimgurlhighlight.png" ), png );
        // @todo: Check for missing image url replacements
    }
    QString theme;
    QFile f( SL( ":/ui-theme.css" ) ); //jquery ui theme CSS
    if ( f.open( QIODevice::ReadOnly ) ) {
        theme=QString::fromLatin1( f.readAll() );
        f.close();
        /*
        theme.replace( QRegExp( _phireg( "ffDefault" ) ), ": "+fontarr );
        if ( font.pointSize()>-1 ) theme.replace( QRegExp( _phireg( "fsDefault" ) ),
            ": "+QByteArray::number( font.pointSize() )+"pt" );
        */
        theme.replace( QRegExp( _phireg( "iconsError" ) ), _phiurl( "uiiconserror" ) );
        theme.replace( QRegExp( _phireg( "iconsContent" ) ), _phiurl( "uiiconscontent" ) );
        theme.replace( QRegExp( _phireg( "iconsHeader" ) ), _phiurl( "uiiconsheader" ) );
        theme.replace( QRegExp( _phireg( "iconsDefault" ) ), _phiurl( "uiiconsdefault" ) );
        theme.replace( QRegExp( _phireg( "iconsHighlight" ) ), _phiurl( "uiiconshighlight" ) );
        theme.replace( QRegExp( _phireg( "iconsActive" ) ), _phiurl( "uiiconsactive" ) );
        theme.replace( QRegExp( _phireg( "iconsHover" ) ), _phiurl( "uiiconsactive" ) );

        theme.replace( QRegExp( _phireg( "bgImgUrlContent" ) ), _phiurl( "uibgimgurlcontent" ) );

        theme.replace( QRegExp( _phireg( "bgImgUrlHover" ) ), _phiurl( "uibgimgurlhover" ) );
        theme.replace( QRegExp( _phireg( "bgImgUrlActive" ) ), _phiurl( "uibgimgurlactive" ) );
        theme.replace( QRegExp( _phireg( "bgImgUrlHeader" ) ), _phiurl( "uibgimgurlheader" ) );
        theme.replace( QRegExp( _phireg( "bgImgUrlError" ) ), _phiurl( "uibgimgurlerror" ) );
        theme.replace( QRegExp( _phireg( "bgImgUrlDefault" ) ), _phiurl( "uibgimgurldefault" ) );
        theme.replace( QRegExp( _phireg( "bgImgUrlOverlay" ) ), _phiurl( "uibgimgurloverlay" ) );
        theme.replace( QRegExp( _phireg( "bgImgUrlHighlight" ) ), _phiurl( "uibgimgurlhighlight" ) );

        theme.replace( QRegExp( _phireg( "fcDefault" ) ),
            QLatin1Char( ' ' )+_pal.color( PHIPalette::WindowText ).name() );
        theme.replace( QRegExp( _phireg( "fcActive" ) ),
            QLatin1Char( ' ' )+_pal.color( PHIPalette::ErrorText ).name() );
        theme.replace( QRegExp( _phireg( "fcHover" ) ),
            QLatin1Char( ' ' )+_pal.color( PHIPalette::Link ).name() );
        theme.replace( QRegExp( _phireg( "fcHighlight" ) ),
            QLatin1Char( ' ' )+_pal.color( PHIPalette::HighlightText ).name() );
        theme.replace( QRegExp( _phireg( "bgColorHighlight" ) ),
            QLatin1Char( ' ' )+_pal.color( PHIPalette::Highlight ).name() );
        theme.replace( QRegExp( _phireg( "fcHeader" ) ),
            QLatin1Char( ' ' )+_pal.color( PHIPalette::ButtonText ).name() );
        theme.replace( QRegExp( _phireg( "bgColorHeader" ) ),
            QLatin1Char( ' ' )+_pal.color( PHIPalette::Button ).name() );
        theme.replace( QRegExp( _phireg( "fcError" ) ),
            QLatin1Char( ' ' )+_pal.color( PHIPalette::ErrorText ).name() );
        theme.replace( QRegExp( _phireg( "bgColorError" ) ),
            QLatin1Char( ' ' )+_pal.color( PHIPalette::Error ).name() );
    }
    f.setFileName( req->tmpDir()+SL( "/css/" )+id()+SL( "-theme.css" ) );
    if ( Q_UNLIKELY( !f.open( QIODevice::WriteOnly ) ) ) {
        req->responseRec()->log( PHILOGERR, PHIRC_IO_FILE_CREATION_ERROR,
            tr( "Could not create theme '%1'." ).arg( f.fileName() ) );
    } else {
        f.write( theme.toLatin1() );
    }
}

void PHIBasePage::generateHtml( const PHIRequest *req, QByteArray &out ) const
{
    static const QByteArray eht="\">\n";
    if ( Q_LIKELY( req->agentFeatures() & PHIRequest::HTML5 ) ) {
        out+=BL( "<!DOCTYPE HTML>\n" );
    } else {
        out+=BL( "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\"\n\
\"http://www.w3.org/TR/html4/strict.dtd\">\n" );
    }
    out+=BL( "<html>\n<head>\n\t<title>" )+_variants.value( DTitle ).toByteArray()
        +BL( "</title>\n\t<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">\n\t" )
        +BL( "<meta name=\"generator\" content=\"Phis " )+PHI::libVersion().toLatin1()+'/'
        +req->userEngine().toLatin1()+' '+QByteArray::number( req->engineMajorVersion() )
        +'.'+QByteArray::number( req->engineMinorVersion() )+eht;
    if ( _variants.value( DAuthor ).isValid() )
        out+=BL( "\t<meta name=\"author\" content=\"" )+_variants.value( DAuthor ).toByteArray()+eht;
    if ( _variants.value( DCompany ).isValid() )
        out+=BL( "\t<meta name=\"publisher\" content=\"" )+_variants.value( DCompany ).toByteArray()+eht;
    if ( _variants.value( DCopyright ).isValid() )
        out+=BL( "\t<meta name=\"rights\" content=\"" )+_variants.value( DCopyright ).toByteArray()+eht;
    if ( _variants.value( DVersion ).isValid() )
        out+=BL( "\t<meta name=\"page-version\" content=\"" )+_variants.value( DVersion ).toByteArray()+eht;
    if ( _variants.value( DDescription ).isValid() )
        out+=BL( "\t<meta name=\"description\" content=\"" )+_variants.value( DDescription ).toByteArray()+eht;
    if ( _variants.value( DKeys ).isValid() )
        out+=BL( "\t<meta name=\"keywords\" content=\"" )+_variants.value( DKeys ).toByteArray()+eht;
    if ( Q_LIKELY( !(_flags & FNoUiThemeCSS ) ) ) {
        out+=BL( "\t<link rel=\"stylesheet\" type=\"text/css\" href=\"phi.phis?s=" )+_id+BL( "-theme" )+eht;
        out+=BL( "\t<link rel=\"stylesheet\" type=\"text/css\" href=\"phi.phis?s=ui-core" )+eht;
    }
    if ( Q_LIKELY( !(_flags & FNoSystemCSS) ) )
        out+=BL( "\t<link rel=\"stylesheet\" type=\"text/css\" href=\"phi.phis?s=" )+_id+eht;
    if ( Q_LIKELY( _flags & FHasFavicon ) )
        out+=BL( "\t<link rel=\"shortcut icon\" href=\"phi.phis?i=" )+_id+BL( ".ico&amp;t=1" )+eht;
    if ( Q_UNLIKELY( req->agentFeatures() & PHIRequest::IE678 ) ) {
        out+=BL( "\t<script type=\"text/javascript\" src=\"phi.phis?j=jqueryiefix\"></script>\n" );
        out+=BL( "\t<script type=\"text/javascript\" src=\"phi.phis?j=jquerymigrate\"></script>\n" );
    } else out+=BL( "\t<script type=\"text/javascript\" src=\"phi.phis?j=jquery\"></script>\n" );
    out+=BL( "\t<script type=\"text/javascript\" src=\"phi.phis?j=ui-core\"></script>\n"
        "\t<script type=\"text/javascript\" src=\"phi.phis?j=ui-effects\"></script>\n"
        "\t<script type=\"text/javascript\" src=\"phi.phis?j=ui-button\"></script>\n"
        "\t<script type=\"text/javascript\" src=\"phi.phis?j=transit\"></script>\n"
        "\t<script type=\"text/javascript\" src=\"phi.phis?j=datefmt\"></script>\n"
        "\t<script type=\"text/javascript\" src=\"phi.phis?j=phibase\"></script>\n" );
    QByteArray script, indent="\t", tmp;
    script.reserve( 4*1024 );
    foreach ( tmp, _scriptExtensions.values() ) script+=tmp;
    foreach ( tmp, _headerExtensions.values() ) out+='\t'+tmp;
    out+=BL( "</head>\n<body" );
    if ( _flags & FUseBgImage ) {
        out+=BL( " style=\"background-image:url(/phi.phis?i=" );
        out+=_variants.value( DBgImageUrl ).toByteArray()+BL( "&t=1);" );
        quint8 opts=bgImageOptions();
        if ( opts & IRepeatX ) {
            if ( opts & IRepeatY ) out+=BL( "background-repeat:repeat;" );
            else out+=BL( "background-repeat:repeat-x;" );
        } else if ( opts & IRepeatY ) {
            out+=BL( "background-repeat:repeat-y;" );
        } else out+=BL( "background-repeat:no-repeat;" );
        if ( opts & IFixed ) out+=BL( "background-attachment:fixed" );
        out+='"';
        script+=BL( "jQuery(window).resize(function(){\n\tvar off=jQuery('#phihtml').offset();\n" );
        script+=BL( "\tvar bgx=off.left+" )+QByteArray::number( bgImageXOff() )
            +BL( ",bgy=off.top+" )+QByteArray::number( bgImageYOff() );
        script+=BL( ";\n\tjQuery('body').css('background-position',bgx+'px '+bgy+'px');\n}).resize();\n" );
    }
    out+=BL( ">\n<div id=\"phihtml\" class=\"phicontent\">\n" );
    if ( _flags & FHasAction ) {
        out+=BL( "<form id=\"phiform\" action=\"" )+_variants.value( DAction ).toByteArray()
            +BL( "\" method=\"post\" enctype=\"multipart/form-data\""
                 " onsubmit=\"return phi.onsubmit();\" accept-charset=\"utf-8\">\n" );
    }
    const PHIBaseItem *it;
    const QList <const PHIBaseItem*> children=findChildren<const PHIBaseItem*>(QString(), Qt::FindDirectChildrenOnly);
    foreach( it, children ) {
        if ( it->isChild() ) continue; // handled by layouts
        it->html( req, out, script, indent );
    }
    if ( _flags & FHasAction ) {
        if ( !(_flags & FHasPhiLangItem) && currentLang()!=BL( "C" ) )
            out+=indent+BL( "<input type=\"hidden\" name=\"philang\" value=\"" )+currentLang()+BL( "\">\n" );
        if ( _variants.value( DSession ).isValid() ) {
            out+=indent+BL( "<input type=\"hidden\" name=\"phisid\" value=\"" )+_variants.value( DSession ).toByteArray()+BL( "\">\n" );
        }
        out+=BL( "</form>\n" );
    }
    out+=BL( "</div>\n<script type=\"text/javascript\">\n/* <![CDATA[ */\n" );
    out+=BL( "var phi=new Phi('" )+_currentLang+BL( "','" )
        +_variants.value( DSession ).toByteArray()+BL( "');\n" )+script;
    if ( _flags & FJavaScript ) {
        out+=BL( "/* BEGIN custom script */\n" )+_variants.value( DJavascript ).toByteArray();
        out+=BL( "\n/* END custom script */\n" );
    }
    out+=BL( "/* ]]> */\n</script>\n</body>\n</html>\n" );
}

// only run once at loading time from server
void PHIBasePage::phisCreateData( const PHIDataParser &p )
{
    squeeze();
    if ( _flags & FUseBgImage ) p.createImages( bgImageData() );
    else { delete _pageData->_bgImage; _pageData->_bgImage=0; }
    // @todo: reorder data checking for better caching performance:
    if ( companyData()->isUnparsedStatic() ) {
        if ( !companyData()->text().isEmpty() ) _variants.insert( DCompany, companyData()->variant() );
        delete _pageData->_company; _pageData->_company=0;
    }
    _variants.insert( DTitle, p.text( titleData() ) );
    if ( titleData()->isUnparsedStatic() ) {
        delete _pageData->_title; _pageData->_title=0;
        if ( _variants.value( DTitle ).toByteArray().isEmpty() ) _variants.remove( DTitle );
    }
    if ( _flags & FUseCSS ) {
        _variants.insert( DStyleSheet, p.text( styleSheetData() ) );
        if ( styleSheetData()->isUnparsedStatic() ) {
            delete _pageData->_styleSheet; _pageData->_styleSheet=0;
        }
    } else { delete _pageData->_styleSheet; _pageData->_styleSheet=0; }
    if ( versionData()->isUnparsedStatic() ) {
        if ( !versionData()->text().isEmpty() ) _variants.insert( DVersion, versionData()->variant() );
        delete _pageData->_version; _pageData->_version=0;
    }
    if ( opengraphData()->isUnparsedStatic() ) {
        if ( !opengraphData()->text().isEmpty() ) _variants.insert( DOpenGraph, opengraphData()->variant() );
        delete _pageData->_opengraph; _pageData->_opengraph=0;
    }
    if ( _flags & FJavaScript ) {
        _variants.insert( DJavascript, p.text( javascriptData() ) );
        if ( Q_LIKELY( javascriptData()->isUnparsedStatic() ) ) {
            delete _pageData->_javascript; _pageData->_javascript=0;
        }
    } else {  delete _pageData->_javascript; _pageData->_javascript=0; }
    if ( _flags & FServerScript ) {
        QByteArray arr=p.text( serverscriptData() ).toByteArray();
        if ( Q_LIKELY( serverscriptData()->isUnparsedStatic() ) ) {
            if ( Q_LIKELY( !arr.isEmpty() ) ) _script=QScriptProgram( QString::fromUtf8( arr ) );
            delete _pageData->_serverscript; _pageData->_serverscript=0;
        }
    } else { delete _pageData->_serverscript; _pageData->_serverscript=0; }
    if ( authorData()->isUnparsedStatic() ) {
        if ( !authorData()->text().isEmpty() ) _variants.insert( DAuthor, authorData()->variant() );
        delete _pageData->_author; _pageData->_author=0;
    }
    if ( copyrightData()->isUnparsedStatic() ) {
        if ( !copyrightData()->text().isEmpty() ) _variants.insert( DCopyright, copyrightData()->variant() );
        delete _pageData->_copyright; _pageData->_copyright=0;
    }
    if ( sessionRedirectData()->isUnparsedStatic() ) {
        if ( !sessionRedirectData()->text().isEmpty() ) _variants.insert( DSessionRedirect, sessionRedirectData()->variant() );
        delete _pageData->_sessionRedirect; _pageData->_sessionRedirect=0;
    }
    if ( actionData()->isUnparsedStatic() ) {
        if ( !actionData()->text().isEmpty() ) _variants.insert( DAction, actionData()->variant() );
        delete _pageData->_action; _pageData->_action=0;
    }
    if ( templatePageData()->isUnparsedStatic() ) {
        if ( !templatePageData()->text().isEmpty() ) _variants.insert( DTemplatePage, templatePageData()->variant() );
        delete _pageData->_template; _pageData->_template=0;
    }
    if ( descriptionData()->isUnparsedStatic() ) {
        if ( !descriptionData()->text().isEmpty() ) _variants.insert( DDescription, descriptionData()->variant() );
        delete _pageData->_description; _pageData->_description=0;
    }
    if ( keywordsData()->isUnparsedStatic() ) {
        if ( !keywordsData()->text().isEmpty() ) _variants.insert( DKeys, keywordsData()->variant() );
        delete _pageData->_keys; _pageData->_keys=0;
    }
    if ( !_favicon.isNull() ) {
        _favicon.save( p.request()->imgDir()+QLatin1Char( '/' )+id()+L1( ".ico" ), "ICO" );
        _flags |= FHasFavicon;
    }
}

void PHIBasePage::phisParseData( const PHIDataParser &p )
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
        _variants.insert( DStyleSheet, p.text( styleSheetData() ) );
    }
    if ( _flags & FUseBgImage ) {
        _variants.insert( DBgImageUrl, p.imagePath( bgImageData() ) );
    }
    for ( int i=0; i<_menuEntries.count(); i++ ) {
        _menuEntries[i].setText( p.text( _menuEntries[i].textData() ).toByteArray() );
    }
}

void PHIBasePage::copyMasterData( const PHIBasePage *m )
{
    if ( _flags & FUseMasterPalette ) {
        _pal=m->phiPalette();
        _flags |= FUseOwnPalette;
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

void PHIBasePage::save( QDataStream &out, int version, bool client )
{
    if ( version<3 ) return saveVersion1_x( out, client );
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

    out << _id << static_cast<quint32>(_flags) << _pal;
    if ( !client ) out << _pageData;
    out << static_cast<quint16>( itemCount() ) << _variants
        << _favicon << _font << _menuEntries;
}

quint16 PHIBasePage::load( QDataStream &in, int version, bool client )
{
    PHIBaseItem *it;
    foreach ( it, items() ) {
        PHIAbstractLayoutItem *lit=qobject_cast<PHIAbstractLayoutItem*>(it);
        if ( lit ) lit->breakLayout(); // break all page containing layouts
    }
    foreach( it, items() ) delete it;
    if ( Q_UNLIKELY( version<3 ) ) return loadVersion1_x( in );
    in.setVersion( PHI_DSV2 );
    quint32 flags;
    quint16 itemCount;
    in >> _id >> flags >> _pal;
    if ( !client ) in >> _pageData;
    else _flags |= FClient;
    in >> itemCount >> _variants >> _favicon >> _font >> _menuEntries;
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

quint16 PHIBasePage::loadVersion1_x( QDataStream &in, bool client )
{
    if ( client ) _flags |= FClient;
    // @todo: enable phia client loading for version 1.x
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
        qreal w=_width, h=_height;
        if ( attributes & ALandscape ) {
            _width=h;
            _height=w;
        }
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
        _flags |= FVersion1x;
        if ( attributes & ATemplate ) _flags |= FHasMasterTemplate;
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
        if ( attributes & APalette ) _flags |= FUseOwnPalette;
        if ( attributes & AFormAction ) _flags |= FHasAction;
        if ( extensions & EHidePhiMenu ) _flags |= FHidePhiMenu;
        // @todo: check AUsejQueryUI

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

void PHIBasePage::saveVersion1_x( QDataStream &out, bool client )
{
    Q_UNUSED( out )
    Q_UNUSED( client )
    // @todo: implement saving for file versions<3
}
