/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2014  Phisys AG, Switzerland
#    Copyright (C) 2012-2014  Phisketeer.org team
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
#ifndef PHISHTTP_H
#define PHISHTTP_H

#include <QByteArray>
#include <QTcpSocket>
#include <QSslError>
#include <QTimer>
#include <QTemporaryFile>
#include "phiresponserec.h"
#include "phisserverrequest.h"
#include "phisserverrec.h"
#include "phierror.h"

class PHISHttp : public QObject
{
    Q_OBJECT

public:
    enum State { Connected, Reading, Sending, Closing };
    enum Content { Empty, MultipartEncoded, UrlEncoded, PhiEncoded };
    explicit PHISHttp( QObject *parent=0 );
    virtual ~PHISHttp();

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
    PHISServerRequest *_req;
    PHISServerRec _server;
    QByteArray _contentEntity, _boundary, _contentType, _name;
    QTemporaryFile *_tmpFile;
    int _dbConnId;

protected slots:
    void readyRead();
    void slotSslErrors( const QList<QSslError>& );

signals:
    void log( int type, const char* file, int line, const QDateTime &dt, PHIRC rc, const QString &todo );
    void close();
};

#endif // PHISHTTP_H
