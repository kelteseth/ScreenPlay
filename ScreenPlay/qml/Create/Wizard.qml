import QtQuick
import Qt5Compat.GraphicalEffects
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import ScreenPlay 1.0
import ScreenPlay.Create 1.0
import ScreenPlayUtil 1.0 

Item {
    id: root

    function setSource(path, arguments) {
        loader_wrapperContent.setSource(path, arguments);
        root.state = "in";
    }

    anchors.fill: parent
    state: "out"

    //Blocks some MouseArea from create page
    MouseArea {
        anchors.fill: parent
    }

    RectangularGlow {
        id: effect

        height: wrapper.height
        width: wrapper.width
        cached: true
        glowRadius: 3
        spread: 0.2
        color: "black"
        opacity: 0.4
        cornerRadius: 15

        anchors {
            top: wrapper.top
            left: wrapper.left
            right: wrapper.right
            topMargin: 3
        }

    }

    Rectangle {
        id: wrapper

        color: Material.theme === Material.Light ? "white" : Material.background
        height: 580
        radius: 4
        width: {
            // Add small margin left and right

            if (parent.width < 1200)
                return parent.width - 20;
            else
                return 1200;
        }

        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            topMargin: 0
        }

        Loader {
            id: loader_wrapperContent

            anchors.fill: parent
        }

        CloseIcon {
            onClicked: {
                timerBack.start();
            }

            Timer {
                id: timerBack

                interval: 400
                onTriggered: {
                    ScreenPlay.util.setNavigationActive(true);
                    ScreenPlay.util.setNavigation("Create");
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

        },
        State {
            name: "in"

            PropertyChanges {
                target: wrapper
                opacity: 1
                anchors.topMargin: {
                    if (root.height < 650)
                        return 20;
                    else
                        return 70;
                }
            }

            PropertyChanges {
                target: effect
                opacity: 0.4
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
                opacity: 0.4
            }

            PropertyChanges {
                target: loader_wrapperContent
                opacity: 1
                z: 1
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
                opacity: 0.4
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
            to: "error"

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

            }

        }
    ]
}
