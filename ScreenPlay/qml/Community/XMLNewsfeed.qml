import QtQuick 2.12
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import QtQuick.XmlListModel 2.12
import ScreenPlay 1.0

Item {
    id: root

    GridView {
        id: changelogFlickableWrapper
        flickableDirection: Flickable.VerticalFlick
        maximumFlickVelocity: 5000
        flickDeceleration: 5000
        cellHeight: 250
        cellWidth: 450
        clip: true

        model: feedModel
        anchors {
            top: parent.top
            right: parent.right
            bottom: parent.bottom
            left: parent.left
            leftMargin: 30
        }

        XmlListModel {
            id: feedModel
            source: "https://screen-play.app/blog/index.xml"
            onCountChanged: print(count)
            query: "/rss/channel/item"
            XmlRole {
                name: "title"
                query: "title/string()"
            }
            XmlRole {
                name: "image"
                query: "image/string()"
            }
            XmlRole {
                name: "pubDate"
                query: "pubDate/string()"
            }
            XmlRole {
                name: "link"
                query: "link/string()"
            }
            XmlRole {
                name: "description"
                query: "description/string()"
            }
        }

        header: Item {
            height: 100
            width: parent.width

            Text {
                id: name
                text: qsTr("News & Patchnotes")
                wrapMode: Text.WordWrap
                color: Material.primaryTextColor

                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignLeft
                font.pointSize: 32
                font.family: ScreenPlay.settings.font
                font.weight: Font.Light

                anchors {
                    top: parent.top
                    topMargin: 30
                    horizontalCenter: parent.horizontalCenter
                }
            }
        }

        delegate: Item {
            id: delegate
            width: changelogFlickableWrapper.cellWidth - 20
            height: changelogFlickableWrapper.cellHeight - 20

            Rectangle {
                anchors.fill: parent
                anchors.margins: 5
                color: Material.backgroundColor
                clip: true

                Image {
                    id: img
                    asynchronous: true
                    anchors.fill: parent
                    fillMode: Image.PreserveAspectCrop
                    source: image
                    opacity: status === Image.Ready ? 1 : 0
                    Behavior on opacity {
                        PropertyAnimation {
                            duration: 250
                        }
                    }
                }

                LinearGradient {
                    anchors.fill: parent

                    start: Qt.point(0, 0)
                    end: Qt.point(0, parent.height)
                    gradient: Gradient {
                        GradientStop {
                            position: 1.0
                            color: "#ee111111"
                        }
                        GradientStop {
                            position: 0.0
                            color: "transparent"
                        }
                    }
                }

                Text {
                    id: txtTitle
                    text: title

                    anchors {
                        right: parent.right
                        bottom: parent.bottom
                        left: parent.left
                        margins: 20
                    }
                    color: Material.primaryTextColor
                    font.family: ScreenPlay.settings.font
                    font.weight: Font.Normal
                    font.pointSize: 14
                    wrapMode: Text.WordWrap
                }

                Text {
                    id: txtPubDate
                    text: pubDate
                    anchors {
                        right: parent.right
                        rightMargin: 20
                        bottom: txtTitle.top
                        bottomMargin: 10
                        left: parent.left
                        leftMargin: 20
                    }
                    color: Material.primaryTextColor
                    font.family: ScreenPlay.settings.font

                    font.pointSize: 8
                    wrapMode: Text.WordWrap
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: Qt.openUrlExternally(link)
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onEntered: delegate.state = "hover"
                    onExited: delegate.state = ""
                }
            }
            transitions: [
                Transition {
                    from: ""
                    to: "hover"

                    ScaleAnimator {
                        target: img
                        duration: 80
                        from: 1
                        to: 1.05
                    }
                },
                Transition {
                    from: "hover"
                    to: ""

                    ScaleAnimator {
                        target: img
                        duration: 80
                        from: 1.05
                        to: 1
                    }
                }
            ]
        }

        ScrollBar.vertical: ScrollBar {
            snapMode: ScrollBar.SnapOnRelease
            policy: ScrollBar.AlwaysOn
        }
    }
}
