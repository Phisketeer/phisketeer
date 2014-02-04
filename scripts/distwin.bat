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
if not exist accessible mkdir accessible
if not exist sqldrivers mkdir sqldrivers
if not exist platforms mkdir platforms
if not exist iconengines mkdir iconengines
if not exist printsupport mkdir printsupport
if not exist mediaservice mkdir mediaservice
if not exist pages mkdir pages
if not exist ts mkdir ts
if not exist modules mkdir modules
if not exist items mkdir items
copy /y %QTPLUGINS%\imageformats\*.dll imageformats
del imageformats\*d.dll
copy /y %QTPLUGINS%\accessible\*.dll accessible
del accessible\*d.dll
copy /y %QTPLUGINS%\sqldrivers\*.dll sqldrivers
del sqldrivers\*d.dll
copy /y %QTPLUGINS%\platforms\*.dll platforms
del platforms\*d.dll
copy /y %QTPLUGINS%\printsupport\*.dll printsupport
del printsupport\*d.dll
copy /y %QTPLUGINS%\iconengines\*.dll iconengines
del iconengines\*d.dll
copy /y %QTPLUGINS%\mediaservice\*.dll mediaservice
del mediaservice\*d.dll
copy /y %QTTRANSLATIONS%\qt_*.qm ts
copy /y %QTTRANSLATIONS%\qtmultimedia_*.qm ts
copy /y %QTTRANSLATIONS%\qtbase_*.qm ts
copy /y %QTTRANSLATIONS%\qtconfig*.qm ts
copy /y %QTTRANSLATIONS%\qtscript*.qm ts
del ts\qt_help*
copy /y ..\src\ts\*.qm ts
cd ts
for %%i in (*.ts) do %QTBINS%\lrelease -removeidentical %%i
del *.ts
cd ..
copy /y ..\bin\*.dll
copy /y ..\3rdparty\win32\Artephis.exe
copy /y ..\bin\*.exe
copy /y ..\bin\plugins\modules\*.dll modules
copy /y ..\bin\plugins\items\*.dll items
copy /y ..\doc\readme.txt README.txt
copy /y ..\doc\license.txt
copy /y ..\doc\trademarks.txt
copy /y ..\pages\*.phis pages
copy /y ..\scripts\Phis.iss
copy /y ..\3rdparty\win32\openssl\libeay32.dll
copy /y ..\3rdparty\win32\openssl\ssleay32.dll
copy /y ..\3rdparty\win32\redist\msvcr110.dll
copy /y ..\3rdparty\win32\redist\msvcp110.dll
if exist C:\PROGRA~2\MySQL\MYSQLS~1.5\lib\libmysql.dll copy /y C:\PROGRA~2\MySQL\MYSQLS~1.5\lib\libmysql.dll
if exist "C:\Program Files (x86)\MySQL\MySQL Connector C 6.1\lib\libmysql.dll" copy /y "C:\Program Files (x86)\MySQL\MySQL Connector C 6.1\lib\libmysql.dll"

REM add type library:
REM @echo on
REM %QTBINS%\idc.exe npphiplugin1.dll /idl npphiplugin.idl -version %PHIRELEASE%
REM midl npphiplugin.idl /nologo /tlb npphiplugin.tlb
REM %QTBINS%\idc.exe npphiplugin1.dll /tlb npphiplugin.tlb
cd ..
