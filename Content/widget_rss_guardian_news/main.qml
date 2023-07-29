
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
        source: "https://www.theguardian.com/world/rss"
        query: "/rss/channel/item"
        onStatusChanged: {
            if (status === XmlListModel.Error) {
                console.log("Error: " + errorString)
            }
        }

        XmlListModelRole {
            name: "title"
            elementName: "title"
        }
        XmlListModelRole {
            name: "link"
            elementName: "link"
        }
        XmlListModelRole {
            name: "description"
            elementName: "description"
        }
        XmlListModelRole {
            name: "category"
            elementName: "category"
            attributeName: "domain"
        }
        XmlListModelRole {
            name: "pubDate"
            elementName: "pubDate"
        }
        XmlListModelRole {
            name: "guid"
            elementName: "guid"
        }
        XmlListModelRole {
            name: "creator"
            elementName: "dc:creator"
        }
        XmlListModelRole {
            name: "date"
            elementName: "dc:date"
        }
        XmlListModelRole {
            name: "mediaContent"
            elementName: "media:content"
            attributeName: "url"
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
            width: root.width
        }
    }
}
