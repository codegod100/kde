#!/usr/bin/env bash
# SPDX-License-Identifier: MIT
set -euo pipefail
[[ ${EUID:-$(id -u)} != 0 ]] || { echo 'Run as your desktop user, without sudo.' >&2; exit 1; }
root=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
mode=${1:---install}
[[ $mode == --install || $mode == --build-only ]] || { echo 'Usage: install.sh [--install|--build-only]' >&2; exit 2; }
build_dir=${LOOP_EYES_BUILD_DIR:-"$root/build"}
for command in cmake c++ kpackagetool6; do
    command -v "$command" >/dev/null || { echo "Missing $command. See README.md for dependencies." >&2; exit 1; }
done
cmake_args=(-DCMAKE_BUILD_TYPE=Release)
if [[ -n ${LOOP_EYES_SYSTEM_QT_DIR:-} ]]; then
    cmake_args+=(-DQt6_DIR="$LOOP_EYES_SYSTEM_QT_DIR" -DCMAKE_CXX_COMPILER=/usr/bin/c++ -DCMAKE_FIND_USE_CMAKE_ENVIRONMENT_PATH=OFF)
fi
cmake -S "$root" -B "$build_dir" "${cmake_args[@]}"
cmake --build "$build_dir" --parallel 2
if [[ $mode == --build-only ]]; then
    printf 'Built: %s/backend/libloopeyesbackend.so\n' "$build_dir"
    exit 0
fi
stage=$(mktemp -d)
trap 'rm -rf -- "$stage"' EXIT
cp -R "$root/plasmoid" "$stage/plasmoid"
cp "$build_dir/backend/libloopeyesbackend.so" "$stage/plasmoid/contents/ui/backend/"
data=${XDG_DATA_HOME:-"$HOME/.local/share"}
install_package() {
    local type=$1 id=$2 source=$3 directory=$4
    local action=--install
    [[ ! -d "$directory/$id" ]] || action=--upgrade
    kpackagetool6 --type "$type" "$action" "$source"
}
install_package KWin/Script org.loop.eyes.cursor "$root/kwin" "$data/kwin/scripts"
install_package Plasma/Applet org.loop.eyes "$stage/plasmoid" "$data/plasma/plasmoids"
install_package Plasma/Applet org.loop.eyes.panel "$root/panel" "$data/plasma/plasmoids"
printf '\nInstalled for your user. Next:\n1. System Settings → Window Management → KWin Scripts: enable “Loop Eyes Cursor”, then Apply.\n2. Panel edit mode → Add Widgets: add “Loop Eyes — Panel Head”.\nIf updating a loaded native plugin, log out and back in before testing.\n'
