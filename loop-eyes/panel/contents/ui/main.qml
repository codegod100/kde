// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Layouts
import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.plasmoid
// Both variants import the same installed bridge, sharing one native endpoint.
import "../../../org.loop.eyes/contents/ui/backend" as Backend
import "../../../org.loop.eyes/contents/ui/gaze.js" as Gaze

PlasmoidItem {
    id: root
    preferredRepresentation: fullRepresentation
    Plasmoid.backgroundHints: PlasmaCore.Types.NoBackground
    toolTipMainText: "Loop"
    toolTipSubText: tracker.connected ? "Keeping an eye on your cursor" : "Enable Loop Eyes Cursor in KWin Scripts"
    Backend.CursorTracker { id: tracker }
    fullRepresentation: Item {
        id: face
        Layout.minimumWidth: 48
        Layout.preferredWidth: 52
        Layout.minimumHeight: 24
        Layout.preferredHeight: 32
        readonly property real ratio: Math.min(width / 96, height / 48)
        Item {
            id: art
            width: 96; height: 48
            scale: face.ratio
            transformOrigin: Item.TopLeft
            x: (face.width - width * scale) / 2
            y: (face.height - height * scale) / 2
            // Rounded head and three small lobes retain the octopus identity.
            Repeater {
                model: [24, 41, 58]
                delegate: Rectangle {
                    required property real modelData
                    x: modelData; y: 38
                    width: 14; height: 10; radius: 7
                    color: "#10396e"
                }
            }
            Rectangle {
                x: 6; y: 0; width: 84; height: 44; radius: 22
                color: "#10396e"
            }
            Rectangle {
                x: 45; y: 4; width: 6; height: 6; radius: 3
                color: "#f1cb4a"
            }
            Repeater {
                model: [31, 65]
                delegate: Rectangle {
                    id: eye
                    required property real modelData
                    x: modelData - 14; y: 11
                    width: 28; height: 31; radius: 14
                    color: "#fffedf"
                    property point gaze: Qt.point(0, 0)
                    Rectangle {
                        width: 12; height: 17; radius: 6
                        color: "#061b37"
                        x: (parent.width - width) / 2 + eye.gaze.x
                        y: (parent.height - height) / 2 + eye.gaze.y
                        Behavior on x { NumberAnimation { duration: 65 } }
                        Behavior on y { NumberAnimation { duration: 65 } }
                    }
                    function updateGaze() {
                        if (!tracker.connected) { gaze = Qt.point(0, 0); return; }
                        var center = mapToGlobal(width / 2, height / 2);
                        var p = Gaze.offset(tracker.x - center.x, tracker.y - center.y, 7, 6, 160);
                        gaze = Qt.point(p.x, p.y);
                    }
                    Connections { target: tracker; function onChanged() { eye.updateGaze(); } }
                    Timer { interval: 100; running: face.visible; repeat: true; onTriggered: eye.updateGaze() }
                    Component.onCompleted: updateGaze()
                }
            }
        }
    }
}
