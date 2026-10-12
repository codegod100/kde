// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Layouts
import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.plasmoid
import "backend" as Backend
import "gaze.js" as Gaze

PlasmoidItem {
    id: root
    preferredRepresentation: fullRepresentation
    Plasmoid.backgroundHints: PlasmaCore.Types.NoBackground
    toolTipMainText: "Loop"
    toolTipSubText: tracker.connected ? "Keeping an eye on your cursor" : "Enable Loop Eyes Cursor in System Settings → KWin Scripts"
    Backend.CursorTracker { id: tracker }
    fullRepresentation: Item {
        id: face
        Layout.minimumWidth: 48
        Layout.minimumHeight: 32
        Layout.preferredWidth: 56
        Layout.preferredHeight: 40
        Layout.maximumWidth: Infinity
        Layout.maximumHeight: Plasmoid.formFactor === PlasmaCore.Types.Vertical ? width : Infinity
        // A close portrait makes the eyes legible within a short panel.
        // Preserve the image's aspect ratio and recognizable octopus head.
        readonly property real ratio: Math.min(width / 272, height / 216)
        Item {
            id: art
            width: 272
            height: 216
            scale: face.ratio
            transformOrigin: Item.TopLeft
            x: (face.width - width * scale) / 2
            y: (face.height - height * scale) / 2
            clip: true
            Image {
                x: -120; y: -160; width: 512; height: 512
                source: "../images/loop-avatar.png"
                smooth: true
            }
            Repeater {
                model: [84, 189]
                delegate: Rectangle {
                    id: eye
                    required property real modelData
                    x: modelData - 26; y: 151 - 28
                    width: 52; height: 56; radius: 26
                    color: "#fffedf"
                    property point gaze: Qt.point(0, 0)
                    Rectangle {
                        width: 23; height: 29; radius: 12
                        color: "#061b37"
                        x: (parent.width - width) / 2 + eye.gaze.x
                        y: (parent.height - height) / 2 + eye.gaze.y
                        Behavior on x { NumberAnimation { duration: 65 } }
                        Behavior on y { NumberAnimation { duration: 65 } }
                    }
                    function updateGaze() {
                        if (!tracker.connected) { gaze = Qt.point(0, 0); return; }
                        // mapToGlobal includes actual panel position and item transforms.
                        // Both Qt Quick and KWin use logical desktop coordinates; never
                        // multiply by DPR, normalize each screen, or drop negative origins.
                        var center = mapToGlobal(width / 2, height / 2);
                        var p = Gaze.offset(tracker.x - center.x, tracker.y - center.y, 12, 11, 160);
                        gaze = Qt.point(p.x, p.y);
                    }
                    Connections { target: tracker; function onChanged() { eye.updateGaze(); } }
                    // Re-evaluate if a panel moves while the pointer stays still.
                    Timer { interval: 100; running: face.visible; repeat: true; onTriggered: eye.updateGaze() }
                    Component.onCompleted: updateGaze()
                }
            }
        }
    }
}
