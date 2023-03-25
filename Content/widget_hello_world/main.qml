// SPDX-License-Identifier: BSD-3-Clause

import QtQuick // Base include for basic types like Item and Rectangle
import QtQuick.Layouts // Include for all layout types
import QtQuick.Controls // Include for all controls types like Slider
import QtQuick.Controls.Material // This includes tells qml to use Material style

Item {
    id: root
    implicitWidth: 250
    implicitHeight: 360

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20

        Rectangle {
            width: 100
            height: 100
            radius: 10
            color: "steelBlue"
            // Bind the value of the slider
            // to the opacity of the rectangle
            opacity: sizeSlider.value
            // Animate the opacity change to make it smooth
            Behavior on opacity  {
                NumberAnimation {
                    duration: 100
                }
            }
        }

        Text {
            text: "Hi! Slider value: " + Math.floor(sizeSlider.value * 100) + "%"
            color: "white"
        }

        // See all available controls like Buttons, TextFields, etc. here:
        // https://doc.qt.io/qt-6/qtquick-controls2-qmlmodule.html
        Slider {
            id: sizeSlider
            from: 0
            value: 0.8
            stepSize: 0.4
            to: 1
        }
    }
}
