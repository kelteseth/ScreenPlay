import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Particles 2.0
import QtGraphicalEffects 1.0

import net.aimber.create 1.0

Item {
    id: create
    anchors.fill: parent
    state: "out"

    Component.onCompleted: {
        create.state = "in"
    }
    property url activeVideoFile: ""
    property url activeFolder: ""

    Connections {
        target: createWallpaper
        onVideoFileSelected: {
            create.state = "import"

            activeVideoFile = videoFile
            loader.setSource(
                        "Wizards/CreateWallpaper/CreateWallpaperWizard.qml", {
                            "filePath": activeVideoFile
                        })
        }
        onProjectFileSelected: {
            create.state = "import"

            activeFolder = projectFile
            loader.setSource("CreateNew.qml", {

                             })
        }
    }

    Connections {
        target: loader.item
        ignoreUnknownSignals: true
        onBackToCreate: {
            create.state = "in"
            loader.active = false
            activeVideoFile = ""
        }
        onUploadToSteamWorkshop: {
            loader.setSource("CreateUpload.qml", {
                                 "file": activeVideoFile
                             })
        }
    }

    Item {
        anchors.fill: parent

        MouseArea {
            id: ma
            anchors.fill: parent
            hoverEnabled: true
            onPressed: {
                attractor.enabled = true
            }
            onPositionChanged: {
                if (ma.pressed) {
                    attractor.pointX = mouseX
                    attractor.pointY = mouseY
                }
            }
            onReleased: {
                attractor.enabled = false
            }
        }

        Attractor {
            id: attractor
            system: particleSystem
            affectedParameter: Attractor.Acceleration
            strength: 8000000
            proportionalToDistance: Attractor.InverseQuadratic
        }

        ParticleSystem {
            id: particleSystem
        }

        Emitter {
            id: emitter
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
            }

            width: parent.width
            height: 80
            system: particleSystem
            emitRate: 25
            lifeSpan: 5000
            lifeSpanVariation: 1000
            size: 8
            endSize: 18
            sizeVariation: 4
            velocity: AngleDirection {
                angle: -90
                magnitude: 50
                magnitudeVariation: 25
                angleVariation: 10
            }
        }

        ImageParticle {
            height: 16
            width: 16
            color: "orange"
            source: "qrc:/assets/particle/dot.png"
            system: particleSystem
            opacity: .75
        }

        Image {
            id: bgGlow
            width: 500
            height: width
            opacity: .3
            source: "qrc:/assets/particle/backgroundGlow.png"
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
                bottomMargin: -width * .65
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
        onButtonPressed: {
            create.state = "new"
            loader.setSource("CreateNew.qml", {
                                 "project": type,
                                 "projectTitle": title,
                                 "icon": iconSource
                             })
        }
    }

    Text {
        id: txtDescriptionBottom
        text: qsTr("Create wallpapers and widgets for local usage or the steam workshop!")
        font.family: "Roboto"
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
                text: qsTr("QML Quickstart Guide")
                Material.background: Material.Orange
                Material.foreground: "white"
                icon.source: "qrc:/assets/icons/icon_info.svg"
                icon.color: "white"
                icon.width: 16
                icon.height: 16
                onClicked: Qt.openUrlExternally(
                               "http://qmlbook.github.io/en/ch04/index.html#qml-syntax")
            }
            Button {
                text: qsTr("Documentation")
                Material.background: Material.LightGreen
                Material.foreground: "white"
                icon.source: "qrc:/assets/icons/icon_document.svg"
                icon.color: "white"
                icon.width: 16
                icon.height: 16
                onClicked: Qt.openUrlExternally(
                               "https://aimber.gitlab.io/ScreenPlayDocs/")
            }
            Button {
                text: qsTr("Forums")
                Material.background: Material.Blue
                Material.foreground: "white"
                icon.source: "qrc:/assets/icons/icon_people.svg"
                icon.color: "white"
                icon.width: 16
                icon.height: 16
                onClicked: Qt.openUrlExternally(
                               "https://forum.screen-play.app/")
            }
            Button {
                text: qsTr("Workshop")
                Material.background: Material.Red
                Material.foreground: "white"

                icon.source: "qrc:/assets/icons/icon_steam.svg"
                icon.color: "white"
                icon.width: 16
                icon.height: 16
                onClicked: Qt.openUrlExternally(
                               "steam://url/SteamWorkshopPage/672870")
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
            name: "in"
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
            PropertyChanges {
                target: footer
                anchors.bottomMargin: -80
            }
        },
        State {
            name: "new"

            PropertyChanges {
                target: footer
                anchors.bottomMargin: -80
            }
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
            to: "in"
            reversible: true

            NumberAnimation {
                targets: [txtDescriptionBottom, footer]
                property: "opacity"
                duration: 400
                easing.type: Easing.InOutQuart
            }
            SequentialAnimation {

                PauseAnimation {
                    duration: 300
                }

                NumberAnimation {
                    target: spaceBar
                    property: "opacity"
                    duration: 250
                    easing.type: Easing.InOutQuart
                }
            }

            NumberAnimation {
                target: footer
                property: "anchors.bottomMargin"
                duration: 400
                easing.type: Easing.InOutQuart
            }
        },
        Transition {
            from: "in"
            to: "new"
            reversible: true

            NumberAnimation {
                target: footer
                property: "anchors.bottomMargin"
                duration: 400
                easing.type: Easing.InOutQuart
            }
        }
    ]
}

/*##^## Designer {
    D{i:0;autoSize:true;height:768;width:1366}
}
 ##^##*/

