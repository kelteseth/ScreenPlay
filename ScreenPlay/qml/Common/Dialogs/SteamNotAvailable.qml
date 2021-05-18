import QtQuick 2.12
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

Dialog {
    id: dialogSteam

    modal: true
    anchors.centerIn: Overlay.overlay
    standardButtons: Dialog.Ok
    title: qsTr("Could not load steam integration!")
}
