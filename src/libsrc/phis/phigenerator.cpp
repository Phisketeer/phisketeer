/*
#    Copyright (C) 2010-2012  Marius B. Schumacher
#    Copyright (C) 2011-2012  Phisys AG, Switzerland
#    Copyright (C) 2012  Phisketeer.org team
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
#include <QDataStream>
#include <QHash>
#include "phigenerator.h"
#include "phi.h"

PHIGenerator::PHIGenerator( PHIResponseRec *resp )
    : _resp( resp )
{
    qDebug( "PHIGenerator::PHIGenerator()" );
    Q_ASSERT( resp );
}

PHIGenerator::~PHIGenerator()
{
    qDebug( "PHIGenerator::~PHIGenerator()" );
}

QByteArray PHIGenerator::genPhi( const PHIBasePage *p )
{
    _resp->setContentType( PHI::mimeType() );
    QByteArray arr, block;
    QDataStream out( &arr, QIODevice::WriteOnly );
    out.setVersion( PHI_DSV );
    QDataStream ds( &block, QIODevice::WriteOnly );
    ds.setVersion( PHI_DSV );
    // send magic number and version number
    out << static_cast<quint32>(PHI_MAGIC) << static_cast<quint8>(1)
        << static_cast<quint8>(PHI::CDAll);
    ds << p;
    out << block;
    PHIBaseItem *it;
    foreach ( it, p->items() ) {
        block.clear();
        ds.device()->reset();
        ds << it;
        out << block;
        qDebug( "BLOCK SIZE %d", block.size() );
    }
    return arr;
}
