# KDE projects

KDE-specific projects by codegod100.

- [Loop Eyes](loop-eyes/): a Plasma 6 panel companion with oversized eyes that follow the cursor through a KWin script. Includes source, installation instructions, tests, and a source-only conda recipe.
- [Whitelist System Tray](tray/): a fork of the Plasma 6 system tray that keeps every icon in the expander popup unless you whitelist it.
- [Flatkit window decorations](aurorae/): Aurorae themes (light and dark) that make KWin title bars match the [flatkit](https://github.com/codegod100/flutterkit) Flutter widgets.
- [prefix.dev package search](prefix-runner/): a KRunner plugin; type `pixi <name>` in KRunner or Kickoff to search conda packages on prefix.dev.

## Install

Components with a `pixi.toml` build with [pixi](https://pixi.sh) and install
through [just](https://just.systems):

```sh
just install prefix-runner   # or: just install tray, just install loop-eyes
just components              # what `just install` accepts
just run tray clean          # any pixi task of a component
```

Each component's README lists the distro packages it needs.
