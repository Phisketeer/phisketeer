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
#ifndef PHIINPUTTOOLS_H
#define PHIINPUTTOOLS_H
#include "phirequest.h"

class PHIInputTools
{
public:
    static QRectF adjustedLineEdit( const PHIRequest *req, const QRectF &r );
    static QRectF adjustedButton( const PHIRequest *req, const QRectF &r );
    static QRectF adjustedTextArea( const PHIRequest *req, const QRectF &r );
    static QRectF adjustedMultiSelect( const PHIRequest *req, const QRectF &r );
    static QRectF adjustedSelect( const PHIRequest *req, const QRectF &r );
};

inline QRectF PHIInputTools::adjustedLineEdit( const PHIRequest *req, const QRectF &r )
{
    switch ( req->agentEngine() ) {
    case PHIRequest::WebKit: return r.adjusted( 0, 0, -4, -4 );
    case PHIRequest::Trident: return r.adjusted( 0, 0, 0, -6 );
    case PHIRequest::Gecko: return r.adjusted( 0, 0, 0, 0 );
    case PHIRequest::Presto: return r.adjusted( 0, 0, 0, 0 );
    default:;
    }
    return r;
}

inline QRectF PHIInputTools::adjustedButton( const PHIRequest *req, const QRectF &r )
{
    switch ( req->agentEngine() ) {
    case PHIRequest::WebKit: return r.adjusted( 3, 1, 0, 0 );
    case PHIRequest::Trident: return r.adjusted( 0, 0, 0, -6 );
    case PHIRequest::Gecko: return r.adjusted( 0, 0, 0, 0 );
    case PHIRequest::Presto: return r.adjusted( 0, 0, 0, 0 );
    default:;
    }
    return r;
}

inline QRectF PHIInputTools::adjustedTextArea( const PHIRequest *req, const QRectF &r )
{
    switch ( req->agentEngine() ) {
    case PHIRequest::WebKit: return r.adjusted( 0, 0, -6, -6 );
    case PHIRequest::Trident: return r.adjusted( 0, 0, 0, -6 );
    case PHIRequest::Gecko: return r.adjusted( 0, 0, 0, 0 );
    case PHIRequest::Presto: return r.adjusted( 0, 0, 0, 0 );
    default:;
    }
    return r;
}

inline QRectF PHIInputTools::adjustedMultiSelect( const PHIRequest *req, const QRectF &r )
{
    switch ( req->agentEngine() ) {
    case PHIRequest::WebKit: return r;
    case PHIRequest::Trident: return r.adjusted( 0, 0, 0, -6 );
    case PHIRequest::Gecko: return r.adjusted( 0, 0, 0, 0 );
    case PHIRequest::Presto: return r.adjusted( 0, 0, 0, 0 );
    default:;
    }
    return r;
}

inline QRectF PHIInputTools::adjustedSelect( const PHIRequest *req, const QRectF &r )
{
    switch ( req->agentEngine() ) {
    case PHIRequest::WebKit: return r.adjusted( 1, 1, -2, -2 );
    case PHIRequest::Trident: return r.adjusted( 0, 0, 0, -6 );
    case PHIRequest::Gecko: return r.adjusted( 0, 0, 0, 0 );
    case PHIRequest::Presto: return r.adjusted( 0, 0, 0, 0 );
    default:;
    }
    return r;
}

#endif // PHIINPUTTOOLS_H
