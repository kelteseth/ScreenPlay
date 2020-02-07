import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Particles 2.0
import QtGraphicalEffects 1.0

import ScreenPlay 1.0
import ScreenPlay.Create 1.0
import ScreenPlay.QMLUtilities 1.0

import "Wizards/CreateWallpaper"

Item {
    id: create
    anchors.fill: parent
    state: "out"

    Component.onCompleted: {
        create.state = "in"
    }

    function checkFFMPEG() {
        if (!ScreenPlay.util.ffmpegAvailable) {
            ffmpegPopup.open()
        }
    }

    FFMPEGPopup {
        id: ffmpegPopup
        anchors.centerIn: create
        closePolicy: Popup.NoAutoClose
        focus: true
        modal: true
        parent: create
    }

    BackgroundParticleSystem {
        id: particleSystemWrapper
        anchors.fill: parent
    }

    Wizard {
        id: wizard
        anchors.fill: parent
    }

    CreateContent {
        id: createContent
        width: 500
        height: 400
        anchors {
            top: parent.top
            topMargin: 80
            right: verticalSeperator.left
            rightMargin: 50
        }
        onCreateContent: {
            create.state = "wizard"
            ScreenPlay.util.setNavigationActive(false)
            if (type === "emptyHtmlWallpaper") {
                wizard.setSource(
                            "qrc:/qml/Create/Wizards/CreateEmptyHtmlWallpaper/CreateEmptyHtmlWallpaper.qml",
                            {})
            } else if (type === "emptyWidget") {
                wizard.setSource(
                            "qrc:/qml/Create/Wizards/CreateEmptyWidget/CreateEmptyWidget.qml",
                            {})
            } else if (type === "musicVisualizer") {
                wizard.setSource(
                            "qrc:/qml/Create/Wizards/CreateEmptyWidget/CreateEmptyWidget.qml",
                            {})
            } else if (type === "unsplashSlideshow") {
                wizard.setSource(
                            "qrc:/qml/Create/Wizards/CreateEmptyWidget/CreateEmptyWidget.qml",
                            {})
            }
        }
    }

    Rectangle {
        id: verticalSeperator
        width: 2
        height: 400
        opacity: .4
        anchors {
            top: parent.top
            topMargin: 100
            horizontalCenter: parent.horizontalCenter
        }
    }

    ImportContent {
        id: importContent
        width: 500
        height: 400
        anchors {
            top: parent.top
            topMargin: 84
            left: verticalSeperator.right
            leftMargin: 50
        }
        onVideoImportConvertFileSelected: {
            create.state = "wizard"
            ScreenPlay.util.setNavigationActive(false)
            wizard.setSource(
                        "qrc:/qml/Create/Wizards/CreateWallpaper/CreateWallpaper.qml",
                        {
                            "filePath": videoFile
                        })
        }
    }

    Text {
        id: txtDescriptionBottom
        text: qsTr("Create wallpapers and widgets for local usage or the steam workshop!")
        font.family: "Roboto"

        font.pointSize: 10
        color: "white"
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: footer.top
            bottomMargin: 20
        }
    }

    Footer {
        id: footer
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
    }

    states: [
        State {
            name: "out"
            PropertyChanges {
                target: verticalSeperator
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
                target: verticalSeperator
                opacity: .4
            }
            PropertyChanges {
                target: footer
                anchors.bottomMargin: 0
            }
        },
        State {
            name: "wizard"

            PropertyChanges {
                target: footer
                anchors.bottomMargin: -80
            }
            PropertyChanges {
                target: importContent
                state: "out"
                 enabled:false
            }
            PropertyChanges {
                target: createContent
                state: "out"
                 enabled:false
            }
            PropertyChanges {
                target: verticalSeperator
                opacity: 0
                enabled:false
            }
            PropertyChanges {
                target: txtDescriptionBottom
                opacity: 0
                 enabled:false
            }
            PropertyChanges {
                target: wizard
                z: 99
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
                    target: verticalSeperator
                    property: "opacity"
                    duration: 250
                    easing.type: Easing.InOutQuart
                }
            }

            PropertyAnimation {
                target: footer
                property: "anchors.bottomMargin"
                duration: 400
                easing.type: Easing.InOutQuart
            }
        },
        Transition {
            from: "in"
            to: "wizard"
            reversible: true

            PropertyAnimation {
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

