# Validation

Version 0.1.2, October 2026.

- Native Release compilation passed with GCC 16.2.1 and Qt 6.12.0.
- Seven Node behavior/source tests passed, including gaze bounds, negative
  logical origins, coalescing, heartbeat and transport/package checks.
- Installer/uninstaller and package helper shell syntax checks passed.
- Original avatar bytes preserved: SHA-256
  `edfc3e939976775452d9a059ca3981b358a2bf55fc8e571dbc3f21401f152568`.
- Live Garuda Plasma/KWin 6.7.5 Wayland: compact head rendered on a 40-pixel
  bottom panel, cross-application cursor tracking and stationary heartbeat
  verified. Pupils centered after three seconds with only the Loop script
  stopped, then resumed on script reload. Shared bridge worked with both
  variants simultaneously.
- An initial detached plasmashell replacement exited unexpectedly. No
  Loop-related crash was recorded. Service-based recovery restored the shell
  and all panel contents; KWin was unchanged. The final panel-head variant was
  loaded without another shell restart, and the shell remained stable.
- No system dependencies were installed on the desktop during testing.

Only one display was connected. Multiple outputs/mixed scaling, other panel
orientations, hotplug, full-session/KWin restart and uninstall/reinstall remain
untested. QML lint reports advisory static-type metadata/outer-ID warnings;
actual runtime imports and rendering succeeded.

Conda delivery contains source and explicit helpers only. Its native plugin is
compiled against the host desktop Qt on demand; no desktop-linked binary is
redistributed. Package-content and installed-helper checks are encoded in the
recipe and publication checks.

The noarch source-only conda recipe built successfully and passed its installed
helper and package-content tests. The installed package's --check and
--build-only also passed under a pixi/conda environment with deliberately
conflicting Qt/compiler/library environment variables. CMake selected the
system compiler and Qt; readelf showed no RPATH/RUNPATH and ldd resolved all
libraries to the system installation. No binary or build directory was included
in the package. Publication uses the CI-built artifact, not desktop build logs
or local build metadata.
