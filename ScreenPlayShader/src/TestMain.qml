import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material

Window {
    id: root
    width: 1366
    height: 768
    visible: true
    title: qsTr("ScreenPlayShader")

    Image {
        id: img
        anchors.centerIn: parent

        layer.enabled: true
        layer.effect: ShaderEffect {
            //property variant source: theSource
            property real amplitude: 0.04 * 1
            property real frequency: 20
            property real time: 0
            NumberAnimation on time { loops: Animation.Infinite; from: 0; to: Math.PI * 2; duration: 600 }

            fragmentShader: " :/ScreenPlayShader/shaders/wobble.frag.qsb"
        }
        source: Qt.resolvedUrl(
                    "file:///C:/Users/Eli/Desktop/jasmin-ne-Z4zOxNBri5I-unsplash.jpg")
    }
}
