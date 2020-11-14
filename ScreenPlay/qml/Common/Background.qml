import QtQuick 2.12
import QtQuick.Controls.Material 2.12
import QtGraphicalEffects 1.0
import QtQuick.Particles 2.0

Rectangle {
    id: element
    anchors.fill: parent
    color: Material.theme === Material.Light ? "white" : Qt.darker(
                                                   Material.background)
    state: "init"
    onStateChanged: {
        if (state === "init") {
            colorShaderCreateTimer.stop()
        } else {
            colorShaderCreateTimer.start()
        }
    }

    Rectangle {
        id: bgCommunity
        anchors.fill: parent
    }
    Rectangle {
        id: bgWorkshop
        color: "#161C1D"
        anchors.fill: parent
    }

    property var myDate: new Date()

    Timer {
        id: colorShaderCreateTimer
        interval: 16
        repeat: true
        onTriggered: colorShaderCreate.time = myDate.getMilliseconds()
    }

    ShaderEffect {
        id: colorShaderCreate
        anchors.fill: parent
        blending: true
        property real shaderOpacity: 0
        property real time: 45
        property vector2d resolution: Qt.vector2d(parent.width,
                                                  parent.height * 2)
        fragmentShader: "qrc:/assets/shader/movingcolorramp.fsh"
    }

    states: [
        State {
            name: "init"
            PropertyChanges {
                target: colorShaderCreate
                shaderOpacity: 0
            }
            PropertyChanges {
                target: bgCommunity
                opacity: 0
            }
            PropertyChanges {
                target: bgWorkshop
                opacity: 0
            }
        },
        State {
            name: "create"
            PropertyChanges {
                target: colorShaderCreate
                shaderOpacity: 1
            }
            PropertyChanges {
                target: bgCommunity
                opacity: 0
            }
            PropertyChanges {
                target: bgWorkshop
                opacity: 0
            }
        },
        State {
            name: "community"
            PropertyChanges {
                target: colorShaderCreate
                shaderOpacity: 0
            }
            PropertyChanges {
                target: bgCommunity
                opacity: 1
            }
            PropertyChanges {
                target: bgWorkshop
                opacity: 0
            }
        },
        State {
            name: "workshop"
            PropertyChanges {
                target: colorShaderCreate
                shaderOpacity: 0
            }
            PropertyChanges {
                target: bgCommunity
                opacity: 0
            }
            PropertyChanges {
                target: bgWorkshop
                opacity: 1
            }
        }
    ]

    transitions: [

        Transition {
            from: "*"
            to: "*"

            PropertyAnimation {
                targets: [bgCommunity, bgWorkshop]
                property: "opacity"
                duration: 400
                easing.type: Easing.InOutQuart
            }
            PropertyAnimation {
                target: colorShaderCreate
                property: "shaderOpacity"
                duration: 0
            }
        }
    ]
}
