#!/usr/bin/env bash
set -euo pipefail

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