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
        boundsBehavior: Flickable.DragOverBounds
        maximumFlickVelocity: 2500
        flickDeceleration: 500
        clip: true
        cellWidth: 186
        cellHeight: 280
        ScrollBar.vertical: ScrollBar {
            snapMode: ScrollBar.SnapOnRelease
        }
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
                text: "Godot"
                image: "qrc:/assets/startinfo/godot.png"
                link: "https://godotengine.org/"
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
                text: "Gimp"
                image: "qrc:/assets/startinfo/gimp.png"
                link: "https://gimp.org/"
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

        delegate: StartInfoLinkImage {
            id: delegate
            image: model.image
            category: model.category + ":"
            description: model.description
            text: model.text
            link: model.link
            width: gridView.cellWidth
            height: gridView.cellHeight
        }
    }
}
