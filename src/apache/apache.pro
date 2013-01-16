#    Copyright (C) 2010-2012  Marius B. Schumacher
#    Copyright (C) 2011-2012  Phisys AG, Switzerland
#    Copyright (C) 2012  Phisketeer.org team
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

HEADERS = apacherequest.h
SOURCES = mod_phi.cpp \
    apacherequest.cpp

VERSION = 1.1.8
PHIDOM = $$[PHIDOM]
PHIORG = $$[PHIORG]

DEFINES += QT_NO_CAST_TO_ASCII PHIVERSION=\\\"$$VERSION\\\"
TEMPLATE = lib
TARGET = mod_phi
CONFIG += qt thread largefile $$[PHICONF]
#CONFIG += create_prl link_prl

QT = core gui network sql script widgets
INCLUDEPATH += ../libsrc/phi ../libsrc/phis
OBJECTS_DIR = .tmp
MOC_DIR = .tmp
RCC_DIR = .tmp

unix {
    INCLUDEPATH += /usr/include/apr-1.0 /usr/include/apr-1 /usr/include/openssl /usr/include/xmltok /usr/include/apache2
    DEFINES += _LARGEFILE64_SOURCE
    DESTDIR = ../../lib
    LIBS = -L../../lib -lphi -lphis
    QMAKE_LFLAGS_RPATH =

    CONFIG(debug,debug|release) {
        TARGET = mod_phi_debug
        LIBS = -L../../lib -lphi_debug -lphis_debug
        DEFINES += PHIDEBUG
    } else: DEFINES += QT_NO_DEBUG_OUTPUT
    mac {
        error( The Phi Apache module mod_phi is currently not supported for Mac OS X )
        # Any help is really appreciated
        # contains( CONFIG, appstore ){
        #    DEFINES += PHIAPPSTORE
        #    QMAKE_CXXFLAGS += -gdwarf-2
        #    QMAKE_CFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
        #    QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO
        #    QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
        # }
        # QMAKE_MAC_SDK=$$[PHIMACSDK]
        # QMAKE_LFLAGS += -undefined dynamic_lookup
        # QMAKE_MACOSX_DEPLOYMENT_TARGET = $$[PHIMACDEPLOY]
        # LIBS += -L/usr/lib -lapr-1 -laprutil-1
        # TARGET = mod_phi
    } else {
        QMAKE_LFLAGS +=-Wl,-rpath,\'\$$ORIGIN\'
        DEFINES += LINUX=2
    }
}

win32 {
    CONFIG(debug,debug|release){
        LIBS += -L../../bin phid1.lib phisd1.lib
        TARGET = mod_phid
        DEFINES += PHIDEBUG
    } else {
        LIBS += -L../../bin phi1.lib phis1.lib
        DEFINES += QT_NO_DEBUG_OUTPUT
    }
    DESTDIR = ../../bin
    INCLUDEPATH +="C:/Program Files (x86)/Apache Software Foundation/Apache2.2/include"
    LIBS += "C:/Program Files (x86)/Apache Software Foundation/Apache2.2/lib/libhttpd.lib"
    LIBS += "C:/Program Files (x86)/Apache Software Foundation/Apache2.2/lib/libapr-1.lib"
    LIBS += "C:/Program Files (x86)/Apache Software Foundation/Apache2.2/lib/libaprutil-1.lib"
    QMAKE_CLEAN += mod_phi_resource.rc mod_phid_resource.rc
    QMAKE_DISTCLEAN += mod_phi_resource.rc mod_phid_resource.rc
}
