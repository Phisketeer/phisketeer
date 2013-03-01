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
#ifndef PHISHTMLTRIDENT4_H
#define PHISHTMLTRIDENT4_H
#include "phishtmlgeneric.h"

class PHISHtmlTrident4 : public PHISHtmlGeneric
{
public:
    explicit PHISHtmlTrident4( const PHISRequest *req, const PHIBasePage *page );
    virtual ~PHISHtmlTrident4();

protected:
    inline virtual QRect adjustSelectSize() const {
        return QRect( 0, 0, 0, static_cast<int>(-_it->height()) ); }

    // MS provides only filters for transformations until IE9 / Trident5
    //virtual QByteArray transformKeyword() const { return QByteArray( "-ms-transform" ); }
    //virtual QByteArray transformOriginKeyword() const { return QByteArray( "-ms-transform-origin" ); }
    //virtual QByteArray boxShadowKeyword() const { return QByteArray( "-ms-box-shadow" ); }
    inline virtual QByteArray effectStyle() const { return QByteArray(); } // handled in positionStyle()

    virtual QByteArray positionStyle( const QRect &off=QRect( 0, 0, 0, 0 ) ) const;
    virtual void imageSource( const QByteArray &src, const QByteArray &style,
        const QByteArray &id, const QByteArray &title,
        const QByteArray &src2=QByteArray() ) const;

private:
    virtual QByteArray phisEngine() const;
    virtual QByteArray phiObject( const QUrl &url, const QByteArray &codebase ) const;
    virtual QByteArray postProcessPhiObjectJS() const;
    virtual QByteArray phiObjectExclusive( const QUrl &url, const QByteArray &codebase ) const;
    virtual QByteArray postProcessPhiObjectExclusiveJS() const;

    virtual void youtubeVideo() const;
    virtual void facebookLikeButton() const;
    virtual void externalHtmlDoc() const;
    virtual void imageButton() const;
    virtual void diaShow() const;
    virtual void googleCalendar() const;
    virtual void googleDynamicMaps() const;
};

#endif // PHISHTMLTRIDENT4_H
