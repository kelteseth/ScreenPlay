import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import QtQuick.Controls.Material
import QtQuick.Layouts


Popup {
    id: root
    property Item modalSource
    // Workaround for missing animation on hide
    // when using hideSource
    property bool aboutToHide: false
    dim: true
    anchors.centerIn: Overlay.overlay
    modal: true
    focus: true

    Overlay.modal: ModalBackgroundBlur {
        id: blurBg
        sourceItem: root.modalSource
        hideSource: root.aboutToHide
    }
    onAboutToHide: root.aboutToHide = false
    onClosed: root.aboutToHide = true
}
