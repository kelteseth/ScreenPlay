import QtQuick 2.12
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.14
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import ScreenPlay 1.0
import QtWebEngine 1.8

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
            background: Item {}
            anchors {
                top: parent.top
                left: parent.left
                bottom: parent.bottom
                right: parent.right
            }

            CommunityNavItem {
                text: qsTr("News")
                openLink: webModel.get(0).url
                icon.source: "qrc:/assets/icons/icon_document.svg"
            }

            CommunityNavItem {
                text: qsTr("Wiki")
                openLink: webModel.get(1).url
                icon.source: "qrc:/assets/icons/icon_help_center.svg"
            }

            CommunityNavItem {
                text: qsTr("Forum")
                openLink: webModel.get(2).url
                icon.source: "qrc:/assets/icons/icon_forum.svg"
            }
            CommunityNavItem {
                text: qsTr("Issue List")
                openLink: webModel.get(3).url
                icon.source: "qrc:/assets/icons/icon_report_problem.svg"
            }
            CommunityNavItem {
                text: qsTr("Contribute")
                openLink: webModel.get(4).url
                icon.source: "qrc:/assets/icons/icon_supervisor_account.svg"
            }
            CommunityNavItem {
                text: qsTr("Steam Workshop")
                openLink: "steam://url/GameHub/672870"
                icon.source: "qrc:/assets/icons/icon_steam.svg"
            }
        }
    }

    LinearGradient {
        height: 6
        z: 99
        anchors {
            top: navWrapper.bottom
            left: parent.left
            right: parent.right
        }

        start: Qt.point(0, 0)
        end: Qt.point(0, 6)
        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: "#33333333"
            }
            GradientStop {
                position: 1.0
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

            Loader {
                active: SwipeView.isCurrentItem || SwipeView.isNextItem
                        || SwipeView.isPreviousItem
                asynchronous: true
                sourceComponent: Item {
                    Component.onCompleted: timer.start()
                    Timer {
                        id: timer
                        interval: 200
                        onTriggered: webView.url = webModel.get(index).url
                    }

                    WebEngineView {
                        id: webView
                        anchors.fill: parent
                    }
                }
            }
        }
    }

    Binding{
        target: nav
        property: "currentIndex"
        value: swipeView.currentIndex
    }
}
