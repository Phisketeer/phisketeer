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
#ifndef PHISHTMLPRESTO22_H
#define PHISHTMLPRESTO22_H
#include "phishtmlgeneric.h"

class PHISHtmlPresto22 : public PHISHtmlGeneric
{
public:
    explicit PHISHtmlPresto22( const PHISRequest *req, const PHIBasePage *page );
    virtual ~PHISHtmlPresto22();

protected:
    //inline virtual QRect adjustSelectSize() const { return QRect( 0, 0, 0, -_it->height() ); }
    virtual QByteArray transformKeyword() const { return QByteArray( "-o-transform" ); }
    virtual QByteArray transformOriginKeyword() const { return QByteArray( "-o-transform-origin" ); }
    virtual QByteArray boxShadowKeyword() const { return QByteArray( "box-shadow" ); }

private:
    virtual QByteArray phisEngine() const;
};

#endif // PHISHTMLPRESTO22_H
