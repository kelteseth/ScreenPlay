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

import "../Common" as Common

Item {
    id: root

    Common.Headline {
        id: headline
        text: qsTr("Free Tools to create wallpaper")
        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
            margins: 20
        }
    }

    Text {
        id: introText
        color: Material.primaryTextColor
        anchors {
            top: headline.bottom
            right: parent.right
            left: parent.left
            margins: 20
        }
        font.pointSize: 12
        font.family: ScreenPlay.settings.font
        text: qsTr("Below you can find tools to create wallaper beyond the tools that ScreenPlay provides for you!")
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
                image: "qrc:/assets/startinfo/reddit.png"
                link: "https://www.reddit.com/r/ScreenPlayApp/"
                description: "Lorem ipsum dolor sit amet, consectetuer adipiscing elit."
                category: "Community"
            }
            ListElement {
                text: "Forums"
                image: "qrc:/assets/startinfo/forums.png"
                link: "https://forum.screen-play.app/"
                description: "Lorem ipsum dolor sit amet, consectetuer adipiscing elit."
                category: "Community"
            }
            ListElement {
                text: "QML Online Editor"
                image: "qrc:/assets/startinfo/qml_online.png"
                link: "https://qmlonline.kde.org/"
                description: "Lorem ipsum dolor sit amet, consectetuer adipiscing elit."
                category: "Tools"
            }
            ListElement {
                text: "Handbreak"
                image: "qrc:/assets/startinfo/handbreak.png"
                link: "https://handbrake.fr/"
                description: "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Cum sociis natoque penatibus et magnis dis parturient montes,"
                category: "Tools"
            }
            ListElement {
                text: "Blender"
                image: "qrc:/assets/startinfo/blender.png"
                link: "https://www.blender.org/"
                description: ""
                category: "Tools"
            }
            ListElement {
                text: "OBS Studio"
                image: "qrc:/assets/startinfo/obs.png"
                link: "https://obsproject.com/"
                description: ""
                category: "Tools"
            }
            ListElement {
                text: "Krita"
                image: "qrc:/assets/startinfo/krita.png"
                link: "https://krita.org/"
                description: ""
                category: "Tools"
            }
            ListElement {
                text: "Inscape"
                image: "qrc:/assets/startinfo/inkscape.png"
                link: "https://inkscape.org/"
                description: ""
                category: "Tools"
            }
            ListElement {
                text: "Kdenlive"
                image: "qrc:/assets/startinfo/kdeenlive.png"
                link: "https://kdenlive.org/"
                description: ""
                category: "Tools"
            }
            ListElement {
                text: "ShareX"
                image: "qrc:/assets/startinfo/sharex.png"
                link: "https://getsharex.com/"
                description: ""
                category: "Tools"
            }
            ListElement {
                text: "GitLab"
                image: "qrc:/assets/startinfo/gitlab.png"
                link: "https://about.gitlab.com/"
                description: ""
                category: "Tools"
            }
            ListElement {
                text: "Git Extensions - Git UI for Windows"
                image: "qrc:/assets/startinfo/git_extentions.png"
                link: "https://gitextensions.github.io/"
                description: ""
                category: "Tools"
            }
            ListElement {
                text: "Visual Studio Code"
                image: "qrc:/assets/startinfo/vscode.png"
                link: "https://code.visualstudio.com/"
                description: ""
                category: "Tools"
            }
            ListElement {
                text: "Shadertoy"
                image: "qrc:/assets/startinfo/shadertoy.png"
                link: "https://www.shadertoy.com/"
                description: ""
                category: "Resources"
            }
            ListElement {
                text: "Flaticon"
                image: "qrc:/assets/startinfo/flaticon.png"
                link: "https://www.flaticon.com/"
                description: ""
                category: "Resources"
            }
            ListElement {
                text: "Unsplash"
                image: "qrc:/assets/startinfo/unsplash.png"
                link: "https://unsplash.com/"
                description: ""
                category: "Resources"
            }
            ListElement {
                text: "FreeSound"
                image: "qrc:/assets/startinfo/freesound.png"
                link: "https://freesound.org/"
                description: ""
                category: "Resources"
            }
        }

        delegate: Item {
            width: gridView.cellWidth
            height: gridView.cellHeight

            Rectangle {
                id: img
                anchors.fill: parent
                anchors.margins: 5
                clip: true
                layer.enabled: true
                layer.effect: ElevationEffect {
                    elevation: 4
                }

                Image {
                    anchors.fill: parent
                    fillMode: Image.PreserveAspectCrop
                    source: model.image
                }

                LinearGradient {
                    anchors.fill: parent
                    end: Qt.point(0, 0)
                    start: Qt.point(0, parent.height * .66)
                    gradient: Gradient {
                        GradientStop {
                            position: 0.0
                            color: "#DD000000"
                        }
                        GradientStop {
                            position: 1.0
                            color: "#00000000"
                        }
                    }
                }
                Text {
                    id: txtCategory
                    text: model.category + ":"
                    font.pointSize: 10
                    font.family: ScreenPlay.settings.font
                    color: "white"
                    anchors {
                        left: parent.left
                        right: parent.right
                        bottom: txtText.top
                        margins: 15
                        bottomMargin: 5
                    }
                }
                Text {
                    id: txtText
                    text: model.text
                    font.pointSize: 16
                    font.family: ScreenPlay.settings.font
                    color: "white"
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                    anchors {
                        left: parent.left
                        right: parent.right
                        bottom: parent.bottom
                        margins: 15
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
