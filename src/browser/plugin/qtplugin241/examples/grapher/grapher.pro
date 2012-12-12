TARGET		= grapher

SOURCES		= grapher.cpp

win32 {
# On Windows, uncomment the following lines to build a plugin that can
# be used also in Internet Explorer, through ActiveX.
#  CONFIG += qaxserver
#  LIBS += -lurlmon

   RC_FILE		= grapher.rc
} else:mac {
   QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.4
   CONFIG += ppc x86
   QMAKE_INFO_PLIST = Info.plist
   REZ_FILES += grapher.r
   rsrc_files.files = grapher.rsrc
   rsrc_files.path = Contents/Resources
   QMAKE_BUNDLE_DATA += rsrc_files
   #QMAKE_LFLAGS += -undefined dynamic_lookup
}

include(../../src/qtbrowserplugin.pri)
