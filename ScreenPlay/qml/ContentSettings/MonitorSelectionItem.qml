import QtQuick
import QtQuick.Controls.Material
import ScreenPlay
import ScreenPlayCore

Item {
    id: root

    required property int fontSize
    required property bool monitorWithoutContentSelectable
    // Model
    required property string appID
    required property string name
    required property rect geometry
    required property string previewImage
    required property var installedType
    required property int appState
    onAppStateChanged: print(appState)
    required property int monitorIndex

    property bool isSelected: false

    onGeometryChanged: {
        root.width = root.geometry.width;
        root.height = root.geometry.height;
        root.x = root.geometry.x;
        root.y = root.geometry.y;
    }

    signal monitorSelected(int monitorIndex)
    signal removeWallpaper(int monitorIndex)

    onIsSelectedChanged: root.state = isSelected ? "selected" : "default"
    property bool hasContent: false
    onPreviewImageChanged: {
        print(root.previewImage);
        if (root.previewImage === "") {
            root.hasContent = false;
        } else {
            imgPreview.source = Qt.resolvedUrl("file:///" + root.previewImage);
            root.hasContent = true;
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
            text: root.geometry.width + "x" + root.geometry.height + "," + root.name
            color: "white"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pointSize: 10
            font.family: App.settings.font
            wrapMode: Text.WrapAnywhere

            anchors {
                left: parent.left
                top: parent.top
                topMargin: 2
                leftMargin: 2
            }
        }

        Text {
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
                bottomMargin: 5
            }
            visible: true
            font.pointSize: 10
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: Material.primaryTextColor
            styleColor: Material.secondaryTextColor
            style: Text.Outline
            text: {
                switch (root.appState) {
                case ScreenPlayEnums.AppState.NotSet:
                    return "";
                case ScreenPlayEnums.AppState.Inactive:
                    return qsTr("Inactive");
                case ScreenPlayEnums.AppState.Starting:
                    return qsTr("Starting");
                case ScreenPlayEnums.AppState.StartingFailed:
                    return qsTr("StartingFailed");
                case ScreenPlayEnums.AppState.ClosingFailed:
                    return qsTr("ClosingFailed");
                case ScreenPlayEnums.AppState.Closing:
                    return qsTr("Closing");
                case ScreenPlayEnums.AppState.Active:
                    return qsTr("Active");
                case ScreenPlayEnums.AppState.Timeout:
                    return qsTr("Timeout");
                case ScreenPlayEnums.AppState.ErrorOccouredWhileActive:
                    return qsTr("Error Occoured While Active");
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
