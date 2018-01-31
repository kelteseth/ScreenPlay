import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3

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
            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: Qt.openUrlExternally("https://stomt.com/")
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
    Flickable {
        id: changelogFlickableWrapper
        width: 800
        height: parent.height
        contentHeight: columnWrapper.childrenRect.height
        contentWidth: 800
        flickableDirection: Flickable.VerticalFlick
        maximumFlickVelocity: 5000
        flickDeceleration: 5000

        anchors {
            top: parent.top
            topMargin: 20
            right:parent.right
            rightMargin: 20
            bottom:footer.top

            left:stomtWrapper.right
            leftMargin: 40
        }

        ScrollBar.vertical: ScrollBar {
            snapMode: ScrollBar.SnapOnRelease
            policy: ScrollBar.AlwaysOn
        }

        Column {
            id: columnWrapper
            spacing: 30
            Item {
                id: changelogWrapper
                width: 800
                height: 1020

                RectangularGlow {
                    id: effectchangelogWrapper
                    anchors {
                        top: parent.top
                    }
                    height: 1000
                    width: parent.width
                    cached: true
                    glowRadius: 3
                    spread: 0.2
                    color: "black"
                    opacity: 0.2
                    cornerRadius: 15
                }

                Rectangle {
                    height: 1000
                    width: parent.width
                    radius: 4

                    Text {
                        id: name
                        text: qsTr("News & Patchnotes")
                        wrapMode: Text.WordWrap
                        color: "#FFAB00"
                        renderType: Text.NativeRendering
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignLeft
                        font.pixelSize: 32
                        font.family: "Roboto"
                        anchors {
                            top:parent.top
                            topMargin: 40
                            horizontalCenter: parent.horizontalCenter
                        }
                    }
                }
            }
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
            }
            Button {
                text: qsTr("Documentation")
                Material.background: Material.LightGreen
                Material.foreground: "white"
            }
            Button {
                text: qsTr("Github")
                Material.background: Material.Orange
                Material.foreground: "white"
                onClicked: Qt.openUrlExternally("https://github.com/Aimber/")
            }
            Button {
                text: qsTr("Workshop")
                Material.background: Material.Red
                Material.foreground: "white"
                onClicked: Qt.openUrlExternally("http://steamcommunity.com/app/672870/workshop/")
            }
        }
    }
}
