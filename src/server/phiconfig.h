/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2013  Phisys AG, Switzerland
#    Copyright (C) 2012-2013  Phisketeer.org team
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PHICONFIG_H
#define PHICONFIG_H

/** @file phiconfig.h PHI manager configuration and settings.
 * All configuration settings for this mgr stored in
 * <tt>/etc/xdg/Phisys/phis.conf</tt> (Unix), or in the
 * registry (Windows) can be accessed through this class. */

/** @defgroup server_api PHI server (internal)
 * This is the internal server API for PHI. You do @b not need this
 * for client programming. Use one of the client APIs instead.
 * @sa client_c_api. */

#include <QDateTime>
#include <QVariant>
#include <QReadWriteLock>
#include <QReadLocker>
#include <QAbstractSocket>
#include <QHash>
#include <QSettings>
#include "phi.h"
#include "phis.h"

/** PHI manager configuration class.
 * PHIConfig is implemented as singleton. This class holds all configuration
 * issues for this mgr including settings from <tt>/etc/xdg/PhiTechnologies/phi.conf
 * </tt> or the registry and settings from the license file.
 * @todo Implementing license file settings.
 * @note This class is thread safe.
 * @ingroup server_api */
class PHIConfig
{
    Q_DISABLE_COPY( PHIConfig )
    friend class PHIManager;

public:
    /** Instance of the config class. */
    static PHIConfig* instance();

    /** Version of the PHI libs. */
    inline QString version() const { return PHIS::libVersion(); }

    /** Mgr name of this deamon instance. */
    inline QString mgrName() const {
        QReadLocker lock( &_lock ); return _mgrName; }

    /** Maximum of creatable queue objects. */
    inline qint32 maxConnections() const {
        QReadLocker lock( &_lock ); return _maxConnections; }

    inline quint8 logFilter() const { QReadLocker lock( &_lock ); return _logFilter; }

    inline QString mimeTypesFile() const {
        QReadLocker lock( &_lock ); return _mimeTypesFile; }
    inline QString rootDir() const {
        QReadLocker lock( &_lock ); return _rootDir; }
    inline QString baseDir() const {
        QReadLocker lock( &_lock ); return _baseDir; }
    inline QString logDir() const {
        QReadLocker lock( &_lock ); return _logDir; }
    inline QString index() const {
        QReadLocker lock( &_lock ); return _index; }
    inline qint32 keepAlive() const {
        QReadLocker lock( &_lock ); return _keepAlive; }
    inline QString admin() const {
        QReadLocker lock( &_lock ); return _admin; }

    QString documentRoot( const QString &domain ) const;

    /** Accessing configuration settings.
      * Reads an @c .ini file (Unix) or the registry (Windows).
      * @return Config for @a key as a QVariant. Returns @a v if @a key is not set.*/
    QVariant configValue( const QString &key, const QVariant &v=QVariant() ) const;
    //void setConfigValue( const QString& key, const QVariant& );

    /** Initialize QSetting and configuration object.
      * @internal */
    PHIRC init( QObject* );
    /** Updates the config after a HUP signal. */
    PHIRC updateConfig();

protected:
    PHIConfig();
    virtual ~PHIConfig();

    PHIRC loadMimeTypes();

private:
    static PHIConfig *_instance;
    mutable QReadWriteLock _lock;
    QSettings *_settings;
    QString _mgrName, _index, _rootDir, _baseDir, _logDir, _admin;
    qint32 _maxConnections, _keepAlive;
    quint8 _logFilter;
    mutable QHash <QString, QString> _docRoots;
    QString _mimeTypesFile;
};

#endif // PHICONFIG_H
