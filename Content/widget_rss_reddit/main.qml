// SPDX-License-Identifier: BSD-3-Clause

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Particles
import QtQml.XmlListModel

Item {
    id: root
    implicitWidth: 480
    implicitHeight: 480

    property string subreddit: "funny"

    XmlListModel {
        id: feedModel
        onStatusChanged: {
            print("status ",status)

            if (status === XmlListModel.Error) {
                console.log("Error: " + errorString);
            }
        }

        source: "https://www.reddit.com/r/" + root.subreddit + "/.rss"
        query: "/feed/entry"

        XmlListModelRole { name: "updated"; elementName: "updated" }
        XmlListModelRole { name: "subtitle"; elementName: "subtitle"}
        XmlListModelRole { name: "content"; elementName: "content"}
        XmlListModelRole { name: "link"; elementName: "link"; attributeName: "href" }
        XmlListModelRole { name: "title"; elementName: "title"}

    }

    ListView {
        id: list
        anchors.fill: parent
        model: feedModel
        delegate: PostDelegate {
            width: parent.width
        }
    }
}
