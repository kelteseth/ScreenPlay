import QtQuick 2.9
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import Qt.labs.platform 1.0
import QtQuick.Layouts 1.3

Item {
    id: createNew
    anchors.fill: parent
    Component.onCompleted: state = "in"
    state: "out"

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

        width: 910
        radius: 4
        height: 560

        Text {
            id: txtHeadline
            text: qsTr("Headline")
            height: 40
            renderType: Text.NativeRendering
            font.pixelSize: 23
            anchors {
                top: parent.top
                left: parent.left
                margins: 40
                bottomMargin: 0
            }
        }

        SwipeView {
            id: view
            clip: true
            currentIndex: 0
            onCurrentIndexChanged: print(view.currentIndex, view.count)
            anchors {
                top: txtHeadline.bottom
                right: parent.right
                bottom: indicator.top
                left: parent.left
                margins: 40
                bottomMargin: 20
                topMargin: 0
            }

            interactive: false

            Rectangle {
                id: firstPage
                color: "orange"
            }
            Rectangle {
                id: secondPage
                color: "grey"
            }
            Rectangle {
                id: thirdPage
                color: "steelblue"
            }
        }

        PageIndicator {
            id: indicator
            count: view.count
            currentIndex: view.currentIndex
            anchors {
                bottom: parent.bottom
                bottomMargin: 20
                left: parent.left
                leftMargin: 40
            }

            delegate: Item {
                width: 100
                height: 30
                Text {
                    id: name
                    text: {
                        switch (index) {
                        case 0:
                            return "1. sdfwerf"
                            break
                        case 1:
                            return "2. 34t3 345t w4"
                            break
                        case 2:
                            return "3. 45zde erg3q45t"
                            break
                        default:
                            return "Undefiend"
                        }
                    }
                    color: view.currentIndex == index ? "orange" : "gray"

                    anchors {
                        left: parent.left
                        verticalCenter: parent.verticalCenter
                    }
                }
            }
        }

        Row {
            width: 160
            height: 50
            spacing: 20
            anchors {
                bottom: parent.bottom
                right: parent.right
                margins: 20
                rightMargin: 40
            }
            Button {
                text: qsTr("Back")
                Material.background: Material.Gray
                Material.foreground: "white"

                icon.source: "qrc:/assets/icons/icon_arrow_left.svg"
                icon.color: "white"
                icon.width: 16
                icon.height: 16
                onClicked: {
                    if (view.currentIndex > 0)
                        view.setCurrentIndex(view.currentIndex - 1)
                }
            }
            Button {
                text: qsTr("Next")
                Material.background: Material.Orange
                Material.foreground: "white"

                icon.source: "qrc:/assets/icons/icon_arrow_right.svg"
                icon.color: "white"
                icon.width: 16
                icon.height: 16
                onClicked: {
                    if (view.currentIndex < view.count - 1)
                        view.setCurrentIndex(view.currentIndex + 1)
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
                id: timerBack
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
