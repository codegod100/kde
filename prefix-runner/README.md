# prefix.dev package search for KRunner

A KDE Plasma 6 KRunner plugin that searches conda packages on
[prefix.dev](https://prefix.dev) through its public GraphQL API
(`https://prefix.dev/api/graphql`, no token needed). Results show up in
KRunner (Alt+Space) and in the Kickoff application launcher's search.

Type `pixi <name>` (or `conda <name>`), at least two letters:

- each result shows the package name, its channel, latest version and summary;
  exact name matches and conda-forge come first;
- Enter opens the package's page on prefix.dev;
- the result's actions copy a `pixi add` or `pixi global install` command for
  that package and channel.

Only queries starting with the trigger word are sent to prefix.dev.

## Build and install

Needs CMake, extra-cmake-modules, Qt 6 (Core, Gui, Network) and KF6 Runner,
I18n and CoreAddons from your distro (on Arch: `krunner ki18n kcoreaddons
qt6-base extra-cmake-modules`).

```sh
cmake -S prefix-runner -B build -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release
cmake --build build
sudo cmake --install build
kquitapp6 krunner   # it restarts on the next Alt+Space
```

Nandi OS ships it as the `nandi-prefixdev-runner` package.

## Tests

```sh
cmake -S prefix-runner -B build -DBUILD_TESTING=ON && cmake --build build
ctest --test-dir build -LE online   # offline: parsing, ranking, commands
ctest --test-dir build -L online    # loads the plugin and searches prefix.dev
```
