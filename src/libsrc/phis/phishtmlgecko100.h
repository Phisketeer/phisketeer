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
#ifndef PHISHTMLGECKO100_H
#define PHISHTMLGECKO100_H
#include "phishtml5base.h"

class PHISHtmlGecko100 : public PHISHtml5Base
{
public:
    PHISHtmlGecko100();
    explicit PHISHtmlGecko100( const PHISRequest *req, const PHIBasePage *page );
    virtual ~PHISHtmlGecko100();

protected:
    virtual QRect adjustMultiLineEditSize() const;
    virtual QByteArray effectStyle() const;
    virtual QByteArray transformKeyword() const { return QByteArray( "-moz-transform" ); }
    virtual QByteArray transformOriginKeyword() const { return QByteArray( "-moz-transform-origin" ); }
    virtual QByteArray boxShadowKeyword() const { return QByteArray( "box-shadow" ); }

private:
    virtual QByteArray phisEngine() const;

protected:
    bool _isMacOSX;
};

#endif // PHISHTMLGECKO100_H
