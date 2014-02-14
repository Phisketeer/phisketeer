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
#include <QCheckBox>
#include <QRadioButton>
#include <QHttpMultiPart>
#include <QHttpPart>
#include "phicheckitems.h"
#include "phidatasources.h"
#include "phibasepage.h"
#include "phirequest.h"
#include "phidataparser.h"

void PHICheckBoxItem::initWidget()
{
    QCheckBox *cb=new QCheckBox();
    setWidget( cb );
    setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed, QSizePolicy::CheckBox ) );
    if ( !isClientItem() ) return;
    connect( cb, &QCheckBox::clicked, this, &PHICheckBoxItem::slotChanged );
}

void PHICheckBoxItem::ideInit()
{
    setColor( PHIPalette::WidgetText, PHIPalette::WindowText, page()->phiPalette().color( PHIPalette::WindowText ) );
    setColor( PHIPalette::WidgetBase, PHIPalette::Window, page()->phiPalette().color( PHIPalette::Window ) );
    textData()->setText( tr( "Checkbox label" ) );
}

void PHICheckBoxItem::loadItemData( QDataStream &in, int version )
{
    PHIAbstractInputItem::loadItemData( in, version );
    in >> &_checkedData;
}

void PHICheckBoxItem::saveItemData(QDataStream &out, int version )
{
    PHIAbstractInputItem::saveItemData( out, version );
    out << &_checkedData;
}

QSizeF PHICheckBoxItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    QSizeF s=PHIAbstractTextItem::sizeHint( which, constraint );
    if ( which==Qt::PreferredSize ) s.setWidth( s.width()+22 );
    return s;
}

void PHICheckBoxItem::setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col )
{
    if ( ir==PHIPalette::WidgetText ) {
        setData( DColor, col );
        setColorRole( ir, cr );
    } else if ( ir==PHIPalette::WidgetBase ) {
        setData( DBackgroundColor, col );
        setColorRole( ir, cr );
    } else return;
    QWidget *w=widget();
    if ( !w ) return;
    QPalette::ColorRole role=QPalette::WindowText;
    if ( ir==PHIPalette::WidgetBase ) role=QPalette::Window;
    QPalette pal=w->palette();
    pal.setColor( role, col );
    w->setPalette( pal );
}

void PHICheckBoxItem::setChecked( bool check )
{
    PHIBaseItem::setChecked( check );
    QAbstractButton *ab=qobject_cast<QAbstractButton*>(widget());
    if ( !ab ) return;
    ab->setChecked( check );
}

void PHICheckBoxItem::setWidgetText( const QString &s )
{
    QAbstractButton *ab=qobject_cast<QAbstractButton*>(widget());
    if ( !ab ) return;
    ab->setText( s );
}

void PHICheckBoxItem::ideUpdateData()
{
    PHIAbstractInputItem::ideUpdateData();
    QAbstractButton *ab=qobject_cast<QAbstractButton*>(widget());
    Q_ASSERT( ab );
    if ( _checkedData.isUnparsedStatic() )
        ab->setChecked( _checkedData.boolean() );
    else if ( checkedData()->isUnparsedTranslated() )
        ab->setChecked( _checkedData.boolean( page()->currentLang() ) );
    else ab->setChecked( false );
}


void PHICheckBoxItem::phisCreateData( const PHIDataParser &parser )
{
    PHIAbstractInputItem::phisCreateData( parser );
    setChecked( parser.text( &_checkedData ).toBool() );
    if ( !_checkedData.isUnparsedStatic() ) setDirtyFlag( DFCustom1 );
}

void PHICheckBoxItem::phisParseData( const PHIDataParser &parser )
{
    PHIAbstractInputItem::phisParseData( parser );
    if ( dirtyFlags() & DFCustom1 ) setChecked( parser.text( &_checkedData ).toBool() );
}

void PHICheckBoxItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    htmlInitItem( script, false );
    if ( realChecked() ) script+=BL( ".checked(1)" );
    if ( Q_UNLIKELY( colorRole( PHIPalette::WidgetText )==PHIPalette::Custom ) )
        script+=BL( ".color('" )+cssColor( realColor() )+BL( "')" );
    if ( Q_UNLIKELY( colorRole( PHIPalette::WidgetBase )==PHIPalette::Custom ) )
        script+=BL( ".bgColor('" )+cssColor( realBackgroundColor() )+BL( "')" );
    script+=BL( ";\n" );
    out+=indent+BL( "<div" );
    htmlBase( req, out, script );
    out+=BL( "\">\n" )+indent+BL( "\t<table class=\"phi\"><tr style=\"vertical-align\"><td style=\"" );
    if ( data( DFont ).isValid() ) {
        QFont f=data( DFont ).value<QFont>();
        if ( f.underline() ) {
            out+=BL( "text-decoration:underline;" );
        }
    }
    // @todo: implement align?
    out+=BL( "width:18px;padding-left:4px\"><input id=\"" )+id()
        +BL( "_phi\" class=\"phi\" type=\"checkbox\" name=\"" );
    if ( parentId().isEmpty() ) out+=id();
    else out+=parentId();
    out+=BL( "\" style=\"position:relative\" value=\"" )+data( DValue ).toByteArray()
        +BL( "\"></td><td style=\"" );
    out+=BL( "\"><label class=\"phi\" for=\"" )+id()+BL( "_phi\" id=\"" )+id()
        +BL( "_phit\">" )+data( DText ).toByteArray().replace( '\n', BL( "<br>" ) )
        +BL( "</label></td></tr></table>\n" )+indent+BL( "</div>\n" );
}

void PHICheckBoxItem::cssStatic( const PHIRequest *req, QByteArray &out ) const
{
    Q_UNUSED( req )
    out+='#'+id()+BL( "{overflow:hidden;white-space:nowrap;" );
    // fallback:
    if ( colorRole( PHIPalette::WidgetText )!=PHIPalette::WindowText ) out+=BL( "color:" )+realColor().name().toLatin1()+';';
    if ( colorRole( PHIPalette::WidgetBase )!=PHIPalette::Window ) out+=BL( "background-color:" )+realBackgroundColor().name().toLatin1()+';';
    out+=BL( "}\n" );
}

void PHICheckBoxItem::slotChanged()
{
    QCheckBox *cb=qobject_cast<QCheckBox*>(widget());
    Q_ASSERT( cb );
    setChecked( cb->isChecked() );
    if ( flags() & FHasChangeEventHandler ) trigger( L1( "change" ) );
}

QScriptValue PHICheckBoxItem::text( const QScriptValue &v )
{
    if ( !v.isValid() ) return realText();
    setText( v.toString() );
    return self();
}

QScriptValue PHICheckBoxItem::checked( const QScriptValue &v )
{
    if ( !v.isValid() ) return realChecked();
    if ( v.toBool()==realChecked() ) return self();
    setChecked( v.toBool() );
    slotChanged();
    return self();
}

void PHICheckBoxItem::clientPostData( QHttpMultiPart *multiPart ) const
{
    if ( !realChecked() ) return;
    QByteArray parent=id();
    if ( isChild() ) parent=parentId();
    QHttpPart hp;
    hp.setHeader( QNetworkRequest::ContentTypeHeader, BL( "text/plain" ) );
    hp.setHeader( QNetworkRequest::ContentDispositionHeader, BL( "form-data; name=\"" )+parent+BL( "\"" ) );
    hp.setBody( realValue().toUtf8() );
    multiPart->append( hp );
}

void PHIRadioButtonItem::initWidget()
{
    QRadioButton *rb=new QRadioButton();
    setWidget( rb );
    setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed, QSizePolicy::RadioButton ) );
    rb->setAutoExclusive( false );
    if ( !isClientItem() ) return;
    connect( rb, &QRadioButton::clicked, this, &PHIRadioButtonItem::slotChanged );
}

void PHIRadioButtonItem::ideInit()
{
    PHICheckBoxItem::ideInit();
    textData()->setText( tr( "Radio button" ) );
}

QSizeF PHIRadioButtonItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    QSizeF s=PHIAbstractTextItem::sizeHint( which, constraint );
    if ( which==Qt::PreferredSize ) s.setWidth( s.width()+22 );
    return s;
}

void PHIRadioButtonItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    htmlInitItem( script, false );
    if ( realChecked() ) script+=BL( ".checked(1)" );
    if ( Q_UNLIKELY( colorRole( PHIPalette::WidgetText )==PHIPalette::Custom ) )
        script+=BL( ".color('" )+cssColor( realColor() )+BL( "')" );
    if ( Q_UNLIKELY( colorRole( PHIPalette::WidgetBase )==PHIPalette::Custom ) )
        script+=BL( ".bgColor('" )+cssColor( realBackgroundColor() )+BL( "')" );
    script+=BL( ";\n" );
    out+=indent+BL( "<div" );
    htmlBase( req, out, script );
    // @todo: implement align?
    out+=BL( "\">\n" )+indent
        +BL( "\t<table class=\"phi\"><tr style=\"vertical-align\"><td style=\"" );
    if ( data( DFont ).isValid() ) {
        QFont f=data( DFont ).value<QFont>();
        if ( f.underline() ) {
            out+=BL( "text-decoration:underline;" );
        }
    }
    out+=BL( "width:18px;padding-left:4px\"><input id=\"" )+id()
        +BL( "_phi\" class=\"phi\" type=\"radio\" name=\"" );
    if ( parentId().isEmpty() ) out+=id();
    else out+=parentId();
    out+=BL( "\" style=\"position:relative\" value=\"" )+data( DValue ).toByteArray()
        +BL( "\"></td><td style=\"" );
    out+=BL( "\"><label class=\"phi\" for=\"" )+id()+BL( "_phi\" id=\"" )+id()
        +BL( "_phit\">" )+data( DText ).toByteArray().replace( '\n', BL( "<br>" ) )
        +BL( "</label></td></tr></table>\n" )+indent+BL( "</div>\n" );
}

void PHIRadioButtonItem::slotChanged()
{
    QRadioButton *rb=qobject_cast<QRadioButton*>(widget());
    Q_ASSERT( rb );
    setChecked( rb->isChecked() );
    if ( flags() & FHasChangeEventHandler ) trigger( L1( "change" ) );
    if ( !rb->isChecked() || parentId().isEmpty() ) return;
    PHIAbstractLayoutItem *lit=qobject_cast<PHIAbstractLayoutItem*>(page()->findItem( parentId() ));
    if ( !lit ) return;
    foreach ( PHIBaseItem *it, lit->childItems() ) {
        if ( id()==it->id() ) continue;
        PHIRadioButtonItem *phirb=qobject_cast<PHIRadioButtonItem*>(it);
        if ( phirb ) phirb->setChecked( false );
    }
}
