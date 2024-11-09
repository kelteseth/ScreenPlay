import QtQuick
import Qt5Compat.GraphicalEffects

FastBlur {
    id: root
    property Item sourceItem
    property bool hideSource: true
    source: ShaderEffectSource {
        id: effectSource
        sourceItem: root.sourceItem
        live: false
        hideSource: root.hideSource
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
        source: "qrc:/qml/ScreenPlayApp/assets/images/noisy-texture-3.png"
        fillMode: Image.Tile
    }
}
