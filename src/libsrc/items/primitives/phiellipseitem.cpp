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
#include <QPainter>
#include <QToolButton>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include "phiellipseitem.h"
#include "phibasepage.h"

PHIEllipseConfig::PHIEllipseConfig( PHIBaseItem *it, QWidget *parent )
    : PHIColorConfig( it, parent )
{
    _configBox->show();
    _note->hide();
    _oldStartData=new PHIIntData();
    _oldSpanData=new PHIIntData();
    _startTool=new QToolButton();
    _startTool->setArrowType( Qt::RightArrow );
    connect( _startTool, &QToolButton::clicked, this, &PHIEllipseConfig::startToolClicked );
    _spanTool=new QToolButton();
    _spanTool->setArrowType( Qt::RightArrow );
    connect( _spanTool, &QToolButton::clicked, this, &PHIEllipseConfig::spanToolClicked );
    _startSpin=new QSpinBox();
    _startSpin->setRange( 0, 5760 );
    _startSpin->setSingleStep( 16 );
    _spanSpin=new QSpinBox();
    _spanSpin->setRange( 0, 5760 );
    _spanSpin->setSingleStep( 16 );
    _spanSpin->setValue( 5760 );
    _startLabel=new QLabel( tr( "Start angle" ) );
    _startLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    QHBoxLayout *l=new QHBoxLayout();
    l->setContentsMargins( 12, 6, 12, 6 );
    l->setSpacing( 6 );
    l->addWidget( _startLabel );
    l->addWidget( _startSpin );
    l->addWidget( _startTool );
    _spanLabel=new QLabel( tr( "Span angle" ) );
    _spanLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    l->addWidget( _spanLabel );
    l->addWidget( _spanSpin );
    l->addWidget( _spanTool );
    _configBox->setLayout( l );
    PHIEllipseItem *eit=qobject_cast<PHIEllipseItem*>(it);
    _startSpin->setEnabled( eit->startData()->unparsedStatic() );
    _startLabel->setEnabled( eit->startData()->unparsedStatic() );
    _spanSpin->setEnabled( eit->spanData()->unparsedStatic() );
    _spanLabel->setEnabled( eit->spanData()->unparsedStatic() );
    _startSpin->setValue( eit->startData()->unparsedStatic() ? eit->startData()->integer() : 0 );
    _spanSpin->setValue( eit->spanData()->unparsedStatic() ? eit->spanData()->integer() : 5760 );
    *_oldStartData=*eit->startData();
    *_oldSpanData=*eit->spanData();
    connect( _startSpin, SIGNAL(valueChanged(int)), this, SLOT(startSpinChanged(int)) );
    connect( _spanSpin, SIGNAL(valueChanged(int)), this, SLOT(spanSpinChanged(int)) );
}

PHIEllipseConfig::~PHIEllipseConfig()
{
    delete _oldStartData;
    delete _oldSpanData;
}

void PHIEllipseConfig::startSpinChanged( int v )
{
    PHIEllipseItem *eit=qobject_cast<PHIEllipseItem*>(item());
    eit->startData()->setInteger( v );
    eit->updateData();
    repaint();
}

void PHIEllipseConfig::spanSpinChanged( int v )
{
    PHIEllipseItem *eit=qobject_cast<PHIEllipseItem*>(item());
    eit->spanData()->setInteger( v );
    eit->updateData();
    repaint();
}

PHIConfigData PHIEllipseConfig::oldData() const
{
    PHIEllipseItem *eit=qobject_cast<PHIEllipseItem*>(item());
    PHIConfigData cd;
    if ( *eit->startData()!=*_oldStartData ) cd.insert( eit->startData(), _oldStartData );
    if ( *eit->spanData()!=*_oldSpanData ) cd.insert( eit->spanData(), _oldSpanData );
    return cd;
}

void PHIEllipseConfig::startToolClicked()
{
    PHIEllipseItem *it=qobject_cast<PHIEllipseItem*>(item());
    emit requestTextConfig( it->startData() );
    _startSpin->setEnabled( it->startData()->unparsedStatic() );
    _startLabel->setEnabled( it->startData()->unparsedStatic() );
    _startSpin->blockSignals( true );
    if ( it->startData()->unparsedStatic() ) _startSpin->setValue( it->startData()->integer() );
    else _startSpin->setValue( 0 );
    _startSpin->blockSignals( false );
    it->updateData();
}

void PHIEllipseConfig::spanToolClicked()
{
    PHIEllipseItem *it=qobject_cast<PHIEllipseItem*>(item());
    emit requestTextConfig( it->spanData() );
    _spanSpin->setEnabled( it->spanData()->unparsedStatic() );
    _spanLabel->setEnabled( it->spanData()->unparsedStatic() );
    _spanSpin->blockSignals( true );
    if ( it->spanData()->unparsedStatic() ) _spanSpin->setValue( it->spanData()->integer() );
    else _spanSpin->setValue( 5760 );
    _spanSpin->blockSignals( false );
    it->updateData();
}

bool PHIEllipseConfig::storeData()
{
    PHIEllipseItem *it=qobject_cast<PHIEllipseItem*>(item());
    bool changed=PHIColorConfig::storeData();
    if ( *_oldStartData!=*it->startData() ) changed=true;
    if ( *_oldSpanData!=*it->spanData() ) changed=true;
    return changed;
}

PHIEllipseItem::PHIEllipseItem( const PHIBaseItemPrivate &p )
    : PHIAbstractShapeItem( p )
{
    setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
}

void PHIEllipseItem::initIDE()
{
    PHIAbstractShapeItem::initIDE();
    _startData.setInteger( 0 );
    _spanData.setInteger( 5760 );
}

void PHIEllipseItem::drawShape( QPainter *p, const QRectF& )
{
    p->setRenderHint( QPainter::Antialiasing );
    if ( spanAngle()==5760 ) p->drawEllipse( rect() );
    else p->drawPie( rect(), startAngle(), spanAngle() );
}

PHIConfigWidget* PHIEllipseItem::configWidget()
{
    return new PHIEllipseConfig( this );
}

void PHIEllipseItem::loadItemData( QDataStream &in, int version )
{
    PHIAbstractShapeItem::loadItemData( in, version );
    in >> &_startData >> &_spanData;
}

void PHIEllipseItem::saveItemData( QDataStream &out, int version )
{
    PHIAbstractShapeItem::saveItemData( out, version );
    out << &_startData << &_spanData;
}

void PHIEllipseItem::squeeze()
{
    PHIAbstractShapeItem::squeeze();
    removeData( DStartAngle );
    removeData( DSpanAngle );
}

void PHIEllipseItem::updateData()
{
    if ( _startData.unparsedStatic() ) setData( DStartAngle, _startData.integer() );
    else if ( _startData.translated() ) setData( DStartAngle, _startData.integer( page()->currentLang() ) );
    else setData( DStartAngle, 0 );
    if ( _spanData.unparsedStatic() ) setData( DSpanAngle, _spanData.integer() );
    else if ( _spanData.translated() ) setData( DSpanAngle, _spanData.integer( page()->currentLang() ) );
    else setData( DSpanAngle, 5760 );
    update();
}
