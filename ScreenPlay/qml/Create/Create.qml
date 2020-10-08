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
    anchors.fill: parent

    BackgroundParticleSystem {
        id: particleSystemWrapper
        anchors.fill: parent
    }

    Sidebar {
        id: sidebar
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
            margins: 20
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
                margins: 20
            }

            Loader {
                id: loader
                onLoaded: loaderConnections.target = loader.item
                source: "qrc:/qml/Create/StartInfo.qml"
                anchors {
                    fill: parent
                    margins: 20
                }
                Connections {
                    id: loaderConnections
                    ignoreUnknownSignals: true
                    function onWizardStarted() {
                        sidebar.expanded = false
                    }
                    function onWizardExited() {
                        sidebar.expanded = true
                    }
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
                        easing.type: Easing.InOutQuart
                        properties: "anchors.topMargin, opacity"
                    }

                    ScriptAction {
                        script: wizardContentWrapper.anchors.left = sidebar.right
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

