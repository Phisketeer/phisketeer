/*
#    Copyright (C) 2010-2012  Marius B. Schumacher
#    Copyright (C) 2011-2012  Phisys AG, Switzerland
#    Copyright (C) 2012  Phisketeer.org team
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef APACHEREQUEST_H
#define APACHEREQUEST_H

#include "phisrequest.h"
#include "phiresponserec.h"

struct request_rec;

class ApacheRequest : public PHISRequest
{
public:
    explicit ApacheRequest( request_rec *rec, PHIResponseRec *resp );
    virtual ~ApacheRequest();

    inline void insertTmpFile( const QByteArray &name, QTemporaryFile *tmpFile )
        { _tmpFiles.insertMulti( name, tmpFile ); }
    inline void addPostQueryItem( const QString &name, const QString &value )
        {  _postData.insertMulti( name, value ); }
    void setPostEncodedUrl( const QByteArray &query );
};

#endif // APACHEREQUEST_H
