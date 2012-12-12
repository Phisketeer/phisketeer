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
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "phiapage.h"

PHIAPage::PHIAPage( QObject *parent )
    : PHIBasePage( parent )
{
    qDebug( "PHIAPage::PHIAPage()" );
}

PHIAPage::~PHIAPage()
{
    // clearItems(); // done by ~PHIBasePage()
    qDebug( "PHIAPage::~PHIAPage()" );
}

void PHIAPage::clearItems()
{
    PHIBaseItem *it;
    foreach ( it, _items ) delete it;
    _items.clear();
}

QList <PHIBaseItem*> PHIAPage::childrenOf( const QString &parent ) const
{
    QList <PHIBaseItem*> list;
    PHIBaseItem *it;
    foreach ( it, _items ) if ( it->parentName()==parent ) list.append( it );
    return list;
}

QIcon PHIAPage::icon() const
{
    if ( _image.isNull() ) return QIcon();
    return QIcon( QPixmap::fromImage( _image ) );
}

void PHIAPage::setTitle( const QString &t )
{
    if ( t==title() ) return;
    PHIBasePage::setTitle( t );
    emit titleChanged();
}

QDataStream& operator<<( QDataStream &out, const PHIAPage *p )
{
    out << dynamic_cast<const PHIBasePage*>(p);
    return out;
}

QDataStream& operator>>( QDataStream &in, PHIAPage *p )
{
    in >> dynamic_cast<PHIBasePage*>(p);
    return in;
}
