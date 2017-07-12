import QtQuick 2.7
import QtQuick.Controls 2.2

Tab {
    id: page
    tabName: ""

    Rectangle {
        id: rectangle
        width: 200
        height: 46
        color: "#ffffff"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 30

        TextEdit {
            id: textEdit
            text: qsTr("Text Edit")
            anchors.fill: parent
            font.pixelSize: 12
        }
    }

    Connections {
        target:  steamWorkshop
        onWorkshopItemCreatedQML: print(eResult,publishedFileId)
    }

    Button {
        id: button
        x: 285
        y: 173
        text: qsTr("Button")
        onClicked: steamWorkshop.createWorkshopItem()
    }



}
