import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Material 2.2

Item {
    id: settingsBool
    property string headline: "Headline"
    property string description: "Lorem ipsum dolor sit amet, consectetuer adipiscing elit."
    property bool isChecked: false
    signal checkboxChanged(bool checked)

    height: 50
    width: parent.width

    FontLoader {
        id: font_Roboto_Regular
        source: "qrc:/assets/fonts/Roboto-Regular.ttf"
    }
    Text {
        id: txtHeadline
        color: "#5D5D5D"
        text: settingsBool.headline
        renderType: Text.NativeRendering
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        anchors{
            top:parent.top
            topMargin: 6
            left:parent.left
            leftMargin: 20

        }

        font.pixelSize: 16
        font.family: font_Roboto_Regular.name
    }

    Text {
        id: txtDescription
        text: settingsBool.description
        color: "#B5B5B5"
        renderType: Text.NativeRendering
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        font.pixelSize: 12
        font.family: font_Roboto_Regular.name
        anchors{
            top:txtHeadline.bottom
            topMargin: 6
            left:parent.left
            leftMargin: 20
        }
    }


    ComboBox {

        anchors {
            right: parent.right
            rightMargin: 20
            verticalCenter: parent.verticalCenter
        }

        model: ListModel {
            id: model
            ListElement { text: "CUDA" }
            ListElement { text: "D3D11" }
            ListElement { text: "DXVA" }
            ListElement { text: "VAAPI" }
            ListElement { text: "FFmpeg" }
        }
        onAccepted: {

        }
    }
}
