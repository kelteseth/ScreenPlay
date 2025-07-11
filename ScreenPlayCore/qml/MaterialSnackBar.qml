pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Controls.Material.impl

/*!
    \qmltype MaterialSnackBar
    \inqmlmodule ScreenPlayCore
    \brief A simple Material Design snackbar component.

    Shows a text message for 1.5 seconds then automatically hides.
*/
Popup {
    id: root

    /*!
        \qmlmethod void show(string message)
        Shows the snackbar with the given message.
        \param message The text message to display
    */
    function show(message) {
        messageText.text = message;
        root.open();
        hideTimer.start();
    }

    function hide() {
        root.close();
        hideTimer.stop();
    }

    width: Math.min(parent.width - 32, 800)
    height: 48
    modal: false
    focus: false
    closePolicy: Popup.NoAutoClose
    transformOrigin: Popup.Center

    anchors.centerIn: undefined
    parent: Overlay.overlay

    x: (parent.width - width) / 2
    y: parent.height - height - 16

    background: Rectangle {
        color: Material.accent
        radius: 4

        layer.enabled: true
        layer.effect: ElevationEffect {
            elevation: 6
        }
    }

    contentItem: Rectangle {
        id: contentRect
        color: "transparent"

        Text {
            id: messageText

            anchors {
                left: parent.left
                right: parent.right
                verticalCenter: parent.verticalCenter
                leftMargin: 16
                rightMargin: 16
            }

            color: "white"
            font.pointSize: 14
            wrapMode: Text.WordWrap
            maximumLineCount: 2
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
        }
    }

    // Enter animation - triggered when popup opens
    enter: Transition {
        ParallelAnimation {
            PropertyAnimation {
                target: root
                properties: "opacity"
                from: 0
                to: 1
                duration: 300
                easing.type: Easing.OutQuart
            }
            PropertyAnimation {
                target: root
                properties: "scale"
                from: 0.95
                to: 1.0
                duration: 300
                easing.type: Easing.OutQuart
            }
            PropertyAnimation {
                target: root
                properties: "y"
                from: root.parent.height - root.height + 20
                to: root.parent.height - root.height - 16
                duration: 350
                easing.type: Easing.OutBack
                easing.overshoot: 1.2
            }
        }
    }

    // Exit animation - triggered when popup closes
    exit: Transition {
        ParallelAnimation {
            PropertyAnimation {
                target: root
                properties: "opacity"
                from: 1
                to: 0
                duration: 250
                easing.type: Easing.InQuart
            }
            PropertyAnimation {
                target: root
                properties: "scale"
                from: 1.0
                to: 0.95
                duration: 250
                easing.type: Easing.InQuart
            }
            PropertyAnimation {
                target: root
                properties: "y"
                from: root.parent.height - root.height - 16
                to: root.parent.height - root.height + 20
                duration: 300
                easing.type: Easing.InBack
            }
        }
    }

    Timer {
        id: hideTimer
        interval: 1500
        onTriggered: root.hide()
    }
}
