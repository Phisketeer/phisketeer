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
#ifndef PHISSYSTEM_H
#define PHISSYSTEM_H
#include <QCryptographicHash>
#include <QStandardPaths>
#include <QUuid>
#include <QFile>
#include "phismodule.h"
#include "phisysinfo.h"

class PHISSystemModule : public PHISModule
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.phisketeer.phis.module.system" FILE "phissystem.json")
    Q_CLASSINFO( "Author", "Marius Schumacher" )
    Q_CLASSINFO( "Url", "http://www.phisketeer.org" )
    Q_CLASSINFO( "Version", "1.0" )
    Q_CLASSINFO( "License", "LGPL" )
    Q_CLASSINFO( "Copyright", "2013 Phisys AG, 2013 Phisketeer Team" )

public:
    virtual PHISScriptObj* create( const QString &key, const PHISInterface* ) const;
    virtual QStringList keys() const;
};

class PHISSystemObj : public PHISScriptObj
{
    Q_OBJECT
    Q_PROPERTY( int exitcode READ exitCode )
    Q_PROPERTY( QString lasterror READ lastError )
    Q_PROPERTY( QString systemname READ systemName )

public:
    explicit PHISSystemObj( const PHISInterface *phisif )
        : PHISScriptObj( phisif ), _exitCode( 0 ) {}

public slots:
    inline int exitCode() const { return _exitCode; }
    inline QString lastError() const { return _lastError; }
    inline QString systemName() const { return PHISysInfo::systemString(); }
    void log( const QString &t );
    QString exec( const QString &procName, const QString &attributes=QString(),
        const QString &codec=_utf8 );
    QString exec( const QString &procName, const QStringList &atts,
        const QString &codec=_utf8 );

    inline QString md5( const QString &s ) const { return QString::fromUtf8(
        QCryptographicHash::hash( s.toUtf8(), QCryptographicHash::Md5 ).toHex() ); }
    inline QString md4( const QString &s ) const { return QString::fromUtf8(
        QCryptographicHash::hash( s.toUtf8(), QCryptographicHash::Md4 ).toHex() ); }
    inline QString sha1( const QString &s ) const { return QString::fromUtf8(
        QCryptographicHash::hash( s.toUtf8(), QCryptographicHash::Sha1 ).toHex() ); }
    inline QString sha512( const QString &s ) const { return QString::fromUtf8(
        QCryptographicHash::hash( s.toUtf8(), QCryptographicHash::Sha512 ).toHex() ); }
    inline QString sha3_512( const QString &s ) const { return QString::fromUtf8(
        QCryptographicHash::hash( s.toUtf8(), QCryptographicHash::Sha3_512 ).toHex() ); }
    inline QString createUuid() const {
        return QUuid::createUuid().toString().mid( 1, 36 ); }
    inline QString toLocaleString( const QDateTime &dt, const QString &lang=_C ) {
        return dt.toString( QLocale( toLocaleFormat( lang ) ).dateTimeFormat() ); }
    inline QString dayName( int day, const QString &lang=_C ) {
        return QLocale( toLocaleFormat( lang ) ).dayName( day+1 ); }
    inline QString monthName( int month, const QString &lang=_C ) {
        return QLocale( toLocaleFormat( lang ) ).monthName( month+1 ); }
    inline QString countryName( const QString &lang=_C ) {
        return QLocale::countryToString( QLocale( toLocaleFormat( lang ) ).country() ); }
    inline QString languageName( const QString &lang=_C ) {
        return QLocale::languageToString( QLocale( toLocaleFormat( lang ) ).language() ); }
    inline QString toFormatedString( const QDateTime &dt, const QString &format,
            const QString &lang=_C ) {
        return QLocale( toLocaleFormat( lang ) ).toString( dt, format ); }

protected:
    static QString _C, _utf8;
    QString toLocaleFormat( const QString& );
    QString _lastError;
    int _exitCode;
};

class PHISServerObj : public PHISScriptObj
{
    Q_OBJECT
    Q_PROPERTY( QString documentroot READ documentRoot )
    Q_PROPERTY( QString servername READ serverName )
    Q_PROPERTY( QString serverhost READ serverHost )
    Q_PROPERTY( QString serverdef READ serverDef )
    Q_PROPERTY( QString admin READ admin )
    Q_PROPERTY( QString localaddress READ localAddress )
    Q_PROPERTY( QString remoteaddress READ remoteAddress )
    Q_PROPERTY( QString tempdir READ tempDir )
    Q_PROPERTY( QString today READ today )
    Q_PROPERTY( QString nowutc READ nowUtc )
    Q_PROPERTY( QDateTime utc READ utc )
    Q_PROPERTY( qint32 port READ port )
    Q_PROPERTY( qint32 keepalive READ keepAlive )

public:
    explicit PHISServerObj( const PHISInterface *interface ) : PHISScriptObj( interface ) {}

public slots:
    inline QString documentRoot() const { return PHIS_IF()->documentRoot(); }
    inline QString serverName() const { return PHIS_IF()->serverName(); }
    inline QString serverHost() const { return PHIS_IF()->serverHost(); }
    inline QString serverDef() const { return PHIS_IF()->serverDef(); }
    inline QString admin() const { return PHIS_IF()->admin(); }
    inline QString localAddress() const { return PHIS_IF()->localIP(); }
    inline QString remoteAddress() const { return PHIS_IF()->remoteIP(); }
    inline QString tempDir() const { return PHIS_IF()->tempDir(); }
    inline QString today() const { return PHIS_IF()->today(); }
    inline QString nowUtc() const { return PHIS_IF()->nowUtc(); }
    inline QDateTime utc() const { return PHIS_IF()->utc(); }
    inline qint32 port() const { return PHIS_IF()->port(); }
    inline qint32 keepAlive() const { return PHIS_IF()->keepAlive(); }
    void log( const QString &t );
};

class PHISFileObj : public PHISScriptObj
{
    Q_OBJECT
    Q_PROPERTY( int readonly READ readOnly )
    Q_PROPERTY( int writeonly READ writeOnly )
    Q_PROPERTY( int readwrite READ readWrite )
    Q_PROPERTY( int append READ append )
    Q_PROPERTY( QString lasterror READ lastError )
    Q_PROPERTY( QString tempdir READ tempDir )

public:
    explicit PHISFileObj( const PHISInterface *interface ) : PHISScriptObj( interface ), _handleCounter( 0 ) {}
    virtual ~PHISFileObj();

public slots:
    inline bool exists( const QString &f ) const { return QFile::exists( f ); }
    inline bool copy( const QString &src, const QString &dest ) const { return QFile::copy( src, dest ); }
    inline int readOnly() const { return 0x01; }
    inline int writeOnly() const { return 0x02; }
    inline int readWrite() const { return 0x03; }
    inline int append() const { return 0x04; }
    inline bool isOpen( int handle ) const { return _files.contains( handle ) ?
        _files.value( handle )->isOpen() : false; }
    inline void close( int handle ) { if ( _files.contains( handle ) ) _files.value( handle )->close(); }
    inline QString lastError() const { return _error; }
    inline QString error( int handle ) const { return _files.contains( handle ) ?
        _files.value( handle )->errorString() : QString(); }
    inline bool atEnd( int handle ) const { return _files.contains( handle ) ?
        _files.value( handle )->atEnd() : true; }
    inline bool remove( const QString &f ) const { return QFile::remove( f ); }
    inline QString tempDir() const { return QStandardPaths::writableLocation( QStandardPaths::TempLocation ); }
    int open( const QString &fileName, int mode );
    bool write( int handle, const QString &data, const QString &codec=_utf8 );
    QString read( int handle, const QString &codec=_utf8 ) const;

private:
    QHash <int, QFile*> _files;
    int _handleCounter;
    mutable QString _error;
    static QString _utf8;
};

inline PHISScriptObj* PHISSystemModule::create( const QString &key, const PHISInterface *interface ) const
{
    if ( key==QLatin1String( "system" ) ) return new PHISSystemObj( interface );
    else if ( key==QLatin1String( "server" ) ) return new PHISServerObj( interface );
    else if ( key==QLatin1String( "file" ) ) return new PHISFileObj( interface );
    return 0;
}

inline QStringList PHISSystemModule::keys() const
{
    return QStringList() << QStringLiteral( "system" ) << QStringLiteral( "server" ) << QStringLiteral( "file" );
}

inline void PHISSystemObj::log( const QString &t )
{
    PHIS_LOG(PHISInterface::LTUser,t);
}

inline QString PHISSystemObj::exec( const QString &name, const QString &a, const QString &codec )
{
    QStringList args=a.split( QRegExp( QStringLiteral( "\\s" ) ), QString::SkipEmptyParts );
    return PHISSystemObj::exec( name, args, codec );
}

inline void PHISServerObj::log( const QString &t )
{
    PHIS_LOG(PHISInterface::LTUser,t);
}

#endif // PHISSYSTEM_H
