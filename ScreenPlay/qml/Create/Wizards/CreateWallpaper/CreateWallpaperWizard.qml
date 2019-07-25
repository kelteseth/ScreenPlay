import QtQuick 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.3
import Qt.labs.platform 1.0
import QtQuick.Layouts 1.3

import ScreenPlay.Create 1.0

Item {
    id: createNew
    anchors.fill: parent
    state: "out"

    property string filePath
    property bool canNext: false

    Component.onCompleted: {
        state = "in"
        utility.setNavigationActive(false)

        loader_wrapperContent.setSource(
                    "qrc:/qml/Create/Wizards/CreateWallpaper/CreateWallpaperVideoImportConvert.qml",
                    {
                        "filePath": createNew.filePath
                    })
    }

    //Blocks some MouseArea from create page
    MouseArea {
        anchors.fill: parent
    }

    Connections {
        target: screenPlayCreate
        onCreateWallpaperStateChanged: {
            if (state === CreateImportVideo.AnalyseVideoError || state
                    === CreateImportVideo.ConvertingVideoError || state
                    === CreateImportVideo.ConvertingPreviewGifError || state
                    === CreateImportVideo.ConvertingPreviewImageError || state
                    === CreateImportVideo.ConvertingAudioError || state
                    === CreateImportVideo.AbortCleanupError || state
                    === CreateImportVideo.CopyFilesError || state
                    === CreateImportVideo.CreateProjectFileError || state
                    === CreateImportVideo.CreateTmpFolderError) {
                createNew.state = "result"
            }
        }
    }

    Timer {
        interval: 1000
        triggeredOnStart: false
        running: true
        repeat: false
        onTriggered: {
            screenPlayCreate.createWallpaperStart(filePath)
        }
    }

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
        width: 910
        radius: 4
        height: 460
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            topMargin: 0
        }

        Loader {
            id: loader_wrapperContent
            anchors.fill: parent
            z: 10
            Connections {
                target: loader_wrapperContent.sourceComponent
                onSave: {
                    createNew.state = "result"
                }
            }
        }

        CreateWallpaperResult {
            id: wrapperResult
            anchors.fill: parent
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
                id: timerBack
                interval: 800
                onTriggered: {
                    screenPlayCreate.abortAndCleanup()
                    utility.setNavigationActive(true)
                    utility.setNavigation("Create")
                }
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
            PropertyChanges {
                target: wrapperResult
                opacity: 0
            }
        },
        State {
            name: "in"
            PropertyChanges {
                target: wrapper
                anchors.topMargin: 100
                opacity: 1
            }
            PropertyChanges {
                target: effect
                opacity: .4
            }
            PropertyChanges {
                target: wrapperResult
                opacity: 0
            }
        },
        State {
            name: "error"
            PropertyChanges {
                target: wrapper
                anchors.topMargin: 100
                opacity: 1
            }
            PropertyChanges {
                target: effect
                opacity: .4
            }
            PropertyChanges {
                target: loader_wrapperContent
                opacity: 0
                z: 0
            }
            PropertyChanges {
                target: wrapperResult
                opacity: 1
            }
        },
        State {
            name: "success"
            PropertyChanges {
                target: wrapper
                anchors.topMargin: 100
                opacity: 1
            }
            PropertyChanges {
                target: effect
                opacity: .4
            }
            PropertyChanges {
                target: loader_wrapperContent
                opacity: 0
                z: 0
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
                        easing.type: Easing.OutQuart
                    }
                    PropertyAnimation {
                        target: wrapper
                        duration: 600
                        property: "opacity"
                        easing.type: Easing.OutQuart
                    }
                    SequentialAnimation {

                        PauseAnimation {
                            duration: 1000
                        }
                        PropertyAnimation {
                            target: effect
                            duration: 300
                            property: "opacity"
                            easing.type: Easing.OutQuart
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
                    easing.type: Easing.OutQuart
                }
                PropertyAnimation {
                    target: wrapper
                    duration: 600
                    property: "opacity"
                    easing.type: Easing.OutQuart
                }
                SequentialAnimation {

                    PauseAnimation {
                        duration: 500
                    }
                    PropertyAnimation {
                        target: effect
                        duration: 300
                        property: "opacity"
                        easing.type: Easing.OutQuart
                    }
                }
            }
        },
        Transition {
            from: "in"
            to: "result"
            SequentialAnimation {
                PropertyAnimation {
                    target: loader_wrapperContent
                    duration: 600
                    property: "opacity"
                    easing.type: Easing.OutQuart
                }
                PauseAnimation {
                    duration: 50
                }
                PropertyAnimation {
                    target: wrapperResult
                    duration: 200
                    property: "opacity"
                    easing.type: Easing.OutQuart
                }
            }
        }
    ]
}

/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/

