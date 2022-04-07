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
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop

        layer.enabled: true
        layer.effect: ShaderEffect {
            property real amplitude: 0.04 * 1
            property real frequency: 200
            property real time: 0
            NumberAnimation on time {
                loops: Animation.Infinite
                from: 0
                to: Math.PI * 2
                duration: 600
            }

            fragmentShader: "/shaders/wobble.frag.qsb"
        }
        source: "qrc:/assets/test_image_andras-vas-Bd7gNnWJBkU-unsplash.jpg"
    }
}
