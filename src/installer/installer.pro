#    Copyright (C) 2010-2013  Marius B. Schumacher
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

HEADERS += phiwizard.h
SOURCES += main.cpp \
    phiwizard.cpp \
    unixinstall.cpp
VERSION = $$[PHIRELEASE]
PHIDOM = $$[PHIDOM]
PHIORG = $$[PHIORG]
TEMPLATE = app
TARGET = phiinstaller
CONFIG += qt thread $$[PHICONF]
QT = core gui

INCLUDEPATH = ./include
DEFINES += QT_NO_CAST_TO_ASCII
DEFINES += PHIVERSION=\\\"$$VERSION\\\" PHIDOM=\\\"$$PHIDOM\\\" PHIORG=\\\"$$PHIORG\\\"
OBJECTS_DIR = .tmp
MOC_DIR = .tmp
RCC_DIR = .tmp
RESOURCES += installer.qrc

win32 { 
    DESTDIR = ../../bin
    CONFIG(debug,debug|release) { 
        CONFIG += console
        TARGET = phiinstallerd
        DEFINES += PHIDEBUG
    } else {
        DEFINES += QT_NO_DEBUG_OUTPUT
        CONFIG += embed_manifest_exe
    }
}
unix { 
    DESTDIR = ../../bin
    CONFIG(debug,debug|release): TARGET = phiinstaller_debug
    else: DEFINES += QT_NO_DEBUG_OUTPUT
    QMAKE_LFLAGS_RPATH =
    QMAKE_LFLAGS +=-Wl,-rpath,\'\$$ORIGIN/../lib\',-rpath-link,$$[QT_INSTALL_LIBS]
    mac {
        error( "The installer application is not supported for Mac OS X." )
    }
}
FORMS += wizardstart.ui \
    wizardlicense.ui \
    wizardroot.ui \
    wizardcomponents.ui \
    wizarddata.ui \
    wizardphis.ui \
    wizardapache.ui \
    wizardinstall.ui \
    wizardcommit.ui
