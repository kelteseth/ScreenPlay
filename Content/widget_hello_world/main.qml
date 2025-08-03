// SPDX-License-Identifier: BSD-3-Clause

import QtQuick // Base include for basic types like Item and Rectangle
import QtQuick.Layouts // Include for all layout types
import QtQuick.Controls // Include for all controls types like Slider
import QtQuick.Controls.Material // This includes tells qml to use Material style

Item {
    id: root
    implicitWidth: 350
    implicitHeight: 180

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        RowLayout {
            spacing : 20
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignHCenter

            Rectangle {
                implicitWidth: 100
                implicitHeight: 100
                Layout.alignment: Qt.AlignHCenter
                radius: 10
                color: Material.color(Material.Orange)
                // Bind the value of the slider
                // to the opacity of the rectangle
                opacity: sizeSlider.value
                // Animate the opacity change to make it smooth
                Behavior on opacity {
                    NumberAnimation {
                        duration: 100
                    }
                }
            }

            Label {
                text: "Slider value: " + Math.floor(sizeSlider.value * 100) + "%"
                Layout.alignment: Qt.AlignHCenter
            }
        }

        // See all available controls like Buttons, TextFields, etc. here:
        // https://doc.qt.io/qt-6/qtquick-controls2-qmlmodule.html
        Slider {
            id: sizeSlider
            Layout.alignment: Qt.AlignHCenter
            from: 0
            value: 0.8
            stepSize: 0.1
            to: 1
        }
    }
}
