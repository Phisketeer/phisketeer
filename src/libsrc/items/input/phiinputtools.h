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
    static QRectF adjustedFileButton( const PHIRequest *req, const QRectF &r );
    static QRectF adjustedImageButton( const PHIRequest *req, const QRectF &r );
};

inline QRectF PHIInputTools::adjustedLineEdit( const PHIRequest *req, const QRectF &r )
{
    switch ( req->agentEngine() ) {
    case PHIRequest::WebKit: {
        if ( req->osType()==PHIRequest::iOS ) return r.adjusted( 0, 0, -2, -2 );
        return r.adjusted( 0, 0, -4, -4 );
    }
    case PHIRequest::Trident:
        if ( req->agentFeatures() & PHIRequest::IE678 || req->engineMajorVersion()==5 )
            return r.adjusted( 0, 0, -4, -4 );
        return r.adjusted( 0, 0, -2, -2 );
    case PHIRequest::Gecko:
        if ( req->osType()==PHIRequest::Windows ) return r.adjusted( 0, 0, -2, -2 );
        else if ( req->osType()==PHIRequest::Linux ) return r.adjusted( -1, -1, -5, -5 );
        return r.adjusted( 0, 0, -6, -6 );
    case PHIRequest::Presto: return r.adjusted( 0, 0, -4, -4 );
    default:;
    }
    return r;
}

inline QRectF PHIInputTools::adjustedButton( const PHIRequest *req, const QRectF &r )
{
    switch ( req->agentEngine() ) {
    case PHIRequest::WebKit: {
        if ( req->osType()==PHIRequest::MacOS ) return r.adjusted( 4, 3, -4, 0 );
        else if ( req->osType()==PHIRequest::Windows ) return r;
        else if ( req->osType()==PHIRequest::iOS ) return r.adjusted( 1, 1, -1, -1 );
        return r.adjusted( 1, 1, -1, -1 );
    }
    case PHIRequest::Trident:
        if ( req->agentFeatures() & PHIRequest::IE678 || req->engineMajorVersion()==5 ) return r;
        return r.adjusted( 1, 1, -1, -1 );
    case PHIRequest::Gecko:
        if ( req->osType()==PHIRequest::Windows || req->osType()==PHIRequest::Linux ) return r;
        return r.adjusted( 1, 1, -1, -1 );
    case PHIRequest::Presto: return r;
    default:;
    }
    return r;
}

inline QRectF PHIInputTools::adjustedTextArea( const PHIRequest *req, const QRectF &r )
{
    switch ( req->agentEngine() ) {
    case PHIRequest::WebKit: return r.adjusted( 0, 0, -6, -6 );
    case PHIRequest::Trident: return r.adjusted( 0, 0, -6, -6 );
    case PHIRequest::Gecko:
        if ( req->osType()==PHIRequest::Windows ) return r.adjusted( 0, -1, -6, -7 );
        else if ( req->osType()==PHIRequest::Linux ) return r.adjusted( -1, -2, -5, -6 );
        return r.adjusted( 0, -1, -2, -3 );
    case PHIRequest::Presto: return r.adjusted( 0, 0, -6, -6 );
    default:;
    }
    return r;
}

inline QRectF PHIInputTools::adjustedMultiSelect( const PHIRequest *req, const QRectF &r )
{
    Q_UNUSED( req )
    return r;
}

inline QRectF PHIInputTools::adjustedSelect( const PHIRequest *req, const QRectF &r )
{
    switch ( req->agentEngine() ) {
    case PHIRequest::WebKit:
        if ( req->osType()==PHIRequest::MacOS ) return r.adjusted( 1, 3, -1, -3 );
        else if ( req->osType()==PHIRequest::iOS ) return r.adjusted( 0, 1, 0, -1 );
    case PHIRequest::Gecko:
        if ( req->osType()==PHIRequest::Linux ) return r.adjusted( -1, 0, 1, 0 );
    }
    return r.adjusted( 0, 1, 0, -1 );
}

inline QRectF PHIInputTools::adjustedFileButton( const PHIRequest *req, const QRectF &r )
{
    switch ( req->agentEngine() ) {
    case PHIRequest::Gecko:
        if ( req->osType()==PHIRequest::Windows || req->osType()==PHIRequest::Linux ) return r;
    default:;
    }
    return r.adjusted( 1, 1, -1, -1 );
}

inline QRectF PHIInputTools::adjustedImageButton( const PHIRequest *req, const QRectF &r )
{
    Q_UNUSED( req )
    return r.adjusted( 3, 3, -3, -3 );
}

#endif // PHIINPUTTOOLS_H
