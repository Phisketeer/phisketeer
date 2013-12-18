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
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PHIBASEPAGE_H
#define PHIBASEPAGE_H

#include <QObject>
#include <QVariant>
#include <QHash>
#include <QDataStream>
#include <QScriptProgram>
#include <QScriptValue>
#include <QScriptEngine>
#include "phi.h"
#include "phipagemenuentry.h"
#include "phipalette.h"

class PHIBaseItem;
class PHIDynPageData;
class PHITextData;
class PHIImageData;
class PHIDataParser;
class PHIRequest;

class PHIDynPageData
{
public:
    PHIDynPageData();
    ~PHIDynPageData();
    PHIDynPageData( const PHIDynPageData& );
    PHIDynPageData& operator=( const PHIDynPageData& );
    bool operator==( const PHIDynPageData &d );
    inline bool operator!=( const PHIDynPageData &d ) { return !operator==(d); }

    PHITextData *_title, *_styleSheet, *_author, *_version, *_company, *_copyright, *_template,
        *_javascript, *_serverscript, *_action, *_keys, *_sessionRedirect, *_description,
        *_opengraph;
    PHIImageData *_bgImage;
};

class PHIEXPORT PHIBasePage : public QObject
{
    // The stream operators are used for sending the page data over TCP/IP in Phi mode only
    friend PHIEXPORT QDataStream& operator<<( QDataStream&, const PHIBasePage* );
    friend PHIEXPORT QDataStream& operator>>( QDataStream&, PHIBasePage* );

    Q_OBJECT
    Q_PROPERTY( QString id READ id )
    Q_PROPERTY( QString title READ title WRITE setTitle NOTIFY titleChanged )
    Q_PROPERTY( quint32 width READ width WRITE setWidth )
    Q_PROPERTY( quint32 height READ height WRITE setHeight )
    Q_PROPERTY( QString author WRITE setAuthor READ author )
    Q_PROPERTY( QString company WRITE setCompany READ company )
    Q_PROPERTY( QString version WRITE setVersion READ version )
    Q_PROPERTY( QString action WRITE setAction READ action )
    Q_PROPERTY( QString copyright WRITE setCopyright READ copyright )
    Q_PROPERTY( QString javascript WRITE setJavascript READ javascript )
    Q_PROPERTY( QString styleSheet WRITE setStyleSheet READ styleSheet )
    Q_PROPERTY( QString bgColor WRITE setBgColor READ bgColor )
    Q_PROPERTY( QString keywords WRITE setKeywords READ keywords )
    Q_PROPERTY( QString sessionRedirect WRITE setSessionRedirect READ sessionRedirect )
    Q_PROPERTY( QString description WRITE setDescription READ description )
    Q_PROPERTY( QString templatePage READ templatePage ) // to keep old code working
    Q_PROPERTY( quint16 itemCount READ itemCount )
    Q_PROPERTY( QString session WRITE setSession READ session )
    Q_PROPERTY( QString fontFamily WRITE setFontFamily READ fontFamily )
    Q_PROPERTY( QStringList itemIds READ itemIds )
    Q_PROPERTY( QStringList properties READ properties )
    // provided languages for this page
    Q_PROPERTY( QStringList languages READ languages )
    // default lang (initialized with philang or first matching lang from accepted languages of the browser)
    Q_PROPERTY( QString lang WRITE setLang READ lang )
    Q_PROPERTY( QString openGraph WRITE setOpenGraph READ openGraph )
    // Q_PROPERTY( QSize size READ size WRITE setSize )
    Q_PROPERTY( quint8 sessionOptions READ sessionOptions WRITE setSessionOptions )
    Q_PROPERTY( qint32 sessionTimeout READ sessionTimeout WRITE setSessionTimeout )

public:
    enum DataType { DTemplatePage=1, DTitle=2, DSession=3, DAuthor=4, DCompany=5,
        DVersion=6, DAction=7, DCopyright=8, DSessionRedirect=9, DJavascript=10,
        DStyleSheet=11, DKeys=12, DLanguages=13, DGridSize=14, DDescription=15,
        DSessionOptions=16, DOpenGraph=17, DBgImageUrl=18, DSessionTimeout=19,
        DBgImageOptions=20, DBgImageXOff=21, DBgImageYOff=22, DDefaultLang=23,
        DGeometry=24, DDBUser=25, DDBName=26, DDBDriver=27, DDBPasswd=28,
        DDBPort=29, DDBOptions=30, DDBHost=31, DDBFileName=32, DServerModules=33,
        DBgColor=34, DWidth=35, DHeight=36, DServerscript=37, DBgImage=38 }; // quint8
    enum Flag { FNone=0x0, FUseOwnPalette=0x1, FApplicationMode=0x2, FPageLeftAlign=0x4,
        FUseOpenGraph=0x8, FHasAction=0x10, FUseSession=0x20, FHidePhiMenu=0x40,
        FUseMasterPalette=0x80, FUseBgImage=0x100, FNoUnderlinedLinks=0x200,
        FHasMasterTemplate=0x400, FServerScript=0x800, FJavaScript=0x1000,
        FNoSystemCSS=0x2000, FNoUiThemeCSS=0x4000, FUseCSS=0x8000, FUseDB=0x10000,
        FDBFile=0x20000, FServerModulesCombat=0x40000, FHasFavicon=0x80000,
        FClient=0x100000 }; // quint32
    enum Geometry { GUnknown=0, GA4=1, GLetter=2, GCustom=3, GPhi=4, G4_3=5, G16_9=6, GiPad=7 };
    enum SessionOption { SNone=0x0, SRequiresLogin=0x1, SRequiresSession=0x2,
        SSessionCookie=0x4, SCreateSession=0x8 };
    enum ImageOption { INone=0x0, IFixed=0x1, IRepeatX=0x2, IRepeatY=0x4 };

#ifdef PHIDEBUG
    Q_DECLARE_FLAGS( Flags, Flag )
#else
    typedef quint32 Flags;
#endif

    explicit PHIBasePage( QObject *parent );
    virtual ~PHIBasePage() { delete _pageData; qDebug() << "delete" << _id; }
    PHIBasePage( const PHIBasePage &p ) : QObject( p.parent() ),
        _pageData( new PHIDynPageData( *p._pageData ) ), _id( p._id ), _currentLang( p._currentLang ),
        _width( p._width ), _height( p._height ), _variants( p._variants ), _headerExtensions( p._headerExtensions ),
        _scriptExtensions( p._scriptExtensions ), _dbName( p._dbName ), _dbHost( p._dbHost ), _dbPasswd( p._dbPasswd ),
        _dbUser( p._dbUser ), _dbDriver( p._dbDriver ), _dbOptions( p._dbOptions ),
        _dbFileName( p._dbFileName ), _dbPort( p._dbPort ), _favicon( p._favicon ),
        _font( p._font ), _bgColor( p._bgColor), _menuEntries( p._menuEntries ),
        _flags( p._flags ), _pal( p._pal ), _script( p._script ) {}
    PHIBasePage& operator=( const PHIBasePage &p );
    bool operator==( const PHIBasePage &p ); // IDE only
    inline bool operator!=( const PHIBasePage &p ) { return !operator==(p); } // IDE only
    inline Flags flags() const { return _flags; }
    inline void setFlag( Flag f, bool b=true ) { b ? _flags |= f : _flags &= ~f; }
    inline void setMenuEntries( const QList <PHIPageMenuEntry> &list ) { _menuEntries=list; }
    inline QList <PHIPageMenuEntry> menuEntries() const { return _menuEntries; }
    inline void setBgColor( const QColor &c ) { _bgColor=c; }
    inline QFont font() const { return _font; }
    inline void setFont( const QFont &f ) { _font=f; }
    inline void setLanguages( const QStringList &langs ) { _variants.insert( DLanguages, langs ); }
    inline void setDefaultLanguage( const QByteArray &l ) { _variants.insert( DDefaultLang, l ); }
    inline void setDefaultLanguage( const QString &l ) { _variants.insert( DDefaultLang, l.toLatin1() ); }
    inline QByteArray defaultLanguage() const { return _variants.value( DDefaultLang ).toByteArray(); }
    inline QByteArray currentLang() const { return _currentLang; }
    inline bool containsItemId( const QString &id ) const { return findItem( id ) ? true : false; }
    inline const PHIPalette& phiPalette() const { return _pal; }
    inline void setPhiPalette( const PHIPalette &p ) { _pal=p; emit phiPaletteChanged( _pal ); }
    inline void addMenuEntry( const PHIPageMenuEntry &e ) { _menuEntries.append( e ); }
    inline void setId( const QByteArray &id ) { _id=id; setObjectName( QString::fromLatin1( id ) ); }
    inline void setId( const QString &id ) { _id=id.toLatin1(); setObjectName( id ); }
    inline Geometry geometry() const { return static_cast<Geometry>(_variants.value( DGeometry, 0 ).value<quint8>()); }
    inline void setServerModules( qint32 s ) { _variants.insert( DServerModules, s ); }
    inline qint32 serverModules() const { return _variants.value( DServerModules, 0 ).value<qint32>(); }
    inline const QHash <quint8, QVariant>& data() const { return _variants; }
    inline void insertHtmlHeaderExtension( PHIWID wid, const QByteArray &ext ) { _headerExtensions.insert( wid, ext ); }
    inline void insertHtmlScriptExtension( PHIWID wid, const QByteArray &ext ) { _scriptExtensions.insert( wid, ext ); }
    inline const QHash <PHIWID, QByteArray>& htmlHeaderExtensions() const { return _headerExtensions; }

    void setGeometry( Geometry g );
    void setFavicon( const QImage &pix ) { _favicon=pix; }
    QImage favicon() const { return _favicon; }
    QList <PHIBaseItem*> items() const;
    PHIBaseItem* findItem( const QString &id ) const;
    PHIBaseItem* findItem( const QByteArray &id ) const;
    QColor backgroundColor() const { return _bgColor; }
    QString nextFreeId( const QString &requestedId ) const;
    QRect rect() const { return QRect( QPoint(), QSize( _width, _height ) ); }
    quint8 gridSize() const { return _variants.value( DGridSize, 16 ).value<quint8>(); }
    void setGridSize( quint8 s ) { _variants.insert( DGridSize, s ); }
    PHIByteArrayList itemIdsByteArray() const;
    quint16 load( QDataStream &in, int version, bool client=false ); // returns item count
    void save( QDataStream &out, int version, bool client=false );
    void squeeze();
    void phisParseData( const PHIDataParser &parser );
    void phisCreateData( const PHIDataParser &parser );
    void copyMasterData( const PHIBasePage *master );
    QScriptEngine* scriptEngine() const { return findChild<QScriptEngine*>(QString(), Qt::FindDirectChildrenOnly); }
    void createCSSFile( const PHIRequest *req ) const;
    void generateHtml( const PHIRequest *req, QByteArray &out ) const;
    void genJQueryThemeFile( const PHIRequest *req ) const;

    inline const QString& dbFileName() const { return _dbFileName; }
    inline void setDbFileName( const QString &fn ) { _dbFileName=fn; }
    inline const QString& dbName() const { return _dbName; }
    inline void setDbName( const QString &n ) { _dbName=n; }
    inline const QString& dbHost() const { return _dbHost; }
    inline void setDbHost( const QString &h ) { _dbHost=h; }
    inline const QString& dbPasswd() const { return _dbPasswd; }
    inline void setDbPasswd( const QString &p ) { _dbPasswd=p; }
    inline const QString& dbUser() const { return _dbUser; }
    inline void setDbUser( const QString &u ) { _dbUser=u; }
    inline const QString& dbConnectOptions() const { return _dbOptions; }
    inline void setDbConnectOptions( const QString &o ) { _dbOptions=o; }
    inline const QString& dbDriver() const { return _dbDriver; }
    inline void setDbDriver( const QString &d ) { _dbDriver=d; }
    inline qint32 dbPort() const { return _dbPort; }
    inline void setDbPort( qint32 p ) { _dbPort=p; }
    inline QImage bgImage() const { return _variants.value( DBgImage ).value<QImage>(); }
    inline void setBgImage( const QImage &img ) { _variants.insert( DBgImage, img ); }
    inline const QScriptProgram& serverScript() const { return _script; }

    // Available for scripting
public slots:
    inline QString id() const { return QString::fromLatin1( _id ); }
    inline void setTitle( const QString &s ) { _variants.insert( DTitle, s.toUtf8() ); }
    inline QString title() const { return QString::fromUtf8( _variants.value( DTitle ).toByteArray() ); }
    inline void setWidth( quint32 w ) { _width=w; emit documentSizeChanged(); }
    inline quint32 width() const { return _width; }
    inline void setHeight( quint32 h ) { _height=h; emit documentSizeChanged(); }
    inline quint32 height() const { return _height; }
    inline QSize size() const { return QSize( _width, _height ); }
    inline void setSize( const QSize &s ) { _width=s.width(); _height=s.height(); emit documentSizeChanged(); }
    inline void setSize( quint32 w, quint32 h ) { _width=w; _height=h; emit documentSizeChanged(); }
    quint16 itemCount() const;
    QStringList itemIds() const;

    inline QStringList languages() const { return _variants.value( DLanguages ).toStringList(); }
    inline QString fontFamily() const { return _font.family(); }
    inline void setFontFamily( const QString &f ) { _font.setFamily( f ); }
    inline void setSession( const QString &s ) { _variants.insert( DSession, s.toLatin1() ); }
    inline QString session() const {
        return QString::fromLatin1( _variants.value( DSession ).toByteArray() ); }
    inline QString templatePage() const {
        return QString::fromUtf8( _variants.value( DTemplatePage ).toByteArray() ); }

    inline void setAuthor( const QString &s ) { _variants.insert( DAuthor, s ); }
    inline QString author() const { return _variants.value( DAuthor ).toString(); }
    inline void setCompany( const QString &s ) { _variants.insert( DCompany, s ); }
    inline QString company() const { return _variants.value( DCompany ).toString(); }
    inline void setVersion( const QString &s ) { _variants.insert( DVersion, s ); }
    inline QString version() const { return _variants.value( DVersion ).toString(); }
    inline void setAction( const QString &s ) { _variants.insert( DAction, s.toUtf8() ); }
    inline QString action() const {
        return QString::fromUtf8( _variants.value( DAction ).toByteArray() ); }
    inline void setCopyright( const QString &s ) { _variants.insert( DCopyright, s ); }
    inline QString copyright() const { return _variants.value( DCopyright ).toString(); }
    inline void setSessionRedirect( const QString &s ) {
        _variants.insert( DSessionRedirect, s.toUtf8() ); }
    inline QString sessionRedirect() const {
        return QString::fromUtf8( _variants.value( DSessionRedirect ).toByteArray() ); }
    inline void setSessionTimeout( qint32 t ) { if ( t!=60 ) _variants.insert( DSessionTimeout, t ); else _variants.remove( DSessionTimeout ); }
    inline qint32 sessionTimeout() const { return _variants.value( DSessionTimeout, 60 ).value<qint32>(); }

    inline void setJavascript( const QString &s ) { _variants.insert( DJavascript, s.toUtf8() ); }
    inline QString javascript() const {
        return QString::fromUtf8( _variants.value( DJavascript ).toByteArray() ); }
    inline void setStyleSheet( const QString &s ) { _variants.insert( DStyleSheet, s.toUtf8() ); }
    inline QString styleSheet() const {
        return QString::fromUtf8( _variants.value( DStyleSheet ).toByteArray() ); }
    inline void setKeywords( const QString &s ) { _variants.insert( DKeys, s.toUtf8() ); }
    inline QString keywords() const {
        return QString::fromUtf8( _variants.value( DKeys ).toByteArray() ); }
    inline void setDescription( const QString &s ) { _variants.insert( DDescription, s.toUtf8() ); }
    inline QString description() const {
        return QString::fromUtf8( _variants.value( DDescription ).toByteArray() ); }
    inline void setOpenGraph( const QString &s ) { _variants.insert( DOpenGraph, s.toUtf8() ); }
    inline QString openGraph() const { return QString::fromUtf8( _variants.value( DOpenGraph ).toByteArray() ); }

    inline void setBgColor( const QString &c ) { _bgColor=QColor( c ); }
    inline QString bgColor() const { return _bgColor.name(); }
    inline void setLang( const QString &l ) { _currentLang=l.toLatin1(); emit currentLangChanged( _currentLang ); } // user can overwrite default
    inline QString lang() const { return QString::fromLatin1( _currentLang ); }

    inline QStringList properties() const { return PHI::properties( this ); }
    QScriptValue getElementById( const QString &id ) const;
    QScriptValue createElementById( quint16 type, const QString &id, qreal x, qreal y, qreal width=-1, qreal height=-1 );
    bool removeElementById( const QString &id );
    inline void setBgImageOptions( quint8 opts ) { if ( opts ) _variants.insert( DBgImageOptions, opts ); else _variants.remove( DBgImageOptions ); }
    inline quint8 bgImageOptions() const { return _variants.value( DBgImageOptions, 0 ).value<quint8>(); }
    inline void setSessionOptions( quint8 opts ) { if ( opts ) _variants.insert( DSessionOptions, opts ); else _variants.remove( DSessionOptions ); }
    inline quint8 sessionOptions() const { return _variants.value( DSessionOptions, 0 ).value<quint8>(); }

    inline void setBgImageUrl( const QString &url ) { _variants.insert( DBgImageUrl, url.toUtf8() ); }
    inline QString bgImageUrl() const {
        return QString::fromUtf8( _variants.value( DBgImageUrl ).toByteArray() ); }
    inline void setBgImageXOff( qint32 x ) { _variants.insert( DBgImageXOff, x ); }
    inline qint32 bgImageXOff() const { return _variants.value( DBgImageXOff ).toInt(); }
    inline void setBgImageYOff( qint32 y ) { _variants.insert( DBgImageYOff, y ); }
    inline qint32 bgImageYOff() const { return _variants.value( DBgImageYOff ).toInt(); }

public:
    inline PHITextData* titleData() const { return _pageData->_title; }
    inline PHITextData* styleSheetData() const { return _pageData->_styleSheet; }
    inline PHITextData* authorData() const { return _pageData->_author; }
    inline PHITextData* versionData() const { return _pageData->_version; }
    inline PHITextData* companyData() const { return _pageData->_company; }
    inline PHITextData* copyrightData() const { return _pageData->_copyright; }
    inline PHITextData* templatePageData() const { return _pageData->_template; }
    inline PHITextData* javascriptData() const { return _pageData->_javascript; }
    inline PHITextData* serverscriptData() const { return _pageData->_serverscript; }
    inline PHITextData* actionData() const { return _pageData->_action; }
    inline PHITextData* keywordsData() const { return _pageData->_keys; }
    inline PHITextData* sessionRedirectData() const { return _pageData->_sessionRedirect; }
    inline PHITextData* descriptionData() const { return _pageData->_description; }
    inline PHITextData* opengraphData() const { return _pageData->_opengraph; }
    inline PHIImageData* bgImageData() const { return _pageData->_bgImage; }

signals:
    void titleChanged( const QString &title );
    void iconChanged( const QIcon &favicon );
    void documentSizeChanged();
    void phiPaletteChanged( const PHIPalette &pal );
    void currentLangChanged( const QByteArray &lang );

private:
    quint16 loadVersion1_x( QDataStream &in, bool client=false ); // returns item count
    void saveVersion1_x( QDataStream &out, bool client );

private:
    PHIDynPageData *_pageData;
    QByteArray _id, _currentLang;
    quint32 _width, _height;
    QHash <quint8, QVariant> _variants;
    QHash <PHIWID, QByteArray> _headerExtensions, _scriptExtensions;
    QString _dbName, _dbHost, _dbPasswd, _dbUser, _dbDriver, _dbOptions, _dbFileName;
    qint32 _dbPort;
    QImage _favicon;
    QFont _font;
    QColor _bgColor;
    QList <PHIPageMenuEntry> _menuEntries;
    Flags _flags;
    PHIPalette _pal;
    QScriptProgram _script;
};

#ifdef PHIDEBUG
    Q_DECLARE_OPERATORS_FOR_FLAGS( PHIBasePage::Flags )
#endif

#endif // PHIBASEPAGE_H
