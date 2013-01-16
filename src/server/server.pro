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

HEADERS += phimanager.h \
    phiconfig.h \
    philistener.h \
    phiconnthread.h \
    phihttp.h \
    phihttpheader.h \
    phiobject.h \
    philogwriter.h \
    phiserverrequest.h \
    phiserverrec.h
SOURCES += main.cpp \
    phimanager.cpp \
    phiconfig.cpp \
    philistener.cpp \
    phiconnthread.cpp \
    phihttp.cpp \
    phihttpheader.cpp \
    phiobject.cpp \
    philogwriter.cpp \
    phiserverrequest.cpp \
    phiserverrec.cpp
mac {
    HEADERS +=macservice.h
    SOURCES +=macservice.cpp
} else {
    HEADERS +=phiservice.h
    SOURCES +=phiservice.cpp
}

include( ../../scripts/phiconf.pri )
VERSION = $$PHIRELEASE
TEMPLATE = app
TARGET = phis
CONFIG += console
QT = core gui widgets network sql script
INCLUDEPATH += ../libsrc/phi ../libsrc/phis
DEFINES += PHIVERSION=\\\"$$VERSION\\\"

win32 { 
    DESTDIR = ../../bin
    # LIBS = -L../../bin -lphi1 -lphis1 -l"C:\Program Files\Microsoft SDKs\Windows\V6.0A\lib\advapi32"
    LIBS = -L../../bin phi1.lib phis1.lib
    CONFIG(debug,debug|release) { 
        LIBS = -L../../bin phid1.lib phisd1.lib
        TARGET = phisd
    }
    QMAKE_CLEAN += phis_resource.rc phisd_resource.rc
    QMAKE_DISTCLEAN += phis_resource.rc phisd_resource.rc
    include( qtservice-2.6/src/qtservice.pri )
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
        CONFIG -= app_bundle
        ICON = phis.icns
        TARGET = phis
    } else {
        QMAKE_LFLAGS +=-Wl,-rpath,\'\$$ORIGIN/../lib\',-rpath-link,$$[QT_INSTALL_LIBS]
        include( qtservice-2.6/src/qtservice.pri )
    }
}
