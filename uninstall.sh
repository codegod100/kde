#!/usr/bin/env bash
# SPDX-License-Identifier: MIT
set -euo pipefail
[[ ${EUID:-$(id -u)} != 0 ]] || { echo 'Run as your desktop user, without sudo.' >&2; exit 1; }
for command in kwriteconfig6 kpackagetool6; do
    command -v "$command" >/dev/null || { echo "Missing $command" >&2; exit 1; }
done
kwriteconfig6 --file kwinrc --group Plugins --key org.loop.eyes.cursorEnabled false
if command -v qdbus6 >/dev/null; then
    qdbus6 org.kde.KWin /KWin reconfigure || echo 'Could not reload KWin. Log out and back in to stop the script.' >&2
else
    echo 'Log out and back in to ensure the cursor script stops.' >&2
fi
data=${XDG_DATA_HOME:-"$HOME/.local/share"}
if [[ -d "$data/plasma/plasmoids/org.loop.eyes.panel" ]]; then
    kpackagetool6 --type Plasma/Applet --remove org.loop.eyes.panel
fi
if [[ -d "$data/plasma/plasmoids/org.loop.eyes" ]]; then
    kpackagetool6 --type Plasma/Applet --remove org.loop.eyes
fi
if [[ -d "$data/kwin/scripts/org.loop.eyes.cursor" ]]; then
    kpackagetool6 --type KWin/Script --remove org.loop.eyes.cursor
fi
echo 'Removed the user packages. Remove any remaining Loop panel item and log out/in to unload its native code.'
