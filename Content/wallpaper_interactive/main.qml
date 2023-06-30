// SPDX-License-Identifier: BSD-3-Clause

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

Rectangle {
    id: root
    color: "#333333"
    anchors.fill: parent

    Button {
        anchors.centerIn: parent
        text: "Click me!"
    }
}
