Unicode true
!include "MUI2.nsh"

SetCompressor /SOLID lzma
RequestExecutionLevel admin

!define VERSION "0.0.1"

!define MUI_ICON "..\..\assets\icons\CherryGrove-trs.ico"
!define MUI_UNICON "..\..\assets\icons\CherryGrove-trs.ico"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "CherryGrove-header.bmp"
!define MUI_HEADERIMAGE_BITMAP_STRETCH "AspectFitHeight"
!define MUI_WELCOMEFINISHPAGE_BITMAP "CherryGrove-page.bmp"
!define MUI_WELCOMEFINISHPAGE_BITMAP_STRETCH "NoStretchNoCropNoAlign"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "CherryGrove-page.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP_STRETCH "NoStretchNoCropNoAlign"

!define MUI_ABORTWARNING
!define MUI_FINISHPAGE_NOAUTOCLOSE

!define MUI_LICENSEPAGE_CHECKBOX
!define MUI_LICENSEPAGE_CHECKBOX_TEXT "I have read and agree to the terms of the License Agreement."#DIF
!define MUI_LICENSEPAGE_BUTTON "Next >"                                                            #DIF

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\..\LICENSE"




!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!define MUI_FINISHPAGE_RUN $INSTDIR\CherryGrove.exe
!define MUI_FINISHPAGE_RUN_TEXT "Run CherryGrove"                                                  #DIF
!define MUI_FINISHPAGE_RUN_NOTCHECKED
!define MUI_FINISHPAGE_SHOWREADME ""
!define MUI_FINISHPAGE_SHOWREADME_NOTCHECKED
!define MUI_FINISHPAGE_SHOWREADME_TEXT "Create Desktop Shortcut"                                   #DIF
!define MUI_FINISHPAGE_SHOWREADME_FUNCTION desktopshortcut
!define MUI_FINISHPAGE_LINK "CherryGrove's Official Website"                                       #DIF
!define MUI_FINISHPAGE_LINK_LOCATION https://cherrygrove.dev
!define MUI_FINISHPAGE_LINK_COLOR 1879e7
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH
!insertmacro MUI_LANGUAGE "English"                                                                #DIF

Function desktopshortcut
    CreateShortcut "$DESKTOP\CherryGrove.lnk" "$INSTDIR\CherryGrove.exe"
FunctionEnd

#region Branding
OutFile "CherryGrove_setup_en_win64.exe"                                                           #DIF
InstallDir "$APPDATA\CherryGrove"
Name "CherryGrove"
Caption "CherryGrove Installer"                                                                    #DIF
BrandingText "CherryGrove (c) 2025 LJM12914"

VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "CherryGrove"                                  #DIF
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "LJM12914"                                     #DIF
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "(c) 2025 LJM12914"                         #DIF
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "CherryGrove Installer"                    #DIF
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${VERSION}.0"                                 #DIF
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductVersion" "${VERSION}.0"                              #DIF
VIProductVersion "${VERSION}.0"
#endregion

Section "MainSection" SEC01
    SetOutPath "$INSTDIR"
    File "..\..\LICENSE"
    File "..\readmes\README.txt"                                                                   #DIF
    File "..\..\out\windows-x64-release\Release\CherryGrove.exe"
    SetOutPath "$INSTDIR\assets"
    File /r "..\..\assets\*"
    SetOutPath "$INSTDIR\captures"
    SetOutPath "$INSTDIR\packs"
    SetOutPath "$INSTDIR\saves"
    SetOutPath "$INSTDIR\shaders"
    File /r "..\..\out\windows-x64-release\Release\shaders\*"
    SetOutPath "$INSTDIR\test"
    File /r "..\..\test\*"

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