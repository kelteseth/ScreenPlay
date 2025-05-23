import QtQuick
import QtQuick.Effects

Item {
    id: root
    property Item sourceItem
    property bool hideSource: true
    property alias blurMax: effect.blurMax
    property alias blur: effect.blur
    property alias colorOverlayOpacity: colorOverlay.opacity

    ShaderEffectSource {
        id: effectSource
        anchors.fill: parent
        sourceItem: root.sourceItem
        live: false
        hideSource: root.hideSource
        visible: false
    }

    MultiEffect {
        id: effect
        anchors.fill: parent
        source: effectSource
        blurEnabled: true
        blurMax: 64
        blur: 1.0
        autoPaddingEnabled: true // Ensures blur doesn't get cut off at edges
    }

    Rectangle {
        id: colorOverlay
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
