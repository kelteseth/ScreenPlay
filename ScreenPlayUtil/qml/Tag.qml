import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import ScreenPlayApp 1.0
import ScreenPlay 1.0

Item {
    id: tag

    property int itemIndex
    property alias text: txt.text

    signal removeThis(var index)

    width: textMetrics.width + 20
    height: 45

    Rectangle {
        id: rectangle

        anchors.fill: parent
        radius: 3
        color: Material.theme === Material.Light ? Qt.lighter(Material.background) : Material.background

        Text {
            id: txt

            text: _name
            color: Material.primaryTextColor
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            anchors.fill: parent
            font.family: App.settings.font
        }

        TextField {
            id: textField

            enabled: false
            opacity: 0
            anchors.fill: parent
            anchors.margins: 10
            anchors.bottomMargin: 0
            font.family: App.settings.font
        }

        TextMetrics {
            id: textMetrics

            text: txt.text
            font.pointSize: 14
            font.family: App.settings.font
        }

    }

    MouseArea {
        id: ma

        width: 10
        height: width
        cursorShape: Qt.PointingHandCursor
        onClicked: {
            tag.removeThis(itemIndex);
        }

        anchors {
            top: parent.top
            right: parent.right
            margins: 5
        }

        Image {
            id: name

            anchors.fill: parent
            source: "qrc:/assets/icons/icon_close.svg"
        }

    }

    states: [
        State {
            name: "edit"

            PropertyChanges {
                target: txt
                opacity: 0
            }

            PropertyChanges {
                target: textField
                opacity: 1
                enabled: true
            }

        }
    ]
}
