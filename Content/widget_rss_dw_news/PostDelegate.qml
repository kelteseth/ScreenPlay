// SPDX-License-Identifier: BSD-3-Clause

import QtQuick
import QtQuick.Layouts

Item {
    id: root

    implicitHeight: 100
    implicitWidth: 300

    property string published
    property string rights
    property string updated
    property string category
    property string title
    property string link
    property string summary

    ColumnLayout {
        anchors.fill: parent
        Text {
            Layout.fillWidth: true
            font.pointSize: 14
            wrapMode: Text.Wrap
            text: model.title
            height: 20
            color: "white"
        }

        Text {
            Layout.fillWidth: true
            wrapMode: Text.Wrap
            text: model.summary
            font.pointSize: 10
            height: 20
            color: "white"
        }
    }
    MouseArea {
        anchors.fill: parent
        onClicked: {
            print(model.link);
            Qt.openUrlExternally(model.link);
        }
    }
}
