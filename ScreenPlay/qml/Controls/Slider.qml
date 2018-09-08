import QtQuick 2.9
import QtQuick.Controls 2.2

Item {
    id: sliderVolumeWrapperBottom
    height: 70

    anchors {
        left: parent.left
        right: parent.right
    }

    property string headline: "dummyHeandline"
    property string iconSource: "qrc:/assets/icons/icon_volume.svg"
    property alias value: slider.value

    Text {
        id: txtHeadline
        text: headline
        height: 20
        font.pixelSize: 14
        font.family: "Roboto"
        color: "#5D5D5D"
        renderType: Text.NativeRendering
        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
        }
    }

    Row {
        spacing: 30
        anchors {
            top: txtHeadline.bottom
            right: parent.right
            bottom: parent.bottom
            left: parent.left
        }

        Image {
            id: imgIcon
            width: 20
            height: 20
            anchors.verticalCenter: parent.verticalCenter
            source: iconSource
            sourceSize: Qt.size(20, 20)
        }

        Slider {
            id: slider
            stepSize: 0.01
            from: 0
            value: 1
            to: 1
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            id: txtValue
            color: "#818181"
            text: Math.round(slider.value * 100) / 100
            anchors.verticalCenter: parent.verticalCenter
            font.family: "Libre Baskerville"
            font.pointSize: 12
            font.italic: true
            verticalAlignment: Text.AlignVCenter
        }
    }
}
