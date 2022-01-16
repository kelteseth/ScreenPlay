import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../"

Dialog {
    id: root
    property var modalSource
    Overlay.modal: ModalBackgroundBlur {
        sourceItem: root.modalSource
    }

    modal: true
    anchors.centerIn: Overlay.overlay
    standardButtons: Dialog.Ok
    title: qsTr("Could not load steam integration!")


}
