#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2014  Phisys AG, Switzerland
#    Copyright (C) 2012-2014  Phisketeer.org team
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
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

HEADERS = apacherequest.h \
    apachereqparent.h
SOURCES = mod_phi.cpp \
    apacherequest.cpp \
    apachereqparent.cpp

include( ../../phiconf.pri )
VERSION = 2.0.0
DEFINES += PHIVERSION=\\\"$$VERSION\\\"
TEMPLATE = lib
TARGET = mod_phi
QT = core gui network sql script widgets
INCLUDEPATH += ../libsrc/phi ../libsrc/phis

unix {
    INCLUDEPATH += /usr/include/apr-1.0 /usr/include/apr-1 /usr/include/openssl /usr/include/xmltok /usr/include/apache2
    DEFINES += _LARGEFILE64_SOURCE
    DESTDIR = ../../lib
    LIBS = -L../../lib -lphi -lphis -lapr-1 -laprutil-1
    CONFIG(debug,debug|release) {
        TARGET = mod_phi_debug
        LIBS = -L../../lib -lphi_debug -lphis_debug
    }
    mac {
        error( The Phi Apache module <mod_phi> is currently not supported for Mac OS X )
        # Any help is really appreciated
        # QMAKE_LFLAGS += -undefined dynamic_lookup
        # LIBS += -L/usr/lib -lapr-1 -laprutil-1
        # TARGET = mod_phi
    } else {
        QMAKE_LFLAGS +=-Wl,-rpath,\'\$$ORIGIN\'
        DEFINES += LINUX=2
    }
}
win32 {
    CONFIG(debug,debug|release){
        LIBS += -L../../bin phid2.lib phisd2.lib
        TARGET = mod_phid
    } else {
        LIBS += -L../../bin phi2.lib phis2.lib
    }
    DESTDIR = ../../bin
    INCLUDEPATH +="C:/Program Files (x86)/Apache Software Foundation/Apache2.2/include"
    LIBS += "C:/Program Files (x86)/Apache Software Foundation/Apache2.2/lib/libhttpd.lib"
    LIBS += "C:/Program Files (x86)/Apache Software Foundation/Apache2.2/lib/libapr-1.lib"
    LIBS += "C:/Program Files (x86)/Apache Software Foundation/Apache2.2/lib/libaprutil-1.lib"
    QMAKE_DISTCLEAN += mod_phi_resource.rc mod_phid_resource.rc
}
