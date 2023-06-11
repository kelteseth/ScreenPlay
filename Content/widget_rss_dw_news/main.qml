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

    XmlListModel {
        id: feedModel
        source: "https://rss.dw.com/atom/rss-en-all"
        query: "/feed/entry"
        onCountChanged: {
            console.log("count ", count);
        }
        onStatusChanged: {
            print("status ", status);
            console.log("count ", count);
            if (status === XmlListModel.Error) {
                console.log("Error: " + errorString);
            }
        }

        XmlListModelRole {
            name: "published"
            elementName: "published"
        }
        XmlListModelRole {
            name: "rights"
            elementName: "rights"
        }
        XmlListModelRole {
            name: "updated"
            elementName: "updated"
        }
        XmlListModelRole {
            name: "category"
            elementName: "category"
            attributeName: "term"
        }
        XmlListModelRole {
            name: "title"
            elementName: "title"
        }
        XmlListModelRole {
            name: "link"
            elementName: "link"
            attributeName: "href"
        }
        XmlListModelRole {
            name: "summary"
            elementName: "summary"
        }
    }

    ListView {
        id: list
        anchors.fill: parent
        anchors.margins: 10
        anchors.topMargin: 20
        spacing: 10
        model: feedModel
        delegate: PostDelegate {
            width: parent.width
        }
    }
}
