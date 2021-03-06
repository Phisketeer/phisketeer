# !/usr/bin/sh

DESTDIR=phisketeer-$1
QTLIBS=$2
QTBINS=$3
QTPLUGINS=$4
QTTS=$5
USEQT="libQt5Core libQt5Network libQt5Sql libQt5Gui libQt5Widgets libQt5PrintSupport libQt5Svg libQt5Qml libQt5OpenGL\
    libQt5Script libQt5WebKit libQt5WebKitWidgets libQt5Xml libQt5Quick libQt5Sensors libQt5Positioning"

rm -rf $DESTDIR
echo "Creating directories"
mkdir -p $DESTDIR/plugins/sqldrivers
mkdir $DESTDIR/plugins/accessible
mkdir $DESTDIR/plugins/imageformats
mkdir $DESTDIR/plugins/iconengines
mkdir $DESTDIR/plugins/platforms
mkdir $DESTDIR/plugins/mediaservice
mkdir $DESTDIR/plugins/printsupport
mkdir $DESTDIR/plugins/modules
mkdir $DESTDIR/plugins/items
mkdir $DESTDIR/lib
mkdir $DESTDIR/lib/fonts
mkdir $DESTDIR/bin
mkdir $DESTDIR/ts

echo "Copying Qt libs" 
for I in $USEQT ; do
	cp -p $QTLIBS/$I.so.? $DESTDIR/lib
done;
cp -p $QTTS/qt_*.qm $DESTDIR/ts
cp -p $QTTS/qtbase_*.qm $DESTDIR/ts
cp -p $QTTS/qtscript_*.qm $DESTDIR/ts
cp -p $QTTS/qtmultimedia_*.qm $DESTDIR/ts
rm -f $DESTDIR/ts/qt_help*
chmod -x $DESTDIR/ts/*

#cp -p src/libsrc/qtscriptclassic/lib/*.so.? $DESTDIR/lib
echo "Copying Qt plugins"
cp -p $QTPLUGINS/sqldrivers/*.so $DESTDIR/plugins/sqldrivers/
cp -p $QTPLUGINS/accessible/*.so $DESTDIR/plugins/accessible/
cp -p $QTPLUGINS/imageformats/*.so $DESTDIR/plugins/imageformats/
cp -p $QTPLUGINS/iconengines/*.so $DESTDIR/plugins/iconengines/
cp -p $QTPLUGINS/platforms/*.so $DESTDIR/plugins/platforms/
cp -p $QTPLUGINS/printsupport/*.so $DESTDIR/plugins/printsupport/
cp -p $QTPLUGINS/mediaservice/*.so $DESTDIR/plugins/mediaservice/

echo "Copying fonts"
cp -Rp $QTLIBS/fonts/* $DESTDIR/lib/fonts/

echo "Copying Phi"
cp -p lib/*.so.? $DESTDIR/lib
#cp -p lib/libphiplugin.so $DESTDIR/lib
cp -p lib/modules/*.so $DESTDIR/plugins/modules
cp -p plugins/items/*.so $DESTDIR/plugins/items
if [ -e /usr/lib/x86_64-linux-gnu/libmysqlclient_r.so.18 ]; then
    cp -p /usr/lib/x86_64-linux-gnu/libmysqlclient_r.so.18 $DESTDIR/lib
elif [ -e /usr/lib/libmysqlclient_r.so.16 ]; then
    cp -p /usr/lib/libmysqlclient_r.so.16 $DESTDIR/lib
elif [ -e /usr/lib64/mysql/libmysqlclient_r.so.15 ]; then
    cp -p /usr/lib64/mysql/libmysqlclient_r.so.15 $DESTDIR/lib
else
    echo "Did not found a 'libmysqlclient_r.so' - mysql will not be available."
fi
cp -p bin/* $DESTDIR/bin
rm -f $DESTDIR/bin/philicense
if [ ! -e $DESTDIR/bin/Artephis ]; then
    if [ `uname -m` = "x86_64" ]; then cp -p 3rdparty/linux/Artephis_64 $DESTDIR/bin/Artephis;
    else cp -p 3rdparty/linux/Artephis_32 $DESTDIR/bin/Artephis; fi
fi
# Some recent Linux distributions only provide libcrypto.so and libssl.so version 1.0.0
#if [ -f /usr/lib/libcrypto.so.0.9.8 ]; then
#    cp -p /usr/lib/libcrypto.so.0.9.8 $DESTDIR/lib/
#fi
#if [ -f /usr/lib/libssl.so.0.9.8 ]; then
#    cp -p /usr/lib/libssl.so.0.9.8 $DESTDIR/lib/
#fi
# ICUI is not installed on every system
if [ -f $QTLIBS/libicui18n.so.?? ]; then
    cp -p $QTLIBS/libicui18n.so.?? $DESTDIR/lib/
    cp -p $QTLIBS/libicuuc.so.?? $DESTDIR/lib/
    cp -p $QTLIBS/libicudata.so.?? $DESTDIR/lib/
elif [ -f /usr/lib/libicui18n.so.?? ]; then
    cp -p /usr/lib/libicui18n.so.?? $DESTDIR/lib/
    cp -p /usr/lib/libicuuc.so.?? $DESTDIR/lib/
    cp -p /usr/lib/libicudata.so.?? $DESTDIR/lib/
fi
# Additional libs
if [ -f /usr/lib/x86_64-linux-gnu/libXcomposite.so.1 ]; then
    cp -p /usr/lib/x86_64-linux-gnu/libXcomposite.so.1 $DESTDIR/lib
fi
if [ -f /usr/lib/x86_64-linux-gnu/mesa/libGL.so.1 ]; then
    cp -p /usr/lib/x86_64-linux-gnu/mesa/libGL.so.1 $DESTDIR/lib
fi
if [ -f /usr/lib/x86_64-linux-gnu/libXxf86vm.so.1 ]; then
    cp -p /usr/lib/x86_64-linux-gnu/libXxf86vm.so.1 $DESTDIR/lib
fi

strip --strip-unneeded $DESTDIR/lib/*
strip --strip-unneeded $DESTDIR/bin/*
cp -p doc/readme.txt $DESTDIR/
mv $DESTDIR/readme.txt $DESTDIR/README.txt
cp -p doc/license.txt $DESTDIR/
cp -p scripts/phis.sh $DESTDIR/
cp -p scripts/linuxuninstall.sh $DESTDIR/
cp -p pages/phitest.phis $DESTDIR/
cp -p pages/phitestpost.phis $DESTDIR/
cp -p src/ts/*.qm $DESTDIR/ts
chmod a+x $DESTDIR/lib/*

echo "Creating tar archive"
tar -cvf $DESTDIR.tar $DESTDIR/
echo "Compressing archive"
gzip $DESTDIR.tar

exit 0

