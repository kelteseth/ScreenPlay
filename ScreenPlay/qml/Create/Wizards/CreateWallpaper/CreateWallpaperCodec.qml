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
    id: root

    property var codec: Create.All
    onCodecChanged: print(codec)

    signal next


    ComboBox {
        id: comboBoxCodec
        textRole: "text"
        valueRole: "value"
        anchors.centerIn: parent
        onCurrentIndexChanged: {
            root.codec = model.get(comboBoxCodec.currentIndex).value
        }

        model: ListModel {
            id: model
            ListElement {
                text: "Both"
                value: Create.All
            }
            ListElement {
                text: "VP8"
                value: Create.VP8
            }
            ListElement {
                text: "VP9"
                value: Create.VP9
            }
        }
    }

    Button {
        text: qsTr("Next")
        onClicked: {
            root.next()
        }

        anchors {
            right: parent.right
            bottom: parent.bottom
        }
    }
}
