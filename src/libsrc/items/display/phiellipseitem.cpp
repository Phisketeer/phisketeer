/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2014  Phisys AG, Switzerland
#    Copyright (C) 2012-2014  Phisketeer.org team
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
#include "phidataparser.h"

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
    _startSpin->setEnabled( eit->startData()->isUnparsedStatic() );
    _startLabel->setEnabled( eit->startData()->isUnparsedStatic() );
    _spanSpin->setEnabled( eit->spanData()->isUnparsedStatic() );
    _spanLabel->setEnabled( eit->spanData()->isUnparsedStatic() );
    _startSpin->setValue( eit->startData()->isUnparsedStatic() ? eit->startData()->integer() : 0 );
    _spanSpin->setValue( eit->spanData()->isUnparsedStatic() ? eit->spanData()->integer() : 5760 );
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
    eit->privateUpdateData();
    repaint();
}

void PHIEllipseConfig::spanSpinChanged( int v )
{
    PHIEllipseItem *eit=qobject_cast<PHIEllipseItem*>(item());
    eit->spanData()->setInteger( v );
    eit->privateUpdateData();
    repaint();
}

PHIConfigData PHIEllipseConfig::originalData() const
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
    _startSpin->setEnabled( it->startData()->isUnparsedStatic() );
    _startLabel->setEnabled( it->startData()->isUnparsedStatic() );
    _startSpin->blockSignals( true );
    if ( it->startData()->isUnparsedStatic() ) _startSpin->setValue( it->startData()->integer() );
    else _startSpin->setValue( 0 );
    _startSpin->blockSignals( false );
    it->privateUpdateData();
}

void PHIEllipseConfig::spanToolClicked()
{
    PHIEllipseItem *it=qobject_cast<PHIEllipseItem*>(item());
    emit requestTextConfig( it->spanData() );
    _spanSpin->setEnabled( it->spanData()->isUnparsedStatic() );
    _spanLabel->setEnabled( it->spanData()->isUnparsedStatic() );
    _spanSpin->blockSignals( true );
    if ( it->spanData()->isUnparsedStatic() ) _spanSpin->setValue( it->spanData()->integer() );
    else _spanSpin->setValue( 5760 );
    _spanSpin->blockSignals( false );
    it->privateUpdateData();
}

bool PHIEllipseConfig::storeData()
{
    PHIEllipseItem *it=qobject_cast<PHIEllipseItem*>(item());
    bool changed=PHIColorConfig::storeData();
    if ( *_oldStartData!=*it->startData() ) changed=true;
    if ( *_oldSpanData!=*it->spanData() ) changed=true;
    return changed;
}

void PHIEllipseItem::ideInit()
{
    PHIAbstractShapeItem::ideInit();
    _startData.setInteger( 0 );
    _spanData.setInteger( 5760 );
}

void PHIEllipseItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    if ( dirtyFlags() & DFColor || dirtyFlags() & DFBackgroundColor || dirtyFlags() & DFDoNotCache )
        setImagePath( PHIDataParser::createImage( req, this, createImage(), PHIData::c(), -1 ) );
    PHIAbstractShapeItem::html( req, out, script, indent );
}

QScriptValue PHIEllipseItem::startAngle( const QScriptValue &v )
{
    if ( !isServerItem() ) return scriptEngine()->undefinedValue();
    if ( !v.isValid() ) return realStartAngle();
    setStartAngle( v.toInt32() );
    setDirtyFlag( DFDoNotCache );
    return self();
}

QScriptValue PHIEllipseItem::spanAngle( const QScriptValue &v )
{
    if ( !isServerItem() ) return scriptEngine()->undefinedValue();
    if ( !v.isValid() ) return realSpanAngle();
    setSpanAngle( v.toInt32() );
    setDirtyFlag( DFDoNotCache );
    return self();
}

void PHIEllipseItem::drawShape( QPainter *p, const QRectF& )
{
    p->setRenderHint( QPainter::Antialiasing );
    if ( realSpanAngle()==5760 ) p->drawEllipse( rect() );
    else p->drawPie( rect(), realStartAngle(), realSpanAngle() );
}

PHIConfigWidget* PHIEllipseItem::ideConfigWidget()
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

void PHIEllipseItem::ideUpdateData()
{
    if ( _startData.isUnparsedStatic() ) setData( DStartAngle, _startData.integer() );
    else if ( _startData.isUnparsedTranslated() ) setData( DStartAngle, _startData.integer( page()->currentLang() ) );
    else setData( DStartAngle, 0 );
    if ( _spanData.isUnparsedStatic() ) setData( DSpanAngle, _spanData.integer() );
    else if ( _spanData.isUnparsedTranslated() ) setData( DSpanAngle, _spanData.integer( page()->currentLang() ) );
    else setData( DSpanAngle, 5760 );
    update();
}

void PHIEllipseItem::phisCreateData( const PHIDataParser &parser )
{
    setData( DStartAngle, parser.text( &_startData ) );
    if ( !_startData.isUnparsedStatic() ) {
        setDirtyFlag( DFInt1 );
        setFlag( FDoNotCache );
    }
    setData( DSpanAngle, parser.text( &_spanData ) );
    if ( !_spanData.isUnparsedStatic() ) {
        setDirtyFlag( DFInt2 );
        setFlag( FDoNotCache );
    }
    // create image only if we are not able to cache:
    if ( !(flags() & FDoNotCache) ) PHIAbstractShapeItem::phisCreateData( parser );
}

void PHIEllipseItem::phisParseData( const PHIDataParser &parser )
{
    if ( dirtyFlags() & DFInt1 ) setData( DStartAngle, parser.text( &_startData ) );
    if ( dirtyFlags() & DFInt2 ) setData( DSpanAngle, parser.text( &_spanData ) );
    PHIAbstractShapeItem::phisParseData( parser );
}

QRectF PHIEllipseItem::boundingRect() const
{
    return PHIAbstractShapeItem::boundingRect().adjusted( -1, -1, 2, 2 );
}
