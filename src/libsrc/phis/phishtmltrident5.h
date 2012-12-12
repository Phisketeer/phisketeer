/*
#    Copyright (C) 2010-2012  Marius B. Schumacher
#    Copyright (C) 2011-2012  Phisys AG, Switzerland
#    Copyright (C) 2012  Phisketeer.org team
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
#ifndef PHISHTMLTRIDENT5_H
#define PHISHTMLTRIDENT5_H
#include "phishtml5base.h"

class PHISHtmlTrident5 : public PHISHtml5Base
{
public:
    explicit PHISHtmlTrident5( const PHISRequest *req, const PHIBasePage *page );
    virtual ~PHISHtmlTrident5();

protected:
    //virtual QByteArray positionStyle( const QRect &off ) const;
    virtual QByteArray effectStyle() const;
    //virtual QByteArray transformKeyword() const { return QByteArray( "-ms-transform" ); }
    //virtual QByteArray transformOriginKeyword() const { return QByteArray( "-ms-transform-origin" ); }
    //virtual QByteArray boxShadowKeyword() const { return QByteArray( "box-shadow" ); }
    bool useBoxShadow() const;

private:
    virtual QByteArray phisEngine() const;
    virtual QByteArray phiObject( const QUrl &url, const QByteArray &codebase ) const;
    virtual QByteArray postProcessPhiObjectJS() const;
    virtual QByteArray phiObjectExclusive( const QUrl &url, const QByteArray &codebase ) const;
    virtual QByteArray postProcessPhiObjectExclusiveJS() const;
};

#endif // PHISHTMLTRIDENT5_H
