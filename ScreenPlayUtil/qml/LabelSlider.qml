import QtQuick
import QtQuick.Controls.Material 2.0 as QQCM
import QtQuick.Layouts
import QtQuick.Controls as QQC
import ScreenPlayApp

Item {
    id: root

    height: 80

    property string headline: "dummyHeandline"
    property string iconSource: ""
    property alias slider: qqcSlider

    property bool userInteracting: false
    property int debounceInterval: 100 // Delay in ms before emitting value
    signal valueEditingFinished(real value)

    Timer {
        id: debounceTimer
        interval: root.debounceInterval
        repeat: false
        onTriggered: {
            valueEditingFinished(qqcSlider.value)
        }
    }

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
            source: root.iconSource
            sourceSize: Qt.size(20, 20)
            Layout.alignment: Qt.AlignVCenter
        }

        QQC.Slider {
            id: qqcSlider
            stepSize: 0.01
            from: 0
            value: 1
            to: 1
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            snapMode: QQC.Slider.SnapAlways

            live: true // Keep visual updates smooth

            // Reset and start timer when value changes from user interaction
            onMoved: {
                debounceTimer.restart()
            }

            // Also handle the case when user releases the slider
            onPressedChanged: {
                if (!pressed) {
                    debounceTimer.restart()
                }
            }
        }

        Text {
            id: txtValue

            color: QQCM.Material.secondaryTextColor
            text: qqcSlider.value.toFixed(1)

            Layout.preferredWidth: 20
            Layout.preferredHeight: 20
            Layout.alignment: Qt.AlignVCenter
            font.pointSize: 12
            font.italic: true
            verticalAlignment: Text.AlignVCenter
        }
    }
}
