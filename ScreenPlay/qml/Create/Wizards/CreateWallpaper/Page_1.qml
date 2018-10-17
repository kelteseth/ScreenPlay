import QtQuick 2.9
import QtQuick.Controls 2.3

Item {
    id: root

    Rectangle {
        id: rectangle1
        color: "#eeeeee"
        width: parent.width * .5
        radius: 3
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
        }

        Flickable {
            anchors.fill: parent
            focus: true
            contentHeight: txtFFMPEG.paintedHeight
            ScrollBar.vertical: ScrollBar {
                snapMode: ScrollBar.SnapOnRelease
                policy: ScrollBar.AlwaysOn
            }
            Text {
                id: txtFFMPEG
                anchors {
                    top: parent.top
                    right: parent.right
                    left: parent.left
                    margins: 20
                }
                wrapMode: Text.WordWrap
                color: "#626262"
                renderType: Text.NativeRendering
                height: txtFFMPEG.paintedHeight

            }
        }
    }
    Connections {
        target: screenPlayCreate
        onProcessOutput: {
            txtFFMPEG.text = text
        }
    }

    Rectangle {
        id: rectangle
        width: parent.width * .5
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.right
        }
    }
}
