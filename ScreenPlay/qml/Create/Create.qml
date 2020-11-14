import QtQuick 2.12
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.12
import QtQuick.Particles 2.0
import QtGraphicalEffects 1.0

import QtQuick.Controls.Material.impl 2.12

import ScreenPlay 1.0
import ScreenPlay.Create 1.0
import ScreenPlay.QMLUtilities 1.0

Item {
    id: root

    Sidebar {
        id: sidebar
        stackView: stackView
        anchors {
            top: parent.top
            left: parent.left
            bottom: parent.bottom
        }
    }

    Item {
        id: wizardContentWrapper
        state: ""
        Component.onCompleted: wizardContentWrapper.state = "in"
        width: parent.width - (sidebar.width + (anchors.margins * 2))
        height: parent.height - (anchors.margins * 2)
        opacity: 0
        anchors {
            margins: 10
            top: parent.top
            right: parent.right
            topMargin: 200
        }

        Rectangle {
            radius: 4
            layer.enabled: true
            layer.effect: ElevationEffect {
                elevation: 6
            }
            color: Material.theme === Material.Light ? "white" : Material.background
            anchors {
                fill: parent
                margins: 10
            }

            StackView {
                id: stackView
                pushEnter: Transition {
                    PropertyAnimation {
                        property: "opacity"
                        from: 0
                        to: 1
                        duration: 300
                    }
                    PropertyAnimation {
                        property: "scale"
                        from: 0.3
                        to: 1
                        duration: 300
                    }
                    XAnimator {
                        from: (stackView.mirrored ? -1 : 1) * -stackView.width
                        to: 0
                        duration: 400
                        easing.type: Easing.OutCubic
                    }
                }

                pushExit: Transition {
                    PropertyAnimation {
                        property: "opacity"
                        from: 1
                        to: 0
                        duration: 250
                    }
                    XAnimator {
                        from: 0
                        to: (stackView.mirrored ? -1 : 1)
                        duration: 400
                        easing.type: Easing.OutCubic
                    }

                    PropertyAnimation {
                        property: "scale"
                        from: 1
                        to: .3
                        duration: 300
                    }
                }

                anchors {
                    fill: parent
                    margins: 20
                }
            }
        }

        states: [
            State {
                name: "in"
                PropertyChanges {
                    target: wizardContentWrapper
                    anchors.topMargin: wizardContentWrapper.anchors.margins
                    opacity: 1
                }
            }
        ]
        transitions: [
            Transition {
                from: ""
                to: "in"
                reversible: true
                SequentialAnimation {

                    PropertyAnimation {
                        target: wizardContentWrapper
                        duration: 400
                        easing.type: Easing.OutCubic
                        properties: "anchors.topMargin, opacity"
                    }

                    ScriptAction {
                        script: {
                            wizardContentWrapper.anchors.left = sidebar.right
                            stackView.push("qrc:/qml/Create/StartInfo.qml")
                        }
                    }
                }
            }
        ]
    }
}

/*##^## Designer {
    D{i:0;autoSize:true;height:768;width:1366}
}
 ##^##*/

