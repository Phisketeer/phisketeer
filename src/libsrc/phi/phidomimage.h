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
#ifndef PHIDOMIMAGE_H
#define PHIDOMIMAGE_H

#include <QObject>
#include <QImage>
#include <QString>
#include <QScriptEngine>
#include "phi.h"

class PHIEXPORT PHIDomImage : public QObject
{
    Q_OBJECT
    Q_PROPERTY( int width READ width )
    Q_PROPERTY( int height READ height )
    Q_PROPERTY( QString src READ src WRITE setSrc )
    Q_PROPERTY( QString name READ name )

public:
    explicit PHIDomImage() {}
    //static void setup( QScriptEngine *e );
    inline const QImage& image() const { return _image; }

public slots:
    inline int width() const { return _image.width(); }
    inline int height() const { return _image.height(); }
    inline QString src() const { return _src; }
    inline void setSrc( const QString &src ) { _image=QImage( src ); _src=src; }
    inline QString name() const { return _src; }
    //static QScriptValue _self;

private:
    QImage  _image;
    QString _src;
    //attribute boolean         isMap;
    //attribute DOMString       longDesc;
    //attribute DOMString       useMap;
    //attribute DOMString       align;
    //attribute DOMString       alt;
    //attribute DOMString       border;
    //attribute long            vspace;
    //attribute long            hspace;
};

//Q_DECLARE_METATYPE( PHIDomImage )
//Q_DECLARE_METATYPE( PHIDomImage* )

#endif // PHIDOMIMAGE_H
