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
#include <QPainter>
#include "phipixlabel.h"

PHIPixLabel::PHIPixLabel( QWidget *parent )
    : QLabel( parent )
{
    setWordWrap( false );
    setScaledContents( true );
    setAutoFillBackground( true );
    //QPalette p=palette();
    //p.setColor( QPalette::Window, Qt::transparent );
    //setPalette( p );
}

void PHIPixLabel::paintEvent( QPaintEvent *e )
{
    QPainter p( this );
    p.setRenderHint( QPainter::TextAntialiasing, true );
    //p.setRenderHint( QPainter::Antialiasing, true );
    if ( !_pix.isNull() ) p.drawPixmap( 0, 0, width(), height(), _pix );
    p.drawText( QRect( 3, 3, width()-6, height()-6 ), alignment(), _text );
    e->accept();
}
