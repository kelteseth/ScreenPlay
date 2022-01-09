import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import Qt5Compat.GraphicalEffects
import QtQuick.Layouts
import ScreenPlay 1.0

Item {
    id: root

    XMLNewsfeed {
        anchors {
            top: navWrapper.bottom
            right: parent.right
            bottom: parent.bottom
            left: parent.left
        }
    }

    Rectangle {
        id: navWrapper

        color: Material.theme === Material.Light ? "white" : Material.background
        height: 60

        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
        }

        RowLayout {
            id: nav

            height: parent.height

            anchors {
                top: parent.top
                left: parent.left
                leftMargin: 20
                bottom: parent.bottom
            }

            Button {
                text: qsTr("News")
                onClicked: Qt.openUrlExternally("https://screen-play.app/blog/")
                icon.source: "qrc:/assets/icons/icon_document.svg"
                icon.width: 14
                icon.height: 14
                Layout.alignment: Qt.AlignVCenter
            }

            Button {
                text: qsTr("Wiki")
                onClicked: Qt.openUrlExternally("https://kelteseth.gitlab.io/ScreenPlayDocs/")
                icon.source: "qrc:/assets/icons/icon_help_center.svg"
                icon.width: 14
                icon.height: 14
                Layout.alignment: Qt.AlignVCenter
            }

            Button {
                text: qsTr("Forum")
                onClicked: Qt.openUrlExternally("https://forum.screen-play.app/")
                icon.source: "qrc:/assets/icons/icon_forum.svg"
                icon.width: 14
                icon.height: 14
                Layout.alignment: Qt.AlignVCenter
            }

            Button {
                text: qsTr("Reddit")
                onClicked: Qt.openUrlExternally("https://www.reddit.com/r/ScreenPlayApp/")
                icon.source: "qrc:/assets/icons/brand_reddit.svg"
                icon.width: 14
                icon.height: 14
                Layout.alignment: Qt.AlignVCenter
            }

            Button {
                text: qsTr("Issue Tracker")
                onClicked: Qt.openUrlExternally("https://gitlab.com/kelteseth/ScreenPlay/-/issues")
                icon.source: "qrc:/assets/icons/icon_report_problem.svg"
                icon.width: 14
                icon.height: 14
                Layout.alignment: Qt.AlignVCenter
            }

            Button {
                text: qsTr("Contribute")
                onClicked: Qt.openUrlExternally("https://gitlab.com/kelteseth/ScreenPlay#general-contributing")
                icon.source: "qrc:/assets/icons/icon_supervisor_account.svg"
                icon.width: 14
                icon.height: 14
                Layout.alignment: Qt.AlignVCenter
            }

            Button {
                text: qsTr("Steam Workshop")
                onClicked: Qt.openUrlExternally("steam://url/GameHub/672870")
                icon.source: "qrc:/assets/icons/icon_steam.svg"
                icon.width: 14
                icon.height: 14
                Layout.alignment: Qt.AlignVCenter
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


}
