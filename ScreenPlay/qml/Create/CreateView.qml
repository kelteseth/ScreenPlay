import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Controls.Material.impl
import ScreenPlay
import ScreenPlayCore

Item {
    id: root
    property Item modalSource
    StackView.onActivated: {
        wizardContentWrapper.state = "in";
        stackView.push("qrc:/qt/qml/ScreenPlay/qml/Create/StartInfo.qml");
    }
    CreateSidebar {
        id: sidebar
        stackView: stackView
        modalSource: root.modalSource
        anchors {
            top: parent.top
            left: parent.left
            bottom: parent.bottom
        }
    }
    Item {
        id: wizardContentWrapper
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
            color: Material.theme === Material.Light ? "white" : Material.background
            anchors {
                fill: parent
                margins: 10
            }
            StackView {
                id: stackView
                anchors {
                    fill: parent
                    margins: 20
                }
                pushEnter: Transition {
                    PropertyAnimation {
                        property: "opacity"
                        from: 0
                        to: 1
                        duration: 400
                        easing.type: Easing.InOutQuart
                    }
                    PropertyAnimation {
                        property: "scale"
                        from: 0.8
                        to: 1
                        duration: 400
                        easing.type: Easing.InOutQuart
                    }
                }
                pushExit: Transition {
                    PropertyAnimation {
                        property: "opacity"
                        from: 1
                        to: 0
                        duration: 200
                        easing.type: Easing.InOutQuart
                    }
                    PropertyAnimation {
                        property: "scale"
                        from: 1
                        to: 0.8
                        duration: 400
                        easing.type: Easing.InOutQuart
                    }
                }
            }
            layer.effect: ElevationEffect {
                elevation: 6
            }
        }
        states: [
            State {
                name: "in"
                PropertyChanges {
                    wizardContentWrapper.anchors.topMargin: wizardContentWrapper.anchors.margins
                    wizardContentWrapper.opacity: 1
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
                            wizardContentWrapper.anchors.left = sidebar.right;
                        }
                    }
                }
            }
        ]
    }
}
