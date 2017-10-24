import QtQuick 2.7

Rectangle {
    id:tab
    width:1366
    height:768
    color: "#eeeeee"

    property string pageName: "tabname"


    Text {
        id: txtLoading
        text: pageName
        anchors.centerIn: parent
        font.family: font_LibreBaskerville_Italic.name
        font.pointSize: 32
        font.italic: true
        color: "#818181"
        FontLoader{
            id: font_LibreBaskerville_Italic
            source: "qrc:/assets/fonts/LibreBaskerville-Italic.ttf"
        }
    }


}
