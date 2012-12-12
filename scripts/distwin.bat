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
if not exist codecs mkdir codecs
if not exist phonon_backend mkdir phonon_backend
if not exist iconengines mkdir iconengines
if not exist pages mkdir pages
if not exist ts mkdir ts
REM if not exist pages\tutorial mkdir pages\tutorial
copy /y %QTPLUGINS%\imageformats\*.dll imageformats
del imageformats\*d4.dll
copy /y %QTPLUGINS%\sqldrivers\*.dll sqldrivers
del sqldrivers\*d4.dll
copy /y %QTPLUGINS%\codecs\*.dll codecs
del codecs\*d4.dll
copy /y %QTPLUGINS%\phonon_backend\*.dll phonon_backend
del phonon_backend\*d4.dll
copy /y %QTPLUGINS%\iconengines\*.dll iconengines
del iconengines\*d4.dll
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
copy /y ..\3rdparty\win32\libeay32.dll
copy /y ..\3rdparty\win32\ssleay32.dll
copy /y ..\3rdparty\win32\msvcr90.dll
copy /y ..\3rdparty\win32\msvcp90.dll
copy /y ..\3rdparty\win32\msvcm90.dll
copy /y ..\3rdparty\win32\Microsoft.VC90.CRT.manifest
copy /y C:\PROGRA~2\MySQL\MYSQLS~1.5\lib\libmysql.dll

REM add type library:
@echo on
%QTBINS%\idc.exe npphiplugin1.dll /idl npphiplugin.idl -version %PHIRELEASE%
midl npphiplugin.idl /nologo /tlb npphiplugin.tlb
%QTBINS%\idc.exe npphiplugin1.dll /tlb npphiplugin.tlb
@cd ..
