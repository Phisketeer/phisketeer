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
#ifndef PHISHTMLBASE_H
#define PHISHTMLBASE_H

#include <QByteArray>
#include <QRect>
#include <QUrl>
#include <QTransform>
#include "phis.h"
#include "phisrequest.h"
#include "phibasepage.h"
#include "phibaseitem.h"
#include "phisitemcache.h"

class PHISHtmlBase
{
public:
    explicit PHISHtmlBase( const PHISRequest *req, const PHIBasePage *page );
    virtual ~PHISHtmlBase();

    QByteArray createHtmlCode();

/*
    enum Option { ONone=0x0, OPhisysLink=0x1 };

#ifdef PHIDEBUG
    Q_DECLARE_FLAGS( Options, Option )
#else
    typedef qint32 Options;
#endif
*/

protected:
    QByteArray fontStyle() const;
    QByteArray alignmentStyle( quint8 align=255 ) const; // 255=gets alignment from item
    QByteArray transformationPositionStyle( const QTransform &t, bool size=false ) const;
    QRect transformationPosition( const QTransform &t ) const;
    QByteArray onClickUrl() const;
    QByteArray textValidatorJS( const QByteArray &regexp, const QByteArray &title ) const;
    QByteArray decimalValidatorJS( int min, int max ) const;
    QByteArray realValidatorJS( qreal min, qreal max, int prec ) const;
    QByteArray toLocaleFormat( const QByteArray &l ) const;
    QByteArray createOpenGraph() const;

    QString storeImage( const QImage &img, const QByteArray &postfix, int number=-1 ) const;
    QImage* deviceReferenceImage() const; // Used to adjust screen resolutions (96dpi / 72dpi) for QFont
    QImage graphicsEffectImage( const QImage &img ) const;
    QTransform transformation() const;
    QImage createRectImage() const;
    QImage createRoundedRectImage() const;
    QImage createEllipseImage() const;
    QImage createSvgImage() const;
    QImage createImageImage( const QString &imageId ) const;
    QImage createRolloverTextImage( int over, const QImage &img=QImage() ) const;
    QImage createGraphicsTextImage() const;
    QImage createRichTextImage() const;
    QImage createHeaderImage() const;
    QImage createPhisysLinkImage() const;
    QImage createStaticGoogleMapImage() const;

    const PHIBaseItem* createPhisysLinkItem() const;

    inline QString checkForImageId( const QByteArray &postfix=QByteArray(), int num=-1 ) const {
        return PHISItemCache::instance()->imageId( _req->hostname(), _it->pageId(),
        _it->id()+postfix, _lang, num );
    }
    inline QByteArray id() const { return QByteArray( " id=\""+_it->id()+"\"" ); }
    inline QByteArray name() const { return QByteArray( id()+" name=\""+_it->id()+"\"" ); }
    inline QByteArray value() const { return QByteArray( " value=\""+_it->valueData()+"\"" ); }
    inline QByteArray maxLength() const { return QByteArray( " maxlength=\""
        +QByteArray::number( _it->maxLength() )+"\"" ); }
    inline QByteArray disabled() const {
        return _it->disabled() ? QByteArray::fromRawData( " disabled=\"disabled\"", 20 ) : QByteArray(); }
    inline QByteArray readOnly() const {
        return _it->readOnly() ? QByteArray::fromRawData( " readonly=\"readonly\"", 20 ) : QByteArray(); }
    inline QByteArray title() const { return _it->toolTip().isEmpty() ? QByteArray() :
        QByteArray( " title=\""+_it->toolTipData()+"\"" ); }
    inline QByteArray tabIndex() const { return _it->tabIndex()>0 ?
        QByteArray( " tabindex=\""+QByteArray::number( _it->tabIndex() )+"\"" ) : QByteArray(); }
    inline QByteArray accessKey() const { return _it->accessKey().isEmpty() ? QByteArray() :
        QByteArray( " accesskey=\""+_it->accessKey().left(1).toUtf8()+"\"" ); }
    inline QByteArray startStyle( const QRect &off=QRect( 0, 0, 0, 0 ) ) const {
        return QByteArray( " style=\""+positionStyle( off )+visibilityStyle() ); }

    inline QByteArray visibilityStyle() const { return _it->visible() ? QByteArray() :
        QByteArray::fromRawData( "display:none;", 13 ); }
    inline QByteArray boldStyle() const { return _it->font().bold() ?
        QByteArray::fromRawData( "font-weight:bold;", 17 ) : QByteArray(); }
    inline QByteArray italicStyle() const { return _it->font().italic() ?
        QByteArray::fromRawData( "font-style:italic;", 18 ) : QByteArray(); }
    inline QByteArray underlineStyle() const { return _it->font().underline() ?
        QByteArray::fromRawData( "text-decoration:underline;", 26 ) : QByteArray(); }
    inline QByteArray widthStyle() const { return QByteArray( "width:"
        +QByteArray::number(static_cast<int>(_it->width()) )+"px;" ); }
    inline QByteArray heightStyle() const { return QByteArray( "height:"
        +QByteArray::number(static_cast<int>(_it->height()) )+"px;" ); }
    inline QByteArray colorStyle() const { return _it->color().isValid() ?
        QByteArray( "color:"+_it->color().name().toLatin1()+';' ) : QByteArray(); }
    inline QByteArray outlineColorStyle() const { return _it->outlineColor().isValid() ?
        QByteArray( "background-color:"+_it->outlineColor().name().toLatin1()+';' ) : QByteArray(); }

    virtual QByteArray effectStyle() const=0;
    virtual QByteArray effectJS() const=0;
    inline virtual QByteArray transformKeyword() const { return QByteArray( "transform" ); }
    inline virtual QByteArray transformOriginKeyword() const { return QByteArray ( "transform-origin" ); }
    inline virtual QByteArray boxShadowKeyword() const { return QByteArray( "box-shadow" ); }
    inline virtual QRect adjustButtonSize() const { return QRect( 0, 0, 0, 0 ); }
    inline virtual QRect adjustLineEditSize() const { return QRect( 0, 0, -6, -6 ); }
    inline virtual QRect adjustMultiLineEditSize() const { return QRect( 0, 0, -6, -6 ); }
    inline virtual QRect adjustSelectSize() const { return QRect( 0, 0, 0, 0 ); }
    inline virtual QRect adjustFileButton() const { return QRect( 0, 0, 0, 0 ); }
    inline virtual qint8 adjustDateEditButtonSpace() const { return static_cast<qint8>(0); }

    virtual QByteArray selectOptions( const QStringList &list=QStringList() ) const;
    virtual QByteArray positionStyle( const QRect &off=QRect( 0, 0, 0, 0 ) ) const;
    virtual QByteArray textShadowStyle() const;
    virtual void imageSource( const QByteArray &src, const QByteArray &style,
        const QByteArray &id, const QByteArray &title,
        const QByteArray &src2=QByteArray() ) const;
    virtual QByteArray phiObject( const QUrl &url, const QByteArray &codebase ) const;
    virtual QByteArray postProcessPhiObjectJS() const;
    virtual QByteArray phiObjectExclusive( const QUrl &url, const QByteArray &codebase ) const;
    virtual QByteArray postProcessPhiObjectExclusiveJS() const;

private:
    virtual QByteArray doctype() const=0;
    inline virtual QByteArray htmlStartTag() const { return PHIS::htmlStartTag(); }

    virtual QByteArray phisEngine() const=0;
    virtual QByteArray phiBaseScript() const;
    virtual QByteArray charset() const;
    virtual QByteArray phiCalendarScript() const;
    virtual QByteArray phiProgressBarScript() const;
    virtual QByteArray phiTwitterScript() const;
    virtual QByteArray phiFacebookScript() const;
    virtual QByteArray phiGooglePlusScript() const;
    virtual QByteArray phiCanvasScript() const;
    virtual QByteArray formAction() const;

    virtual QImage blurEffectImage( const QImage &img ) const;
    virtual QImage shadowEffectImage( const QImage &img ) const;
    virtual QImage surfaceEffectImage( const QImage &img ) const;
    virtual QImage colorizeEffectImage( const QImage &img ) const;

    virtual void unknownItem() const=0;
    virtual void button() const=0;
    virtual void rect() const=0;
    virtual void roundedRect() const=0;
    virtual void ellipse() const=0;
    virtual void image() const=0;
    virtual void svg() const=0;
    virtual void graphicsText() const=0;
    virtual void rolloverButton() const=0;
    virtual void resetButton() const=0;
    virtual void submitButton() const=0;
    virtual void lineEdit() const=0;
    virtual void multiLineEdit() const=0;
    virtual void passwordEdit() const=0;
    virtual void listBox() const=0;
    virtual void selectBox() const=0;
    virtual void countryBox() const=0;
    virtual void table() const=0;
    virtual void richTextLayouted() const=0;
    virtual void richText() const=0;
    virtual void label() const=0;
    virtual void radioButton() const=0;
    virtual void checkBox() const=0;
    virtual void checkBoxList() const=0;
    virtual void externalHtmlDoc() const=0;
    virtual void phisysLink() const=0;
    virtual void langSelector() const=0;
    virtual void hidden() const=0;
    virtual void fileButton() const=0;
    virtual void dateEdit() const=0;
    virtual void calendar() const=0;
    virtual void imageButton() const=0;
    virtual void textEmail() const=0;
    virtual void textSearch() const=0;
    virtual void textPhone() const=0;
    virtual void decimalNumber() const=0;
    virtual void realNumber() const=0;
    virtual void diaShow() const=0;
    virtual void menuLink() const=0;
    virtual void staticGoogleMaps() const=0;
    virtual void progressBar() const=0;
    virtual void twitterButton() const=0;
    virtual void youtubeVideo() const=0;
    virtual void facebookLikeButton() const=0;
    virtual void googlePlusButton() const=0;
    virtual void canvas() const=0;
    virtual void googleDynamicMaps() const=0;
    virtual void googleCalendar() const=0;

    void createHtmlForItem();
    void createLayout();
    void createGridLayout();
    QByteArray jQueryScript() const;
    void jQueryItemSetup() const;

protected:
    const PHISRequest* const _req;
    const PHIBasePage* const _p;
    QByteArray _indent, _endtag;
    mutable QByteArray _out, _js, _jquery;
    const QByteArray _lang;
    const PHIBaseItem *_it;
//    Options _options;
};

/*
#ifdef PHIDEBUG
Q_DECLARE_OPERATORS_FOR_FLAGS( PHISHtmlBase::Options )
#endif
*/

#endif // PHISHTMLBASE_H
