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

Qt 6 (Core, Gui, Network) and KF6 Runner, I18n and CoreAddons come from your
distro (on Arch: `sudo pacman -S --needed qt6-base krunner ki18n kcoreaddons`);
[pixi](https://pixi.sh) provides cmake, ninja and extra-cmake-modules.
From the repo root:

```sh
just install prefix-runner   # tests, sudo-installs, reloads KRunner and Plasma
```

or, inside `prefix-runner/`, the pixi tasks directly: `pixi run test`,
`pixi run install`, `pixi run restart-plasma`, `pixi run uninstall`.

Nandi OS ships it as the `nandi-prefixdev-runner` package.

## Tests

```sh
pixi run test          # offline: parsing, ranking, commands
pixi run test-online   # loads the plugin and searches prefix.dev
```
