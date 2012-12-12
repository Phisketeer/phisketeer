# !/usr/bin/bash
QMAKE=$1/qmake
QTVER=$2
OLDPWD=`pwd`
cd /Users/schumi/Downloads/qt-everywhere-commercial-src-${QTVER}/src/plugins/sqldrivers/mysql/
$QMAKE mysql.pro
cd $OLDPWD
