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

HEADERS += phi.h \
    phisysinfo.h \
    phierr.h \
    phierror.h \
    phiitem.h \
    phipage.h \
    phibaseitem.h \
    phibasepage.h \
    phieffect.h \
    phieffects.h \
    phimemrotate.h \
    philicense.h \
    phiapplication.h \
    phipixlabel.h \
    phiupdateinfo.h \
    phicontext2d.h \
    phidomimage.h
SOURCES += phierror.cpp \
    phierrordesc.cpp \
    phiglobal.cpp \
    phiitem.cpp \
    phipage.cpp \
    phibaseitem.cpp \
    phibasepage.cpp \
    phimimetypes.cpp \
    phieffect.cpp \
    phieffects.cpp \
    phimemrotate.cpp \
    philicense.cpp \
    phiapplication.cpp \
    phipixlabel.cpp \
    phiupdateinfo.cpp \
    phicontext2d.cpp \
    phidomimage.cpp
VERSION = 1.2.0
include( ../../../scripts/phiconf.pri )
isEmpty( PHIDOM ): error( PHIDOM is not specified )
isEmpty( PHIORG ): error( PHIORG is not specified )
TEMPLATE = lib
TARGET = phi
QT = core gui network script widgets
DEFINES += PHILIB PHIVERSION=\\\"$$VERSION\\\" PHIDOM=\\\"$$PHIDOM\\\" PHIORG=\\\"$$PHIORG\\\"

unix { 
    DESTDIR = ../../../lib
    SOURCES += phisysinfo_unix.cpp
    CONFIG(debug,debug|release): TARGET = phi_debug
    mac {
        contains( CONFIG, macappstore ){
            OBJECTIVE_SOURCES += macfilebookmark.mm
            OBJECTIVE_HEADERS += macfilebookmark.h
            LIBS = -framework AppKit
        }
        TARGET = phi
    } else {
        QMAKE_LFLAGS +=-Wl,-rpath,\'\$$ORIGIN\'
    }
}
win32 {
    DESTDIR = ../../../bin
    SOURCES += phisysinfo_win32.cpp
    CONFIG(debug,debug|release): TARGET = phid
    QMAKE_DISTCLEAN += phi_resource.rc phid_resource.rc
}
