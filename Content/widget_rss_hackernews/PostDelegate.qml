// SPDX-License-Identifier: BSD-3-Clause
import QtQuick
import QtQuick.Layouts

Item {
    id: root
    height: wrapper.childrenRect.height

    property string points
    property string commentCount
    property string pubDate
    property string pubDateFormatted
    onPubDateChanged: {
        var date = new Date(pubDate);
        root.pubDateFormatted = date.toLocaleDateString(Qt.locale(), "ddd, dd MMM yyyy") + ' ' + date.toLocaleTimeString(Qt.locale(), "HH:mm:ss");
    }
    property string description
    onDescriptionChanged: {
        // See https://hnrss.org/frontpage <description> content
        // We need to manually parse it here to get the points and comments
        commentCount = parseCommentCount(description);
    }

    function parseCommentCount(raw) {
        var commentPrefix = "<p># Comments: ";
        var commentSuffix = "</p>";
        var startIdx = raw.indexOf(commentPrefix);
        if (startIdx === -1)
            return "N/A"; // return "N/A" if comment count is not found in the description
        startIdx += commentPrefix.length;
        var endIdx = raw.indexOf(commentSuffix, startIdx);
        return raw.substring(startIdx, endIdx);
    }
    function parsePoints(raw) {
        var pointsPrefix = "<p>Points: ";
        var pointsSuffix = "</p>";
        var startIdx = raw.indexOf(pointsPrefix);
        if (startIdx === -1)
            return "N/A"; // return "N/A" if points are not found in the description
        startIdx += pointsPrefix.length;
        var endIdx = raw.indexOf(pointsSuffix, startIdx);
        return raw.substring(startIdx, endIdx);
    }
    RowLayout {
        id: wrapper
        width: root.width
        spacing: 5
        ColumnLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true

            Text {
                id: titleText
                text: model.title
                wrapMode: Text.WordWrap
                font.pointSize: 14
                font.bold: true
                color: "white"
                Layout.maximumWidth: wrapper.width * .9
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        Qt.openUrlExternally(model.link);
                    }
                }
            }

            Text {
                id: descriptionText
                text: root.points + " Points • " + root.commentCount + " Comments 🔗 " + "• Published: " + root.pubDateFormatted
                wrapMode: Text.WordWrap
                font.pointSize: 10
                opacity: .7
                color: "white"
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        Qt.openUrlExternally(model.commentsLink);
                    }
                }
            }
        }
    }
}
