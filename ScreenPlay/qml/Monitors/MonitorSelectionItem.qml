import QtQuick 2.12
import QtGraphicalEffects 1.0
import ScreenPlay 1.0
import QtQuick.Controls.Material 2.12

Item {
    id: root

    property rect monitorSize: Qt.rect(0, 0, 0, 0)
    property string monitorModel
    property string monitorManufacturer
    property string monitorName
    property string monitorID
    property string previewImage: ""
    property string appID
    property var installedType


    onPreviewImageChanged: {
        if (previewImage === "") {
            imgPreview.opacity = 0
        } else {
            imgPreview.source = Qt.resolvedUrl("file:///" + previewImage)
            imgPreview.opacity = 1
        }
    }

    property int fontSize: 10
    property int index
    property bool isSelected: false
    onIsSelectedChanged: root.state = isSelected ? "selected" : "default"

    signal monitorSelected(var index)

    Text {
        text: monitorSize.width + "x" + monitorSize.height
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: wrapper.bottom
            topMargin: 5
        }
        color: Material.foreground

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pointSize: root.fontSize
        font.family: ScreenPlay.settings.font
        wrapMode: Text.WrapAnywhere
    }

    Rectangle {
        id: wrapper
        color: "#828282"
        anchors.fill: parent
        anchors.margins: 10
        border.color: "#1e1e1e"
        border.width: 3
        radius: 3
        clip: true

        Image {
            id: imgPreview
            sourceSize: Qt.size(parent.width, parent.height)
            anchors.margins: 3
            opacity: 0
            anchors.fill: parent
            asynchronous: true
            fillMode: Image.PreserveAspectCrop
        }

        Text {
            font.pointSize: 14
            text: root.index
            anchors.centerIn: parent
            color: "white"
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                monitorSelected(index)
            }
        }
    }

    states: [
        State {
            name: "default"
            PropertyChanges {
                target: wrapper
                border.color: "#373737"
            }
        },
        State {
            name: "selected"
            PropertyChanges {
                target: wrapper
                border.color: "#F28E0D"
            }
        }
    ]
    transitions: [
        Transition {
            from: "default"
            to: "selected"
            reversible: true
            PropertyAnimation {
                target: wrapper
                duration: 200
                easing.type: Easing.InOutQuart
                property: "border.color"
            }
        }
    ]
}
