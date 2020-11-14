import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.15
import QtQuick.Controls.Material 2.12
import QtQuick.Particles 2.0
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material.impl 2.12

import ScreenPlay 1.0
import ScreenPlay.Create 1.0
import ScreenPlay.QMLUtilities 1.0

Item {
    id: root

    Text {
        id: introText
        color: Material.primaryTextColor
        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
            margins: 20
        }
        font.pointSize: 14
        font.family: ScreenPlay.settings.font
        text: "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor.
Aenean massa. \n\n\nCum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. "
    }

    GridView {
        id: gridView
        anchors {
            top: introText.bottom
            right: parent.right
            bottom: parent.bottom
            left: parent.left
            margins: 20
        }
        clip: true
        cellWidth: 186
        cellHeight: 280
        model: ListModel {
            ListElement {
                text: "Subreddit"
                image: "qrc:/qml/Create/StartInfo.qml"
                link: "https://www.reddit.com/r/ScreenPlayApp/"
                description: "Lorem ipsum dolor sit amet, consectetuer adipiscing elit."
                category: "Community"
            }
            ListElement {
                text: "Forums"
                image: "qrc:/qml/Create/StartInfo.qml"
                link: "https://forum.screen-play.app/"
                description: "Lorem ipsum dolor sit amet, consectetuer adipiscing elit."
                category: "Community"
            }
            ListElement {
                text: "QML Online Editor"
                image: "qrc:/qml/Create/StartInfo.qml"
                link: "https://qmlonline.kde.org/"
                description: "Lorem ipsum dolor sit amet, consectetuer adipiscing elit."
                category: "Tools"
            }
            ListElement {
                text: "Handbreak"
                image: "qrc:/qml/Create/StartInfo.qml"
                link: "https://handbrake.fr/"
                description: "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Cum sociis natoque penatibus et magnis dis parturient montes,"
                category: "Tools"
            }
            ListElement {
                text: "Blender"
                image: "qrc:/qml/Create/StartInfo.qml"
                link: "https://www.blender.org/"
                description: ""
                category: "Tools"
            }
            ListElement {
                text: "OBS Studio"
                image: "qrc:/qml/Create/StartInfo.qml"
                link: "https://obsproject.com/"
                description: ""
                category: "Tools"
            }
            ListElement {
                text: "Krita"
                image: "qrc:/qml/Create/StartInfo.qml"
                link: "https://krita.org/"
                description: ""
                category: "Tools"
            }
            ListElement {
                text: "Inscape"
                image: "qrc:/qml/Create/StartInfo.qml"
                link: "https://inkscape.org/"
                description: ""
                category: "Tools"
            }
            ListElement {
                text: "Kdenlive"
                image: "qrc:/qml/Create/StartInfo.qml"
                link: "https://kdenlive.org/"
                description: ""
                category: "Tools"
            }
            ListElement {
                text: "ShareX"
                image: "qrc:/qml/Create/StartInfo.qml"
                link: "https://getsharex.com/"
                description: ""
                category: "Tools"
            }
            ListElement {
                text: "GitLab"
                image: "qrc:/qml/Create/StartInfo.qml"
                link: "https://about.gitlab.com/"
                description: ""
                category: "Tools"
            }
            ListElement {
                text: "Git Extensions - Git UI for Windows"
                image: "qrc:/qml/Create/StartInfo.qml"
                link: "https://gitextensions.github.io/"
                description: ""
                category: "Tools"
            }
            ListElement {
                text: "Visual Studio Code"
                image: "qrc:/qml/Create/StartInfo.qml"
                link: "https://code.visualstudio.com/"
                description: ""
                category: "Tools"
            }
            ListElement {
                text: "Shadertoy"
                image: "qrc:/qml/Create/StartInfo.qml"
                link: "https://www.shadertoy.com/"
                description: ""
                category: "Resources"
            }
            ListElement {
                text: "Flaticon"
                image: "qrc:/qml/Create/StartInfo.qml"
                link: "https://www.flaticon.com/"
                description: ""
                category: "Resources"
            }
            ListElement {
                text: "Unsplash"
                image: "qrc:/qml/Create/StartInfo.qml"
                link: "https://unsplash.com/"
                description: ""
                category: "Resources"
            }
            ListElement {
                text: "FreeSound"
                image: "qrc:/qml/Create/StartInfo.qml"
                link: "https://freesound.org/"
                description: ""
                category: "Resources"
            }
        }

        delegate: ItemDelegate {
            width: gridView.cellWidth
            height: gridView.cellHeight

            Rectangle {
                id: img
                anchors.fill: parent
                anchors.margins: 5
                radius: 3
                clip: true
                layer.enabled: true
                layer.effect: ElevationEffect {
                    elevation: 4
                }

                Rectangle {
                    color: "orange"
                    height: 180
                    radius: 3
                    clip: true
                    anchors {
                        top: parent.top
                        right: parent.right
                        left: parent.left
                    }

                    LinearGradient {
                        anchors.fill: parent
                        end: Qt.point(0, 0)
                        start: Qt.point(0, parent.height * .66)
                        gradient: Gradient {
                            GradientStop {
                                position: 0.0
                                color: "#AA000000"
                            }
                            GradientStop {
                                position: 1.0
                                color: "#00000000"
                            }
                        }
                    }

                    Text {
                        text: model.text
                        font.pointSize: 14
                        font.family: ScreenPlay.settings.font
                        color: "white"
                        anchors {
                            left: parent.left
                            right: parent.right
                            bottom: parent.bottom
                            margins: 5
                        }
                    }
                    Text {
                        text: model.category
                        font.pointSize: 11
                        font.family: ScreenPlay.settings.font
                        color: "white"
                        anchors {
                            left: parent.left
                            right: parent.right
                            margins: 5
                        }
                    }
                }

                Rectangle {
                    color: Material.backgroundDimColor
                    anchors {
                        top: img.bottom
                        right: parent.right
                        left: parent.left
                        bottom: parent.bottom
                    }

                    Text {
                        id: description
                        text: model.description
                        font.pointSize: 14
                        font.family: ScreenPlay.settings.font
                        color: Material.primaryTextColor
                        anchors {
                            fill: parent
                            margins: 5
                        }
                    }
                }
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    hoverEnabled: true
                    onClicked: Qt.openUrlExternally(model.link)
                }
            }
        }
    }
}
