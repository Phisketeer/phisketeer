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
#ifndef PHISHTML5BASE_H
#define PHISHTML5BASE_H
#include "phishtmlgeneric.h"

class PHISHtml5Base : public PHISHtmlGeneric
{
public:
    explicit PHISHtml5Base( const PHISRequest *req, const PHIBasePage *page );
    virtual ~PHISHtml5Base();

private:
    virtual QByteArray phisEngine() const=0;
    inline virtual QByteArray doctype() const { return PHIS::doctypeHtml5(); }

    virtual QByteArray phiObject( const QUrl &url, const QByteArray &codebase ) const;
    virtual QByteArray phiObjectExclusive( const QUrl &url, const QByteArray &codebase ) const;

protected:
    QByteArray rgba( const QColor &c ) const;
    virtual QByteArray textShadowStyle() const;
    virtual QByteArray effectStyle() const;

    virtual void textEmail() const;
    virtual void textSearch() const;
    virtual void textPhone() const;
    virtual void decimalNumber() const;
    virtual void realNumber() const;
    virtual void table() const;
    virtual void checkBoxList() const;
    virtual void radioButton() const;
    virtual void checkBox() const;
    virtual void label() const;
    virtual void svg() const;
    virtual void menuLink() const;
    virtual void rect() const;
    virtual void roundedRect() const;
    virtual void youtubeVideo() const;
};

#endif // PHISHTML5BASE_H
