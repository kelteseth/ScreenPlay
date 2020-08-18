import QtQuick 2.12
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.14
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import ScreenPlay 1.0
import QtWebEngine 1.8

Item {
    id: community
    anchors.fill: parent

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
            background: Item {}
            anchors {
                top: parent.top
                left: parent.left
                bottom: parent.bottom
                right: parent.right
            }

            CommunityNavItem {
                text: qsTr("Forum")
                openLink: swipeView.itemAt(0).url
                icon.source: "qrc:/assets/icons/icon_forum.svg"
            }
            CommunityNavItem {
                text: qsTr("Wiki")
                openLink: swipeView.itemAt(1).url
                icon.source: "qrc:/assets/icons/icon_help_center.svg"
            }
            CommunityNavItem {
                text: qsTr("Issues List")
                openLink: swipeView.itemAt(2).url
                icon.source: "qrc:/assets/icons/icon_report_problem.svg"
            }
            CommunityNavItem {
                text: qsTr("Release Notes")
                openLink: swipeView.itemAt(3).url
                icon.source: "qrc:/assets/icons/icon_new_releases.svg"
            }
            CommunityNavItem {
                text: qsTr("Contribution Guide")
                openLink: swipeView.itemAt(4).url
                icon.source: "qrc:/assets/icons/icon_supervisor_account.svg"
            }
            CommunityNavItem {
                enabled: false
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

        WebEngineView {
            url: "https://forum.screen-play.app/"
        }

        WebEngineView {
            url: "https://kelteseth.gitlab.io/ScreenPlayDocs/"
        }

        WebEngineView {
            url: "https://gitlab.com/kelteseth/ScreenPlay/-/issues"
        }

        WebEngineView {
            url: "https://gitlab.com/kelteseth/ScreenPlay/-/releases"
        }

        WebEngineView {
            url: "https://gitlab.com/kelteseth/ScreenPlay#contributing-for-none-programmer"
        }

        WebEngineView {
            url: "https://steamcommunity.com/app/672870/workshop/"
        }
    }
}
