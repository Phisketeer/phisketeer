@echo off
set QTVER=%1%
echo "QTVER=%QTVER%"
cd C:\Qt\%QTVER%\Src\qtbase\src\plugins\sqldrivers\mysql
C:\Qt\%QTVER%\msvc2010\bin\qmake CONFIG+=release INCLUDEPATH+="C:\PROGRA~2\MySQL\MYSQLS~1.5\include" LIBS+="C:\PROGRA~2\MySQL\MYSQLS~1.5\lib\libmysql.lib" mysql.pro
nmake
nmake install
cd C:\Users\Marius\Documents\phi
