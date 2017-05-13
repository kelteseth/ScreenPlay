import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.0


Item {
    id: headline
    property string name: ""

    width: parent.width
    height: 80

Item {
    height: 70
    anchors {
        top: parent.top
        right: parent.right
        left: parent.left
    }


    Text {
        id:txtName
        text: headline.name
        font.italic: true
        renderType: Text.NativeRendering
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        anchors.bottom: line.top
        anchors.bottomMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 10
        font.pixelSize: 18
        color: "#818181"
        anchors.margins: 10
        font.family: font_LibreBaskerville_Italic.name

        FontLoader {
            id: font_LibreBaskerville_Italic
            source: "qrc:/assets/fonts/LibreBaskerville-Italic.ttf"
        }
    }

    Rectangle {
        id:line
        height: 1
        width: parent.width
        color: "#BBBBBB"
        anchors.bottom: parent.bottom
    }
}
}
