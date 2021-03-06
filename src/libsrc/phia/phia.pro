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
    phia.h \
    phiagraphicsitem.h \
    phiagraphicsscene.h \
    phiaabstractwebview.h \
    phiascriptobjects.h \
    phiamainwindow.h \
    phiawebview.h \
    phiaauthdlg.h \
    phiaappwindow.h
SOURCES += \
    phia.cpp \
    phiagraphicsitem.cpp \
    phiagraphicsscene.cpp \
    phiaabstractwebview.cpp \
    phiascriptobjects.cpp \
    phiamainwindow.cpp \
    phiawebview.cpp \
    phiaauthdlg.cpp \
    phiaappwindow.cpp

include( ../../../phiconf.pri )
VERSION = $$PHIRELEASE
TRANSLATIONS = phia_de.ts phia_fr.ts phia_cs.ts
TEMPLATE = lib
TARGET = phia
QT = core gui network svg script widgets webkit webkitwidgets printsupport
INCLUDEPATH += ../phi
DEFINES += PHIALIB PHIAVERSION=\\\"$$VERSION\\\"
RESOURCES += phia.qrc

win32 {
    LIBS = -L../../../bin phi2.lib
    DESTDIR = ../../../bin
    CONFIG(debug,debug|release) { 
        LIBS = -L../../../bin phid2.lib
        TARGET = phiad
    }
    QMAKE_DISTCLEAN += phia_resource.rc phiad_resource.rc
}
unix {
    DESTDIR = ../../../lib
    LIBS = -L../../../lib -lphi
    CONFIG(debug,debug|release) {
        LIBS = -L../../../lib -lphi_debug
        TARGET = phia_debug
    }
    mac {
        LIBS = -L../../../lib -lphi
        TARGET = phia
    } else {
        LIBS += -lssl -lcrypto
        QMAKE_LFLAGS +=-Wl,-rpath,\'\$$ORIGIN\'
    }
}

FORMS = sourcedialog.ui \
    mainwindow.ui \
    settingsdlg.ui \
    addressdlg.ui \
    authdlg.ui \
    creditcarddlg.ui \
    contactdlg.ui \
    cookiedlg.ui \
    downloaddlg.ui
