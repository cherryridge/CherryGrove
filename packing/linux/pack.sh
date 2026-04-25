#!/usr/bin/env bash
set -euo pipefail

wget -O ./appimagetool https://github.com/AppImage/appimagetool/releases/latest/download/appimagetool-x86_64.AppImage
chmod +x appimagetool

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

icon_src="$assets_dst/icons/CherryGrove-trs-512.png"

if [[ ! -f "$icon_src" ]]; then
    echo "Missing icon file: $icon_src" >&2
    exit 1
fi

cp -a -- "$icon_src" "$app_dir/cherrygrove.png"

copyFileToAppDir() {
    local sourceFile="$1"
    if [[ ! -f "$sourceFile" ]]; then
        printf 'error: source file does not exist or is not a regular file: %s\n' "$sourceFile" >&2
        return 1
    fi
    cp -a -- "$sourceFile" "$app_dir/"
}

copyFileToAppDir "$repo_root/LICENSE"

copyFileToAppDir "$repo_root/out/linux-x64-release/CherryGrove"
ARCH=x86_64 ./appimagetool -v --comp zstd CherryGrove.AppDir CherryGrove_linux_x64.AppImage

rm -rf -- "$app_dir/CherryGrove"
copyFileToAppDir "$repo_root/out/linux-arm64-release/CherryGrove"
ARCH=aarch64 ./appimagetool -v --comp zstd CherryGrove.AppDir CherryGrove_linux_arm64.AppImage