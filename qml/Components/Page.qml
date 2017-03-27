import QtQuick 2.0
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3

Rectangle {
    id:page
    width:1366
    height:768
    color: "#eeeeee"
    property string pageName: "pagename"



    LinearGradient {
        id: pageShadow
        height: 5

        anchors{
            top:parent.top
            right: parent.right
            left: parent.left
        }
        start: Qt.point(0, 0)
        end: Qt.point(0,5)
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#22000000" }
            GradientStop { position: 1.0; color: "#00000000" }
        }
    }

    ColumnLayout {
        id: columnLayout
        anchors.fill: parent

        Rectangle {
            Layout.alignment: Qt.AlignCenter
            color: "steelblue"
            Layout.preferredHeight: 100
        }

    }

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
