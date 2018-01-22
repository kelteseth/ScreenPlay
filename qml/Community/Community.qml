import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.2
import QtGraphicalEffects 1.0
Item {
    id: community
    anchors.fill: parent

    Rectangle {
        id:stomtWrapper
        width:500
        color: "gray"
        anchors {
            top:parent.top
            bottom:parent.bottom
            left:parent.left
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
            width:250
            height:250
            sourceSize: Qt.size(250,250)
            anchors {
                top: parent.top
                topMargin: 0
                horizontalCenter: parent.horizontalCenter
            }
        }
        Feedback {
            height: 236
            width: 420
            anchors {
                top: parent.top
                topMargin: 170
                horizontalCenter: parent.horizontalCenter
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
            onClicked: {
                Qt.openUrlExternally("https://www.stomt.com/screenplay")
            }
        }
    }


    Button {
        id: btbWeb
        text: qsTr("Open ScreenPlay Community")
        anchors {
            top: parent.top
            topMargin: 30
            horizontalCenter: parent.horizontalCenter
        }
        Material.background: Material.Orange
        Material.foreground: "white"
        onClicked: {
            Qt.openUrlExternally("https://www.screen-play.rocks/")
        }
    }
}
