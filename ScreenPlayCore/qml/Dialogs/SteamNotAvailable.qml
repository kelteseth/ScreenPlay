import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ScreenPlayCore as SPCore

SPCore.Dialog {
    id: root

    standardButtons: Dialog.Ok
    title: qsTr("Could not load steam integration!")
}
