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
#include "phisitem.h"
#include "phi.h"

PHISItem::PHISItem( QObject *parent )
    : QObject( parent ), PHIItem()
{
    //qDebug( "PHISItem::PHISItem()" );
}

PHISItem::~PHISItem()
{
    //qDebug( "PHISItem::~PHISItem()" );
}

void PHISItem::loadFromByteArray( quint16 wid, const QByteArray &id, const QByteArray &arr )
{
    PHIItem::loadFromByteArray( wid, id, arr );
    _d.load( _properties, _dynamicData );
    _variants=_d.variants();
}

bool PHISItem::isDragDrop() const
{
    //DragDropOptions opts=static_cast<DragDropOptions>(_variants.value( DDragDropOptions, 0 ).value<qint32>());
    if ( _attributes & ADraggable ) return true;
    if ( _attributes & ADroppable ) return true;
    return false;
}
