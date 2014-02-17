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
#include "phigoogleitems.h"
#include "phinetrequest.h"
#include "phirequest.h"
#include "phidataparser.h"
#include "phiwebpage.h"

QByteArray PHIGoogleStaticMapItem::source() const
{
    return BL( "http://maps.google.com/maps/api/staticmap?size=" )
        +QByteArray::number( qRound(realWidth()) )+'x'
        +QByteArray::number( qRound(realHeight()) )+'&';
}

void PHIGoogleStaticMapItem::ideInit()
{
    PHIAbstractExternalItem::ideInit();
    textData()->setText( L1( "center=Zurich&zoom=14&maptype=terrain&sensor=false" ) );
}

void PHIGoogleStaticMapItem::setWidgetText( const QString &t )
{
    QString tmp=t;
    if ( !t.contains( L1( "language=" ) ) )
        tmp+=L1( "&language=" )+QString::fromLatin1( mapLanguage( page()->currentLang() ) );
    PHIAbstractExternalItem::setWidgetText( tmp );
}

void PHIGoogleStaticMapItem::phisCreateData( const PHIDataParser &parser )
{
    setData( DText, parser.text( textData() ) );
    if ( textData()->isUnparsedStatic() || textData()->isUnparsedTranslated() ) {
        PHINetRequestThread request( parser.request()->responseRec() );
        QString lang;
        QByteArray id;
        foreach( lang, page()->languages() ) {
            QString url;
            url.reserve( 200 );
            url+=QString::fromLatin1( source() );
            if ( textData()->isUnparsedStatic() ) url+=textData()->text();
            else url+=textData()->text( page()->currentLang() );
            if ( !url.contains( L1( "language=" ) ) )
                url+=L1( "&language=" )+QString::fromLatin1( mapLanguage( lang.toLatin1() ) );
            id=parser.createImage( request.getImage( url ), lang.toLatin1() );
        }
    } else setDirtyFlag( DFText );
}

void PHIGoogleStaticMapItem::phisParseData( const PHIDataParser &parser )
{
    if ( Q_LIKELY( !(dirtyFlags() & DFText) ) ) {
        setImagePath( parser.imagePath( page()->currentLang() ) );
        return;
    }
    setData( DText, parser.text( textData() ) );
    PHINetRequestThread request( parser.request()->responseRec() );
    QString url;
    url.reserve( 200 );
    url+=QString::fromLatin1( source() )+realText();
    if ( !url.contains( SL( "language=" ) ) )
        url+=SL( "&language=" )+QString::fromLatin1( mapLanguage( page()->currentLang() ) );
    setImagePath( parser.createImage( request.getImage( url ), page()->currentLang(), -1 ) );
}

void PHIGoogleStaticMapItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    PHIBaseItem::htmlImg( req, out, script, indent );
}

QSizeF PHIGooglePlusItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    if ( which==Qt::PreferredSize ) return QSizeF( 190., 22. );
    return PHIAbstractExternalItem::sizeHint( which, constraint );
}

PHIWID PHIGooglePlusItem::htmlHeaderExtension( const PHIRequest *req, QByteArray &out ) const
{
    Q_UNUSED( req )
    out+=BL( "<script type=\"text/javascript\" src=\"//apis.google.com/js/plusone.js\">{lang:'" )
        +mapLanguage( page()->currentLang() )+BL( "'}</script>\n" );
    return static_cast<PHIWID>(GooglePlus);
}

void PHIGooglePlusItem::ideInit()
{
    PHIAbstractExternalItem::ideInit();
    textData()->setText( L1( "size=medium&href=http://www.phisketeer.org&callback=" ) );
}

void PHIGooglePlusItem::squeeze()
{
    removeData( DTmpUrl );
    PHIAbstractExternalItem::squeeze();
}

void PHIGooglePlusItem::phisCreateData( const PHIDataParser &parser )
{
    setData( DText, parser.text( textData() ) );
    if ( !textData()->isUnparsedStatic() ) setDirtyFlag( DFText );
}

void PHIGooglePlusItem::phisParseData( const PHIDataParser &parser )
{
    if ( dirtyFlags() & DFText ) setData( DText, parser.text( textData() ) );
}

void PHIGooglePlusItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    PHIByteArrayList list=data( DText ).toByteArray().split( '&' );
    QByteArray tmp, size, href, cb, annotation;
    foreach ( tmp, list ) {
        if ( tmp.startsWith( BL( "href=" ) ) ) href=tmp.mid( 5 );
        else if ( tmp.startsWith( BL( "size=" ) ) ) size=tmp.mid( 5 );
        else if ( tmp.startsWith( BL( "callback=" ) ) ) cb=tmp.mid( 9 );
        else if ( tmp.startsWith( BL( "annotation=" ) ) ) annotation=tmp.mid( 11 );
    }
    qDebug() << "href" << href;
    out+=indent+BL( "<div" );
    htmlBase( req, out, script );
    out+=BL( "\">\n" )+indent+BL( "\t<g:plusone href=\"" )+href+BL( "\" size=\"" )
        +size+BL( "\" callback=\"" )+cb.replace( '"', BL( "\\\"" ) )+BL( "\" annotation=\"" )
        +annotation.replace( '"', BL( "\\\"" ) )+BL( "\"></g:plusone>\n" )+indent+BL( "</div>\n" );
    htmlInitItem( script );
}

void PHIGooglePlusItem::setWidgetText( const QString &t )
{
    if ( !webPage() ) return;
    QStringList list=t.split( QLatin1Char( '&' ), QString::SkipEmptyParts );
    QString tmp, size, href, cb, annotation;
    foreach ( tmp, list ) {
        if ( tmp.startsWith( L1( "href=" ) ) ) href=tmp.mid( 5 );
        else if ( tmp.startsWith( L1( "size=" ) ) ) size=tmp.mid( 5 );
        else if ( tmp.startsWith( L1( "callback=" ) ) ) cb=tmp.mid( 9 );
        else if ( tmp.startsWith( L1( "annotation=" ) ) ) annotation=tmp.mid( 11 );
    }
    QByteArray arr="<html><head><style>body { margin:0; padding:0; }</style>"
        "<script type=\"text/javascript\" src=\"//apis.google.com/js/plusone.js\">{lang:'"
        +mapLanguage( page()->currentLang() )+"'}</script></head><body>"
        "<g:plusone href=\""+href.toUtf8()+"\" size=\""+size.toUtf8()+"\" callback=\""+cb.toUtf8()+"\" "
        "annotation=\""+annotation.toUtf8()+"\"></g:plusone>"
        "</body></html>";
    QByteArray content=data( DTmpUrl ).toByteArray();
    if ( arr==content ) return;
    setData( DTmpUrl, arr );
    // the following crashes (Qt 5.1):
    // webPage()->settings()->setAttribute( QWebSettings::PluginsEnabled, true );
    webPage()->mainFrame()->setContent( QByteArray() );
    webPage()->mainFrame()->setContent( arr, L1( "text/html" ), QUrl( L1( "https://www.google.com" ) ) );
}

void PHIYouTubeItem::ideInit()
{
    PHIAbstractExternalItem::ideInit();
    textData()->setText( L1( "<Video ID>?modestbranding=1&version=3" ) );
}

QByteArray PHIYouTubeItem::source() const
{
    return BL( "http://www.youtube-nocookie.com/embed/" );
}

void PHIYouTubeItem::phisCreateData( const PHIDataParser &parser )
{
    setData( DText, parser.text( textData() ) );
    if ( !textData()->isUnparsedStatic() ) setDirtyFlag( DFText );
}

void PHIYouTubeItem::phisParseData( const PHIDataParser &parser )
{
    QByteArray tmp;
    if ( Q_UNLIKELY( dirtyFlags() & DFText ) ) {
        tmp=parse( parser.text( textData() ).toByteArray() );
    } else tmp=parse( data( DText ).toByteArray() );
    setData( DText, tmp );
}

QByteArray PHIYouTubeItem::parse( const QByteArray &arr )
{
    QByteArray vid=arr;
    //vid.replace( '&', BL( "&amp;" ) );
    if ( !vid.contains( BL( "hl=" ) ) ) vid.append( BL( "&hl=" )+mapLanguage( page()->currentLang() ) );
    if ( !vid.contains( BL( "cc_lang_pref=" ) ) )
        vid.append( BL( "&cc_lang_pref=" )+mapLanguage( page()->currentLang() ) )
            +BL( "&cc_load_policy=1" );
    return vid;
}

void PHIGoogleMapsItem::ideInit()
{
    PHIAbstractExternalItem::ideInit();
    textData()->setText( L1( "f=q&source=s_q&geocode=&q=Z%C3%BCrich&sll=51.151786,10.415039&sspn=9.721478,23.269043"
        "&ie=UTF8&hq=&hnear=Z%C3%BCrich,+Schweiz&t=m&ll=47.368594,8.539124&spn=0.111612,0.219727&z=12&iwloc=A&output=embed" ) );
}

QByteArray PHIGoogleMapsItem::source() const
{
    return BL( "//www.google.com/maps?" );
}

void PHIGoogleMapsItem::squeeze()
{
    removeData( DTmpText );
    PHIAbstractExternalItem::squeeze();
}

void PHIGoogleMapsItem::setWidgetText( const QString &t )
{
    QByteArray arr="<html><head><style>body{margin:0;padding:0;background-color:transparent}</style></head>"
        "<body><iframe width=\""+QByteArray::number( qRound(realWidth()) )+"\" height=\""
        +QByteArray::number( qRound(realHeight()) )+"\" src=\"//www.google.com/maps?"
        +t.toUtf8().replace( '"', "\\\"" );
    if ( !t.contains( L1( "hl=" ) ) ) arr+="&hl="+mapLanguage( page()->currentLang() );
    if ( !t.contains( L1( "output=" ) ) ) arr+="&output=embed";
    arr+="\" scrolling=\"no\" frameborder=\"0\" style=\"border:none;overflow:visible\""
        " allowTransparency=\"true\"></iframe></body></html>";
    QByteArray tmp=data( DTmpText ).toByteArray();
    if ( arr==tmp ) return;
    setData( DTmpText, arr );
    webPage()->mainFrame()->setContent( QByteArray() );
    webPage()->mainFrame()->setContent( arr, L1( "text/html" ), QUrl( L1( "https://www.google.com" ) ) );
}

void PHIGoogleMapsItem::phisCreateData( const PHIDataParser &parser )
{
    QByteArray tmp=parser.text( textData() ).toByteArray();
    if ( !textData()->isUnparsedStatic() ) setDirtyFlag( DFText );
    else {
        if ( !tmp.contains( "output=" ) ) tmp+"&output=embed";
        tmp.replace( '"', "\\\"" );
        setData( DText, tmp );
    }
}

void PHIGoogleMapsItem::phisParseData( const PHIDataParser &parser )
{
    QByteArray tmp;
    if ( Q_UNLIKELY( dirtyFlags() & DFText ) ) {
        tmp=parser.text( textData() ).toByteArray();
        if ( !tmp.contains( "output=" ) ) tmp+"&output=embed";
        tmp.replace( '"', "\\\"" );
        setData( DText, tmp );
    } else tmp=data( DText ).toByteArray();
    if ( !tmp.contains( "hl=" ) ) tmp+="&hl="+mapLanguage( page()->currentLang() );
    setData( DText, tmp );
}

void PHIGoogleCalendarItem::ideInit()
{
    PHIAbstractExternalItem::ideInit();
    textData()->setText( L1( "6br0cushftao9k56ldrebkrbls%40group.calendar.google.com&color=%23B1440E&;title=Phisketeer&wkst=1&bgcolor=%23ffffff&ctz=Europe%2FZurich" ) );
}

void PHIGoogleCalendarItem::squeeze()
{
    removeData( DTmpText );
    PHIAbstractExternalItem::squeeze();
}

QByteArray PHIGoogleCalendarItem::source() const
{
    return BL( "//www.google.com/calendar/embed?" );
}

QSizeF PHIGoogleCalendarItem::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    if ( which==Qt::PreferredSize ) return QSizeF( 320., 200. );
    return PHIAbstractExternalItem::sizeHint( which, constraint );
}

void PHIGoogleCalendarItem::setWidgetText( const QString &t )
{
    if ( !webPage() ) return;
    QByteArray arr="<html><head><style>body {margin:0;padding:0;background-color:transparent;}</style></head>"
        "<body><iframe src=\"//www.google.com/calendar/embed?src="+t.toUtf8()+"&amp;"
        "height="+QByteArray::number( qRound(realHeight()) )+"\""
        "scrolling=\"no\" frameborder=\"0\" style=\"border:none;overflow:visible;\""
        "allowTransparency=\"true\" width=\""+QByteArray::number( qRound(realWidth()) )+"\""
        " height=\""+QByteArray::number( qRound(realHeight()) )+"\"></iframe></body></html>";
    QByteArray tmp=data( DTmpText ).toByteArray();
    if ( tmp==arr ) return;
    setData( DTmpText, arr );
    webPage()->mainFrame()->setContent( QByteArray() );
    webPage()->mainFrame()->setContent( arr, L1( "text/html" ), QUrl( L1( "https://www.google.com" ) ) );
}
