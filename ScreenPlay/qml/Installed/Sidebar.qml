import QtQuick 2.9
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.3
import QtQuick.Extras 1.4
import QtQuick.Controls.Material 2.2

import "../"

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

    property string activeScreen
    onActiveScreenChanged: {
        txtHeadline.text = installedListModel.get(activeScreen).screenTitle
        image.source = Qt.resolvedUrl(
                    "file:///" + installedListModel.absoluteStoragePath + "/"
                    + activeScreen + "/" + installedListModel.get(
                        activeScreen).screenPreview)
    }

    property string type
    onTypeChanged: {
        if (type === "widget") {
            state = "activeWidget"
            btnSetWallpaper.text = qsTr("Set Widget")
        } else if (type === "video") {
            btnSetWallpaper.text = qsTr("Set Wallpaper")
        }
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
                    onStatusChanged: {
                        if (image.status === Image.Error) {
                            source = "qrc:/assets/images/missingPreview.png"
                        }
                    }
                }
                LinearGradient {
                    id: tabShadow
                    height: 20
                    cached: true

                    anchors {
                        bottom: parent.bottom
                        right: parent.right
                        left: parent.left
                    }
                    start: Qt.point(0, 30)
                    end: Qt.point(0, 0)
                    gradient: Gradient {
                        GradientStop {
                            position: 0.0
                            color: "#88000000"
                        }
                        GradientStop {
                            position: 1.0
                            color: "#00000000"
                        }
                    }
                }
            }

            MouseArea {
                id: button
                height: 50
                width: 50
                anchors.top: parent.top
                anchors.left: parent.left
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    sidebar.state = sidebar.state === "active" ? "inactive" : "active"
                }

                Image {
                    id: imgBack
                    source: "qrc:/assets/icons/icon_arrow_right.svg"
                    sourceSize: Qt.size(15, 15)
                    fillMode: Image.PreserveAspectFit
                    anchors.centerIn: parent
                }
            }

            Item {
                id: headlineWrapper
                height: 80
                opacity: 0
                enabled: false
                width: 400
                anchors {
                    top: imageWrapper.bottom
                    topMargin: 10
                    right: parent.right
                    rightMargin: 10
                    left: parent.left
                    leftMargin: 10
                }

                Text {
                    id: txtHeadline
                    text: ""
                    height:150
                    renderType: Text.NativeRendering
                    font.family: "Roboto"
                    font.weight: Font.Thin
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 21
                    color: "#2F2F2F"
                    wrapMode: Text.WrapAnywhere
                    anchors {
                        top: parent.top
                        right:parent.right
                        bottom: parent.bottom
                        left: parent.left
                        leftMargin: 20
                    }
                }
            }
            Text {
                id: txtHeadlineMonitor
                text: qsTr("Select a Monitor to display the content")
                height:50
                renderType: Text.NativeRendering
                font.family: "Roboto"
                font.pixelSize: 18
                color: "#2F2F2F"
                wrapMode: Text.WrapAnywhere
                anchors {
                    top: headlineWrapper.bottom
                    topMargin: 20
                    left: parent.left
                    leftMargin: 20
                }
            }

            Rectangle {
                id: monitorSelectionWrapper
                height: 80
                opacity: 0
                enabled: false
                width: 400
                anchors {
                    top: txtHeadlineMonitor.bottom
                }

                MonitorSelection {
                    id: monitorSelection
                    width: 360
                    height: parent.height
                    availableWidth: 360
                    fontSize:11
                    availableHeight: 50
                    anchors {
                        top: parent.top
                        horizontalCenter: parent.horizontalCenter
                    }
                }
            }
        }

        Item {
            id: item1
            width: 400
            height: 100
            opacity: 0
            enabled: false
            anchors {
                bottom: parent.bottom
            }
            Button {
                id: btnSetWallpaper
                text: qsTr("Set wallpaper")
                Material.accent: Material.Orange
                Material.background: Material.Orange
                Material.foreground: "white"

                icon.source: "qrc:/assets/icons/icon_plus.svg"
                icon.color:"white"
                icon.width: 16
                icon.height: 16
                anchors {
                    bottom: parent.bottom
                    bottomMargin: 20
                    horizontalCenter: parent.horizontalCenter
                }

                onClicked: {
                    if (type === "video" || type === "qmlScene") {
                        screenPlay.createWallpaper(
                                    monitorSelection.activeMonitorIndex,
                                    installedListModel.absoluteStoragePath + "/" + activeScreen,
                                    installedListModel.get(activeScreen).screenPreview)
                    } else if (type === "widget") {
                        screenPlay.createWidget(
                                    installedListModel.absoluteStoragePath + "/" + activeScreen,
                                    installedListModel.get(activeScreen).screenPreview)
                    }
                }
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

            PropertyChanges {
                target: item1
                opacity: 1
                enabled: true
            }


            PropertyChanges {
                target: monitorSelectionWrapper
                enabled: true
                opacity: 1
            }

            PropertyChanges {
                target: headlineWrapper
                enabled: true
                opacity: 1
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
        },
        State {
            name: "activeWidget"
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

            PropertyChanges {
                target: monitorSelectionWrapper
                enabled: false
                visible: true
                opacity: 0
            }

            PropertyChanges {
                target: item1
                opacity: 1
                enabled: true
            }

            PropertyChanges {
                target: headlineWrapper
                opacity: 1
                enabled: true
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
                    duration: 250
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
                    NumberAnimation {
                        targets: ["headlineWrapper", "monitorSelectionWrapper", "sliderVolumeWrapper"]
                        property: "opacity"
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
                duration: 250
                easing.type: Easing.InOutQuad
            }
            NumberAnimation {
                targets: ["headlineWrapper", "monitorSelectionWrapper", "sliderVolumeWrapper"]
                property: "opacity"
                duration: 100
            }
        },
        Transition {
            to: "activeWidget"

            SequentialAnimation {
                NumberAnimation {
                    target: sidebarWrapper
                    properties: "anchors.leftMargin"
                    duration: 250
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
                    NumberAnimation {
                        targets: ["headlineWrapper", "monitorSelectionWrapper", "sliderVolumeWrapper"]
                        property: "opacity"
                        duration: 100
                    }
                }
            }
        }
    ]
}
