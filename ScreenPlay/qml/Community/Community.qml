import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import Qt5Compat.GraphicalEffects
import QtQuick.Layouts
import ScreenPlay 1.0
import QtWebEngine

Item {
    id: root

    Rectangle {
        id: navWrapper

        color: Material.theme === Material.Light ? "white" : Material.background
        height: 50

        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
        }

        TabBar {
            id: nav

            height: parent.height
            currentIndex: 0

            anchors {
                top: parent.top
                left: parent.left
                bottom: parent.bottom
                right: parent.right
            }

            CommunityNavItem {
                text: qsTr("News")
                openLink: "https://screen-play.app/blog/"
                icon.source: "qrc:/assets/icons/icon_document.svg"
            }

            CommunityNavItem {
                text: qsTr("Wiki")
                openLink: "https://kelteseth.gitlab.io/ScreenPlayDocs/"
                icon.source: "qrc:/assets/icons/icon_help_center.svg"
            }

            CommunityNavItem {
                text: qsTr("Forum")
                openLink: "https://forum.screen-play.app/"
                icon.source: "qrc:/assets/icons/icon_forum.svg"
            }

            CommunityNavItem {
                text: qsTr("Issue List")
                openLink: "https://gitlab.com/kelteseth/ScreenPlay/-/issues"
                icon.source: "qrc:/assets/icons/icon_report_problem.svg"
            }

            CommunityNavItem {
                text: qsTr("Contribute")
                openLink: "https://gitlab.com/kelteseth/ScreenPlay#general-contributing"
                icon.source: "qrc:/assets/icons/icon_supervisor_account.svg"
            }

            CommunityNavItem {
                text: qsTr("Steam Workshop")
                openLink: "steam://url/GameHub/672870"
                icon.source: "qrc:/assets/icons/icon_steam.svg"
            }

            background: Item {
            }

        }

    }

    LinearGradient {
        height: 6
        z: 99
        start: Qt.point(0, 0)
        end: Qt.point(0, 6)

        anchors {
            top: navWrapper.bottom
            left: parent.left
            right: parent.right
        }

        gradient: Gradient {
            GradientStop {
                position: 0
                color: "#33333333"
            }

            GradientStop {
                position: 1
                color: "transparent"
            }

        }

    }

    SwipeView {
        id: swipeView

        currentIndex: nav.currentIndex

        anchors {
            top: navWrapper.bottom
            right: parent.right
            bottom: parent.bottom
            left: parent.left
        }

        XMLNewsfeed {
        }

        Repeater {
            id: repeater

            Loader {
                active: SwipeView.isCurrentItem || SwipeView.isNextItem || SwipeView.isPreviousItem
                asynchronous: true

                sourceComponent: Item {
                    Component.onCompleted: timer.start()

                    Timer {
                        id: timer

                        interval: 200
                        onTriggered: webView.url = webModel.get(index + 1).url
                    }

                    WebEngineView {
                        id: webView

                        anchors.fill: parent
                    }

                }

            }

            model: ListModel {
                id: webModel

                ListElement {
                    url: "https://screen-play.app/blog/"
                }

                ListElement {
                    url: "https://kelteseth.gitlab.io/ScreenPlayDocs/"
                }

                ListElement {
                    url: "https://forum.screen-play.app/"
                }

                ListElement {
                    url: "https://gitlab.com/kelteseth/ScreenPlay/-/issues"
                }

                ListElement {
                    url: "https://gitlab.com/kelteseth/ScreenPlay#general-contributing"
                }

                ListElement {
                    url: "https://steamcommunity.com/app/672870/workshop/"
                }

            }

        }

    }

    Binding {
        target: nav
        property: "currentIndex"
        value: swipeView.currentIndex
    }

}
