import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.3
import ScreenPlay 1.0

Item {
    id: tag
    width: textMetrics.width + 20
    height: parent.height

    property int itemIndex
    property alias text: txt.text
    signal removeThis(var index)

    Rectangle {
        id: rectangle
        radius: 3
        color: Material.theme === Material.Light ? Qt.lighter(
                                                       Material.background) : Qt.darker(
                                                       Material.background)

        Text {
            id: txt
            text: _name
            color: Material.theme === Material.Light ? Qt.lighter(
                                                           Material.foreground) : Qt.darker(
                                                           Material.foreground)

            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            anchors.fill: parent
            font.family: ScreenPlay.settings.font
        }
        TextField {
            id: textField
            enabled: false
            opacity: 0
            anchors.fill: parent
            anchors.margins: 10
            anchors.bottomMargin: 0
            font.family: ScreenPlay.settings.font
        }

        TextMetrics {
            id: textMetrics
            text: txt.text
            font.pointSize: 14
            font.family: ScreenPlay.settings.font
        }
    }
    MouseArea {
        id: ma
        width: 10
        height: width
        cursorShape: Qt.PointingHandCursor
        anchors {
            top: parent.top
            right: parent.right
            margins: 5
        }
        onClicked: {
            tag.removeThis(itemIndex)
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

/*##^## Designer {
    D{i:0;height:50;width:100}
}
 ##^##*/

