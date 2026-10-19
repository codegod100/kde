#!/bin/bash
set -euo pipefail
target="$PREFIX/share/loop-eyes/source"
mkdir -p "$target" "$PREFIX/bin"
for path in backend kwin panel tests CMakeLists.txt LICENSE README.md TEST-RESULTS.md install.sh uninstall.sh; do
    cp -a "$path" "$target/"
done
install -m755 scripts/loop-eyes-install "$PREFIX/bin/loop-eyes-install"
install -m755 scripts/loop-eyes-uninstall "$PREFIX/bin/loop-eyes-uninstall"
