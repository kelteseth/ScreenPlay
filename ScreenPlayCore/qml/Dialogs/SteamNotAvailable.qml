import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ScreenPlayCore as Util

Util.Dialog {
    id: root

    standardButtons: Dialog.Ok
    title: qsTr("Could not load steam integration!")
}
