import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../" as Common

Common.Dialog {
    id: root

    standardButtons: Dialog.Ok
    title: qsTr("Could not load steam integration!")
}
