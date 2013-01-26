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
#ifndef PHIRESPONSEREC_H
#define PHIRESPONSEREC_H

#include <QByteArray>
#include <QString>
#include <QFile>
#include <QHash>
#include <QFlags>
#include <QDateTime>
#include "phierr.h"
#include "phis.h"

class PHISEXPORT PHIResponseRecLogEntry
{
public:
    PHIRC _rc;
    QDateTime _dt;
    int _type, _line;
    const char* _file;
    QString _desc;
};

class PHISEXPORT PHIResponseRec
{
    friend class PHIProcessor;

public:
    explicit PHIResponseRec();
    virtual ~PHIResponseRec();
    enum Option { None=0x0, SendFile=0x1, HeaderOnly=0x2, NoCache=0x4, Redirect=0x8 };
    Q_DECLARE_FLAGS( Options, Option )

    QByteArray createDocumentHeader() const;
    /** Creates a repsonse header from _headersOut.
      * @note Not used in the Apache module. */
    QByteArray createResponseHeader( PHIRC rc, const QString &serverString ) const;
    QByteArray createErrorResponse( PHIRC rc, const QString &err ) const;
    /** Creates an error repsonse header from _headersOut.
      * @note Not used in the Apache module. */
    QByteArray createErrorResponseHeader( PHIRC rc, const QString &serverString ) const;
    QByteArray timeEncoded( const QDateTime &dt ) const;

    inline qint64 contentLength() const { return _contentLength; }
    inline void setContentLength( qint64 l ) { _contentLength=l; }
    inline QByteArray contentType() const { return _contentType; }
    inline void setContentType( const QByteArray &ct ) { _contentType=ct; }
    inline PHIRC rc() const { return _error._rc; }
    inline const QHash <QByteArray, QByteArray>& headersOut() const { return _headersOut; }
    inline QList <PHIResponseRecLogEntry> logEntries() const { return _logs; }
    inline QByteArray body() const { return _body; }
    inline void setBody( const QByteArray &b ) { _body=b; }
    inline Options options() const { return _options; }
    inline QString errorDesc() const { return _error._desc; }
    inline QString fileName() const { return _fileToSend; }
    inline const char* errorFile() const { return _error._file; }
    inline int errorLine() const { return _error._line; }
    inline int errorType() const { return _error._type; }
    inline QDateTime errorDT() const { return _error._dt; }
    inline void setMinorHttpVer( quint8 ver ) { _minorHttpVer=ver; }
    inline void error( int type, const char* file, int line, const QDateTime &dt, PHIRC rc, const QString &desc )
        { _error._type=type; _error._file=file; _error._line=line; _error._dt=dt; _error._rc=rc; _error._desc=desc; }
    inline void setHeader( const QString &name, const QString &content ) {
        _headersOut.insert( name.toUtf8(), content.toUtf8() ); }
    inline void redirect( const QString &file ) { _fileToSend=file; _options=Redirect; }
    inline void setFileName( const QString &file ) { _fileToSend=file;
        file.isEmpty() ? _options &= ~SendFile : _options |= SendFile; }
    inline void notModified() { _error._rc=PHIRC_HTTP_NOT_MODIFIED; }

    void setCookie( const QString &name, const QString &value, const QDateTime &expires,
        const QString &path=QLatin1String( "/" ), const QString &domain=QString(), bool secure=false, bool discard=false );
    void setCookie( const QString &name, const QString &value, int maxage, const QString &path=QLatin1String( "/" ),
        const QString &domain=QString(), bool secure=false, bool discard=false );
    void log( int type, const char* file, int line, const QDateTime&, PHIRC rc, const QString &desc );
    void clear();

private:
    QHash <QByteArray, QByteArray> _headersOut;
    QList <PHIResponseRecLogEntry> _logs;
    PHIResponseRecLogEntry _error;
    QByteArray _contentType;
    QString _fileToSend;
    QByteArray _body;
    Options _options;
    mutable quint8 _minorHttpVer;
    qint64 _contentLength;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( PHIResponseRec::Options )

inline void PHIResponseRec::setCookie( const QString &name, const QString &value, const QDateTime &expires,
    const QString &path, const QString &domain, bool secure, bool discard )
{
    setCookie( name, value, QDateTime::currentDateTime().secsTo( expires ), path, domain, secure, discard );
}

#endif // PHIRESPONSEREC_H
