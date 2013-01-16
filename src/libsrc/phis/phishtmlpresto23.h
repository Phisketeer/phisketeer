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
#ifndef PHISHTMLPRESTO23_H
#define PHISHTMLPRESTO23_H
#include "phishtml5base.h"

class PHISHtmlPresto23 : public PHISHtml5Base
{
public:
    explicit PHISHtmlPresto23( const PHISRequest *req, const PHIBasePage *page );
    virtual ~PHISHtmlPresto23();

protected:
    //inline virtual QRect adjustSelectSize() const { return QRect( 0, 0, 0, -_it->height() ); }
    virtual QRect adjustButtonSize() const;
    virtual QByteArray transformKeyword() const { return QByteArray( "-o-transform" ); }
    virtual QByteArray transformOriginKeyword() const { return QByteArray( "-o-transform-origin" ); }
    virtual QByteArray boxShadowKeyword() const { return QByteArray( "box-shadow" ); }

private:
    virtual QByteArray phisEngine() const;

};

#endif // PHISHTMLPRESTO23_H
