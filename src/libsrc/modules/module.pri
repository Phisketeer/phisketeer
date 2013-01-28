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

QT += script core gui network widgets sql
TEMPLATE = lib
CONFIG += qt plugin thread largefile
INCLUDEPATH += ../../../libsrc/phi ../../../libsrc/phis
DEFINES += QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_ASCII QT_NO_CAST_FROM_BYTEARRAY
OBJECTS_DIR = .tmp
MOC_DIR = .tmp
RCC_DIR = .tmp
win32 {
    DESTDIR = ../../../../bin/plugins/modules
    LIBS = -L../../../../bin phi1.lib phis1.lib
    CONFIG(debug,debug|release) {
        LIBS = -L../../../../bin phid1.lib phisd1.lib
    } else {
        DEFINES += QT_NO_DEBUG_OUTPUT
    }
}
unix {
    DESTDIR = ../../../../plugins/modules
    LIBS = -L../../../../lib -lphi -lphis
    CONFIG(debug,debug|release){
        LIBS = -L../../../../lib -lphi_debug -lphis_debug
    } else {
        DEFINES += QT_NO_DEBUG_OUTPUT
    }
    mac {
        LIBS = -L../../../../lib -lphi -lphis
    } else {
        QMAKE_LFLAGS +=-Wl,-rpath,\'\$$ORIGIN/../../lib\',-rpath-link,$$[QT_INSTALL_LIBS]
    }
}
