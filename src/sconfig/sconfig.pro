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

HEADERS +=
SOURCES += main.cpp

include( ../../phiconf.pri )
VERSION = 2.0.0
TEMPLATE = app
TARGET = phisconf
DESTDIR = ../../bin
QT = core gui widgets network
INCLUDEPATH += ../libsrc/phi ../libsrc/phis
DEFINES += PHIVERSION=\\\"$$VERSION\\\"

win32 {
    LIBS = -L../../bin phi2.lib phis2.lib
    CONFIG(debug,debug|release) {
        CONFIG += console
        LIBS = -L../../bin phid2.lib phisd2.lib
        TARGET = phisconfd
    }
    RC_FILE = sconfig.rc
    QMAKE_LFLAGS += /MANIFESTUAC:level=\'requireAdministrator\'
    OTHER_FILES += sconfig.rc
}
unix {
    LIBS = -L../../lib -lphi -lphis
    CONFIG(debug,debug|release) {
        LIBS = -L../../lib -lphi_debug -lphis_debug
        TARGET = phisconf_debug
    }
    mac {
        LIBS = -L../../lib -lphi -lphis
        QMAKE_INFO_PLIST = Info.plist
        OTHER_FILES += Info.plist
        CONFIG -= app_bundle
        ICON = sconfig.icns
        TARGET = phisconf
    } else {
        QMAKE_LFLAGS +=-Wl,-rpath,\'\$$ORIGIN/../lib\',-rpath-link,$$[QT_INSTALL_LIBS]
    }
}
