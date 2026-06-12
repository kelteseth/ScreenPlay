import QtQuick
import QtQuick.Controls.Material
import ScreenPlay

Item {
    id: root
    property bool selected: dragHandler.active // User selected
    property bool otherLineHandleActive: false // New property
    property string identifier

    // ── Source of truth ──────────────────────────────────────────────
    // The section's end position as a time-of-day in *seconds* [0, 86400].
    // Everything visual (x, the time label, the section width) is derived
    // from this — pixels are never the stored state. A window resize therefore
    // only re-evaluates the x binding below; it can never reinterpret the
    // handle's time the way an imperative pixel x could.
    property int endSeconds: 0
    // Drag bounds, also in seconds. Timeline.updatePositions keeps these a
    // minSectionSeconds gap away from the neighbours so the user can never
    // drag a section to a size the C++ side would reject.
    property int minSeconds: 0
    property int maxSeconds: 86400

    // Width of the timeline track in pixels — bound to the track by
    // Timeline.qml. The only pixel quantity here, and only ever an output
    // multiplier, never an input we read state back from.
    property real lineWidth: 1
    // Pure presentations of endSeconds.
    readonly property real relativePosition: endSeconds / 86400
    x: relativePosition * lineWidth
    readonly property string timeString: App.util.getTimeString(root.relativePosition)

    property bool isLast: false
    // Default snaps drags to 15-minute increments. Timeline.qml flips this off
    // while the user holds Shift, so the modifier inverts the default rather
    // than enabling snap. 900 s = 15 min, 60 s = 1 min — both keep the handle
    // on a whole-minute boundary so the time string round-trips exactly.
    property bool snapEnabled: true
    readonly property int snapStep: snapEnabled ? 900 : 60
    // Hit-test margin around the visible 20px circle. Timeline.qml clamps this
    // per-handle to half the distance to the nearest neighbour so adjacent
    // hit zones never overlap — otherwise the previously-dragged handle's
    // 30px margin swallows presses aimed at its neighbour.
    property real hitMargin: 30
    property alias dragHandler: dragHandler

    signal handleMoved(var handle)
    signal removeHandle(var handle)
    signal activated(string identifier, bool active)
    // Emitted while this handle has active focus (during drag) so Timeline.qml
    // can propagate the snap toggle to every sibling handle in lockstep.
    signal modifierKeyChanged(bool freeDrag)
    // ⚠️ Note: set the size to 0 so we do not have to
    // handle the offset of the handle width. We set
    // DragHandler margin to 30 to make draggin work.
    width: 0
    height: width

    Keys.onPressed: event => {
        if (event.key === Qt.Key_Shift)
            root.modifierKeyChanged(true)
    }
    Keys.onReleased: event => {
        if (event.key === Qt.Key_Shift && !event.isAutoRepeat)
            root.modifierKeyChanged(false)
    }

    function toString(): void {
        console.log(LoggingCategories.lineHandle, `LineHandle {
        endSeconds: ${endSeconds}
        time: ${timeString}
        selected: ${selected}
        otherLineHandleActive: ${otherLineHandleActive}
        min/max: ${minSeconds}/${maxSeconds}
        isLast: ${isLast}
    }`)
    }

    Rectangle {
        id: handleCircle
        visible: !root.isLast
        radius: width
        color: {
            if (dragHandler.active) {
                return Material.color(Material.Orange)
            }

            if (Material.theme === Material.Dark) {
                return Material.color(Material.Grey, Material.Shade400)
            } else {
                return Material.color(Material.BlueGrey, Material.Shade400)
            }
        }
        x: -(width * .5)
        width: 20
        height: width
        opacity: !root.isLast && (dragHandler.active || hoverHandler.hovered) ? 1 : 0.25
        Behavior on opacity {
            NumberAnimation {}
        }

        HoverHandler {
            id: hoverHandler
            enabled: !root.isLast
            cursorShape: Qt.PointingHandCursor
            margin: root.hitMargin
        }

        // DragHandler lives here (on the visible circle) rather than on root.
        // root has width/height: 0 so it doesn't appear in QQuickItem hit-test
        // — synthesized presses (chuck) would route past it and the drag
        // would never engage. The circle has real bounds + a margin, so the
        // handler activates on a press anywhere on or near the visible knob.
        //
        // target: null because we drive root.x ourselves in
        // onActiveTranslationChanged. With target: root, DragHandler reapplies
        // pressedTargetX + activeTranslation every frame, immediately undoing
        // any snap we tried to write into root.x.
        DragHandler {
            id: dragHandler
            enabled: !root.isLast && !root.otherLineHandleActive
            target: null
            yAxis.enabled: false
            xAxis.enabled: true
            // No xAxis.minimum/maximum: with target null they are inert, and
            // we clamp in seconds below anyway.
            margin: root.hitMargin

            // Captured at press so onActiveTranslationChanged can rebuild the
            // absolute position from a relative gesture translation.
            property int pressSeconds: 0

            onActiveChanged: {
                if (active) {
                    dragHandler.pressSeconds = root.endSeconds
                    // Take focus so Shift press/release reaches Keys handlers
                    // on root while the user is actively dragging.
                    root.forceActiveFocus()
                }
                root.activated(root.identifier, active)
            }

            onActiveTranslationChanged: {
                // Skip the activation-time fire (translation = 0) so a plain
                // click doesn't snap an off-grid handle to the nearest mark
                // without the user actually dragging.
                if (activeTranslation.x === 0 || root.lineWidth <= 0)
                    return
                // Convert the pixel-space drag delta into seconds, add it to
                // the press-time position, then snap and clamp *entirely in
                // seconds*. Pixels are only the unit of the input gesture,
                // never stored state — so there is no sub-pixel rounding that
                // can collapse a section to width 0.
                const deltaSeconds = Math.round(activeTranslation.x / root.lineWidth * 86400)
                let seconds = dragHandler.pressSeconds + deltaSeconds
                seconds = Math.round(seconds / root.snapStep) * root.snapStep
                seconds = Math.max(root.minSeconds, Math.min(root.maxSeconds, seconds))
                if (seconds !== root.endSeconds) {
                    root.endSeconds = seconds
                    root.handleMoved(root)
                }
            }
        }
    }

    Rectangle {
        id: timeLabelBackground
        z: 99
        visible: timeLabel.visible
        color: Material.theme === Material.Dark ? Qt.rgba(0, 0, 0, 0.75) : Qt.rgba(1, 1, 1, 0.9)
        radius: 3
        width: timeLabel.implicitWidth + 8
        height: timeLabel.implicitHeight + 4
        anchors.centerIn: timeLabel
    }

    Text {
        id: timeLabel
        z: 100
        visible: !root.isLast && (dragHandler.active || hoverHandler.hovered)
        text: root.timeString
        color: Material.theme === Material.Dark ? "white" : "black"
        font.pointSize: 10
        anchors {
            horizontalCenter: handleCircle.horizontalCenter
            top: handleCircle.bottom
            topMargin: 10
        }
    }

    // (Previously a MouseArea here blocked the hover-following "+" button
    // from registering clicks on top of the handle. addHandleWrapper now uses
    // a TapHandler with click-anywhere semantics, so the blocker is obsolete
    // — and it was preventing the LineHandle's DragHandler from engaging
    // because QQuickItem hit-test routed the press into the MouseArea before
    // the PointerHandler got a chance.)

    // Text {
    //     id: txt
    //     text: root.minSeconds + " " + root.maxSeconds + " s:" + root.endSeconds
    //     color: Material.secondaryTextColor
    //     opacity: !root.isLast //&& dragHandler.active ? 1 : 0
    //     Behavior on opacity {
    //         NumberAnimation{}
    //     }

    //     anchors {
    //         horizontalCenter: handleCircle.horizontalCenter
    //         top: handleCircle.bottom
    //         topMargin: 30  +  Math.floor(Math.random() * 42);
    //     }
    // }

}
