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
#include <QGraphicsSceneDragDropEvent>
#include "phishapeitem.h"

PHIShapeItem::PHIShapeItem( Type type, PHIBasePage *page )
    : PHIBaseItem( type, page ), _pen( Qt::black ), _brush( Qt::NoBrush )
{
}

void PHIShapeItem::setLine( quint8 l )
{
    Qt::PenStyle s;
    if ( l > static_cast<quint8>(Qt::DashDotDotLine) ) s=Qt::NoPen;
    else s=static_cast<Qt::PenStyle>(l);
    _variants.insert( DLineStyle, static_cast<quint8>(s) );
    if ( s==Qt::NoPen ) _variants.remove( DLineStyle );
    _pen.setStyle( s );
}

bool PHIShapeItem::ideDragEnterEvent( QGraphicsSceneDragDropEvent *e )
{
    e->setProposedAction( Qt::CopyAction );
    e->accept();
    return true;
}
