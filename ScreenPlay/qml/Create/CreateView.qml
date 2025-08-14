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
        stackView.push("qrc:/qt/qml/ScreenPlay/qml/Create/Wizards/ExampleContent.qml");
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
        anchors {
            margins: 10
            left: sidebar.right
            top: parent.top
            right: parent.right
            bottom: parent.bottom
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
                initialItem: Item {} // Needed for some reason for animation to trigger with StackView.onActivated
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
    }
}
