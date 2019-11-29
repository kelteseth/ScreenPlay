import QtQuick 2.12
import QtQuick.Controls 2.3
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import ScreenPlay 1.0

Rectangle {
    id: root
    state: "off"
    clip: true

    property alias text: txtExpander.text

    Flickable {
        anchors.fill: parent
        contentHeight: txtExpander.paintedHeight
        z: 999
        focus: true
        contentWidth: parent.width
        ScrollBar.vertical: ScrollBar {
            snapMode: ScrollBar.SnapOnRelease
            policy: ScrollBar.AlwaysOn
        }
        Text {
            id: txtExpander
            anchors {
                top: parent.top
                right: parent.right
                left: parent.left
                margins: 20
            }
            color: "#626262"
            height: txtExpander.paintedHeight
            wrapMode: Text.WordWrap
        }
        MouseArea {
            anchors.fill: parent
            propagateComposedEvents: true
            acceptedButtons: Qt.RightButton
            onClicked: contextMenu.popup()
        }
    }
    Menu {
        id: contextMenu
        MenuItem {
            text: qsTr("Copy text to clipboard")
            onClicked: {
                ScreenPlay.util.copyToClipboard(txtExpander.text)
            }
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
