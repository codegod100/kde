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

Needs Plasma 6 development packages (libplasma, KF6 Config, I18n, IconThemes,
ItemModels, JobWidgets, KIO, Service, StatusNotifierItem, WindowSystem,
XmlGui) and Qt 6 with its private GUI headers. Tested building against
Plasma 6.6 / KF 6.24 / Qt 6.10 (Ubuntu 26.04).

Debian/Ubuntu:

```sh
sudo apt install build-essential cmake ninja-build extra-cmake-modules gettext \
  qt6-base-dev qt6-base-private-dev qt6-declarative-dev qt6-declarative-private-dev \
  libplasma-dev libkf6config-dev libkf6i18n-dev libkf6iconthemes-dev libkf6itemmodels-dev \
  libkf6jobwidgets-dev libkf6kio-dev libkf6service-dev libkf6statusnotifieritem-dev \
  libkf6windowsystem-dev libkf6xmlgui-dev
```

Arch: `sudo pacman -S --needed base-devel cmake ninja extra-cmake-modules libplasma kstatusnotifieritem kio kxmlgui kitemmodels`

Fedora: `sudo dnf install cmake ninja-build extra-cmake-modules gcc-c++ qt6-qtbase-private-devel qt6-qtdeclarative-devel libplasma-devel kf6-kstatusnotifieritem-devel kf6-kio-devel kf6-kxmlgui-devel kf6-kitemmodels-devel kf6-kiconthemes-devel kf6-ki18n-devel kf6-kconfig-devel kf6-kservice-devel kf6-kwindowsystem-devel kf6-kjobwidgets-devel`

Then:

```sh
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build
sudo cmake --install build
plasmashell --replace &   # or log out and back in
```

Add **Whitelist System Tray** to a panel from *Add Widgets…*. You will usually
want to remove the stock System Tray from that panel, since both trays would
otherwise show the same apps.

Run the tests with `cmake -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build`.

## Layout

- `src/`: the forked applet. The visibility rule is
  `BaseModel::calculateEffectiveStatus` in `src/systemtraymodel.cpp`; the
  settings page is `src/qml/ConfigGeneral.qml`.
- `third_party/libdbusmenuqt/`: plasma-workspace's private copy of
  libdbusmenuqt, vendored because it is not installed as a library.
- `autotests/`: tests for the whitelist rule.

## License

GPL-2.0-or-later, like the upstream system tray. See `LICENSES/`.
