import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import Qt5Compat.GraphicalEffects
import QtQuick.Layouts
import QtQml.XmlListModel
import ScreenPlayApp
import ScreenPlay

Item {
    id: root

    GridView {
        id: changelogFlickableWrapper

        flickableDirection: Flickable.VerticalFlick
        maximumFlickVelocity: 5000
        flickDeceleration: 5000
        cellHeight: 250
        cellWidth: 450
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
            query: "/rss/channel/item"

            XmlListModelRole {
                name: "title"
                elementName: "title"
            }

            XmlListModelRole {
                name: "image"
                elementName: "image"
            }

            XmlListModelRole {
                name: "pubDate"
                elementName: "pubDate"
            }

            XmlListModelRole {
                name: "link"
                elementName: "link"
            }

            XmlListModelRole {
                name: "description"
                elementName: "description"
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
                font.family: App.settings.font
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

                    Behavior on opacity  {
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
                            position: 1
                            color: "#ee111111"
                        }

                        GradientStop {
                            position: 0
                            color: "transparent"
                        }
                    }
                }

                Text {
                    id: txtTitle

                    text: title
                    color: Material.primaryTextColor
                    font.family: App.settings.font
                    font.weight: Font.Normal
                    font.pointSize: 14
                    wrapMode: Text.WordWrap

                    anchors {
                        right: parent.right
                        bottom: parent.bottom
                        left: parent.left
                        margins: 20
                    }
                }

                Text {
                    id: txtPubDate

                    text: pubDate
                    color: Material.primaryTextColor
                    font.family: App.settings.font
                    font.pointSize: 8
                    wrapMode: Text.WordWrap

                    anchors {
                        right: parent.right
                        rightMargin: 20
                        bottom: txtTitle.top
                        bottomMargin: 10
                        left: parent.left
                        leftMargin: 20
                    }
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
