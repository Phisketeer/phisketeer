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
#ifndef PHIERROR_H
#define PHIERROR_H
/** @file phierror.h PHIError can be used to display long error messages
 * for PHIRC reason codes. */
#include <QHash>
#include <QByteArray>
#include <QDateTime>
#include <QString>
#include <QObject>
#include "phi.h"
#include "phierr.h"

#define PHILOGTRACE 0x01, __FILE__ , __LINE__ , QDateTime::currentDateTime()
#define PHILOGERR   0x02, __FILE__ , __LINE__ , QDateTime::currentDateTime()
#define PHILOGWARN  0x04, __FILE__ , __LINE__ , QDateTime::currentDateTime()
#define PHILOGCRIT  0x08, __FILE__ , __LINE__ , QDateTime::currentDateTime()
#define PHILOGDBUG  0x10, __FILE__ , __LINE__ , QDateTime::currentDateTime()
#define PHILOGUSER  0x20, __FILE__ , __LINE__ , QDateTime::currentDateTime()

/** Display text error messages for PHIRC reason codes.
 * Strings are initialised at first call and eventually translated.
 * @ingroup server_api client_api */
class PHIEXPORT PHIError : public QObject
{
    Q_OBJECT

public:
    /** Instance of this singleton. */
    static PHIError* instance( QObject *parent=0 );
    /** Prints an error message to stderr.
     * The messge with
     * @param rc Reason code to print.
     * @param todo Action todo. */
    void print( PHIRC rc, const QString &todo="" ) const;
    // No translations for short description:
    inline const char* shortDesc( PHIRC rc ) const {
        if ( _sl.contains( rc ) ) return _sl[rc].constData();
        return QT_TR_NOOP( "Unknown reason code." );
    }
    inline QString longDesc( PHIRC rc ) const { return _ll[rc]; }

protected:
    /** Initializes the error strings.
     * Translates short and long error strings if nessesary. */
    explicit PHIError( QObject *parent );
    virtual ~PHIError();
    /** Initializes detailed error strings, translates them if necessary.
     * Called during instanciation. */
    void initLongDesc();
    /** Initializes defines as error strings.
     * Called during instanciation. */
    void initShortDesc();

private:
    static PHIError *_instance;
    QHash <int, QString> _ll;
    QHash <int, QByteArray> _sl;
};

#endif // PHIERROR_H
