// SPDX-License-Identifier: MIT
function offset(dx, dy, radiusX, radiusY, distance) {
    if (!isFinite(dx) || !isFinite(dy) || !(radiusX > 0) || !(radiusY > 0))
        return {x: 0, y: 0};
    var length = Math.sqrt(dx * dx + dy * dy);
    if (length < 0.001) return {x: 0, y: 0};
    var amount = Math.min(1, length / Math.max(1, distance));
    return {x: radiusX * dx / length * amount, y: radiusY * dy / length * amount};
}
