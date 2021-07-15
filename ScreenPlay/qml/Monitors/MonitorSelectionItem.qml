import QtQuick 2.12
import Qt5Compat.GraphicalEffects
import QtQuick.Controls.Material 2.12
import ScreenPlay 1.0
import ScreenPlay.Enums.InstalledType 1.0

Item {
    id: root

    property rect monitorSize: Qt.rect(0, 0, 0, 0)
    property string monitorModel
    property string monitorManufacturer
    property string monitorName
    property string monitorID
    property string previewImage
    property string appID
    property var installedType: InstalledType.QMLWallpaper
    property bool monitorWithoutContentSelectable: true
    property bool hasContent: appID !== ""
    property int fontSize: 10
    property int index
    property bool isSelected: false

    signal monitorSelected(var index)

    onIsSelectedChanged: root.state = isSelected ? "selected" : "default"
    onPreviewImageChanged: {
        if (previewImage === "") {
            imgPreview.opacity = 0;
        } else {
            imgPreview.source = Qt.resolvedUrl("file:///" + previewImage);
            imgPreview.opacity = 1;
        }
    }

    Text {
        text: monitorSize.width + "x" + monitorSize.height
        color: Material.foreground
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pointSize: root.fontSize
        font.family: ScreenPlay.settings.font
        wrapMode: Text.WrapAnywhere

        anchors {
            horizontalCenter: parent.horizontalCenter
            top: wrapper.bottom
            topMargin: 5
        }

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
                if (monitorWithoutContentSelectable) {
                    monitorSelected(index);
                    return ;
                }
                if (root.hasContent && !root.monitorWithoutContentSelectable)
                    monitorSelected(index);

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
