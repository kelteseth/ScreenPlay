import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3

Rectangle {
    id: root
    state: "off"
    clip: true

    Component.onCompleted: screenPlaySettings.requestAllLicenses()

    Connections {
        target: screenPlaySettings
        onAllLicenseLoaded: {
            txtLegal.text = licenses
        }
    }

    Flickable {
        anchors.fill: parent
        contentHeight: txtLegal.paintedHeight
        z: 999
        focus: true
        contentWidth: parent.width
        ScrollBar.vertical: ScrollBar {
            snapMode: ScrollBar.SnapOnRelease
            policy: ScrollBar.AlwaysOn
        }
        Text {
            id: txtLegal
            anchors {
                top: parent.top
                right: parent.right
                left: parent.left
                margins: 20
            }
            color: "#626262"
            renderType: Text.NativeRendering
            height: txtLegal.paintedHeight
        }
    }

    function toggle() {
        root.state = root.state == "on" ? "off" : "on"
    }

    states: [
        State {
            name: "on"

            PropertyChanges {
                target: root
                height: 500
            }
        },
        State {
            name: "off"
            PropertyChanges {
                target: root
                height: 0
            }
        }
    ]
    transitions: [
        Transition {
            from: "off"
            to: "on"
            reversible: true
            PropertyAnimation {
                target: root
                property: "height"
                duration: 250
            }
        }
    ]
}
