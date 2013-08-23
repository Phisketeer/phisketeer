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

#include <QHash>
#include <QColor>
#include <QPalette>
#include "phi.h"

class QDataStream;

class PHIPalette
{
public:
    enum ColorRole {
        WindowText=0, Button=1, Light=2, MidLight=3,
        Dark=4, Mid=5, Text=6, BrightText=7, ButtonText=8, Base=9, Window=10,
        Shadow=11, Highlight=12, HighlightText=13, Link=14, LinkVisited=15,
        AlternateBase=16, NoRole=17, Custom=18, User1_20=19, User1_40=20,
        User1_60=21, User1_80=22, User1_100=23, User1_120=24, User1_140=25,
        User1_160=26, User1_180=27, User2_20=28, User2_40=29,
        User2_60=30, User2_80=31, User2_100=32, User2_120=33, User2_140=34,
        User2_160=35, User2_180=36, User3_20=37, User3_40=38,
        User3_60=39, User3_80=40, User3_100=41, User3_120=42, User3_140=43,
        User3_160=44, User3_180=45, User4_20=46, User4_40=47,
        User4_60=48, User4_80=49, User4_100=50, User4_120=51, User4_140=52,
        User4_160=53, User4_180=54, User5_20=55, User5_40=56,
        User5_60=57, User5_80=58, User5_100=59, User5_120=60, User5_140=61,
        User5_160=62, User5_180=63, User6_20=64, User6_40=65,
        User6_60=66, User6_80=67, User6_100=68, User6_120=69, User6_140=70,
        User6_160=71, User6_180=72, Black=73, White=74, ColorRoleMax=75 };
    enum ItemRole { Foreground=0, Background=1, Hover, HoverBackground,
        Url, WidgetText, WidgetBase, User1, User2, User3, User4, User5, User6,
        ItemRoleMax };

    explicit PHIPalette();
    PHIPalette( const PHIPalette &p );
    PHIPalette& operator=( const PHIPalette &p );
    bool operator==( const PHIPalette &p );
    inline bool operator!=( const PHIPalette &p ) { return !operator==(p); }

    QColor color( ColorRole role ) const;
    void setColor( ColorRole role, const QColor &col );
    void setPalette( const QPalette &pal );
    inline const QPalette& palette() const { return _pal; }

protected:
    void createPercentColors( ColorRole, const QColor &col );

private:
    QPalette _pal;
    QHash <quint8, QColor> _userColors;
};

PHIEXPORT QDataStream& operator<<( QDataStream&, const PHIPalette& );
PHIEXPORT QDataStream& operator>>( QDataStream&, PHIPalette& );

#endif // PHIPALETTE_H
