#    Copyright (C) 2010-2012  Marius B. Schumacher
#    Copyright (C) 2011-2012  Phisys AG, Switzerland
#    Copyright (C) 2012  Phisketeer.org team
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
FORMS = mainwindow.ui
VERSION = $$[PHIRELEASE]
TEMPLATE = app
TARGET = amphibia
DESTDIR = ../../../bin
CONFIG += qt $$[PHICONF]
QT = core gui network webkit svg script
INCLUDEPATH = ../../libsrc/phi ../../libsrc/phia
MOC_DIR = .tmp
OBJECTS_DIR = .tmp
RCC_DIR = .tmp
DEFINES += QT_NO_CAST_TO_ASCII PHIVERSION=\\\"$$VERSION\\\"
TRANSLATIONS =  amphibia_de.ts \
                amphibia_fr.ts

OTHER_FILES = Info.plist amphibia.rc

win32 {
    LIBS = -L../../../bin phi1.lib phia1.lib
    RC_FILE=amphibia.rc
    CONFIG(debug,debug|release){
        CONFIG += console
        LIBS = -L../../../bin phid1.lib phiad1.lib
        TARGET = amphibiad
        DEFINES += PHIDEBUG
    } else {
        DEFINES += QT_NO_DEBUG_OUTPUT
        CONFIG += embed_manifest_exe
    }
}

unix {
    LIBS = -L../../../lib -lphi -lphia
    QMAKE_LFLAGS_RPATH =
    CONFIG(debug,debug|release){
        LIBS = -L../../../lib -lphi_debug -lphia_debug
        TARGET = amphibia_debug
        DEFINES += PHIDEBUG
    } else {
        DEFINES += QT_NO_DEBUG_OUTPUT
    }
    mac {
        contains( CONFIG, appstore ){
            DEFINES += PHIAPPSTORE
            QMAKE_CXXFLAGS += -gdwarf-2
            QMAKE_CFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
            QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO
            QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
        }
        LIBS = -L../../../lib -lphi -lphia
        QMAKE_INFO_PLIST = Info.plist
        QMAKE_MAC_SDK=$$[PHIMACSDK]
        QMAKE_MACOSX_DEPLOYMENT_TARGET = $$[PHIMACDEPLOY]
        ICON = amphibia.icns
        TARGET = Amphibia
    } else {
        QMAKE_LFLAGS +=-Wl,-rpath,\'\$$ORIGIN/../lib\',-rpath-link,$$[QT_INSTALL_LIBS]
    }
}
