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
#ifndef PHISERVERREQUEST_H
#define PHISERVERREQUEST_H

#include <QObject>
#include "phisrequest.h"
#include "phiserverrec.h"
#include "phiresponserec.h"
#include "phis.h"

class PHIServerRequest : public QObject, public PHISRequest
{
    Q_OBJECT

public:
    explicit PHIServerRequest( const PHIServerRec *server, PHIResponseRec *resp, QObject *parent );
    virtual ~PHIServerRequest();
    inline void insertTmpFile( const QByteArray &name, QTemporaryFile *tmpFile )
        { _tmpFiles.insertMulti( name, tmpFile ); }
    inline void addPostQueryItem( const QString &name, const QString &value )
        {  _postData.insertMulti( name, value ); }

    void setPostEncodedUrl( const QByteArray &query );
    PHIRC setupHttpHeader( const QString &header, QString &err );
    void clear();

private:
    const PHIServerRec *_server;
};

#endif // PHISERVERREQUEST_H
