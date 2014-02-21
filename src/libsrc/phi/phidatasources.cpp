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
#include "phidatasources.h"

const QByteArray PHIData::_c=QByteArray( "C" );

void PHIData::squeeze()
{
    if ( type()==Text || type()==Integer || type()==Boolean ) {
        foreach( QByteArray k, _data.keys() ) {
            if ( _data.value( k ).toByteArray().isEmpty() ) _data.remove( k );
        }
    }
    _data.squeeze();
}

QDataStream& operator>>( QDataStream &in, PHIData *&phiData )
{
    if ( phiData ) delete phiData;
    quint8 type, source, options;
    in >> type >> source >> options;

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
    case PHIData::ImageBook:
        phiData=new PHIImageBookData(); break;
    default:
        qWarning( "unknown PHIData type" );
        phiData=new PHITextData();
    }
    phiData->setSource( static_cast<PHIData::Source>(source) );
    phiData->setOptions( static_cast<PHIData::Options>(options) );
    in >> phiData->_data;
    phiData->squeeze();
    return in;
}

QDataStream& operator<<( QDataStream &out, PHIData *phiData )
{
    out << static_cast<quint8>(phiData->type()) << static_cast<quint8>(phiData->source())
        << static_cast<quint8>(phiData->options());
    phiData->squeeze();
    if ( phiData->type()==PHIData::Image && (phiData->options() & PHIData::DontSaveImage) ) {
        phiData->_data.remove( PHIData::c() );
    } else if ( phiData->type()==PHIData::ImageBook && (phiData->options() & PHIData::DontSaveImage) ) {
        phiData->_data.remove( PHIData::c() );
    }
    out << phiData->_data;
    return out;
}
