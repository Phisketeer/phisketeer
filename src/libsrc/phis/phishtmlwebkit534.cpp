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
#include "phishtmlwebkit534.h"

PHISHtmlWebKit534::PHISHtmlWebKit534( const PHISRequest *req, const PHIBasePage *page )
    : PHISHtmlWebKit533( req, page )
{
}

PHISHtmlWebKit534::~PHISHtmlWebKit534()
{
}

QByteArray PHISHtmlWebKit534::phisEngine() const
{
    static QByteArray arr="Phis "+PHIS::libVersion().toLatin1()+"/WebKit 534";
    return arr;
}

void PHISHtmlWebKit534::textSearch() const
{
    _out+=_indent+"<input type=\"search\" class=\"phitext\""+maxLength()
        +name()+title()+disabled()+readOnly()+value()+accessKey()+tabIndex()
        +startStyle( QRect( 0, 0, 0, 0 ) )
        +fontStyle()+colorStyle()+effectStyle()+_endtag;
}
