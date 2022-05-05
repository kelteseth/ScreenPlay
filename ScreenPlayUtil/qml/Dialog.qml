import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import QtQuick.Controls.Material.impl
import QtQuick.Controls.Material
import ScreenPlayApp
import ScreenPlay

Dialog {
    id: root
    property Item modalSource
    dim: true
    anchors.centerIn: Overlay.overlay
    modal: true
    focus: true

    Overlay.modal: ModalBackgroundBlur {
        id: blurBg
        sourceItem: root.modalSource
        hideSource: root.hideSource
    }
    // Workaround for missing animation on hide
    // when using hideSource
    property bool hideSource: true
    onAboutToHide: root.hideSource = false
    onAboutToShow: root.hideSource = true
}
