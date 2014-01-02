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
#include <QColorDialog>
#include <QPainter>
#include <QPen>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QGuiApplication>
#include <QGroupBox>
#include "phicolorconfig.h"
#include "phibaseitem.h"
#include "phiabstractitems.h"

static void drawBox( QPainter *p, const QRect &r )
{
    QColor c1( Qt::white );
    c1.setAlpha( 200 );
    QColor c2( Qt::black );
    c2.setAlpha( 200 );
    QPen pen( c1 );
    pen.setWidth( 2 );
    p->setPen( pen );
    p->drawLine( 1, 0, r.width(), 0 );
    p->drawLine( 0, 0, 0, r.height() );
    pen.setColor( c2 );
    p->setPen( pen );
    p->drawLine( r.width(), 1, r.width(), r.height() );
    p->drawLine( 1, r.height(), r.width(), r.height() );
}

static void drawChessPattern( QPainter *p, const QRect &r )
{
    p->fillRect( 0, 0, r.width()/2, r.height()/2, Qt::darkGray );
    p->fillRect( r.width()/2, 0, r.width()/2, r.height()/2, Qt::lightGray );
    p->fillRect( 0, r.height()/2, r.width()/2, r.height()/2, Qt::lightGray );
    p->fillRect( r.width()/2, r.height()/2, r.width()/2, r.height()/2, Qt::darkGray );
}

static QPixmap colorPixmap( const QColor &c, const QSize &s=QSize( 24, 24 ) )
{
    QPixmap pix( s );
    QRect r( QPoint(), s );
    QPainter p( &pix );
    drawChessPattern( &p, r );
    p.fillRect( r, c );
    drawBox( &p, r );
    p.end();
    return pix;
}

PHIColorPreview::PHIColorPreview( PHIBaseItem *it, QWidget *parent )
    : QWidget( parent ), _item( it ), _edit( 0 )
{
    if ( qobject_cast<PHIAbstractTextItem*>(it) ) {
        _edit=new QPlainTextEdit( tr( "Sample colored text" ) );
        _edit->setReadOnly( true );
        QVBoxLayout *l=new QVBoxLayout();
        l->setContentsMargins( 50, 30, 50, 30 );
        l->addWidget( _edit );
        setLayout( l );
    }
}

void PHIColorPreview::paintEvent( QPaintEvent *e )
{
    if ( _edit ) {
        QPalette pal=QGuiApplication::palette();
        pal.setColor( QPalette::Text, _item->colorForRole( PHIPalette::WidgetText ) );
        pal.setColor( QPalette::Base, _item->colorForRole( PHIPalette::WidgetBase ) );
        _edit->setPalette( pal );
        return QWidget::paintEvent( e );
    }
    QPainter p( this );
    QSizeF itemSize=_item->realSize();
    QSize widgetSize=size();
    QPointF off( widgetSize.width()/2.-(itemSize.width()/2.), widgetSize.height()/2.-(itemSize.height()/2.) );
    p.translate( off );
    _item->paint( &p, QRectF() );
    p.end();
}

PHIColorConfig::PHIColorConfig( PHIBaseItem *item, QWidget *parent )
    : PHIConfigWidget( item, parent )
{
    setupUi( this );
    QVBoxLayout *l=new QVBoxLayout();
    l->setContentsMargins( 0, 0, 0, 0 );
    l->addWidget( new PHIColorPreview( item ) );
    _preview->setLayout( l );
    _configBox->hide();
    if ( qobject_cast<PHIAbstractTextItem*>(item) ) {
        _colorLabel->setText( tr( "Text color" ) );
        _outlineColorLabel->setText( tr( "Base color" ) );
        _itemRoles << PHIPalette::WidgetText << PHIPalette::WidgetBase;
    } else {
        _colorLabel->setText( tr( "Pattern color" ) );
        _outlineColorLabel->setText( tr( "Outline color" ) );
        _itemRoles << PHIPalette::Foreground << PHIPalette::Background;
        if ( item->property( "_pattern" ).value<quint8>()==15 ) {
            _colorButton->setEnabled( false );
            _colorLabel->setEnabled( false );
        }
        if ( item->property( "_line" ).value<quint8>()==0 ) {
            _outlineColorButton->setEnabled( false );
            _outlineColorLabel->setEnabled( false );
        }
    }
    foreach( PHIPalette::ItemRole ir, _itemRoles ) {
        _oldColors.insert( ir, item->colorForRole( ir ) );
        _oldRoles.insert( ir, item->colorRole( ir ) );
        if ( ir==PHIPalette::Foreground || ir==PHIPalette::WidgetText )
            _colorButton->setIcon( colorPixmap( item->colorForRole( ir ) ) );
        if ( ir==PHIPalette::Background || ir==PHIPalette::WidgetBase )
            _outlineColorButton->setIcon( colorPixmap( item->colorForRole( ir ) ) );
    }
}

PHIColorConfig::~PHIColorConfig()
{
    qDebug( "PHIColorConfig::~PHIColorConfig()" );
}

bool PHIColorConfig::storeData()
{
    bool changed=false;
    foreach ( PHIPalette::ItemRole ir, _itemRoles ) {
        if ( _oldColors.value( ir )!=item()->colorForRole( ir ) ) changed=true;
    }
    return changed;
}

PHIConfigColors PHIColorConfig::oldColors() const
{
    PHIConfigColors cc;
    foreach ( PHIPalette::ItemRole ir, _itemRoles ) {
        if ( _oldColors.value( ir )==item()->colorForRole( ir ) ) continue;
        cc.insert( ir, qMakePair( _oldRoles.value( ir, PHIPalette::NoRole ), _oldColors.value( ir, QColor() ) ) );
    }
    return cc;
}

void PHIColorConfig::on__colorButton_clicked()
{
    QString title=tr( "Pattern color" );
    if ( _itemRoles.at( 0 )==PHIPalette::WidgetText ) title=tr( "Text color" );
    QColor col=QColorDialog::getColor( item()->colorForRole( _itemRoles.at( 0 ) ), this, title, QColorDialog::ShowAlphaChannel );
    if ( !col.isValid() ) return;
    item()->setColor( _itemRoles.at( 0 ), PHIPalette::Custom, col );
    _colorButton->setIcon( colorPixmap( col ) );
    _preview->repaint();
}

void PHIColorConfig::on__outlineColorButton_clicked()
{
    QString title=tr( "Outline color" );
    if ( _itemRoles.at( 1 )==PHIPalette::WidgetBase ) title=tr( "Base color" );
    QColor col=QColorDialog::getColor( item()->colorForRole( _itemRoles.at( 1 ) ), this, title, QColorDialog::ShowAlphaChannel );
    if ( !col.isValid() ) return;
    item()->setColor( _itemRoles.at( 1 ), PHIPalette::Custom, col );
    _outlineColorButton->setIcon( colorPixmap( col ) );
    _preview->repaint();
}
