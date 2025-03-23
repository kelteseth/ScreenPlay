import QtQuick
import QtQuick.Controls
import QtQuick.Window

Popup {
    id: root
    modal: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    anchors.centerIn: Overlay.overlay

    property string errorMessage: ""

    contentItem: Column {
        spacing: 10
        Label {
            text: qsTr("⚠️ Fatal error encountered:")
            font.pointSize: 16
            font.bold: true
        }
        Label {
            text: root.errorMessage
            wrapMode: Text.WordWrap
        }
        Row {
            spacing: 10
            Button {
                text: qsTr("Ok")
                onClicked: {
                    root.close();
                }
            }
        }
    }
}
