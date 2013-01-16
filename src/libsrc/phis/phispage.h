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
#ifndef PHISPAGE_H
#define PHISPAGE_H

#include <QStringList>
#include <QDataStream>
#include <QColor>
#include <QPixmap>
#include <QFlags>
#include <QByteArray>
#include <QPalette>
#include <QUrl>
#include <QMutex>
#include <QSqlDatabase>
#include <QHash>
#include <QIcon>
#include "phipage.h"
#include "phibasepage.h"
#include "phidatasources.h"
#include "phi.h"
#include "phidataparser.h"
#include "phis.h"

class PHISEXPORT PHISDatabase
{
public:
    QString _name;
    QString _host;
    QString _user;
    QString _passwd;
    QString _connectOptions;
    QString _driver;
    qint32 _port;
};

class PHISEXPORT PHISPageMenuEntry : public PHIPageMenuEntry
{
    friend PHISEXPORT QDataStream& operator<<( QDataStream&, const PHISPageMenuEntry& );
    friend PHISEXPORT QDataStream& operator>>( QDataStream&, PHISPageMenuEntry& );

public:
    PHISPageMenuEntry( const QString &id, const QString &parent, const QPixmap &pix,
        Options options, const PHITextData* );
    PHISPageMenuEntry();
    PHISPageMenuEntry( const PHISPageMenuEntry& );
    virtual ~PHISPageMenuEntry();
    PHISPageMenuEntry& operator=( const PHISPageMenuEntry &e );

    inline const PHITextData *textData() const { return _textData; }
    inline PHIPageMenuEntry parse( const PHIDataParser *parser ) const {
        QString text=parser->text( "#-", _textData );
        return PHIPageMenuEntry( _id, _parent, _img, text.toUtf8(), _options );
    }

protected:
    PHITextData *_textData;
};

PHISEXPORT QDataStream& operator<<( QDataStream&, const PHISPageMenuEntry& );
PHISEXPORT QDataStream& operator>>( QDataStream&, PHISPageMenuEntry& );

class PHISEXPORT PHISPage : public PHIPage
{
public:
    PHISPage();
    virtual ~PHISPage();
    PHISPage( const PHISPage& );
    PHISPage& operator=( const PHISPage& );

    inline virtual quint16 itemCount() const { return _newItemCount; }

    QDataStream& save( QDataStream& ) const;
    QDataStream& load( QDataStream&, bool processEditorData );
    void setDatabase( const PHISDatabase &db );
    QColor color() const;

    inline QByteArray internalId() const { return _id; }
    inline QString id() const { return QString::fromUtf8( _id ); }
    inline void setId( const QString &id ) { _id=id.toUtf8(); }
    inline QStringList languages() const { return _languages; }
    inline void setLanguages( const QStringList &langs ) { _languages=langs; }
    inline QString dbName() const { return _dbName; }
    inline QString dbHost() const { return _dbHost; }
    inline QString dbPasswd() const { return _dbPasswd; }
    inline QString dbUser() const { return _dbUser; }
    inline QString dbConnectOptions() const { return _dbOptions; }
    inline QString dbDriver() const { return _dbDriver; }
    inline qint32 dbPort() const { return _dbPort; }

    inline PHITextData* titleData() const { return _title; }
    inline PHITextData* styleSheetData() const { return _styleSheet; }
    inline PHITextData* authorData() const { return _author; }
    inline PHITextData* versionData() const { return _version; }
    inline PHITextData* companyData() const { return _company; }
    inline PHITextData* copyrightData() const { return _copyright; }
    inline PHITextData* templatePageData() const { return _template; }
    inline PHITextData* javascriptData() const { return _javascript; }
    inline PHITextData* serverscriptData() const { return _serverscript; }
    inline PHITextData* actionData() const { return _action; }
    inline PHITextData* keysData() const { return _keys; }
    inline PHITextData* sessionRedirectData() const { return _sessionRedirect; }
    inline PHITextData* descriptionData() const { return _description; }
    inline PHITextData* opengraphData() const { return _opengraph; }
    //inline PHITextData* shortDayNamesData() const { return _shortDayNames; }
    //inline PHITextData* monthNamesData() const { return _monthNames; }
    
    inline qint32 sessionTimeout() const { return _sessionTimeout; }
    inline void setSessionTimeout( qint32 t ) { _sessionTimeout=t; }
    inline quint32 width() const { return _width; }
    inline void setWidth( quint32 w ) { _width=w; }
    inline quint32 height() const { return _height; }
    inline void setHeight( quint32 h ) { _height=h; }
    inline void setItemCount( quint16 c ) { _newItemCount=c; }
    inline QList<QByteArray> ids() const { return _ids; }
    inline void addId( const QByteArray &id ) { _ids.append( id ); }
    inline PHIPage::Attributes attributes() const { return _attributes; }
    inline void setAttribute( PHIPage::Attribute a, bool set=true ) {
        set ? _attributes |= a : _attributes &= ~a; }
    inline PHIPage::Extensions extensions() const { return _extensions; }
    inline void setExtension( PHIPage::Extension e, bool set=true ) {
        set ? _extensions |= e : _extensions &= ~e; }
    inline void setFont( const QFont &f ) { _font=f; }
    inline QFont font() const { return _font; }
    inline void setPalette( const QPalette &p ) { _palette=p; }
    inline QPalette palette() const { return _palette; }
    inline void setDefinedColors( const PHIColorHash &h ) { _colHash=h; }
    inline PHIColorHash definedColors() const { return _colHash; }
    inline ScriptModules scriptModules() const { return _scriptModules; }
    inline void setScriptModule( PHIPage::ScriptModule s, bool set=true ) {
        set ? _scriptModules |= s : _scriptModules &= ~s; }
    inline void setBgColor( const QColor &c ) { _bgColor=c; }
    inline QColor bgColor() const { return _bgColor; }
    inline Geometry geometry() const { return _geometry; }
    inline void setGeometry( Geometry g ) { _geometry=g; }
    inline QColor color( quint8 id ) const { return _colHash.value( id ); }
    inline void setDbFileName( const QString &f ) { _dbFileName=f; }
    inline QString dbFileName() const { return _dbFileName; }
    inline QList <PHISPageMenuEntry> menuEntries() const { return _menuEntries; }
    inline void setMenuEntries( const QList <PHISPageMenuEntry> &list ) { _menuEntries=list; }
    inline QIcon icon() const { return QIcon( QPixmap::fromImage( _image ) ); }
    inline void setIcon( const QIcon &icon ) { _image=icon.pixmap( 48, 48 ).toImage(); }
    inline QImage image() const { return _image; }

protected:
    QDataStream& dynamicData( QDataStream& ) const;
    QDataStream& setDynamicData( QDataStream& );
    QDataStream& editorData( QDataStream& ) const;
    QDataStream& setEditorData( QDataStream& );

protected:
    QString _dbName, _dbHost, _dbPasswd, _dbUser, _dbDriver, _dbOptions, _dbFileName;
    PHITextData *_title, *_styleSheet, *_author, *_version, *_company, *_copyright, *_template,
        *_javascript, *_serverscript, *_action, *_keys, *_sessionRedirect, *_description, *_opengraph;
    //    , *_shortDayNames, *_monthNames;
    Geometry _geometry;
    QStringList _languages;
    qint32 _sessionTimeout, _dbPort;
    mutable QByteArray _dynamicData, _eventData, _editorData;
    QList <QByteArray> _ids;
    PHIColorHash _colHash;
    quint16 _newItemCount;
    QList <PHISPageMenuEntry> _menuEntries;
};

#endif // PHISPAGE_H
