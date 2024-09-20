import QtQuick
import Qt5Compat.GraphicalEffects
import QtQuick.Controls.Material
import ScreenPlayApp
import ScreenPlayUtil

Item {
    id: root

    required property int fontSize
    required property bool monitorWithoutContentSelectable
    // Model
    required property string appID
    required property rect geometry
    required property string previewImage
    required property var installedType
    required property var appState
    required property int monitorIndex

    property bool isSelected: false

    onGeometryChanged: {
        root.width = geometry.width;
        root.height = geometry.height;
        root.x = geometry.x;
        root.y = geometry.y;
    }

    signal monitorSelected(int monitorIndex)
    signal removeWallpaper(int monitorIndex)

    onIsSelectedChanged: root.state = isSelected ? "selected" : "default"
    property bool hasContent: false
    onPreviewImageChanged: {
        console.debug("xxx previewImage", previewImage, Qt.resolvedUrl(previewImage));
        if (previewImage === "") {
            root.hasContent = false;
        } else {
            imgPreview.source = Qt.resolvedUrl("file:///" + previewImage);
            root.hasContent = true;
        }
    }

    Text {
        text: root.geometry.width + "x" + geometry.height
        color: Material.foreground
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pointSize: root.fontSize
        font.family: App.settings.font
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
            opacity: root.hasContent ? 1 : 0
            Behavior on opacity {
                NumberAnimation {
                    duration: 250
                }
            }

            anchors.fill: parent
            asynchronous: true
            fillMode: Image.PreserveAspectCrop
        }
        Text {
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
                bottomMargin: 5
            }
            visible: false
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: {
                switch (root.appState) {
                case ScreenPlayEnums.AppState.Inactive:
                    return qsTr("Inactive");
                case ScreenPlayEnums.AppState.Starting:
                    return qsTr("Starting");
                case ScreenPlayEnums.AppState.Closing:
                    return qsTr("Closing");
                case ScreenPlayEnums.AppState.Active:
                    return qsTr("Active");
                default:
                    console.error("Invalid state");
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            enabled: root.enabled
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                if (root.monitorWithoutContentSelectable) {
                    root.monitorSelected(root.monitorIndex);
                    return;
                }
                if (root.enabled && !root.monitorWithoutContentSelectable)
                    root.monitorSelected(root.monitorIndex);
            }
        }

        ToolButton {
            text: "❌"
            enabled: root.hasContent
            visible: enabled
            onClicked: root.removeWallpaper(root.monitorIndex)
            z: 99
            anchors {
                top: parent.top
                right: parent.right
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
