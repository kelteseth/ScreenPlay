import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import QtQuick.XmlListModel 2.0

GridView {
    id: changelogFlickableWrapper
    flickableDirection: Flickable.VerticalFlick
    maximumFlickVelocity: 5000
    flickDeceleration: 5000
    cellHeight: 205
    cellWidth: 360
    model: feedModel

    Timer {
        interval: 200
        running: true
        repeat: false
        onTriggered: {
            feedModel.source = "https://screen-play.app/index.php?option=com_content&view=category&layout=blog&id=10&format=feed&type=rss"
        }
    }

    XmlListModel {
        id: feedModel

        query: "/rss/channel/item"
        XmlRole {
            name: "title"
            query: "title/string()"
        }
        XmlRole {
            name: "backgroundImage"
            query: "description/string()"
        }
        XmlRole {
            name: "link"
            query: "link/string()"
        }
        XmlRole {
            name: "pubDate"
            query: "pubDate/string()"
        }
        XmlRole {
            name: "category"
            query: "category/string()"
        }
    }

    header: Item {
        height: 100
        width: parent.width

        Text {
            id: name
            text: qsTr("News & Patchnotes")
            wrapMode: Text.WordWrap
            color: "#626262"
            renderType: Text.NativeRendering
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            font.pixelSize: 32
            font.family: "Roboto"

            anchors {
                top: parent.top
                topMargin: 30
                horizontalCenter: parent.horizontalCenter
            }
        }
    }

    delegate: Item {
        id: root
        width: 352
        height: 197

        RectangularGlow {
            id: effectchangelogWrapper
            anchors {
                top: parent.top
                topMargin: 3
            }
            height: parent.height
            width: parent.width
            cached: true
            glowRadius: 3
            spread: 0.2
            color: "black"
            opacity: 0.2
            cornerRadius: 15
        }

        Rectangle {
            anchors.fill: parent
            anchors.margins: 5
            radius: 4

            Image {
                fillMode: Image.PreserveAspectCrop
                anchors.fill: parent
                source: {
                    var a = backgroundImage
                    var r = new RegExp(/<img[^>]+src="([^">]+)"/)
                    var url = r.exec(a)
                    return url[1].toString()
                }
            }

            LinearGradient {
                visible: true
                opacity: .5
                anchors.fill: parent
                start: Qt.point(0, parent.height)
                end: Qt.point(0, parent.height - 150)
                gradient: Gradient {
                    GradientStop {
                        position: 0.0
                        color: "#BB000000"
                    }
                    GradientStop {
                        position: 1.0
                        color: "#00000000"
                    }
                }
            }

            Text {
                id: txtTitle
                text: title
                renderType: Text.NativeRendering
                anchors {
                    right: parent.right
                    bottom: parent.bottom
                    left: parent.left
                    margins: 20
                }
                color: "white"
                font.family: "Roboto"
                font.weight: Font.Normal
                font.pixelSize: 18
                wrapMode: Text.WordWrap
            }
            Text {
                id: txtPubDate
                text: {
                    return pubDate.replace("+0000", "")
                }
                anchors {
                    right: parent.right
                    rightMargin: 20
                    bottom: txtTitle.top
                    bottomMargin: 10
                    left: parent.left
                    leftMargin: 20
                }
                color: "white"
                font.family: "Roboto"
                renderType: Text.NativeRendering
                font.weight: Font.Normal
                font.pixelSize: 14
                wrapMode: Text.WordWrap
            }
            MouseArea {
                anchors.fill: parent
                onClicked: Qt.openUrlExternally(link)
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
            }
        }
    }

    ScrollBar.vertical: ScrollBar {
        snapMode: ScrollBar.SnapOnRelease
        policy: ScrollBar.AlwaysOn
    }
}
