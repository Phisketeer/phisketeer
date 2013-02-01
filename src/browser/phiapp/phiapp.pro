#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2013  Phisys AG, Switzerland
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

SOURCES = main.cpp
HEADERS = 
include( ../../../scripts/phiconf.pri )
TEMPLATE = app
TARGET = phiapp
DESTDIR = ../../../bin
QT = core gui widgets network svg script webkitwidgets printsupport
VERSION = $$PHIRELEASE
INCLUDEPATH = ../../libsrc/phi ../../libsrc/phia
DEFINES += PHIVERSION=\\\"$$VERSION\\\"

win32 {
    LIBS = -L../../../bin phi1.lib phia1.lib
    CONFIG(debug,debug|release){
        CONFIG += console
        LIBS = -L../../../bin phid1.lib phiad1.lib
        TARGET = amphibiad
    }
    RC_FILE=phiapp.rc
    OTHER_FILES +=phiapp.rc
}

unix {
    LIBS = -L../../../lib -lphi -lphia
    QMAKE_LFLAGS_RPATH =
    CONFIG(debug,debug|release) {
        LIBS = -L../../../lib -lphi_debug -lphia_debug
        TARGET = phiapp_debug
    }
    mac {
        LIBS = -L../../../lib -lphi -lphia
        OTHER_FILES += Info.plist
        CONFIG -= app_bundle
        ICON = phiapp.icns
        TARGET = phiapp
    } else {
        QMAKE_LFLAGS +=-Wl,-rpath,\'\$$ORIGIN/../lib\',-rpath-link,$$[QT_INSTALL_LIBS]
    }
}
