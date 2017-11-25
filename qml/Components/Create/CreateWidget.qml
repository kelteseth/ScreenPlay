import QtQuick 2.7
import QtGraphicalEffects 1.0

Item {
    FontLoader {
        id: font_Roboto_Regular
        source: "qrc:/assets/fonts/Roboto-Regular.ttf"
    }
    Text {
        id: txtCreate
        text: qsTr("Create Widget")
        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
        }

        color: "white"
        font.pixelSize: 28
        renderType: Text.NativeRendering
        font.family: font_Roboto_Regular.name
    }

    CreateWidgetButton {
        id: btnEmpty
        text: qsTr("Create Emtpy Widget")
        anchors.top: txtCreate.bottom
        anchors.topMargin: 20
        onClicked: {

        }
    }
    Text {
        id: txtExamples
        text: qsTr("Examples")
        font.family: font_Roboto_Regular.name
        renderType: Text.NativeRendering
        font.pixelSize: 18
        color: "white"

        anchors {
            top: btnEmpty.bottom
            topMargin: 30
        }
    }

    Column {
        width: parent.width
        spacing: 20
        anchors {
            top: txtExamples.bottom
            topMargin: 10
            bottom: parent.bottom
        }
        CreateWidgetButton {
            id: btnEmpty1
            text: qsTr("Create 123")
            onClicked: {

            }
        }
        CreateWidgetButton {
            id: btnEmpty2
            text: qsTr("Create 456")
            onClicked: {

            }
        }
        CreateWidgetButton {
            id: btnEmpty3
            text: qsTr("Create 789")
            onClicked: {

            }
        }
    }
}
