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
                    text: qsTr("Headline")
                    height: 100
                    width: 400
                    renderType: Text.NativeRendering
                    font.family: "Roboto"
                    font.weight: Font.Thin
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 21
                    color: "#2F2F2F"
                    wrapMode: Text.WrapAnywhere
                    anchors {
                        top: parent.top
                        right: parent.right
                        bottom: parent.bottom
                        left: parent.left
                        leftMargin: 20
                    }
                }
            }
            Text {
                id: txtHeadlineMonitor
                text: qsTr("Select a Monitor to display the content")
                height: 30
                renderType: Text.NativeRendering
                horizontalAlignment: Qt.AlignHCenter
                font.family: "Roboto"
                font.pixelSize: 18
                color: "#2F2F2F"
                wrapMode: Text.WrapAnywhere
                anchors {
                    top: headlineWrapper.bottom
                    topMargin: 20
                    right:parent.right
                    left: parent.left
                }
            }

            Rectangle {
                id: monitorSelectionWrapper
                height: 80
                width: 400
                anchors {
                    top: txtHeadlineMonitor.bottom
                    right:parent.right
                    left: parent.left
                }

                MonitorSelection {
                    id: monitorSelection
                    width: 360
                    height: parent.height
                    availableWidth: 360
                    fontSize: 11
                    availableHeight: 50
                    anchors {
                        top: parent.top
                        horizontalCenter: parent.horizontalCenter
                    }
                }
            }
            Item {
                id: sliderVolumeWrapper
                height: 100
                width: 400
                anchors {
                    top: monitorSelectionWrapper.bottom
                    topMargin: 30
                    right: parent.right
                    rightMargin: 30
                    leftMargin: 30
                    left: parent.left
                }
                Text {
                    id: txtSliderVolume
                    text: qsTr("Volume")
                    height: 30
                    renderType: Text.NativeRendering
                    font.family: "Roboto"
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 18
                    color: "#626262"
                    wrapMode: Text.WrapAnywhere
                    anchors {
                        top: parent.top
                        left: parent.left
                    }
                }
                Row {
                    id: sliderVolumeWrapperBottom
                    height: 70
                    spacing: 30
                    width: 300
                    anchors {
                        bottom: parent.bottom
                        horizontalCenter: parent.horizontalCenter
                    }
                    Image {
                        width: 20
                        height: 20
                        anchors.verticalCenter: parent.verticalCenter
                        source: "qrc:/assets/icons/icon_volume.svg"
                        sourceSize: Qt.size(20, 20)
                    }

                    Slider {
                        id: sliderVolume
                        stepSize: 0.1
                        from: 0
                        live: true
                        value: .8
                        to: 1
                        anchors.verticalCenter: parent.verticalCenter
                        onValueChanged: {
                            print(Math.round(sliderVolume.value * 100) / 100)
                        }
                    }

                    Text {
                        id: name
                        color: "#818181"
                        text: Math.round(sliderVolume.value * 100) / 100
                        anchors.verticalCenter: parent.verticalCenter
                        font.family: "Libre Baskerville"
                        font.pointSize: 12
                        font.italic: true
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }

        Item {
            id: item1
            width: 400
            height: 100
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
                icon.color: "white"
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
                                    installedListModel.get(
                                        activeScreen).screenPreview)
                    } else if (type === "widget") {
                        screenPlay.createWidget(
                                    installedListModel.absoluteStoragePath + "/" + activeScreen,
                                    installedListModel.get(
                                        activeScreen).screenPreview)
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
                target: sidebar
                anchors.rightMargin: 0
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
                target: sidebar
                anchors.rightMargin: - sidebar.width
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
                target: image
                opacity: 1
                anchors.topMargin: 0
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
                    target: sidebar
                    properties: "anchors.rightMargin"
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
                target: sidebar
                properties: "anchors.rightMargin"
                duration: 250
                easing.type: Easing.InOutQuad
            }
        },
        Transition {
            to: "activeWidget"

            SequentialAnimation {
                NumberAnimation {
                    target: sidebar
                    properties: "anchors.rightMargin"
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
                }
            }
        }
    ]
}
