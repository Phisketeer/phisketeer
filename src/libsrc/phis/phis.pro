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
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

HEADERS += phidatasources.h \
    phiresponserec.h \
    phiprocessor.h \
    phispage.h \
    phis.h \
    phidataparser.h \
    phiparent.h \
    phispagecache.h \
    phisscriptobjects.h \
    phisitem.h \
    phisitemcache.h \
    phisitemdata.h \
    phihtmlgenerator.h \
    phigenerator.h \
    phinetrequest.h \
    phisession.h \
    phisimagecache.h \
    phisrequest.h \
    phishtmlbase.h \
    phishtmlgeneric.h \
    phishtmlgecko19.h \
    phishtmlwebkit533.h \
    phishtmltrident4.h \
    phishtmlpresto22.h \
    phishtmlpresto23.h \
    phishtmlwebkit532.h \
    phishtml5base.h \
    phishtmlgecko20.h \
    phishtmlwebkit534.h \
    phishtmltrident5.h \
    phishtmltrident6.h \
    phishtmlgecko100.h
SOURCES += \
    phidatasources.cpp \
    phiresponserec.cpp \
    phiprocessor.cpp \
    phis.cpp \
    phispage.cpp \
    phidataparser.cpp \
    phiparent.cpp \
    phispagecache.cpp \
    phisscriptobjects.cpp \
    phisitem.cpp \
    phisitemcache.cpp \
    phisitemdata.cpp \
    phihtmlgenerator.cpp \
    phigenerator.cpp \
    phinetrequest.cpp \
    phisession.cpp \
    phisimagecache.cpp \
    phisrequest.cpp \
    phishtmlbase.cpp \
    phishtmlgeneric.cpp \
    phishtmlimgcreator.cpp \
    phishtmlgecko19.cpp \
    phishtmlwebkit533.cpp \
    phishtmltrident4.cpp \
    phishtmlpresto22.cpp \
    phishtmlpresto23.cpp \
    phishtmlwebkit532.cpp \
    phishtml5base.cpp \
    phishtmlgecko20.cpp \
    phishtmlwebkit534.cpp \
    phishtmltrident5.cpp \
    phishtmltrident6.cpp \
    phishtmlgecko100.cpp

include( ../../../scripts/phiconf.pri )
VERSION = $$PHIRELEASE
TEMPLATE = lib
TARGET = phis
QT = gui core network sql svg script
INCLUDEPATH += ../phi
DEFINES += PHISLIB PHIVERSION=\\\"$$VERSION\\\"
RESOURCES += phis.qrc
OTHER_FILES += js/phibase.js

win32 { 
    LIBS = -L../../../bin phi1.lib
    DESTDIR = ../../../bin
    CONFIG(debug,debug|release) { 
        LIBS = -L../../../bin phid1.lib
        TARGET = phisd
    }
    LIBS += -L../../../3rdparty/win32 libeay32.lib ssleay32.lib
    QMAKE_CLEAN += phis_resource.rc phisd_resource.rc
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
