[Setup]
AppName=CherryGrove
AppId=CherryGrove
AppVersion=0.0.1
AppPublisher=CherryGrove Contributors
AppCopyright=Copyright (c) 2026 CherryGrove Contributors
DefaultDirName={userappdata}\CherryGrove
DefaultGroupName=CherryGrove
AllowNoIcons=yes
DisableProgramGroupPage=no
OutputBaseFilename=CherryGrove_windows_{#Arch}
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
CreateUninstallRegKey=no

[LangOptions]
DialogFontSize=10
WelcomeFontSize=16

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
Source: "..\..\out\windows-{#Arch}-release\CherryGrove.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\LICENSE"; DestDir: "{app}"; DestName: "{cm:LicenseFileName}.txt"; Flags: ignoreversion

;Readme files
Source: "..\readmes\README.txt"; DestDir: "{app}"; DestName: "README.txt"; Languages: english; Flags: ignoreversion
Source: "..\readmes\必读信息.txt"; DestDir: "{app}"; DestName: "必读信息.txt"; Languages: chinesesimplified; Flags: ignoreversion
;end

Source: "..\..\assets\*"; DestDir: "{app}\assets"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\..\packs\*"; DestDir: "{app}\packs"; Flags: skipifsourcedoesntexist ignoreversion recursesubdirs createallsubdirs
Source: "..\..\saves\*"; DestDir: "{app}\saves"; Flags: skipifsourcedoesntexist ignoreversion recursesubdirs createallsubdirs
Source: "..\..\tests\*"; DestDir: "{app}\tests"; Flags: skipifsourcedoesntexist ignoreversion recursesubdirs createallsubdirs
Source: "..\..\settings.json"; DestDir: "{app}"; Flags: skipifsourcedoesntexist ignoreversion onlyifdoesntexist

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
function InitializeSetup(): Boolean;
var
  buttonLabels: TArrayOfString;
begin
    SetArrayLength(buttonLabels, 0);
    if ActiveLanguage = 'chinesesimplified' then
    begin
        BringToFrontAndRestore;
        TaskDialogMsgBox(
            '重要告示',
            'CherryGrove 是源代码可用软件，供个人完全免费使用，并拥有独立完整的知识产权。' + #13#10 + #13#10 + '如果你付费获得该安装包，请立即差评并举报店铺。' + #13#10 + #13#10 + '如果你在某些平台看到【未明确说明本软件名称的】借用本软件的【商业性推广】，请在评论区等大胆留言质疑。' + #13#10 + #13#10 + '让我们一起坚决抵制这些自由开放软件精神和生态的破坏者。' + #13#10 + #13#10 + '本告示不构成任何条款或法律文本。请在此处查阅完整使用条款：https://docs.cherrygrove.dev/legal#terms-of-use。',
            mbError,
            MB_OK,
            buttonLabels,
            0
        );
    end;
    Result := True;
end;