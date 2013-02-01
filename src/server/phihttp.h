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
#ifndef PHIHTTP_H
#define PHIHTTP_H

#include <QByteArray>
#include <QTcpSocket>
#include <QSslError>
#include <QTimer>
#include <QTemporaryFile>
#include "phiobject.h"
#include "phiresponserec.h"
#include "phiserverrequest.h"
#include "phiserverrec.h"
#include "phierror.h"

class PHIHttp : public PHIObject
{
    Q_OBJECT

public:
    enum State { Connected, Reading, Sending, Closing };
    enum Content { Empty, MultipartEncoded, UrlEncoded, PhiEncoded };
    explicit PHIHttp( PHIObject *parent=0 );
    virtual ~PHIHttp();

    PHIRC init( qintptr socketDesc, bool usessl, QString &err );

protected:
    PHIRC sendResponse( QString &err );
    PHIRC readMultipartEncoded( QString &err );
    PHIRC readUrlEncoded( QString &err );
    PHIRC readPhiEncoded( QString &err );
    PHIRC parseRequestHeader( const QString &headerStr, QString &err ); 
    void sendError( PHIRC, const QString &reason );

private:
    QTcpSocket *_socket;
    bool _readHeader, _newPart, _readPart, _addPart;
    State _state;
    Content _content;
    QString _headerStr;
    qint64 _bytesDone;
    QTimer *_timer;
    PHIResponseRec _resp;
    PHIServerRequest *_req;
    PHIServerRec _server;
    QByteArray _contentEntity, _boundary, _contentType, _name;
    QTemporaryFile *_tmpFile;
    int _dbConnId;

protected slots:
    void readyRead();
    void slotSslErrors( const QList<QSslError>& );

signals:
    void close();
    void processServiceCommand( int code ); //needed for Mac OS X
};

#endif // PHIHTTP_H
