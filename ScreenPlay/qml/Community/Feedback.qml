import QtQuick 2.9
import com.stomt.qmlSDK 1.0

Item {
    id: feedback
    height: 236
    width: 420
    state: "out"

    StomtWidget {
        id: stomtWidget

        anchors {
            top: parent.top
            topMargin: 30
            horizontalCenter: parent.horizontalCenter
        }
        targetID: "screenplay"
        appKey: "C1IfJAruqWFdkM8e7BMPK3dx1"
        targetDisplayName: "ScreenPlay"
        targetImageUrl: "qrc:/assets/images/ScreenPlayIconStomt.png"
        Component.onCompleted: feedback.state = "in"
    }

    states: [
        State {
            name: "out"

            PropertyChanges {
                target: stomtWidget
                opacity: 0
                anchors.topMargin: 100
            }
        },
        State {
            name: "in"
            PropertyChanges {
                target: stomtWidget
                opacity: 1
                anchors.topMargin: 30
            }
        }
    ]
    transitions: [
        Transition {
            from: "out"
            to: "in"
            PropertyAnimation {
                target: stomtWidget
                duration: 300
                easing.type: Easing.InOutQuart
                properties: "opacity,anchors.topMargin"
            }
        }
    ]
}
