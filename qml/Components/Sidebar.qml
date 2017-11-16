import QtQuick 2.7
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import QtQuick.Extras 1.4

Item {
    id: sidebar
    width: 400
    state: "inactive"
    focus: true

    property real navHeight

    MouseArea {
        id: mouseAreaHelper
        anchors.fill: parent
        enabled: true
    }

    property string activeScreen: ""

    onActiveScreenChanged: {
        txtHeadline.text = installedListModel.get(activeScreen).screenTitle
        image.source = Qt.resolvedUrl(
                    "file:///" + installedListModel.absoluteStoragePath + "/"
                    + activeScreen + "/" + installedListModel.get(
                        activeScreen).screenPreview)
    }

    FontLoader {
        id: font_Roboto_Regular
        source: "qrc:/assets/fonts/Roboto-Regular.ttf"
    }

    Item {
        id: sidebarWrapper

        anchors {
            top: sidebar.top
            right: sidebar.right
            bottom: sidebar.bottom
            left: sidebar.left
        }

        Item {
            id: navBackground
            height: navHeight
            anchors {
                top: parent.top
                right: parent.right
                left: parent.left
            }

            LinearGradient {
                anchors.fill: parent
                start: Qt.point(0, 0)
                end: Qt.point(400, 0)
                gradient: Gradient {
                    GradientStop {
                        position: 0.0
                        color: "transparent"
                    }
                    GradientStop {
                        position: 0.1
                        color: "#AAffffff"
                    }
                    GradientStop {
                        position: 1.0
                        color: "#ffffff"
                    }
                }
            }
        }

        Rectangle {
            id: sidebarBackground
            color: "white"
            anchors {
                top: navBackground.bottom
                right: parent.right
                bottom: parent.bottom
                left: parent.left
                leftMargin: 5
            }

            Rectangle {
                id: imageWrapper
                height: 237
                color: "#2b2b2b"
                anchors.right: parent.right
                anchors.rightMargin: 0
                anchors.left: parent.left
                anchors.leftMargin: 0

                Image {
                    id: image
                    fillMode: Image.PreserveAspectCrop
                    asynchronous: true
                    anchors.fill: parent
                }
            }

            Slider {
                id: sider
                visible: false
                height:65
                stepSize: 0.05
                anchors {
                    top:imageWrapper.bottom
                    right: parent.right
                    rightMargin: 10
                    leftMargin: 10
                    left: parent.left
                }
            }

            MouseArea {
                id: button
                height: 30
                width: 30
                anchors.top: parent.top
                anchors.topMargin: 20
                anchors.left: parent.left
                anchors.leftMargin: 20
                onClicked: {
                    sidebar.state = sidebar.state === "active" ? "inactive" : "active"
                }

                Image {
                    id: imgBack

                    anchors.left: parent.left
                    anchors.leftMargin: 0
                    opacity: 1
                    source: "qrc:/assets/icons/icon_arrow_right.svg"
                    sourceSize: Qt.size(14, 23)
                    anchors {
                        top: parent.top
                        topMargin: 0
                    }
                }
            }

            Button {
                id: button1
                y: 710
                text: qsTr("Set wallpaper")
                anchors.left: parent.left
                anchors.leftMargin: 148
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 18
                onClicked: {
                    screenPlaySettings.setWallpaper(
                                monitorSelection.activeMonitorIndex,
                                installedListModel.absoluteStoragePath + "/" + activeScreen)
                }
            }

            Rectangle {
                id: monitorSelectionWrapper
                color: "gray"
                height: 200
                anchors {
                    right: parent.right
                    bottom: parent.bottom
                    left: parent.left
                }

                anchors.bottomMargin: 100
                MonitorSelection {
                    id: monitorSelection
                    anchors.centerIn: parent
                    anchors.fill: parent
                    availableWidth: sidebar.width
                    availableHeight: monitorSelectionWrapper.height
                }
            }

            Text {
                id: txtHeadline
                text: ""
                renderType: Text.NativeRendering
                font.family: font_Roboto_Regular.name
                font.pixelSize: 21
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.top: parent.top
                anchors.topMargin: 267
            }
        }

        Item {
            id: shadow
            width: 5

            anchors {
                top: parent.top
                right: sidebarBackground.left
                bottom: parent.bottom
                left: parent.left
            }

            LinearGradient {
                anchors.fill: parent
                start: Qt.point(0, 0)
                end: Qt.point(5, 0)
                gradient: Gradient {
                    GradientStop {
                        position: 0.0
                        color: "#00000000"
                    }
                    GradientStop {
                        position: 1.0
                        color: "#22000000"
                    }
                }
            }
        }
    }

    states: [
        State {
            name: "active"

            PropertyChanges {
                target: mouseAreaHelper
                enabled: true
            }

            PropertyChanges {
                target: sidebarWrapper
                anchors.leftMargin: 0
            }
            PropertyChanges {
                target: image
                opacity: 1
                anchors.topMargin: 0
            }
        },
        State {
            name: "inactive"

            PropertyChanges {
                target: mouseAreaHelper
                enabled: false
            }

            PropertyChanges {
                target: sidebarWrapper
                anchors.leftMargin: sidebar.width
            }
            PropertyChanges {
                target: image
                opacity: 0
                anchors.topMargin: 20
            }
        }
    ]

    transitions: [
        Transition {
            to: "active"

            SequentialAnimation {
                NumberAnimation {
                    target: sidebarWrapper
                    properties: "anchors.leftMargin"
                    duration: 300
                    easing.type: Easing.InOutQuad
                }

                ParallelAnimation {
                    NumberAnimation {
                        target: image
                        property: "opacity"
                        duration: 200
                    }
                    NumberAnimation {
                        target: image
                        property: "anchors.topMargin"
                        duration: 100
                    }
                }
            }
        },
        Transition {
            to: "inactive"

            NumberAnimation {
                target: image
                property: "opacity"
                duration: 200
            }
            NumberAnimation {
                target: image
                property: "anchors.topMargin"
                duration: 400
            }

            NumberAnimation {
                target: sidebarWrapper
                properties: "anchors.leftMargin"
                duration: 300
                easing.type: Easing.InOutQuad
            }
        }
    ]
}
