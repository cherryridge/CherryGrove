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
!define MUI_LICENSEPAGE_CHECKBOX_TEXT "我已阅读并同意许可条款"                                                #DIF
!define MUI_LICENSEPAGE_BUTTON "下一步(N) >"                                                          #DIF

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\..\LICENSE"
Function warning                                                                                   #DIF
    MessageBox MB_OK "注意：$\r$\nCherryGrove【完全免费开源】，并拥有独立的知识产权。$\r$\n如果你付费获得该软件，请立即差评并举报店铺；$\r$\n如果你在某些平台看到【未明确说明本软件名称的】借用本软件的【商业性推广】，请在评论区等大胆发言。$\r$\n让我们一起坚决抵制这些自由开源精神和生态的破坏者。"#DIF
FunctionEnd                                                                                        #DIF
!define MUI_PAGE_CUSTOMFUNCTION_PRE warning                                                        #DIF
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!define MUI_FINISHPAGE_RUN $INSTDIR\CherryGrove.exe
!define MUI_FINISHPAGE_RUN_TEXT "运行 CherryGrove"                                                   #DIF
!define MUI_FINISHPAGE_RUN_NOTCHECKED
!define MUI_FINISHPAGE_SHOWREADME ""
!define MUI_FINISHPAGE_SHOWREADME_NOTCHECKED
!define MUI_FINISHPAGE_SHOWREADME_TEXT "创建桌面快捷方式"                                                  #DIF
!define MUI_FINISHPAGE_SHOWREADME_FUNCTION desktopshortcut
!define MUI_FINISHPAGE_LINK "访问 CherryGrove 官网"                                                    #DIF
!define MUI_FINISHPAGE_LINK_LOCATION https://cherrygrove.dev
!define MUI_FINISHPAGE_LINK_COLOR 1879e7
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH
!insertmacro MUI_LANGUAGE "SimpChinese"                                                            #DIF

Function desktopshortcut
    CreateShortcut "$DESKTOP\CherryGrove.lnk" "$INSTDIR\CherryGrove.exe"
FunctionEnd

#region Branding
OutFile "CherryGrove_setup_chs_win64.exe"                                                          #DIF
InstallDir "$APPDATA\CherryGrove"
Name "CherryGrove"
Caption "CherryGrove 安装程序"                                                                         #DIF
BrandingText "CherryGrove (c) 2025 LJM12914"

VIAddVersionKey /LANG=${LANG_SimpChinese} "ProductName" "CherryGrove"                              #DIF
VIAddVersionKey /LANG=${LANG_SimpChinese} "CompanyName" "LJM12914"                                 #DIF
VIAddVersionKey /LANG=${LANG_SimpChinese} "LegalCopyright" "(c) 2025 LJM12914"                     #DIF
VIAddVersionKey /LANG=${LANG_SimpChinese} "FileDescription" "CherryGrove 安装程序"                     #DIF
VIAddVersionKey /LANG=${LANG_SimpChinese} "FileVersion" "${VERSION}.0"                             #DIF
VIAddVersionKey /LANG=${LANG_SimpChinese} "ProductVersion" "${VERSION}.0"                          #DIF
VIProductVersion "${VERSION}.0"
#endregion

Section "MainSection" SEC01
    SetOutPath "$INSTDIR"
    File /oname=许可证.txt "..\..\LICENSE"                                                            #DIF
    File "..\readmes\必读信息.txt"                                                                     #DIF
    File "..\..\out\windows-x64-release\CherryGrove.exe"
    SetOutPath "$INSTDIR\assets"
    File /r "..\..\assets\*"
    SetOutPath "$INSTDIR\captures"
    SetOutPath "$INSTDIR\packs"
    SetOutPath "$INSTDIR\saves"
    SetOutPath "$INSTDIR\shaders"
    File /r "..\..\out\windows-x64-release\shaders\*"
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