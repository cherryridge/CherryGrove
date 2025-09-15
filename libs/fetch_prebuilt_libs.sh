#!/usr/bin/env bash
set -euo pipefail

# Usage: ./get_deps.sh [7z_path]
SEVEN_ZIP="${1:-7z}"

# --- Detect OS ---
case "${OSTYPE:-$(uname | tr '[:upper:]' '[:lower:]')}" in
    linux*|gnu*)   os_tag="linux"  ;;
    darwin*|mac*)  os_tag="macos"  ;;
    msys*|cygwin*|win*) echo "This script is for Linux/macOS. Use the PowerShell version on Windows." >&2; exit 1 ;;
    *) echo "Unsupported OS: ${OSTYPE:-unknown}" >&2; exit 1 ;;
esac

# --- Detect Arch ---
uname_m="$(uname -m | tr '[:upper:]' '[:lower:]')"
case "$uname_m" in
    x86_64|amd64) arch_tag="x64"   ;;
    aarch64|arm64) arch_tag="arm64" ;;
    i386|i686) echo "x86 is not supported." >&2; exit 1 ;;
    *) echo "Architecture not detected: $uname_m" >&2; exit 1 ;;
esac

# --- Tool checks ---
command -v curl >/dev/null 2>&1 || { echo "curl is required." >&2; exit 1; }
command -v jq   >/dev/null 2>&1 || { echo "jq is required (sudo apt-get install jq | brew install jq)." >&2; exit 1; }
command -v "$SEVEN_ZIP" >/dev/null 2>&1 || { echo "7z not found at '$SEVEN_ZIP'." >&2; exit 1; }

ua="bash-github-latest-release"
api_ver="2022-11-28"

download_library() {
    local name="$1"
    local repo="cherryridge/dep_${name}"
    local api="https://api.github.com/repos/${repo}/releases/latest"
    echo "Downloading ${name} for ${os_tag} ${arch_tag}..."
    # Fetch release JSON
    json="$(curl -fsSL -H "Accept: application/vnd.github+json" -H "X-GitHub-Api-Version: ${api_ver}" -A "$ua" "$api")" || { echo "Failed to query $api" >&2; return 1; }

    # Pick matching assets by filename
    # Example match: "*linux_x64_debug.7z"
    pat_dbg="${os_tag}_${arch_tag}_debug.7z"
    pat_rel="${os_tag}_${arch_tag}_release.7z"

    url_dbg="$(jq -r --arg pat "$pat_dbg" '
        .assets[] | select(.name | test($pat)) | .browser_download_url
    ' <<<"$json" | head -n1)"
    url_rel="$(jq -r --arg pat "$pat_rel" '
        .assets[] | select(.name | test($pat)) | .browser_download_url
    ' <<<"$json" | head -n1)"

    if [[ -z "${url_dbg}" || -z "${url_rel}" || "$url_dbg" == "null" || "$url_rel" == "null" ]]; then
        echo "Not all assets found in latest release of dep_${name}. Check: ${api}" >&2
        return 1
    fi
    file_dbg="$(basename "$url_dbg")"
    file_rel="$(basename "$url_rel")"
    echo "Downloading $file_dbg ..."
    curl -fL -A "$ua" -o "$file_dbg" "$url_dbg"
    echo "Downloading $file_rel ..."
    curl -fL -A "$ua" -o "$file_rel" "$url_rel"

    # Clean destinations except .gitignore
    script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
    dst_dbg="$script_dir/$name/debug"
    dst_rel="$script_dir/$name/release"
    mkdir -p "$dst_dbg" "$dst_rel"

    find "$dst_dbg" -mindepth 1 ! -name '.gitignore' -exec rm -rf {} +
    find "$dst_rel" -mindepth 1 ! -name '.gitignore' -exec rm -rf {} +

    # Extract (note: no space after -o)
    echo "Extracting $file_dbg → $dst_dbg ..."
    "$SEVEN_ZIP" x "$file_dbg" -o"$dst_dbg" -y
    echo "Extracting $file_rel → $dst_rel ..."
    "$SEVEN_ZIP" x "$file_rel" -o"$dst_rel" -y

    rm -f "$file_dbg" "$file_rel"
}

download_library bgfx
download_library luajit
download_library soloud
download_library v8
download_library wasmtime

echo "All done."