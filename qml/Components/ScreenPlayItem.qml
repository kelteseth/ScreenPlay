import QtQuick 2.0
import QtGraphicalEffects 1.0


Item {
    id:screenPlayItem
    width: 320
    height: 180

    property string customTitle: "name here"
    property string screenId: ""
    signal itemClicked(var screenId)

    OpacityMask {
        anchors.fill: itemWrapper
        source: itemWrapper
        maskSource: mask
        invert: true
    }

    Image {
        id: mask
        source: "qrc:/assets/images/Window.svg"
        sourceSize: Qt.size(itemWrapper.width, itemWrapper.height)
        smooth: true
        visible: false
    }

    Rectangle
    {
        id: itemWrapper
        color: "white"
        radius: 23


        anchors {
            fill: parent
            margins:5
        }

        Rectangle {
            id: rectangle1
            height: 103
            color: "#8b8b8b"
            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.top: parent.top
            anchors.topMargin: 0

            Image {
                id: image
                asynchronous: true
                fillMode: Image.PreserveAspectCrop
                anchors.fill: parent
                source: Qt.resolvedUrl("file:///" + installedListModel._screensPath + screenFolderId + "/" + screenPreview)
            }
        }

        Text {
            id: text1
            text: screenTitle
            anchors.topMargin: 117
            wrapMode: Text.WrapAnywhere
            anchors.fill: parent
            font.pixelSize: 12
            anchors.margins: 10

        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                itemClicked(screenId)
            }
        }
    }



}
