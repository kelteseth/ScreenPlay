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
        anchors {
            margins: 20
            top: parent.top
            right: parent.right
            bottom: parent.bottom
            left: sidebar.right
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
                anchors {
                    top: parent.top
                    bottom: footer.top
                    left: parent.left
                    right: parent.right
                    margins: 20
                }
                Connections {
                    id:loaderConnections
                    ignoreUnknownSignals: true
                    function onWizardStarted(){
                        sidebar.expanded = false
                    }
                    function onWizardExited(){
                        sidebar.expanded = true
                        loader.source = ""

                    }
                }
            }

            Footer {
                id: footer
                anchors {
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                }
            }
        }
    }
}

/*##^## Designer {
    D{i:0;autoSize:true;height:768;width:1366}
}
 ##^##*/

