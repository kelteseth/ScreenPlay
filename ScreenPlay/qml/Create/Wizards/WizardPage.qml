import QtQuick
import Qt5Compat.GraphicalEffects
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import QtQuick.Window
import ScreenPlayApp
import ScreenPlay

FocusScope {
    id: root

    property Component sourceComponent
    property alias savePopup: savePopup
    property bool ready: false

    // Signals for the Sidebar
    signal wizardStarted
    signal wizardExited
    // Show error/success popup
    signal wizardFinished(bool success, string message)
    signal saveClicked
    signal saveFinished
    signal cancelClicked

    ScrollView {
        contentWidth: width
        contentHeight: loader.height

        anchors {
            margins: 20
            top: parent.top
            right: parent.right
            bottom: footer.top
            left: parent.left
        }

        Loader {
            id: loader

            width: parent.width
            Component.onCompleted: height = item.childrenRect.height
            clip: true
            sourceComponent: root.sourceComponent

            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
            }
        }
    }

    RowLayout {
        id: footer

        anchors {
            right: parent.right
            bottom: parent.bottom
            left: parent.left
            margins: 20
        }

        Item {
            Layout.fillWidth: true
        }

        Button {
            id: btnSave
            objectName: "btnSave"
            text: qsTr("Save")
            enabled: root.ready
            Material.background: Material.accent
            Material.foreground: "white"
            Layout.alignment: Qt.AlignRight
            font.family: App.settings.font
            onClicked: {
                btnSave.enabled = false
                root.saveClicked()
                savePopup.open()
                loader.item.create()
            }
        }
    }

    Connections {
        target: root
        function onWizardFinished(success, message) {
            if (success) {
                timerSave.start()
                return
            }

            savePopup.title = message
            savePopup.standardButtons = Dialog.Ok
        }
    }

    Dialog {
        id: savePopup

        modal: true
        focus: true
        width: 250
        height: 200
        anchors.centerIn: Overlay.overlay
        onAccepted: {
            savePopup.close()
            root.wizardExited()
        }
        title: qsTr("Saving...")

        BusyIndicator {
            anchors.centerIn: parent
            running: true
        }
        Timer {
            id: timerSave
            interval: 200 + Math.random() * 500
            onTriggered: {
                savePopup.close()
                root.wizardExited()
            }
        }
    }
}
