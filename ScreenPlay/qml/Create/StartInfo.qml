import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import ScreenPlay
import ScreenPlayCore as Util

Item {
    id: root

    Util.Headline {
        id: headline

        text: qsTr("Free tools to help you to create wallpaper")

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
        font.pointSize: 12
        font.family: App.settings.font
        text: qsTr("Below you can find tools to create wallaper, beyond the tools that ScreenPlay provides for you!")

        anchors {
            top: headline.bottom
            right: parent.right
            left: parent.left
            margins: 20
        }
    }

    GridView {
        id: gridView

        boundsBehavior: Flickable.DragOverBounds
        maximumFlickVelocity: 2500
        flickDeceleration: 500
        clip: true
        cellWidth: 180
        cellHeight: 280

        anchors {
            top: introText.bottom
            right: parent.right
            bottom: parent.bottom
            left: parent.left
            margins: 20
        }

        ScrollBar.vertical: ScrollBar {
            snapMode: ScrollBar.SnapOnRelease
        }

        model: ListModel {
            ListElement {
                text: "Subreddit"
                image: "qrc:/qt/qml/ScreenPlay/assets/startinfo/reddit.png"
                link: "https://www.reddit.com/r/ScreenPlayApp/"
                description: "Lorem ipsum dolor sit amet, consectetuer adipiscing elit."
                category: "Community"
            }

            ListElement {
                text: "Forums"
                image: "qrc:/qt/qml/ScreenPlay/assets/startinfo/forums.png"
                link: "https://forum.screen-play.app/"
                description: "Lorem ipsum dolor sit amet, consectetuer adipiscing elit."
                category: "Community"
            }

            ListElement {
                text: "QML Online Editor"
                image: "qrc:/qt/qml/ScreenPlay/assets/startinfo/qml_online.png"
                link: "https://qmlonline.kde.org/"
                description: "Lorem ipsum dolor sit amet, consectetuer adipiscing elit."
                category: "Tools"
            }

            ListElement {
                text: "Godot"
                image: "qrc:/qt/qml/ScreenPlay/assets/startinfo/godot.png"
                link: "https://godotengine.org/"
                description: "Lorem ipsum dolor sit amet, consectetuer adipiscing elit."
                category: "Tools"
            }

            ListElement {
                text: "Handbreak"
                image: "qrc:/qt/qml/ScreenPlay/assets/startinfo/handbreak.png"
                link: "https://handbrake.fr/"
                description: "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Cum sociis natoque penatibus et magnis dis parturient montes,"
                category: "Tools"
            }

            ListElement {
                text: "Blender"
                image: "qrc:/qt/qml/ScreenPlay/assets/startinfo/blender.png"
                link: "https://www.blender.org/"
                description: ""
                category: "Tools"
            }

            ListElement {
                text: "OBS Studio"
                image: "qrc:/qt/qml/ScreenPlay/assets/startinfo/obs.png"
                link: "https://obsproject.com/"
                description: ""
                category: "Tools"
            }

            ListElement {
                text: "Krita"
                image: "qrc:/qt/qml/ScreenPlay/assets/startinfo/krita.png"
                link: "https://krita.org/"
                description: ""
                category: "Tools"
            }

            ListElement {
                text: "Gimp"
                image: "qrc:/qt/qml/ScreenPlay/assets/startinfo/gimp.png"
                link: "https://gimp.org/"
                description: ""
                category: "Tools"
            }

            ListElement {
                text: "Inscape"
                image: "qrc:/qt/qml/ScreenPlay/assets/startinfo/inkscape.png"
                link: "https://inkscape.org/"
                description: ""
                category: "Tools"
            }

            ListElement {
                text: "Kdenlive"
                image: "qrc:/qt/qml/ScreenPlay/assets/startinfo/kdeenlive.png"
                link: "https://kdenlive.org/"
                description: ""
                category: "Tools"
            }

            ListElement {
                text: "ShareX"
                image: "qrc:/qt/qml/ScreenPlay/assets/startinfo/sharex.png"
                link: "https://getsharex.com/"
                description: ""
                category: "Tools"
            }

            ListElement {
                text: "GitLab"
                image: "qrc:/qt/qml/ScreenPlay/assets/startinfo/gitlab.png"
                link: "https://about.gitlab.com/"
                description: ""
                category: "Tools"
            }

            ListElement {
                text: "Git Extensions - Git UI for Windows"
                image: "qrc:/qt/qml/ScreenPlay/assets/startinfo/git_extentions.png"
                link: "https://gitextensions.github.io/"
                description: ""
                category: "Tools"
            }

            ListElement {
                text: "Visual Studio Code"
                image: "qrc:/qt/qml/ScreenPlay/assets/startinfo/vscode.png"
                link: "https://code.visualstudio.com/"
                description: ""
                category: "Tools"
            }

            ListElement {
                text: "Shadertoy"
                image: "qrc:/qt/qml/ScreenPlay/assets/startinfo/shadertoy.png"
                link: "https://www.shadertoy.com/"
                description: ""
                category: "Resources"
            }

            ListElement {
                text: "Flaticon"
                image: "qrc:/qt/qml/ScreenPlay/assets/startinfo/flaticon.png"
                link: "https://www.flaticon.com/"
                description: ""
                category: "Resources"
            }

            ListElement {
                text: "Unsplash"
                image: "qrc:/qt/qml/ScreenPlay/assets/startinfo/unsplash.png"
                link: "https://unsplash.com/"
                description: ""
                category: "Resources"
            }

            ListElement {
                text: "FreeSound"
                image: "qrc:/qt/qml/ScreenPlay/assets/startinfo/freesound.png"
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
