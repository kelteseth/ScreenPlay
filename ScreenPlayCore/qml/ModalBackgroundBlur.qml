import QtQuick
import QtQuick.Effects

Item {
    id: root
    property Item sourceItem
    property bool hideSource: true

    ShaderEffectSource {
        id: effectSource
        anchors.fill: parent
        sourceItem: root.sourceItem
        live: false
        hideSource: root.hideSource
        visible: false // Hide the source as MultiEffect will display it
    }

    MultiEffect {
        anchors.fill: parent
        source: effectSource
        blurEnabled: true
        blurMax: 64
        blur: 1.0
        autoPaddingEnabled: true // Ensures blur doesn't get cut off at edges
    }

    Rectangle {
        anchors.fill: parent
        opacity: 0.5
        color: "black"
    }

    Image {
        anchors.fill: parent
        opacity: 0.1
        source: "qrc:/qt/qml/ScreenPlay/assets/images/noisy-texture-3.png"
        fillMode: Image.Tile
    }
}
