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
#
#  Phi(sketeer) uses the great Qt C++ toolkit, freely available at <http://qt.digia.com>.
#
#  Qt 4.8.4 configuration for the official release for Linux (Ubuntu64 10.04):
#  ./configure -fast -no-qt3support -no-dbus -release -no-stl -no-pch -no-scripttools -no-multimedia
#     -no-exceptions -no-declarative -webkit -openssl -cups -nis -plugin-sql-mysql -plugin-sql-psql
#     -plugin-sql-odbc -phonon -nomake demos -nomake examples -prefix /usr/local/phi/qt484 -v
#     -fontconfig -xinerama -glib -xinput -xkb -xrender -xrandr -xfixes -xcursor -qmake-optimized
#     -iconv -sm -xsync
#  Note: Qt versions 4.8.2 and 4.8.3 have a bug in QScriptEngine which may crash PhiApp and Amphibia

#  Qt configuration for the official release for Mac OS X (Mountain lion):
#  ./configure -fast -no-qt3support -no-dbus -dwarf2 -release -no-stl -no-pch -no-scripttools
#     -no-multimedia -no-exceptions -no-declarative -webkit -openssl -arch x86_64 -cups -nis -v
#     -harfbuzz -nomake demos -nomake examples -no-rpath -qmake-optimized -prefix /usr/local/phi/qt484
#
#  For the Windows distribution the standard provided precompiled Qt packages are used.
#
#  Note: if you use Qt Creator do not enable shadow builds!

include( scripts/phiconf.pri )
message( Phi configuration: $$PHICONF )
OTHER_FILES = doc/license.txt doc/readme.txt scripts/phiupdate.xml doc/LGPLv3 doc/GPLv3

unix {
    message( "run 'make' to compile Phi(sketeer)" )
    updts.target = updatets
    updts.commands = scripts/updts.sh $$[QT_INSTALL_BINS] $$PWD
    QMAKE_EXTRA_TARGETS += updts
    OTHER_FILES += scripts/updts.sh
    QMAKE_CLEAN += -r bin/* lib/*
    QMAKE_DISTCLEAN += -r bin/* lib/*

    mac {
        message( "run 'make distmac' to create a Mac OSX packaged distribution in 'distmac'" )
        distmac.target = distmac
        distmac.commands = scripts/distmac.sh $$PHIRELEASE $$[QT_INSTALL_LIBS] $$[QT_INSTALL_BINS] \
            $$[QT_INSTALL_PLUGINS] $$[QT_INSTALL_TRANSLATIONS]
        distmac.depends = all

        mysqlmac.target = mysqlsetup
        mysqlmac.commands = cd ~/Downloads/qt-everywhere-commercial-src-$$[QT_VERSION]/src/plugins/sqldrivers/mysql/;
        mysqlmac.commands += $$[QT_INSTALL_BINS]/qmake INCLUDEPATH+="/usr/local/mysql/include" \
            LIBS+=\"-L/usr/local/mysql/lib -lmysqlclient_r\";
        mysqlmac.commands += make; sudo make install;

        QMAKE_EXTRA_TARGETS += distmac mysqlmac
        OTHER_FILES += scripts/distmac.sh
        QMAKE_DISTCLEAN += distmac/*
    } else {
        message( "run 'make targz' to create a Unix tar archive" )
        message( "or run 'make distlinux' to create a linux distribution")

        targz.target = targz
        targz.commands = scripts/targz.sh $$PHIRELEASE $$[QT_INSTALL_LIBS] $$[QT_INSTALL_BINS] \
            $$[QT_INSTALL_PLUGINS] $$[QT_INSTALL_TRANSLATIONS]
        targz.depends = all

        distlinux.target = distlinux
        distlinux.commands = scripts/distlinux.sh $$PHIRELEASE $$[QT_INSTALL_LIBS] \
            $$[QT_INSTALL_BINS] $$[QT_INSTALL_PLUGINS] $$[QT_INSTALL_TRANSLATIONS]
        distlinux.depends = all

        QMAKE_EXTRA_TARGETS += targz distlinux
        OTHER_FILES += scripts/distlinux.sh scripts/linuxinstaller.sh
        OTHER_FILES += scripts/linux.run scripts/targz.sh
        QMAKE_CLEAN += phisketeer-*.bin
        QMAKE_DISTCLEAN += phisketeer-*.bin
    }
}

win32 {
    message( "run 'nmake' to compile phi" )
    message( "run 'nmake distwin' to create a 'distwin' directory for the Windows distribution" )
    message( "You may then use the Phis.iss file to create an installer package." )

    distwin.target = distwin
    distwin.commands = scripts\\distwin.bat distwin $$[QT_INSTALL_BINS]\\.. $$PHIRELEASE
    distwin.depends = all

    mysqlwin.target = mysqlsetup
    mysqlwin.commands = scripts\\mysqlwin.bat $$[QT_VERSION]
    mysqlwin.depends =

    QMAKE_EXTRA_TARGETS += distwin mysqlwin
    OTHER_FILES += scripts/distwin.bat scripts/mysqlwin.bat
    QMAKE_CLEAN += bin\\* lib\\* distwin\\*
    QMAKE_DISTCLEAN += /S /F /Q bin lib distwin && rd /S /Q bin lib distwin
}

TEMPLATE = subdirs
SUBDIRS = src
exists( private/cfg.pri ): include( private/cfg.pri )
