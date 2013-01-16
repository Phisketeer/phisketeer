#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2012  Phisys AG, Switzerland
#    Copyright (C) 2012-2013  Phisketeer.org team
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

SOURCES = main.cpp \
    ampmainwindow.cpp \
    ampwebview.cpp \
    ampwebpage.cpp \
    amptools.cpp
HEADERS = ampmainwindow.h \
    ampwebview.h \
    ampwebpage.h \
    amptools.h
TRANSLATIONS =  amphibia_de.ts \
                amphibia_fr.ts
FORMS = mainwindow.ui

include( ../../../scripts/phiconf.pri )
VERSION = $$PHIRELEASE
TEMPLATE = app
TARGET = amphibia
DESTDIR = ../../../bin
QT = core gui widgets network webkitwidgets svg script printsupport
INCLUDEPATH += ../../libsrc/phi ../../libsrc/phia
DEFINES += PHIVERSION=\\\"$$VERSION\\\"

OTHER_FILES = Info.plist amphibia.rc

win32 {
    LIBS = -L../../../bin phi1.lib phia1.lib
    RC_FILE=amphibia.rc
    CONFIG(debug,debug|release){
        CONFIG += console
        LIBS = -L../../../bin phid1.lib phiad1.lib
        TARGET = amphibiad
    }
}
unix {
    LIBS = -L../../../lib -lphi -lphia
    CONFIG(debug,debug|release){
        LIBS = -L../../../lib -lphi_debug -lphia_debug
        TARGET = amphibia_debug
    }
    mac {
        LIBS = -L../../../lib -lphi -lphia
        QMAKE_INFO_PLIST = Info.plist
        ICON = amphibia.icns
        TARGET = Amphibia
    } else {
        QMAKE_LFLAGS +=-Wl,-rpath,\'\$$ORIGIN/../lib\',-rpath-link,$$[QT_INSTALL_LIBS]
    }
}
