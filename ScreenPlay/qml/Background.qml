import QtQuick 2.9

import QtGraphicalEffects 1.0
import QtQuick.Particles 2.0

Item {
    id: element
    anchors.fill: parent
    state: "init"
    onStateChanged: print(state)

    property var myDate: new Date()

    Timer {
        interval: 16
        running: true
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
            reversible: true

            PropertyAnimation {
                target: colorShaderCreateWrapper
                property: "shaderOpacity"
                duration: 200
                easing.type: Easing.InOutQuad
            }
        },
        Transition {
            from: "create"
            to: "*"
            reversible: true

            PropertyAnimation {
                target: colorShaderCreateWrapper
                property: "shaderOpacity"
                duration: 50
                easing.type: Easing.InOutQuad
            }
        }
    ]
}
