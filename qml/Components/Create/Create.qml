import QtQuick 2.9
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2

//Enums
import LocalWorkshopCreationStatus 1.0

Rectangle {
    id: create
    anchors.fill: parent
    state: "out"
    Component.onCompleted: create.state = "create"
    property url activeVideoFile
    property url activeFolder

    FontLoader {
        id: font_Roboto_Regular
        source: "qrc:/assets/fonts/Roboto-Regular.ttf"
    }

    Connections {
        target: createWallpaper
        onVideoFileSelected: {
            create.state = "import"
            loader.active = true
            activeVideoFile = videoFile
            loader.setSource("CreateImport.qml", {
                                 videoFile: videoFile
                             })
        }
        onProjectFileSelected:{
            create.state = "import"
            loader.active = true
            activeFolder = projectFile
            loader.setSource("CreateUpload.qml", {
                                 projectFile: projectFile
                             })
        }
    }

    Connections {
        target: steamWorkshop
        onLocalWorkshopCreationStatusChanged: {
            if (status === LocalWorkshopCreationStatus.Started) {
                loader.setSource("CreateImportStatus.qml")
            }
        }
    }

    Connections {
        target: loader.item
        ignoreUnknownSignals: true
        onBackToCreate: {
            create.state = "create"
            loader.active = false
            activeVideoFile = ""
        }
        onUploadToSteamWorkshop: {
            loader.setSource("CreateUpload.qml", {
                                 file: activeVideoFile
                             })
        }
    }

    LinearGradient {
        id: gradient
        anchors.fill: parent
        cached: true
        start: Qt.point(0, 0)
        end: Qt.point(gradient.width, gradient.height)
        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: "#FF9700"
                SequentialAnimation on color {
                    loops: Animation.Infinite
                    ColorAnimation {
                        from: "#FF9700"
                        to: "#F83C3C"
                        duration: 5000
                    }
                    ColorAnimation {
                        from: "#F83C3C"
                        to: "#FF9700"
                        duration: 5000
                    }
                }
            }
            GradientStop {
                position: 1.0
                color: "#F83C3C"
                SequentialAnimation on color {
                    loops: Animation.Infinite

                    ColorAnimation {
                        from: "#F83C3C"
                        to: "#FF9700"
                        duration: 100000
                    }
                    ColorAnimation {
                        from: "#FF9700"
                        to: "#F83C3C"
                        duration: 100000
                    }
                }
            }
        }
    }

    Loader {
        id: loader
        anchors.fill: parent
        z: 11
    }

    Image {
        id: name
        source: "qrc:/assets/images/noisy-texture-3.png"
        anchors.fill: parent
        fillMode: Image.Tile
        opacity: .3
    }

    Rectangle {
        id: spaceBar
        width: 2
        height: 400
        opacity: .4
        anchors {
            top: parent.top
            topMargin: 100
            horizontalCenter: parent.horizontalCenter
        }
    }

    CreateWallpaper {
        id: createWallpaper
        width: 500
        height: 400
        anchors {
            top: parent.top
            topMargin: 84
            left: spaceBar.right
            leftMargin: 50
        }
    }

    CreateWidget {
        id: createWidget
        width: 500
        height: 400
        anchors {
            top: parent.top
            topMargin: 80
            right: spaceBar.left
            rightMargin: 50
        }
    }

    Text {
        id: txtDescriptionBottom
        text: qsTr("Create wallpapers and widgets for local usage or the steam workshop!")
        font.family: font_Roboto_Regular.name
        renderType: Text.NativeRendering
        font.pixelSize: 14
        color: "white"
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: footer.top
            bottomMargin: 20
        }
    }

    Rectangle {
        id: footer
        height: 80
        z: 100
        anchors {
            right: parent.right
            bottom: parent.bottom
            left: parent.left
        }
        Row {
            anchors.centerIn: parent
            spacing: 20
            Button {
                text: qsTr("Quickstart Guide")
            }
            Button {
                text: qsTr("Documentation")
            }
            Button {
                text: qsTr("Lore Ipsum")
            }
            Button {
                text: qsTr("Community")
            }
        }
    }
    states: [
        State {
            name: "out"
            PropertyChanges {
                target: spaceBar
                opacity: 0
            }

            PropertyChanges {
                target: footer
                anchors.bottomMargin: -80
            }
        },
        State {
            name: "create"
            PropertyChanges {
                target: spaceBar
                opacity: .4
            }
            PropertyChanges {
                target: footer
                anchors.bottomMargin: 0
            }
        },
        State {
            name: "import"

            PropertyChanges {
                target: createWallpaper
                state: "out"
            }
            PropertyChanges {
                target: createWidget
                state: "out"
            }
            PropertyChanges {
                target: spaceBar
                opacity: 0
            }
            PropertyChanges {
                target: txtDescriptionBottom
                opacity: 0
            }
        }
    ]
    transitions: [
        Transition {
            from: "out"
            to: "create"
            reversible: true

            NumberAnimation {
                target: txtDescriptionBottom
                property: "opacity"
                duration: 200
                easing.type: Easing.InOutQuad
            }
            ParallelAnimation {

                PauseAnimation {
                    duration: 200
                }
            }

            NumberAnimation {
                target: footer
                property: "anchors.bottomMargin"
                duration: 300
                easing.type: Easing.InOutQuad
            }
        }
    ]
}
