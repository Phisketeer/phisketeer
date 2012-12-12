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
#include "phishtmlpresto23.h"

PHISHtmlPresto23::PHISHtmlPresto23( const PHISRequest *req, const PHIBasePage *page )
    : PHISHtml5Base( req, page )
{
}

PHISHtmlPresto23::~PHISHtmlPresto23()
{
}

QByteArray PHISHtmlPresto23::phisEngine() const
{
    static QByteArray arr="Phis "+PHIS::libVersion().toAscii()+"/Presto 23";
    return arr;
}

QRect PHISHtmlPresto23::adjustButtonSize() const
{
    if ( _it->wid()==PHI::DATEEDIT ) return QRect( 0, 0, 0, 0 );
    if ( _it->height()<33 ) return QRect( 0, 0, 0, -_it->height() );
    return QRect( 0, 0, 0, 0 );
}
