import QtQuick 2.9
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import Qt.labs.platform 1.0
import QtQuick.Layouts 1.3

Item {
    id: createNew
    anchors.fill: parent
    state: "out"
    property string project
    property string icon
    property string projectTitle
    Component.onCompleted: createNew.state = "in"

    RectangularGlow {
        id: effect
        anchors {
            top: wrapper.top
            left: wrapper.left
            right: wrapper.right
            topMargin: 3
        }

        height: wrapper.height
        width: wrapper.width
        cached: true
        glowRadius: 3
        spread: 0.2
        color: "black"
        opacity: 0.4
        cornerRadius: 15
    }

    Rectangle {
        id: wrapper
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
        }

        width: 510
        radius: 4
        height: 560

        ColumnLayout {
            anchors {
                top: parent.top
                margins: 50
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }
            spacing: 50

            Item {

                width: 300
                height: 180
                Layout.alignment: Qt.AlignCenter

                Image {
                    id: imgIcon
                    width: 120
                    height: 120
                    sourceSize: Qt.size(120, 120)
                    source: createNew.icon
                    anchors.centerIn: parent
                }
                ColorOverlay {
                    color: "gray"
                    source: imgIcon
                    anchors.fill: imgIcon
                }
                Text {
                    text: projectTitle
                    color: "#5D5D5D"
                    font.pixelSize: 16
                    font.family: "Roboto"
                    renderType: Text.NativeRendering
                    verticalAlignment: Qt.AlignVCenter
                    horizontalAlignment: Qt.AlignHCenter
                    wrapMode: Text.WrapAnywhere
                    clip: true
                    height: 30

                    anchors {
                        left: parent.left
                        bottom: parent.bottom
                        right: parent.right
                    }
                }
            }

            TextField {
                id: txtTitle
                height: 60
                anchors {
                    right: parent.right
                    left: parent.left
                }

                selectByMouse: true
                text: qsTr("")
                placeholderText: "Project Name"
                onTextChanged: {
                    txtPath.text = folderDialog.currentFolder + "/" + txtTitle.text
                }
            }
            Item {
                height: 60
                width: parent.width

                Text {
                    id: txtPath
                    text: StandardPaths.standardLocations(
                              StandardPaths.HomeLocation)[0]
                    color: "#5D5D5D"
                    font.pixelSize: 16
                    font.family: "Roboto"
                    renderType: Text.NativeRendering
                    verticalAlignment: Qt.AlignVCenter
                    wrapMode: Text.WrapAnywhere
                    clip: true
                    anchors {
                        top: parent.top
                        left: parent.left
                        bottom: parent.bottom
                        right: btnFolder.left
                        rightMargin: 30
                    }
                }
                Button {
                    id: btnFolder
                    text: qsTr("Set Folder")
                    Material.background: Material.Orange
                    Material.foreground: "white"
                    anchors {
                        right: parent.right
                        bottom: parent.bottom
                    }

                    onClicked: {

                        folderDialog.open()
                    }
                    FolderDialog {
                        id: folderDialog
                        folder: StandardPaths.standardLocations(
                                    StandardPaths.HomeLocation)[0]
                        onAccepted: {
                            txtPath.text = folderDialog.currentFolder + "/" + txtTitle.text
                        }
                    }
                }
            }

            Button {
                text: qsTr("Create")
                Material.background: Material.Orange
                Material.foreground: "white"
                icon.source: "qrc:/assets/icons/icon_plus.svg"
                icon.color: "white"
                icon.width: 16
                icon.height: 16
                Layout.alignment: Qt.AlignHCenter
                onClicked: {

                }
            }
        }

        MouseArea {
            anchors {
                top: parent.top
                right: parent.right
                margins: 5
            }
            width: 32
            height: width
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                createNew.state = "out"
                timerBack.start()
            }

            Image {
                id: imgClose
                source: "qrc:/assets/icons/font-awsome/close.svg"
                width: 16
                height: 16
                anchors.centerIn: parent
                sourceSize: Qt.size(width, width)
            }
            ColorOverlay {
                id: iconColorOverlay
                anchors.fill: imgClose
                source: imgClose
                color: "gray"
            }
            Timer {
                id:timerBack
                interval: 800
                onTriggered: utility.setNavigation("Create")
            }
        }
    }

    states: [
        State {
            name: "out"
            PropertyChanges {
                target: wrapper
                anchors.topMargin: 800
                opacity: 0
            }
            PropertyChanges {
                target: effect
                opacity: 0
            }
        },
        State {
            name: "in"
            PropertyChanges {
                target: wrapper
                anchors.topMargin: 40
                opacity: 1
            }
            PropertyChanges {
                target: effect
                opacity: .4
            }
        }
    ]
    transitions: [
        Transition {
            from: "out"
            to: "in"
            SequentialAnimation {

                PauseAnimation {
                    duration: 400
                }
                ParallelAnimation {

                    PropertyAnimation {
                        target: wrapper
                        duration: 600
                        property: "anchors.topMargin"
                        easing.type: Easing.InOutQuad
                    }
                    PropertyAnimation {
                        target: wrapper
                        duration: 600
                        property: "opacity"
                        easing.type: Easing.InOutQuad
                    }
                    SequentialAnimation {

                        PauseAnimation {
                            duration: 1000
                        }
                        PropertyAnimation {
                            target: effect
                            duration: 300
                            property: "opacity"
                            easing.type: Easing.InOutQuad
                        }
                    }
                }
            }
        },
        Transition {
            from: "in"
            to: "out"

            ParallelAnimation {

                PropertyAnimation {
                    target: wrapper
                    duration: 600
                    property: "anchors.topMargin"
                    easing.type: Easing.InOutQuad
                }
                PropertyAnimation {
                    target: wrapper
                    duration: 600
                    property: "opacity"
                    easing.type: Easing.InOutQuad
                }
                SequentialAnimation {

                    PauseAnimation {
                        duration: 500
                    }
                    PropertyAnimation {
                        target: effect
                        duration: 300
                        property: "opacity"
                        easing.type: Easing.InOutQuad
                    }
                }
            }
        }
    ]
}
