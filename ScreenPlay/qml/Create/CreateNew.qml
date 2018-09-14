import QtQuick 2.9
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import Qt.labs.platform 1.0
import QtQuick.Layouts 1.3

import "../Wizard"

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

        width: 910
        radius: 4
        height: 560

        SwipeView {
            id: view
            clip: true
            currentIndex: 0
            anchors.fill: parent
            anchors.margins: 40

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

            anchors.bottom: parent.bottom
            anchors.bottomMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter
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
