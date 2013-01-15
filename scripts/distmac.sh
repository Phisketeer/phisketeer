# !/bin/bash

# if you change DESTDIR you must check DISTPLUGINS also:
DESTDIR=distmac
QTLIBS=$2
QTBINS=$3
QTPLUGINS=$4
QTTS=$5
PHIMAS=$6
MYSQLC=libmysqlclient.18.dylib
MYSQLDIR=/usr/local/mysql
USEQT="QtCore QtNetwork QtSql QtGui QtSvg QtScript QtQml QtOpenGL QtWidgets QtWebKit QtWebKitWidgets QtQuick QtPrintSupport"
PLUGINPATH="sqldrivers imageformats iconengines platforms printsupport mediaservice accessible playlistformats"
PHILIBS="libphi.1.dylib libphis.1.dylib libphia.1.dylib"
PHIAPPS="Artephis Amphibia"
PHIBINS="phis phiapp phisconf"

echo "Cleaning previous installation"
rm -rf $DESTDIR
mkdir $DESTDIR
if [ ! -d bin/Artephis.app ] ; then
    cp -Rp 3rdparty/macosx/Artephis.app bin/
fi

echo "Creating app directories"
for A in $PHIAPPS ; do
    echo "cp $A"
    cp -Rp bin/$A.app $DESTDIR/
    mkdir -p $DESTDIR/$A.app/Contents/PlugIns/phi
    for P in $PLUGINPATH ; do
        mkdir $DESTDIR/$A.app/Contents/PlugIns/$P
    done
    mkdir $DESTDIR/$A.app/Contents/Frameworks
    mkdir -p $DESTDIR/$A.app/Contents/SharedSupport/pages
    mkdir -p $DESTDIR/$A.app/Contents/Resources/ts
done

echo "Copying Qt libs"
for A in $PHIAPPS ; do
    for I in $USEQT ; do
        cp -Rp $QTLIBS/$I.framework $DESTDIR/$A.app/Contents/Frameworks
    done;
    cp -Rp $QTTS/qt_*.qm $DESTDIR/$A.app/Contents/Resources/ts
    rm -f $DESTDIR/$A.app/Contents/Resources/ts/qt_help*
    for P in $PLUGINPATH ; do
        cp -p $QTPLUGINS/$P/*.dylib $DESTDIR/$A.app/Contents/PlugIns/$P/
    done
    rm -f $DESTDIR/$A.app/Contents/Frameworks/*/Headers
    #rm -rf $DESTDIR/$A.app/Contents/Frameworks/*/Qt*
    rm -rf $DESTDIR/$A.app/Contents/Frameworks/*/Versions/5/Headers
    #rm -f $DESTDIR/Frameworks/*/Versions/Current
    #rm -f $DESTDIR/Frameworks/QtGui.framework/Resources
    DISTPLUGINS=`cd $DESTDIR/$A.app/Contents/PlugIns; ls -1 */*`
done
echo "Used Qt plugins:\n$DISTPLUGINS"

echo "Copying Phi libs"
for A in $PHIAPPS ; do
    for L in $PHILIBS ; do
        cp -p lib/$L $DESTDIR/$A.app/Contents/PlugIns/phi
    done
done

echo "Copying Artephis stuff"
cp -Rp bin/phis $DESTDIR/Artephis.app/Contents/MacOS/
cp -Rp bin/phisconf $DESTDIR/Artephis.app/Contents/MacOS/
cp -Rp bin/phiapp $DESTDIR/Artephis.app/Contents/MacOS/
if [ -f $MYSQLDIR/lib/$MYSQLC ] ; then
    cp -p $MYSQLDIR/lib/$MYSQLC $DESTDIR/Artephis.app/Contents/PlugIns/phi
else
    echo "Did not find '$MYSQLC' in '$MYSQLDIR/lib' - mysql is not useable."
fi
#cp -p src/libsrc/phi/images/own/philogo_fade_512.png $DESTDIR/Artephis.app/Contents/Resources
#cp -p src/editor/phis.icns $DESTDIR/Artephis.app/Contents/Resources/
cp -p src/ts/*.qm $DESTDIR/Artephis.app/Contents/Resources/ts/
cp -p pages/phitest.phis $DESTDIR/Artephis.app/Contents/SharedSupport/pages
cp -p pages/phitestpost.phis $DESTDIR/Artephis.app/Contents/SharedSupport/pages
cp -p src/server/mime.types $DESTDIR/Artephis.app/Contents/SharedSupport/mimetypes.txt
cp -p doc/nosandbox.entitlements $DESTDIR/Artephis.app/Contents/Resources/editor.entitlements
cp -p doc/nosandbox.entitlements $DESTDIR/Artephis.app/Contents/Resources/server.entitlements
cp -p doc/nosandbox.entitlements $DESTDIR/Artephis.app/Contents/Resources/phiapp.entitlements
cp -p doc/nosandbox.entitlements $DESTDIR/Artephis.app/Contents/Resources/sconfig.entitlements
echo "Sandboxing is disabled"
chmod a-wx $DESTDIR/Artephis.app/Contents/Resources/ts/*
chmod u+w $DESTDIR/Artephis.app/Contents/Resources/ts/*

echo "Copying Amphibia stuff"
cp -Rp bin/phiapp $DESTDIR/Amphibia.app/Contents/MacOS/
#cp -p src/libsrc/phi/images/own/philogo_fade_512.png $DESTDIR/Amphibia.app/Contents/Resources
cp -p src/ts/*.qm $DESTDIR/Amphibia.app/Contents/Resources/ts/
#if [ -f $MYSQLDIR/lib/$MYSQLC ] ; then
#    cp -p $MYSQLDIR/lib/$MYSQLC $DESTDIR/Amphibia.app/Contents/PlugIns/phi
#fi
chmod a-wx $DESTDIR/Amphibia.app/Contents/Resources/ts/*
chmod u+w $DESTDIR/Artephis.app/Contents/Resources/ts/*

echo Stripping libs
for A in $PHIAPPS ; do
    #strip -u -r $DESTDIR/$A.app/Contents/MacOS/*
    strip -x $DESTDIR/$A.app/Contents/Frameworks/*/Versions/5/Qt*
    strip -x $DESTDIR/$A.app/Contents/PlugIns/*/*
    chmod -R go-w $DESTDIR/$A.app/Contents/Plugins/*
    chmod -R go-w $DESTDIR/$A.app/Contents/Frameworks/*
done

echo "Copying misc"
for A in $PHIAPPS ; do
    cp -p doc/readme.txt $DESTDIR/$A.app/Contents/Resources
    cp -p doc/license.txt $DESTDIR/$A.app/Contents/Resources
    chmod a-wx $DESTDIR/$A.app/Contents/Resources/*
    chmod u+w $DESTDIR/$A.app/Contents/Resources/*
    chmod a+x $DESTDIR/$A.app/Contents/Resources/ts
done

echo Changing library ids and pathes
for A in $PHIAPPS ; do
    for I in $PHILIBS ; do
        install_name_tool -id @executable_path/../PlugIns/phi/$I $DESTDIR/$A.app/Contents/PlugIns/phi/$I
        for L in $PHILIBS ; do
            if [ $L = $I ] ; then continue; fi
            install_name_tool -change $L @executable_path/../PlugIns/phi/$L\
                $DESTDIR/$A.app/Contents/PlugIns/phi/$I
        done;
    done;
done;

for A in $PHIAPPS ; do
    for L in $PHILIBS ; do
        install_name_tool -change $L @executable_path/../PlugIns/phi/$L\
            $DESTDIR/$A.app/Contents/MacOS/$A
    done;
done;
if [ -f $MYSQLDIR/lib/$MYSQLC ] ; then
    install_name_tool -id @executable_path/../PlugIns/phi/$MYSQLC\
        $DESTDIR/Artephis.app/Contents/PlugIns/phi/$MYSQLC
    install_name_tool -change $MYSQLC @executable_path/../PlugIns/phi/$MYSQLC\
        $DESTDIR/Artephis.app/Contents/PlugIns/sqldrivers/libqsqlmysql.dylib
fi
for L in $PHILIBS ; do
    install_name_tool -change $L @executable_path/../PlugIns/phi/$L\
        $DESTDIR/Artephis.app/Contents/MacOS/phis
    install_name_tool -change $L @executable_path/../PlugIns/phi/$L\
        $DESTDIR/Artephis.app/Contents/MacOs/phisconf
    install_name_tool -change $L @executable_path/../PlugIns/phi/$L\
        $DESTDIR/Artephis.app/Contents/MacOS/phiapp
    install_name_tool -change $L @executable_path/../PlugIns/phi/$L\
        $DESTDIR/Amphibia.app/Contents/MacOS/phiapp
done;

for A in $PHIAPPS; do
    for I in $DISTPLUGINS ; do
        install_name_tool -id @executable_path/../PlugIns/$I $DESTDIR/$A.app/Contents/PlugIns/$I
    done
done

for A in $PHIAPPS; do
    for I in $USEQT ; do
        install_name_tool -id @executable_path/../Frameworks/$I.framework/Versions/5/$I\
            $DESTDIR/$A.app/Contents/Frameworks/$I.framework/Versions/5/$I
    done
done

for I in $USEQT; do
    # if rpath was set in the Qt configure script:
    install_name_tool -change $QTLIBS/$I.framework/Versions/5/$I\
        @executable_path/../Frameworks/$I.framework/Versions/5/$I\
        $DESTDIR/Artephis.app/Contents/MacOS/phis
    install_name_tool -change $QTLIBS/$I.framework/Versions/5/$I\
        @executable_path/../Frameworks/$I.framework/Versions/5/$I\
        $DESTDIR/Artephis.app/Contents/MacOS/phisconf
    install_name_tool -change $QTLIBS/$I.framework/Versions/5/$I\
        @executable_path/../Frameworks/$I.framework/Versions/5/$I\
        $DESTDIR/Artephis.app/Contents/MacOS/phiapp
    install_name_tool -change $QTLIBS/$I.framework/Versions/5/$I\
        @executable_path/../Frameworks/$I.framework/Versions/5/$I\
        $DESTDIR/Amphibia.app/Contents/MacOS/phiapp
    # if rpath was not set:
    install_name_tool -change $I.framework/Versions/5/$I\
        @executable_path/../Frameworks/$I.framework/Versions/5/$I\
        $DESTDIR/Artephis.app/Contents/MacOS/phis
    install_name_tool -change $I.framework/Versions/5/$I\
        @executable_path/../Frameworks/$I.framework/Versions/5/$I\
        $DESTDIR/Artephis.app/Contents/MacOS/phisconf
    install_name_tool -change $I.framework/Versions/5/$I\
        @executable_path/../Frameworks/$I.framework/Versions/5/$I\
        $DESTDIR/Artephis.app/Contents/MacOS/phiapp
    install_name_tool -change $I.framework/Versions/5/$I\
        @executable_path/../Frameworks/$I.framework/Versions/5/$I\
        $DESTDIR/Amphibia.app/Contents/MacOS/phiapp

    for A in $PHIAPPS ; do
        install_name_tool -change $QTLIBS/$I.framework/Versions/5/$I\
            @executable_path/../Frameworks/$I.framework/Versions/5/$I\
            $DESTDIR/$A.app/Contents/MacOS/$A
        install_name_tool -change $I.framework/Versions/5/$I\
            @executable_path/../Frameworks/$I.framework/Versions/5/$I\
            $DESTDIR/$A.app/Contents/MacOS/$A
        for L in $USEQT ; do
            if [ $L = $I ] ; then continue; fi
            install_name_tool -change $QTLIBS/$I.framework/Versions/5/$I\
                @executable_path/../Frameworks/$I.framework/Versions/5/$I\
                $DESTDIR/$A.app/Contents/Frameworks/$L.framework/Versions/5/$L
            install_name_tool -change $I.framework/Versions/5/$I\
                @executable_path/../Frameworks/$I.framework/Versions/5/$I\
                $DESTDIR/$A.app/Contents/Frameworks/$L.framework/Versions/5/$L
        done
        for L in $PHILIBS ; do
            install_name_tool -change $QTLIBS/$I.framework/Versions/5/$I\
                @executable_path/../Frameworks/$I.framework/Versions/5/$I\
                $DESTDIR/$A.app/Contents/PlugIns/phi/$L
            install_name_tool -change $I.framework/Versions/5/$I\
                @executable_path/../Frameworks/$I.framework/Versions/5/$I\
                $DESTDIR/$A.app/Contents/PlugIns/phi/$L
        done
        for P in $DISTPLUGINS ; do
            install_name_tool -change $QTLIBS/$I.framework/Versions/5/$I\
                @executable_path/../Frameworks/$I.framework/Versions/5/$I\
                $DESTDIR/$A.app/Contents/PlugIns/$P
            install_name_tool -change $I.framework/Versions/5/$I\
                @executable_path/../Frameworks/$I.framework/Versions/5/$I\
                $DESTDIR/$A.app/Contents/PlugIns/$P
        done
    done
done

for A in $PHIAPPS ; do
    echo "Building product for $A"
    productbuild --component $DESTDIR/$A.app /Applications $DESTDIR/$A.pkg
done
echo "You may use '$> sudo installer -store -pkg distmac/Artephis.pkg -target /'"
echo "if you want to install the Artephis package to the system."
exit 0
