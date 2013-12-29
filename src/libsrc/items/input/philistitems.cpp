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
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QLocale>
#include "philistitems.h"
#include "phibasepage.h"
#include "phidatasources.h"
#include "phiinputtools.h"
#include "phicombobox.h"

void PHISelectItem::initWidget()
{
    QComboBox *cb=new PHIComboBox( this );
    setWidget( cb );
    setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed, QSizePolicy::ComboBox ) );
    if ( !isClientItem() ) return;
    connect( cb, SIGNAL( currentIndexChanged( int ) ), this, SLOT( slotChanged() ) );
}

void PHISelectItem::slotChanged()
{
    if ( flags() & FHasChangeEventHandler ) trigger( L1( "change" ) );
}

void PHISelectItem::ideInit()
{
    QComboBox *cb=qobject_cast<QComboBox*>(widget());
    Q_ASSERT( cb );
    cb->setEditable( false );
    textData()->setText( L1( "Select1[s1]\nSelect2[s2][true]" ) );
    setColor( PHIPalette::WidgetBase, PHIPalette::Button, page()->phiPalette().color( PHIPalette::Button ) );
    setColor( PHIPalette::WidgetText, PHIPalette::ButtonText, page()->phiPalette().color( PHIPalette::ButtonText ) );
}

void PHISelectItem::squeeze()
{
    PHIAbstractInputItem::squeeze();
    if ( realDelimiter()==L1( "\n" ) ) removeData( DDelimiter );
}

void PHISelectItem::setWidgetText( const QString &t )
{
    QComboBox *cb=qobject_cast<QComboBox*>(widget());
    Q_ASSERT( cb );
    QStringList list=t.split( realDelimiter(), QString::SkipEmptyParts );
    cb->clear();
    QString d, opt;
    bool isChecked;
    int checked=0;
    foreach ( d, list ) {
        PHI::getItemCheckData( d, opt, isChecked );
        cb->addItem( d, opt.isEmpty() ? d : opt );
        if ( isChecked && checked==0 ) checked=cb->count()-1;
    }
    cb->setCurrentIndex( checked );
}

void PHISelectItem::setValue( const QString &v )
{
    QComboBox *cb=qobject_cast<QComboBox*>(widget());
    if ( !cb ) return;
    cb->setItemData( cb->currentIndex(), v );
}

QString PHISelectItem::realValue() const
{
    QComboBox *cb=qobject_cast<QComboBox*>(widget());
    if ( !cb ) return QString();
    return cb->currentData().toString();
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

void PHISelectItem::htmlSelectOptions( QByteArray &out, const QByteArray &indent ) const
{
    bool isChecked;
    const QByteArray checked=BL( "\" selected=\"selected" );
    if ( Q_LIKELY( realDelimiter().size()==1 ) ) {
        PHIByteArrayList list=data( DText ).toByteArray().split( realDelimiter().at( 0 ).toLatin1() );
        QByteArray arr, opt;
        foreach( arr, list ) {
            PHI::getItemCheckData( arr, opt, isChecked );
            out+=indent+BL( "<option value=\"" )+opt;
            if ( isChecked ) out+=checked;
            out+=BL( "\">" )+arr+BL( "</option>\n" );
        }
    } else { // expensive (utf8 conversion)
        QStringList list=realText().split( realDelimiter() );
        QString s, opt;
        foreach( s, list ) {
            PHI::getItemCheckData( s, opt, isChecked );
            out+=indent+BL( "<option value=\"" )+opt.toUtf8();
            if ( isChecked ) out+=checked;
            out+=BL( "\">" )+s.toUtf8()+BL( "</option>\n" );
        }
    }
}

void PHISelectItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    out+=indent+BL( "<select name=\"" )+id()+BL( "\"" );
    if ( wid()==MultiSelect ) {
        out+=BL( " multiple=\"multiple\"" );
        setAdjustedRect( PHIInputTools::adjustedMultiSelect( req, rect() ) );
    } else {
        setAdjustedRect( PHIInputTools::adjustedSelect( req, rect() ) );
    }
    htmlBase( req, out, script );
    if ( wid()!=MultiSelect && realHeight()>27 ) {
        if ( req->agentEngine()==PHIRequest::WebKit ) out+=BL( "border:0" );
    }
    out+=BL( "\">\n" );
    htmlSelectOptions( out, indent+'\t' );
    out+=indent+BL( "</select>\n" );
    htmlInitItem( script );
}

QScriptValue PHISelectItem::delimiter( const QScriptValue &d )
{
    if ( !isServerItem() ) return QScriptValue( QScriptValue::UndefinedValue );
    if ( !d.isValid() ) return realDelimiter();
    setDelimiter( d.toString() );
    return self();
}

QScriptValue PHISelectItem::selectOptions( const QString &opts, const QString &d )
{
    setDelimiter( d );
    setText( opts );
    return self();
}

QScriptValue PHISelectItem::selected( const QScriptValue &v, const QScriptValue &b )
{
    Q_UNUSED( b )
    if ( !isClientItem() ) return QScriptValue( QScriptValue::UndefinedValue );
    QComboBox *cb=qobject_cast<QComboBox*>(widget());
    Q_ASSERT( cb );
    if ( !v.isValid() ) {
        QScriptValue list=scriptEngine()->newArray( 1 );
        if ( cb->currentData().toString().isEmpty() )
            list.setProperty( 0, cb->currentText() );
        else list.setProperty( 0, cb->currentData().toString() );
        return list;
    }
    if ( !b.isValid() ) {
        for ( int i=0; i<cb->count(); i++ ) {
            if ( cb->itemData( i ).toString().isEmpty() ) {
                if ( cb->itemText( i )==v.toString() ) return cb->currentIndex()==i ? true : false;
            } else if ( cb->itemData( i ).toString()==v.toString() )
                return cb->currentIndex()==i ? true : false;
        }
        return false;
    }
    if ( b.toBool() ) {
        for ( int i=0; i<cb->count(); i++ ) {
            if ( cb->itemData( i ).toString().isEmpty() ) {
                if ( cb->itemText( i )==v.toString() ) {
                    cb->blockSignals( true );
                    cb->setCurrentIndex( i );
                    cb->blockSignals( false );
                    return self();
                }
            } else if ( cb->itemData( i ).toString()==v.toString() ) {
                cb->blockSignals( true );
                cb->setCurrentIndex( i );
                cb->blockSignals( false );
                return self();
            }
        }
        return self();
    }
    cb->blockSignals( true );
    cb->setCurrentIndex( 0 );
    cb->blockSignals( false );
    return self();
}

void PHISelectItem::clientPostData( QHttpMultiPart *multiPart ) const
{
    QComboBox *cb=qobject_cast<QComboBox*>(widget());
    Q_ASSERT( cb );
    QHttpPart hp;
    hp.setHeader( QNetworkRequest::ContentTypeHeader, BL( "text/plain" ) );
    hp.setHeader( QNetworkRequest::ContentDispositionHeader, BL( "form-data; name=\"" )+id()+BL( "\"" ) );
    if ( cb->currentData().toString().isEmpty() ) hp.setBody( cb->currentText().toUtf8() );
    else hp.setBody( cb->currentData().toString().toUtf8() );
    multiPart->append( hp );
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

void PHISelectLangItem::ideInit()
{
}

void PHISelectLangItem::ideUpdateData()
{
    QString langs;
    foreach ( QString lang, page()->languages() ) {
        langs+=lang+L1( "\n" );
    }
    langs.chop( 1 ); // remove last '\n'
    setWidgetText( langs );
}

QScriptValue PHISelectLangItem::selectOptions( const QString &opts, const QString &d )
{
    Q_UNUSED( opts )
    Q_UNUSED( d )
    return QScriptValue( QScriptValue::UndefinedValue );
}

void PHISelectLangItem::slotChanged()
{
    if ( !isClientItem() ) return;
    if ( id()!="philangselector" ) {
        if ( flags() & FHasChangeEventHandler ) trigger( L1( "change" ) );
        return;
    }
    QComboBox *cb=qobject_cast<QComboBox*>(widget());
    Q_ASSERT( cb );
    emit langChangeRequested( cb->currentData( Qt::UserRole ).toString() );
}

void PHISelectLangItem::squeeze()
{
    *textData()=PHITextData();
    PHISelectItem::squeeze();
}

void PHISelectLangItem::phisParseData( const PHIDataParser &parser )
{
    Q_UNUSED( parser )
    QString langs, lang;
    foreach ( lang, page()->languages() ) {
        langs+=QLocale( lang ).nativeLanguageName();
        langs+=SL( "[" )+lang+SL( "]" );
        if ( lang==page()->lang() ) langs+=SL( "[1]" );
        langs+=SL( "\n" );
    }
    langs.chop( 1 ); // remove last '\n'
    setData( DText, langs.toUtf8() );
}

void PHISelectLangItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    PHISelectItem::html( req, out, script, indent );
    if ( id()!="philangselector" ) return;
    QUrl url=req->url();
    QUrlQuery query( url );
    query.removeQueryItem( SL( "philang" ) );
    query.removeQueryItem( SL( "phisid" ) );
    url.setQuery( query );
    QByteArray d=BL( "&" );
    if ( !url.hasQuery() ) d=BL( "?" );
    script+=BL( "$('philangselector').change(function(){phi.href('" )+url.toEncoded()
        +d+BL( "philang='+$('philangselector').val())});\n" );
}

void PHIMultiSelectItem::initWidget()
{
    QListWidget *lw=new QListWidget();
    lw->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
    lw->setSelectionMode( QAbstractItemView::MultiSelection );
#ifdef Q_OS_MAC
    lw->setForegroundRole( QPalette::Button );
    lw->setFrameStyle( QFrame::Box );
#endif
    setWidget( lw );
    setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding, QSizePolicy::Frame ) );
    if ( !isClientItem() ) return;
    connect( lw, &QListWidget::itemSelectionChanged, this, &PHIMultiSelectItem::slotChanged );
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
    QStringList list=t.split( realDelimiter() );
    lw->clear();
    QListWidgetItem *it;
    QString d, opt;
    bool isChecked;
    foreach ( d, list ) {
        PHI::getItemCheckData( d, opt, isChecked );
        it=new QListWidgetItem( d );
        lw->addItem( it );
        it->setData( Qt::UserRole, opt );
        it->setSelected( isChecked );
    }
}

QScriptValue PHIMultiSelectItem::selected( const QScriptValue &v, const QScriptValue &b )
{
    if ( !isClientItem() ) return QScriptValue( QScriptValue::UndefinedValue );
    QListWidget *lw=qobject_cast<QListWidget*>(widget());
    Q_ASSERT( lw );
    if ( !v.isValid() ) {
        QScriptValue list=scriptEngine()->newArray();
        int c=0;
        for ( int i=0; i<lw->count(); i++ ) {
            if ( lw->item( i )->isSelected() ) {
                if ( lw->item( i )->data( Qt::UserRole ).toString().isEmpty() ) {
                    list.setProperty( c++, lw->item( i )->text() );
                } else list.setProperty( c++, lw->item( i )->data( Qt::UserRole ).toString() );
            }
        }
        return list;
    }
    if ( !b.isValid() ) {
        for ( int i=0; i<lw->count(); i++ ) {
            if ( lw->item( i )->data( Qt::UserRole ).toString().isEmpty() ) {
                if ( lw->item( i )->text()==v.toString() ) return lw->item( i )->isSelected();
            } else if ( lw->item( i )->data( Qt::UserRole ).toString()==v.toString() )
                return lw->item( i )->isSelected();
        }
        return false;
    }
    for ( int i=0; i<lw->count(); i++ ) {
        if ( lw->item( i )->data( Qt::UserRole ).toString().isEmpty() ) {
            if ( lw->item( i )->text()==v.toString() ) {
                lw->item( i )->setSelected( b.toBool() );
                return self();
            }
        } else if ( lw->item( i )->data( Qt::UserRole ).toString()==v.toString() ) {
            lw->item( i )->setSelected( b.toBool() );
            return self();
        }
    }
    return self();
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

void PHIMultiSelectItem::clientPostData( QHttpMultiPart *multiPart ) const
{
    QListWidget *lw=qobject_cast<QListWidget*>(widget());
    Q_ASSERT( lw );
    QByteArray contentType=BL( "text/plain" );
    QByteArray disposition=BL( "form-data; name=\"" )+id()+BL( "\"" );
    for ( int i=0; i<lw->count(); i++ ) {
        QListWidgetItem *it=lw->item( i );
        if ( it->isSelected() ) {
            QHttpPart hp;
            hp.setHeader( QNetworkRequest::ContentTypeHeader, contentType );
            hp.setHeader( QNetworkRequest::ContentDispositionHeader, disposition );
            if ( it->data( Qt::UserRole ).toString().isEmpty() ) hp.setBody( it->text().toUtf8() );
            else hp.setBody( it->data( Qt::UserRole ).toString().toUtf8() );
            multiPart->append( hp );
        }
    }
}
