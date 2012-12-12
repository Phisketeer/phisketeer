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
#ifndef PHISHTMLGECKO19_H
#define PHISHTMLGECKO19_H
#include "phishtmlgeneric.h"

class PHISHtmlGecko19 : public PHISHtmlGeneric
{
public:
    explicit PHISHtmlGecko19( const PHISRequest *req, const PHIBasePage *page );
    virtual ~PHISHtmlGecko19();

protected:
    virtual QRect adjustButtonSize() const;
    virtual QRect adjustLineEditSize() const;
    virtual QRect adjustMultiLineEditSize() const;
    virtual QRect adjustSelectSize() const;
    virtual QRect adjustFileButton() const;
    virtual qint8 adjustDateEditButtonSpace() const;

    virtual QByteArray phiObject( const QUrl &url, const QByteArray &codebase ) const;
    virtual QByteArray postProcessPhiObjectJS() const;

    virtual QByteArray effectStyle() const;

    virtual void youtubeVideo() const;
    //virtual QByteArray transformKeyword() const { return QByteArray( "-moz-transform" ); }
    //virtual QByteArray transformOriginKeyword() const { return QByteArray( "-moz-transform-origin" ); }
    //virtual QByteArray boxShadowKeyword() const { return QByteArray( "-moz-box-shadow" ); }

private:
    virtual QByteArray phisEngine() const;

protected:
    const bool _isMacOSX, _isLinux;
};

#endif // PHISHTMLGECKO19_H
