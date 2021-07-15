import QtQuick 2.12
import Qt5Compat.GraphicalEffects
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12
import ScreenPlay 1.0
import ScreenPlay.Create 1.0

FocusScope {
    id: root

    property Component sourceComponent
    property alias savePopup: savePopup
    property bool ready: false

    signal wizardStarted()
    signal wizardExited()
    signal saveClicked()
    signal saveFinished()
    signal cancelClicked()

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

            text: qsTr("Save")
            enabled: root.ready
            Material.background: Material.accent
            Material.foreground: "white"
            Layout.alignment: Qt.AlignRight
            font.family: ScreenPlay.settings.font
            onClicked: {
                btnSave.enabled = false;
                root.saveClicked();
                loader.item.create();
                savePopup.open();
            }
        }

    }

    Popup {
        id: savePopup

        modal: true
        focus: true
        width: 250
        height: 200
        anchors.centerIn: Overlay.overlay
        onOpened: timerSave.start()

        BusyIndicator {
            anchors.centerIn: parent
            running: true
        }

        Text {
            text: qsTr("Saving...")
            color: Material.primaryHighlightedTextColor
            font.family: ScreenPlay.settings.font

            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
                bottomMargin: 30
            }

        }

        Timer {
            id: timerSave

            interval: 1000 + Math.random() * 1000
            onTriggered: {
                savePopup.close();
                root.wizardExited();
            }
        }

    }

}
