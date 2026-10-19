# Build and install the components in this repo with their pixi tasks.
# Each installable component has its own pixi.toml; see its README for the
# distro packages (Qt 6, KDE Frameworks 6) it needs.

# List the recipes and the installable components
default:
    @just --list
    @echo "Components: $(just components)"

# The components `just install` accepts
components:
    @for f in */pixi.toml; do printf '%s ' "${f%/pixi.toml}"; done; echo

# Build, test and install a component, then reload Plasma (e.g. just install prefix-runner)
install pkg: (_check pkg)
    pixi run --manifest-path {{pkg}}/pixi.toml test
    pixi run --manifest-path {{pkg}}/pixi.toml install
    pixi run --manifest-path {{pkg}}/pixi.toml restart-plasma

# Configure, build and run a component's tests
test pkg: (_check pkg)
    pixi run --manifest-path {{pkg}}/pixi.toml test

# Run any pixi task of a component (e.g. just run tray clean)
run pkg task: (_check pkg)
    pixi run --manifest-path {{pkg}}/pixi.toml {{task}}

_check pkg:
    @[ -f "{{pkg}}/pixi.toml" ] || { echo "Unknown component '{{pkg}}'. Choose one of: $(just components)" >&2; exit 1; }
