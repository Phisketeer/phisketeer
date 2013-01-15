#    Copyright (C) 2010-2012  Marius B. Schumacher
#    Copyright (C) 2011-2012  Phisys AG, Switzerland
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

HEADERS += phia.h \
    phiatools.h \
    phianetmanager.h \
    phiawebview.h \
    phiascene.h \
    phiaabstractwebview.h \
    phiamainwindow.h \
    phiapage.h \
    phiaitem.h \
    gitems.h \
    phiascriptobjects.h \
    phiahistory.h \
    phiaformprocessor.h \
    gwidgetitems.h \
    glayoutitems.h \
    gwidgetitem.h \
    phiaconfig.h \
    phiasettingsdlg.h \
    phiaaddressdlg.h \
    phiaauthdlg.h \
    phiadownloaddlg.h \
    phiacomboboxhack.h \
    gwebitems.h \
    phiascriptevent.h \
    phiascriptitem.h \
    phiaanimation.h \
    phiajavascriptlog.h
SOURCES += phia.cpp \
    phianetmanager.cpp \
    phiatools.cpp \
    phiawebview.cpp \
    phiascene.cpp \
    phiaabstractwebview.cpp \
    phiamainwindow.cpp \
    phiapage.cpp \
    phiaitem.cpp \
    gitems.cpp \
    phiascriptobjects.cpp \
    phiahistory.cpp \
    phiaformprocessor.cpp \
    gwidgetitems.cpp \
    glayoutitems.cpp \
    phiaitemhandler.cpp \
    gwidgetitem.cpp \
    phiaconfig.cpp \
    phiasettingsdlg.cpp \
    phiaaddressdlg.cpp \
    phiaauthdlg.cpp \
    phiadownloaddlg.cpp \
    phiacomboboxhack.cpp \
    gwebitems.cpp \
    phiascriptevent.cpp \
    phiascriptitem.cpp \
    phiaanimation.cpp \
    phiajavascriptlog.cpp
VERSION = $$[PHIRELEASE]
TRANSLATIONS =  phia_de.ts \
                phia_fr.ts

TEMPLATE = lib
TARGET = phia
CONFIG += qt thread $$[PHICONF]
QT = core gui network svg script webkitwidgets widgets printsupport
INCLUDEPATH = ../phi
OBJECTS_DIR = .tmp
MOC_DIR = .tmp
RCC_DIR = .tmp
DEFINES += QT_NO_CAST_TO_ASCII PHIALIB
DEFINES += PHIVERSION=\\\"$$VERSION\\\"
OTHER_FILES = Info.plist \
    vars.txt
RESOURCES += phia.qrc

win32 {
    LIBS = -L../../../bin phi1.lib
    DESTDIR = ../../../bin
    CONFIG(debug,debug|release) { 
        CONFIG += console
        LIBS = -L../../../bin phid1.lib
        TARGET = phiad
        DEFINES += PHIDEBUG
    } else: DEFINES += QT_NO_DEBUG_OUTPUT
    LIBS += -L../../../3rdparty/win32 libeay32.lib ssleay32.lib
    QMAKE_CLEAN += phia_resource.rc phiad_resource.rc
    QMAKE_DISTCLEAN += phia_resource.rc phiad_resource.rc
}
unix {
    DESTDIR = ../../../lib
    LIBS = -L../../../lib -lphi
    QMAKE_LFLAGS_RPATH =
    CONFIG(debug,debug|release) {
        LIBS = -L../../../lib -lphi_debug
        TARGET = phia_debug
        DEFINES += PHIDEBUG
    } else: DEFINES += QT_NO_DEBUG_OUTPUT
    mac {
        contains( CONFIG, appstore ){
            DEFINES += PHIAPPSTORE
            QMAKE_CXXFLAGS += -gdwarf-2
            QMAKE_CFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
            QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO
            QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
        }
        DEFINES += PHIMACFONT
        LIBS = -L../../../lib -lphi
        QMAKE_MAC_SDK=$$[PHIMACSDK]
        QMAKE_INFO_PLIST = Info.plist
        QMAKE_MACOSX_DEPLOYMENT_TARGET = $$[PHIMACDEPLOY]
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
