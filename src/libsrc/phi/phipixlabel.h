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
#ifndef PHIPIXLABEL_H
#define PHIPIXLABEL_H
#include <QLabel>
#include <QPixmap>
#include <QWidget>
#include <QPaintEvent>
#include "phi.h"

class PHIEXPORT PHIPixLabel : public QLabel
{
public:
    PHIPixLabel( QWidget *parent=0 );
    inline void setText( const QString &t ) { _text=t; }
    inline void setPixmap( const QPixmap &p ) { _pix=p; }
    inline QPixmap pixmap() const { return _pix; }
    inline QString text() const { return _text; }
    virtual void paintEvent( QPaintEvent *e );

private:
    QPixmap _pix;
    QString _text;
};

#endif // PHIPIXLABEL_H
