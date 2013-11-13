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
#include "phirequest.h"
#include "phidataparser.h"

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
    it->privateUpdateData();
}

void PHIRectConfig::radiusSpinChanged( int r )
{
    PHIRectItem *it=qobject_cast<PHIRectItem*>(item());
    it->radiusData()->setInteger( r );
    it->privateUpdateData();
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
    p->setRenderHint( QPainter::Antialiasing, hasTransformation() );
    QPen pen=p->pen();
    p->setPen( Qt::NoPen );
    QRectF cr=rect();
    if ( realLine()>0 ) {
        qreal rw=realPenWidth();
        cr.adjust( -rw, -rw, rw, rw );
    }
    if ( borderRadius() ) {
        p->setRenderHint( QPainter::Antialiasing );
        p->drawRoundedRect( cr, borderRadius(), borderRadius() );
        p->setRenderHint( QPainter::Antialiasing, false );
    } else p->drawRect( cr );
    if ( realLine()>0 ) {
        // draw border:
        qreal off=realPenWidth()/2.;
        cr=QRectF( -off, -off, realWidth()+2*off, realHeight()+2*off );
        p->setBrush( Qt::NoBrush );
        p->setPen( pen );
        if ( borderRadius() ) p->drawRoundedRect( cr, borderRadius(), borderRadius() );
        else p->drawRect( cr );
    }
}

void PHIRectItem::createTmpData( const PHIDataParser &parser )
{
    if ( _radiusData.unparsedStatic() ) {
        setData( DBorderRadius, _radiusData.integer() );
        PHIAbstractShapeItem::createTmpData( parser ); // create image
    } else {
        setDirtyFlag( DFInt1 );
        setFlag( FDoNotCache );
    }
}

void PHIRectItem::parseData( const PHIDataParser &parser )
{
    if ( dirtyFlags() & DFInt1 ) setData( DBorderRadius, parser.text( &_radiusData ).toInt() );
}

void PHIRectItem::html( const PHIRequest *req, QByteArray &out, QByteArray &jquery, const QByteArray &indent ) const
{
    if ( Q_UNLIKELY( req->agentFeatures() & PHIRequest::IE678 ) )
        return PHIAbstractShapeItem::html( req, out, jquery, indent );
    bool needImage=false;
    if ( Q_UNLIKELY( borderRadius() && !(req->agentFeatures() & PHIRequest::BorderRadius) ) ) needImage=true;
    else if ( Q_UNLIKELY( realLine()>3 ) ) needImage=true;
    else if ( Q_UNLIKELY( !(req->agentFeatures() & PHIRequest::RGBA) ) ) needImage=true;
    else if ( Q_UNLIKELY( realPattern()>1 && realPattern()<15 ) ) needImage=true;
    else if ( hasGraphicEffect() ) {
        if ( effect()->graphicsType()==PHIEffect::GTShadow ) {
            if ( !(req->agentFeatures() & PHIRequest::BoxShadow) ) needImage=true;
        } else needImage=true;
    } else if ( realPattern()==15 ) {
        if( Q_UNLIKELY( !(req->agentFeatures() & PHIRequest::Gradients) ) ) needImage=true;
        else if ( data( DGradientType, 0 ).toInt()!=0 ) needImage=true;
    }
    if ( !needImage && hasTransformation() ) {
        if ( computeTransformation().isAffine() && !(req->agentFeatures() & PHIRequest::Transform2D) ) needImage=true;
        else if ( Q_UNLIKELY( !(req->agentFeatures() & PHIRequest::Transform3D) ) ) needImage=true;
    }
    if ( Q_LIKELY( !needImage ) ) {
        out+=indent+BL( "<div" );
        startCSS( req, out, jquery, true );
        if ( realPattern()==1 ) out+=BL( "background-color:" )+rgba( realColor() )+';';
        else if ( realPattern()==15 ) genLinearGradient( req, out );
        qDebug() << borderRadius();
        if ( borderRadius() ) { // rounded corners
            QByteArray prefix=QByteArray();
            if ( req->agentEngine()==PHIRequest::WebKit && req->engineMajorVersion()<534 ) prefix=req->agentPrefix();
            else if ( req->agentEngine()==PHIRequest::Gecko && req->engineMajorVersion()<2 ) prefix=req->agentPrefix();
            out+=prefix+BL( "border-radius:" )+QByteArray::number( borderRadius() )+BL( "px;" );
        }
        if ( realLine()>0 ) { // border
            QByteArray style=BL( "solid" );
            if ( realLine()==2 ) style=BL( "dashed" );
            else if ( realLine()==3 ) style=BL( "dotted" );
            out+=BL( "border:" )+QByteArray::number( qRound(realPenWidth()) )+BL( "px " )+style+' '+rgba( realOutlineColor() )+';';
            setAdjustedRect( QRectF( -realPenWidth(), -realPenWidth(), realWidth(), realHeight() ) );
            genAdjustedPos( jquery );
        }
        if ( effect()->graphicsType()==PHIEffect::GTShadow ) {
            QByteArray prefix=req->agentPrefix();
            if ( req->agentEngine()==PHIRequest::Gecko && req->engineMajorVersion()>1 ) prefix=QByteArray();
            else if ( req->agentEngine()==PHIRequest::WebKit && req->engineMajorVersion()>534 ) prefix=QByteArray();
            else if ( req->agentEngine()==PHIRequest::Presto ) prefix=QByteArray();
            QColor c;
            qreal xOff, yOff, radius;
            effect()->shadow( c, xOff, yOff, radius );
            out+=prefix+BL( "box-shadow:" )+QByteArray::number( qRound(xOff) )+"px ";
            out+=QByteArray::number( qRound(yOff) )+"px ";
            out+=QByteArray::number( qRound(radius) )+"px "+rgba( c )+';';
        }
        out+=BL( "\"></div>\n" );
    } else {
        out+=indent+BL( "<img" );
        startCSS( req, out, jquery, false );
        QRectF br=boundingRect();
        QByteArray imgId=PHIDataParser::createTransformedImageId( req, this, 0, br );
        out+=BL( "\" src=\"phi.phis?i=" )+imgId+BL( "&amp;t=1\">\n" );
        setAdjustedRect( br );
        genAdjustedPos( jquery );
        genAdjustedSize( jquery );
    }
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
