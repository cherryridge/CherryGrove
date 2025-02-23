Unicode true
!include "MUI2.nsh"

SetCompressor /SOLID lzma
RequestExecutionLevel admin

!define VERSION "0.0.1"

!define MUI_ICON "..\..\assets\icons\CherryGrove-trs.ico"
!define MUI_UNICON "..\..\assets\icons\CherryGrove-trs.ico"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "..\..\packing_resources\CherryGrove-header.bmp"
!define MUI_HEADERIMAGE_BITMAP_STRETCH "AspectFitHeight"
!define MUI_WELCOMEFINISHPAGE_BITMAP "..\..\packing_resources\CherryGrove-page.bmp"
!define MUI_WELCOMEFINISHPAGE_BITMAP_STRETCH "NoStretchNoCropNoAlign"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "..\..\packing_resources\CherryGrove-page.bmp"
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
OutFile "..\..\build\x64\CherryGrove_setup_en_x64.exe"                                                #DIF
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
    FileOpen $2 "README.txt" w                                                                     #DIF
    ${If} $2 != ""
        FileWrite $2 "Folder Introduction:$\r$\n"                                                  #DIF
        FileWrite $2 "assets: Stores assets necessary for CherryGrove to run, including fonts and logos. Not for adding packs.$\r$\n"#DIF
        FileWrite $2 "packs: Stores content packs. Here is the place to add packs to CherryGrove, in format of folder or .zip archive. Ensure that manifest.json of the pack is one folder away from packs folder.$\r$\n"#DIF
        FileWrite $2 "saves: Stores saves.$\r$\n"                                                  #DIF
        FileWrite $2 "captures: Stores screenshots and screen recordings.$\r$\n"                   #DIF
        FileWrite $2 "shaders: Stores shaders necessary for CherryGrove to run. Not for adding shader packs. Add them to packs folder instead.$\r$\n"#DIF
        FileWrite $2 "test (may not exist): Stores test files for the development version of CherryGrove.$\r$\n"#DIF
        FileWrite $2 "$\r$\n"
        FileWrite $2 "File Introduction:$\r$\n"                                                    #DIF
        FileWrite $2 "CherryGrove.exe: CherryGrove program.$\r$\n"                                 #DIF
        FileWrite $2 "uninstall.exe: Program for uninstalling CherryGrove.$\r$\n"                  #DIF
        FileWrite $2 "$\r$\n"
        FileWrite $2 "For more information, please visit our official website: https://cherrygrove.dev.$\r$\n"#DIF
        FileWrite $2 "This software is open source and is licensed under GPL-3.0-or-later.$\r$\n"  #DIF
        FileClose $2
    ${EndIf}
    File "..\..\LICENSE"
    File "..\..\build\x64\Release\CherryGrove.exe"
    File "..\..\packing_resources\VC_redist.x64.exe"
    SetOutPath "$INSTDIR\assets"
    File /r "..\..\assets\*"
    SetOutPath "$INSTDIR\captures"
    SetOutPath "$INSTDIR\packs"
    SetOutPath "$INSTDIR\saves"
    SetOutPath "$INSTDIR\shaders"
    File /r "..\..\shaders\*"
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
    
    ExecWait '"$INSTDIR\VC_redist.x64.exe" /quiet /norestart'
    Delete "$INSTDIR\VC_redist.x64.exe"
SectionEnd

Section "Uninstall"
    RMDir /r "$INSTDIR"
    Delete "$DESKTOP\CherryGrove.lnk"
    Delete "$SMPROGRAMS\CherryGrove.lnk"
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CherryGrove"
SectionEnd