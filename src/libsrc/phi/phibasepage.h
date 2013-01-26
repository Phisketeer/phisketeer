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
#ifndef PHIBASEPAGE_H
#define PHIBASEPAGE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QUuid>
#include <QHash>
#include <QApplication>
#include "phipage.h"
#include "phi.h"
#include "phibaseitem.h"

class PHIEXPORT PHIPageMenuEntry
{
    friend PHIEXPORT QDataStream& operator<<( QDataStream&, const PHIPageMenuEntry& );
    friend PHIEXPORT QDataStream& operator>>( QDataStream&, PHIPageMenuEntry& );

public:
    enum Option { None=0x0, SubMenu=0x1, Checkable=0x2, Checked=0x4, Disabled=0x8 };
#ifdef PHIDEBUG
    Q_DECLARE_FLAGS( Options, Option ) // quint8
#else
    typedef quint8 Options;
#endif

    PHIPageMenuEntry();
    PHIPageMenuEntry( const QByteArray &id, const QByteArray &parent, const QImage &image,
        const QByteArray &text, Options options );
    virtual ~PHIPageMenuEntry();
    PHIPageMenuEntry( const PHIPageMenuEntry& );
    PHIPageMenuEntry& operator=( const PHIPageMenuEntry& );

    inline QString id() const { return QString::fromUtf8( _id ); }
    inline QString parent() const { return QString::fromUtf8( _parent ); }
    inline QString text() const { return QString::fromUtf8( _text ); }
    inline Options options() const { return _options; }
    inline QImage image() const { return _img; }
    inline QPixmap pixmap() const { return QPixmap::fromImage( _img ); }

    inline void setText( const QString &text ) { _text=text.toUtf8(); }

protected:
    QByteArray _id, _parent, _text;
    QImage _img;
    Options _options;
};

#ifdef PHIDEBUG
Q_DECLARE_OPERATORS_FOR_FLAGS( PHIPageMenuEntry::Options )
#endif

PHIEXPORT QDataStream& operator<<( QDataStream&, const PHIPageMenuEntry* );
PHIEXPORT QDataStream& operator>>( QDataStream&, PHIPageMenuEntry* );

class PHIEXPORT PHIBasePage : public QObject, public PHIPage
{
    Q_OBJECT
    Q_DISABLE_COPY( PHIBasePage )

    Q_PROPERTY( QString title WRITE setTitle READ title )
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
    Q_PROPERTY( QString id READ id )
    Q_PROPERTY( quint16 itemCount READ itemCount )
    Q_PROPERTY( QString session WRITE setSession READ session )
    Q_PROPERTY( quint32 width WRITE setWidth READ width )
    Q_PROPERTY( quint32 height WRITE setHeight READ height )
    Q_PROPERTY( QString fontFamily WRITE setFontFamily READ fontFamily )
    Q_PROPERTY( QStringList itemIds READ itemIds )
    Q_PROPERTY( QStringList properties READ properties )
    // provided languages by current page
    Q_PROPERTY( QStringList languages READ languages )
    // default lang (initialized with philang or first matching lang from accepted languages of the browser)
    Q_PROPERTY( QString lang WRITE setLang READ lang )
    Q_PROPERTY( QString openGraph WRITE setOpenGraph READ openGraph )

    friend PHIEXPORT QDataStream& operator<<( QDataStream&, const PHIBasePage* );
    friend PHIEXPORT QDataStream& operator>>( QDataStream&, PHIBasePage* );

public:
    explicit PHIBasePage( const PHIPage&, QObject *parent=0 );
    explicit PHIBasePage( QObject *parent=0 );
    virtual ~PHIBasePage();
    // PHIBasePage( const PHIBasePage& );

    inline void insertElementById( const QByteArray &id, PHIBaseItem *item ) { _items.insert( id, item ); }
    inline void removeElementById( const QByteArray &id ) { PHIBaseItem *it=_items.take( id );
        if ( it ) delete it; }
    inline virtual quint16 itemCount() const { return _items.count(); }
    inline QHash <QByteArray, PHIBaseItem*> items() const { return _items; }
    inline void setTemplatePage( const QString &s ) { _variants.insert( DTemplatePage, s.toUtf8() ); }
    inline PHIPage::Attributes attributes() const { return _attributes; }
    inline PHIPage::Extensions extensions() const { return _extensions; }
    inline void addExtensions( PHIPage::Extensions e ) { _extensions |= e; }
    inline bool contains( const QByteArray &id ) const { return _items.value( id, 0 ) ? true : false; }
    inline QPalette palette() const { return _palette; }
    inline void setPalette( const QPalette &p ) { _palette=p; }
    inline void setTemplatePageOverwritesPalette( const QPalette &p )
        { _palette=p; _attributes |= PHIPage::APalette; }

    inline QFont font() const { return _font; }
    inline void setFont( const QFont &f ) { _font=f; }
    inline QImage image() const { return _image; }
    inline void setImage( const QImage &img ) { _image=img; }
    inline void setLanguages( const QStringList &langs ) { _variants.insert( DLanguages, langs ); }
    inline ScriptModules scriptModules() const { return _scriptModules; }
    inline void joinScriptModules( ScriptModules s ) { _scriptModules |=s; }
    inline void setPageUsesCalendar() { _attributes |= PHIPage::AHasCalendar; }
    inline void addMenuEntry( const PHIPageMenuEntry &e ) { _menuEntries.append( e ); }
    inline QByteArray pageId() const { return _id; }
    inline QVariant variant( DataType t ) const { return _variants.value( t ); }

    QColor additionalColor( DataType c ) const;
    bool needjQueryUI() const;

// Available for scripting in the server environment
public slots:
    inline virtual void setTitle( const QString &s ) { _variants.insert( DTitle, s.toUtf8() ); }
    inline virtual QString title() const {
        return QString::fromUtf8( _variants.value( DTitle ).toByteArray() ); }
    inline void setOpenGraph( const QString &s ) { _variants.insert( DOpenGraph, s.toUtf8() ); }
    inline QString openGraph() const {
        return QString::fromUtf8( _variants.value( DOpenGraph ).toByteArray() ); }

    QStringList itemIds() const;
    inline QStringList languages() const { return _variants.value( DLanguages ).toStringList(); }
    inline QString fontFamily() const { return _font.family(); }
    inline void setFontFamily( const QString &f ) { _font.setFamily( f ); }
    inline QString id() const { return QString::fromUtf8( _id ); }
    inline void setSession( const QString &s ) { _variants.insert( DSession, s.toUtf8() ); }
    inline QString session() const {
        return QString::fromUtf8( _variants.value( DSession ).toByteArray() ); }
    inline QString templatePage() const {
        return QString::fromUtf8( _variants.value( DTemplatePage ).toByteArray() ); }

    inline void setAuthor( const QString &s ) { _variants.insert( DAuthor, s ); }
    inline QString author() const { return _variants.value( DAuthor ).toString(); }
    inline void setCompany( const QString &s ) { _variants.insert( DCompany, s ); }
    inline QString company() const { return _variants.value( DCompany ).toString(); }
    inline void setVersion( const QString &s ) { _variants.insert( DVersion, s ); }
    inline QString version() const { return _variants.value( DVersion ).toString(); }
    inline void setAction( const QString &s ) {
        _variants.insert( DAction, s.toUtf8() ); _attributes |= PHIPage::AFormAction; }
    inline QString action() const {
        return QString::fromUtf8( _variants.value( DAction ).toByteArray() ); }
    inline void setCopyright( const QString &s ) { _variants.insert( DCopyright, s ); }
    inline QString copyright() const { return _variants.value( DCopyright ).toString(); }
    inline void setSessionRedirect( const QString &s ) {
        _variants.insert( DSessionRedirect, s.toUtf8() ); }
    inline QString sessionRedirect() const {
        return QString::fromUtf8( _variants.value( DSessionRedirect ).toByteArray() ); }

    inline void setJavascript( const QString &s ) {
        _variants.insert( DJavascript, s.toUtf8() ); _attributes |= PHIPage::AJavascript; }
    inline QString javascript() const {
        return QString::fromUtf8( _variants.value( DJavascript ).toByteArray() ); }
    inline void setStyleSheet( const QString &s ) {
        _variants.insert( DStyleSheet, s.toUtf8() ); _attributes |= PHIPage::AStyleSheet; }
    inline QString styleSheet() const {
        return QString::fromUtf8( _variants.value( DStyleSheet ).toByteArray() ); }
    inline void setKeywords( const QString &s ) {
        _variants.insert( DKeys, s.toUtf8() ); _attributes |= PHIPage::AKeywords; }
    inline QString keywords() const {
        return QString::fromUtf8( _variants.value( DKeys ).toByteArray() ); }
    inline void setDescription( const QString &s ) {
        _variants.insert( DDescription, s.toUtf8() ); }
    inline QString description() const {
        return QString::fromUtf8( _variants.value( DDescription ).toByteArray() ); }

    inline void setWidth( quint32 w ) { _width=w; }
    inline quint32 width() const { return _width; }
    inline void setHeight( quint32 h ) { _height=h; }
    inline quint32 height() const { return _height; }
    inline void setBgColor( const QString &c ) {
        _bgColor=QColor( c ); _attributes |= PHIPage::ABgColor; }
    inline QString bgColor() const { return _bgColor.name(); }
    inline void setLang( const QString &l ) { _variants.insert( DCurrentLang, l.toUtf8() ); }
    inline QString lang() const {
        return QString::fromUtf8( _variants.value( DCurrentLang ).toByteArray() ); }

    inline QStringList properties() const { return PHI::properties( this ); }
    inline virtual PHIBaseItem* getElementById( const QString &id ) const
        { return _items.value( id.toUtf8() ); }
    virtual PHIBaseItem* createElementById( quint16 type, const QString &id, qreal x, qreal y,
        qreal width=0, qreal height=0 );
    inline virtual void removeElementById( const QString &id )
        { removeElementById( id.toUtf8() ); }

protected:
    QHash <quint8, QVariant> _variants;
    QHash <QByteArray, PHIBaseItem*> _items;
    QList <PHIPageMenuEntry> _menuEntries;
};

PHIEXPORT QDataStream& operator<<( QDataStream&, const PHIBasePage* );
PHIEXPORT QDataStream& operator>>( QDataStream&, PHIBasePage* );

#endif // PHIBASEPAGE_H
