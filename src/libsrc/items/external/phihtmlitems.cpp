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
#include <QWebFrame>
#include "phihtmlitems.h"
#include "phiwebpage.h"
#include "phidataparser.h"

void PHIHtmlDocItem::ideInit()
{
    PHIAbstractExternalItem::ideInit();
    textData()->setText( L1( "http://www.phisketeer.org" ) );
}

void PHITwitterItem::ideInit()
{
    PHIAbstractExternalItem::ideInit();
    textData()->setText( L1( "count=horizontal&url=http://www.phisketeer.org&text=my+title" ) );
}

void PHITwitterItem::squeeze()
{
    removeData( DTmpText );
    PHIAbstractExternalItem::squeeze();
}

QSizeF PHITwitterItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    if ( isChild() ) return realSize();
    if ( which==Qt::PreferredSize ) return QSizeF( 100., 24. );
    return PHIAbstractExternalItem::sizeHint( which, constraint );
}

PHIWID PHITwitterItem::htmlHeaderExtension( const PHIRequest *req, QByteArray &out ) const
{
    Q_UNUSED( req )
    out+=BL( "<script type=\"text/javascript\" charset=\"utf-8\" src=\"//platform.twitter.com/widgets.js\"></script>\n" );
    return static_cast<PHIWID>(Twitter);
}

void PHITwitterItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    out+=indent+BL( "<div" );
    htmlBase( req, out, script );
    out+=BL( "\">\n" )+indent+BL( "\t<a href=\"//twitter.com/share?" )+data( DText ).toByteArray();
    if ( !data( DText ).toByteArray().contains( BL( "lang" ) ) ) out+=BL( "&amp;lang=" )+mapLanguage( page()->currentLang() );
    out+=BL( "\" class=\"twitter-share-button\">Tweet</a>\n" )+indent+BL( "</div>\n" );
    htmlInitItem( script );
}

void PHITwitterItem::setWidgetText( const QString &t )
{
    if ( !webPage() ) return;
    QByteArray arr="<html><head><style>body { margin:0; padding:0; }</style>"
        "<script type=\"text/javascript\" charset=\"utf-8\" src=\"//platform.twitter.com/widgets.js\"></script></head><body>"
        "<a href=\"//twitter.com/share?"+t.toUtf8().replace( '"', "\\\"" );
    if ( !t.contains( L1( "lang=" ) ) ) arr+="&lang="+mapLanguage( page()->currentLang() );
    arr+="\" class=\"twitter-share-button\">Tweet</a></body></html>";
    QByteArray tmp=data( DTmpText ).toByteArray();
    if ( tmp==arr ) return;
    setData( DTmpText, arr );
    webPage()->mainFrame()->setContent( arr, L1( "text/html" ), QUrl( L1( "http://platform.twitter.com" ) ) );
}

void PHIFacebookItem::ideInit()
{
    PHIAbstractExternalItem::ideInit();
    textData()->setText( L1( "href=http://www.phisketeer.org&layout=standard&show_faces=true&action=like" ) );
}

void PHIFacebookItem::squeeze()
{
    removeData( DTmpText );
    PHIAbstractExternalItem::squeeze();
}

QSizeF PHIFacebookItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    if ( isChild() ) return realSize();
    if ( which==Qt::PreferredSize ) return QSizeF( 400., 80. );
    return PHIAbstractExternalItem::sizeHint( which, constraint );
}

void PHIFacebookItem::setWidgetText( const QString &t )
{
    if ( !webPage() ) return;
    QByteArray arr="<html><head><style>body {margin:0;padding:0;background-color:transparent;}</style></head>"
        "<body><iframe src=\"//www.facebook.com/plugins/like.php?"+t.toUtf8()+"&width="
        +QByteArray::number( qRound(realWidth()) )+"&height="
        +QByteArray::number( qRound(realHeight()) )+"&locale="+mapLanguage( page()->currentLang() )
        +"\" scrolling=\"no\" frameborder=\"0\" style=\""
        "border:none;overflow:visible\" width=\""+QByteArray::number( qRound(realWidth()) )+"\" height=\""
        +QByteArray::number( qRound(realHeight()) )+"\" allowTransparency=\"true\"></iframe>"
        "</body></html>";
    QByteArray tmp=data( DTmpText ).toByteArray();
    if ( tmp==arr ) return;
    setData( DTmpText, arr );
    webPage()->mainFrame()->setContent( arr, L1( "text/html" ), QUrl( L1( "https://www.facebook.com" ) ) );
}

QByteArray PHIFacebookItem::source() const
{
    return BL( "//www.facebook.com/plugins/like.php?" );
}

void PHIFacebookItem::phisCreateData( const PHIDataParser &parser )
{
    setData( DText, parser.text( textData() ) );
    if ( !textData()->isUnparsedStatic() ) setDirtyFlag( DFText );
}

void PHIFacebookItem::phisParseData( const PHIDataParser &parser )
{
    QByteArray tmp;
    if ( Q_UNLIKELY( dirtyFlags() & DFText ) ) {
        tmp=escapeChars( parser.text( textData() ).toByteArray() );
    } else tmp=escapeChars( data( DText ).toByteArray() );
    if ( !tmp.contains( BL( "locale=" ) ) ) tmp+=BL( "&amp;locale=" )
        +mapLanguage( page()->currentLang() );
    tmp+=BL( "&amp;width=" )+QByteArray::number( qRound(realWidth()) )
        +BL( "&amp;height=" )+QByteArray::number( qRound(realHeight()) );
    setData( DText, tmp );
}

QByteArray PHIFacebookItem::mapLanguage( const QByteArray &lang ) const
{
    // @todo: read from Facebook language XML file
    if ( lang.size()>2 ) {
        if ( lang=="en_EN" ) return QByteArray( "en_US" );
        return lang;
    }
    if ( lang=="C" ) return QByteArray();
    if ( lang=="en" ) return QByteArray( "en_US" );
    if ( lang=="ja" ) return QByteArray( "ja_JP" );
    return lang+'_'+lang.toUpper();
}
