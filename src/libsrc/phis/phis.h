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
#ifndef PHIS_H
#define PHIS_H

#include <QByteArray>
#include <QString>
#include <QVariant>
#include "phierr.h"

#ifdef PHISLIB
#define PHISEXPORT Q_DECL_EXPORT
#else
#define PHISEXPORT Q_DECL_IMPORT
#endif

class PHISEXPORT PHIS
{
    Q_DISABLE_COPY( PHIS )

public:
    static inline QByteArray xmlStartTag() { return _xmlStartTag; }
    static inline QByteArray doctypeHtml4() { return _doctypeHtml4; }
    static inline QByteArray doctypeHtml5() { return _doctypeHtml5; }
    static inline QByteArray doctypeXhtml() { return _doctypeXhtml; }
    static inline QByteArray htmlXmlStartTag() { return _htmlXmlStartTag; }
    static inline QByteArray htmlStartTag() { return _htmlStartTag; }
    static QString libVersion();
    static QByteArray textForHttpCode( PHIRC rc );

protected:
    PHIS();
    ~PHIS();

private:
    static QByteArray _xmlStartTag;
    static QByteArray _doctypeXhtml;
    static QByteArray _doctypeHtml4;
    static QByteArray _doctypeHtml5;
    static QByteArray _htmlStartTag;
    static QByteArray _htmlXmlStartTag;
    static QByteArray _calendarJS;
    static QByteArray _phibaseJS;
};

#endif // PHIS_H
