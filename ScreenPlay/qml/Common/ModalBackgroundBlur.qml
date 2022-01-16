import QtQuick
import Qt5Compat.GraphicalEffects

FastBlur {
    id: root
    property var sourceItem
    source: ShaderEffectSource {
        sourceItem: root.sourceItem
        live: false
    }
    radius: 64
    transparentBorder: true
    Rectangle {
        anchors.fill: parent
        opacity: 0.5
        color: "black"
    }
    Image {
        anchors.fill: parent
        opacity: 0.1
        source: "qrc:/assets/images/noisy-texture-3.png"
        fillMode: Image.Tile
    }
}
