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
#include "phierror.h"
#include "phishtmlbase.h"
#include "phihtmlgenerator.h"
#include "phishtmlgeneric.h"
#include "phishtmlgecko19.h"
#include "phishtmlgecko20.h"
#include "phishtmlgecko100.h"
#include "phishtmlwebkit532.h"
#include "phishtmlwebkit533.h"
#include "phishtmlwebkit534.h"
#include "phishtmltrident4.h"
#include "phishtmltrident5.h"
#include "phishtmltrident6.h"
#include "phishtmlpresto22.h"
#include "phishtmlpresto23.h"

PHIHtmlGenerator::PHIHtmlGenerator( const PHISRequest *req, const PHIBasePage *p )
    : _req( req ), _page( p )
{
    qDebug( "PHIHtmlGenerator::PHIHtmlGenerator()" );
    //if ( _vars.value( "accept", PHIVars::Server ).contains( "application/xhtml+xml" ) ) {
    //    resp->setContentType( "application/xhtml+xml" );
    //}
}

PHIHtmlGenerator::~PHIHtmlGenerator()
{
    qDebug( "PHIHtmlGenerator::~PHIHtmlGenerator()" );
}

QByteArray PHIHtmlGenerator::genHtml()
{
    qDebug( "ENGINE %d (%d:%d) - %d", _req->agentEngine(), _req->engineMajorVersion(),
           _req->engineMinorVersion(), _req->agentId() );
    static QByteArray arr;
    try {
        PHISHtmlBase *html(0);
        switch ( _req->agentEngine() ) {
        case PHISRequest::Trident:
            if ( _req->engineMajorVersion()<5 ) html=new PHISHtmlTrident4( _req, _page );
            else if ( _req->engineMajorVersion()==5 ) html=new PHISHtmlTrident5( _req, _page );
            else html=new PHISHtmlTrident6( _req, _page );
            break;
        case PHISRequest::Gecko:
            if ( _req->engineMajorVersion()>9 ) html=new PHISHtmlGecko100( _req, _page );
            else if ( _req->engineMajorVersion()>1 ) html=new PHISHtmlGecko20( _req, _page );
            else html=new PHISHtmlGecko19( _req, _page );
            break;
        case PHISRequest::WebKit:
            if ( _req->engineMajorVersion()>533 ) html=new PHISHtmlWebKit534( _req, _page );
            else if ( _req->engineMajorVersion()==533 ) html=new PHISHtmlWebKit533( _req, _page );
            else html=new PHISHtmlWebKit532( _req, _page );
            break;
        case PHISRequest::Presto:
            if ( _req->engineMajorVersion()==2 ) {
                if ( _req->engineMinorVersion()<3 ) html=new PHISHtmlPresto22( _req, _page );
                else html=new PHISHtmlPresto23( _req, _page );
            } else if ( _req->engineMajorVersion()>2 ) html=new PHISHtmlPresto23( _req, _page );
            else html=new PHISHtmlPresto22( _req, _page );
            break;
        default:;
        }
        if ( !html ) html=new PHISHtmlGeneric( _req, _page );
        arr=html->createHtmlCode();
    } catch ( std::bad_alloc& ) {
        _req->responseRec()->log( PHILOGCRIT, PHIRC_OUT_OF_MEMORY, QObject::tr( "Out of memory." ) );
        _req->responseRec()->error( PHILOGCRIT, PHIRC_HTTP_INTERNAL_SERVER_ERROR,
            QObject::tr( "System ressource error.") );
        arr.clear();
        return arr;
    }
    return arr;
}
