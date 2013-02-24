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
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PHIPAGE_H
#define PHIPAGE_H

#include <QStringList>
#include <QColor>
#include <QImage>
#include <QFlags>
#include <QByteArray>
#include <QPalette>
#include <QFont>
#include <QDataStream>
#include "phi.h"

class PHIEXPORT PHIPage
{
    friend PHIEXPORT QDataStream& operator<<( QDataStream&, const PHIPage* );
    friend PHIEXPORT QDataStream& operator>>( QDataStream&, PHIPage* );

public:
    PHIPage();
    virtual ~PHIPage();
    PHIPage( const PHIPage& );
    PHIPage& operator=( const PHIPage& );

    virtual quint16 itemCount() const=0;
    inline quint16 storedItemCount() const { return _itemCount; }

    enum Attribute { ANone=0x0, ARequiresSession=0x1, ARequiresLogin=0x2, AFormAction=0x4, APalette=0x8,
        ABgColor=0x10, AStyleSheet=0x20, ACreateSession=0x40, AKeywords=0x80, ALandscape=0x100, ATemplate=0x200,
        AIcon=0x400, AExtensions=0x800, AJavascript=0x1000, ADatabase=0x2000, ASetCookie=0x4000,
        AServerscript=0x8000, AUsejQueryUI=0x10000, AApplication=0x20000, AUseTemplatePalette=0x40000,
        ANoSystemCSS=0x80000, AHasCalendar=0x100000, APhiNoObjectTag=0x200000,
        ADbSettingsFromFile=0x400000, ANoUiCSS=0x800000, AHasDragDrop=0x1000000,
        AForceHtmlOutput=0x2000000, ADocumentLeft=0x4000000, AUseOpenGraph=0x8000000,
        ANoUnderlineLinks=0x10000000, ABgImage=0x20000000, AMax=0x40000000 }; // qint32

    enum ScriptModule { SNone=0x0, SDatabase=0x1, SFile=0x2, SSystem=0x4, SProcess=0x8,
        SServer=0x10, SRequest=0x20, SEmail=0x40, SReply=0x80, SAll=0xFFFF }; // qint32 (deprecated)

    enum Extension { ENone=0x0, EProgressBar=0x1, EHasFacebookLike=0x2, EHasTwitter=0x4,
        EHasGooglePlus=0x8, EHidePhiMenu=0x10, EHasCanvas=0x20 }; // qint32

    enum Geometry { GUnknown=0, GA4=1, GLetter=2, GCustom=3, GPhi=4, G4_3=5, G16_9=6, GiPad=7 };

    enum DataType { DNone=0, DTemplatePage=1, DTitle=2, DSession=3, DAuthor=4, DCompany=5,
        DVersion=6, DAction=7, DCopyright=8, DSessionRedirect=9, DJavascript=10,
        DStyleSheet=11, DKeys=12, DLanguages=13, DCurrentLang=14, DDescription=15,
        DColorError=16, DColorBgError=17, DColorHover=18, DOpenGraph=19, DBGImageUrl=20,
        DBGImageOptions=21, DBGImageXOff=22, DBGImageYOff=23 }; // quint8

    enum ImageOption { INone=0x00, IFixed=0x01, IRepeatX=0x02, IRepeatY=0x04 };

#ifdef PHIDEBUG
    Q_DECLARE_FLAGS( Attributes, Attribute ) // qint32
    Q_DECLARE_FLAGS( ScriptModules, ScriptModule ) // (deprecated)
    Q_DECLARE_FLAGS( Extensions, Extension )
    Q_DECLARE_FLAGS( ImageOptions, ImageOption )
#else
    typedef qint32 Attributes;
    typedef qint32 ScriptModules; // (deprecated)
    typedef qint32 Extensions;
    typedef qint32 ImageOptions;
#endif

protected:
    Attributes _attributes; // qint32 in QDataStream
    quint8 _internalVersion;
    QByteArray _id, _extensionData;
    quint32 _width, _height;
    QColor _bgColor;
    QPalette _palette;
    QImage _image;
    QFont _font;
    Extensions _extensions; // qint32 in QDataStream
    ScriptModules _scriptModules; // qint32 in QDataStream (deprecated)

private:
    quint16 _itemCount;
};

#ifdef PHIDEBUG
Q_DECLARE_OPERATORS_FOR_FLAGS( PHIPage::Attributes )
Q_DECLARE_OPERATORS_FOR_FLAGS( PHIPage::Extensions )
Q_DECLARE_OPERATORS_FOR_FLAGS( PHIPage::ScriptModules )
Q_DECLARE_OPERATORS_FOR_FLAGS( PHIPage::ImageOptions )
#endif

PHIEXPORT QDataStream& operator<<( QDataStream&, const PHIPage* );
PHIEXPORT QDataStream& operator>>( QDataStream&, PHIPage* );

#endif // PHIPAGE_H
