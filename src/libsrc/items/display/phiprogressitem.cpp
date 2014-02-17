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
#include <QProgressBar>
#include "phiprogressitem.h"

void PHIProgressItem::ideInit()
{
    setProgress( 30 );
}

QSizeF PHIProgressItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    if ( which==Qt::PreferredSize ) return QSizeF( 160., 18. );
    if ( which==Qt::MaximumSize ) return QSizeF( 4000., 200. );
    return PHIBaseItem::sizeHint( which, constraint );
}

void PHIProgressItem::initWidget()
{
    QProgressBar *bar=new QProgressBar();
    bar->setRange( 0, 100 );
    setWidget( bar );
    setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum, QSizePolicy::Frame ) );
}

void PHIProgressItem::updateWidget()
{
    QProgressBar *bar=qobject_cast<QProgressBar*>(widget());
    if ( !bar ) return;
    bar->setValue( realProgress() );
}

bool PHIProgressItem::paint( QPainter *painter, const QRectF &exposed )
{
    Q_UNUSED( painter )
    Q_UNUSED( exposed )
    return false;
}

PHIWID PHIProgressItem::htmlHeaderExtension( const PHIRequest *req, QByteArray &out ) const
{
    Q_UNUSED( req )
    out+=BL( "<script type=\"text/javascript\" src=\"phi.phis?j=ui-progressbar\"></script>\n" );
    return static_cast<PHIWID>(Progress);
}

void PHIProgressItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    setAdjustedRect( rect().adjusted( 0, 0, -2, -2 ) );
    htmlInitItem( script );
    out+=indent+BL( "<div" );
    htmlBase( req, out, script );
    out+=BL( "\"></div>\n" );
}

QScriptValue PHIProgressItem::progress( const QScriptValue &v )
{
    if ( !v.isValid() ) return realProgress();
    setProgress( qBound( 0, v.toInt32(), 100 ) );
    return self();
}
