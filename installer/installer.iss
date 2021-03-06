; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{D96B7DA2-42A0-41A7-9F61-6AAC620C06EE}
AppName=Jumail
AppVersion=1.0
;AppVerName=Jumail 1.0
AppPublisherURL=http://www.esgi.fr
AppSupportURL=http://www.esgi.fr
AppUpdatesURL=http://www.esgi.fr
DefaultDirName={pf}\Jumail
DefaultGroupName=Jumail
AllowNoIcons=yes
OutputDir=F:\Eclipse\git\jumail\installer
OutputBaseFilename=jumail_setup
SetupIconFile=F:\Eclipse\git\jumail\installer\emblem-mail.ico
Compression=lzma
SolidCompression=yes

[Languages]
Name: "french"; MessagesFile: "compiler:Languages\French.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "F:\Eclipse\git\jumail\Debug\Jumail.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "F:\Eclipse\git\jumail\resources\MainWindow.glade"; DestDir: "{app}\resources"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:\msys64\mingw64\share\icons\Adwaita\16x16\actions\help-about.png"; DestDir: "{app}\share\icons\Adwaita\16x16\actions"; Flags: ignoreversion
Source: "C:\msys64\mingw64\share\icons\Adwaita\16x16\actions\mail-message-new.png"; DestDir: "{app}\share\icons\Adwaita\16x16\actions"; Flags: ignoreversion
Source: "C:\msys64\mingw64\share\icons\Adwaita\16x16\actions\document-open.png"; DestDir: "{app}\share\icons\Adwaita\16x16\actions"; Flags: ignoreversion
Source: "C:\msys64\mingw64\share\icons\Adwaita\24x24\actions\edit-delete.png"; DestDir: "{app}\share\icons\Adwaita\24x24\actions"; Flags: ignoreversion
Source: "C:\msys64\mingw64\share\icons\Adwaita\24x24\actions\mail-reply-sender.png"; DestDir: "{app}\share\icons\Adwaita\24x24\actions"; Flags: ignoreversion
Source: "C:\msys64\mingw64\share\icons\Adwaita\16x16\actions\edit-delete.png"; DestDir: "{app}\share\icons\Adwaita\16x16\actions"; Flags: ignoreversion
Source: "C:\msys64\mingw64\share\icons\Adwaita\24x24\actions\document-save.png"; DestDir: "{app}\share\icons\Adwaita\24x24\actions"; Flags: ignoreversion
Source: "C:\msys64\mingw64\share\icons\Adwaita\16x16\actions\document-save.png"; DestDir: "{app}\share\icons\Adwaita\16x16\actions"; Flags: ignoreversion
Source: "C:\msys64\mingw64\share\icons\Adwaita\32x32\actions\mail-mark-unread.png"; DestDir: "{app}\share\icons\Adwaita\32x32\actions"; Flags: ignoreversion
Source: "C:\msys64\mingw64\share\icons\Adwaita\16x16\actions\view-refresh.png"; DestDir: "{app}\share\icons\Adwaita\16x16\actions"; Flags: ignoreversion
Source: "C:\msys64\mingw64\share\icons\Adwaita\16x16\actions\folder-new.png"; DestDir: "{app}\share\icons\Adwaita\16x16\actions"; Flags: ignoreversion
Source: "C:\msys64\mingw64\share\icons\Adwaita\16x16\actions\list-add.png"; DestDir: "{app}\share\icons\Adwaita\16x16\actions"; Flags: ignoreversion
Source: "C:\msys64\mingw64\share\icons\Adwaita\16x16\actions\go-next.png"; DestDir: "{app}\share\icons\Adwaita\16x16\actions"; Flags: ignoreversion
Source: "C:\msys64\mingw64\share\icons\Adwaita\16x16\actions\go-previous.png"; DestDir: "{app}\share\icons\Adwaita\16x16\actions"; Flags: ignoreversion
Source: "C:\msys64\mingw64\share\icons\Adwaita\16x16\actions\application-exit.png"; DestDir: "{app}\share\icons\Adwaita\16x16\actions"; Flags: ignoreversion
Source: "C:\msys64\mingw64\lib\gdk-pixbuf-2.0\2.10.0\*"; DestDir: "{app}\lib\gdk-pixbuf-2.0\2.10.0"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:\msys64\mingw64\bin\*.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "F:\Eclipse\git\jumail\installer\emblem-mail.ico"; DestDir: "{app}";
Source: "C:\msys64\mingw64\ssl\certs\ca-bundle.crt"; DestDir: "{app}\ssl\certs\"; Flags: recursesubdirs createallsubdirs 
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\Jumail"; Filename: "{app}\Jumail.exe"
Name: "{commondesktop}\Jumail"; Filename: "{app}\Jumail.exe"; IconFilename: "{app}\emblem-mail.ico"; Tasks: desktopicon

[Run]
Filename: "{app}\Jumail.exe"; Description: "{cm:LaunchProgram,Jumail}"; Flags: nowait postinstall skipifsilent

