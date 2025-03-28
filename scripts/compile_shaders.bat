@echo off
setlocal enabledelayedexpansion

set "source_root=..\src\shader"
set "dest_root=..\shaders"

if not exist %dest_root% (mkdir "%dest_root%")

pushd "%source_root%" >nul 2>&1
set "source_root=%CD%"
popd >nul
pushd "%dest_root%" >nul 2>&1
set "dest_root=%CD%"
popd >nul

if not exist "!dest_root!\glsl" (mkdir "!dest_root!\glsl")
if not exist "!dest_root!\dx11" (mkdir "!dest_root!\dx11")
if not exist "!dest_root!\spirv" (mkdir "!dest_root!\spirv")
if not exist "!dest_root!\metal" (mkdir "!dest_root!\metal")

echo Compiling vertex shaders...
for /R "%source_root%" %%F in (*.vert) do (
    set "relative_path=%%F"
    set "relative_path=!relative_path:%source_root%\=!"
    echo Compiling !relative_path!
    set "dest_file_gl=%dest_root%\glsl\!relative_path!.bin"
    set "dest_file_dx=%dest_root%\dx11\!relative_path!.bin"
    set "dest_file_vk=%dest_root%\spirv\!relative_path!.bin"
    set "dest_file_mt=%dest_root%\metal\!relative_path!.bin"
    .\shadercDebug.exe -f %%F -o !dest_file_gl! --type v -p 440 --platform windows
    .\shadercDebug.exe -f %%F -o !dest_file_dx! --type v -p s_5_0 --platform windows -O 3
    .\shadercDebug.exe -f %%F -o !dest_file_vk! --type v -p spirv16-13 --platform linux
    .\shadercDebug.exe -f %%F -o !dest_file_mt! --type v -p metal31-14 --platform osx
)

echo Compiling fragment shaders...
for /R "%source_root%" %%F in (*.frag) do (
    set "relative_path=%%F"
    set "relative_path=!relative_path:%source_root%\=!"
    echo Compiling !relative_path!
    set "dest_file_gl=%dest_root%\glsl\!relative_path!.bin"
    set "dest_file_dx=%dest_root%\dx11\!relative_path!.bin"
    set "dest_file_vk=%dest_root%\spirv\!relative_path!.bin"
    set "dest_file_mt=%dest_root%\metal\!relative_path!.bin"
    .\shadercDebug.exe -f %%F -o !dest_file_gl! --type f -p 440 --platform windows
    .\shadercDebug.exe -f %%F -o !dest_file_dx! --type f -p s_5_0 --platform windows -O 3
    .\shadercDebug.exe -f %%F -o !dest_file_vk! --type f -p spirv16-13 --platform linux
    .\shadercDebug.exe -f %%F -o !dest_file_mt! --type f -p metal31-14 --platform osx
)

echo Compilation finished.
endlocal
pause