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
#ifndef PHIHTTPHEADER_H
#define PHIHTTPHEADER_H

#include <QHash>
#include <QUrl>
#include <QLocale>
#include "phierr.h"
#include "phiobject.h"

class PHIHttpHeader : public PHIObject
{
    Q_OBJECT

public:
    explicit PHIHttpHeader( PHIObject *parent=0 );
    virtual ~PHIHttpHeader();

    inline QByteArray requestHeaderValue( const QByteArray &head ) const { return _request.value( head ); }
    inline QUrl url() const { return _url; }
    inline quint8 major() const { return _major; }
    inline quint8 minor() const { return _minor; }
    inline QByteArray method() const { return _method; }
    inline QString requestedFile() const { return _requestedFile; }
    inline QLocale locale() const { return _locale; }
    PHIRC parseRequestHeader( const QString &headerStr, QString &err );
    QByteArray createErrorResponse( PHIRC, const QString& ) const;
    QByteArray createResponseHeader( PHIRC ) const;
    QByteArray createDocumentHeader() const;

private:
    QLocale _locale;
    QHash <QByteArray, QByteArray> _request;
    QUrl _url;
    quint8 _major, _minor;
    QByteArray _method;
    QString _requestedFile;
};

#endif // PHIHTTPHEADER_H
