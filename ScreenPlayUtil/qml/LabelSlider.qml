import QtQuick
import QtQuick.Controls.Material 2.0 as QQCM
import QtQuick.Layouts
import QtQuick.Controls as QQC
import ScreenPlayApp


Item {
    id: root

    property string headline: "dummyHeandline"
    property string iconSource: "qrc:/qml/ScreenPlayApp/assets/icons/icon_volume.svg"
    property alias slider: slider

    height: 80

    Text {
        id: txtHeadline

        text: headline
        height: 20
        font.pointSize: 14
        font.family: App.settings.font
        color: QQCM.Material.primaryTextColor

        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
        }
    }

    RowLayout {
        spacing: 15

        anchors {
            top: txtHeadline.bottom
            right: parent.right
            bottom: parent.bottom
            left: parent.left
        }

        ColorImage {
            id: imgIcon

            width: 20
            height: 20
            source: iconSource
            sourceSize: Qt.size(20, 20)
            Layout.alignment: Qt.AlignVCenter
        }

        QQC.Slider {
            id: slider

            stepSize: 0.01
            from: 0
            value: 1
            to: 1
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
        }

        Text {
            id: txtValue

            color: QQCM.Material.secondaryTextColor
            text: Math.round(slider.value * 100) / 100

            Layout.preferredWidth: 20
            Layout.preferredHeight: 20
            Layout.alignment: Qt.AlignVCenter
            font.pointSize: 12
            font.italic: true
            verticalAlignment: Text.AlignVCenter
        }
    }
}
