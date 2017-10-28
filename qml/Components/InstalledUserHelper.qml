import QtQuick 2.0

Item {
    anchors {
        fill: parent
    }

    Text {
        id: name
        text: qsTr("taaaaaaaaaaaaaaaaaaaaaaaaaaaext")
        anchors.verticalCenterOffset: 0
        anchors.horizontalCenterOffset: 231
        font.pixelSize: 30
        anchors.centerIn: parent
    }
    Image {
        id: imgRock
        source: "qrc:/assets/icons/icon_heavy_metal.svg"
    }
}
