#    Copyright (C) 2010-2012  Marius B. Schumacher
#    Copyright (C) 2011-2012  Phisys AG, Switzerland
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

HEADERS += phi.h \
    phisysinfo.h \
    phierr.h \
    phierror.h \
    phiitem.h \
    phipage.h \
    phibaseitem.h \
    phibasepage.h \
    phieffect.h \
    phieffects.h \
    phimemrotate.h \
    philicense.h \
    phiapplication.h \
    phipixlabel.h \
    phiupdateinfo.h
SOURCES += phierror.cpp \
    phierrordesc.cpp \
    phiglobal.cpp \
    phiitem.cpp \
    phipage.cpp \
    phibaseitem.cpp \
    phibasepage.cpp \
    phimimetypes.cpp \
    phieffect.cpp \
    phieffects.cpp \
    phimemrotate.cpp \
    philicense.cpp \
    phiapplication.cpp \
    phipixlabel.cpp \
    phiupdateinfo.cpp
VERSION = 1.1.4
PHIDOM = $$[PHIDOM]
PHIORG = $$[PHIORG]
isEmpty( PHIDOM ):error( PHIDOM is not specified )
isEmpty( PHIORG ):error( PHIORG is not specified )
TEMPLATE = lib
TARGET = phi
CONFIG += qt thread $$[PHICONF]
QT = core gui network script
INCLUDEPATH += .
DEFINES += QT_NO_CAST_TO_ASCII PHILIB
DEFINES += PHIVERSION=\\\"$$VERSION\\\" PHIDOM=\\\"$$PHIDOM\\\" PHIORG=\\\"$$PHIORG\\\"

#exists( images/own ): RESOURCES = phi.qrc
OBJECTS_DIR = .tmp
MOC_DIR = .tmp
RCC_DIR = .tmp
unix { 
    DESTDIR = ../../../lib
    QMAKE_LFLAGS_RPATH =
    SOURCES += phisysinfo_unix.cpp
    CONFIG(debug,debug|release) { 
        TARGET = phi_debug
        DEFINES += PHIDEBUG
    } else: DEFINES += QT_NO_DEBUG_OUTPUT
    mac {
        contains( CONFIG, appstore ){
            DEFINES += PHIAPPSTORE
            QMAKE_CFLAGS += -gdwarf-2
            QMAKE_CXXFLAGS += -gdwarf-2
            QMAKE_CFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
            QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO
            QMAKE_OBJECTIVE_CFLAGS_RELEASE = $$QMAKE_OBJECTIVE_CFLAGS_RELEASE_WITH_DEBUGINFO
            QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
            OBJECTIVE_SOURCES += macfilebookmark.mm
            OBJECTIVE_HEADERS += macfilebookmark.h
            LIBS = -framework AppKit
        }
        QMAKE_MAC_SDK=$$[PHIMACSDK]
        QMAKE_MACOSX_DEPLOYMENT_TARGET = $$[PHIMACDEPLOY]
        QMAKE_INFO_PLIST = Info.plist
        DEFINES += PHIMACFONT
        TARGET = phi
        OTHER_FILES = Info.plist
    } else {
        QMAKE_LFLAGS +=-Wl,-rpath,\'\$$ORIGIN\'
    }
}
win32 {
    DESTDIR = ../../../bin
    SOURCES += phisysinfo_win32.cpp
    CONFIG(debug,debug|release) { 
        TARGET = phid
        DEFINES += PHIDEBUG
    } else: DEFINES += QT_NO_DEBUG_OUTPUT
    QMAKE_CLEAN += phi_resource.rc phid_resource.rc
    QMAKE_DISTCLEAN += phi_resource.rc phid_resource.rc
}
