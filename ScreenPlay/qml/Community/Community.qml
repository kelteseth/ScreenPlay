import QtQuick 2.12
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import ScreenPlay 1.0
import QtWebEngine 1.8

Item {
    id: community
    anchors.fill: parent

    Rectangle {
        id: stomtWrapper
        width: 500
        color: "gray"
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
        }
        LinearGradient {
            id: tabShadow
            cached: true
            anchors.fill: parent
            start: Qt.point(0, 0)
            end: Qt.point(stomtWrapper.width, stomtWrapper.height)
            gradient: Gradient {
                GradientStop {
                    position: 0.0
                    color: "#199EF1"
                }
                GradientStop {
                    position: 1.0
                    color: "#092E6C"
                }
            }
        }
        Image {
            id: imgStomt
            source: "qrc:/assets/images/Stomt_Logo+Wordmark_Mono_BLK.svg"
            width: 250
            height: 250
            sourceSize: Qt.size(250, 250)
            anchors {
                top: parent.top
                topMargin: -30
                horizontalCenter: parent.horizontalCenter
            }
            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: Qt.openUrlExternally("https://stomt.com/")
            }
        }
        Text {
            id: txtStomtDescription
            font.pointSize: 14
            color: "white"
            height: 100
            text: qsTr("We use Stomt because it provides quick and easy feedback via I like/I wish. So you can easily give us feedback and speak your mind. We will read these wishes on a daily basis!")
            font.family: ScreenPlay.settings.font
            font.weight: Font.Normal
            wrapMode: Text.WordWrap
            horizontalAlignment: Qt.AlignHCenter
            
            anchors {
                top: imgStomt.bottom
                topMargin: -50
                right: parent.right
                rightMargin: 50
                left: parent.left
                leftMargin: 50
            }
        }

        Component {
            id: component_feedback
            Feedback {
                id: feedback
                anchors.fill: parent
            }
        }
        Loader {
            id: loader_feedback
            height: 236
            width: 420
            anchors {
                top: txtStomtDescription.bottom
                horizontalCenter: parent.horizontalCenter
            }
        }

        Timer {
            interval: 200
            running: true
            repeat: false
            onTriggered: {
                loader_feedback.sourceComponent = component_feedback
            }
        }

        Button {
            id: btnStomt
            text: qsTr("Open ScreenPlay Stomt page")
            anchors {
                bottom: parent.bottom
                bottomMargin: 30
                horizontalCenter: parent.horizontalCenter
            }
            Material.background: Material.Orange
            Material.foreground: "white"

            icon.source: "qrc:/assets/icons/icon_share.svg"
            icon.color: "white"
            icon.width: 16
            icon.height: 16
            onClicked: {
                Qt.openUrlExternally("https://www.stomt.com/screenplay")
            }
        }
    }

    LinearGradient {
        id: tabShadowFoot
        height: 4
        z: 500
        cached: true
        anchors {
            bottom: footer.top
            right: parent.right
            left: stomtWrapper.right
        }
        start: Qt.point(0, 4)
        end: Qt.point(0, 0)
        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: "#33000000"
            }
            GradientStop {
                position: 1.0
                color: "#00000000"
            }
        }
    }

    //    XMLNewsfeed {
    //        id: changelogFlickableWrapper
    //        anchors {
    //            bottom: footer.top
    //            left: stomtWrapper.right
    //            leftMargin: 90
    //            right: parent.right
    //            rightMargin: 20
    //            top: parent.top
    //            topMargin: 20
    //        }
    //    }
    WebEngineView {
        id: we
        url:"https://forum.screen-play.app/"
        onUrlChanged: {
            var tmp = we.url.toString()
            if(!tmp.includes("https://forum.screen-play.app/")) {
                we.url = "https://forum.screen-play.app/"
            }
        }

        anchors {
            bottom: footer.top
            left: stomtWrapper.right
            leftMargin: 0
            right: parent.right
            rightMargin: 0
            top: parent.top
            topMargin:0
        }

    }

    Rectangle {
        id: footer
        height: 80
        z: 100
        anchors {
            right: parent.right
            bottom: parent.bottom
            left: stomtWrapper.right
        }
        Row {
            anchors.centerIn: parent
            spacing: 20
            Button {
                text: qsTr("Forums")
                Material.background: Material.Blue
                Material.foreground: "white"
                font.family: ScreenPlay.settings.font
                onClicked: Qt.openUrlExternally(
                               "https://forum.screen-play.app/")
                icon.source: "qrc:/assets/icons/icon_people.svg"
                icon.color: "white"
                icon.width: 16
                icon.height: 16
            }
            Button {
                text: qsTr("Blog")
                Material.background: Material.LightGreen
                Material.foreground: "white"
                font.family: ScreenPlay.settings.font
                icon.source: "qrc:/assets/icons/icon_document.svg"
                icon.color: "white"
                icon.width: 16
                icon.height: 16
                onClicked: Qt.openUrlExternally(
                               "https://screen-play.app/blog/")
            }
            Button {
                text: qsTr("Source Code")
                Material.background: Material.Orange
                Material.foreground: "white"
                font.family: ScreenPlay.settings.font
                icon.source: "qrc:/assets/icons/icon_code.svg"
                icon.color: "white"
                icon.width: 16
                icon.height: 16
                onClicked: Qt.openUrlExternally(
                               "https://gitlab.com/kelteseth/ScreenPlay/")
            }
            Button {
                text: qsTr("Workshop")
                Material.background: Material.Red
                Material.foreground: "white"
                font.family: ScreenPlay.settings.font
                icon.source: "qrc:/assets/icons/icon_steam.svg"
                icon.color: "white"
                icon.width: 16
                icon.height: 16
                onClicked: Qt.openUrlExternally(
                               "steam://url/SteamWorkshopPage/672870")
            }
            Button {
                text: qsTr("Changelog")
                Material.background: Material.Purple
                Material.foreground: "white"
                font.family: ScreenPlay.settings.font
                icon.source: "qrc:/assets/icons/icon_info.svg"
                icon.color: "white"
                icon.width: 16
                icon.height: 16
                onClicked: Qt.openUrlExternally(
                               "https://gitlab.com/kelteseth/ScreenPlay/-/releases")
            }
        }
    }
}
