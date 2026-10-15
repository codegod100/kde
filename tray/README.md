# Whitelist System Tray

A Plasma 6 system tray widget that keeps **every** icon tucked away in the
expander popup unless you put it on the whitelist. It is a fork of the stock
Plasma system tray (`plasma-workspace/applets/systemtray`, Plasma 6.7 branch)
with a new plugin id, `org.kde.plasma.whitelisttray`, so it installs alongside
the stock tray without replacing it.

## How visibility works

Each entry in the widget's settings (right click the tray, **Configure System
Tray…**, **Entries**) has one of these choices:

| Choice | Behaviour |
| --- | --- |
| **Keep in popup** (default) | Lives in the expander popup, never on the panel. |
| **Show on panel when relevant** | On the panel while the app reports itself as active or needing attention, otherwise in the popup. This matches the stock tray's default behaviour, but only for whitelisted entries. |
| **Always show on panel** | Always on the panel. |
| **Never show (disabled)** | Hidden entirely. |

The **Move to panel while requesting attention** checkbox lets entries that
are not whitelisted still pop out to the panel while they flash for attention.
It is off by default. **Always show all** still exists and overrides the
whitelist.

The whitelist is stored in the widget's config as `shownItems` (always) and
`allowedItems` (when relevant).

## Build and install

The build uses [pixi](https://pixi.sh) for the build tools (cmake, ninja,
extra-cmake-modules). Qt 6, KDE Frameworks 6 and libplasma come from your
distro: KF6 and libplasma are not on conda-forge, and the widget has to link
against the same Qt/KF6 that your plasmashell runs on. The pixi tasks use the
system compiler and system libraries and install into the system plugin
directory.

1. Install the system libraries (and a C++ compiler). Only the Ubuntu list has
   been tested; the Arch and Fedora names follow those distros' usual naming.

   ```sh
   # Arch
   sudo pacman -S --needed base-devel libplasma kstatusnotifieritem kio kxmlgui kitemmodels kiconthemes

   # Fedora
   sudo dnf install gcc-c++ qt6-qtbase-private-devel qt6-qtdeclarative-devel libplasma-devel kf6-kstatusnotifieritem-devel kf6-kio-devel kf6-kxmlgui-devel kf6-kitemmodels-devel kf6-kiconthemes-devel kf6-ki18n-devel kf6-kconfig-devel kf6-kservice-devel kf6-kwindowsystem-devel kf6-kjobwidgets-devel

   # Ubuntu / Kubuntu / KDE neon
   sudo apt install build-essential gettext qt6-base-dev qt6-base-private-dev qt6-declarative-dev qt6-declarative-private-dev libplasma-dev libkf6config-dev libkf6i18n-dev libkf6iconthemes-dev libkf6itemmodels-dev libkf6jobwidgets-dev libkf6kio-dev libkf6service-dev libkf6statusnotifieritem-dev libkf6windowsystem-dev libkf6xmlgui-dev
   ```

2. Install pixi if you don't have it: `curl -fsSL https://pixi.sh/install.sh | sh`

3. Build, test, install and restart Plasma:

   ```sh
   git clone https://github.com/codegod100/kde && cd kde/tray
   pixi run test             # configures, builds and runs the tests
   pixi run install          # sudo install into the system plugin dir
   pixi run restart-plasma   # or log out and back in
   ```

Other tasks: `pixi run build`, `pixi run clean`. Without pixi, the same steps
are plain cmake: `cmake -B build -G Ninja -DCMAKE_INSTALL_PREFIX=/usr && cmake --build build && sudo cmake --install build`
(needs cmake, ninja and extra-cmake-modules from your distro).

Add **Whitelist System Tray** to a panel from *Add Widgets…*. You will usually
want to remove the stock System Tray from that panel, since both trays would
otherwise show the same apps.

## Layout

- `src/`: the forked applet. The visibility rule is
  `BaseModel::calculateEffectiveStatus` in `src/systemtraymodel.cpp`; the
  settings page is `src/qml/ConfigGeneral.qml`.
- `third_party/libdbusmenuqt/`: plasma-workspace's private copy of
  libdbusmenuqt, vendored because it is not installed as a library.
- `autotests/`: tests for the whitelist rule.

## License

GPL-2.0-or-later, like the upstream system tray. See `LICENSES/`.
