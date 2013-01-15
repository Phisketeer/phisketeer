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

HEADERS += sconfig.h
SOURCES += sconfig.cpp \
    main.cpp
VERSION = 1.2.3
TEMPLATE = app
TARGET = phisconf
CONFIG += qt thread $$[PHICONF]
QT = core gui widgets network
INCLUDEPATH = ../libsrc/phi ../libsrc/phis
DEFINES += QT_NO_CAST_TO_ASCII PHIVERSION=\\\"$$VERSION\\\"
FORMS += sconfigdlg.ui
TRANSLATIONS =  phisconf_de.ts \
                phisconf_fr.ts
OBJECTS_DIR = .tmp
MOC_DIR = .tmp
RCC_DIR = .tmp
RESOURCES += sconfig.qrc

win32 { 
    DESTDIR = ../../bin
    LIBS = -L../../bin phi1.lib phis1.lib
    CONFIG(debug,debug|release) { 
        LIBS = -L../../bin phid1.lib phisd1.lib
        TARGET = phisconfd
        CONFIG += console
    } else {
        DEFINES += QT_NO_DEBUG_OUTPUT
        CONFIG += embed_manifest_exe
    }
    RC_FILE += sconfig.rc
    QMAKE_LFLAGS += /MANIFESTUAC:level=\'requireAdministrator\'
    OTHER_FILES += sconfig.rc
}
unix { 
    LIBS = -L../../lib -lphi -lphis
    QMAKE_LFLAGS_RPATH =
    DESTDIR = ../../bin
    CONFIG(debug,debug|release) { 
        LIBS = -L../../lib -lphi_debug -lphis_debug
        TARGET = phisconf_debug
    } else: DEFINES += QT_NO_DEBUG_OUTPUT
    mac {
        LIBS = -L../../lib -lphi -lphis
        contains( CONFIG, appstore ){
            DEFINES += PHIAPPSTORE
            QMAKE_CXXFLAGS += -gdwarf-2
            QMAKE_CFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
            QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO
            QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
        }
        QMAKE_MAC_SDK=$$[PHIMACSDK]
        QMAKE_INFO_PLIST = Info.plist
        QMAKE_MACOSX_DEPLOYMENT_TARGET = $$[PHIMACDEPLOY]
        CONFIG -= app_bundle
        ICON = sconfig.icns
        TARGET = phisconf
    } else {
        QMAKE_LFLAGS +=-Wl,-rpath,\'\$$ORIGIN/../lib\',-rpath-link,$$[QT_INSTALL_LIBS]
    }
}
