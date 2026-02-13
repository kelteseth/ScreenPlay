import QtQuick
import QtQuick.Controls.Material
import QtQuick.Effects

/*!
    \qmltype ImageBlurContainer
    \brief Blurs a region of a background image and overlays a tint.

    Point \c backgroundSource at a visible Image, bind \c flickable to the
    enclosing Flickable / GridView, done. Falls back to a plain tinted
    rectangle when no source is set.
*/
Item {
    id: root

    // The visual item to sample and blur behind this panel.
    property Image backgroundSource: null

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

    // Bind to the enclosing Flickable / GridView so the blur
    // refreshes on scroll.
    property Flickable flickable: null

    // Bind to the enclosing StackView so the blur refreshes
    // after push/pop transitions complete.
    property StackView stackView: null

    Connections {
        target: root.flickable
        function onContentYChanged(): void {
            root.updateSourceRect()
        }
        function onContentXChanged(): void {
            root.updateSourceRect()
        }
    }

    // Refresh the blur every frame while the StackView transition
    // is animating so the effect tracks the sliding page smoothly.
    FrameAnimation {
        running: root.stackView !== null && root.stackView.busy
        onTriggered: root.updateSourceRect()
    }

    function updateSourceRect(): void {
        if (!root.backgroundSource) {
            effectSource.sourceRect = Qt.rect(0, 0, 0, 0)
            return
        }
        const global = root.mapToGlobal(0, 0)
        const local = root.backgroundSource.mapFromGlobal(global.x, global.y)
        effectSource.sourceRect = Qt.rect(local.x, local.y, root.width, root.height)
        // print("sourceRect:", effectSource.sourceRect)
    }

    onXChanged: root.updateSourceRect()
    onYChanged: root.updateSourceRect()
    onWidthChanged: root.updateSourceRect()
    onHeightChanged: root.updateSourceRect()
    onVisibleChanged: root.updateSourceRect()
    onBackgroundSourceChanged: root.updateSourceRect()

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
        visible: false
        live: true
        recursive: false
    }

    // Rounded-rectangle mask for the blur (not rendered directly).
    Rectangle {
        id: blurMask
        anchors.fill: parent
        radius: root.radius
        visible: false
        layer.enabled: true
        layer.smooth: true
    }

    MultiEffect {
        anchors.fill: parent
        source: effectSource
        visible: root.backgroundSource
        blurEnabled: true
        blurMax: 64
        blur: root.blurAmount
        autoPaddingEnabled: false
        maskEnabled: true
        maskSource: blurMask
        maskThresholdMin: 0.5
        maskSpreadAtMin: 1.0
    }

    // Tint overlay on top of the blur.
    Rectangle {
        anchors.fill: parent
        radius: root.radius
        color: root.tintColor
        opacity: .4
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
