[Setup]
AppName=CherryGrove
AppId=CherryGrove
AppVersion=0.0.1
AppPublisher=CherryGrove Contributors
AppCopyright=Copyright (c) 2026 CherryGrove Contributors
DefaultDirName={userappdata}\CherryGrove
DefaultGroupName=CherryGrove
OutputBaseFilename=CherryGrove_setup_win64
OutputDir=.
SetupIconFile=..\..\assets\icons\CherryGrove-trs.ico
UninstallDisplayIcon={app}\CherryGrove.exe
WizardImageFile=side.bmp
WizardSmallImageFile=header.bmp
LicenseFile=..\..\LICENSE
PrivilegesRequired=none
Compression=lzma/max
SolidCompression=yes
DisableWelcomePage=no
DisableFinishedPage=no
ShowLanguageDialog=auto
LanguageDetectionMethod=uilanguage
UsePreviousTasks=no
WizardStyle=modern
WizardSizePercent=150,150
WizardResizable=no
CreateUninstallRegKey=no

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl";
Name: "chinesesimplified"; MessagesFile: "translations\chs\ChineseSimplified.isl";

[Messages]
english.SetupWindowTitle=CherryGrove Setup
chinesesimplified.SetupWindowTitle=CherryGrove 安装程序
english.FinishedLabel=CherryGrove has been successfully installed on your computer.%n%nVisit our website for more information: https://cherrygrove.dev.
chinesesimplified.FinishedLabel=CherryGrove 已成功安装到您的计算机上。%n%n访问我们的官方网站以获取更多信息：https://cherrygrove.dev。

[CustomMessages]
;English
english.Title=CherryGrove Setup
english.FinishRunText=Launch CherryGrove
english.FinishDesktopText=Create a desktop shortcut
english.FinishLinkText=Visit CherryGrove website
english.LicenseFileName=LICENSE
english.AdditionalIcons=Additional Icons:
english.SystemIntegration=System integration:
english.WriteRegistryText=Write to Windows registry (If you've installed CherryGrove, this option will override the previous installation in the "Applications and Features" list.)

;Simplified Chinese
chinesesimplified.Title=CherryGrove 安装程序
chinesesimplified.FinishRunText=立即运行 CherryGrove
chinesesimplified.FinishDesktopText=创建桌面快捷方式
chinesesimplified.FinishLinkText=访问 CherryGrove 官网
chinesesimplified.LicenseFileName=许可证
chinesesimplified.AdditionalIcons=附加图标：
chinesesimplified.SystemIntegration=系统集成：
chinesesimplified.WriteRegistryText=写入 Windows 注册表（如果之前已经安装过，系统设置“应用和功能”列表中之前的安装信息将被覆盖。）

[Files]
Source: "..\..\out\windows-x64-release\CherryGrove.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\LICENSE"; DestDir: "{app}"; DestName: "{cm:LicenseFileName}.txt"; Flags: ignoreversion

;Readme files
Source: "..\readmes\README.txt"; DestDir: "{app}"; DestName: "README.txt"; Languages: english; Flags: ignoreversion
Source: "..\readmes\必读信息.txt"; DestDir: "{app}"; DestName: "必读信息.txt"; Languages: chinesesimplified; Flags: ignoreversion
;end

Source: "..\..\assets\*"; DestDir: "{app}\assets"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\..\out\windows-x64-release\shaders\*"; DestDir: "{app}\shaders"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\..\test\*"; DestDir: "{app}\test"; Flags: ignoreversion recursesubdirs createallsubdirs

[Dirs]
Name: "{app}\captures"
Name: "{app}\packs"
Name: "{app}\saves"

[Icons]
;Start Menu shortcut
Name: "{group}\CherryGrove"; Filename: "{app}\CherryGrove.exe"

;Desktop shortcut (optional, controlled by user choice)
Name: "{autodesktop}\CherryGrove"; Filename: "{app}\CherryGrove.exe"; Tasks: desktopicon

[Tasks]
Name: "desktopicon"; Description: "{cm:FinishDesktopText}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "writeregistry"; Description: "{cm:WriteRegistryText}"; GroupDescription: "{cm:SystemIntegration}"

[Registry]
Root: HKLM; Subkey: "Software\Microsoft\Windows\CurrentVersion\Uninstall\CherryGrove"; ValueType: string; ValueName: "DisplayName"; ValueData: "CherryGrove"; Tasks: writeregistry
Root: HKLM; Subkey: "Software\Microsoft\Windows\CurrentVersion\Uninstall\CherryGrove"; ValueType: string; ValueName: "DisplayVersion"; ValueData: "0.0.1"; Tasks: writeregistry
Root: HKLM; Subkey: "Software\Microsoft\Windows\CurrentVersion\Uninstall\CherryGrove"; ValueType: string; ValueName: "Publisher"; ValueData: "CherryGrove Contributors"; Tasks: writeregistry
Root: HKLM; Subkey: "Software\Microsoft\Windows\CurrentVersion\Uninstall\CherryGrove"; ValueType: string; ValueName: "InstallLocation"; ValueData: "{app}"; Tasks: writeregistry
Root: HKLM; Subkey: "Software\Microsoft\Windows\CurrentVersion\Uninstall\CherryGrove"; ValueType: string; ValueName: "UninstallString"; ValueData: """{uninstallexe}"""; Tasks: writeregistry
Root: HKLM; Subkey: "Software\Microsoft\Windows\CurrentVersion\Uninstall\CherryGrove"; ValueType: string; ValueName: "DisplayIcon"; ValueData: "{app}\CherryGrove.exe"; Tasks: writeregistry
Root: HKLM; Subkey: "Software\Microsoft\Windows\CurrentVersion\Uninstall\CherryGrove"; ValueType: dword; ValueName: "NoModify"; ValueData: 1; Tasks: writeregistry
Root: HKLM; Subkey: "Software\Microsoft\Windows\CurrentVersion\Uninstall\CherryGrove"; ValueType: dword; ValueName: "NoRepair"; ValueData: 1; Tasks: writeregistry

[Run]
Filename: "{app}\CherryGrove.exe"; Description: "{cm:FinishRunText}"; Flags: nowait postinstall skipifsilent unchecked

[UninstallDelete]
Type: filesandordirs; Name: "{app}"
Type: files; Name: "{group}\CherryGrove.lnk"
Type: files; Name: "{autodesktop}\CherryGrove.lnk"

[Code]
const
    SWP_NOSIZE     = $0001;
    SWP_NOMOVE     = $0002;
    SWP_NOACTIVATE = $0010;
    HWND_TOPMOST   = -1;
    HWND_NOTOPMOST = -2;

function SetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags: Integer): Boolean;
    external 'SetWindowPos@user32.dll stdcall';

procedure BumpWizardToFront;
begin
    SetWindowPos(WizardForm.Handle, HWND_TOPMOST,    0,0,0,0, SWP_NOMOVE or SWP_NOSIZE or SWP_NOACTIVATE);
    SetWindowPos(WizardForm.Handle, HWND_NOTOPMOST,  0,0,0,0, SWP_NOMOVE or SWP_NOSIZE or SWP_NOACTIVATE);
  BringToFrontAndRestore;
end;
procedure InitializeWizard();
begin
    BringToFrontAndRestore;
    WizardForm.BringToFront;
end;
procedure CurPageChanged(CurPageID :Integer);
begin
    BringToFrontAndRestore;
end;
function InitializeSetup(): Boolean;
begin
    if ActiveLanguage = 'chinesesimplified' then
    begin
        MsgBox('CherryGrove 是源代码可用软件，供个人完全免费使用，并拥有独立完整的知识产权。' + #13#10 + #13#10 + '如果你付费获得该安装包，请立即差评并举报店铺。' + #13#10 + #13#10 + '如果你在某些平台看到【未明确说明本软件名称的】借用本软件的【商业性推广】，请在评论区等大胆留言质疑。' + #13#10 + #13#10 + '让我们一起坚决抵制这些自由开放软件精神和生态的破坏者。' + #13#10 + #13#10 + '本告示不构成任何条款或法律文本。请在此处查阅完整使用条款：https://docs.cherrygrove.dev/legal#terms-of-use。', mbError, MB_OK);
    end;
    Result := True;
end;