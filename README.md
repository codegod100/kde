# Loop Eyes

A quiet KDE Plasma 6 panel companion: a compact blue head with oversized cream
eyes, navy pupils, a small gold forehead dot, and tiny lower lobes. Inspired by
Loop's octopus avatar, its eyes follow the cursor over other applications using
a local KWin script. The original image portrait is also included.

## Install from prefix.dev

```sh
pixi global install -c https://prefix.dev/nandi-testing loop-eyes
loop-eyes-install --check
loop-eyes-install --install
```

This is a **source-only `noarch` conda package for Linux**, not a prebuilt Qt
plugin. The explicit installer builds the small native bridge against your
system Qt, then installs three user-local desktop packages. It does not enable
the script, add widgets, restart the shell, or install system dependencies.
There are no conda post-link, activation, or removal hooks.

Host requirements: Linux, Plasma 6/KWin, system CMake, a C++17 compiler, Qt 6.4+
Core/Qml/DBus development files, `qmake6`, `kpackagetool6`, and `kwriteconfig6`
for removal. Use the system Qt matching your installed Plasma. On Arch/Garuda,
these are provided by `cmake`, `gcc`, `qt6-base`, `qt6-declarative`, `kpackage`,
and `kconfig`. On Ubuntu, install the equivalent Plasma runtime and
`cmake g++ qt6-base-dev qt6-declarative-dev` development packages.

The helper removes conda compiler, Qt, CMake and library search overrides while
preserving desktop-session variables. It builds outside the installed conda
tree, in a new directory under `$XDG_CACHE_HOME/loop-eyes` (default
`~/.cache/loop-eyes`). It retains that directory for inspection; you may remove
old build directories after installation. `--build-only` validates compilation
without writing desktop packages. Source packaging is reproducible; the local
native binary depends on the host Qt/compiler and must be rebuilt after Qt
upgrades. No conda Qt library is bundled or loaded into system plasmashell.

After installation:

1. System Settings → Window Management → KWin Scripts: enable **Loop Eyes Cursor**.
2. Add **Loop Eyes — Panel Head** to your panel. The original **Loop Eyes** portrait
   remains optional. Keep both widget packages installed because they share the
   original package's native bridge and gaze helper.

Package IDs: `org.loop.eyes`, `org.loop.eyes.panel`, `org.loop.eyes.cursor`.
No pointer history, networking, telemetry, keyboard access or shell polling.
The bridge accepts updates only from the current KWin D-Bus owner. Missing
updates center the pupils after about 2.5 seconds; one-second heartbeats keep a
stationary cursor connected. Eye coordinates are logical desktop coordinates.

An existing native plugin cannot be unloaded safely by replacing files. On an
upgrade, remove/re-add the QML-only widget if it reloads correctly; otherwise
apply changes on your next login. If deliberately refreshing just plasmashell,
use your desktop's user-session service. Do not restart KWin. The installer
never performs any restart itself.

## Remove

Remove the Loop widget from the panel, then run `loop-eyes-uninstall` before
removing the conda package. This disables only the Loop KWin script and removes
its three user-local desktop packages. Log out normally later to unload native
code. Removing the conda package alone leaves the separately installed desktop
widget intact. Other widgets are not changed.

## Build from source and package

```sh
node tests/test.js
bash -n install.sh uninstall.sh scripts/loop-eyes-install scripts/loop-eyes-uninstall
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 2
bash install.sh
pixi exec rattler-build build --recipe recipe/recipe.yaml -c conda-forge --output-dir output
```

The conda recipe copies reviewed source files only, and checks that no `.so`
or build directory entered the package. GitHub Actions uses the shared
`nandi-conda/ci` workflow pinned to a commit, with existing inherited publishing
authorization for the public `nandi-testing` channel. The separate Checks
workflow compiles against Ubuntu's system Qt and runs behavior/shell checks.

## Validation and limits

See [TEST-RESULTS.md](TEST-RESULTS.md). Tested on Garuda Plasma/KWin 6.7.5,
Qt 6.12.0 Wayland, one display at 100% scaling, a 40-pixel bottom panel. Larger
eyes, cross-application tracking, stationary heartbeat and script
disconnect/reconnect passed. Multiple displays, mixed scaling, other panel
orientations, hotplug and compositor restart still need testing. Plasma and
KWin are system runtime dependencies; this is not a standalone desktop pet.

Code is MIT licensed. The original bundled avatar is user-supplied artwork and
is not covered by the code license; see [LICENSE-AVATAR](LICENSE-AVATAR).

References: [KWin scripting API](https://develop.kde.org/docs/plasma/kwin/api/)
and [Qt plugin deployment](https://doc.qt.io/qt-6/deployment-plugins.html).
