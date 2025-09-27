import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts
import QtQuick.Effects
import ScreenPlay
import ScreenPlayCore

Item {
    id: settingsHeader
    property string text: "HEADLINE"
    property url image: "qrc:/qt/qml/ScreenPlay/assets/icons/icon_settings.svg"

    width: parent.width
    height: 60
    Rectangle {
        color: Material.color(Material.Orange)
        topLeftRadius: 5
        topRightRadius: 5
        anchors {
            fill: parent
        }
        RowLayout {
            anchors {
                fill: parent
                margins: 20
            }
            spacing: 10
            ColorImage {
                id: imgIcon
                fillMode: Image.PreserveAspectFit
                source: settingsHeader.image
                height: 20
                width: 20
                color: "white"
                sourceSize: Qt.size(width, width)
                Layout.alignment: Qt.AlignVCenter
            }
            Text {
                id: txtHeadline
                text: settingsHeader.text
                font.pointSize: 14
                color: "white"
                verticalAlignment: Text.AlignVCenter
                font.family: App.settings.font
                Layout.alignment: Qt.AlignVCenter
            }
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }
}
