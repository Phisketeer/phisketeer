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
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PHIA_H
#define PHIA_H

#include <QString>
#include <QPalette>
#include <QComboBox>
#include <QLineEdit>

#ifdef PHIALIB
#define PHIAEXPORT Q_DECL_EXPORT
#else
#define PHIAEXPORT Q_DECL_IMPORT
#endif

class PHIAItem;

#define PHITEXTINTERACTIONFLAGS Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard\
    | Qt::LinksAccessibleByKeyboard | Qt::LinksAccessibleByMouse

class PHIAEXPORT PHIA
{
    Q_DISABLE_COPY( PHIA )

public:
    enum Config { CacheDirectory=1, MaxCacheSize=2, PrintBackgrounds=3, AutoLoadImages=4, EnableJava=5,
        EnableJavascript=6, Geometry=7, DockWidgets=8, MaxHistoryItems=9, JSOpenWindow=10, ProxyHost=11,
        ProxyPort=12, DownloadDirectory=13, ConfigFile=14, LastUpdateCheck=15, EnablePlugins=16 };
    static QString libVersion();
    static QString browserName();
    static QString configName( Config );
    static QString viewerName();
    static qreal zoomFactor();
    static QString phiaDir();
    static void setContainerLineEdit( const PHIAItem*, int row, const QString& );
    static void setContainerComboBox( const PHIAItem*, int row, const QString& );
    static void setContainerComboBoxData( const PHIAItem*, int row, const QString& );
    static QComboBox* containerComboBox( const PHIAItem*, int row );
    static QLineEdit* containerLineEdit( const PHIAItem*, int row );

protected:
    PHIA();
    ~PHIA();
};

#endif // PHIA_H
