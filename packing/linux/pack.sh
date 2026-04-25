#!/usr/bin/env bash
set -euo pipefail

wget -O ./appimagetool https://github.com/AppImage/appimagetool/releases/latest/download/appimagetool-x86_64.AppImage
chmod +x appimagetool

script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd -- "$script_dir/../.." && pwd)"
app_dir="$script_dir/CherryGrove.AppDir"

chmod +x -- "$app_dir/AppRun"

rm -rf -- "$app_dir/cherrygrove.png"
cp -a -- "$repo_root/assets/icons/CherryGrove-trs-256.png" "$app_dir/cherrygrove.png"

rm -rf -- "$app_dir/CherryGrove"
cp -a -- "$repo_root/out/linux-x64-release/CherryGrove" "$app_dir/CherryGrove"
chmod +x -- "$app_dir/CherryGrove"
ARCH=x86_64 ./appimagetool -v --comp zstd CherryGrove.AppDir CherryGrove_linux_x64.AppImage

rm -rf -- "$app_dir/CherryGrove"
cp -a -- "$repo_root/out/linux-arm64-release/CherryGrove" "$app_dir/CherryGrove"
chmod +x -- "$app_dir/CherryGrove"
ARCH=aarch64 ./appimagetool -v --comp zstd CherryGrove.AppDir CherryGrove_linux_arm64.AppImage