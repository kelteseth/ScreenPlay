import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    id: screenPlayItem
    width: 320
    height: 180

    property string customTitle: "name here"
    property string screenId: ""
    signal itemClicked(var screenId)


    Image {
        id: mask
        source: "qrc:/assets/images/Window.svg"
        sourceSize: Qt.size(itemWrapper.width, itemWrapper.height)
        smooth: true
        visible: false
    }

    Rectangle {
        id: itemWrapper
        color: "white"
        radius: 6
        clip: true

        anchors {
            fill: parent
            margins: 5
        }

        Rectangle {
            id: rectangle1
            height: 121
            color: "#8b8b8b"
            clip: true
            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.top: parent.top
            anchors.topMargin: 0

            ScreenPlayItemImage {
                id: screenPlayItemImage
                sourceImage: Qt.resolvedUrl(
                                 "file:///" + installedListModel._screensPath
                                 + screenFolderId + "/" + screenPreview)
            }
        }

        Text {
            id: text1
            y: 136
            height: 29
            text: screenTitle
            anchors.rightMargin: 156
            anchors.leftMargin: 5
            anchors.bottomMargin: 5
            anchors.topMargin: 5
            anchors.top: rectangle1.bottom
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            wrapMode: Text.WrapAnywhere
            font.pixelSize: 11
            anchors.margins: 10
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                itemClicked(screenId)
            }
        }

        Item {
            id: item1
            x: 105
            y: 127
            width: 200
            height: 38
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 5
            anchors.right: parent.right
            anchors.rightMargin: 5
        }
    }
}
