import QtQuick 2.9

import QtGraphicalEffects 1.0
import QtQuick.Particles 2.0

Item {
    id: element
    anchors.fill: parent
    state: "init"
    onStateChanged: {
        if (state === "init") {
            colorShaderCreateTimer.stop()
        } else {
            colorShaderCreateTimer.start()
        }
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
        },
        State {
            name: "create"
            PropertyChanges {
                target: colorShaderCreate
                shaderOpacity: 1
            }
        }
    ]

    transitions: [
        Transition {
            from: "init"
            to: "create"

            PropertyAnimation {
                target: colorShaderCreate
                property: "shaderOpacity"
                duration: 400
                easing.type: Easing.OutQuart
            }
        },
        Transition {
            from: "create"
            to: "*"

            PropertyAnimation {
                target: colorShaderCreate
                property: "shaderOpacity"
                duration: 0
            }
        }
    ]
}
