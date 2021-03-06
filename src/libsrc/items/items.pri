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
include( ../../../phiconf.pri )
QT += core gui widgets script network sql
TEMPLATE = lib
INCLUDEPATH += ../../phi
win32 {
    DESTDIR = ../../../../bin/plugins/items
    LIBS = -L../../../../bin phi2.lib
    CONFIG(debug,debug|release) {
        LIBS = -L../../../../bin phid2.lib
    }
}
unix {
    DESTDIR = ../../../../plugins/items
    LIBS = -L../../../../lib -lphi
    CONFIG(debug,debug|release){
        LIBS = -L../../../../lib -lphi_debug
    }
    mac {
        LIBS = -L../../../../lib -lphi
    } else {
        QMAKE_LFLAGS +=-Wl,-rpath,\'\$$ORIGIN/../../lib\',-rpath-link,$$[QT_INSTALL_LIBS]
    }
}
