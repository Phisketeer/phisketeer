@echo off
set DISTDIR=%1%
set QTBINS=%2%\bin
set QTPLUGINS=%2%\plugins
set QTTRANSLATIONS=%2%\translations
set PHIRELEASE=%3%
if not exist %DISTDIR% mkdir %DISTDIR%
cd %DISTDIR%
copy /y %QTBINS%\*.dll
REM del *d4.dll
copy /y %QTBINS%\idc.exe
if not exist imageformats mkdir imageformats
if not exist sqldrivers mkdir sqldrivers
if not exist platforms mkdir platforms
if not exist iconengines mkdir iconengines
if not exist printsupport mkdir printsupport
if not exist mediaservice mkdir mediaservice
if not exist pages mkdir pages
if not exist ts mkdir ts
copy /y %QTPLUGINS%\imageformats\*.dll imageformats
del imageformats\*d4.dll
copy /y %QTPLUGINS%\sqldrivers\*.dll sqldrivers
del sqldrivers\*d4.dll
copy /y %QTPLUGINS%\platforms\*.dll platforms
del platforms\*d4.dll
copy /y %QTPLUGINS%\printsupport\*.dll printsupport
del printsupport\*d4.dll
copy /y %QTPLUGINS%\iconengines\*.dll iconengines
del iconengines\*d4.dll
copy /y %QTPLUGINS%\mediaservice\*.dll mediaservice
del mediaservice\*d4.dll
copy /y %QTTRANSLATIONS%\qt_*.ts ts
del ts\qt_help*
copy /y ..\src\ts\*.qm ts
cd ts
for %%i in (*.ts) do %QTBINS%\lrelease -removeidentical %%i
del *.ts
cd ..
copy /y ..\bin\*.dll
copy /y ..\3rdparty\win32\artephis.exe bin
copy /y ..\bin\*.exe
copy /y ..\doc\readme.txt README.txt
copy /y ..\doc\license.txt
copy /y ..\doc\trademarks.txt
copy /y ..\pages\*.phis pages
copy /y ..\scripts\Phis.iss
copy /y ..\3rdparty\win32\openssl\libeay32.dll
copy /y ..\3rdparty\win32\openssl\ssleay32.dll
copy /y ..\3rdparty\win32\redist\msvcr100.dll
copy /y ..\3rdparty\win32\redits\msvcp100.dll
if exist C:\PROGRA~2\MySQL\MYSQLS~1.5\lib\libmysql.dll copy /y C:\PROGRA~2\MySQL\MYSQLS~1.5\lib\libmysql.dll

REM add type library:
@echo on
%QTBINS%\idc.exe npphiplugin1.dll /idl npphiplugin.idl -version %PHIRELEASE%
midl npphiplugin.idl /nologo /tlb npphiplugin.tlb
%QTBINS%\idc.exe npphiplugin1.dll /tlb npphiplugin.tlb
@cd ..
