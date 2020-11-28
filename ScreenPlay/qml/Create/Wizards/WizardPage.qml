import QtQuick 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.12

import ScreenPlay 1.0
import ScreenPlay.Create 1.0

FocusScope {
    id: root

    signal wizardStarted
    signal wizardExited

    property Component sourceComponent
    property alias savePopup: savePopup

    ScrollView {
        anchors {
            topMargin: 20
            top:parent.top
            right:parent.right
            bottom: parent.bottom
            left:parent.left
        }

        contentWidth: width
        contentHeight: loader.height

        Loader {
            id: loader
            width: Math.min(parent.width, 800)
            height: item.childrenRect.height
            clip: true
            sourceComponent: root.sourceComponent
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
            }
        }
    }

    Popup {
        id: savePopup
        modal: true
        focus: true
        width: 250
        height: 200
        anchors.centerIn: parent
        onOpened: timerSave.start()

        BusyIndicator {
            anchors.centerIn: parent
            running: true
        }

        Text {
            text: qsTr("Saving...")
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
                savePopup.close()
                root.wizardExited()
            }
        }
    }
}
