/*
#    Copyright (C) 2010-2012  Marius B. Schumacher
#    Copyright (C) 2011-2012  Phisys AG, Switzerland
#    Copyright (C) 2012  Phisketeer.org team
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
#ifndef PHIOBJECT_H
#define PHIOBJECT_H

#include <QObject>
#include <QVariant>
#include <QDateTime>
#include "phi.h"
#include "phierror.h"
#include "phiconfig.h"

extern const char *_phioutofmemory;

/** Base class for most PHI classes.
 * PHIObject provides the trace and log mechanism. All derived
 * classes can emit the log signal which protocols to the log
 * writer PHILogWriter.
 * @ingroup server_api */
class PHIObject : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( PHIObject )

protected:
    explicit PHIObject( QObject* );
    virtual ~PHIObject();

    /** Simplifies the call for PHIConfig::instance().
     * Returns the value of the specified phi server config setting.
     * @param[in] setting the key,
     * @param[in] v a default value, if @a setting is not found.
     * @return The QVariant of this setting. */
    inline QVariant configValue( const QString &setting,
        const QVariant &v=QVariant() ) const {
        return PHIConfig::instance()->configValue( setting, v );
    }
    inline const PHIConfig* config() const { return PHIConfig::instance(); }
    inline const PHIError* error() const { return PHIError::instance(); }
    inline QString errorDesc( PHIRC rc ) const { return PHIError::instance()->longDesc( rc ); }

    /** Returns formated object name. */
    QString formatObjectName( const QString &obj=QString() ) const;

protected slots:
    /** Prints reason code directly to stderr.
     * Calls @code PHIError::instance()->print( rc, todo ) @endcode
     * but provides a SLOT functionality.
     * @param[in] type ignored,
     * @param[in] file ignored,
     * @param[in] line ignored,
     * @param[in] dt ignored,
     * @param[in] rc reason code
     * @param[in] todo what todo or how to react
     * All other parameters are currently ignored. */
    void print( int type, const char* file, int line,
        const QDateTime &dt, PHIRC rc, const QString &todo );

signals:
    /** Emit this signal for tracing or logging errors to the PHILogWriter.
     * For the first @b four parameters exists macros for simplifying
     * the call: \n
     * - PHILOGTRACE for tracing,
     * - PHILOGERROR for errors,
     * - PHILOGWARN for warnings,
     * - PHILOGCRIT for critical errors.
     *
     * Example: <tt>emit log( PHILOGTRACE, PHIRC_QMGR_START, "" );</tt>
     * @param[in] type Log type
     * @param[in] file Source in which the error occured (__FILE__)
     * @param[in] line Line number (__LINE__)
     * @param[in] dt Date and time
     * @param[in] rc reason code
     * @param[in] todo what todo
     */
    void log( int type, const char* file, int line,
        const QDateTime &dt, PHIRC rc, const QString &todo ) const;
};

#endif // PHIOBJECT_H
