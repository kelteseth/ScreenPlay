import QtQuick
import Qt5Compat.GraphicalEffects
import QtQuick.Controls.Material
import ScreenPlayApp
import ScreenPlayUtil

Item {
    id: root
    property string previewImage
    property string appID
    property var installedType: ContentTypes.InstalledType.QMLWallpaper
    property bool monitorWithoutContentSelectable: true
    property bool hasContent: appID !== ""
    property int fontSize: 10
    property int index
    property bool isSelected: false

    property rect geometry
    onGeometryChanged: {
        root.width = geometry.width;
        root.height = geometry.height;
        root.x = geometry.x;
        root.y = geometry.y;
    }

    signal monitorSelected(int index)
    signal removeWallpaper(int index)

    onIsSelectedChanged: root.state = isSelected ? "selected" : "default"
    onPreviewImageChanged: {
        if (previewImage === "") {
            imgPreview.hasPreview = false;
        } else {
            imgPreview.source = Qt.resolvedUrl("file:///" + previewImage);
            imgPreview.hasPreview = true;
        }
    }

    Text {
        text: geometry.width + "x" + geometry.height
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
            property bool hasPreview: false
            opacity: {
                if (hasPreview && root.enabled)
                    return 1;
                else
                    return .5;
            }
            Behavior on opacity {
                NumberAnimation {
                    duration: 250
                }
            }

            anchors.fill: parent
            asynchronous: true
            fillMode: Image.PreserveAspectCrop
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            enabled: root.enabled
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                if (monitorWithoutContentSelectable) {
                    root.monitorSelected(root.index);
                    return;
                }
                if (root.hasContent && !root.monitorWithoutContentSelectable)
                    root.monitorSelected(root.index);
            }
        }

        ToolButton {
            text: "❌"
            enabled: root.hasContent && root.enabled
            visible: enabled
            onClicked: root.removeWallpaper(root.index)
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
