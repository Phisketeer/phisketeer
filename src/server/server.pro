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
HEADERS += phisservice.h
SOURCES += main.cpp \
    phisservice.cpp

include( ../../phiconf.pri )
VERSION = $$PHIRELEASE
TEMPLATE = app
TARGET = phis
CONFIG += console
QT = core gui widgets network sql script
INCLUDEPATH += ../libsrc/phi ../libsrc/phis
DEFINES += PHIVERSION=\\\"$$VERSION\\\"
include( qtservice-2.6/src/qtservice.pri )

win32 { 
    DESTDIR = ../../bin
    LIBS = -L../../bin phi2.lib phis2.lib
    CONFIG(debug,debug|release) { 
        LIBS = -L../../bin phid2.lib phisd2.lib
        TARGET = Phisd
    } else: TARGET = Phis
    QMAKE_LFLAGS += /MANIFESTUAC:level=\'requireAdministrator\'
    QMAKE_CLEAN += phis_resource.rc phisd_resource.rc
    QMAKE_DISTCLEAN += phis_resource.rc phisd_resource.rc
}
unix {
    LIBS = -L../../lib -lphi -lphis
    DESTDIR = ../../bin
    CONFIG(debug,debug|release) { 
        LIBS = -L../../lib -lphi_debug -lphis_debug
        TARGET = phis_debug
    }
    mac {
        LIBS = -L../../lib -lphi -lphis
        QMAKE_INFO_PLIST = Info.plist
        OTHER_FILES += Info.plist
        CONFIG -= app_bundle
        ICON = phis.icns
        TARGET = phis
    } else {
        QMAKE_LFLAGS +=-Wl,-rpath,\'\$$ORIGIN/../lib\',-rpath-link,$$[QT_INSTALL_LIBS]
    }
}
