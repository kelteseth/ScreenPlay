// SPDX-License-Identifier: BSD-3-Clause
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQml.XmlListModel

Item {
    id: root
    implicitWidth: 480
    implicitHeight: 480
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10

        RowLayout {
            Layout.fillWidth: true
            ComboBox {
                id: combo
                Layout.fillWidth: true
                Layout.preferredWidth: 1
                Layout.preferredHeight: 50
                valueRole: "rssurl"
                textRole: "text"
                Material.foreground: "white"
                model: ListModel {
                    ListElement {
                        text: "Best"
                        rssurl: "https://hnrss.org/best"
                    }
                    ListElement {
                        text: "Front Page"
                        rssurl: "https://hnrss.org/frontpage"
                    }
                    ListElement {
                        text: "Jobs"
                        rssurl: "https://hnrss.org/jobs"
                    }
                    ListElement {
                        text: "Newest > 100 pts"
                        rssurl: "https://hnrss.org/newest?points=100"
                    }
                }
                onActivated: {
                    rssModel.source = combo.currentValue;
                }
            }
            ToolButton {
                text: "Reload"
                Material.foreground: "white"
                onPressed: rssModel.load()
            }
        }

        ListView {
            id: list
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 10
            clip: true
            model: rssModel
            delegate: PostDelegate {
                width: root.width
                description: model.description
                pubDate: model.pubDate
            }
        }
    }
    Timer {
        interval: 15 * 60 * 1000 // 15 minutes
        running: true
        repeat: true
        onTriggered: rssModel.load()
    }
    XmlListModel {
        id: rssModel
        source: combo.currentValue
        query: "/rss/channel/item"
        function load() {
            var tempSource = rssModel.source;
            rssModel.source = "";
            rssModel.source = tempSource;
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
            name: "creator"
            elementName: "dc:creator"
        }
        XmlListModelRole {
            name: "commentsLink"
            elementName: "comments"
        }
        XmlListModelRole {
            name: "guid"
            elementName: "guid"
        }
        XmlListModelRole {
            name: "description"
            elementName: "description"
        }
        XmlListModelRole {
            name: "pubDate"
            elementName: "pubDate"
        }
    }
}
