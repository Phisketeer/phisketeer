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
#include <QBuffer>
#include <QPixmap>
#include "phidatasources.h"
#include "phi.h"

const QByteArray PHIData::_c=QByteArray( "C" );

PHIData::PHIData(): _source( PHIData::Static ), _options( PHIData::None )
{
}

PHIData::~PHIData()
{
}

PHIData::PHIData( const PHIData &d )
{
    _source=d._source;
    _options=d._options;
    _data=d._data;
}

PHIData& PHIData::operator =( const PHIData &d )
{
    _source=d._source;
    _options=d._options;
    _data=d._data;
    return *this;
}

QDataStream& PHIData::readData( QDataStream &in, PHIData *phiData )
{
    quint8 type, source, options;
    PHIVariantHash data;
    in >> type >> source >> options >> data;
    Q_ASSERT( static_cast<PHIData::Type>(type)==phiData->type() );
    phiData->setSource( static_cast<PHIData::Source>(source) );
    phiData->setOptions( static_cast<PHIData::Options>(options) );
    phiData->_data=data;
    return in;
}

/*
QByteArray PHIData::data() const
{
    QByteArray arr;
    QDataStream out( &arr, QIODevice::WriteOnly );
    out.setVersion( PHI_DSV );
    out << _data;
    return arr;
}

void PHIData::setData( const QByteArray &arr )
{
    QByteArray tmp=arr;
    QDataStream in( &tmp, QIODevice::ReadOnly );
    in.setVersion( PHI_DSV );
    in >> _data;
}
*/

QDataStream& operator>>( QDataStream &in, PHIData *&phiData )
{
    if ( phiData ) delete phiData;
    quint8 type, source, options;
    PHIVariantHash data;
    in >> type >> source >> options >> data;

    switch ( static_cast<PHIData::Type>( type ) ) {
    case PHIData::Text:
        phiData=new PHITextData(); break;
    case PHIData::Image:
        phiData=new PHIImageData(); break;
    case PHIData::Integer:
        phiData=new PHIIntData(); break;
    case PHIData::Boolean:
        phiData=new PHIBooleanData(); break;
    case PHIData::StringList:
        phiData=new PHIStringListData(); break;
    case PHIData::Color:
        phiData=new PHIColorData(); break;
    case PHIData::ImageBook:
        phiData=new PHIImageBookData(); break;
    default:
        qWarning( "unknown PHIData type" );
        phiData=new PHITextData();
    }
    phiData->setSource( static_cast<PHIData::Source>(source) );
    phiData->setOptions( static_cast<PHIData::Options>(options) );
    phiData->_data=data;
    return in;
}

QDataStream& operator<<( QDataStream &out, const PHIData *phiData )
{
    //qRegisterMetaTypeStreamOperators<PHIImageHash>("PHIImageHash");
    //qDebug( "operator << type=%d", static_cast<quint8>(phiData->type()) );
    out << static_cast<quint8>(phiData->type()) << static_cast<quint8>(phiData->source())
        << static_cast<quint8>(phiData->options());
    if ( phiData->type()==PHIData::Image && (phiData->options() & PHIData::DontSaveImage) ) {
        Q_ASSERT( dynamic_cast<const PHIImageData*>(phiData) );
        PHIImageData tmpData=*( dynamic_cast<const PHIImageData*>(phiData) );
        tmpData.remove( PHIData::c() );
        out << tmpData.data();
    } else if ( phiData->type()==PHIData::ImageBook && (phiData->options() & PHIData::DontSaveImage) ) {
        Q_ASSERT( dynamic_cast<const PHIImageBookData*>(phiData) );
        PHIImageBookData bookData=*( dynamic_cast<const PHIImageBookData*>(phiData) );
        bookData.remove( PHIData::c() );
        out << bookData.data();
    } else out << phiData->data();
    return out;
}

PHITextData::PHITextData( const QString &t )
{
    _data.insert( _c, t );
}

PHITextData::~PHITextData()
{
}

QDataStream& operator>>( QDataStream &in, PHITextData *phiData )
{
    //qDebug( "operater >> PHITextData" );
    return PHIData::readData( in, phiData );
}

PHIImageData::PHIImageData( const QImage &im )
{
    _data.insert( _c, im );
}

PHIImageData::~PHIImageData()
{
}

QImage PHIImageData::image( const QByteArray &l ) const
{
    QVariant v=_data.value( l );
    QImage img;
    if ( v.isValid() && v.canConvert<QImage>() ) img=v.value<QImage>();
    else {
        QPixmap pix( QStringLiteral( ":/gnome/brokenimage" ) );
        img=pix.toImage();
    }
    return img;
}

QDataStream& operator>>( QDataStream &in, PHIImageData *phiData )
{
    return PHIData::readData( in, phiData );
}

PHIImageBookData::PHIImageBookData( const PHIImageHash &imgHash )
{
    //qDebug( "PHIImageBookData::PHIImageBookData()" );
    QVariant v;
    v.setValue( imgHash );
    _data.insert( _c, v );
}

PHIImageBookData::~PHIImageBookData()
{
    //qDebug( "PHIImageBookData::~PHIImageBookData()" );
}

PHIImageHash PHIImageBookData::imageBook( const QByteArray &l ) const
{
    QVariant v=_data.value( l );
    PHIImageHash imgHash;
    if ( v.isValid() && v.canConvert<PHIImageHash>() ) imgHash=v.value<PHIImageHash>();
    return imgHash;
}

QDataStream& operator>>( QDataStream &in, PHIImageBookData *phiData )
{
    //qRegisterMetaTypeStreamOperators<PHIImageHash>("PHIImageHash");
    return PHIData::readData( in, phiData );
/*
    qDebug( "COUNT %d", phiData->imageBook().count() );
    QImage img=phiData->imageBook().value( QByteArray::number( 0 ) );
    if ( img.isNull() ) qDebug( "IMAGE ist null" );
    else qDebug( "IMAGE ist valid" );
    return in;
*/
}

PHIIntData::PHIIntData( qint32 i )
{
    _data.insert( _c, i );
}

PHIIntData::~PHIIntData()
{
}

QDataStream& operator>>( QDataStream &in, PHIIntData *phiData )
{
    return PHIData::readData( in, phiData );
}

PHIBooleanData::PHIBooleanData( bool b )
{
    //qDebug( "PHIBooleanData::PHIBooleanData( %d )", b );
    _data.insert( _c, b );
}

PHIBooleanData::~PHIBooleanData()
{
    //qDebug( "PHIBooleanData::~PHIBooleanData()" );
}

QDataStream& operator>>( QDataStream &in, PHIBooleanData *phiData )
{
    return PHIData::readData( in, phiData );
}

PHIStringListData::PHIStringListData( const QStringList &list )
{
    _data.insert( _c, list );
}

PHIStringListData::~PHIStringListData()
{
}

QDataStream& operator>>( QDataStream &in, PHIStringListData *phiData )
{
    return PHIData::readData( in, phiData );
}

PHIColorData::PHIColorData( const QColor &c )
{
    _data.insert( _c, c );
}

PHIColorData::~PHIColorData()
{
}

QDataStream& operator>>( QDataStream &in, PHIColorData *phiData )
{
    return PHIData::readData( in, phiData );
}

/*
QDataStream& operator >>( QDataStream &in, PHIBooleanData *&phiData )
{
    if ( phiData ) delete phiData;
    quint8 type;
    QByteArray data;
    in >> type >> data;
    if ( (type & 15)!=static_cast<quint8>(PHIData::Image) ) {
        qWarning( "unknown PHIBooleanData type" );
        phiData=new PHIStaticImage();
        return in;
    }
    switch ( static_cast<PHIBooleanData::ImageType>((type >> 4) & 7)) {
    case PHIBooleanData::StaticImage:
        phiData=new PHIStaticImage(); break;
    default:
        phiData=new PHIStaticImage();
        qWarning( "unknown PHIBooleanData sub type" );
    }
    if ( data.size()>0 ) phiData->setData( data );
    return in;
}

QDataStream& operator<<( QDataStream &out, const PHIBooleanData *phiData )
{
    quint8 type=phiData->imageType() & 15;
    type=type << 4;
    type&= ~128; // reserved
    type|= (phiData->type() & 15);
    out << type << phiData->data();
    return out;
}
*/
