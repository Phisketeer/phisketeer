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
#
#    Exclusion: the Phi(sketeer) libraries libphi, libphia, libphis and
#    the Apache module mod_phi are released under the terms of the LGPLv3.

PHIDOM = phisketeer.org
PHIORG = Phisketeer
PHICONF = warn_on
PHIRELEASE = 1.5.0
PHIMACDEPLOY = 10.6
PHIMACSDK = /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.8.sdk
#mac: PHICONF += x86_64 macappstore
mac: PHICONF += x86_64

CONFIG += qt thread largefile ordered $$[PHICONF]
INCLUDEPATH += .
DEFINES += QT_NO_CAST_TO_ASCII
OBJECTS_DIR = .tmp
MOC_DIR = .tmp
RCC_DIR = .tmp
CONFIG(debug,debug|release): DEFINES += PHIDEBUG
else: DEFINES += QT_NO_DEBUG_OUTPUT
unix {
    QMAKE_LFLAGS_RPATH =
    mac {
        contains( CONFIG, macappstore ){
            DEFINES += PHIAPPSTORE
            QMAKE_CFLAGS += -gdwarf-2
            QMAKE_CXXFLAGS += -gdwarf-2
            QMAKE_CFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
            QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO
            QMAKE_OBJECTIVE_CFLAGS_RELEASE = $$QMAKE_OBJECTIVE_CFLAGS_RELEASE_WITH_DEBUGINFO
            QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
        }
        QMAKE_MAC_SDK = $$[PHIMACSDK]
        QMAKE_MACOSX_DEPLOYMENT_TARGET = $$[PHIMACDEPLOY]
        DEFINES += PHIMACFONT
    }
}
