import QtQuick 2.9
import QtGraphicalEffects 1.0
import Qt.labs.platform 1.0

Item {
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
        font.pixelSize: 28
        renderType: Text.NativeRendering
        font.family: "Roboto"
        font.weight: Font.Thin
    }

    CreateWidgetButton {
        id: btnCreateEmptyWidget
        text: qsTr("Create Emtpy Widget")
        anchors.top: txtCreate.bottom
        anchors.topMargin: 10
        imgSource: "qrc:/assets/icons/icon_emptyWidget.svg"
        onClicked: {

            buttonPressed("empty", btnCreateEmptyWidget.text , btnCreateEmptyWidget.imgSource)
            //folderDialog.open()
        }
        FolderDialog {
            id: folderDialog
            onAccepted: {
                screenPlayCreate.copyProject("/examples/scenes/empty", folderDialog.currentFolder)
            }
        }

    }

    Text {
        id: txtExamples
        text: qsTr("Examples Widgets and Scenes")
        font.family: "Roboto"
        renderType: Text.NativeRendering
        font.pixelSize: 18
        color: "white"

        anchors {
            top: btnCreateEmptyWidget.bottom
            topMargin: 30
        }
    }

    Column {
        width: parent.width
        spacing: 5
        anchors {
            top: txtExamples.bottom
            topMargin: 10
            bottom: parent.bottom
        }
        CreateWidgetButton {
            id: btnEmpty1
            text: qsTr("Simple clock widget")
            buttonActive: true
            imgSource: "qrc:/assets/icons/icon_time.svg"
            onClicked: {
                buttonPressed("clock", btnEmpty1.text, btnEmpty1.imgSource)

            }
        }
        CreateWidgetButton {
            id: btnEmpty2
            text: qsTr("Musik scene wallpaper visualizer")
            buttonActive: true
            imgSource: "qrc:/assets/icons/icon_library_music.svg"
            onClicked: {
                buttonPressed("music",btnEmpty2.text, btnEmpty2.imgSource)
            }
        }
        CreateWidgetButton {
            id: btnEmpty3
            text: qsTr("Changing scene wallpaper via unsplash.com")
            imgSource: "qrc:/assets/icons/icon_scene.svg"
            buttonActive: true
            onClicked: {
                buttonPressed("slideshow",btnEmpty3.text, btnEmpty3.imgSource)

            }
        }
    }
    states: [
        State {
            name: "out"

            PropertyChanges {
                target: btnEmpty3
                opacity: 0
                buttonActive: false
            }

            PropertyChanges {
                target: btnEmpty2
                opacity: 0
                buttonActive: false
            }

            PropertyChanges {
                target: btnEmpty1
                opacity: 0
                buttonActive: false
            }

            PropertyChanges {
                target: btnCreateEmptyWidget
                opacity: 0
                buttonActive: false
            }

            PropertyChanges {
                target: txtExamples
                opacity: 0
            }

            PropertyChanges {
                target: txtCreate
                opacity: 0
            }
        },
        State {
            name: "in"
            PropertyChanges {
                target: btnEmpty3
                opacity: 1
                buttonActive: true
            }

            PropertyChanges {
                target: btnEmpty2
                opacity: 1
                buttonActive: true
            }

            PropertyChanges {
                target: btnEmpty1
                opacity: 1
                buttonActive: true
            }

            PropertyChanges {
                target: btnCreateEmptyWidget
                opacity: 1
                buttonActive: true
            }

            PropertyChanges {
                target: txtExamples
                opacity: 1
            }

            PropertyChanges {
                target: txtCreate
                opacity: 1
            }
        }
    ]
    transitions: [
        Transition {
            from: "out"
            to: "in"
            reversible: true

            SequentialAnimation {
                PropertyAnimation {
                    target: txtCreate
                    property: "opacity"
                    duration: 80
                    easing.type: Easing.InOutCubic
                }
                PropertyAnimation {
                    target: btnCreateEmptyWidget
                    property: "opacity"
                    duration: 80
                    easing.type: Easing.InOutCubic
                }

                PropertyAnimation {
                    target: txtExamples
                    property: "opacity"
                    duration: 80
                    easing.type: Easing.InOutCubic
                }
                PropertyAnimation {
                    target: btnEmpty1
                    property: "opacity"
                    duration: 80
                    easing.type: Easing.InOutCubic
                }

                PropertyAnimation {
                    target: btnEmpty2
                    property: "opacity"
                    duration: 80
                    easing.type: Easing.InOutCubic
                }

                PropertyAnimation {
                    target: btnEmpty3
                    property: "opacity"
                    duration: 80
                    easing.type: Easing.InOutCubic
                }
            }
        }
    ]
}



/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/
