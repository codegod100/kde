// SPDX-License-Identifier: MIT
// KWin owns global pointer access under Wayland. Logical coordinates, not pixels.
var dirty = true;
var ticks = 0;
workspace.cursorPosChanged.connect(function () { dirty = true; });
var timer = new QTimer();
timer.interval = 33;
timer.timeout.connect(function () {
    ticks++;
    // Coalesce high-rate mice to ~30 updates/s; heartbeat at least once a second.
    if (!dirty && ticks < 30) return;
    dirty = false;
    ticks = 0;
    var p = workspace.cursorPos;
    callDBus("org.loop.Eyes", "/org/loop/Eyes/Cursor", "org.loop.Eyes.Cursor", "Update",
             JSON.stringify({x: p.x, y: p.y}));
});
timer.start();
