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
#include <QLabel>
#include <QSpinBox>
#include "phirectitem.h"
#include "phibasepage.h"

PHIRectConfig::PHIRectConfig( PHIBaseItem *it, QWidget *parent )
    : PHIColorConfig( it, parent ), _oldRadiusData( 0 )
{
    _configBox->show();
    _configBox->setTitle( tr( "Border" ) );
    _note->hide();
    _oldRadiusData=new PHIIntData();
    _radiusSpin=new QSpinBox();
    _radiusSpin->setMaximumWidth( 80 );
    _radiusSpin->setRange( 0, 1000 );
    _radiusSpin->setSingleStep( 1 );
    _radiusLabel=new QLabel( tr( "Radius" ) );
    _radiusLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    _radiusTool=new QToolButton();
    _radiusTool->setArrowType( Qt::RightArrow );
    connect( _radiusTool, &QToolButton::clicked, this, &PHIRectConfig::radiusToolClicked );
    QHBoxLayout *l=new QHBoxLayout();
    l->setContentsMargins( 12, 6, 12, 6 );
    l->setSpacing( 6 );
    l->addWidget( _radiusLabel );
    l->addWidget( _radiusSpin );
    l->addWidget( _radiusTool );
    l->addWidget( new QLabel() );
    _configBox->setLayout( l );

    PHIRectItem *rit=qobject_cast<PHIRectItem*>(it);
    *_oldRadiusData=*rit->radiusData();
    _radiusSpin->setEnabled( rit->radiusData()->unparsedStatic() );
    _radiusLabel->setEnabled( rit->radiusData()->unparsedStatic() );
    _radiusSpin->setValue( rit->radiusData()->unparsedStatic() ? rit->radiusData()->integer() : 0 );
    connect( _radiusSpin, SIGNAL(valueChanged(int)), this, SLOT(radiusSpinChanged(int)) );
}

PHIRectConfig::~PHIRectConfig()
{
    delete _oldRadiusData;
}

void PHIRectConfig::radiusToolClicked()
{
    PHIRectItem *it=qobject_cast<PHIRectItem*>(item());
    emit requestTextConfig( it->radiusData() );
    _radiusSpin->setEnabled( it->radiusData()->unparsedStatic() );
    _radiusLabel->setEnabled( it->radiusData()->unparsedStatic() );
    _radiusSpin->blockSignals( true );
    if ( it->radiusData()->unparsedStatic() ) _radiusSpin->setValue( it->radiusData()->integer() );
    else _radiusSpin->setValue( 0 );
    _radiusSpin->blockSignals( false );
    it->updateData();
}

void PHIRectConfig::radiusSpinChanged( int r )
{
    PHIRectItem *it=qobject_cast<PHIRectItem*>(item());
    it->radiusData()->setInteger( r );
    it->updateData();
    repaint();
}

bool PHIRectConfig::storeData()
{
    PHIRectItem *it=qobject_cast<PHIRectItem*>(item());
    bool changed=PHIColorConfig::storeData();
    if ( *_oldRadiusData!=*it->radiusData() ) changed=true;
    return changed;
}

PHIConfigData PHIRectConfig::oldData() const
{
    PHIRectItem *it=qobject_cast<PHIRectItem*>(item());
    PHIConfigData cd;
    if ( *it->radiusData()!=*_oldRadiusData ) cd.insert( it->radiusData(), _oldRadiusData );
    return cd;
}

void PHIRectItem::drawShape( QPainter *p, const QRectF& )
{
    //bool b=( borderRadius() || hasTransformation() ) ? true : false;
    p->setRenderHint( QPainter::Antialiasing, hasTransformation() ? true : false );
    if ( borderRadius() ) p->drawRoundedRect( rect(), borderRadius(), borderRadius() );
    else p->drawRect( rect() );
}

void PHIRectItem::loadItemData( QDataStream &in, int version )
{
    PHIAbstractShapeItem::loadItemData( in, version );
    in >> &_radiusData;
}

void PHIRectItem::saveItemData( QDataStream &out, int version )
{
    PHIAbstractShapeItem::saveItemData( out, version );
    out << &_radiusData;
}

void PHIRectItem::squeeze()
{
    PHIAbstractShapeItem::squeeze();
    removeData( DBorderRadius );
}

void PHIRectItem::updateData()
{
    if ( _radiusData.unparsedStatic() ) setData( DBorderRadius, _radiusData.integer() );
    else if ( _radiusData.translated() ) setData( DBorderRadius, _radiusData.integer( page()->currentLang() ) );
    else setData( DBorderRadius, 0 );
    update();
}

PHIConfigWidget* PHIRectItem::configWidget()
{
    return new PHIRectConfig( this );
}
