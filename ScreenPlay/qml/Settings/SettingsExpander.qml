import QtQuick 2.12
import QtQuick.Controls 2.3
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.2
import ScreenPlay 1.0

Item {
    id: root

    property alias text: txtExpander.text

    function toggle() {
        root.state = root.state == "on" ? "off" : "on";
    }

    state: "off"
    clip: true
    width: parent.width
    implicitHeight: 50

    Flickable {
        anchors.fill: parent
        contentHeight: txtExpander.paintedHeight
        z: 999
        focus: true
        contentWidth: parent.width

        Text {
            id: txtExpander

            color: Material.theme === Material.Light ? Qt.lighter(Material.foreground) : Qt.darker(Material.foreground)
            lineHeight: 1.2
            height: txtExpander.paintedHeight
            wrapMode: Text.WordWrap
            font.family: ScreenPlay.settings.font

            anchors {
                top: parent.top
                right: parent.right
                left: parent.left
                margins: 20
            }

        }

        MouseArea {
            anchors.fill: parent
            propagateComposedEvents: true
            acceptedButtons: Qt.RightButton
            onClicked: contextMenu.popup()
        }

        ScrollBar.vertical: ScrollBar {
            snapMode: ScrollBar.SnapOnRelease
            policy: ScrollBar.AlwaysOn
        }

    }

    Menu {
        id: contextMenu

        MenuItem {
            text: qsTr("Copy text to clipboard")
            onClicked: {
                ScreenPlay.util.copyToClipboard(txtExpander.text);
            }
        }

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
