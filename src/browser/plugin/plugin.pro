#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2013  Phisys AG, Switzerland
#    Copyright (C) 2012-2013  Phisketeer.org team
#
#    This plug-in is free software: you can redistribute it and/or modify
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

SOURCES += phiplugin.cpp
HEADERS += phiplugin.h
include( ../../../scripts/phiconf.pri )
VERSION = 1.3.5
TARGET = phiplugin
QT = core gui widgets network svg script webkitwidgets printsupport
INCLUDEPATH = ../../libsrc/phi ../../libsrc/phia ./qtplugin241/src
DEFINES += PHIVERSION=\\\"$$VERSION\\\"
DEFINES -= QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_BYTEARRAY
TARGET = phiplugin
OTHER_FILES = phiplugin.r phiplugin.rc Info.plist

win32 { 
    LIBS = -L../../../bin phi1.lib phia1.lib urlmon.lib
    DESTDIR = ../../../bin
    CONFIG += axserver
    CONFIG(debug,debug|release) {
        LIBS = -L../../../bin phid1.lib phiad1.lib
        TARGET = phiplugind
    }
    LIBS += -L../../../3rdparty/win32 libeay32.lib ssleay32.lib
    RC_FILE = phiplugin.rc
}
unix { 
    DESTDIR = ../../../lib
    LIBS = -L../../../lib -lphi -lphia
    QMAKE_LFLAGS_RPATH =
    CONFIG(debug,debug|release) { 
        LIBS = -L../../../lib -lphi_debug -lphia_debug
        TARGET = phiplugin_debug
    }
    mac {
        error( "The PhiPlugin for Mac OS X is currently not supported" )
        # Any help is really appreciated!
        #QMAKE_LFLAGS += -undefined dynamic_lookup
        #QMAKE_INFO_PLIST = Info.plist
        #REZ_FILES += phiplugin.r
        #rsrc_files.files = phiplugin.rsrc
        #rsrc_files.path = Contents/Resources
        #QMAKE_BUNDLE_DATA += rsrc_files
        #TARGET = phiplugin
    } else {
        LIBS += -lssl -lcrypto
        QMAKE_LFLAGS +=-Wl,-rpath,\'\$$ORIGIN\',-rpath-link,$$[QT_INSTALL_LIBS]
    }
}
# The following is released under the BSD license by Nokia:
include( ./qtplugin241/src/qtbrowserplugin.pri )
