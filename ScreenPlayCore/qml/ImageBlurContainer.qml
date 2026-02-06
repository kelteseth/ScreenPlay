import QtQuick
import QtQuick.Controls.Material
import QtQuick.Effects

/*!
    \qmltype ImageBlurContainer
    \brief Blurs a region of a background image and overlays a tint.

    Point \c backgroundSource at a visible Image, bind \c scrollY to the
    Flickable's contentY, done. Falls back to a plain tinted rectangle
    when no source is set.
*/
Item {
    id: root

    // The visual item to sample and blur behind this panel.
    property Image backgroundSource: null

    // Bind to the enclosing Flickable.contentY to keep the blur
    // aligned during scrolling.
    property real scrollY: 0

    // Blur intensity from 0.0 (none) to 1.0 (maximum).
    property real blurAmount: 0.6

    // Tint colour painted on top of the blur. Use alpha for transparency.
    property color tintColor: Material.backgroundColor

    // Corner radius of the panel.
    property real radius: 8

    // Width of the subtle highlight border. Set to 0 to disable.
    property real borderWidth: 1

    // Colour of the subtle highlight border.
    property color borderColor: Qt.rgba(1, 1, 1, 0.08)

    // Fallback: plain tinted rectangle when there is no source.
    Rectangle {
        id: fallback
        anchors.fill: parent
        radius: root.radius
        color: root.tintColor
        visible: !root.backgroundSource
    }

    // Frosted-glass pipeline (only active when a source exists).

    ShaderEffectSource {
        id: effectSource
        anchors.fill: parent
        sourceItem: root.backgroundSource
        sourceRect: {
            // Touch these so the binding re-evaluates after StackView
            // transitions settle the item's global position.
            const _deps = [root.scrollY, root.x, root.y,
                           root.width, root.height, root.visible]
            if (!root.backgroundSource)
                return Qt.rect(0, 0, 0, 0)
            const global = root.mapToGlobal(0, 0)
            const local = root.backgroundSource.mapFromGlobal(global.x, global.y)
            return Qt.rect(local.x, local.y, root.width, root.height)
        }
        visible: false
        live: true
    }

    MultiEffect {
        anchors.fill: parent
        source: effectSource
        visible: root.backgroundSource
        blurEnabled: true
        blurMax: 64
        blur: root.blurAmount
        autoPaddingEnabled: false
    }

    // Tint overlay on top of the blur.
    Rectangle {
        anchors.fill: parent
        radius: root.radius
        color: root.tintColor
        opacity: .25
        visible: root.backgroundSource
    }

    // Subtle edge highlight.
    Rectangle {
        anchors.fill: parent
        radius: root.radius
        color: "transparent"
        border.width: root.borderWidth
        border.color: root.borderColor
        visible: root.borderWidth > 0
    }
}
