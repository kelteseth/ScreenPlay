import QtQuick
import QtQuick.Effects
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import ScreenPlay

import ScreenPlayCore

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

    RectangularShadow {        // Replace RectangularGlow with RectangularShadow
        id: effect

        height: wrapper.height
        width: wrapper.width
        cached: true
        blur: 3                // Replace glowRadius with blur
        spread: 0.2
        color: "black"
        opacity: 0.4
        radius: 15             // Replace cornerRadius with radius
        offset: Qt.vector2d(0, 3)  // Add offset to replace the topMargin in anchors

        anchors {
            top: wrapper.top
            left: wrapper.left
            right: wrapper.right
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
                    App.uiAppStateSignals.setNavigationActive(true);
                    App.uiAppStateSignals.setNavigation("Create");
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
