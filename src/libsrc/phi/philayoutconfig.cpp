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
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QTimer>
#include "philayoutconfig.h"
#include "phiabstractitems.h"

PHILayoutWidget::PHILayoutWidget( PHIAbstractLayoutItem *item, QWidget *parent )
    : QWidget( parent ), _item( item )
{
    setupUi( this );
    _headerBox->setEnabled( false );
    _enableHeader->setChecked( _item->enableHeader() );
    _borderWidth->setValue( _item->penWidth() );
    _top->setValue( _item->topMargin() );
    _left->setValue( _item->leftMargin() );
    _right->setValue( _item->rightMargin() );
    _bottom->setValue( _item->bottomMargin() );
    _topLeft->setValue( _item->topLeftRadius() );
    _topRight->setValue( _item->topRightRadius() );
    _bottomLeft->setValue( _item->bottomLeftRadius() );
    _bottomRight->setValue( _item->bottomRightRadius() );
    _hspace->setValue( _item->horizontalSpacing() );
    _vspace->setValue( _item->verticalSpacing() );
    if ( _item->line()==0 && _item->pattern()==0 ) _borderBox->setEnabled( false );
    else _borderBox->setEnabled( true );
    connect( _enableHeader, &QCheckBox::toggled, this, &PHILayoutWidget::enableHeaderChanged );
    QTimer::singleShot( 0, this, SLOT(init()) );
}

void PHILayoutWidget::init()
{
    emit enableHeaderChanged( _enableHeader->isChecked() );
}

void PHILayoutWidget::storeData()
{
    _item->setEnableHeader( _enableHeader->isChecked() );
    _item->setPenWidth( _borderWidth->value() );
    _item->setTopMargin( _top->value() );
    _item->setLeftMargin( _left->value() );
    _item->setRightMargin( _right->value() );
    _item->setBottomMargin( _bottom->value() );
    _item->setTopLeftRadius( _topLeft->value() );
    _item->setTopRightRadius( _topRight->value() );
    _item->setBottomLeftRadius( _bottomLeft->value() );
    _item->setBottomRightRadius( _bottomRight->value() );
    _item->setHorizontalSpacing( _hspace->value() );
    _item->setVerticalSpacing( _vspace->value() );
}

PHILayoutConfig::PHILayoutConfig( PHIAbstractLayoutItem *item, QWidget *parent )
    : PHIColorConfig( item, parent )
{
    _layoutWidget=new PHILayoutWidget( item );
    connect( _layoutWidget, &PHILayoutWidget::enableHeaderChanged, this, &PHILayoutConfig::enableHeaderChanged );
    _note->hide();
    _preview->hide();
    _colorLabel->setText( tr( "Background color" ) );
    _outlineColorLabel->setText( tr( "Border color" ) );
    QVBoxLayout *l=qobject_cast<QVBoxLayout*>(layout());
    Q_ASSERT( l );
    l->insertWidget( 1, _layoutWidget );
    l->insertItem( 3, new QSpacerItem( 0, 30, QSizePolicy::Minimum, QSizePolicy::Expanding ) );
}

bool PHILayoutConfig::storeData()
{
    _layoutWidget->storeData();
    return PHIColorConfig::storeData();
}
