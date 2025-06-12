import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
import ScreenPlayShader

ApplicationWindow {
    id: root
    width: 1366
    height: 768
    visible: true
    title: qsTr("ScreenPlayShader")

    Image {
        id: img
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
        source: "qrc:/qt/qml/ScreenPlayShader/assets/test_image_andras-vas-Bd7gNnWJBkU-unsplash.jpg"
        layer.enabled: true
        layer.effect: ShaderEffect {
            property real amplitude: 0.04 * .1
            property real frequency: 2
            property real time: 0
            property real framerate: 60
            property real updateInterval: Math.round((1000 / framerate) * 10) / 10

            Timer {
                interval: updateInterval
                onIntervalChanged: print(updateInterval)
                running: true
                repeat: true
                onTriggered: {
                    if (parent.time > 600) {
                        parent.time = 0;
                    }
                    parent.time += 1;
                }
            }

            fragmentShader: "/shaders/wobble.frag.qsb"
        }
    }
}
