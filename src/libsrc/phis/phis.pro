#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2014  Phisys AG, Switzerland
#    Copyright (C) 2012-2014  Phisketeer.org team
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

HEADERS += \
    phis.h \
    phisparent.h \
    phismodule.h \
    phismodulefactory.h \
    phishttp.h \
    phislistener.h \
    phispagecache.h \
    phisconnthread.h \
    phislogwriter.h \
    phisserverrec.h \
    phisserverrequest.h \
    phisscriptobjects.h \
    phisprocessor.h \
    phisserverconf.h \
    phislogviewer.h
SOURCES += \
    phis.cpp \
    phisparent.cpp \
    phismodule.cpp \
    phismodulefactory.cpp \
    phishttp.cpp \
    phislistener.cpp \
    phisconnthread.cpp \
    phislogwriter.cpp \
    phispagecache.cpp \
    phisserverrequest.cpp \
    phisscriptobjects.cpp \
    phisprocessor.cpp \
    phisserverconf.cpp \
    phislogviewer.cpp
include( ../../../phiconf.pri )
VERSION = $$PHIRELEASE
TEMPLATE = lib
TARGET = phis
QT = gui core network sql svg script widgets
INCLUDEPATH += ../phi
DEFINES += PHISLIB PHISVERSION=\\\"$$VERSION\\\"
RESOURCES += phis.qrc serverconf.qrc
OTHER_FILES += js/phibase.js js/excanvas.js
FORMS += serverconf.ui
TRANSLATIONS = phis_de.ts phis_fr.ts phis_cs.ts

win32 { 
    LIBS = -L../../../bin phi2.lib
    DESTDIR = ../../../bin
    CONFIG(debug,debug|release) { 
        LIBS = -L../../../bin phid2.lib
        TARGET = phisd
    }
    QMAKE_DISTCLEAN += phis_resource.rc phisd_resource.rc
}
unix { 
    DESTDIR = ../../../lib
    LIBS = -L../../../lib -lphi
    CONFIG(debug,debug|release) {
        LIBS = -L../../../lib -lphi_debug
        TARGET = phis_debug
    }
    mac {
        LIBS = -L../../../lib -lphi
        TARGET = phis
    } else {
        LIBS += -lssl -lcrypto
        QMAKE_LFLAGS +=-Wl,-rpath,\'\$$ORIGIN\'
    }
}
