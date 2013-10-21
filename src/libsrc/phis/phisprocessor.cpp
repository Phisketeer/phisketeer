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
#include "phisprocessor.h"
#include "phirequest.h"
#include "phibasepage.h"
#include "phispagecache.h"
#include "phiresponserec.h"

PHIBasePage* PHISProcessor::loadPage() const
{
    return 0;
}

void PHISProcessor::run() const
{
    _req->responseRec()->clear();
    _req->responseRec()->setMinorHttpVer( _req->httpServerMinorVersion() );
    if ( Q_UNLIKELY( _req->keyword( PHIRequest::KMethod )=="HEAD" ) ) _req->responseRec()->setHeadRequestOnly();
    if ( _req->canonicalFilename().endsWith( L1( "/phi.phis" ) ) ) return genSysItem();
    QFile f( _req->canonicalFilename() );
    QFileInfo fi( f );
    if ( fi.suffix()!=L1( "phis" ) ) {
        // not used in Apache (other file types are handled by Apache itself)
        if ( Q_UNLIKELY( !fi.isReadable() ) ) return _req->responseRec()->error( PHILOGERR,
               PHIRC_HTTP_NOT_FOUND, tr( "File not found on server." ) );
        _req->responseRec()->setFileName( _req->canonicalFilename() );
        _req->responseRec()->setContentLength( f.size() );
        _req->responseRec()->setHeader( BL( "Last-Modified" ),
            _req->responseRec()->timeEncoded( fi.lastModified() ) );
        return;
    }
    PHIBasePage *page=PHISPageCache::page( _req );
    if ( Q_UNLIKELY( !page ) ) page=PHISPageCache::insert( _req, loadPage() );
    //if ( Q_UNLIKELY( !page ) ) return _req->responseRec()->error( PHILOGCRIT,
    //    PHIRC_HTTP_INTERNAL_SERVER_ERROR, tr( "Resource error." ) );
    _req->responseRec()->setBody( QByteArray( "<html><h1>Hello world</h1></html>" ) );
    _req->responseRec()->setContentLength( _req->responseRec()->body().size() );
}

void PHISProcessor::genSysItem() const
{

}
