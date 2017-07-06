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
        sourceSize: Qt.size(rectangle1.width, rectangle1.height)
        visible: false
        fillMode: Image.PreserveAspectFit
        antialiasing: true
    }

    RectangularGlow {
        id: effect
        anchors.fill: itemWrapper
        glowRadius: 2
        spread: 0.5
        color: "black"
        opacity: .2
        cornerRadius: itemWrapper.radius + glowRadius
    }

    Rectangle {
        id: itemWrapper
        color: "white"
        radius: 2
        anchors {
            fill: parent
            margins: 5
        }



        Rectangle {
            id: rectangle1
            height: 121
            color: "#8b8b8b"
            visible: false
            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.top: parent.top
            anchors.topMargin: 0


            ScreenPlayItemImage {
                id: screenPlayItemImage

                sourceImage: Qt.resolvedUrl(
                                 "file:///" + installedListModel.absoluteStoragePath + "/Wallpaper/"
                                 + screenFolderId + "/" + screenPreview)
            }
        }

        OpacityMask {
            anchors.fill: rectangle1
            antialiasing: true
            source: rectangle1
            maskSource: mask
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                itemClicked(screenId)
            }
        }


        Text {
            id: text1
            y: 136
            height: 29
            text: screenTitle
            renderType: Text.NativeRendering
            anchors.rightMargin: 108
            anchors.leftMargin: 10
            anchors.bottomMargin: 10
            anchors.topMargin: 10
            anchors.top: rectangle1.bottom
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            wrapMode: Text.WordWrap

            font.pixelSize: 12
            color: "#2F2F2F"
            anchors.margins: 10
            font.family: font_Roboto_Regular.name

            FontLoader {
                id: font_Roboto_Regular
                source: "qrc:/assets/fonts/Roboto-Regular.ttf"
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
