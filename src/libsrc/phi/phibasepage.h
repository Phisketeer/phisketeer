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
#include "phi.h"
#include "phipagemenuentry.h"
#include "phipalette.h"

class PHIBaseItem;
class PHITextData;
class PHIImageData;

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
    Q_PROPERTY( QString templatePage READ templatePage )
    Q_PROPERTY( quint16 itemCount READ itemCount )
    Q_PROPERTY( QString session WRITE setSession READ session )
    Q_PROPERTY( QString fontFamily WRITE setFontFamily READ fontFamily )
    Q_PROPERTY( QStringList itemIds READ itemIds )
    Q_PROPERTY( QStringList properties READ properties )
    // provided languages by current page
    Q_PROPERTY( QStringList languages READ languages )
    // default lang (initialized with philang or first matching lang from accepted languages of the browser)
    Q_PROPERTY( QString lang WRITE setLang READ lang )
    Q_PROPERTY( QString openGraph WRITE setOpenGraph READ openGraph )
    Q_PROPERTY( QSize size READ size WRITE setSize )

public:
    enum DataType { DFlags=0, DTemplatePage=1, DTitle=2, DSession=3, DAuthor=4, DCompany=5,
        DVersion=6, DAction=7, DCopyright=8, DSessionRedirect=9, DJavascript=10,
        DStyleSheet=11, DKeys=12, DLanguages=13, DGridSize=14, DDescription=15,
        DColorError=16, DColorBgError=17, DColorHover=18, DOpenGraph=19, DBgImageUrl=20,
        DBgImageOptions=21, DBgImageXOff=22, DBgImageYOff=23, DDefaultLang=24,
        DGeometry=25 }; // quint8
    enum Flag { FNone=0x0, FUseClientPalette=0x1, FApplicationMode=0x2, FPageLeftAlign=0x4,
            FBGRepeatX=0x8, FBGRepeatY=0x10, FBGFixed=0x20, FBGImg=0x40 }; // quint32
    enum Geometry { GUnknown=0, GA4=1, GLetter=2, GCustom=3, GPhi=4, G4_3=5, G16_9=6, GiPad=7 };

#ifdef PHIDEBUG
    Q_DECLARE_FLAGS( Flags, Flag )
#else
    typedef quint32 Flags;
#endif

    explicit PHIBasePage( QObject *parent );
    virtual ~PHIBasePage();
    PHIBasePage( const PHIBasePage &p );
    PHIBasePage& operator=( const PHIBasePage &p );
    bool operator==( const PHIBasePage &p );
    inline bool operator!=( const PHIBasePage &p ) { return !operator==(p); }
    inline Flags flags() const { return _flags; }
    inline void setFlag( Flag f, bool b=true ) { b ? _flags |= f : _flags &= ~f; }

    inline void setBgColor( const QColor &c ) { _bgColor=c; }
    inline bool pageLeftAligned() const { return _flags & FPageLeftAlign; }
    inline void setPageLeftAligned( bool b=true ) { b ? _flags |= FPageLeftAlign : _flags &= ~FPageLeftAlign; }
    inline bool applicationMode() const { return _flags & FPageLeftAlign; }
    inline void setApplicationMode( bool b=true ) { b ? _flags |= FApplicationMode : _flags &= ~FApplicationMode; }
    inline QFont font() const { return _font; }
    inline void setFont( const QFont &f ) { _font=f; }
    inline void setLanguages( const QStringList &langs ) { _variants.insert( DLanguages, langs ); }
    inline void setDefaultLanguage( const QByteArray &l ) { _variants.insert( DDefaultLang, l ); }
    inline void setDefaultLanguage( const QString &l ) { _variants.insert( DDefaultLang, l.toLatin1() ); }
    inline QByteArray defaultLanguage() const { return _variants.value( DDefaultLang ).toByteArray(); }
    inline bool containsItemId( const QString &id ) const { return findItem( id ) ? true : false; }
    inline PHIPalette phiPalette() const { return _pal; }
    inline void addMenuEntry( const PHIPageMenuEntry &e ) { _menuEntries.append( e ); }
    inline void setId( const QByteArray &id ) { _id=id; }
    inline void setId( const QString &id ) { _id=id.toLatin1(); }
    inline Geometry geometry() const { return static_cast<Geometry>(_variants.value( DGeometry, 0 ).value<quint8>()); }
    void setGeometry( Geometry g );
    void setFavicon( const QPixmap &pix ) { _favicon=pix.toImage(); }
    QPixmap favicon() const { return QPixmap::fromImage( _favicon ); }
    PHIBaseItem* findItem( const QString &id ) const;
    QList <PHIBaseItem*> items() const;
    QString nextFreeId( const QString &requestedId ) const;
    QRect rect() const { return QRect( QPoint(), QSize( _width, _height ) ); }
    quint8 gridSize() const { return _variants.value( DGridSize, 16 ).value<quint8>(); }
    void setGridSize( quint8 s ) { _variants.insert( DGridSize, s ); }
    void load( QDataStream &in, quint8 version );
    void save( QDataStream &out, quint8 version );

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
    QStringList itemIds() const;
    quint16 itemCount() const;

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
    inline void setLang( const QString &l ) { _currentLang=l.toLatin1(); } // user can overwrite default
    inline QString lang() const { return QString::fromLatin1( _currentLang ); }

    inline QStringList properties() const { return PHI::properties( this ); }
    inline PHIBaseItem* getElementById( const QString &id ) const { return findItem( id ); }
    PHIBaseItem* createElementById( quint16 type, const QString &id, qreal x, qreal y, qreal width=-1, qreal height=-1 );
    bool removeElementById( const QString &id );
    void setBgImageOptions( qint32 opts );
    qint32 bgImageOptions() const;

    inline void setBgImageUrl( const QString &url ) { _variants.insert( DBgImageUrl, url.toUtf8() ); }
    inline QString bgImageUrl() const {
        return QString::fromUtf8( _variants.value( DBgImageUrl ).toByteArray() ); }
    inline void setBgImageXOff( qint32 x ) { _variants.insert( DBgImageXOff, x ); }
    inline qint32 bgImageXOff() const { return _variants.value( DBgImageXOff ).toInt(); }
    inline void setBgImageYOff( qint32 y ) { _variants.insert( DBgImageYOff, y ); }
    inline qint32 bgImageYOff() const { return _variants.value( DBgImageYOff ).toInt(); }
    inline QString dbName() const { return _dbName; }
    inline QString dbHost() const { return _dbHost; }
    inline QString dbPasswd() const { return _dbPasswd; }
    inline QString dbUser() const { return _dbUser; }
    inline QString dbConnectOptions() const { return _dbOptions; }
    inline QString dbDriver() const { return _dbDriver; }
    inline qint32 dbPort() const { return _dbPort; }

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
    void titleChanged( QString );
    void documentSizeChanged();

protected:
    void loadVersion1_x( QDataStream &in );

private:
    PHIDynPageData *_pageData;
    QByteArray _id, _currentLang;
    quint32 _width, _height;
    QHash <quint8, QVariant> _variants;
    QString _dbName, _dbHost, _dbPasswd, _dbUser, _dbDriver, _dbOptions, _dbFileName;
    qint32 _dbPort;
    QImage _favicon;
    QFont _font;
    QColor _bgColor;
    QList <PHIPageMenuEntry> _menuEntries;
    Flags _flags;
    PHIPalette _pal;
};

#ifdef PHIDEBUG
    Q_DECLARE_OPERATORS_FOR_FLAGS( PHIBasePage::Flags )
#endif

PHIEXPORT QDataStream& operator<<( QDataStream&, const PHIBasePage* );
PHIEXPORT QDataStream& operator>>( QDataStream&, PHIBasePage* );

#endif // PHIBASEPAGE_H
