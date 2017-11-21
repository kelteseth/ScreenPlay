import QtQuick 2.7
import QtGraphicalEffects 1.0

Item {

    property rect monitorSize: Qt.rect(0, 0, 0, 0)
    property string monitorModel
    property string monitorManufacturer
    property string monitorName
    property string monitorID
    property int index: 0
    property bool isSelected: false

    signal monitorSelected(var index)

    onIsSelectedChanged: {
        if (isSelected) {
            wrapper.border.color = "orange"
            effect.opacity = .4
        } else {
            wrapper.border.color = "white"
            effect.opacity = .2
        }
    }

    FontLoader {
        id: font_Roboto_Regular
        source: "qrc:/assets/fonts/Roboto-Regular.ttf"
    }
    RectangularGlow {
        id: effect
        anchors.centerIn: parent

        height: wrapper.height
        width: wrapper.width
        cached: true
        glowRadius: 5
        spread: 0
        color: "black"
        opacity: 0.2
        cornerRadius: 3
    }

    Rectangle {
        id: wrapper
        color: "gray"
        anchors.fill: parent
        anchors.margins: 10
        border.color: "white"
        border.width: 3
        radius: 3

        Image {
            id: imgPreview
            anchors.fill: parent
            asynchronous: true
            source: screenPlaySettings.getPreviewImageByMonitorID(monitorID)
        }

        Text {
            renderType: Text.NativeRendering
            text: index + "\n" + monitorName + " " + monitorSize.width + "x" + monitorSize.height
            anchors.fill: parent

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: "white"
            font.pixelSize: 12
            font.family: font_Roboto_Regular.name
            wrapMode: Text.WrapAnywhere
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                monitorSelected(index)
            }
        }
    }
}
