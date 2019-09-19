import QtQuick 2.12
import QtGraphicalEffects 1.0
import Qt.labs.platform 1.0
import ScreenPlay 1.0

Item {
    id: createWidget
    state: "out"
    Component.onCompleted: state = "in"

    signal buttonPressed(var type, var title, var iconSource)

    Text {
        id: txtCreate
        text: qsTr("Create Widgets and Scenes")
        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
        }

        color: "white"
        font.pointSize: 21

        font.family: "Roboto"
        font.weight: Font.Thin
    }

    CreateWidgetButton {
        id: btnCreateEmptyWidget
        text: qsTr("Create Emtpy Widget")
        anchors.top: txtCreate.bottom
        anchors.topMargin: 10
        animOffset: 50
        state: createWidget.state
        imgSource: "qrc:/assets/icons/icon_emptyWidget.svg"
        onClicked: {

            buttonPressed("empty", btnCreateEmptyWidget.text,
                          btnCreateEmptyWidget.imgSource)
            //folderDialog.open()
        }
        FolderDialog {
            id: folderDialog
            onAccepted: {
                ScreenPlay.create.copyProject("/examples/scenes/empty",
                                             folderDialog.currentFolder)
            }
        }
    }

    Text {
        id: txtExamples
        text: qsTr("Examples Widgets and Scenes")
        font.family: "Roboto"

        font.pointSize: 16
        color: "white"

        anchors {
            top: btnCreateEmptyWidget.bottom
            topMargin: 30
            left: parent.left
            leftMargin: 30
        }
    }

    Column {
        width: parent.width
        spacing: 5
        anchors {
            top: parent.top
            topMargin: 200
            bottom: parent.bottom
        }
        CreateWidgetButton {
            id: btnEmpty1
            text: qsTr("Simple clock widget")

            animOffset: 150
            state: createWidget.state
            imgSource: "qrc:/assets/icons/icon_time.svg"
            onClicked: {
                buttonPressed("clock", btnEmpty1.text, btnEmpty1.imgSource)
            }
        }
        CreateWidgetButton {
            id: btnEmpty2
            text: qsTr("Musik scene wallpaper visualizer")

            animOffset: 200
            state: createWidget.state
            imgSource: "qrc:/assets/icons/icon_library_music.svg"
            onClicked: {
                buttonPressed("music", btnEmpty2.text, btnEmpty2.imgSource)
            }
        }
        CreateWidgetButton {
            id: btnEmpty3
            text: qsTr("Changing scene wallpaper via unsplash.com")
            imgSource: "qrc:/assets/icons/icon_scene.svg"

            animOffset: 250
            state: createWidget.state
            onClicked: {
                buttonPressed("slideshow", btnEmpty3.text, btnEmpty3.imgSource)
            }
        }
    }
    states: [
        State {
            name: "out"

            PropertyChanges {
                target: txtExamples
                opacity: 0
                anchors.topMargin: -50
            }

            PropertyChanges {
                target: txtCreate
                opacity: 0
                anchors.topMargin: -50
            }

            PropertyChanges {
                target: btnEmpty1
                opacity: 0
            }

            PropertyChanges {
                target: btnEmpty2
                opacity: 0
            }

            PropertyChanges {
                target: btnEmpty3
                opacity: 0
            }

            PropertyChanges {
                target: btnCreateEmptyWidget
                opacity: 0
            }
        },
        State {
            name: "in"

            PropertyChanges {
                target: txtExamples
                opacity: 1
                anchors.topMargin: 30
            }

            PropertyChanges {
                target: txtCreate
                opacity: 1
                anchors.topMargin: 0
            }
        }
    ]
    transitions: [
        Transition {
            from: "out"
            to: "in"
            reversible: true

            ScriptAction {
                script: {
                    if (state === "out") {
                        btnEmpty1.state = "out"
                        btnEmpty2.state = "out"
                        btnEmpty3.state = "out"
                    }
                }
            }

            PropertyAnimation {
                target: txtCreate
                properties: "opacity,anchors.topMargin"
                duration: 400
                easing.type: Easing.InOutQuart
            }

            SequentialAnimation {

                PauseAnimation {
                    duration: 50
                }
                PropertyAnimation {
                    target: txtExamples
                    properties: "opacity,anchors.topMargin"
                    duration: 400
                    easing.type: Easing.InOutQuart
                }
            }
        }
    ]
}

/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/

