/*
#    Copyright (C) 2008  Trolltech AS
#    Copyright (C) 2012  Digia Plc and/or its subsidiary(-ies).
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
#
#    Note: some parts are based on code provided by the Qt source.
*/
#ifndef PHIPALETTE_H
#define PHIPALETTE_H

#include <QObject>

class PHIPalette : public QObject
{
    Q_OBJECT

public:
    enum ColorRole { WindowText=0, Foreground=WindowText, Button=1, Light=2, MidLight=3,
        Dark=4, Mid=5, Text=6, BrightText=7, ButtonText=8, Base=9, Window=10,
        Background=Window, Shadow=11, Highlight=12, HoverBackground=Highlight,
        HighlightText=13, HoverText=HighlightText, Link=14, LinkVisited=15,
        AlternateBase=16, NoRole=17, Custom=18, User1=19, User2=20, User3=21,
        User4=22, User5=23, User6=24 };
    explicit PHIPalette( QObject *parent );
    
signals:
    
public slots:
    
};

#endif // PHIPALETTE_H
