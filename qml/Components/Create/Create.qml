import QtQuick 2.7
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2

Rectangle {
    id: create
    anchors.fill: parent
    state: "out"
    Component.onCompleted: create.state = "in"
    onStateChanged: print(state)

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

    Image {
        id: name
        source: "qrc:/assets/images/noisy-texture-3.png"
        anchors.fill: parent
        fillMode: Image.Tile
        opacity: .3
    }

    Rectangle{
        id:spaceBar
        width:2

        height: 400
        opacity: .4
        anchors {
            top:parent.top
            topMargin: 100
            horizontalCenter: parent.horizontalCenter
        }
    }

    CreateWallpaper {
        width: 500
        height: 400
        anchors {
            top:parent.top
            topMargin: 80
            left: spaceBar.right
            leftMargin: 50
        }
    }

    CreateWidget {
        height: 400
        width: 500
        anchors {
            top:parent.top
            topMargin: 80
            right: spaceBar.left
            rightMargin: 50
        }
    }

    FontLoader{
        id: font_Roboto_Regular
        source: "qrc:/assets/fonts/Roboto-Regular.ttf"
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
        id:footer
        height: 80
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
            name: "in"
            PropertyChanges {
                target: spaceBar
                opacity: .4
            }
            PropertyChanges {
                target: footer
                anchors.bottomMargin: 0
            }
        }
    ]
    transitions: [
        Transition {
            from: "out"
            to: "in"
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
                target:footer
                property: "anchors.bottomMargin"
                duration: 300
                easing.type: Easing.InOutQuad
            }
        }
    ]
}
