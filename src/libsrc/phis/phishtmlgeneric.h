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
#ifndef PHISHTMLGENERIC_H
#define PHISHTMLGENERIC_H

#include "phishtmlbase.h"

class PHISHtmlGeneric : public PHISHtmlBase
{
public:
    explicit PHISHtmlGeneric( const PHISRequest *req, const PHIBasePage *page );
    virtual ~PHISHtmlGeneric();

protected:
    enum GraphicType { GTRect, GTRoundedRect, GTEllipse, GTImage, GTSvg, GTGraphicsText,
        GTRichText, GTStaticGoogleMap };

private:
    virtual void createGraphicsImage( GraphicType ) const;
    virtual QByteArray phisEngine() const;
    inline virtual QByteArray doctype() const { return PHIS::doctypeHtml4(); }

protected:
    virtual QByteArray effectStyle() const;
    virtual QByteArray effectJS() const;

    inline virtual void roundedRect() const { createGraphicsImage( GTRoundedRect ); }
    inline virtual void rect() const { createGraphicsImage( GTRect ); }
    inline virtual void ellipse() const { createGraphicsImage( GTEllipse ); }
    inline virtual void svg() const { createGraphicsImage( GTSvg ); }
    inline virtual void graphicsText() const { createGraphicsImage( GTGraphicsText ); }
    inline virtual void countryBox() const { selectBox(); }

    virtual void unknownItem() const;
    virtual void rolloverButton() const;
    virtual void image() const;
    virtual void button() const;
    virtual void resetButton() const;
    virtual void submitButton() const;
    virtual void lineEdit() const;
    virtual void multiLineEdit() const;
    virtual void passwordEdit() const;
    virtual void listBox() const;
    virtual void selectBox() const;
    virtual void table() const;
    virtual void label() const;
    virtual void richTextLayouted() const;
    virtual void richText() const;
    virtual void radioButton() const;
    virtual void checkBox() const;
    virtual void checkBoxList() const;
    virtual void externalHtmlDoc() const;
    virtual void langSelector() const;
    virtual void phisysLink() const;
    virtual void hidden() const;
    virtual void fileButton() const;
    virtual void dateEdit() const;
    virtual void calendar() const;
    virtual void imageButton() const;
    virtual void textEmail() const;
    virtual void textSearch() const;
    virtual void textPhone() const;
    virtual void decimalNumber() const;
    virtual void realNumber() const;
    virtual void diaShow() const;
    virtual void menuLink() const;
    virtual void staticGoogleMaps() const;
    virtual void progressBar() const;
    virtual void twitterButton() const;
    virtual void youtubeVideo() const;
    virtual void facebookLikeButton() const;
    virtual void googlePlusButton() const;
};

#endif // PHISHTMLGENERIC_H
