#!/usr/bin/env bash
set -euo pipefail

wget -O ./appimagetool https://github.com/AppImage/appimagetool/releases/latest/download/appimagetool-x86_64.AppImage
chmod +x appimagetool
chmod +x -- "$app_dir/AppRun"

script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd -- "$script_dir/../.." && pwd)"

app_dir="$script_dir/CherryGrove.AppDir"

assets_src="$repo_root/assets"
assets_dst="$app_dir/assets"

if [[ ! -d "$assets_src" ]]; then
    echo "Missing assets directory: $assets_src" >&2
    exit 1
fi

rm -rf -- "$assets_dst"
cp -a -- "$assets_src" "$app_dir/"

cp -a -- "$assets_dst/icons/CherryGrove-trs-512.png" "$app_dir/cherrygrove.png"
cp -a -- "$repo_root/LICENSE" "$app_dir/LICENSE"

cp -a -- "$repo_root/out/linux-x64-release/CherryGrove" "$app_dir/CherryGrove"
chmod +x -- "$app_dir/CherryGrove"
ARCH=x86_64 ./appimagetool -v --comp zstd CherryGrove.AppDir CherryGrove_linux_x64.AppImage

rm -rf -- "$app_dir/CherryGrove"
cp -a -- "$repo_root/out/linux-arm64-release/CherryGrove" "$app_dir/CherryGrove"
chmod +x -- "$app_dir/CherryGrove"
ARCH=aarch64 ./appimagetool -v --comp zstd CherryGrove.AppDir CherryGrove_linux_arm64.AppImage