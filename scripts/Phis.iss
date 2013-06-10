[Setup]
AppName=Phisketeer
AppVerName=Phisketeer V1.5.0
DefaultDirName={pf}\Phisketeer
DefaultGroupName=Phisketeer
UninstallDisplayIcon={app}\artephis.exe
OutputDir=setup
LicenseFile=license.txt
PrivilegesRequired=admin
ChangesAssociations=yes
AppPublisherURL=http://www.phisketeer.org
AppSupportURL=http://www.phisketeer.org
AppUpdatesURL=http://www.phisketeer.org

[Types]
Name: full; Description: Full installation (standalone)
Name: apache; Description: Full installation as Apache module (mod_phi)
Name: custom; Description: Custom installation; Flags: iscustom

[Components]
Name: core; Description: Program libraries; Types: full apache custom; Flags: fixed
;Name: plugin; Description: Phi browser plug-in; Types: full custom
Name: server; Description: Phis server; Types: full custom
Name: apache; Description: Apache module; Types: apache
Name: browser; Description: Amphibia browser; Types: full apache custom
Name: editor; Description: Artephis editor; Types: full apache custom
;Name: docs; Description: Documentation; Types: full apache custom
;Name: tutorial; Description: Tutorial; Types: full apache custom; Flags: disablenouninstallwarning
;Name: readme\en; Description: English; Flags: exclusive
;Name: readme\de; Description: German; Flags: exclusive

[Files]
Source: phi1.dll; DestDir: {app}\bin; Components: core; Flags: 32bit replacesameversion restartreplace uninsnosharedfileprompt
Source: phia1.dll; DestDir: {app}\bin; Components: core; Flags: 32bit replacesameversion restartreplace uninsnosharedfileprompt
;Source: npphiplugin1.dll; DestDir: {app}\bin; Components: plugin; Flags: regserver regtypelib replacesameversion 32bit uninsnosharedfileprompt

Source: msvcr100.dll; DestDir: {app}\bin; Components: core; Flags: 32bit uninsnosharedfileprompt
Source: msvcp100.dll; DestDir: {app}\bin; Components: core; Flags: 32bit uninsnosharedfileprompt
;Source: msvcm90.dll; DestDir: {app}\bin; Components: core; Flags: 32bit uninsnosharedfileprompt
;Source: Microsoft.VC90.CRT.manifest; DestDir: {app}\bin; Components: core; Flags: 32bit uninsnosharedfileprompt
Source: libmysql.dll; DestDir: {app}\bin; Components: core; Flags: 32bit replacesameversion

Source: Qt5Core.dll; DestDir: {app}\bin; Components: core; Flags: 32bit restartreplace uninsnosharedfileprompt
Source: Qt5Gui.dll; DestDir: {app}\bin; Components: core; Flags: 32bit restartreplace uninsnosharedfileprompt
Source: Qt5Script.dll; DestDir: {app}\bin; Components: core; Flags: 32bit restartreplace uninsnosharedfileprompt
Source: Qt5Widgets.dll; DestDir: {app}\bin; Components: core; Flags: 32bit restartreplace uninsnosharedfileprompt
Source: Qt5Network.dll; DestDir: {app}\bin; Components: core; Flags: 32bit restartreplace uninsnosharedfileprompt
Source: Qt5Sql.dll; DestDir: {app}\bin; Components: core; Flags: 32bit restartreplace uninsnosharedfileprompt
Source: Qt5Svg.dll; DestDir: {app}\bin; Components: core; Flags: 32bit restartreplace uninsnosharedfileprompt
Source: Qt5MultimediaWidgets.dll; DestDir: {app}\bin; Components: core; Flags: 32bit restartreplace uninsnosharedfileprompt
Source: Qt5Multimedia.dll; DestDir: {app}\bin; Components: core; Flags: 32bit restartreplace uninsnosharedfileprompt
Source: Qt5WebKit.dll; DestDir: {app}\bin; Components: core; Flags: 32bit restartreplace uninsnosharedfileprompt
Source: Qt5WebKitWidgets.dll; DestDir: {app}\bin; Components: core; Flags: 32bit restartreplace uninsnosharedfileprompt
Source: Qt5V8.dll; DestDir: {app}\bin; Components: core; Flags: 32bit restartreplace uninsnosharedfileprompt
Source: Qt5Xml.dll; DestDir: {app}\bin; Components: core; Flags: 32bit restartreplace uninsnosharedfileprompt
;Source: Qt5XmlPatterns.dll; DestDir: {app}\bin; Components: core; Flags: 32bit restartreplace uninsnosharedfileprompt
;Source: Qt5CLucene.dll; DestDir: {app}\bin; Components: core; Flags: 32bit restartreplace uninsnosharedfileprompt
Source: Qt5PrintSupport.dll; DestDir: {app}\bin; Components: core; Flags: 32bit restartreplace uninsnosharedfileprompt
Source: Qt5OpenGL.dll; DestDir: {app}\bin; Components: core; Flags: 32bit restartreplace uninsnosharedfileprompt
Source: Qt5Quick.dll; DestDir: {app}\bin; Components: core; Flags: 32bit restartreplace uninsnosharedfileprompt
Source: Qt5Qml.dll; DestDir: {app}\bin; Components: core; Flags: 32bit restartreplace uninsnosharedfileprompt

Source: D3DCompiler_43.dll; DestDir: {app}\bin; Components: core; Flags: 32bit restartreplace uninsnosharedfileprompt
Source: libEGL.dll; DestDir: {app}\bin; Components: core; Flags: 32bit restartreplace uninsnosharedfileprompt
Source: libGLESv2.dll; DestDir: {app}\bin; Components: core; Flags: 32bit restartreplace uninsnosharedfileprompt
Source: libeay32.dll; DestDir: {app}\bin; Components: core; Flags: 32bit restartreplace uninsnosharedfileprompt
Source: ssleay32.dll; DestDir: {app}\bin; Components: core; Flags: 32bit restartreplace uninsnosharedfileprompt
Source: icudt49.dll; DestDir: {app}\bin; Components: core; Flags: 32bit replacesameversion uninsnosharedfileprompt
Source: icuin49.dll; DestDir: {app}\bin; Components: core; Flags: 32bit replacesameversion uninsnosharedfileprompt
Source: icuuc49.dll; DestDir: {app}\bin; Components: core; Flags: 32bit replacesameversion uninsnosharedfileprompt
;Source: idc.exe; DestDir: {app}\bin; Components: core; Flags: 32bit replacesameversion deleteafterinstall

Source: sqldrivers\*.dll; DestDir: {app}\bin\plugins\sqldrivers; Components: core; Flags: 32bit replacesameversion
Source: printsupport\*.dll; DestDir: {app}\bin\plugins\printsupport; Components: core; Flags: 32bit replacesameversion
Source: imageformats\*.dll; DestDir: {app}\bin\plugins\imageformats; Components: core; Flags: 32bit replacesameversion
Source: platforms\*.dll; DestDir: {app}\bin\plugins\platforms; Components: core; Flags: 32bit replacesameversion
Source: iconengines\*.dll; DestDir: {app}\bin\plugins\iconengines; Components: core; Flags: 32bit replacesameversion
Source: mediaservice\*.dll; DestDir: {app}\bin\plugins\mediaservice; Components: core; Flags: 32bit replacesameversion
Source: modules\*.dll; DestDir: {app}\bin\plugins\modules; Components: core; Flags: 32bit replacesameversion
Source: ts\*.qm; DestDir: {app}\ts; Components: core; Flags: replacesameversion

Source: phis.exe; DestDir: {app}\bin; Components: server; Flags: 32bit replacesameversion restartreplace
;Source: pages\tutorial\*.phis; DestDir: {code:GetDataDir}\default\{code:GetFQDN}\phitutorial; Components: server; Check: CheckForTutorial(); Flags: replacesameversion
;Source: pages\tutorial\*.phis; DestDir: {code:GetApacheDir}\htdocs\phitutorial; Components: apache; Check: CheckForTutorial(); Flags: replacesameversion
Source: phis1.dll; DestDir: {app}\bin; Components: server editor; Flags: 32bit replacesameversion restartreplace uninsnosharedfileprompt
Source: artephis.exe; DestDir: {app}\bin; Components: editor; Flags: 32bit replacesameversion restartreplace
Source: amphibia.exe; DestDir: {app}\bin; Components: browser; Flags: 32bit replacesameversion restartreplace
Source: phisconf.exe; DestDir: {app}\bin; Components: server; Flags: 32bit replacesameversion restartreplace
Source: phiapp.exe; DestDir: {app}\bin; Components: core; Flags: 32bit replacesameversion restartreplace uninsnosharedfileprompt
Source: mod_phi1.dll; DestDir: {code:GetApacheDir}\modules; Components: apache; DestName: mod_phi.so; Flags: replacesameversion 32bit

Source: pages\phitest.phis; DestDir: {code:GetDataDir}\default\{code:GetFQDN}; Components: editor; Flags: replacesameversion
Source: pages\phitestpost.phis; DestDir: {code:GetDataDir}\default\{code:GetFQDN}; Components: editor; Flags: replacesameversion
Source: README.txt; DestDir: {app}\doc; Components: core; Flags: isreadme ignoreversion
;Source: Readme-German.txt; DestName: Liesmich.txt; DestDir: {app}; Components: readme\de; Flags: isreadme

[Messages]
BeveledLabel=www.phisketeer.org

[Icons]
Name: {group}\Artephis editor; Filename: {app}\bin\artephis.exe; WorkingDir: {app}; IconFilename: {app}\bin\artephis.exe; Flags: createonlyiffileexists; Components: editor
Name: {group}\Amphibia browser; Filename: {app}\bin\amphibia.exe; WorkingDir: {app}; IconFilename: {app}\bin\amphibia.exe; Flags: createonlyiffileexists; Components: browser
Name: {group}\Phisketeer online; Filename: http://www.phisketeer.org
;Name: {group}\Getting started; Filename: http://www.phisketeerr.com/dev/phistart.phis; Components: editor
Name: {group}\Documentation; Filename: http://wiki.phisketeer.com; Components: editor browser
Name: {group}\Phi test page; Filename: http://{code:GetFQDN}:{code:getPort}/phitest.phis; Components: editor
;Name: {group}\Tutorial; Filename: http://localhost:{code:GetPort}/phitutorial/index.phis; Components: tutorial
Name: {group}\Uninstall\Phisketeer; Filename: {uninstallexe}
;Name: {group}\Phis\Clear Phis cache; Filename: {app}\bin\phis.exe; Parameters: -c; WorkingDir: {app}; IconFilename: {app}\bin\phiapp.exe; Flags: createonlyiffileexists useapppaths; Components: server
Name: {group}\Phis configuration; Filename: {app}\bin\phisconf.exe; WorkingDir: {app}; IconFilename: {app}\bin\phiapp.exe; Flags: createonlyiffileexists useapppaths; Components: server
Name: {commondesktop}\Artephis editor; Filename: {app}\bin\artephis.exe; WorkingDir: {app}; IconFilename: {app}\bin\artephis.exe; Flags: createonlyiffileexists useapppaths; Components: editor

[Dirs]
Name: {app}\bin; Components: core; Flags: uninsalwaysuninstall
Name: {code:GetDataDir}; Components: server; Flags: uninsalwaysuninstall
Name: {code:GetDataDir}\default; Components: server; Flags: uninsalwaysuninstall; Permissions: users-modify
Name: {code:GetDataDir}\default\log; Components: server; Flags: uninsalwaysuninstall
Name: {code:GetDataDir}\default\{code:GetFQDN}; Components: server; Flags: uninsalwaysuninstall
Name: {code:GetDataDir}\default\{code:GetFQDN}\phitutorial; Components: server; Check: CheckForTutorial(); Flags: uninsalwaysuninstall
;Name: {code:GetDataDir}\default\temp; Components: server; Flags: uninsalwaysuninstall

[Registry]
Root: HKLM; Subkey: SOFTWARE\Phisketeer\phis; ValueName: Phisketeer; ValueType: string; ValueData: 1; Components: core; Flags: uninsdeletekey
Root: HKLM; Subkey: SOFTWARE\Phisketeer\phis; ValueName: PluginsPath; ValueType: string; ValueData: {app}\bin\plugins; Components: core
Root: HKLM; Subkey: SOFTWARE\Phisketeer\phis; ValueName: BinDir; ValueType: string; ValueData: {app}\bin; Components: core
Root: HKLM; Subkey: SOFTWARE\Phisketeer\phis; ValueName: Path; ValueType: string; ValueData: {app}\bin\phiapp.exe; Components: core
Root: HKLM; Subkey: SOFTWARE\Phisketeer\phis; ValueName: RootDir; ValueType: string; ValueData: {code:GetDataDir}; Components: server
Root: HKLM; Subkey: SOFTWARE\Phisketeer\phis\default; ValueName: ListenerPort; ValueType: dword; ValueData: {code:GetPort}; Components: server
Root: HKLM; Subkey: SOFTWARE\Phisketeer\phis\default; ValueName: SSLEnabled; ValueType: string; ValueData: false; Components: server; Flags: createvalueifdoesntexist
Root: HKLM; Subkey: SOFTWARE\Phisketeer\phis\default; ValueName: Admin; ValueType: string; ValueData: {code:GetAdmin}; Components: server
Root: HKLM; Subkey: SOFTWARE\Phisketeer\phis\default\{code:GetFQDN}; ValueName: DocumentRoot; ValueType: string; ValueData: {code:GetDataDir}\default\{code:GetFQDN}; Components: server
Root: HKLM; Subkey: SOFTWARE\Phisketeer\phis\default\{code:GetFQDN}; ValueName: TempDir; ValueType: string; ValueData: {code:GetDataDir}\default\temp; Components: server
Root: HKLM; Subkey: SOFTWARE\Classes\CLSID\{{F4463E56-F3CA-45E4-93FE-B8F007376088}\Implemented Categories\{{7DD95801-9882-11CF-9FA9-00AA006C42C4}; ValueType: none; Flags: uninsdeletekey; Components: browser
Root: HKLM; Subkey: SOFTWARE\Classes\CLSID\{{F4463E56-F3CA-45E4-93FE-B8F007376088}\Implemented Categories\{{7DD95802-9882-11CF-9FA9-00AA006C42C4}; ValueType: none; Flags: uninsdeletekey; Components: browser

;Root: HKLM; Subkey: SOFTWARE\MozillaPlugins\@phisketeer.org/npphi,version=1.5.0; ValueType: none; Components: plugin; Flags: uninsdeletekey deletekey
;Root: HKLM; Subkey: SOFTWARE\MozillaPlugins\@phisketeer.org/npphi,version=1.5.0; ValueName: Path; ValueType: string; ValueData: {app}\bin\npphiplugin1.dll; Components: plugin
;Root: HKLM; Subkey: SOFTWARE\MozillaPlugins\@phisketeer.org/npphi,version=1.5.0; ValueName: Description; ValueType: string; ValueData: Phi plug-in V1.5.0; Components: plugin
;Root: HKLM; Subkey: SOFTWARE\MozillaPlugins\@phisketeer.org/npphi,version=1.5.0; ValueName: Vendor; ValueType: string; ValueData: Phisketeer; Components: plugin
;Root: HKLM; Subkey: SOFTWARE\MozillaPlugins\@phisketeer.org/npphi,version=1.5.0\MimeTypes\application/x-phi; ValueType: none; Components: plugin; Flags: uninsdeletekey deletekey
;Root: HKLM; Subkey: SOFTWARE\MozillaPlugins\@phisketeer.org/npphi,version=1.5.0\MimeTypes\application/x-phi; ValueName: Description; ValueType: string; ValueData: Phi plug-in; Components: plugin
;Root: HKLM; Subkey: SOFTWARE\MozillaPlugins\@phisketeer.org/npphi,version=1.5.0\MimeTypes\application/x-phi; ValueName: Suffixes; ValueType: string; ValueData: phi; Components: plugin

Root: HKCR; Subkey: .phis; ValueType: string; ValueName: ; ValueData: PhisketeerArtephis; Flags: uninsdeletevalue; Components: editor
Root: HKCR; Subkey: PhisketeerArtephis; ValueType: string; ValueName: ; ValueData: Phi server page; Flags: uninsdeletekey; Components: editor
Root: HKCR; Subkey: PhisketeerArtephis\DefaultIcon; ValueType: string; ValueData: {app}\bin\phiapp.exe,0; Components: editor
Root: HKCR; Subkey: PhisketeerArtephis\shell\open\command; ValueType: string; ValueName: ; ValueData: """{app}\bin\artephis.exe"" ""%1%"""; Components: editor

Root: HKCR; Subkey: .phi; ValueType: string; ValueName: ; ValueData: PhisketeerPhiApp; Flags: uninsdeletevalue; Components: core
Root: HKCR; Subkey: PhisketeerPhiApp; ValueType: string; ValueName: ; ValueData: Phi page; Flags: uninsdeletekey; Components: core
Root: HKCR; Subkey: PhisketeerPhiApp\DefaultIcon; ValueType: string; ValueData: {app}\bin\phiapp.exe,0; Components: core
Root: HKCR; Subkey: PhisketeerPhiApp\shell\open\command; ValueType: string; ValueName: ; ValueData: """{app}\bin\phiapp.exe"" ""%1%"""; Components: core

Root: HKCU; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\Ext\Stats\{{F4463E56-F3CA-45E4-93FE-B8F007376088}\iexplore\AllowedDomains\*; ValueType: none; Flags: uninsdeletevalue; Components: browser
;Root: HKCU; Subkey: SOFTWARE\Phisketeer\Artephis; ValueType: string; Flags: createvalueifdoesntexist; Components: editor; ValueName: LastFile; ValueData: {code:GetDataDir}\default\{code:GetFQDN}\phitest.phis
Root: HKCU; Subkey: SOFTWARE\Phisketeer\Artephis; ValueType: string; Flags: createvalueifdoesntexist; Components: editor; ValueName: LastOpenedDir; ValueData: {code:GetDataDir}\default\{code:GetFQDN}
Root: HKCU; Subkey: SOFTWARE\Phisketeer\Artephis; ValueType: string; Flags: createvalueifdoesntexist; Components: editor; ValueName: RootDir; ValueData: {code:GetDataDir}\default\{code:GetFQDN}

[Run]
Filename: {app}\bin\phis.exe; Description: Install Phis server as a service; Parameters: -i; WorkingDir: {app}\bin; StatusMsg: Installing the Phis service; Flags: postinstall runascurrentuser; Components: server
Filename: {app}\bin\phis.exe; Description: Run Phis server; WorkingDir: {app}\bin; StatusMsg: Starting the Phis service; Flags: postinstall runascurrentuser; Components: server
;Filename: {app}\bin\phis.exe; Description: Run Phis server automatically on system startup; WorkingDir: {app}\bin; StatusMsg: Setting up the Phis service; Flags: postinstall runascurrentuser; Components: server; Parameters: -c 2
;Filename: {app}\bin\idc.exe; Description: Register ActiveX control; Parameters: npphiplugin1.dll /regserver; WorkingDir: {app}\bin; StatusMsg: Registering ActiveX control for Phi; Flags: runascurrentuser; Components: core
;Filename: {sys}\setx; Description: Add the Phi library dir to PATH variable; Parameters: "PATH %PATH%;{app}\bin"; WorkingDir: {app}\bin; StatusMsg: Adding PATH variable; Flags: postinstall runascurrentuser skipifdoesntexist; Components: apache

[UninstallRun]
Filename: {app}\bin\phis.exe; Parameters: -t; WorkingDir: {app}\bin; StatusMsg: Uninstalling the Phis service; Components: server
Filename: {app}\bin\phis.exe; Parameters: -u; WorkingDir: {app}\bin; StatusMsg: Uninstalling the Phis service; Components: server
;Filename: {app}\bin\idc.exe; Parameters: npphiplugin1.dll /unregserver; WorkingDir: {app}\bin; StatusMsg: Unregistering the ActiveX control; Components: core

[Code]
var
	DataDirPage: TInputDirWizardPage;
	PhisPage: TInputQueryWizardPage;
	ApacheDirPage: TInputDirWizardPage;
	ApacheOutPage: TOutputMsgMemoWizardPage;
	Buffer: String;
	Number: Cardinal;
	List: TArrayOfString;

procedure InitializeWizard;
begin
	DataDirPage := CreateInputDirPage( wpSelectComponents,
		'Select data directory', 'Where should the Phis server data files be installed?',
		'Select the folder in which Setup should install personal data files, then click Next.',
		False, '');
	PhisPage := CreateInputQueryPage( DataDirPage.ID,
		'Setup Phis server', 'What are the Phis server settings?',
		'Enter the port, full qualified domain name and administrator email, then click Next.' );
	ApacheDirPage := CreateInputDirPage( PhisPage.ID,
		'Select Apache root directory', 'Where should the Phi module be installed?',
		'Select the folder in which Setup should install the Phi files for Apache (exclude the module dir), then click Next.',
		False, '');

	Buffer := ExpandConstant( '{pf}\Phisketeer\bin' );
	ApacheOutPage := CreateOutputMsgMemoPage( ApacheDirPage.ID,
		'Apache Phi module information', 'What do I need to change to get the module running?',
		'Please read the following important information on how to configure Apache, then click Next.',
		'In httpd.conf add the following lines:' + #13#10#13#10 + 'LoadModule phi_module modules/mod_phi.so'
                + #13#10 + '<IfModule phi_module>' + #13#10 + 'AddHandler phi .phis' + #13#10 + 'DirectoryIndex index.phis'
		+ #13#10 + 'AddType application/x-phi phi' + #13#10 + '</IfModule>' + #13#10#13#10 + 'Add the Phi libraries directory '
		+ '(ie.' + Buffer + ') to the system PATH environment variable, that they can be located by Apache.'
		);

	DataDirPage.Add('Data dir:');
	if RegQueryStringValue(HKLM, 'SOFTWARE\Phisketeer\phis',
			'RootDir', Buffer) then
		DataDirPage.Values[0] := Buffer
	else
		DataDirPage.Values[0] := ExpandConstant( '{pf}\Phisketeer\phis' );

	PhisPage.Add('Port:', False);
	PhisPage.Add('Full qualified server name:', False);
	PhisPage.Add('Admin email:', False);

	if RegQueryDWordValue(HKLM, 'SOFTWARE\Phisketeer\phis\default',
			'ListenerPort', Number) then
		PhisPage.Values[0] := IntToStr( Number )
	else
		PhisPage.Values[0] := '8080';

	if RegGetSubkeyNames(HKLM, 'SOFTWARE\Phisketeer\phis\default', List) then
		if ( GetArrayLength( List )>0 ) then
			PhisPage.Values[1] := List[0]
		else
			PhisPage.Values[1] := 'localhost'
	else
		PhisPage.Values[1] := 'localhost';

	if RegQueryStringValue(HKLM, 'SOFTWARE\Phisketeer\phis\default',
			'Admin', Buffer) then
		PhisPage.Values[2] := Buffer
	else
		PhisPage.Values[2] := 'admin@localhost';

	ApacheDirPage.Add('Apache root dir:');
	if RegGetSubkeyNames(HKLM, 'SOFTWARE\Apache Software Foundation\Apache', List) then
	begin
		Buffer := 'SOFTWARE\Apache Software Foundation\Apache\'+List[0];
		if RegQueryStringValue(HKLM, Buffer, 'ServerRoot', Buffer) then
			ApacheDirPage.Values[0] := Buffer
		else
			ApacheDirPage.Values[0] := ExpandConstant( '{pf}\Apache Software Foundation\Apache' )
	end
	else
		ApacheDirPage.Values[0] := ExpandConstant( '{pf}' );

end;

function ShouldSkipPage(PageID: Integer): Boolean;
begin
	{ Skip pages that shouldn't be shown }
	if ( PageID = DataDirPage.ID ) or ( PageID = PhisPage.ID ) then
		if isComponentSelected( 'server' ) then
			Result := False
		else
			Result := True
	else if ( PageID = ApacheDirPage.ID ) or ( PageID = ApacheOutPage.ID ) then
		if isComponentSelected( 'apache' ) then
			Result := False
		else
			Result := True
	else
		Result := False;
end;

function GetDataDir(Param: String): String;
begin
  { Return the selected DataDir }
  Result := DataDirPage.Values[0];
end;

function GetPort(Param: String): String;
begin
  { Return the selected RootDir }
  Result := PhisPage.Values[0];
end;

function GetFQDN(Param: String): String;
begin
  Result := PhisPage.Values[1];
end;

function GetAdmin(Param: String): String;
begin
  Result := PhisPage.Values[2];
end;

function GetApacheDir(Param: String): String;
begin
  Result := ApacheDirPage.Values[0];
end;

function CheckForTutorial() : Boolean;
begin
  if IsComponentSelected( 'tutorial' ) then
	Result := true
  else
	Result := false
end;
