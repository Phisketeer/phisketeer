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
    _radiusSpin->setEnabled( rit->radiusData()->isUnparsedStatic() );
    _radiusLabel->setEnabled( rit->radiusData()->isUnparsedStatic() );
    _radiusSpin->setValue( rit->radiusData()->isUnparsedStatic() ? rit->radiusData()->integer() : 0 );
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
    _radiusSpin->setEnabled( it->radiusData()->isUnparsedStatic() );
    _radiusLabel->setEnabled( it->radiusData()->isUnparsedStatic() );
    _radiusSpin->blockSignals( true );
    if ( it->radiusData()->isUnparsedStatic() ) _radiusSpin->setValue( it->radiusData()->integer() );
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

PHIConfigData PHIRectConfig::originalData() const
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
    if ( realBorderRadius() ) {
        p->setRenderHint( QPainter::Antialiasing );
        p->drawRoundedRect( cr, realBorderRadius(), realBorderRadius() );
        p->setRenderHint( QPainter::Antialiasing, false );
    } else p->drawRect( cr );
    if ( realLine()>0 ) {
        // draw border:
        qreal off=realPenWidth()/2.;
        cr=QRectF( -off, -off, realWidth()+2*off, realHeight()+2*off );
        p->setBrush( Qt::NoBrush );
        p->setPen( pen );
        if ( realBorderRadius() ) {
            if ( realPenWidth()>1. ) p->setRenderHint( QPainter::Antialiasing );
            p->drawRoundedRect( cr, realBorderRadius(), realBorderRadius() );
        } else p->drawRect( cr );
    }
}

void PHIRectItem::phisCreateData( const PHIDataParser &parser )
{
    setData( DBorderRadius, parser.text( &_radiusData ) );
    if ( !_radiusData.isUnparsedStatic() ) setDirtyFlag( DFInt1 );
    PHIAbstractShapeItem::phisCreateData( parser );
}

void PHIRectItem::phisParseData( const PHIDataParser &parser )
{
    if ( dirtyFlags() & DFInt1 ) {
        setData( DBorderRadius, parser.text( &_radiusData ) );
        setImagePath( parser.createImage( createImage(), PHIData::c(), -1 ) );
    } else if ( dirtyFlags() & DFDoNotCache || dirtyFlags() & DFColor || dirtyFlags() & DFBackgroundColor ) {
        setImagePath( parser.createImage( createImage(), PHIData::c(), -1 ) );
    }
}

void PHIRectItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    if ( Q_UNLIKELY( req->agentFeatures() & PHIRequest::IE678 ) ) return PHIAbstractShapeItem::html( req, out, script, indent );
    bool needImage=false;
    if ( Q_UNLIKELY( realBorderRadius() && !(req->agentFeatures() & PHIRequest::BorderRadius) ) ) needImage=true;
    else if ( Q_UNLIKELY( !(req->agentFeatures() & PHIRequest::RGBA) ) ) needImage=true;
    else if ( Q_UNLIKELY( realLine()>3 ) ) needImage=true;
    else if ( Q_UNLIKELY( realPattern()>1 && realPattern()<15 ) ) needImage=true;
    else if ( realPattern()==15 && !cssGradientCreateable( req ) ) needImage=true;
    else if ( hasGraphicEffect() ) {
        if ( effect()->graphicsType()==PHIEffect::GTShadow ) {
            if ( !(req->agentFeatures() & PHIRequest::BoxShadow) ) needImage=true;
        } else needImage=true;
    }
    if ( !needImage && hasTransformation() ) {
        if ( computeTransformation( false ).isAffine() && !(req->agentFeatures() & PHIRequest::Transform2D) ) needImage=true;
        else if ( Q_UNLIKELY( !(req->agentFeatures() & PHIRequest::Transform3D) ) ) needImage=true;
    }
    if ( Q_LIKELY( !needImage ) ) {
        QByteArray tmp;
        out+=indent+BL( "<div" );
        htmlBase( req, out, script, true );
        if ( realPattern()==1 ) tmp+=BL( ".color('" )+cssColor( realColor() )+BL( "')" );
        else if ( realPattern()==15 ) cssLinearGradient( req, out );
        if ( realBorderRadius() ) { // rounded corners
            tmp+=BL( ".borderRadius(" )+QByteArray::number( realBorderRadius() )+')';
        }
        if ( realLine()>0 ) { // border
            QByteArray style=BL( "solid" );
            if ( realLine()==2 ) style=BL( "dashed" );
            else if ( realLine()==3 ) style=BL( "dotted" );
            out+=BL( "border:" )+QByteArray::number( qRound(realPenWidth()) )+BL( "px " )+style+';';
            tmp+=BL( ".borderColor('" )+cssColor( realOutlineColor() )+BL( "')" );
            QPointF off=boundingRect().topLeft();
            if ( hasTransformation() ) off=computeTransformation( false ).map( off );
            setAdjustedRect( QRectF( off, realSize() ) );
        }
        if ( hasGraphicEffect() ) PHIBaseItem::cssGraphicEffect( req, out, script );
        /*
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
            out+=QByteArray::number( qRound(radius) )+"px "+cssRgba( c )+';';
        }
        */
        out+=BL( "\"></div>\n" );
        htmlInitItem( script, false );
        script+=tmp+BL( ";\n" );
    } else {
        out+=indent+BL( "<img" );
        htmlBase( req, out, script, false );
        QRectF br=boundingRect();
        QByteArray imgId=PHIDataParser::createTransformedImage( req, this, 0, br );
        out+=BL( "\" src=\"phi.phis?i=" )+imgId+BL( "&t=1\">\n" );
        setAdjustedRect( br );
        htmlInitItem( script );
    }
}

QScriptValue PHIRectItem::borderRadius( const QScriptValue &r )
{
    if ( !r.isValid() ) return realBorderRadius();
    setDirtyFlag( DFDoNotCache );
    setBorderRadius( static_cast<qint16>(r.toInt32()) );
    return self();
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

void PHIRectItem::ideUpdateData()
{
    PHIAbstractShapeItem::ideUpdateData();
    if ( _radiusData.isUnparsedStatic() ) setData( DBorderRadius, _radiusData.integer() );
    else if ( _radiusData.isUnparsedTranslated() ) setData( DBorderRadius, _radiusData.integer( page()->currentLang() ) );
    else setData( DBorderRadius, 0 );
    update();
}

PHIConfigWidget* PHIRectItem::ideConfigWidget()
{
    return new PHIRectConfig( this );
}
