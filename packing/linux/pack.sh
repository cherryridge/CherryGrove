#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd -- "$script_dir/../.." && pwd)"
app_dir="$script_dir/CherryGrove.AppDir"
app_image_path="$script_dir/CherryGrove"

wget -O "$script_dir/appimagetool" https://github.com/AppImage/appimagetool/releases/latest/download/appimagetool-x86_64.AppImage
chmod +x -- "$script_dir/appimagetool"

copy_item_to_package_root() {
    local source_path="$1"
    local staging_directory="$2"
    local destination_path="$staging_directory/$(basename -- "$source_path")"
    if [ -e "$destination_path" ] || [ -L "$destination_path" ]; then
        echo "Package root conflict: $destination_path already exists." >&2
        return 1
    fi
    cp -a -- "$source_path" "$staging_directory/"
}

new_game_package_tar_gz() (
    local app_image="$1"
    local destination_tar_gz="$2"
    local mandatory_root_directory="$script_dir/../readmes"
    local mandatory_items=(
        "$repo_root/LICENSE"
        "$repo_root/assets"
        "$app_image"
    )
    local optional_items=(
        "$repo_root/packs"
        "$repo_root/saves"
        "$repo_root/tests"
        "$repo_root/settings.json"
    )
    local staging_directory
    staging_directory="$(mktemp -d "${TMPDIR:-/tmp}/cherrygrove-tar-staging.XXXXXX")"
    trap 'rm -rf -- "$staging_directory"' EXIT
    for item in "${mandatory_items[@]}"; do
        if [ ! -e "$item" ] && [ ! -L "$item" ]; then
            echo "Mandatory item does not exist: $item" >&2
            return 1
        fi
        copy_item_to_package_root "$item" "$staging_directory"
    done
    if [ ! -d "$mandatory_root_directory" ]; then
        echo "Mandatory root directory does not exist: $mandatory_root_directory" >&2
        return 1
    fi
    shopt -s dotglob nullglob
    for item in "$mandatory_root_directory"/*; do
        copy_item_to_package_root "$item" "$staging_directory"
    done
    shopt -u dotglob nullglob
    for item in "${optional_items[@]}"; do
        if [ -e "$item" ] || [ -L "$item" ]; then
            copy_item_to_package_root "$item" "$staging_directory"
        fi
    done
    rm -f -- "$destination_tar_gz"
    tar -czf "$destination_tar_gz" -C "$staging_directory" .
)

new_game_package() {
    local preset="$1"
    local appimage_arch="$2"
    local destination_tar_gz="$3"
    local executable_path="$repo_root/out/$preset/CherryGrove"
    if [ ! -f "$executable_path" ]; then
        echo "Mandatory item does not exist: $executable_path" >&2
        return 1
    fi
    rm -rf -- "$app_dir/CherryGrove"
    cp -a -- "$executable_path" "$app_dir/CherryGrove"
    chmod +x -- "$app_dir/CherryGrove"
    rm -f -- "$app_image_path"
    ARCH="$appimage_arch" "$script_dir/appimagetool" -v --comp zstd "$app_dir" "$app_image_path"
    chmod +x -- "$app_image_path"

    new_game_package_tar_gz "$app_image_path" "$destination_tar_gz"
}

chmod +x -- "$app_dir/AppRun"

rm -rf -- "$app_dir/cherrygrove.png"
cp -a -- "$repo_root/assets/icons/CherryGrove-trs-256.png" "$app_dir/cherrygrove.png"

new_game_package "linux-x64-release" "x86_64" "$script_dir/CherryGrove_linux_x64.tar.gz"
new_game_package "linux-arm64-release" "aarch64" "$script_dir/CherryGrove_linux_arm64.tar.gz"