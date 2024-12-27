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
!define MUI_LICENSEPAGE_CHECKBOX_TEXT "我已阅读并同意许可条款"                                                #DIF
!define MUI_LICENSEPAGE_BUTTON "下一步(N) >"                                                          #DIF

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\..\LICENSE"
Function warning                                                                            #DIF
    MessageBox MB_OK "注意：$\r$\nCherryGrove【完全免费开源】，并拥有独立的知识产权。$\r$\n如果你付费获得该软件，请立即差评并举报店铺；$\r$\n如果你在某些平台看到【未明确说明本软件名称的】借用本软件的【商业性推广】，请在评论区等大胆发言。$\r$\n让我们一起坚决抵制这些自由开源精神和生态的破坏者。"#DIF
FunctionEnd                                                                                        #DIF
!define MUI_PAGE_CUSTOMFUNCTION_PRE warning
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
OutFile "..\..\build\x64\CherryGrove_setup_chs_x64.exe"                                               #DIF
InstallDir "$APPDATA\CherryGrove"
Name "CherryGrove"
Caption "CherryGrove 安装程序"                                                                         #DIF
BrandingText "CherryGrove (c) 2024 LJM12914"

VIAddVersionKey /LANG=${LANG_SimpChinese} "ProductName" "CherryGrove"                              #DIF
VIAddVersionKey /LANG=${LANG_SimpChinese} "CompanyName" "LJM12914"                                 #DIF
VIAddVersionKey /LANG=${LANG_SimpChinese} "LegalCopyright" "(c) 2024 LJM12914"                     #DIF
VIAddVersionKey /LANG=${LANG_SimpChinese} "FileDescription" "CherryGrove 安装程序"                     #DIF
VIAddVersionKey /LANG=${LANG_SimpChinese} "FileVersion" "${VERSION}.0"                             #DIF
VIAddVersionKey /LANG=${LANG_SimpChinese} "ProductVersion" "${VERSION}.0"                          #DIF
VIProductVersion "${VERSION}.0"
#endregion

Section "MainSection" SEC01
    SetOutPath "$INSTDIR"
    FileOpen $2 "必读信息.txt" w                                                                       #DIF
    ${If} $2 != ""
        FileWrite $2 "文件夹介绍：$\r$\n"                                                                #DIF
        FileWrite $2 "assets：存放 CherryGrove 基本运行所需要的文件，包括字体、Logo 等。不是用来放置内容包的地方。$\r$\n"            #DIF
        FileWrite $2 "packs：存放内容包的地方，也是普通用户唯一需要修改的地方。允许放入文件夹或 .zip 压缩包，放入文件夹时，需要保证在 pack 目录中点进一次文件夹就能看到内容包的 manifest.json。$\r$\n"#DIF
        FileWrite $2 "saves：存放存档的地方。$\r$\n"                                                        #DIF
        FileWrite $2 "captures：存放屏幕截图和屏幕录制的地方。$\r$\n"                                              #DIF
        FileWrite $2 "shaders：存放 CherryGrove 基本运行所需要的着色器。不是用来放置内容包/光影包的地方！请把光影包也放到 packs 文件夹中。$\r$\n"#DIF
        FileWrite $2 "test（可能不存在）：存放开发版 CherryGrove 的测试用途文件，需要了解代码中的测试内容才能正确使用该文件夹。$\r$\n"         #DIF
        FileWrite $2 "$\r$\n"
        FileWrite $2 "文件介绍：$\r$\n"                                                                 #DIF
        FileWrite $2 "CherryGrove.exe：CherryGrove 软件。$\r$\n"                                       #DIF
        FileWrite $2 "uninstall.exe：卸载 CherryGrove 需要的程序。双击打开以卸载本软件。$\r$\n"                        #DIF
        FileWrite $2 "$\r$\n"
        FileWrite $2 "更多信息，请访问官方网站：https://cherrygrove.dev。$\r$\n"                                 #DIF
        FileWrite $2 "本软件以 GPL-3.0-or-later 许可证免费开源。如果你付费获得该软件，请立即差评并举报店铺。$\r$\n"                  #DIF
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