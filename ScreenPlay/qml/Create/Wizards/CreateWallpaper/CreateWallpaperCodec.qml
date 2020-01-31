import QtQuick 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.3
import Qt.labs.platform 1.0
import QtQuick.Layouts 1.12

import ScreenPlay 1.0
import ScreenPlay.Create 1.0

import "../../../Common"

Item {
    id:root

    signal next

    Rectangle {
        anchors.centerIn: parent
        width: 100
        height: 100
        color: "orange"
    }

    Button {
        text: qsTr("Next")
        onClicked: {
            root.next()
        }

        anchors {
            right:parent.right
            bottom: parent.bottom
        }
    }
}
