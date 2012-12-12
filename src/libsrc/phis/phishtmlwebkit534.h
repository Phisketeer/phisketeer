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
#ifndef PHISHTMLWEBKIT534_H
#define PHISHTMLWEBKIT534_H
#include "phishtmlwebkit533.h"

class PHISHtmlWebKit534 : public PHISHtmlWebKit533
{
public:
    explicit PHISHtmlWebKit534( const PHISRequest *req, const PHIBasePage *page );
    virtual ~PHISHtmlWebKit534();

protected:
    inline virtual void svg() const { return PHISHtml5Base::svg(); }
    virtual void textSearch() const;

private:
    virtual QByteArray phisEngine() const;
};

#endif // PHISHTMLWEBKIT534_H
