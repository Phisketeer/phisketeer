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
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PHICTX2DWIDGET_H
#define PHICTX2DWIDGET_H

#include <QWidget>
#include <QImage>
#include "phicontext2d.h"

class QPaintEvent;
class QResizeEvent;

class PHIEXPORT PHICtx2DWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PHICtx2DWidget( QWidget *parent=0 );
    inline PHIContext2D* context() const { return _context; }
    
protected slots:
    inline void imageChanged( const QImage &img ) { _image=img; update(); }

protected:
    virtual void paintEvent( QPaintEvent *e );
    virtual void resizeEvent( QResizeEvent *e );

private:
    PHIContext2D *_context;
    QImage _image;
    
};

#endif // PHICTX2DWIDGET_H
