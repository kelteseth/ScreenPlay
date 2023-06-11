// SPDX-License-Identifier: BSD-3-Clause

import QtQuick
import QtQuick.Layouts

Item {
    id: root

    implicitHeight: 300
    implicitWidth: 300

    property string contentRaw: model.content
    property url href
    onContentRawChanged: {
        // Define the regular expression to match the image URL
        var regex = /<a\s+href="((?:https?:\/\/)?i\.redd\.it\/[^"]+\.jpg)">\[link\]<\/a>/i;

        // Parse the image URL from the HTML content
        var match = regex.exec(contentRaw);
        if (match != null) {
            var imageUrl = match[1];
            var href = match[2];
            img.source = "" + imageUrl;
            root.href = "" + href;
        }
    }

    Image {
        id: img
        asynchronous: true
        anchors.fill: parent
        clip: true
        fillMode: Image.PreserveAspectCrop
    }
    Rectangle {
        anchors.fill: img
        gradient: Gradient {
            GradientStop {
                position: 0
                color: "#00333333"
            }
            GradientStop {
                position: 1
                color: "#ff333333"
            }
        }
    }

    Text {
        anchors {
            right: parent.right
            bottom: parent.bottom
            left: parent.left
            leftMargin: 10
        }
        wrapMode: Text.Wrap
        width: parent.width
        text: model.title
        height: 20
        color: "white"
    }
    MouseArea {
        anchors.fill: parent
        onClicked: {
            print(model.link);
            Qt.openUrlExternally(model.link);
        }
    }
}
