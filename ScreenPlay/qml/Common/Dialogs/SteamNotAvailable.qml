import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: dialogSteam

    modal: true
    anchors.centerIn: Overlay.overlay
    standardButtons: Dialog.Ok
    title: qsTr("Could not load steam integration!")
}
