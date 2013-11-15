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
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QComboBox>
#include <QListWidget>
#include "philistitems.h"
#include "phibasepage.h"
#include "phidatasources.h"

void PHISelectItem::initWidget()
{
    setWidget( new QComboBox() );
    setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed, QSizePolicy::ComboBox ) );
}

void PHISelectItem::ideInit()
{
    QComboBox *cb=qobject_cast<QComboBox*>(widget());
    Q_ASSERT( cb );
    cb->setEditable( false );
    textData()->setText( L1( "Select1[s1][true]\nSelect2[s2]" ) );
    setColor( PHIPalette::WidgetBase, PHIPalette::Button, page()->phiPalette().color( PHIPalette::Button ) );
    setColor( PHIPalette::WidgetText, PHIPalette::ButtonText, page()->phiPalette().color( PHIPalette::ButtonText ) );
}

void PHISelectItem::squeeze()
{
    PHIAbstractInputItem::squeeze();
    if ( delimiter()==L1( "\n" ) ) removeData( DDelimiter );
}

void PHISelectItem::setWidgetText( const QString &t )
{
    QComboBox *cb=qobject_cast<QComboBox*>(widget());
    Q_ASSERT( cb );
    QStringList list=t.split( delimiter(), QString::SkipEmptyParts );
    cb->clear();
    cb->addItems( list );
}

void PHISelectItem::setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col )
{
    if ( ir==PHIPalette::WidgetText ) {
        setData( DColor, col );
        setColorRole( cr );
    } else if ( ir==PHIPalette::WidgetBase ) {
        setData( DBackgroundColor, col );
        setBackgroundColorRole( cr );
    } else return;
    QWidget *w=widget();
    if ( !w ) return;
    QPalette::ColorRole role=QPalette::ButtonText;
    if ( ir==PHIPalette::WidgetBase ) role=QPalette::Button;
    QPalette pal=w->palette();
    pal.setColor( role, col );
    pal.setColor( QPalette::Window, Qt::transparent );
    w->setPalette( pal );
}

QSizeF PHISelectItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    QSizeF s=PHIAbstractTextItem::sizeHint( which, constraint );
    if ( which==Qt::MinimumSize ) {
        s.setWidth( qMax( s.width(), 32. ) );
    } else if ( which==Qt::PreferredSize ) {
        s.setHeight( qMax( s.height(), 24. ) );
        s.setWidth( 96. );
    }
    return s;
}

void PHISelectCountryItem::ideInit()
{
    PHISelectItem::ideInit();
    QFile f( L1( ":/countries" ) );
    if ( !f.open( QIODevice::ReadOnly ) ) return;
    QTextStream t( &f );
    t.setCodec( "Latin1" );
    QString countries;
    QString line, country, code;
    while ( !t.atEnd() ) {
        line=t.readLine();
        code=line.right( 2 ).toLower();
        line.replace( QRegExp( L1( ";..$" ) ), QString() );
        country=line.toLower();
        country[0]=line[0].toUpper();
        int pos=country.indexOf( QLatin1Char(' ') );
        while ( pos>-1 ) {
            country[pos+1]=line[pos+1].toUpper();
            pos=country.indexOf( QLatin1Char(' '), pos+1 );
        }
        countries+=country+QLatin1Char('[')+code+L1( "]\n" );
    }
    f.close();
    textData()->setText( countries );
}

void PHIMultiSelectItem::initWidget()
{
    QListWidget *lw=new QListWidget();
    lw->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
#ifdef Q_OS_MAC
    lw->setForegroundRole( QPalette::Button );
    lw->setFrameStyle( QFrame::Box );
#endif
    setWidget( lw );
    setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding, QSizePolicy::Frame ) );
}

void PHIMultiSelectItem::ideInit()
{
    textData()->setText( L1( "select_1[s1][true]\nselect_2[s2]\nselect_3[s3][1]" ) );
    setColor( PHIPalette::WidgetBase, PHIPalette::Base, page()->phiPalette().color( PHIPalette::Base ) );
    setColor( PHIPalette::WidgetText, PHIPalette::Text, page()->phiPalette().color( PHIPalette::Text ) );
}

void PHIMultiSelectItem::setWidgetText( const QString &t )
{
    QListWidget *lw=qobject_cast<QListWidget*>(widget());
    Q_ASSERT( lw );
    QStringList list=t.split( delimiter(), QString::SkipEmptyParts );
    lw->clear();
    lw->addItems( list );
}

void PHIMultiSelectItem::setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col )
{
    PHIAbstractInputItem::setColor( ir, cr, col );
}

QSizeF PHIMultiSelectItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    QSizeF s=PHIAbstractTextItem::sizeHint( which, constraint );
    if ( which==Qt::PreferredSize ) {
        s.setHeight( qMax( s.height(), 96. ) );
        s.setWidth( 144. );
    }
    return s;
}
