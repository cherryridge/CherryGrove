!include "MUI2.nsh"

SetCompressor /SOLID lzma
RequestExecutionLevel admin

!define VERSION "0.0.1"

!define MUI_ICON "..\assets\icons\CherryGrove-trs.ico"
!define MUI_UNICON "..\assets\icons\CherryGrove-trs.ico"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "..\assets\icons\CherryGrove-header.bmp"
!define MUI_HEADERIMAGE_BITMAP_STRETCH "AspectFitHeight"
!define MUI_WELCOMEFINISHPAGE_BITMAP "..\assets\icons\CherryGrove-page.bmp"
!define MUI_WELCOMEFINISHPAGE_BITMAP_STRETCH "NoStretchNoCropNoAlign"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "..\assets\icons\CherryGrove-page.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP_STRETCH "NoStretchNoCropNoAlign"

!define MUI_ABORTWARNING

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\LICENSE"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH
!insertmacro MUI_LANGUAGE "English"

#region Branding
OutFile "..\build\x64\CherryGrove_setup_en_x64.exe"
InstallDir "$PROGRAMFILES64\CherryGrove"
Name "CherryGrove"
Caption "CherryGrove Installer"
BrandingText "CherryGrove (c) 2024 LJM12914"

VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "CherryGrove"
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "LJM12914"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "(c) 2024 LJM12914"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "CherryGrove Installer"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${VERSION}.0"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductVersion" "${VERSION}.0"
VIProductVersion "${VERSION}.0"
#endregion

Section "MainSection" SEC01
    SetOutPath "$INSTDIR"
    File "..\build\x64\Release\CherryGrove.exe"
    SetOutPath "$INSTDIR\shaders"
    File /r "..\shaders\*"
    SetOutPath "$INSTDIR\assets"
    File /r "..\assets\*"
    SetOutPath "$INSTDIR"
    CreateShortcut "$DESKTOP\CherryGrove.lnk" "$INSTDIR\CherryGrove.exe"
    CreateShortcut "$SMPROGRAMS\CherryGrove.lnk" "$INSTDIR\CherryGrove.exe"
    WriteUninstaller "$INSTDIR\uninstall.exe"

    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CherryGrove" "DisplayName" "CherryGrove"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CherryGrove" "DisplayVersion" "${VERSION}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CherryGrove" "Publisher" "LJM12914"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CherryGrove" "InstallLocation" "$INSTDIR"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CherryGrove" "UninstallString" '"$INSTDIR\uninstall.exe"'
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CherryGrove" "DisplayIcon" "$INSTDIR\CherryGrove.exe"
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CherryGrove" "NoModify" 1
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CherryGrove" "NoRepair" 1
SectionEnd

Section "Uninstall"
    RMDir /r "$INSTDIR"
    Delete "$DESKTOP\CherryGrove.lnk"
    Delete "$SMPROGRAMS\CherryGrove.lnk"
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CherryGrove"
SectionEnd