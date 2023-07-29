
// SPDX-License-Identifier: BSD-3-Clause
import QtQuick
import QtQuick.Layouts

Item {
    id: root

    implicitHeight: 80
    implicitWidth: 300

    property string published
    property string rights
    property string updated
    property string category
    property string title
    property string link
    property string mediaContent
    onMediaContentChanged: {
        print("src")
        const src = parseItem(model.mediaContent, 'url="', '"')
        print("src", src)
        //img.source = src;
    }

    function parseItem(raw, startTag, endTag) {
        var startIdx = raw.indexOf(startTag) + startTag.length
        var endIdx = raw.indexOf(endTag, startIdx)
        return raw.substring(startIdx, endIdx)
    }

    RowLayout {
        anchors.fill: parent
        Image {
            id: img
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
        Text {
            Layout.fillHeight: true
            Layout.fillWidth: true
            font.pointSize: 12
            wrapMode: Text.Wrap
            text: model.title
            height: 20
            color: "white"
        }
    }
    MouseArea {
        anchors.fill: parent
        onClicked: {
            print(model.category)
            print(model.mediaContent)
            //Qt.openUrlExternally(model.link);
        }
    }
}
