import QtQuick 2.0

Item {
    id:screenPlayItem
    width: 320
    height: 180

    property string customTitle: "name here"
    signal itemClicked(var screenName)

    Rectangle
    {
        id: rectangle
        color: "white"
        radius: 23
        anchors {
            fill: parent
            margins:5
        }

        Text {
            id: text1
            text: customTitle
            renderType: Text.QtRendering
            wrapMode: Text.WrapAnywhere
            anchors.fill: parent
            font.pixelSize: 18
            anchors.margins: 10


        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                itemClicked(customTitle)
            }
        }
    }



}
